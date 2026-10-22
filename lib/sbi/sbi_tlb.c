/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_atomic.h>
#समावेश <sbi/riscv_barrier.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_fifo.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_ipi.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_tlb.h>
#समावेश <sbi/sbi_hfence.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_pmu.h>

अटल अचिन्हित दीर्घ tlb_sync_off;
अटल अचिन्हित दीर्घ tlb_fifo_off;
अटल अचिन्हित दीर्घ tlb_fifo_mem_off;
अटल अचिन्हित दीर्घ tlb_range_flush_limit;

अटल व्योम sbi_tlb_local_fence_i(काष्ठा sbi_tlb_info *tinfo);
अटल व्योम sbi_tlb_local_sfence_vma(काष्ठा sbi_tlb_info *tinfo);
अटल व्योम sbi_tlb_local_sfence_vma_asid(काष्ठा sbi_tlb_info *tinfo);
अटल व्योम sbi_tlb_local_hfence_gvma_vmid(काष्ठा sbi_tlb_info *tinfo);
अटल व्योम sbi_tlb_local_hfence_gvma(काष्ठा sbi_tlb_info *tinfo);
अटल व्योम sbi_tlb_local_hfence_vvma_asid(काष्ठा sbi_tlb_info *tinfo);
अटल व्योम sbi_tlb_local_hfence_vvma(काष्ठा sbi_tlb_info *tinfo);

अटल काष्ठा sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

अटल स्थिर काष्ठा sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

स्थिर काष्ठा sbi_tlb_local_operations *sbi_tlb_get_local_operations(व्योम)
{
	वापस tlb_local_ops;
}

व्योम sbi_tlb_set_local_operations(
		स्थिर काष्ठा sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

व्योम __sbi_sfence_vma_all(व्योम)
{
	__asm__ __volatile("sfence.vma");
}

अटल व्योम sbi_tlb_local_hfence_vvma(काष्ठा sbi_tlb_info *tinfo)
{
	अचिन्हित दीर्घ start = tinfo->start;
	अचिन्हित दीर्घ आकार  = tinfo->आकार;
	अचिन्हित दीर्घ vmid  = tinfo->vmid;
	अचिन्हित दीर्घ i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	अगर ((start == 0 && आकार == 0) || (आकार == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		जाओ done;
	}

	क्रम (i = 0; i < आकार; i += PAGE_आकार) {
		__sbi_hfence_vvma_va(start+i);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

अटल व्योम sbi_tlb_local_hfence_gvma(काष्ठा sbi_tlb_info *tinfo)
{
	अचिन्हित दीर्घ start = tinfo->start;
	अचिन्हित दीर्घ आकार  = tinfo->आकार;
	अचिन्हित दीर्घ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	अगर ((start == 0 && आकार == 0) || (आकार == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		वापस;
	}

	क्रम (i = 0; i < आकार; i += PAGE_आकार) {
		__sbi_hfence_gvma_gpa((start + i) >> 2);
	}
}

अटल व्योम sbi_tlb_local_sfence_vma(काष्ठा sbi_tlb_info *tinfo)
{
	अचिन्हित दीर्घ start = tinfo->start;
	अचिन्हित दीर्घ आकार  = tinfo->आकार;
	अचिन्हित दीर्घ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	अगर ((start == 0 && आकार == 0) || (आकार == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		वापस;
	}

	क्रम (i = 0; i < आकार; i += PAGE_आकार) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(start + i)
				     : "memory");
	}
}

अटल व्योम sbi_tlb_local_hfence_vvma_asid(काष्ठा sbi_tlb_info *tinfo)
{
	अचिन्हित दीर्घ start = tinfo->start;
	अचिन्हित दीर्घ आकार  = tinfo->आकार;
	अचिन्हित दीर्घ asid  = tinfo->asid;
	अचिन्हित दीर्घ vmid  = tinfo->vmid;
	अचिन्हित दीर्घ i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	अगर ((start == 0 && आकार == 0) || (आकार == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		जाओ done;
	}

	क्रम (i = 0; i < आकार; i += PAGE_आकार) {
		__sbi_hfence_vvma_asid_va(start + i, asid);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

अटल व्योम sbi_tlb_local_hfence_gvma_vmid(काष्ठा sbi_tlb_info *tinfo)
{
	अचिन्हित दीर्घ start = tinfo->start;
	अचिन्हित दीर्घ आकार  = tinfo->आकार;
	अचिन्हित दीर्घ vmid  = tinfo->vmid;
	अचिन्हित दीर्घ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	अगर ((start == 0 && आकार == 0) || (आकार == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		वापस;
	}

	क्रम (i = 0; i < आकार; i += PAGE_आकार) {
		__sbi_hfence_gvma_vmid_gpa((start + i) >> 2, vmid);
	}
}

अटल व्योम sbi_tlb_local_sfence_vma_asid(काष्ठा sbi_tlb_info *tinfo)
{
	अचिन्हित दीर्घ start = tinfo->start;
	अचिन्हित दीर्घ आकार  = tinfo->आकार;
	अचिन्हित दीर्घ asid  = tinfo->asid;
	अचिन्हित दीर्घ i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	अगर ((start == 0 && आकार == 0) || (आकार == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		वापस;
	}

	क्रम (i = 0; i < आकार; i += PAGE_आकार) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(start + i), "r"(asid)
				     : "memory");
	}
}

अटल व्योम sbi_tlb_local_fence_i(काष्ठा sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

अटल व्योम tlb_entry_local_process(काष्ठा sbi_tlb_info *तथ्य)
{
	अगर (unlikely(!तथ्य))
		वापस;

	चयन (तथ्य->type) {
	हाल SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(तथ्य);
		अवरोध;
	हाल SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(तथ्य);
		अवरोध;
	हाल SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(तथ्य);
		अवरोध;
	हाल SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(तथ्य);
		अवरोध;
	हाल SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(तथ्य);
		अवरोध;
	हाल SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(तथ्य);
		अवरोध;
	हाल SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(तथ्य);
		अवरोध;
	शेष:
		अवरोध;
	};
}

अटल व्योम tlb_entry_process(काष्ठा sbi_tlb_info *tinfo)
{
	u32 rindex;
	काष्ठा sbi_scratch *rscratch = शून्य;
	atomic_t *rtlb_sync = शून्य;

	tlb_entry_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_to_scratch(rindex);
		अगर (!rscratch)
			जारी;

		rtlb_sync = sbi_scratch_offset_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

अटल bool tlb_process_once(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_tlb_info tinfo;
	काष्ठा sbi_fifo *tlb_fifo =
			sbi_scratch_offset_ptr(scratch, tlb_fifo_off);

	अगर (!sbi_fifo_dequeue(tlb_fifo, &tinfo)) {
		tlb_entry_process(&tinfo);
		वापस true;
	}

	वापस false;
}

अटल व्योम tlb_process(काष्ठा sbi_scratch *scratch)
{
	जबतक (tlb_process_once(scratch));
}

अटल व्योम tlb_sync(काष्ठा sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_offset_ptr(scratch, tlb_sync_off);

	जबतक (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	वापस;
}

अटल अंतरभूत पूर्णांक tlb_range_check(काष्ठा sbi_tlb_info *curr,
					काष्ठा sbi_tlb_info *अग्र)
{
	अचिन्हित दीर्घ curr_end;
	अचिन्हित दीर्घ अग्र_end;
	पूर्णांक ret = SBI_FIFO_UNCHANGED;

	अगर (!curr || !अग्र)
		वापस ret;

	अग्र_end = अग्र->start + अग्र->आकार;
	curr_end = curr->start + curr->आकार;
	अगर (अग्र->start <= curr->start && अग्र_end > curr_end) {
		curr->start = अग्र->start;
		curr->आकार  = अग्र->आकार;
		sbi_hartmask_or(&curr->smask, &curr->smask, &अग्र->smask);
		ret = SBI_FIFO_UPDATED;
	} अन्यथा अगर (अग्र->start >= curr->start && अग्र_end <= curr_end) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &अग्र->smask);
		ret = SBI_FIFO_SKIP;
	}

	वापस ret;
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
अटल पूर्णांक tlb_update_cb(व्योम *in, व्योम *तथ्य)
{
	काष्ठा sbi_tlb_info *curr;
	काष्ठा sbi_tlb_info *अग्र;
	पूर्णांक ret = SBI_FIFO_UNCHANGED;

	अगर (!in || !तथ्य)
		वापस ret;

	curr = (काष्ठा sbi_tlb_info *)तथ्य;
	अग्र = (काष्ठा sbi_tlb_info *)in;

	अगर (अग्र->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		अगर (अग्र->asid == curr->asid)
			ret = tlb_range_check(curr, अग्र);
	} अन्यथा अगर (अग्र->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, अग्र);
	}

	वापस ret;
}

अटल पूर्णांक tlb_update(काष्ठा sbi_scratch *scratch,
			  काष्ठा sbi_scratch *remote_scratch,
			  u32 remote_hartindex, व्योम *तथ्य)
{
	पूर्णांक ret;
	atomic_t *tlb_sync;
	काष्ठा sbi_fifo *tlb_fifo_r;
	काष्ठा sbi_tlb_info *tinfo = तथ्य;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	अगर (sbi_hartindex_to_hartid(remote_hartindex) == curr_hartid) {
		tlb_entry_local_process(tinfo);
		वापस SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_offset_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, तथ्य, tlb_update_cb);

	अगर (ret == SBI_FIFO_UNCHANGED &&
	    sbi_fifo_enqueue(tlb_fifo_r, तथ्य, false) < 0) {
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
		वापस SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	atomic_add_return(tlb_sync, 1);

	वापस SBI_IPI_UPDATE_SUCCESS;
}

अटल काष्ठा sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

अटल u32 tlb_event = SBI_IPI_EVENT_MAX;

अटल स्थिर u32 tlb_type_to_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_SENT,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_SENT,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_SENT,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_SENT,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_SENT,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_SENT,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_SENT,
};

पूर्णांक sbi_tlb_request(ulong hmask, ulong hbase, काष्ठा sbi_tlb_info *tinfo)
{
	अगर (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		वापस SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	अगर (tinfo->आकार > tlb_range_flush_limit) {
		tinfo->start = 0;
		tinfo->आकार = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_to_pmu_fw_event[tinfo->type]);

	वापस sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

पूर्णांक sbi_tlb_init(काष्ठा sbi_scratch *scratch, bool cold_boot)
{
	पूर्णांक ret;
	व्योम *tlb_mem;
	atomic_t *tlb_sync;
	काष्ठा sbi_fifo *tlb_q;
	स्थिर काष्ठा sbi_platform *plat = sbi_platform_ptr(scratch);

	अगर (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_offset(माप(*tlb_sync));
		अगर (!tlb_sync_off)
			वापस SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_offset(माप(*tlb_q));
		अगर (!tlb_fifo_off) {
			sbi_scratch_free_offset(tlb_sync_off);
			वापस SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_offset(माप(tlb_mem));
		अगर (!tlb_fifo_mem_off) {
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			वापस SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		अगर (ret < 0) {
			sbi_scratch_free_offset(tlb_fifo_mem_off);
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			वापस ret;
		}
		tlb_event = ret;
		tlb_range_flush_limit = sbi_platform_tlbr_flush_limit(plat);
	} अन्यथा {
		अगर (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			वापस SBI_ENOMEM;
		अगर (SBI_IPI_EVENT_MAX <= tlb_event)
			वापस SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_offset_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, व्योम *, tlb_fifo_mem_off);
	अगर (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_अंक_entries(plat) * SBI_TLB_INFO_आकार);
		अगर (!tlb_mem)
			वापस SBI_ENOMEM;
		sbi_scratch_write_type(scratch, व्योम *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_अंक_entries(plat), SBI_TLB_INFO_आकार);

	वापस 0;
}
