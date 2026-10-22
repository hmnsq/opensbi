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

static ចំនួនគត់ sbi_ipi_send(sa_nracanaa sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, void *data)
{
	ចំនួនគត់ ret = 0;
	sa_nracanaa sbi_scratch *remote_scratch = NULL;
	sa_nracanaa sbi_ipi_data *ipi_data;
	const sa_nracanaa sbi_ipi_event_ops *ipi_ops;

	ប្រសិនបើ ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		ត្រឡប់ SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_to_scratch(remote_hartindex);
	ប្រសិនបើ (!remote_scratch)
		ត្រឡប់ SBI_EINVAL;

	ipi_data = sbi_scratch_offset_ptr(remote_scratch, ipi_data_off);

	ប្រសិនបើ (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, data);
		ប្រសិនបើ (ret != SBI_IPI_UPDATE_SUCCESS)
			ត្រឡប់ ret;
	} បើមិនដូច្នេះ ប្រសិនបើ (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		ត្រឡប់ 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	ប្រសិនបើ (!__atomic_fetch_or(&ipi_data->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_SENT);

	ត្រឡប់ ret;
}

static ចំនួនគត់ sbi_ipi_sync(sa_nracanaa sbi_scratch *scratch, u32 event)
{
	const sa_nracanaa sbi_ipi_event_ops *ipi_ops;

	ប្រសិនបើ ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		ត្រឡប់ SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	ប្រសិនបើ (ipi_ops->sync)
		ipi_ops->sync(scratch);

	ត្រឡប់ 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
ចំនួនគត់ sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, void *data)
{
	ចំនួនគត់ rc = 0;
	bool retry_needed;
	ulong i;
	sa_nracanaa sbi_hartmask target_mask;
	sa_nracanaa sbi_domain *dom = sbi_domain_thishart_ptr();
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ប្រសិនបើ (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		ត្រឡប់ 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_mask(dom, &target_mask);
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	ប្រសិនបើ (hbase != -1UL) {
		sa_nracanaa sbi_hartmask tmp_mask = { 0 };
		sa_nracanaa sbi_hartmask domain_mask;
		ចំនួនគត់ count = sbi_popcount(hmask);

		សម្រាប់ (i = hbase; hmask; i++, hmask >>= 1) {
			ប្រសិនបើ (hmask & 1UL)
				sbi_hartmask_set_hartid(i, &tmp_mask);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_mask);
		ប្រសិនបើ (rc)
			ត្រឡប់ rc;

		sbi_hartmask_and(&domain_mask, &domain_mask, &tmp_mask);
		ប្រសិនបើ (sbi_hartmask_weight(&domain_mask) != count)
			ត្រឡប់ SBI_EINVAL;

		sbi_hartmask_and(&target_mask, &target_mask, &tmp_mask);
	}

	/* Send IPIs */
	do {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_mask) {
			rc = sbi_ipi_send(scratch, i, event, data);
			ប្រសិនបើ (rc < 0)
				goto done;
			ប្រសិនបើ (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			បើមិនដូច្នេះ
				sbi_hartmask_clear_hartindex(i, &target_mask);
			rc = 0;
		}
	} ខណៈពេល (retry_needed);

done:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	ត្រឡប់ rc;
}

ចំនួនគត់ sbi_ipi_event_create(const sa_nracanaa sbi_ipi_event_ops *ops)
{
	ចំនួនគត់ i, ret = SBI_ENOSPC;

	ប្រសិនបើ (!ops || !ops->process)
		ត្រឡប់ SBI_EINVAL;

	សម្រាប់ (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		ប្រសិនបើ (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			bha_nga;
		}
	}

	ត្រឡប់ ret;
}

void sbi_ipi_event_destroy(u32 event)
{
	ប្រសិនបើ (SBI_IPI_EVENT_MAX <= event)
		ត្រឡប់;

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

ចំនួនគត់ sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	ត្រឡប់ sbi_ipi_send_many(hmask, hbase, ipi_smode_event, NULL);
}

void sbi_ipi_clear_smode(void)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

static ចំនួនគត់ sbi_ipi_update_halt(sa_nracanaa sbi_scratch *scratch,
			       sa_nracanaa sbi_scratch *remote_scratch,
			       u32 remote_hartindex, void *data)
{
	/* Never send a halt IPI to the local hart. */
	ត្រឡប់ scratch == remote_scratch ?
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

ចំនួនគត់ sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	ត្រឡប់ sbi_ipi_send_many(hmask, hbase, ipi_halt_event, NULL);
}

void sbi_ipi_process(void)
{
	unsigned long ipi_type;
	unsigned ចំនួនគត់ ipi_event;
	const sa_nracanaa sbi_ipi_event_ops *ipi_ops;
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	sa_nracanaa sbi_ipi_data *ipi_data =
			sbi_scratch_offset_ptr(scratch, ipi_data_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_data->ipi_type, 0);
	ipi_event = 0;
	ខណៈពេល (ipi_type) {
		ប្រសិនបើ (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			ប្រសិនបើ (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

ចំនួនគត់ sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	sa_nracanaa sbi_ipi_device_node *entry;

	ប្រសិនបើ (!ipi_dev || !ipi_dev->ipi_send)
		ត្រឡប់ SBI_EINVAL;

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

	ប្រសិនបើ (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_node_list, head) {
			ប្រសិនបើ (entry->dev->ipi_send)
				entry->dev->ipi_send(hartindex);
		}
	} បើមិនដូច្នេះ {
		ipi_dev->ipi_send(hartindex);
	}

	ត្រឡប់ 0;
}

void sbi_ipi_raw_clear(bool all_devices)
{
	sa_nracanaa sbi_ipi_device_node *entry;

	ប្រសិនបើ (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_node_list, head) {
			ប្រសិនបើ (entry->dev->ipi_clear)
				entry->dev->ipi_clear();
		}
	} បើមិនដូច្នេះ {
		ប្រសិនបើ (ipi_dev && ipi_dev->ipi_clear)
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
	ត្រឡប់ ipi_dev;
}

void sbi_ipi_add_device(const sa_nracanaa sbi_ipi_device *dev)
{
	sa_nracanaa sbi_ipi_device_node *entry;

	ប្រសិនបើ (!dev)
		ត្រឡប់;

	entry = sbi_zalloc(parimaa_na(*entry));
	ប្រសិនបើ (!entry)
		ត្រឡប់;
	SBI_INIT_LIST_HEAD(&entry->head);
	entry->dev = dev;
	sbi_list_add_tail(&entry->head, &ipi_dev_node_list);

	ប្រសិនបើ (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

ចំនួនគត់ sbi_ipi_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	ចំនួនគត់ ret;
	sa_nracanaa sbi_ipi_data *ipi_data;

	ប្រសិនបើ (cold_boot) {
		ipi_data_off = sbi_scratch_alloc_offset(parimaa_na(*ipi_data));
		ប្រសិនបើ (!ipi_data_off)
			ត្រឡប់ SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		ប្រសិនបើ (ret < 0)
			ត្រឡប់ ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		ប្រសិនបើ (ret < 0)
			ត្រឡប់ ret;
		ipi_halt_event = ret;
	} បើមិនដូច្នេះ {
		ប្រសិនបើ (!ipi_data_off)
			ត្រឡប់ SBI_ENOMEM;
		ប្រសិនបើ (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			ត្រឡប់ SBI_ENOSPC;
	}

	ipi_data = sbi_scratch_offset_ptr(scratch, ipi_data_off);
	ipi_data->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_set(CSR_MIE, MIP_MSIP);

	ត្រឡប់ 0;
}

void sbi_ipi_exit(sa_nracanaa sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
