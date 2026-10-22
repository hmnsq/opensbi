/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_atomic.h>
#شامل <sbi/riscv_barrier.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_fifo.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_tlb.h>
#شامل <sbi/sbi_hfence.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_pmu.h>

ایستا unsigned long tlb_sync_off;
ایستا unsigned long tlb_fifo_off;
ایستا unsigned long tlb_fifo_mem_off;
ایستا unsigned long tlb_range_flush_limit;

ایستا تهی sbi_tlb_local_fence_i(ساختار sbi_tlb_info *tinfo);
ایستا تهی sbi_tlb_local_sfence_vma(ساختار sbi_tlb_info *tinfo);
ایستا تهی sbi_tlb_local_sfence_vma_asid(ساختار sbi_tlb_info *tinfo);
ایستا تهی sbi_tlb_local_hfence_gvma_vmid(ساختار sbi_tlb_info *tinfo);
ایستا تهی sbi_tlb_local_hfence_gvma(ساختار sbi_tlb_info *tinfo);
ایستا تهی sbi_tlb_local_hfence_vvma_asid(ساختار sbi_tlb_info *tinfo);
ایستا تهی sbi_tlb_local_hfence_vvma(ساختار sbi_tlb_info *tinfo);

ایستا ساختار sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

ایستا ثابت ساختار sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

ثابت ساختار sbi_tlb_local_operations *sbi_tlb_get_local_operations(تهی)
{
	بازگشت tlb_local_ops;
}

تهی sbi_tlb_set_local_operations(
		ثابت ساختار sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

تهی __sbi_sfence_vma_all(تهی)
{
	__asm__ __volatile("sfence.vma");
}

ایستا تهی sbi_tlb_local_hfence_vvma(ساختار sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	اگر ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		برو done;
	}

	برای (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_va(start+i);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

ایستا تهی sbi_tlb_local_hfence_gvma(ساختار sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	اگر ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		بازگشت;
	}

	برای (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_gpa((start + i) >> 2);
	}
}

ایستا تهی sbi_tlb_local_sfence_vma(ساختار sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	اگر ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		بازگشت;
	}

	برای (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(start + i)
				     : "memory");
	}
}

ایستا تهی sbi_tlb_local_hfence_vvma_asid(ساختار sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	اگر ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		برو done;
	}

	برای (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_asid_va(start + i, asid);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

ایستا تهی sbi_tlb_local_hfence_gvma_vmid(ساختار sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	اگر ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		بازگشت;
	}

	برای (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_vmid_gpa((start + i) >> 2, vmid);
	}
}

ایستا تهی sbi_tlb_local_sfence_vma_asid(ساختار sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	اگر ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		بازگشت;
	}

	برای (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(start + i), "r"(asid)
				     : "memory");
	}
}

ایستا تهی sbi_tlb_local_fence_i(ساختار sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

ایستا تهی tlb_entry_local_process(ساختار sbi_tlb_info *data)
{
	اگر (unlikely(!data))
		بازگشت;

	انتخاب (data->type) {
	حالت SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(data);
		بشکن;
	حالت SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(data);
		بشکن;
	حالت SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(data);
		بشکن;
	حالت SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(data);
		بشکن;
	حالت SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(data);
		بشکن;
	حالت SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(data);
		بشکن;
	حالت SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(data);
		بشکن;
	پیشفرض:
		بشکن;
	};
}

ایستا تهی tlb_entry_process(ساختار sbi_tlb_info *tinfo)
{
	u32 rindex;
	ساختار sbi_scratch *rscratch = پوچ;
	atomic_t *rtlb_sync = پوچ;

	tlb_entry_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_to_scratch(rindex);
		اگر (!rscratch)
			ادامه;

		rtlb_sync = sbi_scratch_offset_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

ایستا bool tlb_process_once(ساختار sbi_scratch *scratch)
{
	ساختار sbi_tlb_info tinfo;
	ساختار sbi_fifo *tlb_fifo =
			sbi_scratch_offset_ptr(scratch, tlb_fifo_off);

	اگر (!sbi_fifo_dequeue(tlb_fifo, &tinfo)) {
		tlb_entry_process(&tinfo);
		بازگشت true;
	}

	بازگشت false;
}

ایستا تهی tlb_process(ساختار sbi_scratch *scratch)
{
	وقتی (tlb_process_once(scratch));
}

ایستا تهی tlb_sync(ساختار sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_offset_ptr(scratch, tlb_sync_off);

	وقتی (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	بازگشت;
}

ایستا درونخطی صحیح tlb_range_check(ساختار sbi_tlb_info *curr,
					ساختار sbi_tlb_info *next)
{
	unsigned long curr_end;
	unsigned long next_end;
	صحیح ret = SBI_FIFO_UNCHANGED;

	اگر (!curr || !next)
		بازگشت ret;

	next_end = next->start + next->size;
	curr_end = curr->start + curr->size;
	اگر (next->start <= curr->start && next_end > curr_end) {
		curr->start = next->start;
		curr->size  = next->size;
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_UPDATED;
	} وگرنه اگر (next->start >= curr->start && next_end <= curr_end) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_SKIP;
	}

	بازگشت ret;
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
ایستا صحیح tlb_update_cb(تهی *in, تهی *data)
{
	ساختار sbi_tlb_info *curr;
	ساختار sbi_tlb_info *next;
	صحیح ret = SBI_FIFO_UNCHANGED;

	اگر (!in || !data)
		بازگشت ret;

	curr = (ساختار sbi_tlb_info *)data;
	next = (ساختار sbi_tlb_info *)in;

	اگر (next->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		اگر (next->asid == curr->asid)
			ret = tlb_range_check(curr, next);
	} وگرنه اگر (next->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, next);
	}

	بازگشت ret;
}

ایستا صحیح tlb_update(ساختار sbi_scratch *scratch,
			  ساختار sbi_scratch *remote_scratch,
			  u32 remote_hartindex, تهی *data)
{
	صحیح ret;
	atomic_t *tlb_sync;
	ساختار sbi_fifo *tlb_fifo_r;
	ساختار sbi_tlb_info *tinfo = data;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	اگر (sbi_hartindex_to_hartid(remote_hartindex) == curr_hartid) {
		tlb_entry_local_process(tinfo);
		بازگشت SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_offset_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, data, tlb_update_cb);

	اگر (ret == SBI_FIFO_UNCHANGED &&
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
		بازگشت SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	atomic_add_return(tlb_sync, 1);

	بازگشت SBI_IPI_UPDATE_SUCCESS;
}

ایستا ساختار sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

ایستا u32 tlb_event = SBI_IPI_EVENT_MAX;

ایستا ثابت u32 tlb_type_to_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_SENT,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_SENT,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_SENT,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_SENT,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_SENT,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_SENT,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_SENT,
};

صحیح sbi_tlb_request(ulong hmask, ulong hbase, ساختار sbi_tlb_info *tinfo)
{
	اگر (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		بازگشت SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	اگر (tinfo->size > tlb_range_flush_limit) {
		tinfo->start = 0;
		tinfo->size = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_to_pmu_fw_event[tinfo->type]);

	بازگشت sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

صحیح sbi_tlb_init(ساختار sbi_scratch *scratch, bool cold_boot)
{
	صحیح ret;
	تهی *tlb_mem;
	atomic_t *tlb_sync;
	ساختار sbi_fifo *tlb_q;
	ثابت ساختار sbi_platform *plat = sbi_platform_ptr(scratch);

	اگر (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_offset(اندازه(*tlb_sync));
		اگر (!tlb_sync_off)
			بازگشت SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_offset(اندازه(*tlb_q));
		اگر (!tlb_fifo_off) {
			sbi_scratch_free_offset(tlb_sync_off);
			بازگشت SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_offset(اندازه(tlb_mem));
		اگر (!tlb_fifo_mem_off) {
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			بازگشت SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		اگر (ret < 0) {
			sbi_scratch_free_offset(tlb_fifo_mem_off);
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			بازگشت ret;
		}
		tlb_event = ret;
		tlb_range_flush_limit = sbi_platform_tlbr_flush_limit(plat);
	} وگرنه {
		اگر (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			بازگشت SBI_ENOMEM;
		اگر (SBI_IPI_EVENT_MAX <= tlb_event)
			بازگشت SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_offset_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, تهی *, tlb_fifo_mem_off);
	اگر (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_رقم_entries(plat) * SBI_TLB_INFO_SIZE);
		اگر (!tlb_mem)
			بازگشت SBI_ENOMEM;
		sbi_scratch_write_type(scratch, تهی *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_رقم_entries(plat), SBI_TLB_INFO_SIZE);

	بازگشت 0;
}
