/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_atomic.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_barrier.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_init.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_list.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmu.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_tlb.h>

ಸಂರಚನಾ sbi_ipi_data {
	unsigned long ipi_type;
};

_Static_assert(
	8 * ಪರಿಮಾಣ(((ಸಂರಚನಾ sbi_ipi_data*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

ಸಂರಚನಾ sbi_ipi_device_ಗಂಟು {
	ಸಂರಚನಾ sbi_dlist head;
	ಧ್ರುವ ಸಂರಚನಾ sbi_ipi_device *dev;
};

ಸ್ಥಿರ unsigned long ipi_data_off;
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sbi_ipi_device *ipi_dev = ರಿಕ್ತ;
ಸ್ಥಿರ SBI_LIST_HEAD(ipi_dev_ಗಂಟು_list);
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ipi_send(ಸಂರಚನಾ sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, ಶೂನ್ಯ *data)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;
	ಸಂರಚನಾ sbi_scratch *remote_scratch = ರಿಕ್ತ;
	ಸಂರಚನಾ sbi_ipi_data *ipi_data;
	ಧ್ರುವ ಸಂರಚನಾ sbi_ipi_event_ops *ipi_ops;

	ಯದಿ ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_to_scratch(remote_hartindex);
	ಯದಿ (!remote_scratch)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ipi_data = sbi_scratch_offset_ptr(remote_scratch, ipi_data_off);

	ಯದಿ (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, data);
		ಯದಿ (ret != SBI_IPI_UPDATE_SUCCESS)
			ಪ್ರತ್ಯಾಗಮ ret;
	} ಅನ್ಯಥಾ ಯದಿ (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	ಯದಿ (!__atomic_fetch_or(&ipi_data->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_SENT);

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ipi_sync(ಸಂರಚನಾ sbi_scratch *scratch, u32 event)
{
	ಧ್ರುವ ಸಂರಚನಾ sbi_ipi_event_ops *ipi_ops;

	ಯದಿ ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	ಯದಿ (ipi_ops->sync)
		ipi_ops->sync(scratch);

	ಪ್ರತ್ಯಾಗಮ 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
ಪೂರ್ಣಾಙ್ಕ sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, ಶೂನ್ಯ *data)
{
	ಪೂರ್ಣಾಙ್ಕ rc = 0;
	bool retry_needed;
	ulong i;
	ಸಂರಚನಾ sbi_hartmask target_mask;
	ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ಯದಿ (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_mask(dom, &target_mask);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	ಯದಿ (hbase != -1UL) {
		ಸಂರಚನಾ sbi_hartmask tmp_mask = { 0 };
		ಸಂರಚನಾ sbi_hartmask domain_mask;
		ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = sbi_popcount(hmask);

		ಪ್ರತಿ (i = hbase; hmask; i++, hmask >>= 1) {
			ಯದಿ (hmask & 1UL)
				sbi_hartmask_set_hartid(i, &tmp_mask);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_mask);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		sbi_hartmask_and(&domain_mask, &domain_mask, &tmp_mask);
		ಯದಿ (sbi_hartmask_weight(&domain_mask) != ಎಣಿಕೆ)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

		sbi_hartmask_and(&target_mask, &target_mask, &tmp_mask);
	}

	/* Send IPIs */
	ಕುರು {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_mask) {
			rc = sbi_ipi_send(scratch, i, event, data);
			ಯದಿ (rc < 0)
				ಗಚ್ಛ done;
			ಯದಿ (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			ಅನ್ಯಥಾ
				sbi_hartmask_clear_hartindex(i, &target_mask);
			rc = 0;
		}
	} ಯಾವತ್ (retry_needed);

done:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	ಪ್ರತ್ಯಾಗಮ rc;
}

ಪೂರ್ಣಾಙ್ಕ sbi_ipi_event_create(ಧ್ರುವ ಸಂರಚನಾ sbi_ipi_event_ops *ops)
{
	ಪೂರ್ಣಾಙ್ಕ i, ret = SBI_ENOSPC;

	ಯದಿ (!ops || !ops->process)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಪ್ರತಿ (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		ಯದಿ (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			ಭಙ್ಗ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ sbi_ipi_event_destroy(u32 event)
{
	ಯದಿ (SBI_IPI_EVENT_MAX <= event)
		ಪ್ರತ್ಯಾಗಮ;

	ipi_ops_array[event] = ರಿಕ್ತ;
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_ipi_process_smode(ಸಂರಚನಾ sbi_scratch *scratch)
{
	csr_set(CSR_MIP, MIP_SSIP);
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

ಸ್ಥಿರ u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

ಪೂರ್ಣಾಙ್ಕ sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	ಪ್ರತ್ಯಾಗಮ sbi_ipi_send_many(hmask, hbase, ipi_smode_event, ರಿಕ್ತ);
}

ಶೂನ್ಯ sbi_ipi_clear_smode(ಶೂನ್ಯ)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ipi_update_halt(ಸಂರಚನಾ sbi_scratch *scratch,
			       ಸಂರಚನಾ sbi_scratch *remote_scratch,
			       u32 remote_hartindex, ಶೂನ್ಯ *data)
{
	/* Never send a halt IPI to the local hart. */
	ಪ್ರತ್ಯಾಗಮ scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_ipi_process_halt(ಸಂರಚನಾ sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

ಸ್ಥಿರ u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

ಪೂರ್ಣಾಙ್ಕ sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	ಪ್ರತ್ಯಾಗಮ sbi_ipi_send_many(hmask, hbase, ipi_halt_event, ರಿಕ್ತ);
}

ಶೂನ್ಯ sbi_ipi_process(ಶೂನ್ಯ)
{
	unsigned long ipi_type;
	unsigned ಪೂರ್ಣಾಙ್ಕ ipi_event;
	ಧ್ರುವ ಸಂರಚನಾ sbi_ipi_event_ops *ipi_ops;
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಸಂರಚನಾ sbi_ipi_data *ipi_data =
			sbi_scratch_offset_ptr(scratch, ipi_data_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_data->ipi_type, 0);
	ipi_event = 0;
	ಯಾವತ್ (ipi_type) {
		ಯದಿ (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			ಯದಿ (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

ಪೂರ್ಣಾಙ್ಕ sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	ಸಂರಚನಾ sbi_ipi_device_ಗಂಟು *entry;

	ಯದಿ (!ipi_dev || !ipi_dev->ipi_send)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

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

	ಯದಿ (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_ಗಂಟು_list, head) {
			ಯದಿ (entry->dev->ipi_send)
				entry->dev->ipi_send(hartindex);
		}
	} ಅನ್ಯಥಾ {
		ipi_dev->ipi_send(hartindex);
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ sbi_ipi_raw_clear(bool all_devices)
{
	ಸಂರಚನಾ sbi_ipi_device_ಗಂಟು *entry;

	ಯದಿ (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_ಗಂಟು_list, head) {
			ಯದಿ (entry->dev->ipi_clear)
				entry->dev->ipi_clear();
		}
	} ಅನ್ಯಥಾ {
		ಯದಿ (ipi_dev && ipi_dev->ipi_clear)
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

ಧ್ರುವ ಸಂರಚನಾ sbi_ipi_device *sbi_ipi_get_device(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ ipi_dev;
}

ಶೂನ್ಯ sbi_ipi_add_device(ಧ್ರುವ ಸಂರಚನಾ sbi_ipi_device *dev)
{
	ಸಂರಚನಾ sbi_ipi_device_ಗಂಟು *entry;

	ಯದಿ (!dev)
		ಪ್ರತ್ಯಾಗಮ;

	entry = sbi_zalloc(ಪರಿಮಾಣ(*entry));
	ಯದಿ (!entry)
		ಪ್ರತ್ಯಾಗಮ;
	SBI_INIT_LIST_HEAD(&entry->head);
	entry->dev = dev;
	sbi_list_add_tail(&entry->head, &ipi_dev_ಗಂಟು_list);

	ಯದಿ (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

ಪೂರ್ಣಾಙ್ಕ sbi_ipi_init(ಸಂರಚನಾ sbi_scratch *scratch, bool cold_boot)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ sbi_ipi_data *ipi_data;

	ಯದಿ (cold_boot) {
		ipi_data_off = sbi_scratch_alloc_offset(ಪರಿಮಾಣ(*ipi_data));
		ಯದಿ (!ipi_data_off)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		ಯದಿ (ret < 0)
			ಪ್ರತ್ಯಾಗಮ ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		ಯದಿ (ret < 0)
			ಪ್ರತ್ಯಾಗಮ ret;
		ipi_halt_event = ret;
	} ಅನ್ಯಥಾ {
		ಯದಿ (!ipi_data_off)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
		ಯದಿ (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOSPC;
	}

	ipi_data = sbi_scratch_offset_ptr(scratch, ipi_data_off);
	ipi_data->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_set(CSR_MIE, MIP_MSIP);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ sbi_ipi_exit(ಸಂರಚನಾ sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
