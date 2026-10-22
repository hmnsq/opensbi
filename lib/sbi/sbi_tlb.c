/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_atomic.h>
#अन्तर्भाव <sbi/riscv_barrier.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_fifo.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_ipi.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_tlb.h>
#अन्तर्भाव <sbi/sbi_hfence.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_pmu.h>

स्थिर अचिह्नित दीर्घ tlb_sync_off;
स्थिर अचिह्नित दीर्घ tlb_fifo_off;
स्थिर अचिह्नित दीर्घ tlb_fifo_mem_off;
स्थिर अचिह्नित दीर्घ tlb_range_flush_सीमा;

स्थिर शून्य sbi_tlb_local_fence_i(संरचना sbi_tlb_info *tinfo);
स्थिर शून्य sbi_tlb_local_sfence_vma(संरचना sbi_tlb_info *tinfo);
स्थिर शून्य sbi_tlb_local_sfence_vma_asid(संरचना sbi_tlb_info *tinfo);
स्थिर शून्य sbi_tlb_local_hfence_gvma_vmid(संरचना sbi_tlb_info *tinfo);
स्थिर शून्य sbi_tlb_local_hfence_gvma(संरचना sbi_tlb_info *tinfo);
स्थिर शून्य sbi_tlb_local_hfence_vvma_asid(संरचना sbi_tlb_info *tinfo);
स्थिर शून्य sbi_tlb_local_hfence_vvma(संरचना sbi_tlb_info *tinfo);

स्थिर संरचना sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

स्थिर ध्रुव संरचना sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

ध्रुव संरचना sbi_tlb_local_operations *sbi_tlb_get_local_operations(शून्य)
{
	प्रत्यागम tlb_local_ops;
}

शून्य sbi_tlb_समुच्चयः_local_operations(
		ध्रुव संरचना sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

शून्य __sbi_sfence_vma_all(शून्य)
{
	__asm__ __volatile("sfence.vma");
}

स्थिर शून्य sbi_tlb_local_hfence_vvma(संरचना sbi_tlb_info *tinfo)
{
	अचिह्नित दीर्घ start = tinfo->start;
	अचिह्नित दीर्घ आकार  = tinfo->आकार;
	अचिह्नित दीर्घ vmid  = tinfo->vmid;
	अचिह्नित दीर्घ i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_सरणम्) & HGATP_VMID_आवरणम्);

	यदि ((start == 0 && आकार == 0) || (आकार == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		गच्छ समाप्तम्;
	}

	प्रति (i = 0; i < आकार; i += PAGE_आकार) {
		__sbi_hfence_vvma_va(start+i);
	}

समाप्तम्:
	csr_write(CSR_HGATP, hgatp);
}

स्थिर शून्य sbi_tlb_local_hfence_gvma(संरचना sbi_tlb_info *tinfo)
{
	अचिह्नित दीर्घ start = tinfo->start;
	अचिह्नित दीर्घ आकार  = tinfo->आकार;
	अचिह्नित दीर्घ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	यदि ((start == 0 && आकार == 0) || (आकार == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		प्रत्यागम;
	}

	प्रति (i = 0; i < आकार; i += PAGE_आकार) {
		__sbi_hfence_gvma_gpa((start + i) >> 2);
	}
}

स्थिर शून्य sbi_tlb_local_sfence_vma(संरचना sbi_tlb_info *tinfo)
{
	अचिह्नित दीर्घ start = tinfo->start;
	अचिह्नित दीर्घ आकार  = tinfo->आकार;
	अचिह्नित दीर्घ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	यदि ((start == 0 && आकार == 0) || (आकार == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		प्रत्यागम;
	}

	प्रति (i = 0; i < आकार; i += PAGE_आकार) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(start + i)
				     : "memory");
	}
}

स्थिर शून्य sbi_tlb_local_hfence_vvma_asid(संरचना sbi_tlb_info *tinfo)
{
	अचिह्नित दीर्घ start = tinfo->start;
	अचिह्नित दीर्घ आकार  = tinfo->आकार;
	अचिह्नित दीर्घ asid  = tinfo->asid;
	अचिह्नित दीर्घ vmid  = tinfo->vmid;
	अचिह्नित दीर्घ i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_सरणम्) & HGATP_VMID_आवरणम्);

	यदि ((start == 0 && आकार == 0) || (आकार == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		गच्छ समाप्तम्;
	}

	प्रति (i = 0; i < आकार; i += PAGE_आकार) {
		__sbi_hfence_vvma_asid_va(start + i, asid);
	}

समाप्तम्:
	csr_write(CSR_HGATP, hgatp);
}

स्थिर शून्य sbi_tlb_local_hfence_gvma_vmid(संरचना sbi_tlb_info *tinfo)
{
	अचिह्नित दीर्घ start = tinfo->start;
	अचिह्नित दीर्घ आकार  = tinfo->आकार;
	अचिह्नित दीर्घ vmid  = tinfo->vmid;
	अचिह्नित दीर्घ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	यदि ((start == 0 && आकार == 0) || (आकार == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		प्रत्यागम;
	}

	प्रति (i = 0; i < आकार; i += PAGE_आकार) {
		__sbi_hfence_gvma_vmid_gpa((start + i) >> 2, vmid);
	}
}

स्थिर शून्य sbi_tlb_local_sfence_vma_asid(संरचना sbi_tlb_info *tinfo)
{
	अचिह्नित दीर्घ start = tinfo->start;
	अचिह्नित दीर्घ आकार  = tinfo->आकार;
	अचिह्नित दीर्घ asid  = tinfo->asid;
	अचिह्नित दीर्घ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	यदि ((start == 0 && आकार == 0) || (आकार == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		प्रत्यागम;
	}

	प्रति (i = 0; i < आकार; i += PAGE_आकार) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(start + i), "r"(asid)
				     : "memory");
	}
}

स्थिर शून्य sbi_tlb_local_fence_i(संरचना sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

स्थिर शून्य tlb_प्रविष्टिः_local_process(संरचना sbi_tlb_info *दत्त)
{
	यदि (unlikely(!दत्त))
		प्रत्यागम;

	विकल्प (दत्त->type) {
	पक्ष SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(दत्त);
		भङ्ग;
	पक्ष SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(दत्त);
		भङ्ग;
	पक्ष SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(दत्त);
		भङ्ग;
	पक्ष SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(दत्त);
		भङ्ग;
	पक्ष SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(दत्त);
		भङ्ग;
	पक्ष SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(दत्त);
		भङ्ग;
	पक्ष SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(दत्त);
		भङ्ग;
	स्वभाव:
		भङ्ग;
	};
}

स्थिर शून्य tlb_प्रविष्टिः_process(संरचना sbi_tlb_info *tinfo)
{
	u32 rindex;
	संरचना sbi_scratch *rscratch = रिक्त;
	atomic_t *rtlb_sync = रिक्त;

	tlb_प्रविष्टिः_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_गन्तव्यम्_scratch(rindex);
		यदि (!rscratch)
			अनुवर्त;

		rtlb_sync = sbi_scratch_विस्थापनम्_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

स्थिर bool tlb_process_once(संरचना sbi_scratch *scratch)
{
	संरचना sbi_tlb_info tinfo;
	संरचना sbi_fifo *tlb_fifo =
			sbi_scratch_विस्थापनम्_ptr(scratch, tlb_fifo_off);

	यदि (!sbi_fifo_पङ्क्तेः_निष्कासय(tlb_fifo, &tinfo)) {
		tlb_प्रविष्टिः_process(&tinfo);
		प्रत्यागम true;
	}

	प्रत्यागम false;
}

स्थिर शून्य tlb_process(संरचना sbi_scratch *scratch)
{
	यावत् (tlb_process_once(scratch));
}

स्थिर शून्य tlb_sync(संरचना sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_विस्थापनम्_ptr(scratch, tlb_sync_off);

	यावत् (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	प्रत्यागम;
}

स्थिर अन्तःस्थ पूर्णाङ्क tlb_range_check(संरचना sbi_tlb_info *curr,
					संरचना sbi_tlb_info *अग्र)
{
	अचिह्नित दीर्घ curr_अन्तः;
	अचिह्नित दीर्घ अग्र_अन्तः;
	पूर्णाङ्क ret = SBI_FIFO_UNCHANGED;

	यदि (!curr || !अग्र)
		प्रत्यागम ret;

	अग्र_अन्तः = अग्र->start + अग्र->आकार;
	curr_अन्तः = curr->start + curr->आकार;
	यदि (अग्र->start <= curr->start && अग्र_अन्तः > curr_अन्तः) {
		curr->start = अग्र->start;
		curr->आकार  = अग्र->आकार;
		sbi_hartmask_or(&curr->smask, &curr->smask, &अग्र->smask);
		ret = SBI_FIFO_UPDATED;
	} अन्यथा यदि (अग्र->start >= curr->start && अग्र_अन्तः <= curr_अन्तः) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &अग्र->smask);
		ret = SBI_FIFO_SKIP;
	}

	प्रत्यागम ret;
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
स्थिर पूर्णाङ्क tlb_update_cb(शून्य *अन्तर्, शून्य *दत्त)
{
	संरचना sbi_tlb_info *curr;
	संरचना sbi_tlb_info *अग्र;
	पूर्णाङ्क ret = SBI_FIFO_UNCHANGED;

	यदि (!अन्तर् || !दत्त)
		प्रत्यागम ret;

	curr = (संरचना sbi_tlb_info *)दत्त;
	अग्र = (संरचना sbi_tlb_info *)अन्तर्;

	यदि (अग्र->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		यदि (अग्र->asid == curr->asid)
			ret = tlb_range_check(curr, अग्र);
	} अन्यथा यदि (अग्र->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, अग्र);
	}

	प्रत्यागम ret;
}

स्थिर पूर्णाङ्क tlb_update(संरचना sbi_scratch *scratch,
			  संरचना sbi_scratch *remote_scratch,
			  u32 remote_hartindex, शून्य *दत्त)
{
	पूर्णाङ्क ret;
	atomic_t *tlb_sync;
	संरचना sbi_fifo *tlb_fifo_r;
	संरचना sbi_tlb_info *tinfo = दत्त;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	यदि (sbi_hartindex_गन्तव्यम्_hartid(remote_hartindex) == curr_hartid) {
		tlb_प्रविष्टिः_local_process(tinfo);
		प्रत्यागम SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_विस्थापनम्_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, दत्त, tlb_update_cb);

	यदि (ret == SBI_FIFO_UNCHANGED &&
	    sbi_fifo_पङ्क्तौ_निक्षिप(tlb_fifo_r, दत्त, false) < 0) {
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
			    sbi_hartindex_गन्तव्यम्_hartid(remote_hartindex));
		प्रत्यागम SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_विस्थापनम्_ptr(scratch, tlb_sync_off);
	atomic_सङ्कलय_return(tlb_sync, 1);

	प्रत्यागम SBI_IPI_UPDATE_SUCCESS;
}

स्थिर संरचना sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

स्थिर u32 tlb_event = SBI_IPI_EVENT_MAX;

स्थिर ध्रुव u32 tlb_type_गन्तव्यम्_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_प्रेषितम्,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_प्रेषितम्,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_प्रेषितम्,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_प्रेषितम्,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_प्रेषितम्,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_प्रेषितम्,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_प्रेषितम्,
};

पूर्णाङ्क sbi_tlb_request(ulong hmask, ulong hbase, संरचना sbi_tlb_info *tinfo)
{
	यदि (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		प्रत्यागम SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	यदि (tinfo->आकार > tlb_range_flush_सीमा) {
		tinfo->start = 0;
		tinfo->आकार = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_गन्तव्यम्_pmu_fw_event[tinfo->type]);

	प्रत्यागम sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

पूर्णाङ्क sbi_tlb_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	पूर्णाङ्क ret;
	शून्य *tlb_mem;
	atomic_t *tlb_sync;
	संरचना sbi_fifo *tlb_q;
	ध्रुव संरचना sbi_platform *plat = sbi_platform_ptr(scratch);

	यदि (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_विस्थापनम्(परिमाण(*tlb_sync));
		यदि (!tlb_sync_off)
			प्रत्यागम SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_विस्थापनम्(परिमाण(*tlb_q));
		यदि (!tlb_fifo_off) {
			sbi_scratch_free_विस्थापनम्(tlb_sync_off);
			प्रत्यागम SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_विस्थापनम्(परिमाण(tlb_mem));
		यदि (!tlb_fifo_mem_off) {
			sbi_scratch_free_विस्थापनम्(tlb_fifo_off);
			sbi_scratch_free_विस्थापनम्(tlb_sync_off);
			प्रत्यागम SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		यदि (ret < 0) {
			sbi_scratch_free_विस्थापनम्(tlb_fifo_mem_off);
			sbi_scratch_free_विस्थापनम्(tlb_fifo_off);
			sbi_scratch_free_विस्थापनम्(tlb_sync_off);
			प्रत्यागम ret;
		}
		tlb_event = ret;
		tlb_range_flush_सीमा = sbi_platform_tlbr_flush_सीमा(plat);
	} अन्यथा {
		यदि (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			प्रत्यागम SBI_ENOMEM;
		यदि (SBI_IPI_EVENT_MAX <= tlb_event)
			प्रत्यागम SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_विस्थापनम्_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_विस्थापनम्_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, शून्य *, tlb_fifo_mem_off);
	यदि (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_अङ्कः_entries(plat) * SBI_TLB_INFO_आकार);
		यदि (!tlb_mem)
			प्रत्यागम SBI_ENOMEM;
		sbi_scratch_write_type(scratch, शून्य *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_अङ्कः_entries(plat), SBI_TLB_INFO_आकार);

	प्रत्यागम 0;
}
