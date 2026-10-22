/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_atomic.h>
#அந்தர்பாவ <sbi/riscv_barrier.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_fifo.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_ipi.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_tlb.h>
#அந்தர்பாவ <sbi/sbi_hfence.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_pmu.h>

நிலையான unsigned long tlb_sync_off;
நிலையான unsigned long tlb_fifo_off;
நிலையான unsigned long tlb_fifo_mem_off;
நிலையான unsigned long tlb_range_flush_limit;

நிலையான வெறுமை sbi_tlb_local_fence_i(கட்டமைப்பு sbi_tlb_info *tinfo);
நிலையான வெறுமை sbi_tlb_local_sfence_vma(கட்டமைப்பு sbi_tlb_info *tinfo);
நிலையான வெறுமை sbi_tlb_local_sfence_vma_asid(கட்டமைப்பு sbi_tlb_info *tinfo);
நிலையான வெறுமை sbi_tlb_local_hfence_gvma_vmid(கட்டமைப்பு sbi_tlb_info *tinfo);
நிலையான வெறுமை sbi_tlb_local_hfence_gvma(கட்டமைப்பு sbi_tlb_info *tinfo);
நிலையான வெறுமை sbi_tlb_local_hfence_vvma_asid(கட்டமைப்பு sbi_tlb_info *tinfo);
நிலையான வெறுமை sbi_tlb_local_hfence_vvma(கட்டமைப்பு sbi_tlb_info *tinfo);

நிலையான கட்டமைப்பு sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

நிலையான மாறிலி கட்டமைப்பு sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

மாறிலி கட்டமைப்பு sbi_tlb_local_operations *sbi_tlb_get_local_operations(வெறுமை)
{
	திரும்பு tlb_local_ops;
}

வெறுமை sbi_tlb_set_local_operations(
		மாறிலி கட்டமைப்பு sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

வெறுமை __sbi_sfence_vma_all(வெறுமை)
{
	__asm__ __volatile("sfence.vma");
}

நிலையான வெறுமை sbi_tlb_local_hfence_vvma(கட்டமைப்பு sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	எனில் ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		செல் done;
	}

	க்கு (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_va(start+i);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

நிலையான வெறுமை sbi_tlb_local_hfence_gvma(கட்டமைப்பு sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	எனில் ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		திரும்பு;
	}

	க்கு (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_gpa((start + i) >> 2);
	}
}

நிலையான வெறுமை sbi_tlb_local_sfence_vma(கட்டமைப்பு sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	எனில் ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		திரும்பு;
	}

	க்கு (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(start + i)
				     : "memory");
	}
}

நிலையான வெறுமை sbi_tlb_local_hfence_vvma_asid(கட்டமைப்பு sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	எனில் ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		செல் done;
	}

	க்கு (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_asid_va(start + i, asid);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

நிலையான வெறுமை sbi_tlb_local_hfence_gvma_vmid(கட்டமைப்பு sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	எனில் ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		திரும்பு;
	}

	க்கு (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_vmid_gpa((start + i) >> 2, vmid);
	}
}

நிலையான வெறுமை sbi_tlb_local_sfence_vma_asid(கட்டமைப்பு sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	எனில் ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		திரும்பு;
	}

	க்கு (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(start + i), "r"(asid)
				     : "memory");
	}
}

நிலையான வெறுமை sbi_tlb_local_fence_i(கட்டமைப்பு sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

நிலையான வெறுமை tlb_entry_local_process(கட்டமைப்பு sbi_tlb_info *data)
{
	எனில் (unlikely(!data))
		திரும்பு;

	தேர்வு (data->type) {
	நிலை SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(data);
		நிறுத்து;
	நிலை SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(data);
		நிறுத்து;
	நிலை SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(data);
		நிறுத்து;
	நிலை SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(data);
		நிறுத்து;
	நிலை SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(data);
		நிறுத்து;
	நிலை SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(data);
		நிறுத்து;
	நிலை SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(data);
		நிறுத்து;
	இயல்பு:
		நிறுத்து;
	};
}

நிலையான வெறுமை tlb_entry_process(கட்டமைப்பு sbi_tlb_info *tinfo)
{
	u32 rindex;
	கட்டமைப்பு sbi_scratch *rscratch = வெற்று;
	atomic_t *rtlb_sync = வெற்று;

	tlb_entry_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_to_scratch(rindex);
		எனில் (!rscratch)
			தொடர்;

		rtlb_sync = sbi_scratch_offset_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

நிலையான bool tlb_process_once(கட்டமைப்பு sbi_scratch *scratch)
{
	கட்டமைப்பு sbi_tlb_info tinfo;
	கட்டமைப்பு sbi_fifo *tlb_fifo =
			sbi_scratch_offset_ptr(scratch, tlb_fifo_off);

	எனில் (!sbi_fifo_dequeue(tlb_fifo, &tinfo)) {
		tlb_entry_process(&tinfo);
		திரும்பு true;
	}

	திரும்பு false;
}

நிலையான வெறுமை tlb_process(கட்டமைப்பு sbi_scratch *scratch)
{
	போது (tlb_process_once(scratch));
}

நிலையான வெறுமை tlb_sync(கட்டமைப்பு sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_offset_ptr(scratch, tlb_sync_off);

	போது (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	திரும்பு;
}

நிலையான உள்வரி முழு tlb_range_check(கட்டமைப்பு sbi_tlb_info *curr,
					கட்டமைப்பு sbi_tlb_info *next)
{
	unsigned long curr_end;
	unsigned long next_end;
	முழு ret = SBI_FIFO_UNCHANGED;

	எனில் (!curr || !next)
		திரும்பு ret;

	next_end = next->start + next->size;
	curr_end = curr->start + curr->size;
	எனில் (next->start <= curr->start && next_end > curr_end) {
		curr->start = next->start;
		curr->size  = next->size;
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_UPDATED;
	} இல்லையெனில் எனில் (next->start >= curr->start && next_end <= curr_end) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_SKIP;
	}

	திரும்பு ret;
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
நிலையான முழு tlb_update_cb(வெறுமை *in, வெறுமை *data)
{
	கட்டமைப்பு sbi_tlb_info *curr;
	கட்டமைப்பு sbi_tlb_info *next;
	முழு ret = SBI_FIFO_UNCHANGED;

	எனில் (!in || !data)
		திரும்பு ret;

	curr = (கட்டமைப்பு sbi_tlb_info *)data;
	next = (கட்டமைப்பு sbi_tlb_info *)in;

	எனில் (next->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		எனில் (next->asid == curr->asid)
			ret = tlb_range_check(curr, next);
	} இல்லையெனில் எனில் (next->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, next);
	}

	திரும்பு ret;
}

நிலையான முழு tlb_update(கட்டமைப்பு sbi_scratch *scratch,
			  கட்டமைப்பு sbi_scratch *remote_scratch,
			  u32 remote_hartindex, வெறுமை *data)
{
	முழு ret;
	atomic_t *tlb_sync;
	கட்டமைப்பு sbi_fifo *tlb_fifo_r;
	கட்டமைப்பு sbi_tlb_info *tinfo = data;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	எனில் (sbi_hartindex_to_hartid(remote_hartindex) == curr_hartid) {
		tlb_entry_local_process(tinfo);
		திரும்பு SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_offset_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, data, tlb_update_cb);

	எனில் (ret == SBI_FIFO_UNCHANGED &&
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
		திரும்பு SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	atomic_add_return(tlb_sync, 1);

	திரும்பு SBI_IPI_UPDATE_SUCCESS;
}

நிலையான கட்டமைப்பு sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

நிலையான u32 tlb_event = SBI_IPI_EVENT_MAX;

நிலையான மாறிலி u32 tlb_type_to_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_SENT,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_SENT,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_SENT,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_SENT,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_SENT,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_SENT,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_SENT,
};

முழு sbi_tlb_request(ulong hmask, ulong hbase, கட்டமைப்பு sbi_tlb_info *tinfo)
{
	எனில் (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		திரும்பு SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	எனில் (tinfo->size > tlb_range_flush_limit) {
		tinfo->start = 0;
		tinfo->size = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_to_pmu_fw_event[tinfo->type]);

	திரும்பு sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

முழு sbi_tlb_init(கட்டமைப்பு sbi_scratch *scratch, bool cold_boot)
{
	முழு ret;
	வெறுமை *tlb_mem;
	atomic_t *tlb_sync;
	கட்டமைப்பு sbi_fifo *tlb_q;
	மாறிலி கட்டமைப்பு sbi_platform *plat = sbi_platform_ptr(scratch);

	எனில் (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_offset(அளவு(*tlb_sync));
		எனில் (!tlb_sync_off)
			திரும்பு SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_offset(அளவு(*tlb_q));
		எனில் (!tlb_fifo_off) {
			sbi_scratch_free_offset(tlb_sync_off);
			திரும்பு SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_offset(அளவு(tlb_mem));
		எனில் (!tlb_fifo_mem_off) {
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			திரும்பு SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		எனில் (ret < 0) {
			sbi_scratch_free_offset(tlb_fifo_mem_off);
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			திரும்பு ret;
		}
		tlb_event = ret;
		tlb_range_flush_limit = sbi_platform_tlbr_flush_limit(plat);
	} இல்லையெனில் {
		எனில் (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			திரும்பு SBI_ENOMEM;
		எனில் (SBI_IPI_EVENT_MAX <= tlb_event)
			திரும்பு SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_offset_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, வெறுமை *, tlb_fifo_mem_off);
	எனில் (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_இலக்கம்_entries(plat) * SBI_TLB_INFO_SIZE);
		எனில் (!tlb_mem)
			திரும்பு SBI_ENOMEM;
		sbi_scratch_write_type(scratch, வெறுமை *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_இலக்கம்_entries(plat), SBI_TLB_INFO_SIZE);

	திரும்பு 0;
}
