/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_atomic.h>
#அந்தர்பாவ <sbi/riscv_barrier.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_hsm.h>
#அந்தர்பாவ <sbi/sbi_init.h>
#அந்தர்பாவ <sbi/sbi_ipi.h>
#அந்தர்பாவ <sbi/sbi_list.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_pmu.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi/sbi_tlb.h>

கட்டமைப்பு sbi_ipi_data {
	unsigned long ipi_type;
};

_Static_assert(
	8 * அளவு(((கட்டமைப்பு sbi_ipi_data*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

கட்டமைப்பு sbi_ipi_device_முனை {
	கட்டமைப்பு sbi_dlist head;
	மாறிலி கட்டமைப்பு sbi_ipi_device *dev;
};

நிலையான unsigned long ipi_data_off;
நிலையான மாறிலி கட்டமைப்பு sbi_ipi_device *ipi_dev = வெற்று;
நிலையான SBI_LIST_HEAD(ipi_dev_முனை_list);
நிலையான மாறிலி கட்டமைப்பு sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

நிலையான முழு sbi_ipi_send(கட்டமைப்பு sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, வெறுமை *data)
{
	முழு ret = 0;
	கட்டமைப்பு sbi_scratch *remote_scratch = வெற்று;
	கட்டமைப்பு sbi_ipi_data *ipi_data;
	மாறிலி கட்டமைப்பு sbi_ipi_event_ops *ipi_ops;

	எனில் ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		திரும்பு SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_to_scratch(remote_hartindex);
	எனில் (!remote_scratch)
		திரும்பு SBI_EINVAL;

	ipi_data = sbi_scratch_offset_ptr(remote_scratch, ipi_data_off);

	எனில் (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, data);
		எனில் (ret != SBI_IPI_UPDATE_SUCCESS)
			திரும்பு ret;
	} இல்லையெனில் எனில் (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		திரும்பு 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	எனில் (!__atomic_fetch_or(&ipi_data->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_SENT);

	திரும்பு ret;
}

நிலையான முழு sbi_ipi_sync(கட்டமைப்பு sbi_scratch *scratch, u32 event)
{
	மாறிலி கட்டமைப்பு sbi_ipi_event_ops *ipi_ops;

	எனில் ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		திரும்பு SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	எனில் (ipi_ops->sync)
		ipi_ops->sync(scratch);

	திரும்பு 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
முழு sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, வெறுமை *data)
{
	முழு rc = 0;
	bool retry_needed;
	ulong i;
	கட்டமைப்பு sbi_hartmask target_mask;
	கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	எனில் (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		திரும்பு 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_mask(dom, &target_mask);
	எனில் (rc)
		திரும்பு rc;

	எனில் (hbase != -1UL) {
		கட்டமைப்பு sbi_hartmask tmp_mask = { 0 };
		கட்டமைப்பு sbi_hartmask domain_mask;
		முழு மொத்தம் = sbi_popcount(hmask);

		க்கு (i = hbase; hmask; i++, hmask >>= 1) {
			எனில் (hmask & 1UL)
				sbi_hartmask_set_hartid(i, &tmp_mask);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_mask);
		எனில் (rc)
			திரும்பு rc;

		sbi_hartmask_and(&domain_mask, &domain_mask, &tmp_mask);
		எனில் (sbi_hartmask_weight(&domain_mask) != மொத்தம்)
			திரும்பு SBI_EINVAL;

		sbi_hartmask_and(&target_mask, &target_mask, &tmp_mask);
	}

	/* Send IPIs */
	செய் {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_mask) {
			rc = sbi_ipi_send(scratch, i, event, data);
			எனில் (rc < 0)
				செல் done;
			எனில் (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			இல்லையெனில்
				sbi_hartmask_clear_hartindex(i, &target_mask);
			rc = 0;
		}
	} போது (retry_needed);

done:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	திரும்பு rc;
}

முழு sbi_ipi_event_create(மாறிலி கட்டமைப்பு sbi_ipi_event_ops *ops)
{
	முழு i, ret = SBI_ENOSPC;

	எனில் (!ops || !ops->process)
		திரும்பு SBI_EINVAL;

	க்கு (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		எனில் (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			நிறுத்து;
		}
	}

	திரும்பு ret;
}

வெறுமை sbi_ipi_event_destroy(u32 event)
{
	எனில் (SBI_IPI_EVENT_MAX <= event)
		திரும்பு;

	ipi_ops_array[event] = வெற்று;
}

நிலையான வெறுமை sbi_ipi_process_smode(கட்டமைப்பு sbi_scratch *scratch)
{
	csr_set(CSR_MIP, MIP_SSIP);
}

நிலையான கட்டமைப்பு sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

நிலையான u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

முழு sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	திரும்பு sbi_ipi_send_many(hmask, hbase, ipi_smode_event, வெற்று);
}

வெறுமை sbi_ipi_clear_smode(வெறுமை)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

நிலையான முழு sbi_ipi_update_halt(கட்டமைப்பு sbi_scratch *scratch,
			       கட்டமைப்பு sbi_scratch *remote_scratch,
			       u32 remote_hartindex, வெறுமை *data)
{
	/* Never send a halt IPI to the local hart. */
	திரும்பு scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

நிலையான வெறுமை sbi_ipi_process_halt(கட்டமைப்பு sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

நிலையான கட்டமைப்பு sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

நிலையான u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

முழு sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	திரும்பு sbi_ipi_send_many(hmask, hbase, ipi_halt_event, வெற்று);
}

வெறுமை sbi_ipi_process(வெறுமை)
{
	unsigned long ipi_type;
	unsigned முழு ipi_event;
	மாறிலி கட்டமைப்பு sbi_ipi_event_ops *ipi_ops;
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	கட்டமைப்பு sbi_ipi_data *ipi_data =
			sbi_scratch_offset_ptr(scratch, ipi_data_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_data->ipi_type, 0);
	ipi_event = 0;
	போது (ipi_type) {
		எனில் (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			எனில் (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

முழு sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	கட்டமைப்பு sbi_ipi_device_முனை *entry;

	எனில் (!ipi_dev || !ipi_dev->ipi_send)
		திரும்பு SBI_EINVAL;

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

	எனில் (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_முனை_list, head) {
			எனில் (entry->dev->ipi_send)
				entry->dev->ipi_send(hartindex);
		}
	} இல்லையெனில் {
		ipi_dev->ipi_send(hartindex);
	}

	திரும்பு 0;
}

வெறுமை sbi_ipi_raw_clear(bool all_devices)
{
	கட்டமைப்பு sbi_ipi_device_முனை *entry;

	எனில் (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_முனை_list, head) {
			எனில் (entry->dev->ipi_clear)
				entry->dev->ipi_clear();
		}
	} இல்லையெனில் {
		எனில் (ipi_dev && ipi_dev->ipi_clear)
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

மாறிலி கட்டமைப்பு sbi_ipi_device *sbi_ipi_get_device(வெறுமை)
{
	திரும்பு ipi_dev;
}

வெறுமை sbi_ipi_add_device(மாறிலி கட்டமைப்பு sbi_ipi_device *dev)
{
	கட்டமைப்பு sbi_ipi_device_முனை *entry;

	எனில் (!dev)
		திரும்பு;

	entry = sbi_zalloc(அளவு(*entry));
	எனில் (!entry)
		திரும்பு;
	SBI_INIT_LIST_HEAD(&entry->head);
	entry->dev = dev;
	sbi_list_add_tail(&entry->head, &ipi_dev_முனை_list);

	எனில் (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

முழு sbi_ipi_init(கட்டமைப்பு sbi_scratch *scratch, bool cold_boot)
{
	முழு ret;
	கட்டமைப்பு sbi_ipi_data *ipi_data;

	எனில் (cold_boot) {
		ipi_data_off = sbi_scratch_alloc_offset(அளவு(*ipi_data));
		எனில் (!ipi_data_off)
			திரும்பு SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		எனில் (ret < 0)
			திரும்பு ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		எனில் (ret < 0)
			திரும்பு ret;
		ipi_halt_event = ret;
	} இல்லையெனில் {
		எனில் (!ipi_data_off)
			திரும்பு SBI_ENOMEM;
		எனில் (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			திரும்பு SBI_ENOSPC;
	}

	ipi_data = sbi_scratch_offset_ptr(scratch, ipi_data_off);
	ipi_data->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_set(CSR_MIE, MIP_MSIP);

	திரும்பு 0;
}

வெறுமை sbi_ipi_exit(கட்டமைப்பு sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
