/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_atomic.h>
#include <sbi/riscv_barrier.h>
#include <sbi/sbi_bitops.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_hsm.h>
#include <sbi/sbi_init.h>
#include <sbi/sbi_ipi.h>
#include <sbi/sbi_list.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_pmu.h>
#include <sbi/sbi_string.h>
#include <sbi/sbi_tlb.h>

struct sbi_ipi_data {
	unsigned long ipi_type;
};

_Static_assert(
	8 * sizeof(((struct sbi_ipi_data*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

struct sbi_ipi_device_node {
	struct sbi_dlist head;
	const struct sbi_ipi_device *dev;
};

static unsigned long ipi_data_off;
static const struct sbi_ipi_device *ipi_dev = NULL;
static SBI_LIST_HEAD(ipi_dev_node_list);
static const struct sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

static số_nguyên sbi_ipi_send(struct sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, void *data)
{
	số_nguyên ret = 0;
	struct sbi_scratch *remote_scratch = NULL;
	struct sbi_ipi_data *ipi_data;
	const struct sbi_ipi_event_ops *ipi_ops;

	nếu ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		trả_về SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_to_scratch(remote_hartindex);
	nếu (!remote_scratch)
		trả_về SBI_EINVAL;

	ipi_data = sbi_scratch_offset_ptr(remote_scratch, ipi_data_off);

	nếu (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, data);
		nếu (ret != SBI_IPI_UPDATE_SUCCESS)
			trả_về ret;
	} không_thì nếu (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		trả_về 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	nếu (!__atomic_fetch_or(&ipi_data->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_SENT);

	trả_về ret;
}

static số_nguyên sbi_ipi_sync(struct sbi_scratch *scratch, u32 event)
{
	const struct sbi_ipi_event_ops *ipi_ops;

	nếu ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		trả_về SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	nếu (ipi_ops->sync)
		ipi_ops->sync(scratch);

	trả_về 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
số_nguyên sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, void *data)
{
	số_nguyên rc = 0;
	bool retry_needed;
	ulong i;
	struct sbi_hartmask target_mask;
	struct sbi_domain *dom = sbi_domain_thishart_ptr();
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	nếu (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		trả_về 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_mask(dom, &target_mask);
	nếu (rc)
		trả_về rc;

	nếu (hbase != -1UL) {
		struct sbi_hartmask tmp_mask = { 0 };
		struct sbi_hartmask domain_mask;
		số_nguyên count = sbi_popcount(hmask);

		cho (i = hbase; hmask; i++, hmask >>= 1) {
			nếu (hmask & 1UL)
				sbi_hartmask_set_hartid(i, &tmp_mask);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_mask);
		nếu (rc)
			trả_về rc;

		sbi_hartmask_and(&domain_mask, &domain_mask, &tmp_mask);
		nếu (sbi_hartmask_weight(&domain_mask) != count)
			trả_về SBI_EINVAL;

		sbi_hartmask_and(&target_mask, &target_mask, &tmp_mask);
	}

	/* Send IPIs */
	do {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_mask) {
			rc = sbi_ipi_send(scratch, i, event, data);
			nếu (rc < 0)
				goto done;
			nếu (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			không_thì
				sbi_hartmask_clear_hartindex(i, &target_mask);
			rc = 0;
		}
	} trong_khi (retry_needed);

done:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	trả_về rc;
}

số_nguyên sbi_ipi_event_create(const struct sbi_ipi_event_ops *ops)
{
	số_nguyên i, ret = SBI_ENOSPC;

	nếu (!ops || !ops->process)
		trả_về SBI_EINVAL;

	cho (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		nếu (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			break;
		}
	}

	trả_về ret;
}

void sbi_ipi_event_destroy(u32 event)
{
	nếu (SBI_IPI_EVENT_MAX <= event)
		trả_về;

	ipi_ops_array[event] = NULL;
}

static void sbi_ipi_process_smode(struct sbi_scratch *scratch)
{
	csr_set(CSR_MIP, MIP_SSIP);
}

static struct sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

static u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

số_nguyên sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	trả_về sbi_ipi_send_many(hmask, hbase, ipi_smode_event, NULL);
}

void sbi_ipi_clear_smode(void)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

static số_nguyên sbi_ipi_update_halt(struct sbi_scratch *scratch,
			       struct sbi_scratch *remote_scratch,
			       u32 remote_hartindex, void *data)
{
	/* Never send a halt IPI to the local hart. */
	trả_về scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

static void sbi_ipi_process_halt(struct sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

static struct sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

static u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

số_nguyên sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	trả_về sbi_ipi_send_many(hmask, hbase, ipi_halt_event, NULL);
}

void sbi_ipi_process(void)
{
	unsigned long ipi_type;
	unsigned số_nguyên ipi_event;
	const struct sbi_ipi_event_ops *ipi_ops;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	struct sbi_ipi_data *ipi_data =
			sbi_scratch_offset_ptr(scratch, ipi_data_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_data->ipi_type, 0);
	ipi_event = 0;
	trong_khi (ipi_type) {
		nếu (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			nếu (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

số_nguyên sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	struct sbi_ipi_device_node *entry;

	nếu (!ipi_dev || !ipi_dev->ipi_send)
		trả_về SBI_EINVAL;

	/*
	 * Ensure that memory or MMIO writes done before
	 * this function are not observed after the memory
	 * or MMIO writes done by the ipi_send() device
	 * callback. This also allows the ipi_send() device
	 * callback to use relaxed MMIO writes.
	 *
	 * This pairs with the wmb() in sbi_ipi_raw_clear().
	 */
	wmb();

	nếu (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_node_list, head) {
			nếu (entry->dev->ipi_send)
				entry->dev->ipi_send(hartindex);
		}
	} không_thì {
		ipi_dev->ipi_send(hartindex);
	}

	trả_về 0;
}

void sbi_ipi_raw_clear(bool all_devices)
{
	struct sbi_ipi_device_node *entry;

	nếu (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_node_list, head) {
			nếu (entry->dev->ipi_clear)
				entry->dev->ipi_clear();
		}
	} không_thì {
		nếu (ipi_dev && ipi_dev->ipi_clear)
			ipi_dev->ipi_clear();
	}

	/*
	 * Ensure that memory or MMIO writes after this
	 * function returns are not observed before the
	 * memory or MMIO writes done by the ipi_clear()
	 * device callback. This also allows ipi_clear()
	 * device callback to use relaxed MMIO writes.
	 *
	 * This pairs with the wmb() in sbi_ipi_raw_send().
	 */
	wmb();
}

const struct sbi_ipi_device *sbi_ipi_get_device(void)
{
	trả_về ipi_dev;
}

void sbi_ipi_add_device(const struct sbi_ipi_device *dev)
{
	struct sbi_ipi_device_node *entry;

	nếu (!dev)
		trả_về;

	entry = sbi_zalloc(sizeof(*entry));
	nếu (!entry)
		trả_về;
	SBI_INIT_LIST_HEAD(&entry->head);
	entry->dev = dev;
	sbi_list_add_tail(&entry->head, &ipi_dev_node_list);

	nếu (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

số_nguyên sbi_ipi_init(struct sbi_scratch *scratch, bool cold_boot)
{
	số_nguyên ret;
	struct sbi_ipi_data *ipi_data;

	nếu (cold_boot) {
		ipi_data_off = sbi_scratch_alloc_offset(sizeof(*ipi_data));
		nếu (!ipi_data_off)
			trả_về SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		nếu (ret < 0)
			trả_về ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		nếu (ret < 0)
			trả_về ret;
		ipi_halt_event = ret;
	} không_thì {
		nếu (!ipi_data_off)
			trả_về SBI_ENOMEM;
		nếu (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			trả_về SBI_ENOSPC;
	}

	ipi_data = sbi_scratch_offset_ptr(scratch, ipi_data_off);
	ipi_data->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_set(CSR_MIE, MIP_MSIP);

	trả_về 0;
}

void sbi_ipi_exit(struct sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
