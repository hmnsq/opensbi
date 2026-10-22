/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_atomic.h>
#অন্তর্ভাব <sbi/riscv_barrier.h>
#অন্তর্ভাব <sbi/sbi_bitops.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_hsm.h>
#অন্তর্ভাব <sbi/sbi_init.h>
#অন্তর্ভাব <sbi/sbi_ipi.h>
#অন্তর্ভাব <sbi/sbi_list.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_pmu.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi/sbi_tlb.h>

সংরচনা sbi_ipi_data {
	unsigned long ipi_type;
};

_Static_assert(
	8 * পরিমাণ(((সংরচনা sbi_ipi_data*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

সংরচনা sbi_ipi_device_গ্রন্থি {
	সংরচনা sbi_dlist head;
	ধ্রুব সংরচনা sbi_ipi_device *dev;
};

স্থির unsigned long ipi_data_off;
স্থির ধ্রুব সংরচনা sbi_ipi_device *ipi_dev = রিক্ত;
স্থির SBI_LIST_HEAD(ipi_dev_গ্রন্থি_list);
স্থির ধ্রুব সংরচনা sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

স্থির পূর্ণাঙ্ক sbi_ipi_send(সংরচনা sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, শূন্য *data)
{
	পূর্ণাঙ্ক ret = 0;
	সংরচনা sbi_scratch *remote_scratch = রিক্ত;
	সংরচনা sbi_ipi_data *ipi_data;
	ধ্রুব সংরচনা sbi_ipi_event_ops *ipi_ops;

	যদি ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		প্রত্যাগম SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_to_scratch(remote_hartindex);
	যদি (!remote_scratch)
		প্রত্যাগম SBI_EINVAL;

	ipi_data = sbi_scratch_offset_ptr(remote_scratch, ipi_data_off);

	যদি (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, data);
		যদি (ret != SBI_IPI_UPDATE_SUCCESS)
			প্রত্যাগম ret;
	} অন্যথা যদি (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		প্রত্যাগম 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	যদি (!__atomic_fetch_or(&ipi_data->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_SENT);

	প্রত্যাগম ret;
}

স্থির পূর্ণাঙ্ক sbi_ipi_sync(সংরচনা sbi_scratch *scratch, u32 event)
{
	ধ্রুব সংরচনা sbi_ipi_event_ops *ipi_ops;

	যদি ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		প্রত্যাগম SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	যদি (ipi_ops->sync)
		ipi_ops->sync(scratch);

	প্রত্যাগম 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
পূর্ণাঙ্ক sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, শূন্য *data)
{
	পূর্ণাঙ্ক rc = 0;
	bool retry_needed;
	ulong i;
	সংরচনা sbi_hartmask target_mask;
	সংরচনা sbi_domain *dom = sbi_domain_thishart_ptr();
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	যদি (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		প্রত্যাগম 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_mask(dom, &target_mask);
	যদি (rc)
		প্রত্যাগম rc;

	যদি (hbase != -1UL) {
		সংরচনা sbi_hartmask tmp_mask = { 0 };
		সংরচনা sbi_hartmask domain_mask;
		পূর্ণাঙ্ক গুনতি = sbi_popcount(hmask);

		প্রতি (i = hbase; hmask; i++, hmask >>= 1) {
			যদি (hmask & 1UL)
				sbi_hartmask_set_hartid(i, &tmp_mask);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_mask);
		যদি (rc)
			প্রত্যাগম rc;

		sbi_hartmask_and(&domain_mask, &domain_mask, &tmp_mask);
		যদি (sbi_hartmask_weight(&domain_mask) != গুনতি)
			প্রত্যাগম SBI_EINVAL;

		sbi_hartmask_and(&target_mask, &target_mask, &tmp_mask);
	}

	/* Send IPIs */
	কুরু {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_mask) {
			rc = sbi_ipi_send(scratch, i, event, data);
			যদি (rc < 0)
				গচ্ছ done;
			যদি (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			অন্যথা
				sbi_hartmask_clear_hartindex(i, &target_mask);
			rc = 0;
		}
	} যাবত্ (retry_needed);

done:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	প্রত্যাগম rc;
}

পূর্ণাঙ্ক sbi_ipi_event_create(ধ্রুব সংরচনা sbi_ipi_event_ops *ops)
{
	পূর্ণাঙ্ক i, ret = SBI_ENOSPC;

	যদি (!ops || !ops->process)
		প্রত্যাগম SBI_EINVAL;

	প্রতি (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		যদি (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			ভঙ্গ;
		}
	}

	প্রত্যাগম ret;
}

শূন্য sbi_ipi_event_destroy(u32 event)
{
	যদি (SBI_IPI_EVENT_MAX <= event)
		প্রত্যাগম;

	ipi_ops_array[event] = রিক্ত;
}

স্থির শূন্য sbi_ipi_process_smode(সংরচনা sbi_scratch *scratch)
{
	csr_set(CSR_MIP, MIP_SSIP);
}

স্থির সংরচনা sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

স্থির u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

পূর্ণাঙ্ক sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	প্রত্যাগম sbi_ipi_send_many(hmask, hbase, ipi_smode_event, রিক্ত);
}

শূন্য sbi_ipi_clear_smode(শূন্য)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

স্থির পূর্ণাঙ্ক sbi_ipi_update_halt(সংরচনা sbi_scratch *scratch,
			       সংরচনা sbi_scratch *remote_scratch,
			       u32 remote_hartindex, শূন্য *data)
{
	/* Never send a halt IPI to the local hart. */
	প্রত্যাগম scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

স্থির শূন্য sbi_ipi_process_halt(সংরচনা sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

স্থির সংরচনা sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

স্থির u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

পূর্ণাঙ্ক sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	প্রত্যাগম sbi_ipi_send_many(hmask, hbase, ipi_halt_event, রিক্ত);
}

শূন্য sbi_ipi_process(শূন্য)
{
	unsigned long ipi_type;
	unsigned পূর্ণাঙ্ক ipi_event;
	ধ্রুব সংরচনা sbi_ipi_event_ops *ipi_ops;
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	সংরচনা sbi_ipi_data *ipi_data =
			sbi_scratch_offset_ptr(scratch, ipi_data_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_data->ipi_type, 0);
	ipi_event = 0;
	যাবত্ (ipi_type) {
		যদি (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			যদি (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

পূর্ণাঙ্ক sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	সংরচনা sbi_ipi_device_গ্রন্থি *entry;

	যদি (!ipi_dev || !ipi_dev->ipi_send)
		প্রত্যাগম SBI_EINVAL;

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

	যদি (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_গ্রন্থি_list, head) {
			যদি (entry->dev->ipi_send)
				entry->dev->ipi_send(hartindex);
		}
	} অন্যথা {
		ipi_dev->ipi_send(hartindex);
	}

	প্রত্যাগম 0;
}

শূন্য sbi_ipi_raw_clear(bool all_devices)
{
	সংরচনা sbi_ipi_device_গ্রন্থি *entry;

	যদি (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_গ্রন্থি_list, head) {
			যদি (entry->dev->ipi_clear)
				entry->dev->ipi_clear();
		}
	} অন্যথা {
		যদি (ipi_dev && ipi_dev->ipi_clear)
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

ধ্রুব সংরচনা sbi_ipi_device *sbi_ipi_get_device(শূন্য)
{
	প্রত্যাগম ipi_dev;
}

শূন্য sbi_ipi_add_device(ধ্রুব সংরচনা sbi_ipi_device *dev)
{
	সংরচনা sbi_ipi_device_গ্রন্থি *entry;

	যদি (!dev)
		প্রত্যাগম;

	entry = sbi_zalloc(পরিমাণ(*entry));
	যদি (!entry)
		প্রত্যাগম;
	SBI_INIT_LIST_HEAD(&entry->head);
	entry->dev = dev;
	sbi_list_add_tail(&entry->head, &ipi_dev_গ্রন্থি_list);

	যদি (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

পূর্ণাঙ্ক sbi_ipi_init(সংরচনা sbi_scratch *scratch, bool cold_boot)
{
	পূর্ণাঙ্ক ret;
	সংরচনা sbi_ipi_data *ipi_data;

	যদি (cold_boot) {
		ipi_data_off = sbi_scratch_alloc_offset(পরিমাণ(*ipi_data));
		যদি (!ipi_data_off)
			প্রত্যাগম SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		যদি (ret < 0)
			প্রত্যাগম ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		যদি (ret < 0)
			প্রত্যাগম ret;
		ipi_halt_event = ret;
	} অন্যথা {
		যদি (!ipi_data_off)
			প্রত্যাগম SBI_ENOMEM;
		যদি (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			প্রত্যাগম SBI_ENOSPC;
	}

	ipi_data = sbi_scratch_offset_ptr(scratch, ipi_data_off);
	ipi_data->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_set(CSR_MIE, MIP_MSIP);

	প্রত্যাগম 0;
}

শূন্য sbi_ipi_exit(সংরচনা sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
