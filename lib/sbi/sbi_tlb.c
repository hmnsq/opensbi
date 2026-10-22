/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_atomic.h>
#അന്തര്ഭാവ <sbi/riscv_barrier.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_fifo.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_ipi.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_tlb.h>
#അന്തര്ഭാവ <sbi/sbi_hfence.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_pmu.h>

സ്ഥിതം unsigned long tlb_sync_off;
സ്ഥിതം unsigned long tlb_fifo_off;
സ്ഥിതം unsigned long tlb_fifo_mem_off;
സ്ഥിതം unsigned long tlb_range_flush_limit;

സ്ഥിതം ശൂന്യം sbi_tlb_local_fence_i(ഘടന sbi_tlb_info *tinfo);
സ്ഥിതം ശൂന്യം sbi_tlb_local_sfence_vma(ഘടന sbi_tlb_info *tinfo);
സ്ഥിതം ശൂന്യം sbi_tlb_local_sfence_vma_asid(ഘടന sbi_tlb_info *tinfo);
സ്ഥിതം ശൂന്യം sbi_tlb_local_hfence_gvma_vmid(ഘടന sbi_tlb_info *tinfo);
സ്ഥിതം ശൂന്യം sbi_tlb_local_hfence_gvma(ഘടന sbi_tlb_info *tinfo);
സ്ഥിതം ശൂന്യം sbi_tlb_local_hfence_vvma_asid(ഘടന sbi_tlb_info *tinfo);
സ്ഥിതം ശൂന്യം sbi_tlb_local_hfence_vvma(ഘടന sbi_tlb_info *tinfo);

സ്ഥിതം ഘടന sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

സ്ഥിതം സ്ഥിരം ഘടന sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

സ്ഥിരം ഘടന sbi_tlb_local_operations *sbi_tlb_get_local_operations(ശൂന്യം)
{
	തിരികെ tlb_local_ops;
}

ശൂന്യം sbi_tlb_set_local_operations(
		സ്ഥിരം ഘടന sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

ശൂന്യം __sbi_sfence_vma_all(ശൂന്യം)
{
	__asm__ __volatile("sfence.vma");
}

സ്ഥിതം ശൂന്യം sbi_tlb_local_hfence_vvma(ഘടന sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	എങ്കിൽ ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		പോകുക done;
	}

	വേണ്ടി (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_va(start+i);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

സ്ഥിതം ശൂന്യം sbi_tlb_local_hfence_gvma(ഘടന sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	എങ്കിൽ ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		തിരികെ;
	}

	വേണ്ടി (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_gpa((start + i) >> 2);
	}
}

സ്ഥിതം ശൂന്യം sbi_tlb_local_sfence_vma(ഘടന sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	എങ്കിൽ ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		തിരികെ;
	}

	വേണ്ടി (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(start + i)
				     : "memory");
	}
}

സ്ഥിതം ശൂന്യം sbi_tlb_local_hfence_vvma_asid(ഘടന sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	എങ്കിൽ ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		പോകുക done;
	}

	വേണ്ടി (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_asid_va(start + i, asid);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

സ്ഥിതം ശൂന്യം sbi_tlb_local_hfence_gvma_vmid(ഘടന sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	എങ്കിൽ ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		തിരികെ;
	}

	വേണ്ടി (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_vmid_gpa((start + i) >> 2, vmid);
	}
}

സ്ഥിതം ശൂന്യം sbi_tlb_local_sfence_vma_asid(ഘടന sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	എങ്കിൽ ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		തിരികെ;
	}

	വേണ്ടി (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(start + i), "r"(asid)
				     : "memory");
	}
}

സ്ഥിതം ശൂന്യം sbi_tlb_local_fence_i(ഘടന sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

സ്ഥിതം ശൂന്യം tlb_entry_local_process(ഘടന sbi_tlb_info *data)
{
	എങ്കിൽ (unlikely(!data))
		തിരികെ;

	തിരഞ്ഞെടുപ്പ് (data->type) {
	അവസ്ഥ SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(data);
		നിർത്തുക;
	അവസ്ഥ SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(data);
		നിർത്തുക;
	അവസ്ഥ SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(data);
		നിർത്തുക;
	അവസ്ഥ SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(data);
		നിർത്തുക;
	അവസ്ഥ SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(data);
		നിർത്തുക;
	അവസ്ഥ SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(data);
		നിർത്തുക;
	അവസ്ഥ SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(data);
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	};
}

സ്ഥിതം ശൂന്യം tlb_entry_process(ഘടന sbi_tlb_info *tinfo)
{
	u32 rindex;
	ഘടന sbi_scratch *rscratch = ശൂന്യ;
	atomic_t *rtlb_sync = ശൂന്യ;

	tlb_entry_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_to_scratch(rindex);
		എങ്കിൽ (!rscratch)
			തുടരുക;

		rtlb_sync = sbi_scratch_offset_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

സ്ഥിതം bool tlb_process_once(ഘടന sbi_scratch *scratch)
{
	ഘടന sbi_tlb_info tinfo;
	ഘടന sbi_fifo *tlb_fifo =
			sbi_scratch_offset_ptr(scratch, tlb_fifo_off);

	എങ്കിൽ (!sbi_fifo_dequeue(tlb_fifo, &tinfo)) {
		tlb_entry_process(&tinfo);
		തിരികെ true;
	}

	തിരികെ false;
}

സ്ഥിതം ശൂന്യം tlb_process(ഘടന sbi_scratch *scratch)
{
	ആകുമ്പോൾ (tlb_process_once(scratch));
}

സ്ഥിതം ശൂന്യം tlb_sync(ഘടന sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_offset_ptr(scratch, tlb_sync_off);

	ആകുമ്പോൾ (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	തിരികെ;
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ tlb_range_check(ഘടന sbi_tlb_info *curr,
					ഘടന sbi_tlb_info *next)
{
	unsigned long curr_end;
	unsigned long next_end;
	പൂർണ്ണസംഖ്യ ret = SBI_FIFO_UNCHANGED;

	എങ്കിൽ (!curr || !next)
		തിരികെ ret;

	next_end = next->start + next->size;
	curr_end = curr->start + curr->size;
	എങ്കിൽ (next->start <= curr->start && next_end > curr_end) {
		curr->start = next->start;
		curr->size  = next->size;
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_UPDATED;
	} അല്ലെങ്കിൽ എങ്കിൽ (next->start >= curr->start && next_end <= curr_end) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_SKIP;
	}

	തിരികെ ret;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ tlb_update_cb(ശൂന്യം *in, ശൂന്യം *data)
{
	ഘടന sbi_tlb_info *curr;
	ഘടന sbi_tlb_info *next;
	പൂർണ്ണസംഖ്യ ret = SBI_FIFO_UNCHANGED;

	എങ്കിൽ (!in || !data)
		തിരികെ ret;

	curr = (ഘടന sbi_tlb_info *)data;
	next = (ഘടന sbi_tlb_info *)in;

	എങ്കിൽ (next->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		എങ്കിൽ (next->asid == curr->asid)
			ret = tlb_range_check(curr, next);
	} അല്ലെങ്കിൽ എങ്കിൽ (next->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, next);
	}

	തിരികെ ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ tlb_update(ഘടന sbi_scratch *scratch,
			  ഘടന sbi_scratch *remote_scratch,
			  u32 remote_hartindex, ശൂന്യം *data)
{
	പൂർണ്ണസംഖ്യ ret;
	atomic_t *tlb_sync;
	ഘടന sbi_fifo *tlb_fifo_r;
	ഘടന sbi_tlb_info *tinfo = data;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	എങ്കിൽ (sbi_hartindex_to_hartid(remote_hartindex) == curr_hartid) {
		tlb_entry_local_process(tinfo);
		തിരികെ SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_offset_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, data, tlb_update_cb);

	എങ്കിൽ (ret == SBI_FIFO_UNCHANGED &&
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
		തിരികെ SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	atomic_add_return(tlb_sync, 1);

	തിരികെ SBI_IPI_UPDATE_SUCCESS;
}

സ്ഥിതം ഘടന sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

സ്ഥിതം u32 tlb_event = SBI_IPI_EVENT_MAX;

സ്ഥിതം സ്ഥിരം u32 tlb_type_to_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_SENT,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_SENT,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_SENT,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_SENT,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_SENT,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_SENT,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_SENT,
};

പൂർണ്ണസംഖ്യ sbi_tlb_request(ulong hmask, ulong hbase, ഘടന sbi_tlb_info *tinfo)
{
	എങ്കിൽ (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		തിരികെ SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	എങ്കിൽ (tinfo->size > tlb_range_flush_limit) {
		tinfo->start = 0;
		tinfo->size = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_to_pmu_fw_event[tinfo->type]);

	തിരികെ sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

പൂർണ്ണസംഖ്യ sbi_tlb_init(ഘടന sbi_scratch *scratch, bool cold_boot)
{
	പൂർണ്ണസംഖ്യ ret;
	ശൂന്യം *tlb_mem;
	atomic_t *tlb_sync;
	ഘടന sbi_fifo *tlb_q;
	സ്ഥിരം ഘടന sbi_platform *plat = sbi_platform_ptr(scratch);

	എങ്കിൽ (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_offset(വലുപ്പം(*tlb_sync));
		എങ്കിൽ (!tlb_sync_off)
			തിരികെ SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_offset(വലുപ്പം(*tlb_q));
		എങ്കിൽ (!tlb_fifo_off) {
			sbi_scratch_free_offset(tlb_sync_off);
			തിരികെ SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_offset(വലുപ്പം(tlb_mem));
		എങ്കിൽ (!tlb_fifo_mem_off) {
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			തിരികെ SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		എങ്കിൽ (ret < 0) {
			sbi_scratch_free_offset(tlb_fifo_mem_off);
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			തിരികെ ret;
		}
		tlb_event = ret;
		tlb_range_flush_limit = sbi_platform_tlbr_flush_limit(plat);
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			തിരികെ SBI_ENOMEM;
		എങ്കിൽ (SBI_IPI_EVENT_MAX <= tlb_event)
			തിരികെ SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_offset_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, ശൂന്യം *, tlb_fifo_mem_off);
	എങ്കിൽ (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_അക്കം_entries(plat) * SBI_TLB_INFO_SIZE);
		എങ്കിൽ (!tlb_mem)
			തിരികെ SBI_ENOMEM;
		sbi_scratch_write_type(scratch, ശൂന്യം *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_അക്കം_entries(plat), SBI_TLB_INFO_SIZE);

	തിരികെ 0;
}
