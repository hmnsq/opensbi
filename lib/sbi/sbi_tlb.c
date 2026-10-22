/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_atomic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_barrier.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_fifo.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_tlb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hfence.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmu.h>

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tlb_sync_off;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tlb_fifo_off;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tlb_fifo_mem_off;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tlb_range_flush_ਹੱਦਬੰਦੀ;

ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_fence_i(ਬਣਤਰ sbi_tlb_info *tinfo);
ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_sfence_vma(ਬਣਤਰ sbi_tlb_info *tinfo);
ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_sfence_vma_asid(ਬਣਤਰ sbi_tlb_info *tinfo);
ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_hfence_gvma_vmid(ਬਣਤਰ sbi_tlb_info *tinfo);
ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_hfence_gvma(ਬਣਤਰ sbi_tlb_info *tinfo);
ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_hfence_vvma_asid(ਬਣਤਰ sbi_tlb_info *tinfo);
ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_hfence_vvma(ਬਣਤਰ sbi_tlb_info *tinfo);

ਸਥਿਰ ਬਣਤਰ sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

ਅਚਲ ਬਣਤਰ sbi_tlb_local_operations *sbi_tlb_get_local_operations(ਖਾਲੀ)
{
	ਵਾਪਸ tlb_local_ops;
}

ਖਾਲੀ sbi_tlb_ਸੈੱਟ_local_operations(
		ਅਚਲ ਬਣਤਰ sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

ਖਾਲੀ __sbi_sfence_vma_all(ਖਾਲੀ)
{
	__asm__ __volatile("sfence.vma");
}

ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_hfence_vvma(ਬਣਤਰ sbi_tlb_info *tinfo)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸ਼ੁਰੂ = tinfo->ਸ਼ੁਰੂ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ  = tinfo->ਮਾਪ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ vmid  = tinfo->vmid;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_ਅਦਲਾ_ਬਦਲੀ(CSR_HGATP,
			 (vmid << HGATP_VMID_ਖਿਸਕਾਓ) & HGATP_VMID_ਨਕਾਬ);

	ਜੇ ((ਸ਼ੁਰੂ == 0 && ਮਾਪ == 0) || (ਮਾਪ == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		ਜਾਓ ਪੂਰਾ;
	}

	ਲਈ (i = 0; i < ਮਾਪ; i += ਪੰਨਾ_ਮਾਪ) {
		__sbi_hfence_vvma_va(ਸ਼ੁਰੂ+i);
	}

ਪੂਰਾ:
	csr_write(CSR_HGATP, hgatp);
}

ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_hfence_gvma(ਬਣਤਰ sbi_tlb_info *tinfo)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸ਼ੁਰੂ = tinfo->ਸ਼ੁਰੂ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ  = tinfo->ਮਾਪ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	ਜੇ ((ਸ਼ੁਰੂ == 0 && ਮਾਪ == 0) || (ਮਾਪ == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		ਵਾਪਸ;
	}

	ਲਈ (i = 0; i < ਮਾਪ; i += ਪੰਨਾ_ਮਾਪ) {
		__sbi_hfence_gvma_gpa((ਸ਼ੁਰੂ + i) >> 2);
	}
}

ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_sfence_vma(ਬਣਤਰ sbi_tlb_info *tinfo)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸ਼ੁਰੂ = tinfo->ਸ਼ੁਰੂ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ  = tinfo->ਮਾਪ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	ਜੇ ((ਸ਼ੁਰੂ == 0 && ਮਾਪ == 0) || (ਮਾਪ == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		ਵਾਪਸ;
	}

	ਲਈ (i = 0; i < ਮਾਪ; i += ਪੰਨਾ_ਮਾਪ) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(ਸ਼ੁਰੂ + i)
				     : "memory");
	}
}

ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_hfence_vvma_asid(ਬਣਤਰ sbi_tlb_info *tinfo)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸ਼ੁਰੂ = tinfo->ਸ਼ੁਰੂ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ  = tinfo->ਮਾਪ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ asid  = tinfo->asid;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ vmid  = tinfo->vmid;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_ਅਦਲਾ_ਬਦਲੀ(CSR_HGATP,
			 (vmid << HGATP_VMID_ਖਿਸਕਾਓ) & HGATP_VMID_ਨਕਾਬ);

	ਜੇ ((ਸ਼ੁਰੂ == 0 && ਮਾਪ == 0) || (ਮਾਪ == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		ਜਾਓ ਪੂਰਾ;
	}

	ਲਈ (i = 0; i < ਮਾਪ; i += ਪੰਨਾ_ਮਾਪ) {
		__sbi_hfence_vvma_asid_va(ਸ਼ੁਰੂ + i, asid);
	}

ਪੂਰਾ:
	csr_write(CSR_HGATP, hgatp);
}

ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_hfence_gvma_vmid(ਬਣਤਰ sbi_tlb_info *tinfo)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸ਼ੁਰੂ = tinfo->ਸ਼ੁਰੂ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ  = tinfo->ਮਾਪ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ vmid  = tinfo->vmid;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	ਜੇ ((ਸ਼ੁਰੂ == 0 && ਮਾਪ == 0) || (ਮਾਪ == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		ਵਾਪਸ;
	}

	ਲਈ (i = 0; i < ਮਾਪ; i += ਪੰਨਾ_ਮਾਪ) {
		__sbi_hfence_gvma_vmid_gpa((ਸ਼ੁਰੂ + i) >> 2, vmid);
	}
}

ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_sfence_vma_asid(ਬਣਤਰ sbi_tlb_info *tinfo)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸ਼ੁਰੂ = tinfo->ਸ਼ੁਰੂ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ  = tinfo->ਮਾਪ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ asid  = tinfo->asid;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	ਜੇ ((ਸ਼ੁਰੂ == 0 && ਮਾਪ == 0) || (ਮਾਪ == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		ਵਾਪਸ;
	}

	ਲਈ (i = 0; i < ਮਾਪ; i += ਪੰਨਾ_ਮਾਪ) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(ਸ਼ੁਰੂ + i), "r"(asid)
				     : "memory");
	}
}

ਸਥਿਰ ਖਾਲੀ sbi_tlb_local_fence_i(ਬਣਤਰ sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

ਸਥਿਰ ਖਾਲੀ tlb_ਇੰਦਰਾਜ_local_process(ਬਣਤਰ sbi_tlb_info *ਅੰਕੜੇ)
{
	ਜੇ (unlikely(!ਅੰਕੜੇ))
		ਵਾਪਸ;

	ਚੋਣ (ਅੰਕੜੇ->type) {
	ਹਾਲਤ SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(ਅੰਕੜੇ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(ਅੰਕੜੇ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(ਅੰਕੜੇ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(ਅੰਕੜੇ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(ਅੰਕੜੇ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(ਅੰਕੜੇ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(ਅੰਕੜੇ);
		ਰੋਕੋ;
	ਮੂਲ:
		ਰੋਕੋ;
	};
}

ਸਥਿਰ ਖਾਲੀ tlb_ਇੰਦਰਾਜ_process(ਬਣਤਰ sbi_tlb_info *tinfo)
{
	u32 rindex;
	ਬਣਤਰ sbi_scratch *rscratch = ਨਿਰਾ;
	atomic_t *rtlb_sync = ਨਿਰਾ;

	tlb_ਇੰਦਰਾਜ_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(rindex);
		ਜੇ (!rscratch)
			ਜਾਰੀ;

		rtlb_sync = sbi_scratch_ਵਿੱਥ_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

ਸਥਿਰ bool tlb_process_once(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_tlb_info tinfo;
	ਬਣਤਰ sbi_fifo *tlb_fifo =
			sbi_scratch_ਵਿੱਥ_ptr(scratch, tlb_fifo_off);

	ਜੇ (!sbi_fifo_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(tlb_fifo, &tinfo)) {
		tlb_ਇੰਦਰਾਜ_process(&tinfo);
		ਵਾਪਸ true;
	}

	ਵਾਪਸ false;
}

ਸਥਿਰ ਖਾਲੀ tlb_process(ਬਣਤਰ sbi_scratch *scratch)
{
	ਜਦੋਂ (tlb_process_once(scratch));
}

ਸਥਿਰ ਖਾਲੀ tlb_sync(ਬਣਤਰ sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_ਵਿੱਥ_ptr(scratch, tlb_sync_off);

	ਜਦੋਂ (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	ਵਾਪਸ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ tlb_range_check(ਬਣਤਰ sbi_tlb_info *curr,
					ਬਣਤਰ sbi_tlb_info *ਅਗਲਾ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ curr_ਸਿਰਾ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਗਲਾ_ਸਿਰਾ;
	ਪੂਰਨ ret = SBI_FIFO_UNCHANGED;

	ਜੇ (!curr || !ਅਗਲਾ)
		ਵਾਪਸ ret;

	ਅਗਲਾ_ਸਿਰਾ = ਅਗਲਾ->ਸ਼ੁਰੂ + ਅਗਲਾ->ਮਾਪ;
	curr_ਸਿਰਾ = curr->ਸ਼ੁਰੂ + curr->ਮਾਪ;
	ਜੇ (ਅਗਲਾ->ਸ਼ੁਰੂ <= curr->ਸ਼ੁਰੂ && ਅਗਲਾ_ਸਿਰਾ > curr_ਸਿਰਾ) {
		curr->ਸ਼ੁਰੂ = ਅਗਲਾ->ਸ਼ੁਰੂ;
		curr->ਮਾਪ  = ਅਗਲਾ->ਮਾਪ;
		sbi_hartmask_or(&curr->smask, &curr->smask, &ਅਗਲਾ->smask);
		ret = SBI_FIFO_UPDATED;
	} ਨਹੀਂਤਾਂ ਜੇ (ਅਗਲਾ->ਸ਼ੁਰੂ >= curr->ਸ਼ੁਰੂ && ਅਗਲਾ_ਸਿਰਾ <= curr_ਸਿਰਾ) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &ਅਗਲਾ->smask);
		ret = SBI_FIFO_SKIP;
	}

	ਵਾਪਸ ret;
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
ਸਥਿਰ ਪੂਰਨ tlb_update_cb(ਖਾਲੀ *ਵਿੱਚ_2, ਖਾਲੀ *ਅੰਕੜੇ)
{
	ਬਣਤਰ sbi_tlb_info *curr;
	ਬਣਤਰ sbi_tlb_info *ਅਗਲਾ;
	ਪੂਰਨ ret = SBI_FIFO_UNCHANGED;

	ਜੇ (!ਵਿੱਚ_2 || !ਅੰਕੜੇ)
		ਵਾਪਸ ret;

	curr = (ਬਣਤਰ sbi_tlb_info *)ਅੰਕੜੇ;
	ਅਗਲਾ = (ਬਣਤਰ sbi_tlb_info *)ਵਿੱਚ_2;

	ਜੇ (ਅਗਲਾ->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		ਜੇ (ਅਗਲਾ->asid == curr->asid)
			ret = tlb_range_check(curr, ਅਗਲਾ);
	} ਨਹੀਂਤਾਂ ਜੇ (ਅਗਲਾ->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, ਅਗਲਾ);
	}

	ਵਾਪਸ ret;
}

ਸਥਿਰ ਪੂਰਨ tlb_update(ਬਣਤਰ sbi_scratch *scratch,
			  ਬਣਤਰ sbi_scratch *remote_scratch,
			  u32 remote_hartindex, ਖਾਲੀ *ਅੰਕੜੇ)
{
	ਪੂਰਨ ret;
	atomic_t *tlb_sync;
	ਬਣਤਰ sbi_fifo *tlb_fifo_r;
	ਬਣਤਰ sbi_tlb_info *tinfo = ਅੰਕੜੇ;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	ਜੇ (sbi_hartindex_ਮੰਜ਼ਿਲ_hartid(remote_hartindex) == curr_hartid) {
		tlb_ਇੰਦਰਾਜ_local_process(tinfo);
		ਵਾਪਸ SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_ਵਿੱਥ_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, ਅੰਕੜੇ, tlb_update_cb);

	ਜੇ (ret == SBI_FIFO_UNCHANGED &&
	    sbi_fifo_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(tlb_fifo_r, ਅੰਕੜੇ, false) < 0) {
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
			    sbi_hartindex_ਮੰਜ਼ਿਲ_hartid(remote_hartindex));
		ਵਾਪਸ SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_ਵਿੱਥ_ptr(scratch, tlb_sync_off);
	atomic_ਜੋੜੋ_return(tlb_sync, 1);

	ਵਾਪਸ SBI_IPI_UPDATE_SUCCESS;
}

ਸਥਿਰ ਬਣਤਰ sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

ਸਥਿਰ u32 tlb_event = SBI_IPI_EVENT_MAX;

ਸਥਿਰ ਅਚਲ u32 tlb_type_ਮੰਜ਼ਿਲ_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_ਭੇਜਿਆ,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_ਭੇਜਿਆ,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_ਭੇਜਿਆ,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_ਭੇਜਿਆ,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_ਭੇਜਿਆ,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_ਭੇਜਿਆ,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_ਭੇਜਿਆ,
};

ਪੂਰਨ sbi_tlb_request(ulong hmask, ulong hbase, ਬਣਤਰ sbi_tlb_info *tinfo)
{
	ਜੇ (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		ਵਾਪਸ SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	ਜੇ (tinfo->ਮਾਪ > tlb_range_flush_ਹੱਦਬੰਦੀ) {
		tinfo->ਸ਼ੁਰੂ = 0;
		tinfo->ਮਾਪ = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_ਮੰਜ਼ਿਲ_pmu_fw_event[tinfo->type]);

	ਵਾਪਸ sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

ਪੂਰਨ sbi_tlb_init(ਬਣਤਰ sbi_scratch *scratch, bool cold_boot)
{
	ਪੂਰਨ ret;
	ਖਾਲੀ *tlb_mem;
	atomic_t *tlb_sync;
	ਬਣਤਰ sbi_fifo *tlb_q;
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_ptr(scratch);

	ਜੇ (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_ਵਿੱਥ(ਆਕਾਰ(*tlb_sync));
		ਜੇ (!tlb_sync_off)
			ਵਾਪਸ SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_ਵਿੱਥ(ਆਕਾਰ(*tlb_q));
		ਜੇ (!tlb_fifo_off) {
			sbi_scratch_free_ਵਿੱਥ(tlb_sync_off);
			ਵਾਪਸ SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_ਵਿੱਥ(ਆਕਾਰ(tlb_mem));
		ਜੇ (!tlb_fifo_mem_off) {
			sbi_scratch_free_ਵਿੱਥ(tlb_fifo_off);
			sbi_scratch_free_ਵਿੱਥ(tlb_sync_off);
			ਵਾਪਸ SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		ਜੇ (ret < 0) {
			sbi_scratch_free_ਵਿੱਥ(tlb_fifo_mem_off);
			sbi_scratch_free_ਵਿੱਥ(tlb_fifo_off);
			sbi_scratch_free_ਵਿੱਥ(tlb_sync_off);
			ਵਾਪਸ ret;
		}
		tlb_event = ret;
		tlb_range_flush_ਹੱਦਬੰਦੀ = sbi_platform_tlbr_flush_ਹੱਦਬੰਦੀ(plat);
	} ਨਹੀਂਤਾਂ {
		ਜੇ (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			ਵਾਪਸ SBI_ENOMEM;
		ਜੇ (SBI_IPI_EVENT_MAX <= tlb_event)
			ਵਾਪਸ SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_ਵਿੱਥ_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_ਵਿੱਥ_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, ਖਾਲੀ *, tlb_fifo_mem_off);
	ਜੇ (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_ਅੰਕ_entries(plat) * SBI_TLB_INFO_ਮਾਪ);
		ਜੇ (!tlb_mem)
			ਵਾਪਸ SBI_ENOMEM;
		sbi_scratch_write_type(scratch, ਖਾਲੀ *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_ਅੰਕ_entries(plat), SBI_TLB_INFO_ਮਾਪ);

	ਵਾਪਸ 0;
}
