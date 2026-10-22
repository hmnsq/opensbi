/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_atomic.h>
#অন্তর্ভাব <sbi/riscv_barrier.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_fifo.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_ipi.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_tlb.h>
#অন্তর্ভাব <sbi/sbi_hfence.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_pmu.h>

স্থির unsigned long tlb_sync_off;
স্থির unsigned long tlb_fifo_off;
স্থির unsigned long tlb_fifo_mem_off;
স্থির unsigned long tlb_range_flush_limit;

স্থির শূন্য sbi_tlb_local_fence_i(সংরচনা sbi_tlb_info *tinfo);
স্থির শূন্য sbi_tlb_local_sfence_vma(সংরচনা sbi_tlb_info *tinfo);
স্থির শূন্য sbi_tlb_local_sfence_vma_asid(সংরচনা sbi_tlb_info *tinfo);
স্থির শূন্য sbi_tlb_local_hfence_gvma_vmid(সংরচনা sbi_tlb_info *tinfo);
স্থির শূন্য sbi_tlb_local_hfence_gvma(সংরচনা sbi_tlb_info *tinfo);
স্থির শূন্য sbi_tlb_local_hfence_vvma_asid(সংরচনা sbi_tlb_info *tinfo);
স্থির শূন্য sbi_tlb_local_hfence_vvma(সংরচনা sbi_tlb_info *tinfo);

স্থির সংরচনা sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

স্থির ধ্রুব সংরচনা sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

ধ্রুব সংরচনা sbi_tlb_local_operations *sbi_tlb_get_local_operations(শূন্য)
{
	প্রত্যাগম tlb_local_ops;
}

শূন্য sbi_tlb_set_local_operations(
		ধ্রুব সংরচনা sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

শূন্য __sbi_sfence_vma_all(শূন্য)
{
	__asm__ __volatile("sfence.vma");
}

স্থির শূন্য sbi_tlb_local_hfence_vvma(সংরচনা sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	যদি ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		গচ্ছ done;
	}

	প্রতি (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_va(start+i);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

স্থির শূন্য sbi_tlb_local_hfence_gvma(সংরচনা sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	যদি ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		প্রত্যাগম;
	}

	প্রতি (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_gpa((start + i) >> 2);
	}
}

স্থির শূন্য sbi_tlb_local_sfence_vma(সংরচনা sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	যদি ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		প্রত্যাগম;
	}

	প্রতি (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(start + i)
				     : "memory");
	}
}

স্থির শূন্য sbi_tlb_local_hfence_vvma_asid(সংরচনা sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	যদি ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		গচ্ছ done;
	}

	প্রতি (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_asid_va(start + i, asid);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

স্থির শূন্য sbi_tlb_local_hfence_gvma_vmid(সংরচনা sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	যদি ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		প্রত্যাগম;
	}

	প্রতি (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_vmid_gpa((start + i) >> 2, vmid);
	}
}

স্থির শূন্য sbi_tlb_local_sfence_vma_asid(সংরচনা sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	যদি ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		প্রত্যাগম;
	}

	প্রতি (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(start + i), "r"(asid)
				     : "memory");
	}
}

স্থির শূন্য sbi_tlb_local_fence_i(সংরচনা sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

স্থির শূন্য tlb_entry_local_process(সংরচনা sbi_tlb_info *data)
{
	যদি (unlikely(!data))
		প্রত্যাগম;

	বিকল্প (data->type) {
	পক্ষ SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(data);
		ভঙ্গ;
	পক্ষ SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(data);
		ভঙ্গ;
	পক্ষ SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(data);
		ভঙ্গ;
	পক্ষ SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(data);
		ভঙ্গ;
	পক্ষ SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(data);
		ভঙ্গ;
	পক্ষ SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(data);
		ভঙ্গ;
	পক্ষ SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(data);
		ভঙ্গ;
	স্বভাব:
		ভঙ্গ;
	};
}

স্থির শূন্য tlb_entry_process(সংরচনা sbi_tlb_info *tinfo)
{
	u32 rindex;
	সংরচনা sbi_scratch *rscratch = রিক্ত;
	atomic_t *rtlb_sync = রিক্ত;

	tlb_entry_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_to_scratch(rindex);
		যদি (!rscratch)
			অনুবর্ত;

		rtlb_sync = sbi_scratch_offset_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

স্থির bool tlb_process_once(সংরচনা sbi_scratch *scratch)
{
	সংরচনা sbi_tlb_info tinfo;
	সংরচনা sbi_fifo *tlb_fifo =
			sbi_scratch_offset_ptr(scratch, tlb_fifo_off);

	যদি (!sbi_fifo_dequeue(tlb_fifo, &tinfo)) {
		tlb_entry_process(&tinfo);
		প্রত্যাগম true;
	}

	প্রত্যাগম false;
}

স্থির শূন্য tlb_process(সংরচনা sbi_scratch *scratch)
{
	যাবত্ (tlb_process_once(scratch));
}

স্থির শূন্য tlb_sync(সংরচনা sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_offset_ptr(scratch, tlb_sync_off);

	যাবত্ (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	প্রত্যাগম;
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক tlb_range_check(সংরচনা sbi_tlb_info *curr,
					সংরচনা sbi_tlb_info *next)
{
	unsigned long curr_end;
	unsigned long next_end;
	পূর্ণাঙ্ক ret = SBI_FIFO_UNCHANGED;

	যদি (!curr || !next)
		প্রত্যাগম ret;

	next_end = next->start + next->size;
	curr_end = curr->start + curr->size;
	যদি (next->start <= curr->start && next_end > curr_end) {
		curr->start = next->start;
		curr->size  = next->size;
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_UPDATED;
	} অন্যথা যদি (next->start >= curr->start && next_end <= curr_end) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_SKIP;
	}

	প্রত্যাগম ret;
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
স্থির পূর্ণাঙ্ক tlb_update_cb(শূন্য *in, শূন্য *data)
{
	সংরচনা sbi_tlb_info *curr;
	সংরচনা sbi_tlb_info *next;
	পূর্ণাঙ্ক ret = SBI_FIFO_UNCHANGED;

	যদি (!in || !data)
		প্রত্যাগম ret;

	curr = (সংরচনা sbi_tlb_info *)data;
	next = (সংরচনা sbi_tlb_info *)in;

	যদি (next->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		যদি (next->asid == curr->asid)
			ret = tlb_range_check(curr, next);
	} অন্যথা যদি (next->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, next);
	}

	প্রত্যাগম ret;
}

স্থির পূর্ণাঙ্ক tlb_update(সংরচনা sbi_scratch *scratch,
			  সংরচনা sbi_scratch *remote_scratch,
			  u32 remote_hartindex, শূন্য *data)
{
	পূর্ণাঙ্ক ret;
	atomic_t *tlb_sync;
	সংরচনা sbi_fifo *tlb_fifo_r;
	সংরচনা sbi_tlb_info *tinfo = data;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	যদি (sbi_hartindex_to_hartid(remote_hartindex) == curr_hartid) {
		tlb_entry_local_process(tinfo);
		প্রত্যাগম SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_offset_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, data, tlb_update_cb);

	যদি (ret == SBI_FIFO_UNCHANGED &&
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
		প্রত্যাগম SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	atomic_add_return(tlb_sync, 1);

	প্রত্যাগম SBI_IPI_UPDATE_SUCCESS;
}

স্থির সংরচনা sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

স্থির u32 tlb_event = SBI_IPI_EVENT_MAX;

স্থির ধ্রুব u32 tlb_type_to_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_SENT,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_SENT,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_SENT,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_SENT,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_SENT,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_SENT,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_SENT,
};

পূর্ণাঙ্ক sbi_tlb_request(ulong hmask, ulong hbase, সংরচনা sbi_tlb_info *tinfo)
{
	যদি (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		প্রত্যাগম SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	যদি (tinfo->size > tlb_range_flush_limit) {
		tinfo->start = 0;
		tinfo->size = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_to_pmu_fw_event[tinfo->type]);

	প্রত্যাগম sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

পূর্ণাঙ্ক sbi_tlb_init(সংরচনা sbi_scratch *scratch, bool cold_boot)
{
	পূর্ণাঙ্ক ret;
	শূন্য *tlb_mem;
	atomic_t *tlb_sync;
	সংরচনা sbi_fifo *tlb_q;
	ধ্রুব সংরচনা sbi_platform *plat = sbi_platform_ptr(scratch);

	যদি (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_offset(পরিমাণ(*tlb_sync));
		যদি (!tlb_sync_off)
			প্রত্যাগম SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_offset(পরিমাণ(*tlb_q));
		যদি (!tlb_fifo_off) {
			sbi_scratch_free_offset(tlb_sync_off);
			প্রত্যাগম SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_offset(পরিমাণ(tlb_mem));
		যদি (!tlb_fifo_mem_off) {
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			প্রত্যাগম SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		যদি (ret < 0) {
			sbi_scratch_free_offset(tlb_fifo_mem_off);
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			প্রত্যাগম ret;
		}
		tlb_event = ret;
		tlb_range_flush_limit = sbi_platform_tlbr_flush_limit(plat);
	} অন্যথা {
		যদি (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			প্রত্যাগম SBI_ENOMEM;
		যদি (SBI_IPI_EVENT_MAX <= tlb_event)
			প্রত্যাগম SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_offset_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, শূন্য *, tlb_fifo_mem_off);
	যদি (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_অঙ্ক_entries(plat) * SBI_TLB_INFO_SIZE);
		যদি (!tlb_mem)
			প্রত্যাগম SBI_ENOMEM;
		sbi_scratch_write_type(scratch, শূন্য *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_অঙ্ক_entries(plat), SBI_TLB_INFO_SIZE);

	প্রত্যাগম 0;
}
