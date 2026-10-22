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

ರಚನೆ sbi_ipi_ದತ್ತಾಂಶ {
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ipi_type;
};

_Static_assert(
	8 * ಗಾತ್ರ(((ರಚನೆ sbi_ipi_ದತ್ತಾಂಶ*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

ರಚನೆ sbi_ipi_device_ಗಂಟು {
	ರಚನೆ sbi_dlist ತಲೆ;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_ipi_device *dev;
};

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ipi_ದತ್ತಾಂಶ_off;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_ipi_device *ipi_dev = ಖಾಲಿ;
ಸ್ಥಿರ SBI_LIST_ತಲೆ(ipi_dev_ಗಂಟು_list);
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ipi_send(ರಚನೆ sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
	ಪೂರ್ಣಾಂಕ ret = 0;
	ರಚನೆ sbi_scratch *remote_scratch = ಖಾಲಿ;
	ರಚನೆ sbi_ipi_ದತ್ತಾಂಶ *ipi_ದತ್ತಾಂಶ;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_ipi_event_ops *ipi_ops;

	ಆದರೆ ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_ಗಮ್ಯ_scratch(remote_hartindex);
	ಆದರೆ (!remote_scratch)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ipi_ದತ್ತಾಂಶ = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(remote_scratch, ipi_ದತ್ತಾಂಶ_off);

	ಆದರೆ (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, ದತ್ತಾಂಶ);
		ಆದರೆ (ret != SBI_IPI_UPDATE_SUCCESS)
			ಹಿಂತಿರುಗಿ ret;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		ಹಿಂತಿರುಗಿ 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	ಆದರೆ (!__atomic_fetch_or(&ipi_ದತ್ತಾಂಶ->ipi_type,
				ಬಿಟ್(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_ಕಳುಹಿಸಿದ);

	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ipi_sync(ರಚನೆ sbi_scratch *scratch, u32 event)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_ipi_event_ops *ipi_ops;

	ಆದರೆ ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	ಆದರೆ (ipi_ops->sync)
		ipi_ops->sync(scratch);

	ಹಿಂತಿರುಗಿ 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
ಪೂರ್ಣಾಂಕ sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
	ಪೂರ್ಣಾಂಕ rc = 0;
	bool retry_needed;
	ulong i;
	ರಚನೆ sbi_hartmask ಗುರಿ_ಮುಖವಾಡ;
	ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ಆದರೆ (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		ಹಿಂತಿರುಗಿ 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_ಮುಖವಾಡ(dom, &ಗುರಿ_ಮುಖವಾಡ);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಆದರೆ (hbase != -1UL) {
		ರಚನೆ sbi_hartmask tmp_ಮುಖವಾಡ = { 0 };
		ರಚನೆ sbi_hartmask domain_ಮುಖವಾಡ;
		ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = sbi_popcount(hmask);

		ಗಾಗಿ (i = hbase; hmask; i++, hmask >>= 1) {
			ಆದರೆ (hmask & 1UL)
				sbi_hartmask_ಗಣ_hartid(i, &tmp_ಮುಖವಾಡ);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_ಮುಖವಾಡ);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		sbi_hartmask_and(&domain_ಮುಖವಾಡ, &domain_ಮುಖವಾಡ, &tmp_ಮುಖವಾಡ);
		ಆದರೆ (sbi_hartmask_ತೂಕ(&domain_ಮುಖವಾಡ) != ಎಣಿಕೆ_2)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;

		sbi_hartmask_and(&ಗುರಿ_ಮುಖವಾಡ, &ಗುರಿ_ಮುಖವಾಡ, &tmp_ಮುಖವಾಡ);
	}

	/* Send IPIs */
	ಮಾಡು {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &ಗುರಿ_ಮುಖವಾಡ) {
			rc = sbi_ipi_send(scratch, i, event, ದತ್ತಾಂಶ);
			ಆದರೆ (rc < 0)
				ಹೋಗು ಮುಗಿದ;
			ಆದರೆ (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			ಇಲ್ಲದಿದ್ದರೆ
				sbi_hartmask_clear_hartindex(i, &ಗುರಿ_ಮುಖವಾಡ);
			rc = 0;
		}
	} ಆಗ (retry_needed);

ಮುಗಿದ:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	ಹಿಂತಿರುಗಿ rc;
}

ಪೂರ್ಣಾಂಕ sbi_ipi_event_create(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_ipi_event_ops *ops)
{
	ಪೂರ್ಣಾಂಕ i, ret = SBI_ENOSPC;

	ಆದರೆ (!ops || !ops->process)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಗಾಗಿ (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		ಆದರೆ (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			ನಿಲ್ಲಿಸು;
		}
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ sbi_ipi_event_destroy(u32 event)
{
	ಆದರೆ (SBI_IPI_EVENT_MAX <= event)
		ಹಿಂತಿರುಗಿ;

	ipi_ops_array[event] = ಖಾಲಿ;
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_ipi_process_smode(ರಚನೆ sbi_scratch *scratch)
{
	csr_ಗಣ(CSR_MIP, MIP_SSIP);
}

ಸ್ಥಿರ ರಚನೆ sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

ಸ್ಥಿರ u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

ಪೂರ್ಣಾಂಕ sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	ಹಿಂತಿರುಗಿ sbi_ipi_send_many(hmask, hbase, ipi_smode_event, ಖಾಲಿ);
}

ಶೂನ್ಯ sbi_ipi_clear_smode(ಶೂನ್ಯ)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ipi_update_halt(ರಚನೆ sbi_scratch *scratch,
			       ರಚನೆ sbi_scratch *remote_scratch,
			       u32 remote_hartindex, ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
	/* Never send a halt IPI to the local hart. */
	ಹಿಂತಿರುಗಿ scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_ipi_process_halt(ರಚನೆ sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

ಸ್ಥಿರ ರಚನೆ sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

ಸ್ಥಿರ u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

ಪೂರ್ಣಾಂಕ sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	ಹಿಂತಿರುಗಿ sbi_ipi_send_many(hmask, hbase, ipi_halt_event, ಖಾಲಿ);
}

ಶೂನ್ಯ sbi_ipi_process(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ipi_type;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ipi_event;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_ipi_event_ops *ipi_ops;
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ರಚನೆ sbi_ipi_ದತ್ತಾಂಶ *ipi_ದತ್ತಾಂಶ =
			sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, ipi_ದತ್ತಾಂಶ_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_ದತ್ತಾಂಶ->ipi_type, 0);
	ipi_event = 0;
	ಆಗ (ipi_type) {
		ಆದರೆ (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			ಆದರೆ (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

ಪೂರ್ಣಾಂಕ sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	ರಚನೆ sbi_ipi_device_ಗಂಟು *ನಮೂದು_2;

	ಆದರೆ (!ipi_dev || !ipi_dev->ipi_send)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

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

	ಆದರೆ (all_devices) {
		sbi_list_for_each_ನಮೂದು(ನಮೂದು_2, &ipi_dev_ಗಂಟು_list, ತಲೆ) {
			ಆದರೆ (ನಮೂದು_2->dev->ipi_send)
				ನಮೂದು_2->dev->ipi_send(hartindex);
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		ipi_dev->ipi_send(hartindex);
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ sbi_ipi_raw_clear(bool all_devices)
{
	ರಚನೆ sbi_ipi_device_ಗಂಟು *ನಮೂದು_2;

	ಆದರೆ (all_devices) {
		sbi_list_for_each_ನಮೂದು(ನಮೂದು_2, &ipi_dev_ಗಂಟು_list, ತಲೆ) {
			ಆದರೆ (ನಮೂದು_2->dev->ipi_clear)
				ನಮೂದು_2->dev->ipi_clear();
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (ipi_dev && ipi_dev->ipi_clear)
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

ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_ipi_device *sbi_ipi_get_device(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ ipi_dev;
}

ಶೂನ್ಯ sbi_ipi_ಕೂಡಿಸು_device(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_ipi_device *dev)
{
	ರಚನೆ sbi_ipi_device_ಗಂಟು *ನಮೂದು_2;

	ಆದರೆ (!dev)
		ಹಿಂತಿರುಗಿ;

	ನಮೂದು_2 = sbi_zalloc(ಗಾತ್ರ(*ನಮೂದು_2));
	ಆದರೆ (!ನಮೂದು_2)
		ಹಿಂತಿರುಗಿ;
	SBI_INIT_LIST_ತಲೆ(&ನಮೂದು_2->ತಲೆ);
	ನಮೂದು_2->dev = dev;
	sbi_list_ಕೂಡಿಸು_ಬಾಲ(&ನಮೂದು_2->ತಲೆ, &ipi_dev_ಗಂಟು_list);

	ಆದರೆ (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

ಪೂರ್ಣಾಂಕ sbi_ipi_init(ರಚನೆ sbi_scratch *scratch, bool cold_boot)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ sbi_ipi_ದತ್ತಾಂಶ *ipi_ದತ್ತಾಂಶ;

	ಆದರೆ (cold_boot) {
		ipi_ದತ್ತಾಂಶ_off = sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(ಗಾತ್ರ(*ipi_ದತ್ತಾಂಶ));
		ಆದರೆ (!ipi_ದತ್ತಾಂಶ_off)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		ಆದರೆ (ret < 0)
			ಹಿಂತಿರುಗಿ ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		ಆದರೆ (ret < 0)
			ಹಿಂತಿರುಗಿ ret;
		ipi_halt_event = ret;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (!ipi_ದತ್ತಾಂಶ_off)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
		ಆದರೆ (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			ಹಿಂತಿರುಗಿ SBI_ENOSPC;
	}

	ipi_ದತ್ತಾಂಶ = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, ipi_ದತ್ತಾಂಶ_off);
	ipi_ದತ್ತಾಂಶ->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_ಗಣ(CSR_MIE, MIP_MSIP);

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ sbi_ipi_exit(ರಚನೆ sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
