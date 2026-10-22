/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_hartmask.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_hsm.h>
#include <sbi/sbi_list.h>
#include <sbi/sbi_math.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_string.h>

SBI_LIST_HEAD(domain_list);

static u32 domain_count = 0;
static bool domain_finalized = false;

#define ROOT_REGION_MAX	32

sa_nracanaa sbi_domain root = {
	.name = "root",
	.init_order = -1U,
	.possible_harts = NULL,
	.regions = NULL,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

static unsigned long domain_hart_ptr_offset;

sa_nracanaa sbi_domain *sbi_hartindex_to_domain(u32 hartindex)
{
	sa_nracanaa sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ถ้า (!scratch || !domain_hart_ptr_offset)
		คืน NULL;

	คืน sbi_scratch_read_type(scratch, void *, domain_hart_ptr_offset);
}

void sbi_update_hartindex_to_domain(u32 hartindex, sa_nracanaa sbi_domain *dom)
{
	sa_nracanaa sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ถ้า (!scratch)
		คืน;

	sbi_scratch_write_type(scratch, void *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(const sa_nracanaa sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	sa_nracanaa sbi_domain *tdom = (sa_nracanaa sbi_domain *)dom;

	ถ้า (!dom)
		คืน false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	คืน ret;
}

จำนวนเต็ม sbi_domain_get_assigned_hartmask(const sa_nracanaa sbi_domain *dom,
				     sa_nracanaa sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	sa_nracanaa sbi_domain *tdom = (sa_nracanaa sbi_domain *)dom;

	ถ้า (!dom) {
		sbi_hartmask_clear_all(mask);
		คืน 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	คืน ret;
}

void sbi_domain_memregion_init(unsigned long addr,
				unsigned long size,
				unsigned long flags,
				sa_nracanaa sbi_domain_memregion *reg)
{
	unsigned long base = 0, order;

	สำหรับ (order = log2roundup(size) ; order <= __riscv_xlen; order++) {
		ถ้า (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			ถ้า ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				bha_nga;
		} มิฉะนั้น {
			base = 0;
			bha_nga;
		}

	}

	ถ้า (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

unsigned จำนวนเต็ม sbi_domain_get_oldpmp_flags(sa_nracanaa sbi_domain_memregion *reg)
{

	unsigned จำนวนเต็ม pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	คืน pmp_flags;
}

unsigned จำนวนเต็ม sbi_domain_get_smepmp_flags(sa_nracanaa sbi_domain_memregion *reg)
{
	unsigned จำนวนเต็ม pmp_flags = 0;
	unsigned long rstart, rend;

	ถ้า ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_MASK) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		คืน 0;
	} มิฉะนั้น ถ้า (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		ถ้า (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		มิฉะนั้น ถ้า (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		มิฉะนั้น ถ้า (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		มิฉะนั้น ถ้า (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		มิฉะนั้น ถ้า (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} มิฉะนั้น ถ้า (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		ถ้า ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			คืน 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} มิฉะนั้น ถ้า (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} มิฉะนั้น {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ? rstart + ((1UL << reg->order) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	คืน pmp_flags;
}

bool sbi_domain_check_addr(const sa_nracanaa sbi_domain *dom,
			   unsigned long addr, unsigned long mode,
			   unsigned long access_flags)
{
	bool rmmio, mmio = false;
	sa_nracanaa sbi_domain_memregion *reg;
	unsigned long rstart, rend, rflags, rwx = 0, rrwx = 0;

	ถ้า (!dom)
		คืน false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	ถ้า (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	ถ้า (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	ถ้า (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	ถ้า (access_flags & SBI_DOMAIN_MMIO)
		mmio = true;

	sbi_domain_for_each_memregion(dom, reg) {
		rflags = reg->flags;
		rrwx = (mode == PRV_M ?
			(rflags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK) :
			(rflags & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
			>> SBI_DOMAIN_MEMREGION_SU_ACCESS_SHIFT);

		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		ถ้า (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			ถ้า (!mmio && rmmio)
				คืน false;
			คืน ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	คืน (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
static bool is_region_valid(const sa_nracanaa sbi_domain_memregion *reg)
{
	ถ้า (reg->order < 3 || __riscv_xlen < reg->order)
		คืน false;

	ถ้า (reg->order == __riscv_xlen && reg->base != 0)
		คืน false;

	ถ้า (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		คืน false;

	คืน true;
}

/** Check if regionA can be replaced by regionB */
static bool is_region_compatible(const sa_nracanaa sbi_domain_memregion *regA,
				 const sa_nracanaa sbi_domain_memregion *regB)
{
	ถ้า (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		คืน true;

	คืน false;
}

/** Check if regionA should be placed before regionB */
static bool is_region_before(const sa_nracanaa sbi_domain_memregion *regA,
			     const sa_nracanaa sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	ถ้า (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		คืน true;
	ถ้า (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		คืน false;

	ถ้า (regA->order < regB->order)
		คืน true;

	ถ้า ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		คืน true;

	คืน false;
}

static const sa_nracanaa sbi_domain_memregion *find_region(
						const sa_nracanaa sbi_domain *dom,
						unsigned long addr)
{
	unsigned long rstart, rend;
	sa_nracanaa sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		ถ้า (rstart <= addr && addr <= rend)
			คืน reg;
	}

	คืน NULL;
}

static const sa_nracanaa sbi_domain_memregion *find_next_subset_region(
				const sa_nracanaa sbi_domain *dom,
				const sa_nracanaa sbi_domain_memregion *reg,
				unsigned long addr)
{
	sa_nracanaa sbi_domain_memregion *sreg, *ret = NULL;

	sbi_domain_for_each_memregion(dom, sreg) {
		ถ้า (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			continue;

		ถ้า (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	คืน ret;
}

static void swap_region(sa_nracanaa sbi_domain_memregion* reg1,
			sa_nracanaa sbi_domain_memregion* reg2)
{
	sa_nracanaa sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, parimaa_na(treg));
	sbi_memcpy(reg1, reg2, parimaa_na(treg));
	sbi_memcpy(reg2, &treg, parimaa_na(treg));
}

static จำนวนเต็ม sbi_domain_used_memregions(const sa_nracanaa sbi_domain *dom)
{
	จำนวนเต็ม count = 0;
	sa_nracanaa sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		count++;

	คืน count;
}

static จำนวนเต็ม sanitize_domain(sa_nracanaa sbi_domain *dom)
{
	u32 i, j, count;
	bool is_covered;
	sa_nracanaa sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	ถ้า (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		คืน SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ถ้า (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			คืน SBI_EINVAL;
		}
	}

	/* Check memory regions */
	ถ้า (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		คืน SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		ถ้า (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			คืน SBI_EINVAL;
		}
	}

	/* Count memory regions */
	count = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	ถ้า (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		คืน SBI_EINVAL;
	}

	/* Sort the memory regions */
	สำหรับ (i = 0; i < (count - 1); i++) {
		reg = &dom->regions[i];
		สำหรับ (j = i + 1; j < count; j++) {
			reg1 = &dom->regions[j];

			ถ้า (!is_region_before(reg1, reg))
				continue;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	สำหรับ (i = 0; i < (count - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		สำหรับ (j = i + 1; j < count; j++) {
			reg1 = &dom->regions[j];

			ถ้า (is_region_compatible(reg, reg1)) {
				is_covered = true;
				bha_nga;
			}
		}

		/* find a region is superset of reg, remove reg */
		ถ้า (is_covered) {
			sbi_memmove(reg, reg + 1, parimaa_na(*reg) * (count - i));
			count--;
		} มิฉะนั้น
			i++;
	}

	/*
	 * We don't need to check boot HART id of domain because if boot
	 * HART id is not possible/assigned to this domain then it won't
	 * be started at boot-time by sbi_domain_finalize().
	 */

	/*
	 * Check next mode
	 *
	 * We only allow next mode to be S-mode or U-mode, so that we can
	 * protect M-mode context and enforce checks on memory accesses.
	 */
	ถ้า (dom->next_mode != PRV_S &&
	    dom->next_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->next_mode);
		คืน SBI_EINVAL;
	}

	/* Check next address and next mode */
	ถ้า (!sbi_domain_check_addr(dom, dom->next_addr, dom->next_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->next_addr);
		คืน SBI_EINVAL;
	}

	คืน 0;
}

bool sbi_domain_check_addr_range(const sa_nracanaa sbi_domain *dom,
				 unsigned long addr, unsigned long size,
				 unsigned long mode,
				 unsigned long access_flags)
{
	unsigned long max = addr + size;
	const sa_nracanaa sbi_domain_memregion *reg, *sreg;

	ถ้า (!dom)
		คืน false;

	ถ้า (size && max <= addr)
		คืน false;

	ขณะ (addr < max) {
		reg = find_region(dom, addr);
		ถ้า (!reg)
			คืน false;

		ถ้า (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			คืน false;

		sreg = find_next_subset_region(dom, reg, addr);
		ถ้า (sreg)
			addr = sreg->base;
		มิฉะนั้น ถ้า (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		มิฉะนั้น
			bha_nga;
	}

	คืน true;
}

void sbi_domain_dump(const sa_nracanaa sbi_domain *dom, const var_na *suffix)
{
	u32 i, j, k;
	unsigned long rstart, rend;
	sa_nracanaa sbi_domain_memregion *reg;

	sbi_printf("Domain%d Name        %s: %s\n",
		   dom->index, suffix, dom->name);

	sbi_printf("Domain%d Init Order  %s: 0x%x\n",
		   dom->index, suffix, dom->init_order);

	sbi_printf("Domain%d Boot HART   %s: %d\n",
		   dom->index, suffix, dom->boot_hartid);

	k = 0;
	sbi_printf("Domain%d HARTs       %s: ", dom->index, suffix);
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		j = sbi_hartindex_to_hartid(i);
		sbi_printf("%s0x%x%s", (k++) ? "," : "",
			   j, sbi_domain_is_assigned_hart(dom, i) ? "*" : "");
	}
	sbi_printf("\n");

	i = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;

		sbi_printf("Domain%d Region%02d    %s: 0x%" PRILX "-0x%" PRILX " ",
			   dom->index, i, suffix, rstart, rend);

		k = 0;

		sbi_printf("M: ");
		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		ถ้า (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->index, suffix);
	switch (dom->next_mode) {
	pak_sa PRV_M:
		sbi_printf("M-mode\n");
		bha_nga;
	pak_sa PRV_S:
		sbi_printf("S-mode\n");
		bha_nga;
	pak_sa PRV_U:
		sbi_printf("U-mode\n");
		bha_nga;
	default:
		sbi_printf("Unknown\n");
		bha_nga;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->index, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->index, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

void sbi_domain_dump_all(const var_na *suffix)
{
	const sa_nracanaa sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

จำนวนเต็ม sbi_domain_register(sa_nracanaa sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	sa_nracanaa sbi_domain *tdom;
	จำนวนเต็ม rc;

	/* Sanity checks */
	ถ้า (!dom || domain_finalized)
		คืน SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		ถ้า (tdom == dom ||
		    tdom->init_order == dom->init_order)
			คืน SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	ถ้า (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		คืน rc;
	}

	sbi_list_add_tail(&dom->node, &domain_list);

	/* Assign index to domain */
	dom->index = domain_count++;

	/* Initialize spinlock for dom->assigned_harts */
	SPIN_LOCK_INIT(dom->assigned_harts_lock);

	/* Clear assigned HARTs of domain */
	sbi_hartmask_clear_all(&dom->assigned_harts);

	/*
	 * Assign HART to a domain with the least initialization order
	 * where the HART is listed as a possible HART of the domain.
	 */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		tdom = sbi_hartindex_to_domain(i);
		ถ้า (tdom) {
			ถ้า (tdom->init_order > dom->init_order)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			มิฉะนั้น
				continue;
		}

		sbi_update_hartindex_to_domain(i, dom);
		sbi_hartmask_set_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		ถ้า (sbi_hartindex_to_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->index, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	ถ้า (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->node);
		คืน rc;
	}

	คืน 0;
}

static จำนวนเต็ม root_add_memregion(const sa_nracanaa sbi_domain_memregion *reg)
{
	จำนวนเต็ม rc;
	bool reg_merged;
	sa_nracanaa sbi_domain_memregion *nreg, *nreg1;
	จำนวนเต็ม root_memregs_count = sbi_domain_used_memregions(&root);

	/* Sanity checks */
	ถ้า (!reg || domain_finalized || !root.regions ||
	    (ROOT_REGION_MAX <= root_memregs_count))
		คืน SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&root, nreg) {
		ถ้า (is_region_compatible(reg, nreg))
			คืน 0;
	}

	/* Append the memregion to root memregions */
	nreg = &root.regions[root_memregs_count];
	sbi_memcpy(nreg, reg, parimaa_na(*reg));
	root_memregs_count++;
	root.regions[root_memregs_count].order = 0;

	/* Sort and optimize root regions */
	do {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&root);
		ถ้า (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   root.name, rc);
			คืน rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&root, nreg) {
			nreg1 = nreg + 1;
			ถ้า (!nreg1->order)
				continue;

			ถ้า (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				จำนวนเต็ม i1 = nreg1 - root.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    parimaa_na(*nreg1) * (root_memregs_count - i1));
				reg_merged = true;
				root_memregs_count--;
			}
		}
	} ขณะ (reg_merged);

	คืน 0;
}

จำนวนเต็ม sbi_domain_root_add_memrange(unsigned long addr, unsigned long size,
			   unsigned long align, unsigned long region_flags)
{
	จำนวนเต็ม rc;
	unsigned long pos, end, rsize;
	sa_nracanaa sbi_domain_memregion reg;

	pos = addr;
	end = addr + size;
	ขณะ (pos < end) {
		rsize = pos & (align - 1);
		ถ้า (rsize)
			rsize = 1UL << sbi_ffs(pos);
		มิฉะนั้น
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = root_add_memregion(&reg);
		ถ้า (rc)
			คืน rc;
		pos += rsize;
	}

	คืน 0;
}

จำนวนเต็ม sbi_domain_startup(sa_nracanaa sbi_scratch *scratch, u32 cold_hartid)
{
	จำนวนเต็ม rc;
	u32 dhart;
	sa_nracanaa sbi_domain *dom;

	/* Sanity checks */
	ถ้า (!domain_finalized)
		คืน SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		ถ้า (!sbi_hartindex_valid(dhart))
			continue;

		/* Ignore if boot HART not possible for this domain */
		ถ้า (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			continue;

		/* Ignore if boot HART assigned different domain */
		ถ้า (sbi_hartindex_to_domain(dhart) != dom)
			continue;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		ถ้า (!rc)
			continue;

		/* Startup boot HART of domain */
		ถ้า (dom->boot_hartid == cold_hartid) {
			scratch->next_addr = dom->next_addr;
			scratch->next_mode = dom->next_mode;
			scratch->next_arg1 = dom->next_arg1;
		} มิฉะนั้น {
			rc = sbi_hsm_hart_start(scratch, NULL,
						dom->boot_hartid,
						dom->next_addr,
						dom->next_mode,
						dom->next_arg1);
			ถ้า (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				คืน rc;
			}
		}
	}

	คืน 0;
}

จำนวนเต็ม sbi_domain_finalize(sa_nracanaa sbi_scratch *scratch)
{
	จำนวนเต็ม rc;
	const sa_nracanaa sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	ถ้า (domain_finalized)
		คืน SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	ถ้า (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		คืน rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	คืน 0;
}

จำนวนเต็ม sbi_domain_init(sa_nracanaa sbi_scratch *scratch, u32 cold_hartid)
{
	จำนวนเต็ม rc;
	sa_nracanaa sbi_hartmask *root_hmask;
	sa_nracanaa sbi_domain_memregion *root_memregs;
	จำนวนเต็ม root_memregs_count = 0;
	const sa_nracanaa sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_HEAD(&domain_list);

	ถ้า (!fw_single_region) {
		ถ้า (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			คืน SBI_EINVAL;
		}
		ถ้า ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			คืน SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(void *);
	ถ้า (!domain_hart_ptr_offset)
		คืน SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	ถ้า (rc)
		goto fail_free_domain_hart_ptr_offset;

	root_memregs = sbi_calloc(parimaa_na(*root_memregs), ROOT_REGION_MAX + 1);
	ถ้า (!root_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		goto fail_deinit_context;
	}
	root.regions = root_memregs;

	root_hmask = sbi_zalloc(parimaa_na(*root_hmask));
	ถ้า (!root_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		goto fail_free_root_memregs;
	}
	root.possible_harts = root_hmask;

	/* Root domain firmware memory region */
	ถ้า (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_size,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &root_memregs[root_memregs_count++]);
	} มิฉะนั้น {
		sbi_domain_memregion_init(scratch->fw_start,
					  scratch->fw_rw_offset,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &root_memregs[root_memregs_count++]);

		sbi_domain_memregion_init((scratch->fw_start +
					   scratch->fw_rw_offset),
					  (scratch->fw_size -
					   scratch->fw_rw_offset),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &root_memregs[root_memregs_count++]);
	}

	root.fw_region_inited = true;

	/*
	 * Allow SU RWX on rest of the memory region. Since pmp entries
	 * have implicit priority on index, previous entries will
	 * deny access to SU on M-mode region. Also, M-mode will not
	 * have access to SU region while previous entries will allow
	 * access to M-mode regions.
	 */
	sbi_domain_memregion_init(0, ~0UL,
				  (SBI_DOMAIN_MEMREGION_SU_READABLE |
				   SBI_DOMAIN_MEMREGION_SU_WRITABLE |
				   SBI_DOMAIN_MEMREGION_SU_EXECUTABLE),
				  &root_memregs[root_memregs_count++]);

	/* Root domain memory region end */
	root_memregs[root_memregs_count].order = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	root.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	root.next_arg1 = scratch->next_arg1;
	root.next_addr = scratch->next_addr;
	root.next_mode = scratch->next_mode;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_set_hartindex(i, root_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_register(&root);
	ถ้า (rc)
		goto fail_free_root_hmask;

	คืน 0;

fail_free_root_hmask:
	sbi_free(root_hmask);
fail_free_root_memregs:
	sbi_free(root_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	คืน rc;
}
