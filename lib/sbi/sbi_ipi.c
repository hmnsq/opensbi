/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_atomic.h>
#अन्तर्भाव <sbi/riscv_barrier.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_hsm.h>
#अन्तर्भाव <sbi/sbi_init.h>
#अन्तर्भाव <sbi/sbi_ipi.h>
#अन्तर्भाव <sbi/sbi_list.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_pmu.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_tlb.h>

संरचना sbi_ipi_दत्त {
	अचिह्नित दीर्घ ipi_type;
};

_Static_assert(
	8 * परिमाण(((संरचना sbi_ipi_दत्त*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

संरचना sbi_ipi_device_ग्रन्थिः {
	संरचना sbi_dlist शीर्ष;
	ध्रुव संरचना sbi_ipi_device *dev;
};

स्थिर अचिह्नित दीर्घ ipi_दत्त_off;
स्थिर ध्रुव संरचना sbi_ipi_device *ipi_dev = रिक्त;
स्थिर SBI_LIST_शीर्ष(ipi_dev_ग्रन्थिः_list);
स्थिर ध्रुव संरचना sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

स्थिर पूर्णाङ्क sbi_ipi_send(संरचना sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, शून्य *दत्त)
{
	पूर्णाङ्क ret = 0;
	संरचना sbi_scratch *remote_scratch = रिक्त;
	संरचना sbi_ipi_दत्त *ipi_दत्त;
	ध्रुव संरचना sbi_ipi_event_ops *ipi_ops;

	यदि ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		प्रत्यागम SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_गन्तव्यम्_scratch(remote_hartindex);
	यदि (!remote_scratch)
		प्रत्यागम SBI_EINVAL;

	ipi_दत्त = sbi_scratch_विस्थापनम्_ptr(remote_scratch, ipi_दत्त_off);

	यदि (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, दत्त);
		यदि (ret != SBI_IPI_UPDATE_SUCCESS)
			प्रत्यागम ret;
	} अन्यथा यदि (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		प्रत्यागम 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	यदि (!__atomic_fetch_or(&ipi_दत्त->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_प्रेषितम्);

	प्रत्यागम ret;
}

स्थिर पूर्णाङ्क sbi_ipi_sync(संरचना sbi_scratch *scratch, u32 event)
{
	ध्रुव संरचना sbi_ipi_event_ops *ipi_ops;

	यदि ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		प्रत्यागम SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	यदि (ipi_ops->sync)
		ipi_ops->sync(scratch);

	प्रत्यागम 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
पूर्णाङ्क sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, शून्य *दत्त)
{
	पूर्णाङ्क rc = 0;
	bool retry_needed;
	ulong i;
	संरचना sbi_hartmask target_आवरणम्;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	यदि (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		प्रत्यागम 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_आवरणम्(dom, &target_आवरणम्);
	यदि (rc)
		प्रत्यागम rc;

	यदि (hbase != -1UL) {
		संरचना sbi_hartmask tmp_आवरणम् = { 0 };
		संरचना sbi_hartmask domain_आवरणम्;
		पूर्णाङ्क गणनम् = sbi_popcount(hmask);

		प्रति (i = hbase; hmask; i++, hmask >>= 1) {
			यदि (hmask & 1UL)
				sbi_hartmask_समुच्चयः_hartid(i, &tmp_आवरणम्);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_आवरणम्);
		यदि (rc)
			प्रत्यागम rc;

		sbi_hartmask_and(&domain_आवरणम्, &domain_आवरणम्, &tmp_आवरणम्);
		यदि (sbi_hartmask_weight(&domain_आवरणम्) != गणनम्)
			प्रत्यागम SBI_EINVAL;

		sbi_hartmask_and(&target_आवरणम्, &target_आवरणम्, &tmp_आवरणम्);
	}

	/* Send IPIs */
	कुरु {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_आवरणम्) {
			rc = sbi_ipi_send(scratch, i, event, दत्त);
			यदि (rc < 0)
				गच्छ समाप्तम्;
			यदि (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			अन्यथा
				sbi_hartmask_clear_hartindex(i, &target_आवरणम्);
			rc = 0;
		}
	} यावत् (retry_needed);

समाप्तम्:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	प्रत्यागम rc;
}

पूर्णाङ्क sbi_ipi_event_create(ध्रुव संरचना sbi_ipi_event_ops *ops)
{
	पूर्णाङ्क i, ret = SBI_ENOSPC;

	यदि (!ops || !ops->process)
		प्रत्यागम SBI_EINVAL;

	प्रति (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		यदि (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			भङ्ग;
		}
	}

	प्रत्यागम ret;
}

शून्य sbi_ipi_event_destroy(u32 event)
{
	यदि (SBI_IPI_EVENT_MAX <= event)
		प्रत्यागम;

	ipi_ops_array[event] = रिक्त;
}

स्थिर शून्य sbi_ipi_process_smode(संरचना sbi_scratch *scratch)
{
	csr_समुच्चयः(CSR_MIP, MIP_SSIP);
}

स्थिर संरचना sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

स्थिर u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

पूर्णाङ्क sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	प्रत्यागम sbi_ipi_send_many(hmask, hbase, ipi_smode_event, रिक्त);
}

शून्य sbi_ipi_clear_smode(शून्य)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

स्थिर पूर्णाङ्क sbi_ipi_update_halt(संरचना sbi_scratch *scratch,
			       संरचना sbi_scratch *remote_scratch,
			       u32 remote_hartindex, शून्य *दत्त)
{
	/* Never send a halt IPI to the local hart. */
	प्रत्यागम scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

स्थिर शून्य sbi_ipi_process_halt(संरचना sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

स्थिर संरचना sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

स्थिर u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

पूर्णाङ्क sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	प्रत्यागम sbi_ipi_send_many(hmask, hbase, ipi_halt_event, रिक्त);
}

शून्य sbi_ipi_process(शून्य)
{
	अचिह्नित दीर्घ ipi_type;
	अचिह्नित पूर्णाङ्क ipi_event;
	ध्रुव संरचना sbi_ipi_event_ops *ipi_ops;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	संरचना sbi_ipi_दत्त *ipi_दत्त =
			sbi_scratch_विस्थापनम्_ptr(scratch, ipi_दत्त_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_दत्त->ipi_type, 0);
	ipi_event = 0;
	यावत् (ipi_type) {
		यदि (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			यदि (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

पूर्णाङ्क sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	संरचना sbi_ipi_device_ग्रन्थिः *प्रविष्टिः_2;

	यदि (!ipi_dev || !ipi_dev->ipi_send)
		प्रत्यागम SBI_EINVAL;

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

	यदि (all_devices) {
		sbi_list_for_each_प्रविष्टिः(प्रविष्टिः_2, &ipi_dev_ग्रन्थिः_list, शीर्ष) {
			यदि (प्रविष्टिः_2->dev->ipi_send)
				प्रविष्टिः_2->dev->ipi_send(hartindex);
		}
	} अन्यथा {
		ipi_dev->ipi_send(hartindex);
	}

	प्रत्यागम 0;
}

शून्य sbi_ipi_raw_clear(bool all_devices)
{
	संरचना sbi_ipi_device_ग्रन्थिः *प्रविष्टिः_2;

	यदि (all_devices) {
		sbi_list_for_each_प्रविष्टिः(प्रविष्टिः_2, &ipi_dev_ग्रन्थिः_list, शीर्ष) {
			यदि (प्रविष्टिः_2->dev->ipi_clear)
				प्रविष्टिः_2->dev->ipi_clear();
		}
	} अन्यथा {
		यदि (ipi_dev && ipi_dev->ipi_clear)
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

ध्रुव संरचना sbi_ipi_device *sbi_ipi_get_device(शून्य)
{
	प्रत्यागम ipi_dev;
}

शून्य sbi_ipi_सङ्कलय_device(ध्रुव संरचना sbi_ipi_device *dev)
{
	संरचना sbi_ipi_device_ग्रन्थिः *प्रविष्टिः_2;

	यदि (!dev)
		प्रत्यागम;

	प्रविष्टिः_2 = sbi_zalloc(परिमाण(*प्रविष्टिः_2));
	यदि (!प्रविष्टिः_2)
		प्रत्यागम;
	SBI_INIT_LIST_शीर्ष(&प्रविष्टिः_2->शीर्ष);
	प्रविष्टिः_2->dev = dev;
	sbi_list_सङ्कलय_पुच्छ(&प्रविष्टिः_2->शीर्ष, &ipi_dev_ग्रन्थिः_list);

	यदि (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

पूर्णाङ्क sbi_ipi_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	पूर्णाङ्क ret;
	संरचना sbi_ipi_दत्त *ipi_दत्त;

	यदि (cold_boot) {
		ipi_दत्त_off = sbi_scratch_alloc_विस्थापनम्(परिमाण(*ipi_दत्त));
		यदि (!ipi_दत्त_off)
			प्रत्यागम SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		यदि (ret < 0)
			प्रत्यागम ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		यदि (ret < 0)
			प्रत्यागम ret;
		ipi_halt_event = ret;
	} अन्यथा {
		यदि (!ipi_दत्त_off)
			प्रत्यागम SBI_ENOMEM;
		यदि (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			प्रत्यागम SBI_ENOSPC;
	}

	ipi_दत्त = sbi_scratch_विस्थापनम्_ptr(scratch, ipi_दत्त_off);
	ipi_दत्त->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_समुच्चयः(CSR_MIE, MIP_MSIP);

	प्रत्यागम 0;
}

शून्य sbi_ipi_exit(संरचना sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
