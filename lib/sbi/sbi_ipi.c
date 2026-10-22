/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_atomic.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_barrier.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitops.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hsm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_init.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ipi.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_list.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_pmu.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_tlb.h>

ସଂରଚନା sbi_ipi_data {
	unsigned long ipi_type;
};

_Static_assert(
	8 * ଆକାର(((ସଂରଚନା sbi_ipi_data*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

ସଂରଚନା sbi_ipi_device_ଗଣ୍ଠି {
	ସଂରଚନା sbi_dlist head;
	ଅଚଳ ସଂରଚନା sbi_ipi_device *dev;
};

ସ୍ଥିର unsigned long ipi_data_off;
ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_ipi_device *ipi_dev = ଖାଲି;
ସ୍ଥିର SBI_LIST_HEAD(ipi_dev_ଗଣ୍ଠି_list);
ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ipi_send(ସଂରଚନା sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, ଶୂନ୍ୟ *data)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
	ସଂରଚନା sbi_scratch *remote_scratch = ଖାଲି;
	ସଂରଚନା sbi_ipi_data *ipi_data;
	ଅଚଳ ସଂରଚନା sbi_ipi_event_ops *ipi_ops;

	ଯଦି ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		ଫେରନ୍ତୁ SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_to_scratch(remote_hartindex);
	ଯଦି (!remote_scratch)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ipi_data = sbi_scratch_offset_ptr(remote_scratch, ipi_data_off);

	ଯଦି (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, data);
		ଯଦି (ret != SBI_IPI_UPDATE_SUCCESS)
			ଫେରନ୍ତୁ ret;
	} ଅନ୍ୟଥା ଯଦି (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		ଫେରନ୍ତୁ 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	ଯଦି (!__atomic_fetch_or(&ipi_data->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_SENT);

	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ipi_sync(ସଂରଚନା sbi_scratch *scratch, u32 event)
{
	ଅଚଳ ସଂରଚନା sbi_ipi_event_ops *ipi_ops;

	ଯଦି ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		ଫେରନ୍ତୁ SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	ଯଦି (ipi_ops->sync)
		ipi_ops->sync(scratch);

	ଫେରନ୍ତୁ 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, ଶୂନ୍ୟ *data)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
	bool retry_needed;
	ulong i;
	ସଂରଚନା sbi_hartmask target_mask;
	ସଂରଚନା sbi_domain *dom = sbi_domain_thishart_ptr();
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ଯଦି (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		ଫେରନ୍ତୁ 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_mask(dom, &target_mask);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଯଦି (hbase != -1UL) {
		ସଂରଚନା sbi_hartmask tmp_mask = { 0 };
		ସଂରଚନା sbi_hartmask domain_mask;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = sbi_popcount(hmask);

		ପାଇଁ (i = hbase; hmask; i++, hmask >>= 1) {
			ଯଦି (hmask & 1UL)
				sbi_hartmask_set_hartid(i, &tmp_mask);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_mask);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		sbi_hartmask_and(&domain_mask, &domain_mask, &tmp_mask);
		ଯଦି (sbi_hartmask_weight(&domain_mask) != ଗଣନା_2)
			ଫେରନ୍ତୁ SBI_EINVAL;

		sbi_hartmask_and(&target_mask, &target_mask, &tmp_mask);
	}

	/* Send IPIs */
	କର {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_mask) {
			rc = sbi_ipi_send(scratch, i, event, data);
			ଯଦି (rc < 0)
				ଯାଅ done;
			ଯଦି (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			ଅନ୍ୟଥା
				sbi_hartmask_clear_hartindex(i, &target_mask);
			rc = 0;
		}
	} ଯେତେବେଳେ (retry_needed);

done:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	ଫେରନ୍ତୁ rc;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ipi_event_create(ଅଚଳ ସଂରଚନା sbi_ipi_event_ops *ops)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, ret = SBI_ENOSPC;

	ଯଦି (!ops || !ops->process)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ପାଇଁ (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		ଯଦି (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			ଭାଙ୍ଗ;
		}
	}

	ଫେରନ୍ତୁ ret;
}

ଶୂନ୍ୟ sbi_ipi_event_destroy(u32 event)
{
	ଯଦି (SBI_IPI_EVENT_MAX <= event)
		ଫେରନ୍ତୁ;

	ipi_ops_array[event] = ଖାଲି;
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_ipi_process_smode(ସଂରଚନା sbi_scratch *scratch)
{
	csr_set(CSR_MIP, MIP_SSIP);
}

ସ୍ଥିର ସଂରଚନା sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

ସ୍ଥିର u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	ଫେରନ୍ତୁ sbi_ipi_send_many(hmask, hbase, ipi_smode_event, ଖାଲି);
}

ଶୂନ୍ୟ sbi_ipi_clear_smode(ଶୂନ୍ୟ)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ipi_update_halt(ସଂରଚନା sbi_scratch *scratch,
			       ସଂରଚନା sbi_scratch *remote_scratch,
			       u32 remote_hartindex, ଶୂନ୍ୟ *data)
{
	/* Never send a halt IPI to the local hart. */
	ଫେରନ୍ତୁ scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_ipi_process_halt(ସଂରଚନା sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

ସ୍ଥିର ସଂରଚନା sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

ସ୍ଥିର u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	ଫେରନ୍ତୁ sbi_ipi_send_many(hmask, hbase, ipi_halt_event, ଖାଲି);
}

ଶୂନ୍ୟ sbi_ipi_process(ଶୂନ୍ୟ)
{
	unsigned long ipi_type;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ipi_event;
	ଅଚଳ ସଂରଚନା sbi_ipi_event_ops *ipi_ops;
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ସଂରଚନା sbi_ipi_data *ipi_data =
			sbi_scratch_offset_ptr(scratch, ipi_data_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_data->ipi_type, 0);
	ipi_event = 0;
	ଯେତେବେଳେ (ipi_type) {
		ଯଦି (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			ଯଦି (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	ସଂରଚନା sbi_ipi_device_ଗଣ୍ଠି *entry;

	ଯଦି (!ipi_dev || !ipi_dev->ipi_send)
		ଫେରନ୍ତୁ SBI_EINVAL;

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

	ଯଦି (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_ଗଣ୍ଠି_list, head) {
			ଯଦି (entry->dev->ipi_send)
				entry->dev->ipi_send(hartindex);
		}
	} ଅନ୍ୟଥା {
		ipi_dev->ipi_send(hartindex);
	}

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ sbi_ipi_raw_clear(bool all_devices)
{
	ସଂରଚନା sbi_ipi_device_ଗଣ୍ଠି *entry;

	ଯଦି (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_ଗଣ୍ଠି_list, head) {
			ଯଦି (entry->dev->ipi_clear)
				entry->dev->ipi_clear();
		}
	} ଅନ୍ୟଥା {
		ଯଦି (ipi_dev && ipi_dev->ipi_clear)
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

ଅଚଳ ସଂରଚନା sbi_ipi_device *sbi_ipi_get_device(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ ipi_dev;
}

ଶୂନ୍ୟ sbi_ipi_add_device(ଅଚଳ ସଂରଚନା sbi_ipi_device *dev)
{
	ସଂରଚନା sbi_ipi_device_ଗଣ୍ଠି *entry;

	ଯଦି (!dev)
		ଫେରନ୍ତୁ;

	entry = sbi_zalloc(ଆକାର(*entry));
	ଯଦି (!entry)
		ଫେରନ୍ତୁ;
	SBI_INIT_LIST_HEAD(&entry->head);
	entry->dev = dev;
	sbi_list_add_tail(&entry->head, &ipi_dev_ଗଣ୍ଠି_list);

	ଯଦି (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ipi_init(ସଂରଚନା sbi_scratch *scratch, bool cold_boot)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା sbi_ipi_data *ipi_data;

	ଯଦି (cold_boot) {
		ipi_data_off = sbi_scratch_alloc_offset(ଆକାର(*ipi_data));
		ଯଦି (!ipi_data_off)
			ଫେରନ୍ତୁ SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		ଯଦି (ret < 0)
			ଫେରନ୍ତୁ ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		ଯଦି (ret < 0)
			ଫେରନ୍ତୁ ret;
		ipi_halt_event = ret;
	} ଅନ୍ୟଥା {
		ଯଦି (!ipi_data_off)
			ଫେରନ୍ତୁ SBI_ENOMEM;
		ଯଦି (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			ଫେରନ୍ତୁ SBI_ENOSPC;
	}

	ipi_data = sbi_scratch_offset_ptr(scratch, ipi_data_off);
	ipi_data->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_set(CSR_MIE, MIP_MSIP);

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ sbi_ipi_exit(ସଂରଚନା sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
