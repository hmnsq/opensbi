/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_atomic.h>
#અન્તર્ભાવ <sbi/riscv_barrier.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_fifo.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_ipi.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_tlb.h>
#અન્તર્ભાવ <sbi/sbi_hfence.h>
#અન્તર્ભાવ <sbi/sbi_string.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_pmu.h>

સ્થિર unsigned long tlb_sync_off;
સ્થિર unsigned long tlb_fifo_off;
સ્થિર unsigned long tlb_fifo_mem_off;
સ્થિર unsigned long tlb_range_flush_limit;

સ્થિર શૂન્ય sbi_tlb_local_fence_i(સંરચના sbi_tlb_info *tinfo);
સ્થિર શૂન્ય sbi_tlb_local_sfence_vma(સંરચના sbi_tlb_info *tinfo);
સ્થિર શૂન્ય sbi_tlb_local_sfence_vma_asid(સંરચના sbi_tlb_info *tinfo);
સ્થિર શૂન્ય sbi_tlb_local_hfence_gvma_vmid(સંરચના sbi_tlb_info *tinfo);
સ્થિર શૂન્ય sbi_tlb_local_hfence_gvma(સંરચના sbi_tlb_info *tinfo);
સ્થિર શૂન્ય sbi_tlb_local_hfence_vvma_asid(સંરચના sbi_tlb_info *tinfo);
સ્થિર શૂન્ય sbi_tlb_local_hfence_vvma(સંરચના sbi_tlb_info *tinfo);

સ્થિર સંરચના sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

સ્થિર અચલ સંરચના sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

અચલ સંરચના sbi_tlb_local_operations *sbi_tlb_get_local_operations(શૂન્ય)
{
	પાછા tlb_local_ops;
}

શૂન્ય sbi_tlb_set_local_operations(
		અચલ સંરચના sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

શૂન્ય __sbi_sfence_vma_all(શૂન્ય)
{
	__asm__ __volatile("sfence.vma");
}

સ્થિર શૂન્ય sbi_tlb_local_hfence_vvma(સંરચના sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	જો ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		જાઓ done;
	}

	માટે (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_va(start+i);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

સ્થિર શૂન્ય sbi_tlb_local_hfence_gvma(સંરચના sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	જો ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		પાછા;
	}

	માટે (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_gpa((start + i) >> 2);
	}
}

સ્થિર શૂન્ય sbi_tlb_local_sfence_vma(સંરચના sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	જો ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		પાછા;
	}

	માટે (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(start + i)
				     : "memory");
	}
}

સ્થિર શૂન્ય sbi_tlb_local_hfence_vvma_asid(સંરચના sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	જો ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		જાઓ done;
	}

	માટે (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_asid_va(start + i, asid);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

સ્થિર શૂન્ય sbi_tlb_local_hfence_gvma_vmid(સંરચના sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	જો ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		પાછા;
	}

	માટે (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_vmid_gpa((start + i) >> 2, vmid);
	}
}

સ્થિર શૂન્ય sbi_tlb_local_sfence_vma_asid(સંરચના sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	જો ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		પાછા;
	}

	માટે (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(start + i), "r"(asid)
				     : "memory");
	}
}

સ્થિર શૂન્ય sbi_tlb_local_fence_i(સંરચના sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

સ્થિર શૂન્ય tlb_entry_local_process(સંરચના sbi_tlb_info *data)
{
	જો (unlikely(!data))
		પાછા;

	પસંદગી (data->type) {
	સ્થિતિ SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(data);
		તોડો;
	સ્થિતિ SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(data);
		તોડો;
	સ્થિતિ SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(data);
		તોડો;
	સ્થિતિ SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(data);
		તોડો;
	સ્થિતિ SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(data);
		તોડો;
	સ્થિતિ SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(data);
		તોડો;
	સ્થિતિ SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(data);
		તોડો;
	મૂળભૂત:
		તોડો;
	};
}

સ્થિર શૂન્ય tlb_entry_process(સંરચના sbi_tlb_info *tinfo)
{
	u32 rindex;
	સંરચના sbi_scratch *rscratch = ખાલી;
	atomic_t *rtlb_sync = ખાલી;

	tlb_entry_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_to_scratch(rindex);
		જો (!rscratch)
			ચાલુ;

		rtlb_sync = sbi_scratch_offset_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

સ્થિર bool tlb_process_once(સંરચના sbi_scratch *scratch)
{
	સંરચના sbi_tlb_info tinfo;
	સંરચના sbi_fifo *tlb_fifo =
			sbi_scratch_offset_ptr(scratch, tlb_fifo_off);

	જો (!sbi_fifo_dequeue(tlb_fifo, &tinfo)) {
		tlb_entry_process(&tinfo);
		પાછા true;
	}

	પાછા false;
}

સ્થિર શૂન્ય tlb_process(સંરચના sbi_scratch *scratch)
{
	જ્યારે (tlb_process_once(scratch));
}

સ્થિર શૂન્ય tlb_sync(સંરચના sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_offset_ptr(scratch, tlb_sync_off);

	જ્યારે (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	પાછા;
}

સ્થિર અંતઃરેખ પૂર્ણાંક tlb_range_check(સંરચના sbi_tlb_info *curr,
					સંરચના sbi_tlb_info *next)
{
	unsigned long curr_end;
	unsigned long next_end;
	પૂર્ણાંક ret = SBI_FIFO_UNCHANGED;

	જો (!curr || !next)
		પાછા ret;

	next_end = next->start + next->size;
	curr_end = curr->start + curr->size;
	જો (next->start <= curr->start && next_end > curr_end) {
		curr->start = next->start;
		curr->size  = next->size;
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_UPDATED;
	} નહીંતર જો (next->start >= curr->start && next_end <= curr_end) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_SKIP;
	}

	પાછા ret;
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
સ્થિર પૂર્ણાંક tlb_update_cb(શૂન્ય *in, શૂન્ય *data)
{
	સંરચના sbi_tlb_info *curr;
	સંરચના sbi_tlb_info *next;
	પૂર્ણાંક ret = SBI_FIFO_UNCHANGED;

	જો (!in || !data)
		પાછા ret;

	curr = (સંરચના sbi_tlb_info *)data;
	next = (સંરચના sbi_tlb_info *)in;

	જો (next->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		જો (next->asid == curr->asid)
			ret = tlb_range_check(curr, next);
	} નહીંતર જો (next->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, next);
	}

	પાછા ret;
}

સ્થિર પૂર્ણાંક tlb_update(સંરચના sbi_scratch *scratch,
			  સંરચના sbi_scratch *remote_scratch,
			  u32 remote_hartindex, શૂન્ય *data)
{
	પૂર્ણાંક ret;
	atomic_t *tlb_sync;
	સંરચના sbi_fifo *tlb_fifo_r;
	સંરચના sbi_tlb_info *tinfo = data;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	જો (sbi_hartindex_to_hartid(remote_hartindex) == curr_hartid) {
		tlb_entry_local_process(tinfo);
		પાછા SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_offset_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, data, tlb_update_cb);

	જો (ret == SBI_FIFO_UNCHANGED &&
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
		પાછા SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	atomic_add_return(tlb_sync, 1);

	પાછા SBI_IPI_UPDATE_SUCCESS;
}

સ્થિર સંરચના sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

સ્થિર u32 tlb_event = SBI_IPI_EVENT_MAX;

સ્થિર અચલ u32 tlb_type_to_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_SENT,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_SENT,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_SENT,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_SENT,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_SENT,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_SENT,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_SENT,
};

પૂર્ણાંક sbi_tlb_request(ulong hmask, ulong hbase, સંરચના sbi_tlb_info *tinfo)
{
	જો (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		પાછા SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	જો (tinfo->size > tlb_range_flush_limit) {
		tinfo->start = 0;
		tinfo->size = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_to_pmu_fw_event[tinfo->type]);

	પાછા sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

પૂર્ણાંક sbi_tlb_init(સંરચના sbi_scratch *scratch, bool cold_boot)
{
	પૂર્ણાંક ret;
	શૂન્ય *tlb_mem;
	atomic_t *tlb_sync;
	સંરચના sbi_fifo *tlb_q;
	અચલ સંરચના sbi_platform *plat = sbi_platform_ptr(scratch);

	જો (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_offset(કદ(*tlb_sync));
		જો (!tlb_sync_off)
			પાછા SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_offset(કદ(*tlb_q));
		જો (!tlb_fifo_off) {
			sbi_scratch_free_offset(tlb_sync_off);
			પાછા SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_offset(કદ(tlb_mem));
		જો (!tlb_fifo_mem_off) {
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			પાછા SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		જો (ret < 0) {
			sbi_scratch_free_offset(tlb_fifo_mem_off);
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			પાછા ret;
		}
		tlb_event = ret;
		tlb_range_flush_limit = sbi_platform_tlbr_flush_limit(plat);
	} નહીંતર {
		જો (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			પાછા SBI_ENOMEM;
		જો (SBI_IPI_EVENT_MAX <= tlb_event)
			પાછા SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_offset_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, શૂન્ય *, tlb_fifo_mem_off);
	જો (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_અંક_entries(plat) * SBI_TLB_INFO_SIZE);
		જો (!tlb_mem)
			પાછા SBI_ENOMEM;
		sbi_scratch_write_type(scratch, શૂન્ય *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_અંક_entries(plat), SBI_TLB_INFO_SIZE);

	પાછા 0;
}
