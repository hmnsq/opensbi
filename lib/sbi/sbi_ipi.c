/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_atomic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_barrier.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_init.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_list.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_tlb.h>

ਬਣਤਰ sbi_ipi_ਅੰਕੜੇ {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ipi_type;
};

_Static_assert(
	8 * ਆਕਾਰ(((ਬਣਤਰ sbi_ipi_ਅੰਕੜੇ*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

ਬਣਤਰ sbi_ipi_device_ਗੰਢ {
	ਬਣਤਰ sbi_dlist ਸਿਰ;
	ਅਚਲ ਬਣਤਰ sbi_ipi_device *dev;
};

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ipi_ਅੰਕੜੇ_off;
ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_ipi_device *ipi_dev = ਨਿਰਾ;
ਸਥਿਰ SBI_LIST_ਸਿਰ(ipi_dev_ਗੰਢ_list);
ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

ਸਥਿਰ ਪੂਰਨ sbi_ipi_send(ਬਣਤਰ sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, ਖਾਲੀ *ਅੰਕੜੇ)
{
	ਪੂਰਨ ret = 0;
	ਬਣਤਰ sbi_scratch *remote_scratch = ਨਿਰਾ;
	ਬਣਤਰ sbi_ipi_ਅੰਕੜੇ *ipi_ਅੰਕੜੇ;
	ਅਚਲ ਬਣਤਰ sbi_ipi_event_ops *ipi_ops;

	ਜੇ ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		ਵਾਪਸ SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(remote_hartindex);
	ਜੇ (!remote_scratch)
		ਵਾਪਸ SBI_EINVAL;

	ipi_ਅੰਕੜੇ = sbi_scratch_ਵਿੱਥ_ptr(remote_scratch, ipi_ਅੰਕੜੇ_off);

	ਜੇ (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, ਅੰਕੜੇ);
		ਜੇ (ret != SBI_IPI_UPDATE_SUCCESS)
			ਵਾਪਸ ret;
	} ਨਹੀਂਤਾਂ ਜੇ (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		ਵਾਪਸ 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	ਜੇ (!__atomic_fetch_or(&ipi_ਅੰਕੜੇ->ipi_type,
				ਦੋਆਧਾਰੀ_ਅੰਕ(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_ਭੇਜਿਆ);

	ਵਾਪਸ ret;
}

ਸਥਿਰ ਪੂਰਨ sbi_ipi_sync(ਬਣਤਰ sbi_scratch *scratch, u32 event)
{
	ਅਚਲ ਬਣਤਰ sbi_ipi_event_ops *ipi_ops;

	ਜੇ ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		ਵਾਪਸ SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	ਜੇ (ipi_ops->sync)
		ipi_ops->sync(scratch);

	ਵਾਪਸ 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
ਪੂਰਨ sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, ਖਾਲੀ *ਅੰਕੜੇ)
{
	ਪੂਰਨ rc = 0;
	bool retry_needed;
	ulong i;
	ਬਣਤਰ sbi_hartmask ਨਿਸ਼ਾਨਾ_ਨਕਾਬ;
	ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ਜੇ (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		ਵਾਪਸ 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_ਨਕਾਬ(dom, &ਨਿਸ਼ਾਨਾ_ਨਕਾਬ);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਜੇ (hbase != -1UL) {
		ਬਣਤਰ sbi_hartmask tmp_ਨਕਾਬ = { 0 };
		ਬਣਤਰ sbi_hartmask domain_ਨਕਾਬ;
		ਪੂਰਨ ਗਿਣਤੀ_2 = sbi_popcount(hmask);

		ਲਈ (i = hbase; hmask; i++, hmask >>= 1) {
			ਜੇ (hmask & 1UL)
				sbi_hartmask_ਸੈੱਟ_hartid(i, &tmp_ਨਕਾਬ);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_ਨਕਾਬ);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		sbi_hartmask_and(&domain_ਨਕਾਬ, &domain_ਨਕਾਬ, &tmp_ਨਕਾਬ);
		ਜੇ (sbi_hartmask_ਭਾਰ(&domain_ਨਕਾਬ) != ਗਿਣਤੀ_2)
			ਵਾਪਸ SBI_EINVAL;

		sbi_hartmask_and(&ਨਿਸ਼ਾਨਾ_ਨਕਾਬ, &ਨਿਸ਼ਾਨਾ_ਨਕਾਬ, &tmp_ਨਕਾਬ);
	}

	/* Send IPIs */
	ਕਰੋ {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &ਨਿਸ਼ਾਨਾ_ਨਕਾਬ) {
			rc = sbi_ipi_send(scratch, i, event, ਅੰਕੜੇ);
			ਜੇ (rc < 0)
				ਜਾਓ ਪੂਰਾ;
			ਜੇ (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			ਨਹੀਂਤਾਂ
				sbi_hartmask_clear_hartindex(i, &ਨਿਸ਼ਾਨਾ_ਨਕਾਬ);
			rc = 0;
		}
	} ਜਦੋਂ (retry_needed);

ਪੂਰਾ:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	ਵਾਪਸ rc;
}

ਪੂਰਨ sbi_ipi_event_create(ਅਚਲ ਬਣਤਰ sbi_ipi_event_ops *ops)
{
	ਪੂਰਨ i, ret = SBI_ENOSPC;

	ਜੇ (!ops || !ops->process)
		ਵਾਪਸ SBI_EINVAL;

	ਲਈ (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		ਜੇ (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			ਰੋਕੋ;
		}
	}

	ਵਾਪਸ ret;
}

ਖਾਲੀ sbi_ipi_event_destroy(u32 event)
{
	ਜੇ (SBI_IPI_EVENT_MAX <= event)
		ਵਾਪਸ;

	ipi_ops_array[event] = ਨਿਰਾ;
}

ਸਥਿਰ ਖਾਲੀ sbi_ipi_process_smode(ਬਣਤਰ sbi_scratch *scratch)
{
	csr_ਸੈੱਟ(CSR_MIP, MIP_SSIP);
}

ਸਥਿਰ ਬਣਤਰ sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

ਸਥਿਰ u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

ਪੂਰਨ sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	ਵਾਪਸ sbi_ipi_send_many(hmask, hbase, ipi_smode_event, ਨਿਰਾ);
}

ਖਾਲੀ sbi_ipi_clear_smode(ਖਾਲੀ)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

ਸਥਿਰ ਪੂਰਨ sbi_ipi_update_halt(ਬਣਤਰ sbi_scratch *scratch,
			       ਬਣਤਰ sbi_scratch *remote_scratch,
			       u32 remote_hartindex, ਖਾਲੀ *ਅੰਕੜੇ)
{
	/* Never send a halt IPI to the local hart. */
	ਵਾਪਸ scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

ਸਥਿਰ ਖਾਲੀ sbi_ipi_process_halt(ਬਣਤਰ sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

ਸਥਿਰ ਬਣਤਰ sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

ਸਥਿਰ u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

ਪੂਰਨ sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	ਵਾਪਸ sbi_ipi_send_many(hmask, hbase, ipi_halt_event, ਨਿਰਾ);
}

ਖਾਲੀ sbi_ipi_process(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ipi_type;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ipi_event;
	ਅਚਲ ਬਣਤਰ sbi_ipi_event_ops *ipi_ops;
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਬਣਤਰ sbi_ipi_ਅੰਕੜੇ *ipi_ਅੰਕੜੇ =
			sbi_scratch_ਵਿੱਥ_ptr(scratch, ipi_ਅੰਕੜੇ_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_ਅੰਕੜੇ->ipi_type, 0);
	ipi_event = 0;
	ਜਦੋਂ (ipi_type) {
		ਜੇ (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			ਜੇ (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

ਪੂਰਨ sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	ਬਣਤਰ sbi_ipi_device_ਗੰਢ *ਇੰਦਰਾਜ_2;

	ਜੇ (!ipi_dev || !ipi_dev->ipi_send)
		ਵਾਪਸ SBI_EINVAL;

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

	ਜੇ (all_devices) {
		sbi_list_for_each_ਇੰਦਰਾਜ(ਇੰਦਰਾਜ_2, &ipi_dev_ਗੰਢ_list, ਸਿਰ) {
			ਜੇ (ਇੰਦਰਾਜ_2->dev->ipi_send)
				ਇੰਦਰਾਜ_2->dev->ipi_send(hartindex);
		}
	} ਨਹੀਂਤਾਂ {
		ipi_dev->ipi_send(hartindex);
	}

	ਵਾਪਸ 0;
}

ਖਾਲੀ sbi_ipi_raw_clear(bool all_devices)
{
	ਬਣਤਰ sbi_ipi_device_ਗੰਢ *ਇੰਦਰਾਜ_2;

	ਜੇ (all_devices) {
		sbi_list_for_each_ਇੰਦਰਾਜ(ਇੰਦਰਾਜ_2, &ipi_dev_ਗੰਢ_list, ਸਿਰ) {
			ਜੇ (ਇੰਦਰਾਜ_2->dev->ipi_clear)
				ਇੰਦਰਾਜ_2->dev->ipi_clear();
		}
	} ਨਹੀਂਤਾਂ {
		ਜੇ (ipi_dev && ipi_dev->ipi_clear)
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

ਅਚਲ ਬਣਤਰ sbi_ipi_device *sbi_ipi_get_device(ਖਾਲੀ)
{
	ਵਾਪਸ ipi_dev;
}

ਖਾਲੀ sbi_ipi_ਜੋੜੋ_device(ਅਚਲ ਬਣਤਰ sbi_ipi_device *dev)
{
	ਬਣਤਰ sbi_ipi_device_ਗੰਢ *ਇੰਦਰਾਜ_2;

	ਜੇ (!dev)
		ਵਾਪਸ;

	ਇੰਦਰਾਜ_2 = sbi_zalloc(ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
	ਜੇ (!ਇੰਦਰਾਜ_2)
		ਵਾਪਸ;
	SBI_INIT_LIST_ਸਿਰ(&ਇੰਦਰਾਜ_2->ਸਿਰ);
	ਇੰਦਰਾਜ_2->dev = dev;
	sbi_list_ਜੋੜੋ_ਪੂਛ(&ਇੰਦਰਾਜ_2->ਸਿਰ, &ipi_dev_ਗੰਢ_list);

	ਜੇ (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

ਪੂਰਨ sbi_ipi_init(ਬਣਤਰ sbi_scratch *scratch, bool cold_boot)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ sbi_ipi_ਅੰਕੜੇ *ipi_ਅੰਕੜੇ;

	ਜੇ (cold_boot) {
		ipi_ਅੰਕੜੇ_off = sbi_scratch_alloc_ਵਿੱਥ(ਆਕਾਰ(*ipi_ਅੰਕੜੇ));
		ਜੇ (!ipi_ਅੰਕੜੇ_off)
			ਵਾਪਸ SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		ਜੇ (ret < 0)
			ਵਾਪਸ ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		ਜੇ (ret < 0)
			ਵਾਪਸ ret;
		ipi_halt_event = ret;
	} ਨਹੀਂਤਾਂ {
		ਜੇ (!ipi_ਅੰਕੜੇ_off)
			ਵਾਪਸ SBI_ENOMEM;
		ਜੇ (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			ਵਾਪਸ SBI_ENOSPC;
	}

	ipi_ਅੰਕੜੇ = sbi_scratch_ਵਿੱਥ_ptr(scratch, ipi_ਅੰਕੜੇ_off);
	ipi_ਅੰਕੜੇ->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_ਸੈੱਟ(CSR_MIE, MIP_MSIP);

	ਵਾਪਸ 0;
}

ਖਾਲੀ sbi_ipi_exit(ਬਣਤਰ sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
