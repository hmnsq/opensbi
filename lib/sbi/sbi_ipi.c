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

কাঠামো sbi_ipi_তথ্য {
	unsigned long ipi_type;
};

_Static_assert(
	8 * আকার(((কাঠামো sbi_ipi_তথ্য*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

কাঠামো sbi_ipi_device_গ্রন্থি {
	কাঠামো sbi_dlist শীর্ষ;
	ধ্রুবক কাঠামো sbi_ipi_device *dev;
};

স্থির unsigned long ipi_তথ্য_off;
স্থির ধ্রুবক কাঠামো sbi_ipi_device *ipi_dev = ফাঁকা;
স্থির SBI_LIST_শীর্ষ(ipi_dev_গ্রন্থি_list);
স্থির ধ্রুবক কাঠামো sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

স্থির পূর্ণসংখ্যা sbi_ipi_send(কাঠামো sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, শূন্য *তথ্য)
{
	পূর্ণসংখ্যা ret = 0;
	কাঠামো sbi_scratch *remote_scratch = ফাঁকা;
	কাঠামো sbi_ipi_তথ্য *ipi_তথ্য;
	ধ্রুবক কাঠামো sbi_ipi_event_ops *ipi_ops;

	যদি ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		ফেরত SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_to_scratch(remote_hartindex);
	যদি (!remote_scratch)
		ফেরত SBI_EINVAL;

	ipi_তথ্য = sbi_scratch_offset_ptr(remote_scratch, ipi_তথ্য_off);

	যদি (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, তথ্য);
		যদি (ret != SBI_IPI_UPDATE_SUCCESS)
			ফেরত ret;
	} অন্যথা যদি (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		ফেরত 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	যদি (!__atomic_fetch_or(&ipi_তথ্য->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_SENT);

	ফেরত ret;
}

স্থির পূর্ণসংখ্যা sbi_ipi_sync(কাঠামো sbi_scratch *scratch, u32 event)
{
	ধ্রুবক কাঠামো sbi_ipi_event_ops *ipi_ops;

	যদি ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		ফেরত SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	যদি (ipi_ops->sync)
		ipi_ops->sync(scratch);

	ফেরত 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
পূর্ণসংখ্যা sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, শূন্য *তথ্য)
{
	পূর্ণসংখ্যা rc = 0;
	bool retry_needed;
	ulong i;
	কাঠামো sbi_hartmask target_mask;
	কাঠামো sbi_domain *dom = sbi_domain_thishart_ptr();
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	যদি (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		ফেরত 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_mask(dom, &target_mask);
	যদি (rc)
		ফেরত rc;

	যদি (hbase != -1UL) {
		কাঠামো sbi_hartmask tmp_mask = { 0 };
		কাঠামো sbi_hartmask domain_mask;
		পূর্ণসংখ্যা গুনতি = sbi_popcount(hmask);

		জন্য (i = hbase; hmask; i++, hmask >>= 1) {
			যদি (hmask & 1UL)
				sbi_hartmask_set_hartid(i, &tmp_mask);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_mask);
		যদি (rc)
			ফেরত rc;

		sbi_hartmask_and(&domain_mask, &domain_mask, &tmp_mask);
		যদি (sbi_hartmask_weight(&domain_mask) != গুনতি)
			ফেরত SBI_EINVAL;

		sbi_hartmask_and(&target_mask, &target_mask, &tmp_mask);
	}

	/* Send IPIs */
	করো {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_mask) {
			rc = sbi_ipi_send(scratch, i, event, তথ্য);
			যদি (rc < 0)
				যাও done;
			যদি (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			অন্যথা
				sbi_hartmask_clear_hartindex(i, &target_mask);
			rc = 0;
		}
	} যখন (retry_needed);

done:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	ফেরত rc;
}

পূর্ণসংখ্যা sbi_ipi_event_create(ধ্রুবক কাঠামো sbi_ipi_event_ops *ops)
{
	পূর্ণসংখ্যা i, ret = SBI_ENOSPC;

	যদি (!ops || !ops->process)
		ফেরত SBI_EINVAL;

	জন্য (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		যদি (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			বিরতি;
		}
	}

	ফেরত ret;
}

শূন্য sbi_ipi_event_destroy(u32 event)
{
	যদি (SBI_IPI_EVENT_MAX <= event)
		ফেরত;

	ipi_ops_array[event] = ফাঁকা;
}

স্থির শূন্য sbi_ipi_process_smode(কাঠামো sbi_scratch *scratch)
{
	csr_set(CSR_MIP, MIP_SSIP);
}

স্থির কাঠামো sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

স্থির u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

পূর্ণসংখ্যা sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	ফেরত sbi_ipi_send_many(hmask, hbase, ipi_smode_event, ফাঁকা);
}

শূন্য sbi_ipi_clear_smode(শূন্য)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

স্থির পূর্ণসংখ্যা sbi_ipi_update_halt(কাঠামো sbi_scratch *scratch,
			       কাঠামো sbi_scratch *remote_scratch,
			       u32 remote_hartindex, শূন্য *তথ্য)
{
	/* Never send a halt IPI to the local hart. */
	ফেরত scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

স্থির শূন্য sbi_ipi_process_halt(কাঠামো sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

স্থির কাঠামো sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

স্থির u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

পূর্ণসংখ্যা sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	ফেরত sbi_ipi_send_many(hmask, hbase, ipi_halt_event, ফাঁকা);
}

শূন্য sbi_ipi_process(শূন্য)
{
	unsigned long ipi_type;
	unsigned পূর্ণসংখ্যা ipi_event;
	ধ্রুবক কাঠামো sbi_ipi_event_ops *ipi_ops;
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	কাঠামো sbi_ipi_তথ্য *ipi_তথ্য =
			sbi_scratch_offset_ptr(scratch, ipi_তথ্য_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_তথ্য->ipi_type, 0);
	ipi_event = 0;
	যখন (ipi_type) {
		যদি (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			যদি (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

পূর্ণসংখ্যা sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	কাঠামো sbi_ipi_device_গ্রন্থি *entry;

	যদি (!ipi_dev || !ipi_dev->ipi_send)
		ফেরত SBI_EINVAL;

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
		sbi_list_for_each_entry(entry, &ipi_dev_গ্রন্থি_list, শীর্ষ) {
			যদি (entry->dev->ipi_send)
				entry->dev->ipi_send(hartindex);
		}
	} অন্যথা {
		ipi_dev->ipi_send(hartindex);
	}

	ফেরত 0;
}

শূন্য sbi_ipi_raw_clear(bool all_devices)
{
	কাঠামো sbi_ipi_device_গ্রন্থি *entry;

	যদি (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_গ্রন্থি_list, শীর্ষ) {
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

ধ্রুবক কাঠামো sbi_ipi_device *sbi_ipi_get_device(শূন্য)
{
	ফেরত ipi_dev;
}

শূন্য sbi_ipi_add_device(ধ্রুবক কাঠামো sbi_ipi_device *dev)
{
	কাঠামো sbi_ipi_device_গ্রন্থি *entry;

	যদি (!dev)
		ফেরত;

	entry = sbi_zalloc(আকার(*entry));
	যদি (!entry)
		ফেরত;
	SBI_INIT_LIST_শীর্ষ(&entry->শীর্ষ);
	entry->dev = dev;
	sbi_list_add_লেজ(&entry->শীর্ষ, &ipi_dev_গ্রন্থি_list);

	যদি (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

পূর্ণসংখ্যা sbi_ipi_init(কাঠামো sbi_scratch *scratch, bool cold_boot)
{
	পূর্ণসংখ্যা ret;
	কাঠামো sbi_ipi_তথ্য *ipi_তথ্য;

	যদি (cold_boot) {
		ipi_তথ্য_off = sbi_scratch_alloc_offset(আকার(*ipi_তথ্য));
		যদি (!ipi_তথ্য_off)
			ফেরত SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		যদি (ret < 0)
			ফেরত ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		যদি (ret < 0)
			ফেরত ret;
		ipi_halt_event = ret;
	} অন্যথা {
		যদি (!ipi_তথ্য_off)
			ফেরত SBI_ENOMEM;
		যদি (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			ফেরত SBI_ENOSPC;
	}

	ipi_তথ্য = sbi_scratch_offset_ptr(scratch, ipi_তথ্য_off);
	ipi_তথ্য->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_set(CSR_MIE, MIP_MSIP);

	ফেরত 0;
}

শূন্য sbi_ipi_exit(কাঠামো sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
