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

ساکن غیرعلامتی طویل tlb_sync_off;
ساکن غیرعلامتی طویل tlb_fifo_off;
ساکن غیرعلامتی طویل tlb_fifo_mem_off;
ساکن غیرعلامتی طویل tlb_range_flush_حد_بندی;

ساکن خالی sbi_tlb_local_fence_i(ساخت sbi_tlb_info *tinfo);
ساکن خالی sbi_tlb_local_sfence_vma(ساخت sbi_tlb_info *tinfo);
ساکن خالی sbi_tlb_local_sfence_vma_asid(ساخت sbi_tlb_info *tinfo);
ساکن خالی sbi_tlb_local_hfence_gvma_vmid(ساخت sbi_tlb_info *tinfo);
ساکن خالی sbi_tlb_local_hfence_gvma(ساخت sbi_tlb_info *tinfo);
ساکن خالی sbi_tlb_local_hfence_vvma_asid(ساخت sbi_tlb_info *tinfo);
ساکن خالی sbi_tlb_local_hfence_vvma(ساخت sbi_tlb_info *tinfo);

ساکن ساخت sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

ساکن ثابت ساخت sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

ثابت ساخت sbi_tlb_local_operations *sbi_tlb_get_local_operations(خالی)
{
	واپس tlb_local_ops;
}

خالی sbi_tlb_سیٹ_local_operations(
		ثابت ساخت sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

خالی __sbi_sfence_vma_all(خالی)
{
	__asm__ __volatile("sfence.vma");
}

ساکن خالی sbi_tlb_local_hfence_vvma(ساخت sbi_tlb_info *tinfo)
{
	غیرعلامتی طویل آغاز = tinfo->آغاز;
	غیرعلامتی طویل سائز  = tinfo->سائز;
	غیرعلامتی طویل vmid  = tinfo->vmid;
	غیرعلامتی طویل i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_تبادلہ(CSR_HGATP,
			 (vmid << HGATP_VMID_سرکاؤ) & HGATP_VMID_نقاب);

	اگر ((آغاز == 0 && سائز == 0) || (سائز == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		جاؤ مکمل;
	}

	برائے (i = 0; i < سائز; i += صفحہ_سائز) {
		__sbi_hfence_vvma_va(آغاز+i);
	}

مکمل:
	csr_write(CSR_HGATP, hgatp);
}

ساکن خالی sbi_tlb_local_hfence_gvma(ساخت sbi_tlb_info *tinfo)
{
	غیرعلامتی طویل آغاز = tinfo->آغاز;
	غیرعلامتی طویل سائز  = tinfo->سائز;
	غیرعلامتی طویل i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	اگر ((آغاز == 0 && سائز == 0) || (سائز == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		واپس;
	}

	برائے (i = 0; i < سائز; i += صفحہ_سائز) {
		__sbi_hfence_gvma_gpa((آغاز + i) >> 2);
	}
}

ساکن خالی sbi_tlb_local_sfence_vma(ساخت sbi_tlb_info *tinfo)
{
	غیرعلامتی طویل آغاز = tinfo->آغاز;
	غیرعلامتی طویل سائز  = tinfo->سائز;
	غیرعلامتی طویل i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	اگر ((آغاز == 0 && سائز == 0) || (سائز == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		واپس;
	}

	برائے (i = 0; i < سائز; i += صفحہ_سائز) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(آغاز + i)
				     : "memory");
	}
}

ساکن خالی sbi_tlb_local_hfence_vvma_asid(ساخت sbi_tlb_info *tinfo)
{
	غیرعلامتی طویل آغاز = tinfo->آغاز;
	غیرعلامتی طویل سائز  = tinfo->سائز;
	غیرعلامتی طویل asid  = tinfo->asid;
	غیرعلامتی طویل vmid  = tinfo->vmid;
	غیرعلامتی طویل i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_تبادلہ(CSR_HGATP,
			 (vmid << HGATP_VMID_سرکاؤ) & HGATP_VMID_نقاب);

	اگر ((آغاز == 0 && سائز == 0) || (سائز == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		جاؤ مکمل;
	}

	برائے (i = 0; i < سائز; i += صفحہ_سائز) {
		__sbi_hfence_vvma_asid_va(آغاز + i, asid);
	}

مکمل:
	csr_write(CSR_HGATP, hgatp);
}

ساکن خالی sbi_tlb_local_hfence_gvma_vmid(ساخت sbi_tlb_info *tinfo)
{
	غیرعلامتی طویل آغاز = tinfo->آغاز;
	غیرعلامتی طویل سائز  = tinfo->سائز;
	غیرعلامتی طویل vmid  = tinfo->vmid;
	غیرعلامتی طویل i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	اگر ((آغاز == 0 && سائز == 0) || (سائز == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		واپس;
	}

	برائے (i = 0; i < سائز; i += صفحہ_سائز) {
		__sbi_hfence_gvma_vmid_gpa((آغاز + i) >> 2, vmid);
	}
}

ساکن خالی sbi_tlb_local_sfence_vma_asid(ساخت sbi_tlb_info *tinfo)
{
	غیرعلامتی طویل آغاز = tinfo->آغاز;
	غیرعلامتی طویل سائز  = tinfo->سائز;
	غیرعلامتی طویل asid  = tinfo->asid;
	غیرعلامتی طویل i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	اگر ((آغاز == 0 && سائز == 0) || (سائز == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		واپس;
	}

	برائے (i = 0; i < سائز; i += صفحہ_سائز) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(آغاز + i), "r"(asid)
				     : "memory");
	}
}

ساکن خالی sbi_tlb_local_fence_i(ساخت sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

ساکن خالی tlb_اندراج_local_process(ساخت sbi_tlb_info *ڈیٹا)
{
	اگر (unlikely(!ڈیٹا))
		واپس;

	انتخاب (ڈیٹا->type) {
	حالت SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(ڈیٹا);
		توڑو;
	حالت SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(ڈیٹا);
		توڑو;
	حالت SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(ڈیٹا);
		توڑو;
	حالت SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(ڈیٹا);
		توڑو;
	حالت SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(ڈیٹا);
		توڑو;
	حالت SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(ڈیٹا);
		توڑو;
	حالت SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(ڈیٹا);
		توڑو;
	طےشدہ:
		توڑو;
	};
}

ساکن خالی tlb_اندراج_process(ساخت sbi_tlb_info *tinfo)
{
	u32 rindex;
	ساخت sbi_scratch *rscratch = تہی;
	atomic_t *rtlb_sync = تہی;

	tlb_اندراج_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_مقصد_scratch(rindex);
		اگر (!rscratch)
			جاری;

		rtlb_sync = sbi_scratch_انحراف_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

ساکن bool tlb_process_once(ساخت sbi_scratch *scratch)
{
	ساخت sbi_tlb_info tinfo;
	ساخت sbi_fifo *tlb_fifo =
			sbi_scratch_انحراف_ptr(scratch, tlb_fifo_off);

	اگر (!sbi_fifo_قطار_سے_نکالو(tlb_fifo, &tinfo)) {
		tlb_اندراج_process(&tinfo);
		واپس true;
	}

	واپس false;
}

ساکن خالی tlb_process(ساخت sbi_scratch *scratch)
{
	جب (tlb_process_once(scratch));
}

ساکن خالی tlb_sync(ساخت sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_انحراف_ptr(scratch, tlb_sync_off);

	جب (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	واپس;
}

ساکن اندرونی صحیح tlb_range_check(ساخت sbi_tlb_info *curr,
					ساخت sbi_tlb_info *اگلا)
{
	غیرعلامتی طویل curr_انتہا;
	غیرعلامتی طویل اگلا_انتہا;
	صحیح ret = SBI_FIFO_UNCHANGED;

	اگر (!curr || !اگلا)
		واپس ret;

	اگلا_انتہا = اگلا->آغاز + اگلا->سائز;
	curr_انتہا = curr->آغاز + curr->سائز;
	اگر (اگلا->آغاز <= curr->آغاز && اگلا_انتہا > curr_انتہا) {
		curr->آغاز = اگلا->آغاز;
		curr->سائز  = اگلا->سائز;
		sbi_hartmask_or(&curr->smask, &curr->smask, &اگلا->smask);
		ret = SBI_FIFO_UPDATED;
	} ورنہ اگر (اگلا->آغاز >= curr->آغاز && اگلا_انتہا <= curr_انتہا) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &اگلا->smask);
		ret = SBI_FIFO_SKIP;
	}

	واپس ret;
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
ساکن صحیح tlb_update_cb(خالی *اندر, خالی *ڈیٹا)
{
	ساخت sbi_tlb_info *curr;
	ساخت sbi_tlb_info *اگلا;
	صحیح ret = SBI_FIFO_UNCHANGED;

	اگر (!اندر || !ڈیٹا)
		واپس ret;

	curr = (ساخت sbi_tlb_info *)ڈیٹا;
	اگلا = (ساخت sbi_tlb_info *)اندر;

	اگر (اگلا->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		اگر (اگلا->asid == curr->asid)
			ret = tlb_range_check(curr, اگلا);
	} ورنہ اگر (اگلا->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, اگلا);
	}

	واپس ret;
}

ساکن صحیح tlb_update(ساخت sbi_scratch *scratch,
			  ساخت sbi_scratch *remote_scratch,
			  u32 remote_hartindex, خالی *ڈیٹا)
{
	صحیح ret;
	atomic_t *tlb_sync;
	ساخت sbi_fifo *tlb_fifo_r;
	ساخت sbi_tlb_info *tinfo = ڈیٹا;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	اگر (sbi_hartindex_مقصد_hartid(remote_hartindex) == curr_hartid) {
		tlb_اندراج_local_process(tinfo);
		واپس SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_انحراف_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, ڈیٹا, tlb_update_cb);

	اگر (ret == SBI_FIFO_UNCHANGED &&
	    sbi_fifo_قطار_میں_ڈالو(tlb_fifo_r, ڈیٹا, false) < 0) {
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
			    sbi_hartindex_مقصد_hartid(remote_hartindex));
		واپس SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_انحراف_ptr(scratch, tlb_sync_off);
	atomic_جوڑو_return(tlb_sync, 1);

	واپس SBI_IPI_UPDATE_SUCCESS;
}

ساکن ساخت sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

ساکن u32 tlb_event = SBI_IPI_EVENT_MAX;

ساکن ثابت u32 tlb_type_مقصد_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_بھیجا,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_بھیجا,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_بھیجا,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_بھیجا,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_بھیجا,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_بھیجا,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_بھیجا,
};

صحیح sbi_tlb_request(ulong hmask, ulong hbase, ساخت sbi_tlb_info *tinfo)
{
	اگر (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		واپس SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	اگر (tinfo->سائز > tlb_range_flush_حد_بندی) {
		tinfo->آغاز = 0;
		tinfo->سائز = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_مقصد_pmu_fw_event[tinfo->type]);

	واپس sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

صحیح sbi_tlb_init(ساخت sbi_scratch *scratch, bool cold_boot)
{
	صحیح ret;
	خالی *tlb_mem;
	atomic_t *tlb_sync;
	ساخت sbi_fifo *tlb_q;
	ثابت ساخت sbi_platform *plat = sbi_platform_ptr(scratch);

	اگر (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_انحراف(حجم(*tlb_sync));
		اگر (!tlb_sync_off)
			واپس SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_انحراف(حجم(*tlb_q));
		اگر (!tlb_fifo_off) {
			sbi_scratch_free_انحراف(tlb_sync_off);
			واپس SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_انحراف(حجم(tlb_mem));
		اگر (!tlb_fifo_mem_off) {
			sbi_scratch_free_انحراف(tlb_fifo_off);
			sbi_scratch_free_انحراف(tlb_sync_off);
			واپس SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		اگر (ret < 0) {
			sbi_scratch_free_انحراف(tlb_fifo_mem_off);
			sbi_scratch_free_انحراف(tlb_fifo_off);
			sbi_scratch_free_انحراف(tlb_sync_off);
			واپس ret;
		}
		tlb_event = ret;
		tlb_range_flush_حد_بندی = sbi_platform_tlbr_flush_حد_بندی(plat);
	} ورنہ {
		اگر (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			واپس SBI_ENOMEM;
		اگر (SBI_IPI_EVENT_MAX <= tlb_event)
			واپس SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_انحراف_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_انحراف_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, خالی *, tlb_fifo_mem_off);
	اگر (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_عدد_entries(plat) * SBI_TLB_INFO_سائز);
		اگر (!tlb_mem)
			واپس SBI_ENOMEM;
		sbi_scratch_write_type(scratch, خالی *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_عدد_entries(plat), SBI_TLB_INFO_سائز);

	واپس 0;
}
