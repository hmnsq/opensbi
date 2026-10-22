/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_atomic.h>
#ضمن <sbi/riscv_barrier.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_fifo.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_ipi.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_tlb.h>
#ضمن <sbi/sbi_hfence.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_pmu.h>

ساكن بیعلامت دراز tlb_sync_off;
ساكن بیعلامت دراز tlb_fifo_off;
ساكن بیعلامت دراز tlb_fifo_mem_off;
ساكن بیعلامت دراز tlb_range_flush_حد;

ساكن فراغ sbi_tlb_local_fence_i(بنية sbi_tlb_info *tinfo);
ساكن فراغ sbi_tlb_local_sfence_vma(بنية sbi_tlb_info *tinfo);
ساكن فراغ sbi_tlb_local_sfence_vma_asid(بنية sbi_tlb_info *tinfo);
ساكن فراغ sbi_tlb_local_hfence_gvma_vmid(بنية sbi_tlb_info *tinfo);
ساكن فراغ sbi_tlb_local_hfence_gvma(بنية sbi_tlb_info *tinfo);
ساكن فراغ sbi_tlb_local_hfence_vvma_asid(بنية sbi_tlb_info *tinfo);
ساكن فراغ sbi_tlb_local_hfence_vvma(بنية sbi_tlb_info *tinfo);

ساكن بنية sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

ساكن ثابت بنية sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

ثابت بنية sbi_tlb_local_operations *sbi_tlb_get_local_operations(فراغ)
{
	أرجع tlb_local_ops;
}

فراغ sbi_tlb_مجموعه_local_operations(
		ثابت بنية sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

فراغ __sbi_sfence_vma_all(فراغ)
{
	__asm__ __volatile("sfence.vma");
}

ساكن فراغ sbi_tlb_local_hfence_vvma(بنية sbi_tlb_info *tinfo)
{
	بیعلامت دراز شروع = tinfo->شروع;
	بیعلامت دراز اندازه  = tinfo->اندازه;
	بیعلامت دراز vmid  = tinfo->vmid;
	بیعلامت دراز i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_جابجا(CSR_HGATP,
			 (vmid << HGATP_VMID_لغزش) & HGATP_VMID_ماسک);

	إذا ((شروع == 0 && اندازه == 0) || (اندازه == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		اذهب انجامشده;
	}

	لكل (i = 0; i < اندازه; i += صفحه_اندازه) {
		__sbi_hfence_vvma_va(شروع+i);
	}

انجامشده:
	csr_write(CSR_HGATP, hgatp);
}

ساكن فراغ sbi_tlb_local_hfence_gvma(بنية sbi_tlb_info *tinfo)
{
	بیعلامت دراز شروع = tinfo->شروع;
	بیعلامت دراز اندازه  = tinfo->اندازه;
	بیعلامت دراز i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	إذا ((شروع == 0 && اندازه == 0) || (اندازه == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		أرجع;
	}

	لكل (i = 0; i < اندازه; i += صفحه_اندازه) {
		__sbi_hfence_gvma_gpa((شروع + i) >> 2);
	}
}

ساكن فراغ sbi_tlb_local_sfence_vma(بنية sbi_tlb_info *tinfo)
{
	بیعلامت دراز شروع = tinfo->شروع;
	بیعلامت دراز اندازه  = tinfo->اندازه;
	بیعلامت دراز i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	إذا ((شروع == 0 && اندازه == 0) || (اندازه == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		أرجع;
	}

	لكل (i = 0; i < اندازه; i += صفحه_اندازه) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(شروع + i)
				     : "memory");
	}
}

ساكن فراغ sbi_tlb_local_hfence_vvma_asid(بنية sbi_tlb_info *tinfo)
{
	بیعلامت دراز شروع = tinfo->شروع;
	بیعلامت دراز اندازه  = tinfo->اندازه;
	بیعلامت دراز asid  = tinfo->asid;
	بیعلامت دراز vmid  = tinfo->vmid;
	بیعلامت دراز i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_جابجا(CSR_HGATP,
			 (vmid << HGATP_VMID_لغزش) & HGATP_VMID_ماسک);

	إذا ((شروع == 0 && اندازه == 0) || (اندازه == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		اذهب انجامشده;
	}

	لكل (i = 0; i < اندازه; i += صفحه_اندازه) {
		__sbi_hfence_vvma_asid_va(شروع + i, asid);
	}

انجامشده:
	csr_write(CSR_HGATP, hgatp);
}

ساكن فراغ sbi_tlb_local_hfence_gvma_vmid(بنية sbi_tlb_info *tinfo)
{
	بیعلامت دراز شروع = tinfo->شروع;
	بیعلامت دراز اندازه  = tinfo->اندازه;
	بیعلامت دراز vmid  = tinfo->vmid;
	بیعلامت دراز i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	إذا ((شروع == 0 && اندازه == 0) || (اندازه == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		أرجع;
	}

	لكل (i = 0; i < اندازه; i += صفحه_اندازه) {
		__sbi_hfence_gvma_vmid_gpa((شروع + i) >> 2, vmid);
	}
}

ساكن فراغ sbi_tlb_local_sfence_vma_asid(بنية sbi_tlb_info *tinfo)
{
	بیعلامت دراز شروع = tinfo->شروع;
	بیعلامت دراز اندازه  = tinfo->اندازه;
	بیعلامت دراز asid  = tinfo->asid;
	بیعلامت دراز i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	إذا ((شروع == 0 && اندازه == 0) || (اندازه == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		أرجع;
	}

	لكل (i = 0; i < اندازه; i += صفحه_اندازه) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(شروع + i), "r"(asid)
				     : "memory");
	}
}

ساكن فراغ sbi_tlb_local_fence_i(بنية sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

ساكن فراغ tlb_مدخل_local_process(بنية sbi_tlb_info *داده)
{
	إذا (unlikely(!داده))
		أرجع;

	بدل (داده->type) {
	حالة SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(داده);
		اكسر;
	حالة SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(داده);
		اكسر;
	حالة SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(داده);
		اكسر;
	حالة SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(داده);
		اكسر;
	حالة SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(داده);
		اكسر;
	حالة SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(داده);
		اكسر;
	حالة SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(داده);
		اكسر;
	افتراضي:
		اكسر;
	};
}

ساكن فراغ tlb_مدخل_process(بنية sbi_tlb_info *tinfo)
{
	u32 rindex;
	بنية sbi_scratch *rscratch = عدم;
	atomic_t *rtlb_sync = عدم;

	tlb_مدخل_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_مقصد_scratch(rindex);
		إذا (!rscratch)
			استمر;

		rtlb_sync = sbi_scratch_جابه_جایی_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

ساكن bool tlb_process_once(بنية sbi_scratch *scratch)
{
	بنية sbi_tlb_info tinfo;
	بنية sbi_fifo *tlb_fifo =
			sbi_scratch_جابه_جایی_ptr(scratch, tlb_fifo_off);

	إذا (!sbi_fifo_خارج_از_صف(tlb_fifo, &tinfo)) {
		tlb_مدخل_process(&tinfo);
		أرجع true;
	}

	أرجع false;
}

ساكن فراغ tlb_process(بنية sbi_scratch *scratch)
{
	طالما (tlb_process_once(scratch));
}

ساكن فراغ tlb_sync(بنية sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_جابه_جایی_ptr(scratch, tlb_sync_off);

	طالما (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	أرجع;
}

ساكن مضمن صحيح tlb_range_check(بنية sbi_tlb_info *curr,
					بنية sbi_tlb_info *بعدی)
{
	بیعلامت دراز curr_انتها;
	بیعلامت دراز بعدی_انتها;
	صحيح ret = SBI_FIFO_UNCHANGED;

	إذا (!curr || !بعدی)
		أرجع ret;

	بعدی_انتها = بعدی->شروع + بعدی->اندازه;
	curr_انتها = curr->شروع + curr->اندازه;
	إذا (بعدی->شروع <= curr->شروع && بعدی_انتها > curr_انتها) {
		curr->شروع = بعدی->شروع;
		curr->اندازه  = بعدی->اندازه;
		sbi_hartmask_or(&curr->smask, &curr->smask, &بعدی->smask);
		ret = SBI_FIFO_UPDATED;
	} وإلا إذا (بعدی->شروع >= curr->شروع && بعدی_انتها <= curr_انتها) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &بعدی->smask);
		ret = SBI_FIFO_SKIP;
	}

	أرجع ret;
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
ساكن صحيح tlb_update_cb(فراغ *درون, فراغ *داده)
{
	بنية sbi_tlb_info *curr;
	بنية sbi_tlb_info *بعدی;
	صحيح ret = SBI_FIFO_UNCHANGED;

	إذا (!درون || !داده)
		أرجع ret;

	curr = (بنية sbi_tlb_info *)داده;
	بعدی = (بنية sbi_tlb_info *)درون;

	إذا (بعدی->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		إذا (بعدی->asid == curr->asid)
			ret = tlb_range_check(curr, بعدی);
	} وإلا إذا (بعدی->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, بعدی);
	}

	أرجع ret;
}

ساكن صحيح tlb_update(بنية sbi_scratch *scratch,
			  بنية sbi_scratch *remote_scratch,
			  u32 remote_hartindex, فراغ *داده)
{
	صحيح ret;
	atomic_t *tlb_sync;
	بنية sbi_fifo *tlb_fifo_r;
	بنية sbi_tlb_info *tinfo = داده;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	إذا (sbi_hartindex_مقصد_hartid(remote_hartindex) == curr_hartid) {
		tlb_مدخل_local_process(tinfo);
		أرجع SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_جابه_جایی_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, داده, tlb_update_cb);

	إذا (ret == SBI_FIFO_UNCHANGED &&
	    sbi_fifo_افزودن_به_صف(tlb_fifo_r, داده, false) < 0) {
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
		أرجع SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_جابه_جایی_ptr(scratch, tlb_sync_off);
	atomic_جمع_کن_return(tlb_sync, 1);

	أرجع SBI_IPI_UPDATE_SUCCESS;
}

ساكن بنية sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

ساكن u32 tlb_event = SBI_IPI_EVENT_MAX;

ساكن ثابت u32 tlb_type_مقصد_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_فرستاده,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_فرستاده,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_فرستاده,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_فرستاده,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_فرستاده,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_فرستاده,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_فرستاده,
};

صحيح sbi_tlb_request(ulong hmask, ulong hbase, بنية sbi_tlb_info *tinfo)
{
	إذا (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		أرجع SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	إذا (tinfo->اندازه > tlb_range_flush_حد) {
		tinfo->شروع = 0;
		tinfo->اندازه = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_مقصد_pmu_fw_event[tinfo->type]);

	أرجع sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

صحيح sbi_tlb_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	صحيح ret;
	فراغ *tlb_mem;
	atomic_t *tlb_sync;
	بنية sbi_fifo *tlb_q;
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);

	إذا (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_جابه_جایی(حجم(*tlb_sync));
		إذا (!tlb_sync_off)
			أرجع SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_جابه_جایی(حجم(*tlb_q));
		إذا (!tlb_fifo_off) {
			sbi_scratch_free_جابه_جایی(tlb_sync_off);
			أرجع SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_جابه_جایی(حجم(tlb_mem));
		إذا (!tlb_fifo_mem_off) {
			sbi_scratch_free_جابه_جایی(tlb_fifo_off);
			sbi_scratch_free_جابه_جایی(tlb_sync_off);
			أرجع SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		إذا (ret < 0) {
			sbi_scratch_free_جابه_جایی(tlb_fifo_mem_off);
			sbi_scratch_free_جابه_جایی(tlb_fifo_off);
			sbi_scratch_free_جابه_جایی(tlb_sync_off);
			أرجع ret;
		}
		tlb_event = ret;
		tlb_range_flush_حد = sbi_platform_tlbr_flush_حد(plat);
	} وإلا {
		إذا (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			أرجع SBI_ENOMEM;
		إذا (SBI_IPI_EVENT_MAX <= tlb_event)
			أرجع SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_جابه_جایی_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_جابه_جایی_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, فراغ *, tlb_fifo_mem_off);
	إذا (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_رقم_entries(plat) * SBI_TLB_INFO_اندازه);
		إذا (!tlb_mem)
			أرجع SBI_ENOMEM;
		sbi_scratch_write_type(scratch, فراغ *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_رقم_entries(plat), SBI_TLB_INFO_اندازه);

	أرجع 0;
}
