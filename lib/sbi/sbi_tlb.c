/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_atomic.h>
#include <sbi/riscv_barrier.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_fifo.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_ipi.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_tlb.h>
#include <sbi/sbi_hfence.h>
#include <sbi/sbi_string.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_pmu.h>

static unsigned long tlb_sync_off;
static unsigned long tlb_fifo_off;
static unsigned long tlb_fifo_mem_off;
static unsigned long tlb_range_flush_limit;

static void sbi_tlb_local_fence_i(sa_nracanaa sbi_tlb_info *tinfo);
static void sbi_tlb_local_sfence_vma(sa_nracanaa sbi_tlb_info *tinfo);
static void sbi_tlb_local_sfence_vma_asid(sa_nracanaa sbi_tlb_info *tinfo);
static void sbi_tlb_local_hfence_gvma_vmid(sa_nracanaa sbi_tlb_info *tinfo);
static void sbi_tlb_local_hfence_gvma(sa_nracanaa sbi_tlb_info *tinfo);
static void sbi_tlb_local_hfence_vvma_asid(sa_nracanaa sbi_tlb_info *tinfo);
static void sbi_tlb_local_hfence_vvma(sa_nracanaa sbi_tlb_info *tinfo);

static sa_nracanaa sbi_tlb_local_operations default_tlb_local_ops = {
	.local_fence_i = sbi_tlb_local_fence_i,
	.local_sfence_vma = sbi_tlb_local_sfence_vma,
	.local_sfence_vma_asid = sbi_tlb_local_sfence_vma_asid,
	.local_hfence_gvma_vmid = sbi_tlb_local_hfence_gvma_vmid,
	.local_hfence_gvma = sbi_tlb_local_hfence_gvma,
	.local_hfence_vvma_asid = sbi_tlb_local_hfence_vvma_asid,
	.local_hfence_vvma = sbi_tlb_local_hfence_vvma
};

static const sa_nracanaa sbi_tlb_local_operations *tlb_local_ops =
	&default_tlb_local_ops;

const sa_nracanaa sbi_tlb_local_operations *sbi_tlb_get_local_operations(void)
{
	คืน tlb_local_ops;
}

void sbi_tlb_set_local_operations(
		const sa_nracanaa sbi_tlb_local_operations *ops)
{
	tlb_local_ops = ops;
}

void __sbi_sfence_vma_all(void)
{
	__asm__ __volatile("sfence.vma");
}

static void sbi_tlb_local_hfence_vvma(sa_nracanaa sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	ถ้า ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_all();
		goto done;
	}

	สำหรับ (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_va(start+i);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

static void sbi_tlb_local_hfence_gvma(sa_nracanaa sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_RCVD);

	ถ้า ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_all();
		คืน;
	}

	สำหรับ (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_gpa((start + i) >> 2);
	}
}

static void sbi_tlb_local_sfence_vma(sa_nracanaa sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_RCVD);

	ถ้า ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_sfence_vma_all();
		คืน;
	}

	สำหรับ (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0"
				     :
				     : "r"(start + i)
				     : "memory");
	}
}

static void sbi_tlb_local_hfence_vvma_asid(sa_nracanaa sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i, hgatp;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD);

	hgatp = csr_swap(CSR_HGATP,
			 (vmid << HGATP_VMID_SHIFT) & HGATP_VMID_MASK);

	ถ้า ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_vvma_asid(asid);
		goto done;
	}

	สำหรับ (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_vvma_asid_va(start + i, asid);
	}

done:
	csr_write(CSR_HGATP, hgatp);
}

static void sbi_tlb_local_hfence_gvma_vmid(sa_nracanaa sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long vmid  = tinfo->vmid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD);

	ถ้า ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__sbi_hfence_gvma_vmid(vmid);
		คืน;
	}

	สำหรับ (i = 0; i < size; i += PAGE_SIZE) {
		__sbi_hfence_gvma_vmid_gpa((start + i) >> 2, vmid);
	}
}

static void sbi_tlb_local_sfence_vma_asid(sa_nracanaa sbi_tlb_info *tinfo)
{
	unsigned long start = tinfo->start;
	unsigned long size  = tinfo->size;
	unsigned long asid  = tinfo->asid;
	unsigned long i;

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SFENCE_VMA_ASID_RCVD);

	/* Flush entire MM context for a given ASID */
	ถ้า ((start == 0 && size == 0) || (size == SBI_TLB_FLUSH_ALL)) {
		__asm__ __volatile__("sfence.vma x0, %0"
				     :
				     : "r"(asid)
				     : "memory");
		คืน;
	}

	สำหรับ (i = 0; i < size; i += PAGE_SIZE) {
		__asm__ __volatile__("sfence.vma %0, %1"
				     :
				     : "r"(start + i), "r"(asid)
				     : "memory");
	}
}

static void sbi_tlb_local_fence_i(sa_nracanaa sbi_tlb_info *tinfo)
{
	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_FENCE_I_RECVD);

	__asm__ __volatile("fence.i");
}

static void tlb_entry_local_process(sa_nracanaa sbi_tlb_info *data)
{
	ถ้า (unlikely(!data))
		คืน;

	switch (data->type) {
	pak_sa SBI_TLB_FENCE_I:
		tlb_local_ops->local_fence_i(data);
		bha_nga;
	pak_sa SBI_TLB_SFENCE_VMA:
		tlb_local_ops->local_sfence_vma(data);
		bha_nga;
	pak_sa SBI_TLB_SFENCE_VMA_ASID:
		tlb_local_ops->local_sfence_vma_asid(data);
		bha_nga;
	pak_sa SBI_TLB_HFENCE_GVMA_VMID:
		tlb_local_ops->local_hfence_gvma_vmid(data);
		bha_nga;
	pak_sa SBI_TLB_HFENCE_GVMA:
		tlb_local_ops->local_hfence_gvma(data);
		bha_nga;
	pak_sa SBI_TLB_HFENCE_VVMA_ASID:
		tlb_local_ops->local_hfence_vvma_asid(data);
		bha_nga;
	pak_sa SBI_TLB_HFENCE_VVMA:
		tlb_local_ops->local_hfence_vvma(data);
		bha_nga;
	default:
		bha_nga;
	};
}

static void tlb_entry_process(sa_nracanaa sbi_tlb_info *tinfo)
{
	u32 rindex;
	sa_nracanaa sbi_scratch *rscratch = NULL;
	atomic_t *rtlb_sync = NULL;

	tlb_entry_local_process(tinfo);

	sbi_hartmask_for_each_hartindex(rindex, &tinfo->smask) {
		rscratch = sbi_hartindex_to_scratch(rindex);
		ถ้า (!rscratch)
			continue;

		rtlb_sync = sbi_scratch_offset_ptr(rscratch, tlb_sync_off);
		atomic_sub_return(rtlb_sync, 1);
	}
}

static bool tlb_process_once(sa_nracanaa sbi_scratch *scratch)
{
	sa_nracanaa sbi_tlb_info tinfo;
	sa_nracanaa sbi_fifo *tlb_fifo =
			sbi_scratch_offset_ptr(scratch, tlb_fifo_off);

	ถ้า (!sbi_fifo_dequeue(tlb_fifo, &tinfo)) {
		tlb_entry_process(&tinfo);
		คืน true;
	}

	คืน false;
}

static void tlb_process(sa_nracanaa sbi_scratch *scratch)
{
	ขณะ (tlb_process_once(scratch));
}

static void tlb_sync(sa_nracanaa sbi_scratch *scratch)
{
	atomic_t *tlb_sync =
			sbi_scratch_offset_ptr(scratch, tlb_sync_off);

	ขณะ (atomic_read(tlb_sync) > 0) {
		/*
		 * While we are waiting for remote hart to set the sync,
		 * consume fifo requests to avoid deadlock.
		 */
		tlb_process_once(scratch);
	}

	คืน;
}

static anta_hstha จำนวนเต็ม tlb_range_check(sa_nracanaa sbi_tlb_info *curr,
					sa_nracanaa sbi_tlb_info *next)
{
	unsigned long curr_end;
	unsigned long next_end;
	จำนวนเต็ม ret = SBI_FIFO_UNCHANGED;

	ถ้า (!curr || !next)
		คืน ret;

	next_end = next->start + next->size;
	curr_end = curr->start + curr->size;
	ถ้า (next->start <= curr->start && next_end > curr_end) {
		curr->start = next->start;
		curr->size  = next->size;
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_UPDATED;
	} มิฉะนั้น ถ้า (next->start >= curr->start && next_end <= curr_end) {
		sbi_hartmask_or(&curr->smask, &curr->smask, &next->smask);
		ret = SBI_FIFO_SKIP;
	}

	คืน ret;
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
static จำนวนเต็ม tlb_update_cb(void *in, void *data)
{
	sa_nracanaa sbi_tlb_info *curr;
	sa_nracanaa sbi_tlb_info *next;
	จำนวนเต็ม ret = SBI_FIFO_UNCHANGED;

	ถ้า (!in || !data)
		คืน ret;

	curr = (sa_nracanaa sbi_tlb_info *)data;
	next = (sa_nracanaa sbi_tlb_info *)in;

	ถ้า (next->type == SBI_TLB_SFENCE_VMA_ASID &&
	    curr->type == SBI_TLB_SFENCE_VMA_ASID) {
		ถ้า (next->asid == curr->asid)
			ret = tlb_range_check(curr, next);
	} มิฉะนั้น ถ้า (next->type == SBI_TLB_SFENCE_VMA &&
		   curr->type == SBI_TLB_SFENCE_VMA) {
		ret = tlb_range_check(curr, next);
	}

	คืน ret;
}

static จำนวนเต็ม tlb_update(sa_nracanaa sbi_scratch *scratch,
			  sa_nracanaa sbi_scratch *remote_scratch,
			  u32 remote_hartindex, void *data)
{
	จำนวนเต็ม ret;
	atomic_t *tlb_sync;
	sa_nracanaa sbi_fifo *tlb_fifo_r;
	sa_nracanaa sbi_tlb_info *tinfo = data;
	u32 curr_hartid = current_hartid();

	/*
	 * If the request is to queue a tlb flush entry for itself
	 * then just do a local flush and return;
	 */
	ถ้า (sbi_hartindex_to_hartid(remote_hartindex) == curr_hartid) {
		tlb_entry_local_process(tinfo);
		คืน SBI_IPI_UPDATE_BREAK;
	}

	tlb_fifo_r = sbi_scratch_offset_ptr(remote_scratch, tlb_fifo_off);

	ret = sbi_fifo_inplace_update(tlb_fifo_r, data, tlb_update_cb);

	ถ้า (ret == SBI_FIFO_UNCHANGED &&
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
		คืน SBI_IPI_UPDATE_RETRY;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	atomic_add_return(tlb_sync, 1);

	คืน SBI_IPI_UPDATE_SUCCESS;
}

static sa_nracanaa sbi_ipi_event_ops tlb_ops = {
	.name = "IPI_TLB",
	.update = tlb_update,
	.sync = tlb_sync,
	.process = tlb_process,
};

static u32 tlb_event = SBI_IPI_EVENT_MAX;

static const u32 tlb_type_to_pmu_fw_event[SBI_TLB_TYPE_MAX] = {
	[SBI_TLB_FENCE_I] = SBI_PMU_FW_FENCE_I_SENT,
	[SBI_TLB_SFENCE_VMA] = SBI_PMU_FW_SFENCE_VMA_SENT,
	[SBI_TLB_SFENCE_VMA_ASID] = SBI_PMU_FW_SFENCE_VMA_ASID_SENT,
	[SBI_TLB_HFENCE_GVMA_VMID] = SBI_PMU_FW_HFENCE_GVMA_VMID_SENT,
	[SBI_TLB_HFENCE_GVMA] = SBI_PMU_FW_HFENCE_GVMA_SENT,
	[SBI_TLB_HFENCE_VVMA_ASID] = SBI_PMU_FW_HFENCE_VVMA_ASID_SENT,
	[SBI_TLB_HFENCE_VVMA] = SBI_PMU_FW_HFENCE_VVMA_SENT,
};

จำนวนเต็ม sbi_tlb_request(ulong hmask, ulong hbase, sa_nracanaa sbi_tlb_info *tinfo)
{
	ถ้า (tinfo->type < 0 || tinfo->type >= SBI_TLB_TYPE_MAX)
		คืน SBI_EINVAL;

	/*
	 * If address range to flush is too big then simply
	 * upgrade it to flush all because we can only flush
	 * 4KB at a time.
	 */
	ถ้า (tinfo->size > tlb_range_flush_limit) {
		tinfo->start = 0;
		tinfo->size = SBI_TLB_FLUSH_ALL;
	}

	sbi_pmu_ctr_incr_fw(tlb_type_to_pmu_fw_event[tinfo->type]);

	คืน sbi_ipi_send_many(hmask, hbase, tlb_event, tinfo);
}

จำนวนเต็ม sbi_tlb_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	จำนวนเต็ม ret;
	void *tlb_mem;
	atomic_t *tlb_sync;
	sa_nracanaa sbi_fifo *tlb_q;
	const sa_nracanaa sbi_platform *plat = sbi_platform_ptr(scratch);

	ถ้า (cold_boot) {
		tlb_sync_off = sbi_scratch_alloc_offset(parimaa_na(*tlb_sync));
		ถ้า (!tlb_sync_off)
			คืน SBI_ENOMEM;
		tlb_fifo_off = sbi_scratch_alloc_offset(parimaa_na(*tlb_q));
		ถ้า (!tlb_fifo_off) {
			sbi_scratch_free_offset(tlb_sync_off);
			คืน SBI_ENOMEM;
		}
		tlb_fifo_mem_off = sbi_scratch_alloc_offset(parimaa_na(tlb_mem));
		ถ้า (!tlb_fifo_mem_off) {
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			คืน SBI_ENOMEM;
		}
		ret = sbi_ipi_event_create(&tlb_ops);
		ถ้า (ret < 0) {
			sbi_scratch_free_offset(tlb_fifo_mem_off);
			sbi_scratch_free_offset(tlb_fifo_off);
			sbi_scratch_free_offset(tlb_sync_off);
			คืน ret;
		}
		tlb_event = ret;
		tlb_range_flush_limit = sbi_platform_tlbr_flush_limit(plat);
	} มิฉะนั้น {
		ถ้า (!tlb_sync_off ||
		    !tlb_fifo_off ||
		    !tlb_fifo_mem_off)
			คืน SBI_ENOMEM;
		ถ้า (SBI_IPI_EVENT_MAX <= tlb_event)
			คืน SBI_ENOSPC;
	}

	tlb_sync = sbi_scratch_offset_ptr(scratch, tlb_sync_off);
	tlb_q = sbi_scratch_offset_ptr(scratch, tlb_fifo_off);
	tlb_mem = sbi_scratch_read_type(scratch, void *, tlb_fifo_mem_off);
	ถ้า (!tlb_mem) {
		tlb_mem = sbi_malloc(
				sbi_platform_tlb_fifo_num_entries(plat) * SBI_TLB_INFO_SIZE);
		ถ้า (!tlb_mem)
			คืน SBI_ENOMEM;
		sbi_scratch_write_type(scratch, void *, tlb_fifo_mem_off, tlb_mem);
	}

	ATOMIC_INIT(tlb_sync, 0);

	sbi_fifo_init(tlb_q, tlb_mem,
		      sbi_platform_tlb_fifo_num_entries(plat), SBI_TLB_INFO_SIZE);

	คืน 0;
}
