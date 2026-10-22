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

static цео sbi_ipi_send(struct sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, void *data)
{
	цео ret = 0;
	struct sbi_scratch *remote_scratch = NULL;
	struct sbi_ipi_data *ipi_data;
	const struct sbi_ipi_event_ops *ipi_ops;

	ако ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		врати SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_to_scratch(remote_hartindex);
	ако (!remote_scratch)
		врати SBI_EINVAL;

	ipi_data = sbi_scratch_offset_ptr(remote_scratch, ipi_data_off);

	ако (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, data);
		ако (ret != SBI_IPI_UPDATE_SUCCESS)
			врати ret;
	} инако ако (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		врати 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	ако (!__atomic_fetch_or(&ipi_data->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_SENT);

	врати ret;
}

static цео sbi_ipi_sync(struct sbi_scratch *scratch, u32 event)
{
	const struct sbi_ipi_event_ops *ipi_ops;

	ако ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		врати SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	ако (ipi_ops->sync)
		ipi_ops->sync(scratch);

	врати 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
цео sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, void *data)
{
	цео rc = 0;
	bool retry_needed;
	ulong i;
	struct sbi_hartmask target_mask;
	struct sbi_domain *dom = sbi_domain_thishart_ptr();
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ако (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		врати 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_mask(dom, &target_mask);
	ако (rc)
		врати rc;

	ако (hbase != -1UL) {
		struct sbi_hartmask tmp_mask = { 0 };
		struct sbi_hartmask domain_mask;
		цео count = sbi_popcount(hmask);

		за (i = hbase; hmask; i++, hmask >>= 1) {
			ако (hmask & 1UL)
				sbi_hartmask_set_hartid(i, &tmp_mask);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_mask);
		ако (rc)
			врати rc;

		sbi_hartmask_and(&domain_mask, &domain_mask, &tmp_mask);
		ако (sbi_hartmask_weight(&domain_mask) != count)
			врати SBI_EINVAL;

		sbi_hartmask_and(&target_mask, &target_mask, &tmp_mask);
	}

	/* Send IPIs */
	do {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_mask) {
			rc = sbi_ipi_send(scratch, i, event, data);
			ако (rc < 0)
				goto done;
			ако (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			инако
				sbi_hartmask_clear_hartindex(i, &target_mask);
			rc = 0;
		}
	} док (retry_needed);

done:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	врати rc;
}

цео sbi_ipi_event_create(const struct sbi_ipi_event_ops *ops)
{
	цео i, ret = SBI_ENOSPC;

	ако (!ops || !ops->process)
		врати SBI_EINVAL;

	за (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		ако (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			break;
		}
	}

	врати ret;
}

void sbi_ipi_event_destroy(u32 event)
{
	ако (SBI_IPI_EVENT_MAX <= event)
		врати;

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

цео sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	врати sbi_ipi_send_many(hmask, hbase, ipi_smode_event, NULL);
}

void sbi_ipi_clear_smode(void)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

static цео sbi_ipi_update_halt(struct sbi_scratch *scratch,
			       struct sbi_scratch *remote_scratch,
			       u32 remote_hartindex, void *data)
{
	/* Never send a halt IPI to the local hart. */
	врати scratch == remote_scratch ?
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

цео sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	врати sbi_ipi_send_many(hmask, hbase, ipi_halt_event, NULL);
}

void sbi_ipi_process(void)
{
	unsigned long ipi_type;
	unsigned цео ipi_event;
	const struct sbi_ipi_event_ops *ipi_ops;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	struct sbi_ipi_data *ipi_data =
			sbi_scratch_offset_ptr(scratch, ipi_data_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_data->ipi_type, 0);
	ipi_event = 0;
	док (ipi_type) {
		ако (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			ако (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

цео sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	struct sbi_ipi_device_node *entry;

	ако (!ipi_dev || !ipi_dev->ipi_send)
		врати SBI_EINVAL;

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

	ако (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_node_list, head) {
			ако (entry->dev->ipi_send)
				entry->dev->ipi_send(hartindex);
		}
	} инако {
		ipi_dev->ipi_send(hartindex);
	}

	врати 0;
}

void sbi_ipi_raw_clear(bool all_devices)
{
	struct sbi_ipi_device_node *entry;

	ако (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_node_list, head) {
			ако (entry->dev->ipi_clear)
				entry->dev->ipi_clear();
		}
	} инако {
		ако (ipi_dev && ipi_dev->ipi_clear)
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
	врати ipi_dev;
}

void sbi_ipi_add_device(const struct sbi_ipi_device *dev)
{
	struct sbi_ipi_device_node *entry;

	ако (!dev)
		врати;

	entry = sbi_zalloc(sizeof(*entry));
	ако (!entry)
		врати;
	SBI_INIT_LIST_HEAD(&entry->head);
	entry->dev = dev;
	sbi_list_add_tail(&entry->head, &ipi_dev_node_list);

	ако (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

цео sbi_ipi_init(struct sbi_scratch *scratch, bool cold_boot)
{
	цео ret;
	struct sbi_ipi_data *ipi_data;

	ако (cold_boot) {
		ipi_data_off = sbi_scratch_alloc_offset(sizeof(*ipi_data));
		ако (!ipi_data_off)
			врати SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		ако (ret < 0)
			врати ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		ако (ret < 0)
			врати ret;
		ipi_halt_event = ret;
	} инако {
		ако (!ipi_data_off)
			врати SBI_ENOMEM;
		ако (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			врати SBI_ENOSPC;
	}

	ipi_data = sbi_scratch_offset_ptr(scratch, ipi_data_off);
	ipi_data->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_set(CSR_MIE, MIP_MSIP);

	врати 0;
}

void sbi_ipi_exit(struct sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
