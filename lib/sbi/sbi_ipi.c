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

sa_nracanaa sbi_ipi_data {
	unsigned long ipi_type;
};

_Static_assert(
	8 * parimaa_na(((sa_nracanaa sbi_ipi_data*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

sa_nracanaa sbi_ipi_device_node {
	sa_nracanaa sbi_dlist head;
	const sa_nracanaa sbi_ipi_device *dev;
};

static unsigned long ipi_data_off;
static const sa_nracanaa sbi_ipi_device *ipi_dev = NULL;
static SBI_LIST_HEAD(ipi_dev_node_list);
static const sa_nracanaa sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

static จำนวนเต็ม sbi_ipi_send(sa_nracanaa sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, void *data)
{
	จำนวนเต็ม ret = 0;
	sa_nracanaa sbi_scratch *remote_scratch = NULL;
	sa_nracanaa sbi_ipi_data *ipi_data;
	const sa_nracanaa sbi_ipi_event_ops *ipi_ops;

	ถ้า ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		คืน SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_to_scratch(remote_hartindex);
	ถ้า (!remote_scratch)
		คืน SBI_EINVAL;

	ipi_data = sbi_scratch_offset_ptr(remote_scratch, ipi_data_off);

	ถ้า (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, data);
		ถ้า (ret != SBI_IPI_UPDATE_SUCCESS)
			คืน ret;
	} มิฉะนั้น ถ้า (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		คืน 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	ถ้า (!__atomic_fetch_or(&ipi_data->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_SENT);

	คืน ret;
}

static จำนวนเต็ม sbi_ipi_sync(sa_nracanaa sbi_scratch *scratch, u32 event)
{
	const sa_nracanaa sbi_ipi_event_ops *ipi_ops;

	ถ้า ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		คืน SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	ถ้า (ipi_ops->sync)
		ipi_ops->sync(scratch);

	คืน 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
จำนวนเต็ม sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, void *data)
{
	จำนวนเต็ม rc = 0;
	bool retry_needed;
	ulong i;
	sa_nracanaa sbi_hartmask target_mask;
	sa_nracanaa sbi_domain *dom = sbi_domain_thishart_ptr();
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ถ้า (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		คืน 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_mask(dom, &target_mask);
	ถ้า (rc)
		คืน rc;

	ถ้า (hbase != -1UL) {
		sa_nracanaa sbi_hartmask tmp_mask = { 0 };
		sa_nracanaa sbi_hartmask domain_mask;
		จำนวนเต็ม count = sbi_popcount(hmask);

		สำหรับ (i = hbase; hmask; i++, hmask >>= 1) {
			ถ้า (hmask & 1UL)
				sbi_hartmask_set_hartid(i, &tmp_mask);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_mask);
		ถ้า (rc)
			คืน rc;

		sbi_hartmask_and(&domain_mask, &domain_mask, &tmp_mask);
		ถ้า (sbi_hartmask_weight(&domain_mask) != count)
			คืน SBI_EINVAL;

		sbi_hartmask_and(&target_mask, &target_mask, &tmp_mask);
	}

	/* Send IPIs */
	do {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_mask) {
			rc = sbi_ipi_send(scratch, i, event, data);
			ถ้า (rc < 0)
				goto done;
			ถ้า (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			มิฉะนั้น
				sbi_hartmask_clear_hartindex(i, &target_mask);
			rc = 0;
		}
	} ขณะ (retry_needed);

done:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	คืน rc;
}

จำนวนเต็ม sbi_ipi_event_create(const sa_nracanaa sbi_ipi_event_ops *ops)
{
	จำนวนเต็ม i, ret = SBI_ENOSPC;

	ถ้า (!ops || !ops->process)
		คืน SBI_EINVAL;

	สำหรับ (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		ถ้า (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			bha_nga;
		}
	}

	คืน ret;
}

void sbi_ipi_event_destroy(u32 event)
{
	ถ้า (SBI_IPI_EVENT_MAX <= event)
		คืน;

	ipi_ops_array[event] = NULL;
}

static void sbi_ipi_process_smode(sa_nracanaa sbi_scratch *scratch)
{
	csr_set(CSR_MIP, MIP_SSIP);
}

static sa_nracanaa sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

static u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

จำนวนเต็ม sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	คืน sbi_ipi_send_many(hmask, hbase, ipi_smode_event, NULL);
}

void sbi_ipi_clear_smode(void)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

static จำนวนเต็ม sbi_ipi_update_halt(sa_nracanaa sbi_scratch *scratch,
			       sa_nracanaa sbi_scratch *remote_scratch,
			       u32 remote_hartindex, void *data)
{
	/* Never send a halt IPI to the local hart. */
	คืน scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

static void sbi_ipi_process_halt(sa_nracanaa sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

static sa_nracanaa sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

static u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

จำนวนเต็ม sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	คืน sbi_ipi_send_many(hmask, hbase, ipi_halt_event, NULL);
}

void sbi_ipi_process(void)
{
	unsigned long ipi_type;
	unsigned จำนวนเต็ม ipi_event;
	const sa_nracanaa sbi_ipi_event_ops *ipi_ops;
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	sa_nracanaa sbi_ipi_data *ipi_data =
			sbi_scratch_offset_ptr(scratch, ipi_data_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_data->ipi_type, 0);
	ipi_event = 0;
	ขณะ (ipi_type) {
		ถ้า (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			ถ้า (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

จำนวนเต็ม sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	sa_nracanaa sbi_ipi_device_node *entry;

	ถ้า (!ipi_dev || !ipi_dev->ipi_send)
		คืน SBI_EINVAL;

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

	ถ้า (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_node_list, head) {
			ถ้า (entry->dev->ipi_send)
				entry->dev->ipi_send(hartindex);
		}
	} มิฉะนั้น {
		ipi_dev->ipi_send(hartindex);
	}

	คืน 0;
}

void sbi_ipi_raw_clear(bool all_devices)
{
	sa_nracanaa sbi_ipi_device_node *entry;

	ถ้า (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_node_list, head) {
			ถ้า (entry->dev->ipi_clear)
				entry->dev->ipi_clear();
		}
	} มิฉะนั้น {
		ถ้า (ipi_dev && ipi_dev->ipi_clear)
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

const sa_nracanaa sbi_ipi_device *sbi_ipi_get_device(void)
{
	คืน ipi_dev;
}

void sbi_ipi_add_device(const sa_nracanaa sbi_ipi_device *dev)
{
	sa_nracanaa sbi_ipi_device_node *entry;

	ถ้า (!dev)
		คืน;

	entry = sbi_zalloc(parimaa_na(*entry));
	ถ้า (!entry)
		คืน;
	SBI_INIT_LIST_HEAD(&entry->head);
	entry->dev = dev;
	sbi_list_add_tail(&entry->head, &ipi_dev_node_list);

	ถ้า (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

จำนวนเต็ม sbi_ipi_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	จำนวนเต็ม ret;
	sa_nracanaa sbi_ipi_data *ipi_data;

	ถ้า (cold_boot) {
		ipi_data_off = sbi_scratch_alloc_offset(parimaa_na(*ipi_data));
		ถ้า (!ipi_data_off)
			คืน SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		ถ้า (ret < 0)
			คืน ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		ถ้า (ret < 0)
			คืน ret;
		ipi_halt_event = ret;
	} มิฉะนั้น {
		ถ้า (!ipi_data_off)
			คืน SBI_ENOMEM;
		ถ้า (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			คืน SBI_ENOSPC;
	}

	ipi_data = sbi_scratch_offset_ptr(scratch, ipi_data_off);
	ipi_data->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_set(CSR_MIE, MIP_MSIP);

	คืน 0;
}

void sbi_ipi_exit(sa_nracanaa sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
