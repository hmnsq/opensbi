/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_atomic.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_barrier.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_fifo.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_tlb.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hfence.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmu.h>

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tlb_sync_off;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tlb_fifo_off;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tlb_fifo_mem_off;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tlb_range_flush_ಎಲ್ಲೆ;

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_fence_i(ರಚನೆ sbi_tlb_info *tinfo);
ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_sfence_vma(ರಚನೆ sbi_tlb_info *tinfo);
ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_sfence_vma_asid(ರಚನೆ sbi_tlb_info *tinfo);
ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_gvma_vmid(ರಚನೆ sbi_tlb_info *tinfo);
ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_gvma(ರಚನೆ sbi_tlb_info *tinfo);
ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_vvma_asid(ರಚನೆ sbi_tlb_info *tinfo);
ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_vvma(ರಚನೆ sbi_tlb_info *tinfo);

ಸ್ಥಿರ ರಚನೆ sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_tlb_local_operations *sbi_tlb_get_local_operations(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ tlb_local_ops;
}

ಶೂನ್ಯ sbi_tlb_ಗಣ_local_operations(
		ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

ಶೂನ್ಯ __sbi_sfence_vma_all(ಶೂನ್ಯ)
{
	__asm__ __volatile("sfence.vma");
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_vvma(ರಚನೆ sbi_tlb_info *tinfo)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಆರಂಭ = tinfo->ಆರಂಭ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2  = tinfo->ಗಾತ್ರ_2;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ vmid  = tinfo->vmid;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_ಅದಲುಬದಲು(CSR_HGATP,
			 (vmid << HGATP_VMID_ಸರಿಸು) & HGATP_VMID_ಮುಖವಾಡ);

	ಆದರೆ ((ಆರಂಭ == 0 && ಗಾತ್ರ_2 == 0) || (ಗಾತ್ರ_2 == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		ಹೋಗು ಮುಗಿದ;
	}

	ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; i += ಪುಟ_ಗಾತ್ರ) {
		__sbi_hfence_vvma_va(ಆರಂಭ+i);
	}

ಮುಗಿದ:
	csr_write(CSR_HGATP, hgatp);
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_gvma(ರಚನೆ sbi_tlb_info *tinfo)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಆರಂಭ = tinfo->ಆರಂಭ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2  = tinfo->ಗಾತ್ರ_2;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	ಆದರೆ ((ಆರಂಭ == 0 && ಗಾತ್ರ_2 == 0) || (ಗಾತ್ರ_2 == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		ಹಿಂತಿರುಗಿ;
	}

	ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; i += ಪುಟ_ಗಾತ್ರ) {
		__sbi_hfence_gvma_gpa((ಆರಂಭ + i) >> 2);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_sfence_vma(ರಚನೆ sbi_tlb_info *tinfo)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಆರಂಭ = tinfo->ಆರಂಭ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2  = tinfo->ಗಾತ್ರ_2;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	ಆದರೆ ((ಆರಂಭ == 0 && ಗಾತ್ರ_2 == 0) || (ಗಾತ್ರ_2 == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		ಹಿಂತಿರುಗಿ;
	}

	ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; i += ಪುಟ_ಗಾತ್ರ) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(ಆರಂಭ + i)
				     : "memory");
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_vvma_asid(ರಚನೆ sbi_tlb_info *tinfo)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಆರಂಭ = tinfo->ಆರಂಭ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2  = tinfo->ಗಾತ್ರ_2;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ asid  = tinfo->asid;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ vmid  = tinfo->vmid;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_ಅದಲುಬದಲು(CSR_HGATP,
			 (vmid << HGATP_VMID_ಸರಿಸು) & HGATP_VMID_ಮುಖವಾಡ);

	ಆದರೆ ((ಆರಂಭ == 0 && ಗಾತ್ರ_2 == 0) || (ಗಾತ್ರ_2 == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		ಹೋಗು ಮುಗಿದ;
	}

	ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; i += ಪುಟ_ಗಾತ್ರ) {
		__sbi_hfence_vvma_asid_va(ಆರಂಭ + i, asid);
	}

ಮುಗಿದ:
	csr_write(CSR_HGATP, hgatp);
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_gvma_vmid(ರಚನೆ sbi_tlb_info *tinfo)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಆರಂಭ = tinfo->ಆರಂಭ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2  = tinfo->ಗಾತ್ರ_2;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ vmid  = tinfo->vmid;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	ಆದರೆ ((ಆರಂಭ == 0 && ಗಾತ್ರ_2 == 0) || (ಗಾತ್ರ_2 == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		ಹಿಂತಿರುಗಿ;
	}

	ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; i += ಪುಟ_ಗಾತ್ರ) {
		__sbi_hfence_gvma_vmid_gpa((ಆರಂಭ + i) >> 2, vmid);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_sfence_vma_asid(ರಚನೆ sbi_tlb_info *tinfo)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಆರಂಭ = tinfo->ಆರಂಭ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2  = tinfo->ಗಾತ್ರ_2;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ asid  = tinfo->asid;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	ಆದರೆ ((ಆರಂಭ == 0 && ಗಾತ್ರ_2 == 0) || (ಗಾತ್ರ_2 == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		ಹಿಂತಿರುಗಿ;
	}

	ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; i += ಪುಟ_ಗಾತ್ರ) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(ಆರಂಭ + i), "r"(asid)
				     : "memory");
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_fence_i(ರಚನೆ sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

ಸ್ಥಿರ ಶೂನ್ಯ tlb_ನಮೂದು_local_process(ರಚನೆ sbi_tlb_info *ದತ್ತಾಂಶ)
{
	ಆದರೆ (unlikely(!ದತ್ತಾಂಶ))
		ಹಿಂತಿರುಗಿ;

	ಆಯ್ಕೆ (ದತ್ತಾಂಶ->type) {
	ಸ್ಥಿತಿ SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(ದತ್ತಾಂಶ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(ದತ್ತಾಂಶ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(ದತ್ತಾಂಶ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(ದತ್ತಾಂಶ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(ದತ್ತಾಂಶ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(ದತ್ತಾಂಶ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(ದತ್ತಾಂಶ);
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	};
}

ಸ್ಥಿರ ಶೂನ್ಯ tlb_ನಮೂದು_process(ರಚನೆ sbi_tlb_info *tinfo)
{
	u32 rindex;
	ರಚನೆ sbi_scratch *rscratch = ಖಾಲಿ;
	atomic_t *rtlb_sync = ಖಾಲಿ;

	tlb_ನಮೂದು_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_ಗಮ್ಯ_scratch(rindex);
		ಆದರೆ (!rscratch)
			ಮುಂದುವರಿ;

		rtlb_sync = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

ಸ್ಥಿರ bool tlb_process_once(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_tlb_info tinfo;
	ರಚನೆ sbi_fifo *tlb_fifo =
			sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, tlb_fifo_off);

	ಆದರೆ (!sbi_fifo_ಸರತಿಯಿಂದ_ತೆಗೆ(tlb_fifo, &tinfo)) {
		tlb_ನಮೂದು_process(&tinfo);
		ಹಿಂತಿರುಗಿ true;
	}

	ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ಶೂನ್ಯ tlb_process(ರಚನೆ sbi_scratch *scratch)
{
	ಆಗ (tlb_process_once(scratch));
}

ಸ್ಥಿರ ಶೂನ್ಯ tlb_sync(ರಚನೆ sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, tlb_sync_off);

	ಆಗ (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	ಹಿಂತಿರುಗಿ;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ tlb_range_check(ರಚನೆ sbi_tlb_info *curr,
					ರಚನೆ sbi_tlb_info *ಮುಂದಿನ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ curr_ಕೊನೆ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಂದಿನ_ಕೊನೆ;
	ಪೂರ್ಣಾಂಕ ret = SBI_FIFO_UNCHANGED;

	ಆದರೆ (!curr || !ಮುಂದಿನ)
		ಹಿಂತಿರುಗಿ ret;

	ಮುಂದಿನ_ಕೊನೆ = ಮುಂದಿನ->ಆರಂಭ + ಮುಂದಿನ->ಗಾತ್ರ_2;
	curr_ಕೊನೆ = curr->ಆರಂಭ + curr->ಗಾತ್ರ_2;
	ಆದರೆ (ಮುಂದಿನ->ಆರಂಭ <= curr->ಆರಂಭ && ಮುಂದಿನ_ಕೊನೆ > curr_ಕೊನೆ) {
		curr->ಆರಂಭ = ಮುಂದಿನ->ಆರಂಭ;
		curr->ಗಾತ್ರ_2  = ಮುಂದಿನ->ಗಾತ್ರ_2;
		sbi_hartmask_or(&curr->smask, &curr->smask, &ಮುಂದಿನ->smask);
		ret = SBI_FIFO_UPDATED;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮುಂದಿನ->ಆರಂಭ >= curr->ಆರಂಭ && ಮುಂದಿನ_ಕೊನೆ <= curr_ಕೊನೆ) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &ಮುಂದಿನ->smask);
		ret = SBI_FIFO_SKIP;
	}

	ಹಿಂತಿರುಗಿ ret;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tlb_update_cb(ಶೂನ್ಯ *ಒಳ, ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
	ರಚನೆ sbi_tlb_info *curr;
	ರಚನೆ sbi_tlb_info *ಮುಂದಿನ;
	ಪೂರ್ಣಾಂಕ ret = SBI_FIFO_UNCHANGED;

	ಆದರೆ (!ಒಳ || !ದತ್ತಾಂಶ)
		ಹಿಂತಿರುಗಿ ret;

	curr = (ರಚನೆ sbi_tlb_info *)ದತ್ತಾಂಶ;
	ಮುಂದಿನ = (ರಚನೆ sbi_tlb_info *)ಒಳ;

	ಆದರೆ (ಮುಂದಿನ->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		ಆದರೆ (ಮುಂದಿನ->asid == curr->asid)
			ret = tlb_range_check(curr, ಮುಂದಿನ);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮುಂದಿನ->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, ಮುಂದಿನ);
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tlb_update(ರಚನೆ sbi_scratch *scratch,
			  ರಚನೆ sbi_scratch *remote_scratch,
			  u32 remote_hartindex, ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
	ಪೂರ್ಣಾಂಕ ret;
	atomic_t *tlb_sync;
	ರಚನೆ sbi_fifo *tlb_fifo_r;
	ರಚನೆ sbi_tlb_info *tinfo = ದತ್ತಾಂಶ;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	ಆದರೆ (sbi_hartindex_ಗಮ್ಯ_hartid(remote_hartindex) == curr_hartid) {
		tlb_ನಮೂದು_local_process(tinfo);
		ಹಿಂತಿರುಗಿ SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, ದತ್ತಾಂಶ, tlb_update_cb);

	ಆದರೆ (ret == SBI_FIFO_UNCHANGED &&
	    sbi_fifo_ಸರತಿಗೆ_ಸೇರಿಸು(tlb_fifo_r, ದತ್ತಾಂಶ, false) < 0) {
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
			    sbi_hartindex_ಗಮ್ಯ_hartid(remote_hartindex));
		ಹಿಂತಿರುಗಿ SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, tlb_sync_off);
	atomic_ಕೂಡಿಸು_return(tlb_sync, 1);

	ಹಿಂತಿರುಗಿ SBI_IPI_UPDATE_SUCCESS;
}

ಸ್ಥಿರ ರಚನೆ sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

ಸ್ಥಿರ u32 tlb_event = SBI_IPI_EVENT_MAX;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ u32 tlb_type_ಗಮ್ಯ_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_ಕಳುಹಿಸಿದ,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_ಕಳುಹಿಸಿದ,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_ಕಳುಹಿಸಿದ,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_ಕಳುಹಿಸಿದ,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_ಕಳುಹಿಸಿದ,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_ಕಳುಹಿಸಿದ,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_ಕಳುಹಿಸಿದ,
};

ಪೂರ್ಣಾಂಕ sbi_tlb_request(ulong hmask, ulong hbase, ರಚನೆ sbi_tlb_info *tinfo)
{
	ಆದರೆ (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	ಆದರೆ (tinfo->ಗಾತ್ರ_2 > tlb_range_flush_ಎಲ್ಲೆ) {
		tinfo->ಆರಂಭ = 0;
		tinfo->ಗಾತ್ರ_2 = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_ಗಮ್ಯ_pmu_fw_event[tinfo->type]);

	ಹಿಂತಿರುಗಿ sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

ಪೂರ್ಣಾಂಕ sbi_tlb_init(ರಚನೆ sbi_scratch *scratch, bool cold_boot)
{
	ಪೂರ್ಣಾಂಕ ret;
	ಶೂನ್ಯ *tlb_mem;
	atomic_t *tlb_sync;
	ರಚನೆ sbi_fifo *tlb_q;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_ptr(scratch);

	ಆದರೆ (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(ಗಾತ್ರ(*tlb_sync));
		ಆದರೆ (!tlb_sync_off)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(ಗಾತ್ರ(*tlb_q));
		ಆದರೆ (!tlb_fifo_off) {
			sbi_scratch_free_ಸ್ಥಾನಾಂತರ(tlb_sync_off);
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(ಗಾತ್ರ(tlb_mem));
		ಆದರೆ (!tlb_fifo_mem_off) {
			sbi_scratch_free_ಸ್ಥಾನಾಂತರ(tlb_fifo_off);
			sbi_scratch_free_ಸ್ಥಾನಾಂತರ(tlb_sync_off);
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		ಆದರೆ (ret < 0) {
			sbi_scratch_free_ಸ್ಥಾನಾಂತರ(tlb_fifo_mem_off);
			sbi_scratch_free_ಸ್ಥಾನಾಂತರ(tlb_fifo_off);
			sbi_scratch_free_ಸ್ಥಾನಾಂತರ(tlb_sync_off);
			ಹಿಂತಿರುಗಿ ret;
		}
		tlb_event = ret;
		tlb_range_flush_ಎಲ್ಲೆ = sbi_platform_tlbr_flush_ಎಲ್ಲೆ(plat);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
		ಆದರೆ (SBI_IPI_EVENT_MAX <= tlb_event)
			ಹಿಂತಿರುಗಿ SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, ಶೂನ್ಯ *, tlb_fifo_mem_off);
	ಆದರೆ (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_ಅಂಕೆ_entries(plat) * SBI_TLB_INFO_ಗಾತ್ರ);
		ಆದರೆ (!tlb_mem)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
		sbi_scratch_write_type(scratch, ಶೂನ್ಯ *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_ಅಂಕೆ_entries(plat), SBI_TLB_INFO_ಗಾತ್ರ);

	ಹಿಂತಿರುಗಿ 0;
}
