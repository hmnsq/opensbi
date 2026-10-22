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

ಸ್ಥಿರ unsigned long tlb_sync_off;
ಸ್ಥಿರ unsigned long tlb_fifo_off;
ಸ್ಥಿರ unsigned long tlb_fifo_mem_off;
ಸ್ಥಿರ unsigned long tlb_range_flush_limit;

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_fence_i(ಸಂರಚನಾ sbi_tlb_info *tinfo);
ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_sfence_vma(ಸಂರಚನಾ sbi_tlb_info *tinfo);
ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_sfence_vma_asid(ಸಂರಚನಾ sbi_tlb_info *tinfo);
ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_gvma_vmid(ಸಂರಚನಾ sbi_tlb_info *tinfo);
ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_gvma(ಸಂರಚನಾ sbi_tlb_info *tinfo);
ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_vvma_asid(ಸಂರಚನಾ sbi_tlb_info *tinfo);
ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_vvma(ಸಂರಚನಾ sbi_tlb_info *tinfo);

ಸ್ಥಿರ ಸಂರಚನಾ sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

ಧ್ರುವ ಸಂರಚನಾ sbi_tlb_local_operations *sbi_tlb_get_local_operations(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ tlb_local_ops;
}

ಶೂನ್ಯ sbi_tlb_set_local_operations(
		ಧ್ರುವ ಸಂರಚನಾ sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

ಶೂನ್ಯ __sbi_sfence_vma_all(ಶೂನ್ಯ)
{
	__asm__ __volatile("sfence.vma");
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_vvma(ಸಂರಚನಾ sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	ಯದಿ ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		ಗಚ್ಛ done;
	}

	ಪ್ರತಿ (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_va(start+i);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_gvma(ಸಂರಚನಾ sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	ಯದಿ ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಪ್ರತಿ (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_gpa((start + i) >> 2);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_sfence_vma(ಸಂರಚನಾ sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	ಯದಿ ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಪ್ರತಿ (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(start + i)
				     : "memory");
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_vvma_asid(ಸಂರಚನಾ sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	ಯದಿ ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		ಗಚ್ಛ done;
	}

	ಪ್ರತಿ (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_asid_va(start + i, asid);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_hfence_gvma_vmid(ಸಂರಚನಾ sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	ಯದಿ ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಪ್ರತಿ (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_vmid_gpa((start + i) >> 2, vmid);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_sfence_vma_asid(ಸಂರಚನಾ sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	ಯದಿ ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಪ್ರತಿ (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(start + i), "r"(asid)
				     : "memory");
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_tlb_local_fence_i(ಸಂರಚನಾ sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

ಸ್ಥಿರ ಶೂನ್ಯ tlb_entry_local_process(ಸಂರಚನಾ sbi_tlb_info *data)
{
	ಯದಿ (unlikely(!data))
		ಪ್ರತ್ಯಾಗಮ;

	ವಿಕಲ್ಪ (data->type) {
	ಪಕ್ಷ SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(data);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(data);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(data);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(data);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(data);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(data);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(data);
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	};
}

ಸ್ಥಿರ ಶೂನ್ಯ tlb_entry_process(ಸಂರಚನಾ sbi_tlb_info *tinfo)
{
	u32 rindex;
	ಸಂರಚನಾ sbi_scratch *rscratch = ರಿಕ್ತ;
	atomic_t *rtlb_sync = ರಿಕ್ತ;

	tlb_entry_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_to_scratch(rindex);
		ಯದಿ (!rscratch)
			ಅನುವರ್ತ;

		rtlb_sync = sbi_scratch_offset_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

ಸ್ಥಿರ bool tlb_process_once(ಸಂರಚನಾ sbi_scratch *scratch)
{
	ಸಂರಚನಾ sbi_tlb_info tinfo;
	ಸಂರಚನಾ sbi_fifo *tlb_fifo =
			sbi_scratch_offset_ptr(scratch, tlb_fifo_off);

	ಯದಿ (!sbi_fifo_dequeue(tlb_fifo, &tinfo)) {
		tlb_entry_process(&tinfo);
		ಪ್ರತ್ಯಾಗಮ true;
	}

	ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ ಶೂನ್ಯ tlb_process(ಸಂರಚನಾ sbi_scratch *scratch)
{
	ಯಾವತ್ (tlb_process_once(scratch));
}

ಸ್ಥಿರ ಶೂನ್ಯ tlb_sync(ಸಂರಚನಾ sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_offset_ptr(scratch, tlb_sync_off);

	ಯಾವತ್ (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	ಪ್ರತ್ಯಾಗಮ;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ tlb_range_check(ಸಂರಚನಾ sbi_tlb_info *curr,
					ಸಂರಚನಾ sbi_tlb_info *next)
{
	unsigned long curr_end;
	unsigned long next_end;
	ಪೂರ್ಣಾಙ್ಕ ret = SBI_FIFO_UNCHANGED;

	ಯದಿ (!curr || !next)
		ಪ್ರತ್ಯಾಗಮ ret;

	next_end = next->start + next->size;
	curr_end = curr->start + curr->size;
	ಯದಿ (next->start <= curr->start && next_end > curr_end) {
		curr->start = next->start;
		curr->size  = next->size;
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_UPDATED;
	} ಅನ್ಯಥಾ ಯದಿ (next->start >= curr->start && next_end <= curr_end) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_SKIP;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tlb_update_cb(ಶೂನ್ಯ *in, ಶೂನ್ಯ *data)
{
	ಸಂರಚನಾ sbi_tlb_info *curr;
	ಸಂರಚನಾ sbi_tlb_info *next;
	ಪೂರ್ಣಾಙ್ಕ ret = SBI_FIFO_UNCHANGED;

	ಯದಿ (!in || !data)
		ಪ್ರತ್ಯಾಗಮ ret;

	curr = (ಸಂರಚನಾ sbi_tlb_info *)data;
	next = (ಸಂರಚನಾ sbi_tlb_info *)in;

	ಯದಿ (next->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		ಯದಿ (next->asid == curr->asid)
			ret = tlb_range_check(curr, next);
	} ಅನ್ಯಥಾ ಯದಿ (next->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, next);
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tlb_update(ಸಂರಚನಾ sbi_scratch *scratch,
			  ಸಂರಚನಾ sbi_scratch *remote_scratch,
			  u32 remote_hartindex, ಶೂನ್ಯ *data)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	atomic_t *tlb_sync;
	ಸಂರಚನಾ sbi_fifo *tlb_fifo_r;
	ಸಂರಚನಾ sbi_tlb_info *tinfo = data;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	ಯದಿ (sbi_hartindex_to_hartid(remote_hartindex) == curr_hartid) {
		tlb_entry_local_process(tinfo);
		ಪ್ರತ್ಯಾಗಮ SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_offset_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, data, tlb_update_cb);

	ಯದಿ (ret == SBI_FIFO_UNCHANGED &&
	    sbi_fifo_enqueue(tlb_fifo_r, data, false) < 0) {
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
			    sbi_hartindex_to_hartid(remote_hartindex));
		ಪ್ರತ್ಯಾಗಮ SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	atomic_add_return(tlb_sync, 1);

	ಪ್ರತ್ಯಾಗಮ SBI_IPI_UPDATE_SUCCESS;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

ಸ್ಥಿರ u32 tlb_event = SBI_IPI_EVENT_MAX;

ಸ್ಥಿರ ಧ್ರುವ u32 tlb_type_to_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_SENT,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_SENT,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_SENT,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_SENT,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_SENT,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_SENT,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_SENT,
};

ಪೂರ್ಣಾಙ್ಕ sbi_tlb_request(ulong hmask, ulong hbase, ಸಂರಚನಾ sbi_tlb_info *tinfo)
{
	ಯದಿ (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	ಯದಿ (tinfo->size > tlb_range_flush_limit) {
		tinfo->start = 0;
		tinfo->size = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_to_pmu_fw_event[tinfo->type]);

	ಪ್ರತ್ಯಾಗಮ sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

ಪೂರ್ಣಾಙ್ಕ sbi_tlb_init(ಸಂರಚನಾ sbi_scratch *scratch, bool cold_boot)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಶೂನ್ಯ *tlb_mem;
	atomic_t *tlb_sync;
	ಸಂರಚನಾ sbi_fifo *tlb_q;
	ಧ್ರುವ ಸಂರಚನಾ sbi_platform *plat = sbi_platform_ptr(scratch);

	ಯದಿ (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_offset(ಪರಿಮಾಣ(*tlb_sync));
		ಯದಿ (!tlb_sync_off)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_offset(ಪರಿಮಾಣ(*tlb_q));
		ಯದಿ (!tlb_fifo_off) {
			sbi_scratch_free_offset(tlb_sync_off);
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_offset(ಪರಿಮಾಣ(tlb_mem));
		ಯದಿ (!tlb_fifo_mem_off) {
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		ಯದಿ (ret < 0) {
			sbi_scratch_free_offset(tlb_fifo_mem_off);
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			ಪ್ರತ್ಯಾಗಮ ret;
		}
		tlb_event = ret;
		tlb_range_flush_limit = sbi_platform_tlbr_flush_limit(plat);
	} ಅನ್ಯಥಾ {
		ಯದಿ (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
		ಯದಿ (SBI_IPI_EVENT_MAX <= tlb_event)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_offset_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, ಶೂನ್ಯ *, tlb_fifo_mem_off);
	ಯದಿ (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_ಅಂಕೆ_entries(plat) * SBI_TLB_INFO_SIZE);
		ಯದಿ (!tlb_mem)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
		sbi_scratch_write_type(scratch, ಶೂನ್ಯ *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_ಅಂಕೆ_entries(plat), SBI_TLB_INFO_SIZE);

	ಪ್ರತ್ಯಾಗಮ 0;
}
