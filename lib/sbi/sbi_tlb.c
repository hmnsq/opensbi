/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_atomic.h>
#అన్తర్భావ <sbi/riscv_barrier.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_fifo.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_ipi.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_tlb.h>
#అన్తర్భావ <sbi/sbi_hfence.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_pmu.h>

స్థిర గుర్తులేని దీర్ఘ tlb_sync_off;
స్థిర గుర్తులేని దీర్ఘ tlb_fifo_off;
స్థిర గుర్తులేని దీర్ఘ tlb_fifo_mem_off;
స్థిర గుర్తులేని దీర్ఘ tlb_range_flush_హద్దు;

స్థిర శూన్య sbi_tlb_local_fence_i(నిర్మాణం sbi_tlb_info *tinfo);
స్థిర శూన్య sbi_tlb_local_sfence_vma(నిర్మాణం sbi_tlb_info *tinfo);
స్థిర శూన్య sbi_tlb_local_sfence_vma_asid(నిర్మాణం sbi_tlb_info *tinfo);
స్థిర శూన్య sbi_tlb_local_hfence_gvma_vmid(నిర్మాణం sbi_tlb_info *tinfo);
స్థిర శూన్య sbi_tlb_local_hfence_gvma(నిర్మాణం sbi_tlb_info *tinfo);
స్థిర శూన్య sbi_tlb_local_hfence_vvma_asid(నిర్మాణం sbi_tlb_info *tinfo);
స్థిర శూన్య sbi_tlb_local_hfence_vvma(నిర్మాణం sbi_tlb_info *tinfo);

స్థిర నిర్మాణం sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

స్థిర స్థిరరాశి నిర్మాణం sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

స్థిరరాశి నిర్మాణం sbi_tlb_local_operations *sbi_tlb_get_local_operations(శూన్య)
{
	తిరిగి tlb_local_ops;
}

శూన్య sbi_tlb_సమితి_local_operations(
		స్థిరరాశి నిర్మాణం sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

శూన్య __sbi_sfence_vma_all(శూన్య)
{
	__asm__ __volatile("sfence.vma");
}

స్థిర శూన్య sbi_tlb_local_hfence_vvma(నిర్మాణం sbi_tlb_info *tinfo)
{
	గుర్తులేని దీర్ఘ ప్రారంభం = tinfo->ప్రారంభం;
	గుర్తులేని దీర్ఘ కొలత  = tinfo->కొలత;
	గుర్తులేని దీర్ఘ vmid  = tinfo->vmid;
	గుర్తులేని దీర్ఘ i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_మార్పిడి(CSR_HGATP,
			 (vmid << HGATP_VMID_జరుపు) & HGATP_VMID_ముసుగు);

	ఒకవేళ ((ప్రారంభం == 0 && కొలత == 0) || (కొలత == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		వెళ్ళు పూర్తి;
	}

	కొరకు (i = 0; i < కొలత; i += పుట_కొలత) {
		__sbi_hfence_vvma_va(ప్రారంభం+i);
	}

పూర్తి:
	csr_write(CSR_HGATP, hgatp);
}

స్థిర శూన్య sbi_tlb_local_hfence_gvma(నిర్మాణం sbi_tlb_info *tinfo)
{
	గుర్తులేని దీర్ఘ ప్రారంభం = tinfo->ప్రారంభం;
	గుర్తులేని దీర్ఘ కొలత  = tinfo->కొలత;
	గుర్తులేని దీర్ఘ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	ఒకవేళ ((ప్రారంభం == 0 && కొలత == 0) || (కొలత == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		తిరిగి;
	}

	కొరకు (i = 0; i < కొలత; i += పుట_కొలత) {
		__sbi_hfence_gvma_gpa((ప్రారంభం + i) >> 2);
	}
}

స్థిర శూన్య sbi_tlb_local_sfence_vma(నిర్మాణం sbi_tlb_info *tinfo)
{
	గుర్తులేని దీర్ఘ ప్రారంభం = tinfo->ప్రారంభం;
	గుర్తులేని దీర్ఘ కొలత  = tinfo->కొలత;
	గుర్తులేని దీర్ఘ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	ఒకవేళ ((ప్రారంభం == 0 && కొలత == 0) || (కొలత == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		తిరిగి;
	}

	కొరకు (i = 0; i < కొలత; i += పుట_కొలత) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(ప్రారంభం + i)
				     : "memory");
	}
}

స్థిర శూన్య sbi_tlb_local_hfence_vvma_asid(నిర్మాణం sbi_tlb_info *tinfo)
{
	గుర్తులేని దీర్ఘ ప్రారంభం = tinfo->ప్రారంభం;
	గుర్తులేని దీర్ఘ కొలత  = tinfo->కొలత;
	గుర్తులేని దీర్ఘ asid  = tinfo->asid;
	గుర్తులేని దీర్ఘ vmid  = tinfo->vmid;
	గుర్తులేని దీర్ఘ i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_మార్పిడి(CSR_HGATP,
			 (vmid << HGATP_VMID_జరుపు) & HGATP_VMID_ముసుగు);

	ఒకవేళ ((ప్రారంభం == 0 && కొలత == 0) || (కొలత == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		వెళ్ళు పూర్తి;
	}

	కొరకు (i = 0; i < కొలత; i += పుట_కొలత) {
		__sbi_hfence_vvma_asid_va(ప్రారంభం + i, asid);
	}

పూర్తి:
	csr_write(CSR_HGATP, hgatp);
}

స్థిర శూన్య sbi_tlb_local_hfence_gvma_vmid(నిర్మాణం sbi_tlb_info *tinfo)
{
	గుర్తులేని దీర్ఘ ప్రారంభం = tinfo->ప్రారంభం;
	గుర్తులేని దీర్ఘ కొలత  = tinfo->కొలత;
	గుర్తులేని దీర్ఘ vmid  = tinfo->vmid;
	గుర్తులేని దీర్ఘ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	ఒకవేళ ((ప్రారంభం == 0 && కొలత == 0) || (కొలత == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		తిరిగి;
	}

	కొరకు (i = 0; i < కొలత; i += పుట_కొలత) {
		__sbi_hfence_gvma_vmid_gpa((ప్రారంభం + i) >> 2, vmid);
	}
}

స్థిర శూన్య sbi_tlb_local_sfence_vma_asid(నిర్మాణం sbi_tlb_info *tinfo)
{
	గుర్తులేని దీర్ఘ ప్రారంభం = tinfo->ప్రారంభం;
	గుర్తులేని దీర్ఘ కొలత  = tinfo->కొలత;
	గుర్తులేని దీర్ఘ asid  = tinfo->asid;
	గుర్తులేని దీర్ఘ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	ఒకవేళ ((ప్రారంభం == 0 && కొలత == 0) || (కొలత == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		తిరిగి;
	}

	కొరకు (i = 0; i < కొలత; i += పుట_కొలత) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(ప్రారంభం + i), "r"(asid)
				     : "memory");
	}
}

స్థిర శూన్య sbi_tlb_local_fence_i(నిర్మాణం sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

స్థిర శూన్య tlb_నమోదు_local_process(నిర్మాణం sbi_tlb_info *దత్తాంశం)
{
	ఒకవేళ (unlikely(!దత్తాంశం))
		తిరిగి;

	ఎంపిక (దత్తాంశం->type) {
	స్థితి SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(దత్తాంశం);
		ఆపు;
	స్థితి SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(దత్తాంశం);
		ఆపు;
	స్థితి SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(దత్తాంశం);
		ఆపు;
	స్థితి SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(దత్తాంశం);
		ఆపు;
	స్థితి SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(దత్తాంశం);
		ఆపు;
	స్థితి SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(దత్తాంశం);
		ఆపు;
	స్థితి SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(దత్తాంశం);
		ఆపు;
	అప్రమేయ:
		ఆపు;
	};
}

స్థిర శూన్య tlb_నమోదు_process(నిర్మాణం sbi_tlb_info *tinfo)
{
	u32 rindex;
	నిర్మాణం sbi_scratch *rscratch = ఖాళీ;
	atomic_t *rtlb_sync = ఖాళీ;

	tlb_నమోదు_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_గమ్యం_scratch(rindex);
		ఒకవేళ (!rscratch)
			కొనసాగు;

		rtlb_sync = sbi_scratch_స్థానభ్రంశం_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

స్థిర bool tlb_process_once(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_tlb_info tinfo;
	నిర్మాణం sbi_fifo *tlb_fifo =
			sbi_scratch_స్థానభ్రంశం_ptr(scratch, tlb_fifo_off);

	ఒకవేళ (!sbi_fifo_వరుస_నుండి_తీయి(tlb_fifo, &tinfo)) {
		tlb_నమోదు_process(&tinfo);
		తిరిగి true;
	}

	తిరిగి false;
}

స్థిర శూన్య tlb_process(నిర్మాణం sbi_scratch *scratch)
{
	అయితే (tlb_process_once(scratch));
}

స్థిర శూన్య tlb_sync(నిర్మాణం sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_స్థానభ్రంశం_ptr(scratch, tlb_sync_off);

	అయితే (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	తిరిగి;
}

స్థిర అంతఃవరుస పూర్ణాంకం tlb_range_check(నిర్మాణం sbi_tlb_info *curr,
					నిర్మాణం sbi_tlb_info *తదుపరి)
{
	గుర్తులేని దీర్ఘ curr_చివర;
	గుర్తులేని దీర్ఘ తదుపరి_చివర;
	పూర్ణాంకం ret = SBI_FIFO_UNCHANGED;

	ఒకవేళ (!curr || !తదుపరి)
		తిరిగి ret;

	తదుపరి_చివర = తదుపరి->ప్రారంభం + తదుపరి->కొలత;
	curr_చివర = curr->ప్రారంభం + curr->కొలత;
	ఒకవేళ (తదుపరి->ప్రారంభం <= curr->ప్రారంభం && తదుపరి_చివర > curr_చివర) {
		curr->ప్రారంభం = తదుపరి->ప్రారంభం;
		curr->కొలత  = తదుపరి->కొలత;
		sbi_hartmask_or(&curr->smask, &curr->smask, &తదుపరి->smask);
		ret = SBI_FIFO_UPDATED;
	} లేకపోతే ఒకవేళ (తదుపరి->ప్రారంభం >= curr->ప్రారంభం && తదుపరి_చివర <= curr_చివర) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &తదుపరి->smask);
		ret = SBI_FIFO_SKIP;
	}

	తిరిగి ret;
}

/**
 * Call back to decide if an inplace fifo update is required or next entry can
 * can be skipped. Here are the different cases that are being handled.
 *
 * Case1:
 *	if next flush request range lies within one of the existing entry, skip
 *	the next entry.
 * Case2:
 *	if flush request range in current fifo entry lies within next flush
 *	request, update the current entry.
 *
 * Note:
 *	We can not issue a fifo reset anymore if a complete vma flush is requested.
 *	This is because we are queueing FENCE.I requests as well now.
 *	To ease up the pressure in enqueue/fifo sync path, try to dequeue 1 element
 *	before continuing the while loop. This method is preferred over wfi/ipi because
 *	of MMIO cost involved in later method.
 */
స్థిర పూర్ణాంకం tlb_update_cb(శూన్య *లోపల, శూన్య *దత్తాంశం)
{
	నిర్మాణం sbi_tlb_info *curr;
	నిర్మాణం sbi_tlb_info *తదుపరి;
	పూర్ణాంకం ret = SBI_FIFO_UNCHANGED;

	ఒకవేళ (!లోపల || !దత్తాంశం)
		తిరిగి ret;

	curr = (నిర్మాణం sbi_tlb_info *)దత్తాంశం;
	తదుపరి = (నిర్మాణం sbi_tlb_info *)లోపల;

	ఒకవేళ (తదుపరి->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		ఒకవేళ (తదుపరి->asid == curr->asid)
			ret = tlb_range_check(curr, తదుపరి);
	} లేకపోతే ఒకవేళ (తదుపరి->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, తదుపరి);
	}

	తిరిగి ret;
}

స్థిర పూర్ణాంకం tlb_update(నిర్మాణం sbi_scratch *scratch,
			  నిర్మాణం sbi_scratch *remote_scratch,
			  u32 remote_hartindex, శూన్య *దత్తాంశం)
{
	పూర్ణాంకం ret;
	atomic_t *tlb_sync;
	నిర్మాణం sbi_fifo *tlb_fifo_r;
	నిర్మాణం sbi_tlb_info *tinfo = దత్తాంశం;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	ఒకవేళ (sbi_hartindex_గమ్యం_hartid(remote_hartindex) == curr_hartid) {
		tlb_నమోదు_local_process(tinfo);
		తిరిగి SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_స్థానభ్రంశం_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, దత్తాంశం, tlb_update_cb);

	ఒకవేళ (ret == SBI_FIFO_UNCHANGED &&
	    sbi_fifo_వరుసలో_చేర్చు(tlb_fifo_r, దత్తాంశం, false) < 0) {
		/**
		 * For now, Busy loop until there is space in the fifo.
		 * There may be case where target hart is also
		 * enqueue in source hart's fifo. Both hart may busy
		 * loop leading to a deadlock.
		 * TODO: Introduce a wait/wakeup event mechanism to handle
		 * this properly.
		 */
		tlb_process_once(scratch);
		sbi_dprintf("hart%d: hart%d tlb fifo full\n", curr_hartid,
			    sbi_hartindex_గమ్యం_hartid(remote_hartindex));
		తిరిగి SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_స్థానభ్రంశం_ptr(scratch, tlb_sync_off);
	atomic_కూడు_return(tlb_sync, 1);

	తిరిగి SBI_IPI_UPDATE_SUCCESS;
}

స్థిర నిర్మాణం sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

స్థిర u32 tlb_event = SBI_IPI_EVENT_MAX;

స్థిర స్థిరరాశి u32 tlb_type_గమ్యం_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_పంపిన,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_పంపిన,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_పంపిన,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_పంపిన,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_పంపిన,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_పంపిన,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_పంపిన,
};

పూర్ణాంకం sbi_tlb_request(ulong hmask, ulong hbase, నిర్మాణం sbi_tlb_info *tinfo)
{
	ఒకవేళ (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		తిరిగి SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	ఒకవేళ (tinfo->కొలత > tlb_range_flush_హద్దు) {
		tinfo->ప్రారంభం = 0;
		tinfo->కొలత = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_గమ్యం_pmu_fw_event[tinfo->type]);

	తిరిగి sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

పూర్ణాంకం sbi_tlb_init(నిర్మాణం sbi_scratch *scratch, bool cold_boot)
{
	పూర్ణాంకం ret;
	శూన్య *tlb_mem;
	atomic_t *tlb_sync;
	నిర్మాణం sbi_fifo *tlb_q;
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_ptr(scratch);

	ఒకవేళ (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_స్థానభ్రంశం(పరిమాణం(*tlb_sync));
		ఒకవేళ (!tlb_sync_off)
			తిరిగి SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_స్థానభ్రంశం(పరిమాణం(*tlb_q));
		ఒకవేళ (!tlb_fifo_off) {
			sbi_scratch_free_స్థానభ్రంశం(tlb_sync_off);
			తిరిగి SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_స్థానభ్రంశం(పరిమాణం(tlb_mem));
		ఒకవేళ (!tlb_fifo_mem_off) {
			sbi_scratch_free_స్థానభ్రంశం(tlb_fifo_off);
			sbi_scratch_free_స్థానభ్రంశం(tlb_sync_off);
			తిరిగి SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		ఒకవేళ (ret < 0) {
			sbi_scratch_free_స్థానభ్రంశం(tlb_fifo_mem_off);
			sbi_scratch_free_స్థానభ్రంశం(tlb_fifo_off);
			sbi_scratch_free_స్థానభ్రంశం(tlb_sync_off);
			తిరిగి ret;
		}
		tlb_event = ret;
		tlb_range_flush_హద్దు = sbi_platform_tlbr_flush_హద్దు(plat);
	} లేకపోతే {
		ఒకవేళ (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			తిరిగి SBI_ENOMEM;
		ఒకవేళ (SBI_IPI_EVENT_MAX <= tlb_event)
			తిరిగి SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_స్థానభ్రంశం_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_స్థానభ్రంశం_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, శూన్య *, tlb_fifo_mem_off);
	ఒకవేళ (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_అంకె_entries(plat) * SBI_TLB_INFO_కొలత);
		ఒకవేళ (!tlb_mem)
			తిరిగి SBI_ENOMEM;
		sbi_scratch_write_type(scratch, శూన్య *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_అంకె_entries(plat), SBI_TLB_INFO_కొలత);

	తిరిగి 0;
}
