/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_atomic.h>
#അന്തര്ഭാവ <sbi/riscv_barrier.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_hsm.h>
#അന്തര്ഭാവ <sbi/sbi_init.h>
#അന്തര്ഭാവ <sbi/sbi_ipi.h>
#അന്തര്ഭാവ <sbi/sbi_list.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_pmu.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi/sbi_tlb.h>

ഘടന sbi_ipi_data {
	unsigned long ipi_type;
};

_Static_assert(
	8 * വലുപ്പം(((ഘടന sbi_ipi_data*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

ഘടന sbi_ipi_device_കണ്ണി {
	ഘടന sbi_dlist head;
	സ്ഥിരം ഘടന sbi_ipi_device *dev;
};

സ്ഥിതം unsigned long ipi_data_off;
സ്ഥിതം സ്ഥിരം ഘടന sbi_ipi_device *ipi_dev = ശൂന്യ;
സ്ഥിതം SBI_LIST_HEAD(ipi_dev_കണ്ണി_list);
സ്ഥിതം സ്ഥിരം ഘടന sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ipi_send(ഘടന sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, ശൂന്യം *data)
{
	പൂർണ്ണസംഖ്യ ret = 0;
	ഘടന sbi_scratch *remote_scratch = ശൂന്യ;
	ഘടന sbi_ipi_data *ipi_data;
	സ്ഥിരം ഘടന sbi_ipi_event_ops *ipi_ops;

	എങ്കിൽ ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		തിരികെ SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_to_scratch(remote_hartindex);
	എങ്കിൽ (!remote_scratch)
		തിരികെ SBI_EINVAL;

	ipi_data = sbi_scratch_offset_ptr(remote_scratch, ipi_data_off);

	എങ്കിൽ (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, data);
		എങ്കിൽ (ret != SBI_IPI_UPDATE_SUCCESS)
			തിരികെ ret;
	} അല്ലെങ്കിൽ എങ്കിൽ (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		തിരികെ 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	എങ്കിൽ (!__atomic_fetch_or(&ipi_data->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_SENT);

	തിരികെ ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ipi_sync(ഘടന sbi_scratch *scratch, u32 event)
{
	സ്ഥിരം ഘടന sbi_ipi_event_ops *ipi_ops;

	എങ്കിൽ ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		തിരികെ SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	എങ്കിൽ (ipi_ops->sync)
		ipi_ops->sync(scratch);

	തിരികെ 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
പൂർണ്ണസംഖ്യ sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, ശൂന്യം *data)
{
	പൂർണ്ണസംഖ്യ rc = 0;
	bool retry_needed;
	ulong i;
	ഘടന sbi_hartmask target_mask;
	ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	എങ്കിൽ (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		തിരികെ 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_mask(dom, &target_mask);
	എങ്കിൽ (rc)
		തിരികെ rc;

	എങ്കിൽ (hbase != -1UL) {
		ഘടന sbi_hartmask tmp_mask = { 0 };
		ഘടന sbi_hartmask domain_mask;
		പൂർണ്ണസംഖ്യ എണ്ണം = sbi_popcount(hmask);

		വേണ്ടി (i = hbase; hmask; i++, hmask >>= 1) {
			എങ്കിൽ (hmask & 1UL)
				sbi_hartmask_set_hartid(i, &tmp_mask);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_mask);
		എങ്കിൽ (rc)
			തിരികെ rc;

		sbi_hartmask_and(&domain_mask, &domain_mask, &tmp_mask);
		എങ്കിൽ (sbi_hartmask_weight(&domain_mask) != എണ്ണം)
			തിരികെ SBI_EINVAL;

		sbi_hartmask_and(&target_mask, &target_mask, &tmp_mask);
	}

	/* Send IPIs */
	ചെയ്യുക {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_mask) {
			rc = sbi_ipi_send(scratch, i, event, data);
			എങ്കിൽ (rc < 0)
				പോകുക done;
			എങ്കിൽ (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			അല്ലെങ്കിൽ
				sbi_hartmask_clear_hartindex(i, &target_mask);
			rc = 0;
		}
	} ആകുമ്പോൾ (retry_needed);

done:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	തിരികെ rc;
}

പൂർണ്ണസംഖ്യ sbi_ipi_event_create(സ്ഥിരം ഘടന sbi_ipi_event_ops *ops)
{
	പൂർണ്ണസംഖ്യ i, ret = SBI_ENOSPC;

	എങ്കിൽ (!ops || !ops->process)
		തിരികെ SBI_EINVAL;

	വേണ്ടി (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		എങ്കിൽ (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			നിർത്തുക;
		}
	}

	തിരികെ ret;
}

ശൂന്യം sbi_ipi_event_destroy(u32 event)
{
	എങ്കിൽ (SBI_IPI_EVENT_MAX <= event)
		തിരികെ;

	ipi_ops_array[event] = ശൂന്യ;
}

സ്ഥിതം ശൂന്യം sbi_ipi_process_smode(ഘടന sbi_scratch *scratch)
{
	csr_set(CSR_MIP, MIP_SSIP);
}

സ്ഥിതം ഘടന sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

സ്ഥിതം u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

പൂർണ്ണസംഖ്യ sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	തിരികെ sbi_ipi_send_many(hmask, hbase, ipi_smode_event, ശൂന്യ);
}

ശൂന്യം sbi_ipi_clear_smode(ശൂന്യം)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ipi_update_halt(ഘടന sbi_scratch *scratch,
			       ഘടന sbi_scratch *remote_scratch,
			       u32 remote_hartindex, ശൂന്യം *data)
{
	/* Never send a halt IPI to the local hart. */
	തിരികെ scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

സ്ഥിതം ശൂന്യം sbi_ipi_process_halt(ഘടന sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

സ്ഥിതം ഘടന sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

സ്ഥിതം u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

പൂർണ്ണസംഖ്യ sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	തിരികെ sbi_ipi_send_many(hmask, hbase, ipi_halt_event, ശൂന്യ);
}

ശൂന്യം sbi_ipi_process(ശൂന്യം)
{
	unsigned long ipi_type;
	unsigned പൂർണ്ണസംഖ്യ ipi_event;
	സ്ഥിരം ഘടന sbi_ipi_event_ops *ipi_ops;
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ഘടന sbi_ipi_data *ipi_data =
			sbi_scratch_offset_ptr(scratch, ipi_data_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_data->ipi_type, 0);
	ipi_event = 0;
	ആകുമ്പോൾ (ipi_type) {
		എങ്കിൽ (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			എങ്കിൽ (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

പൂർണ്ണസംഖ്യ sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	ഘടന sbi_ipi_device_കണ്ണി *entry;

	എങ്കിൽ (!ipi_dev || !ipi_dev->ipi_send)
		തിരികെ SBI_EINVAL;

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

	എങ്കിൽ (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_കണ്ണി_list, head) {
			എങ്കിൽ (entry->dev->ipi_send)
				entry->dev->ipi_send(hartindex);
		}
	} അല്ലെങ്കിൽ {
		ipi_dev->ipi_send(hartindex);
	}

	തിരികെ 0;
}

ശൂന്യം sbi_ipi_raw_clear(bool all_devices)
{
	ഘടന sbi_ipi_device_കണ്ണി *entry;

	എങ്കിൽ (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_കണ്ണി_list, head) {
			എങ്കിൽ (entry->dev->ipi_clear)
				entry->dev->ipi_clear();
		}
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (ipi_dev && ipi_dev->ipi_clear)
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

സ്ഥിരം ഘടന sbi_ipi_device *sbi_ipi_get_device(ശൂന്യം)
{
	തിരികെ ipi_dev;
}

ശൂന്യം sbi_ipi_add_device(സ്ഥിരം ഘടന sbi_ipi_device *dev)
{
	ഘടന sbi_ipi_device_കണ്ണി *entry;

	എങ്കിൽ (!dev)
		തിരികെ;

	entry = sbi_zalloc(വലുപ്പം(*entry));
	എങ്കിൽ (!entry)
		തിരികെ;
	SBI_INIT_LIST_HEAD(&entry->head);
	entry->dev = dev;
	sbi_list_add_tail(&entry->head, &ipi_dev_കണ്ണി_list);

	എങ്കിൽ (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

പൂർണ്ണസംഖ്യ sbi_ipi_init(ഘടന sbi_scratch *scratch, bool cold_boot)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന sbi_ipi_data *ipi_data;

	എങ്കിൽ (cold_boot) {
		ipi_data_off = sbi_scratch_alloc_offset(വലുപ്പം(*ipi_data));
		എങ്കിൽ (!ipi_data_off)
			തിരികെ SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		എങ്കിൽ (ret < 0)
			തിരികെ ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		എങ്കിൽ (ret < 0)
			തിരികെ ret;
		ipi_halt_event = ret;
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (!ipi_data_off)
			തിരികെ SBI_ENOMEM;
		എങ്കിൽ (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			തിരികെ SBI_ENOSPC;
	}

	ipi_data = sbi_scratch_offset_ptr(scratch, ipi_data_off);
	ipi_data->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_set(CSR_MIE, MIP_MSIP);

	തിരികെ 0;
}

ശൂന്യം sbi_ipi_exit(ഘടന sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
