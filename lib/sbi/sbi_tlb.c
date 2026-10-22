/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_atomic.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_barrier.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_fifo.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ipi.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_tlb.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hfence.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_pmu.h>

ସ୍ଥିର unsigned long tlb_sync_off;
ସ୍ଥିର unsigned long tlb_fifo_off;
ସ୍ଥିର unsigned long tlb_fifo_mem_off;
ସ୍ଥିର unsigned long tlb_range_flush_limit;

ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_fence_i(ସଂରଚନା sbi_tlb_info *tinfo);
ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_sfence_vma(ସଂରଚନା sbi_tlb_info *tinfo);
ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_sfence_vma_asid(ସଂରଚନା sbi_tlb_info *tinfo);
ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_hfence_gvma_vmid(ସଂରଚନା sbi_tlb_info *tinfo);
ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_hfence_gvma(ସଂରଚନା sbi_tlb_info *tinfo);
ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_hfence_vvma_asid(ସଂରଚନା sbi_tlb_info *tinfo);
ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_hfence_vvma(ସଂରଚନା sbi_tlb_info *tinfo);

ସ୍ଥିର ସଂରଚନା sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

ଅଚଳ ସଂରଚନା sbi_tlb_local_operations *sbi_tlb_get_local_operations(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ tlb_local_ops;
}

ଶୂନ୍ୟ sbi_tlb_set_local_operations(
		ଅଚଳ ସଂରଚନା sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

ଶୂନ୍ୟ __sbi_sfence_vma_all(ଶୂନ୍ୟ)
{
	__asm__ __volatile("sfence.vma");
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_hfence_vvma(ସଂରଚନା sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	ଯଦି ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		ଯାଅ done;
	}

	ପାଇଁ (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_va(start+i);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_hfence_gvma(ସଂରଚନା sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	ଯଦି ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		ଫେରନ୍ତୁ;
	}

	ପାଇଁ (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_gpa((start + i) >> 2);
	}
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_sfence_vma(ସଂରଚନା sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	ଯଦି ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		ଫେରନ୍ତୁ;
	}

	ପାଇଁ (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(start + i)
				     : "memory");
	}
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_hfence_vvma_asid(ସଂରଚନା sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	ଯଦି ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		ଯାଅ done;
	}

	ପାଇଁ (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_asid_va(start + i, asid);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_hfence_gvma_vmid(ସଂରଚନା sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	ଯଦି ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		ଫେରନ୍ତୁ;
	}

	ପାଇଁ (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_vmid_gpa((start + i) >> 2, vmid);
	}
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_sfence_vma_asid(ସଂରଚନା sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	ଯଦି ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		ଫେରନ୍ତୁ;
	}

	ପାଇଁ (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(start + i), "r"(asid)
				     : "memory");
	}
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_tlb_local_fence_i(ସଂରଚନା sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

ସ୍ଥିର ଶୂନ୍ୟ tlb_entry_local_process(ସଂରଚନା sbi_tlb_info *data)
{
	ଯଦି (unlikely(!data))
		ଫେରନ୍ତୁ;

	ବାଛ (data->type) {
	ସ୍ଥିତି SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(data);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(data);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(data);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(data);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(data);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(data);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(data);
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	};
}

ସ୍ଥିର ଶୂନ୍ୟ tlb_entry_process(ସଂରଚନା sbi_tlb_info *tinfo)
{
	u32 rindex;
	ସଂରଚନା sbi_scratch *rscratch = ଖାଲି;
	atomic_t *rtlb_sync = ଖାଲି;

	tlb_entry_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_to_scratch(rindex);
		ଯଦି (!rscratch)
			ଜାରି;

		rtlb_sync = sbi_scratch_offset_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

ସ୍ଥିର bool tlb_process_once(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_tlb_info tinfo;
	ସଂରଚନା sbi_fifo *tlb_fifo =
			sbi_scratch_offset_ptr(scratch, tlb_fifo_off);

	ଯଦି (!sbi_fifo_dequeue(tlb_fifo, &tinfo)) {
		tlb_entry_process(&tinfo);
		ଫେରନ୍ତୁ true;
	}

	ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ଶୂନ୍ୟ tlb_process(ସଂରଚନା sbi_scratch *scratch)
{
	ଯେତେବେଳେ (tlb_process_once(scratch));
}

ସ୍ଥିର ଶୂନ୍ୟ tlb_sync(ସଂରଚନା sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_offset_ptr(scratch, tlb_sync_off);

	ଯେତେବେଳେ (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	ଫେରନ୍ତୁ;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା tlb_range_check(ସଂରଚନା sbi_tlb_info *curr,
					ସଂରଚନା sbi_tlb_info *next)
{
	unsigned long curr_end;
	unsigned long next_end;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = SBI_FIFO_UNCHANGED;

	ଯଦି (!curr || !next)
		ଫେରନ୍ତୁ ret;

	next_end = next->start + next->size;
	curr_end = curr->start + curr->size;
	ଯଦି (next->start <= curr->start && next_end > curr_end) {
		curr->start = next->start;
		curr->size  = next->size;
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_UPDATED;
	} ଅନ୍ୟଥା ଯଦି (next->start >= curr->start && next_end <= curr_end) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_SKIP;
	}

	ଫେରନ୍ତୁ ret;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tlb_update_cb(ଶୂନ୍ୟ *in, ଶୂନ୍ୟ *data)
{
	ସଂରଚନା sbi_tlb_info *curr;
	ସଂରଚନା sbi_tlb_info *next;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = SBI_FIFO_UNCHANGED;

	ଯଦି (!in || !data)
		ଫେରନ୍ତୁ ret;

	curr = (ସଂରଚନା sbi_tlb_info *)data;
	next = (ସଂରଚନା sbi_tlb_info *)in;

	ଯଦି (next->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		ଯଦି (next->asid == curr->asid)
			ret = tlb_range_check(curr, next);
	} ଅନ୍ୟଥା ଯଦି (next->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, next);
	}

	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tlb_update(ସଂରଚନା sbi_scratch *scratch,
			  ସଂରଚନା sbi_scratch *remote_scratch,
			  u32 remote_hartindex, ଶୂନ୍ୟ *data)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	atomic_t *tlb_sync;
	ସଂରଚନା sbi_fifo *tlb_fifo_r;
	ସଂରଚନା sbi_tlb_info *tinfo = data;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	ଯଦି (sbi_hartindex_to_hartid(remote_hartindex) == curr_hartid) {
		tlb_entry_local_process(tinfo);
		ଫେରନ୍ତୁ SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_offset_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, data, tlb_update_cb);

	ଯଦି (ret == SBI_FIFO_UNCHANGED &&
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
		ଫେରନ୍ତୁ SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	atomic_add_return(tlb_sync, 1);

	ଫେରନ୍ତୁ SBI_IPI_UPDATE_SUCCESS;
}

ସ୍ଥିର ସଂରଚନା sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

ସ୍ଥିର u32 tlb_event = SBI_IPI_EVENT_MAX;

ସ୍ଥିର ଅଚଳ u32 tlb_type_to_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_SENT,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_SENT,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_SENT,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_SENT,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_SENT,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_SENT,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_SENT,
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_tlb_request(ulong hmask, ulong hbase, ସଂରଚନା sbi_tlb_info *tinfo)
{
	ଯଦି (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	ଯଦି (tinfo->size > tlb_range_flush_limit) {
		tinfo->start = 0;
		tinfo->size = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_to_pmu_fw_event[tinfo->type]);

	ଫେରନ୍ତୁ sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_tlb_init(ସଂରଚନା sbi_scratch *scratch, bool cold_boot)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ଶୂନ୍ୟ *tlb_mem;
	atomic_t *tlb_sync;
	ସଂରଚନା sbi_fifo *tlb_q;
	ଅଚଳ ସଂରଚନା sbi_platform *plat = sbi_platform_ptr(scratch);

	ଯଦି (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_offset(ଆକାର(*tlb_sync));
		ଯଦି (!tlb_sync_off)
			ଫେରନ୍ତୁ SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_offset(ଆକାର(*tlb_q));
		ଯଦି (!tlb_fifo_off) {
			sbi_scratch_free_offset(tlb_sync_off);
			ଫେରନ୍ତୁ SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_offset(ଆକାର(tlb_mem));
		ଯଦି (!tlb_fifo_mem_off) {
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			ଫେରନ୍ତୁ SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		ଯଦି (ret < 0) {
			sbi_scratch_free_offset(tlb_fifo_mem_off);
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			ଫେରନ୍ତୁ ret;
		}
		tlb_event = ret;
		tlb_range_flush_limit = sbi_platform_tlbr_flush_limit(plat);
	} ଅନ୍ୟଥା {
		ଯଦି (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			ଫେରନ୍ତୁ SBI_ENOMEM;
		ଯଦି (SBI_IPI_EVENT_MAX <= tlb_event)
			ଫେରନ୍ତୁ SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_offset_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, ଶୂନ୍ୟ *, tlb_fifo_mem_off);
	ଯଦି (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_ଅଙ୍କ_entries(plat) * SBI_TLB_INFO_SIZE);
		ଯଦି (!tlb_mem)
			ଫେରନ୍ତୁ SBI_ENOMEM;
		sbi_scratch_write_type(scratch, ଶୂନ୍ୟ *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_ଅଙ୍କ_entries(plat), SBI_TLB_INFO_SIZE);

	ଫେରନ୍ତୁ 0;
}
