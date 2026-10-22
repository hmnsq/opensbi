/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_atomic.h>
#అన్తర్భావ <sbi/riscv_barrier.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <sbi/sbi_init.h>
#అన్తర్భావ <sbi/sbi_ipi.h>
#అన్తర్భావ <sbi/sbi_list.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_pmu.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_tlb.h>

నిర్మాణం sbi_ipi_దత్తాంశం {
	గుర్తులేని దీర్ఘ ipi_type;
};

_Static_assert(
	8 * పరిమాణం(((నిర్మాణం sbi_ipi_దత్తాంశం*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

నిర్మాణం sbi_ipi_device_కణుపు {
	నిర్మాణం sbi_dlist తల;
	స్థిరరాశి నిర్మాణం sbi_ipi_device *dev;
};

స్థిర గుర్తులేని దీర్ఘ ipi_దత్తాంశం_off;
స్థిర స్థిరరాశి నిర్మాణం sbi_ipi_device *ipi_dev = ఖాళీ;
స్థిర SBI_LIST_తల(ipi_dev_కణుపు_list);
స్థిర స్థిరరాశి నిర్మాణం sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

స్థిర పూర్ణాంకం sbi_ipi_send(నిర్మాణం sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, శూన్య *దత్తాంశం)
{
	పూర్ణాంకం ret = 0;
	నిర్మాణం sbi_scratch *remote_scratch = ఖాళీ;
	నిర్మాణం sbi_ipi_దత్తాంశం *ipi_దత్తాంశం;
	స్థిరరాశి నిర్మాణం sbi_ipi_event_ops *ipi_ops;

	ఒకవేళ ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		తిరిగి SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_గమ్యం_scratch(remote_hartindex);
	ఒకవేళ (!remote_scratch)
		తిరిగి SBI_EINVAL;

	ipi_దత్తాంశం = sbi_scratch_స్థానభ్రంశం_ptr(remote_scratch, ipi_దత్తాంశం_off);

	ఒకవేళ (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, దత్తాంశం);
		ఒకవేళ (ret != SBI_IPI_UPDATE_SUCCESS)
			తిరిగి ret;
	} లేకపోతే ఒకవేళ (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		తిరిగి 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	ఒకవేళ (!__atomic_fetch_or(&ipi_దత్తాంశం->ipi_type,
				బిట్టు(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_పంపిన);

	తిరిగి ret;
}

స్థిర పూర్ణాంకం sbi_ipi_sync(నిర్మాణం sbi_scratch *scratch, u32 event)
{
	స్థిరరాశి నిర్మాణం sbi_ipi_event_ops *ipi_ops;

	ఒకవేళ ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		తిరిగి SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	ఒకవేళ (ipi_ops->sync)
		ipi_ops->sync(scratch);

	తిరిగి 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
పూర్ణాంకం sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, శూన్య *దత్తాంశం)
{
	పూర్ణాంకం rc = 0;
	bool retry_needed;
	ulong i;
	నిర్మాణం sbi_hartmask లక్ష్యం_ముసుగు;
	నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ఒకవేళ (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		తిరిగి 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_ముసుగు(dom, &లక్ష్యం_ముసుగు);
	ఒకవేళ (rc)
		తిరిగి rc;

	ఒకవేళ (hbase != -1UL) {
		నిర్మాణం sbi_hartmask tmp_ముసుగు = { 0 };
		నిర్మాణం sbi_hartmask domain_ముసుగు;
		పూర్ణాంకం లెక్క = sbi_popcount(hmask);

		కొరకు (i = hbase; hmask; i++, hmask >>= 1) {
			ఒకవేళ (hmask & 1UL)
				sbi_hartmask_సమితి_hartid(i, &tmp_ముసుగు);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_ముసుగు);
		ఒకవేళ (rc)
			తిరిగి rc;

		sbi_hartmask_and(&domain_ముసుగు, &domain_ముసుగు, &tmp_ముసుగు);
		ఒకవేళ (sbi_hartmask_బరువు(&domain_ముసుగు) != లెక్క)
			తిరిగి SBI_EINVAL;

		sbi_hartmask_and(&లక్ష్యం_ముసుగు, &లక్ష్యం_ముసుగు, &tmp_ముసుగు);
	}

	/* Send IPIs */
	చేయి {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &లక్ష్యం_ముసుగు) {
			rc = sbi_ipi_send(scratch, i, event, దత్తాంశం);
			ఒకవేళ (rc < 0)
				వెళ్ళు పూర్తి;
			ఒకవేళ (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			లేకపోతే
				sbi_hartmask_clear_hartindex(i, &లక్ష్యం_ముసుగు);
			rc = 0;
		}
	} అయితే (retry_needed);

పూర్తి:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	తిరిగి rc;
}

పూర్ణాంకం sbi_ipi_event_create(స్థిరరాశి నిర్మాణం sbi_ipi_event_ops *ops)
{
	పూర్ణాంకం i, ret = SBI_ENOSPC;

	ఒకవేళ (!ops || !ops->process)
		తిరిగి SBI_EINVAL;

	కొరకు (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		ఒకవేళ (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			ఆపు;
		}
	}

	తిరిగి ret;
}

శూన్య sbi_ipi_event_destroy(u32 event)
{
	ఒకవేళ (SBI_IPI_EVENT_MAX <= event)
		తిరిగి;

	ipi_ops_array[event] = ఖాళీ;
}

స్థిర శూన్య sbi_ipi_process_smode(నిర్మాణం sbi_scratch *scratch)
{
	csr_సమితి(CSR_MIP, MIP_SSIP);
}

స్థిర నిర్మాణం sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

స్థిర u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

పూర్ణాంకం sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	తిరిగి sbi_ipi_send_many(hmask, hbase, ipi_smode_event, ఖాళీ);
}

శూన్య sbi_ipi_clear_smode(శూన్య)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

స్థిర పూర్ణాంకం sbi_ipi_update_halt(నిర్మాణం sbi_scratch *scratch,
			       నిర్మాణం sbi_scratch *remote_scratch,
			       u32 remote_hartindex, శూన్య *దత్తాంశం)
{
	/* Never send a halt IPI to the local hart. */
	తిరిగి scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

స్థిర శూన్య sbi_ipi_process_halt(నిర్మాణం sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

స్థిర నిర్మాణం sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

స్థిర u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

పూర్ణాంకం sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	తిరిగి sbi_ipi_send_many(hmask, hbase, ipi_halt_event, ఖాళీ);
}

శూన్య sbi_ipi_process(శూన్య)
{
	గుర్తులేని దీర్ఘ ipi_type;
	గుర్తులేని పూర్ణాంకం ipi_event;
	స్థిరరాశి నిర్మాణం sbi_ipi_event_ops *ipi_ops;
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	నిర్మాణం sbi_ipi_దత్తాంశం *ipi_దత్తాంశం =
			sbi_scratch_స్థానభ్రంశం_ptr(scratch, ipi_దత్తాంశం_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_దత్తాంశం->ipi_type, 0);
	ipi_event = 0;
	అయితే (ipi_type) {
		ఒకవేళ (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			ఒకవేళ (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

పూర్ణాంకం sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	నిర్మాణం sbi_ipi_device_కణుపు *నమోదు_3;

	ఒకవేళ (!ipi_dev || !ipi_dev->ipi_send)
		తిరిగి SBI_EINVAL;

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

	ఒకవేళ (all_devices) {
		sbi_list_for_each_నమోదు(నమోదు_3, &ipi_dev_కణుపు_list, తల) {
			ఒకవేళ (నమోదు_3->dev->ipi_send)
				నమోదు_3->dev->ipi_send(hartindex);
		}
	} లేకపోతే {
		ipi_dev->ipi_send(hartindex);
	}

	తిరిగి 0;
}

శూన్య sbi_ipi_raw_clear(bool all_devices)
{
	నిర్మాణం sbi_ipi_device_కణుపు *నమోదు_3;

	ఒకవేళ (all_devices) {
		sbi_list_for_each_నమోదు(నమోదు_3, &ipi_dev_కణుపు_list, తల) {
			ఒకవేళ (నమోదు_3->dev->ipi_clear)
				నమోదు_3->dev->ipi_clear();
		}
	} లేకపోతే {
		ఒకవేళ (ipi_dev && ipi_dev->ipi_clear)
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

స్థిరరాశి నిర్మాణం sbi_ipi_device *sbi_ipi_get_device(శూన్య)
{
	తిరిగి ipi_dev;
}

శూన్య sbi_ipi_కూడు_device(స్థిరరాశి నిర్మాణం sbi_ipi_device *dev)
{
	నిర్మాణం sbi_ipi_device_కణుపు *నమోదు_3;

	ఒకవేళ (!dev)
		తిరిగి;

	నమోదు_3 = sbi_zalloc(పరిమాణం(*నమోదు_3));
	ఒకవేళ (!నమోదు_3)
		తిరిగి;
	SBI_INIT_LIST_తల(&నమోదు_3->తల);
	నమోదు_3->dev = dev;
	sbi_list_కూడు_తోక(&నమోదు_3->తల, &ipi_dev_కణుపు_list);

	ఒకవేళ (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

పూర్ణాంకం sbi_ipi_init(నిర్మాణం sbi_scratch *scratch, bool cold_boot)
{
	పూర్ణాంకం ret;
	నిర్మాణం sbi_ipi_దత్తాంశం *ipi_దత్తాంశం;

	ఒకవేళ (cold_boot) {
		ipi_దత్తాంశం_off = sbi_scratch_alloc_స్థానభ్రంశం(పరిమాణం(*ipi_దత్తాంశం));
		ఒకవేళ (!ipi_దత్తాంశం_off)
			తిరిగి SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		ఒకవేళ (ret < 0)
			తిరిగి ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		ఒకవేళ (ret < 0)
			తిరిగి ret;
		ipi_halt_event = ret;
	} లేకపోతే {
		ఒకవేళ (!ipi_దత్తాంశం_off)
			తిరిగి SBI_ENOMEM;
		ఒకవేళ (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			తిరిగి SBI_ENOSPC;
	}

	ipi_దత్తాంశం = sbi_scratch_స్థానభ్రంశం_ptr(scratch, ipi_దత్తాంశం_off);
	ipi_దత్తాంశం->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_సమితి(CSR_MIE, MIP_MSIP);

	తిరిగి 0;
}

శూన్య sbi_ipi_exit(నిర్మాణం sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
