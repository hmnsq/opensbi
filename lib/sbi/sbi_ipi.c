/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_atomic.h>
#અન્તર્ભાવ <sbi/riscv_barrier.h>
#અન્તર્ભાવ <sbi/sbi_bitops.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_hsm.h>
#અન્તર્ભાવ <sbi/sbi_init.h>
#અન્તર્ભાવ <sbi/sbi_ipi.h>
#અન્તર્ભાવ <sbi/sbi_list.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_pmu.h>
#અન્તર્ભાવ <sbi/sbi_string.h>
#અન્તર્ભાવ <sbi/sbi_tlb.h>

સંરચના sbi_ipi_data {
	unsigned long ipi_type;
};

_Static_assert(
	8 * કદ(((સંરચના sbi_ipi_data*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

સંરચના sbi_ipi_device_ગાંઠ {
	સંરચના sbi_dlist head;
	અચલ સંરચના sbi_ipi_device *dev;
};

સ્થિર unsigned long ipi_data_off;
સ્થિર અચલ સંરચના sbi_ipi_device *ipi_dev = ખાલી;
સ્થિર SBI_LIST_HEAD(ipi_dev_ગાંઠ_list);
સ્થિર અચલ સંરચના sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

સ્થિર પૂર્ણાંક sbi_ipi_send(સંરચના sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, શૂન્ય *data)
{
	પૂર્ણાંક ret = 0;
	સંરચના sbi_scratch *remote_scratch = ખાલી;
	સંરચના sbi_ipi_data *ipi_data;
	અચલ સંરચના sbi_ipi_event_ops *ipi_ops;

	જો ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		પાછા SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_to_scratch(remote_hartindex);
	જો (!remote_scratch)
		પાછા SBI_EINVAL;

	ipi_data = sbi_scratch_offset_ptr(remote_scratch, ipi_data_off);

	જો (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, data);
		જો (ret != SBI_IPI_UPDATE_SUCCESS)
			પાછા ret;
	} નહીંતર જો (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		પાછા 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	જો (!__atomic_fetch_or(&ipi_data->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_SENT);

	પાછા ret;
}

સ્થિર પૂર્ણાંક sbi_ipi_sync(સંરચના sbi_scratch *scratch, u32 event)
{
	અચલ સંરચના sbi_ipi_event_ops *ipi_ops;

	જો ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		પાછા SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	જો (ipi_ops->sync)
		ipi_ops->sync(scratch);

	પાછા 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
પૂર્ણાંક sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, શૂન્ય *data)
{
	પૂર્ણાંક rc = 0;
	bool retry_needed;
	ulong i;
	સંરચના sbi_hartmask target_mask;
	સંરચના sbi_domain *dom = sbi_domain_thishart_ptr();
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	જો (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		પાછા 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_mask(dom, &target_mask);
	જો (rc)
		પાછા rc;

	જો (hbase != -1UL) {
		સંરચના sbi_hartmask tmp_mask = { 0 };
		સંરચના sbi_hartmask domain_mask;
		પૂર્ણાંક ગણતરી = sbi_popcount(hmask);

		માટે (i = hbase; hmask; i++, hmask >>= 1) {
			જો (hmask & 1UL)
				sbi_hartmask_set_hartid(i, &tmp_mask);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_mask);
		જો (rc)
			પાછા rc;

		sbi_hartmask_and(&domain_mask, &domain_mask, &tmp_mask);
		જો (sbi_hartmask_weight(&domain_mask) != ગણતરી)
			પાછા SBI_EINVAL;

		sbi_hartmask_and(&target_mask, &target_mask, &tmp_mask);
	}

	/* Send IPIs */
	કરો {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_mask) {
			rc = sbi_ipi_send(scratch, i, event, data);
			જો (rc < 0)
				જાઓ done;
			જો (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			નહીંતર
				sbi_hartmask_clear_hartindex(i, &target_mask);
			rc = 0;
		}
	} જ્યારે (retry_needed);

done:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	પાછા rc;
}

પૂર્ણાંક sbi_ipi_event_create(અચલ સંરચના sbi_ipi_event_ops *ops)
{
	પૂર્ણાંક i, ret = SBI_ENOSPC;

	જો (!ops || !ops->process)
		પાછા SBI_EINVAL;

	માટે (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		જો (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			તોડો;
		}
	}

	પાછા ret;
}

શૂન્ય sbi_ipi_event_destroy(u32 event)
{
	જો (SBI_IPI_EVENT_MAX <= event)
		પાછા;

	ipi_ops_array[event] = ખાલી;
}

સ્થિર શૂન્ય sbi_ipi_process_smode(સંરચના sbi_scratch *scratch)
{
	csr_set(CSR_MIP, MIP_SSIP);
}

સ્થિર સંરચના sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

સ્થિર u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

પૂર્ણાંક sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	પાછા sbi_ipi_send_many(hmask, hbase, ipi_smode_event, ખાલી);
}

શૂન્ય sbi_ipi_clear_smode(શૂન્ય)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

સ્થિર પૂર્ણાંક sbi_ipi_update_halt(સંરચના sbi_scratch *scratch,
			       સંરચના sbi_scratch *remote_scratch,
			       u32 remote_hartindex, શૂન્ય *data)
{
	/* Never send a halt IPI to the local hart. */
	પાછા scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

સ્થિર શૂન્ય sbi_ipi_process_halt(સંરચના sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

સ્થિર સંરચના sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

સ્થિર u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

પૂર્ણાંક sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	પાછા sbi_ipi_send_many(hmask, hbase, ipi_halt_event, ખાલી);
}

શૂન્ય sbi_ipi_process(શૂન્ય)
{
	unsigned long ipi_type;
	unsigned પૂર્ણાંક ipi_event;
	અચલ સંરચના sbi_ipi_event_ops *ipi_ops;
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	સંરચના sbi_ipi_data *ipi_data =
			sbi_scratch_offset_ptr(scratch, ipi_data_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_data->ipi_type, 0);
	ipi_event = 0;
	જ્યારે (ipi_type) {
		જો (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			જો (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

પૂર્ણાંક sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	સંરચના sbi_ipi_device_ગાંઠ *entry;

	જો (!ipi_dev || !ipi_dev->ipi_send)
		પાછા SBI_EINVAL;

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

	જો (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_ગાંઠ_list, head) {
			જો (entry->dev->ipi_send)
				entry->dev->ipi_send(hartindex);
		}
	} નહીંતર {
		ipi_dev->ipi_send(hartindex);
	}

	પાછા 0;
}

શૂન્ય sbi_ipi_raw_clear(bool all_devices)
{
	સંરચના sbi_ipi_device_ગાંઠ *entry;

	જો (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_ગાંઠ_list, head) {
			જો (entry->dev->ipi_clear)
				entry->dev->ipi_clear();
		}
	} નહીંતર {
		જો (ipi_dev && ipi_dev->ipi_clear)
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

અચલ સંરચના sbi_ipi_device *sbi_ipi_get_device(શૂન્ય)
{
	પાછા ipi_dev;
}

શૂન્ય sbi_ipi_add_device(અચલ સંરચના sbi_ipi_device *dev)
{
	સંરચના sbi_ipi_device_ગાંઠ *entry;

	જો (!dev)
		પાછા;

	entry = sbi_zalloc(કદ(*entry));
	જો (!entry)
		પાછા;
	SBI_INIT_LIST_HEAD(&entry->head);
	entry->dev = dev;
	sbi_list_add_tail(&entry->head, &ipi_dev_ગાંઠ_list);

	જો (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

પૂર્ણાંક sbi_ipi_init(સંરચના sbi_scratch *scratch, bool cold_boot)
{
	પૂર્ણાંક ret;
	સંરચના sbi_ipi_data *ipi_data;

	જો (cold_boot) {
		ipi_data_off = sbi_scratch_alloc_offset(કદ(*ipi_data));
		જો (!ipi_data_off)
			પાછા SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		જો (ret < 0)
			પાછા ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		જો (ret < 0)
			પાછા ret;
		ipi_halt_event = ret;
	} નહીંતર {
		જો (!ipi_data_off)
			પાછા SBI_ENOMEM;
		જો (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			પાછા SBI_ENOSPC;
	}

	ipi_data = sbi_scratch_offset_ptr(scratch, ipi_data_off);
	ipi_data->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_set(CSR_MIE, MIP_MSIP);

	પાછા 0;
}

શૂન્ય sbi_ipi_exit(સંરચના sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
