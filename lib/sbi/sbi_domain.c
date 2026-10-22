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
	නම් (!scratch || !domain_hart_ptr_offset)
		ආපසු NULL;

	ආපසු sbi_scratch_read_type(scratch, void *, domain_hart_ptr_offset);
}

void sbi_update_hartindex_to_domain(u32 hartindex, sa_nracanaa sbi_domain *dom)
{
	sa_nracanaa sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	නම් (!scratch)
		ආපසු;

	sbi_scratch_write_type(scratch, void *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(const sa_nracanaa sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	sa_nracanaa sbi_domain *tdom = (sa_nracanaa sbi_domain *)dom;

	නම් (!dom)
		ආපසු false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	ආපසු ret;
}

පූර්ණ_සංඛ්‍යා sbi_domain_get_assigned_hartmask(const sa_nracanaa sbi_domain *dom,
				     sa_nracanaa sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	sa_nracanaa sbi_domain *tdom = (sa_nracanaa sbi_domain *)dom;

	නම් (!dom) {
		sbi_hartmask_clear_all(mask);
		ආපසු 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	ආපසු ret;
}

void sbi_domain_memregion_init(unsigned long addr,
				unsigned long size,
				unsigned long flags,
				sa_nracanaa sbi_domain_memregion *reg)
{
	unsigned long base = 0, order;

	සඳහා (order = log2roundup(size) ; order <= __riscv_xlen; order++) {
		නම් (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			නම් ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				bha_nga;
		} නැතහොත් {
			base = 0;
			bha_nga;
		}

	}

	නම් (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

unsigned පූර්ණ_සංඛ්‍යා sbi_domain_get_oldpmp_flags(sa_nracanaa sbi_domain_memregion *reg)
{

	unsigned පූර්ණ_සංඛ්‍යා pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	නම් (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	නම් (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	නම් (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	නම් (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	ආපසු pmp_flags;
}

unsigned පූර්ණ_සංඛ්‍යා sbi_domain_get_smepmp_flags(sa_nracanaa sbi_domain_memregion *reg)
{
	unsigned පූර්ණ_සංඛ්‍යා pmp_flags = 0;
	unsigned long rstart, rend;

	නම් ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_MASK) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		ආපසු 0;
	} නැතහොත් නම් (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		නම් (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		නැතහොත් නම් (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		නැතහොත් නම් (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		නැතහොත් නම් (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		නැතහොත් නම් (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} නැතහොත් නම් (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		නම් ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			ආපසු 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		නම් (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		නම් (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		නම් (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} නැතහොත් නම් (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		නම් (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		නම් (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		නම් (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} නැතහොත් {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ? rstart + ((1UL << reg->order) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	ආපසු pmp_flags;
}

bool sbi_domain_check_addr(const sa_nracanaa sbi_domain *dom,
			   unsigned long addr, unsigned long mode,
			   unsigned long access_flags)
{
	bool rmmio, mmio = false;
	sa_nracanaa sbi_domain_memregion *reg;
	unsigned long rstart, rend, rflags, rwx = 0, rrwx = 0;

	නම් (!dom)
		ආපසු false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	නම් (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	නම් (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	නම් (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	නම් (access_flags & SBI_DOMAIN_MMIO)
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
		නම් (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			නම් (!mmio && rmmio)
				ආපසු false;
			ආපසු ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	ආපසු (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
static bool is_region_valid(const sa_nracanaa sbi_domain_memregion *reg)
{
	නම් (reg->order < 3 || __riscv_xlen < reg->order)
		ආපසු false;

	නම් (reg->order == __riscv_xlen && reg->base != 0)
		ආපසු false;

	නම් (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		ආපසු false;

	ආපසු true;
}

/** Check if regionA can be replaced by regionB */
static bool is_region_compatible(const sa_nracanaa sbi_domain_memregion *regA,
				 const sa_nracanaa sbi_domain_memregion *regB)
{
	නම් (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		ආපසු true;

	ආපසු false;
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
	නම් (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		ආපසු true;
	නම් (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		ආපසු false;

	නම් (regA->order < regB->order)
		ආපසු true;

	නම් ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		ආපසු true;

	ආපසු false;
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
		නම් (rstart <= addr && addr <= rend)
			ආපසු reg;
	}

	ආපසු NULL;
}

static const sa_nracanaa sbi_domain_memregion *find_next_subset_region(
				const sa_nracanaa sbi_domain *dom,
				const sa_nracanaa sbi_domain_memregion *reg,
				unsigned long addr)
{
	sa_nracanaa sbi_domain_memregion *sreg, *ret = NULL;

	sbi_domain_for_each_memregion(dom, sreg) {
		නම් (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			continue;

		නම් (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	ආපසු ret;
}

static void swap_region(sa_nracanaa sbi_domain_memregion* reg1,
			sa_nracanaa sbi_domain_memregion* reg2)
{
	sa_nracanaa sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, parimaa_na(treg));
	sbi_memcpy(reg1, reg2, parimaa_na(treg));
	sbi_memcpy(reg2, &treg, parimaa_na(treg));
}

static පූර්ණ_සංඛ්‍යා sbi_domain_used_memregions(const sa_nracanaa sbi_domain *dom)
{
	පූර්ණ_සංඛ්‍යා count = 0;
	sa_nracanaa sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		count++;

	ආපසු count;
}

static පූර්ණ_සංඛ්‍යා sanitize_domain(sa_nracanaa sbi_domain *dom)
{
	u32 i, j, count;
	bool is_covered;
	sa_nracanaa sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	නම් (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		ආපසු SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		නම් (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			ආපසු SBI_EINVAL;
		}
	}

	/* Check memory regions */
	නම් (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		ආපසු SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		නම් (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			ආපසු SBI_EINVAL;
		}
	}

	/* Count memory regions */
	count = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	නම් (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		ආපසු SBI_EINVAL;
	}

	/* Sort the memory regions */
	සඳහා (i = 0; i < (count - 1); i++) {
		reg = &dom->regions[i];
		සඳහා (j = i + 1; j < count; j++) {
			reg1 = &dom->regions[j];

			නම් (!is_region_before(reg1, reg))
				continue;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	සඳහා (i = 0; i < (count - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		සඳහා (j = i + 1; j < count; j++) {
			reg1 = &dom->regions[j];

			නම් (is_region_compatible(reg, reg1)) {
				is_covered = true;
				bha_nga;
			}
		}

		/* find a region is superset of reg, remove reg */
		නම් (is_covered) {
			sbi_memmove(reg, reg + 1, parimaa_na(*reg) * (count - i));
			count--;
		} නැතහොත්
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
	නම් (dom->next_mode != PRV_S &&
	    dom->next_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->next_mode);
		ආපසු SBI_EINVAL;
	}

	/* Check next address and next mode */
	නම් (!sbi_domain_check_addr(dom, dom->next_addr, dom->next_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->next_addr);
		ආපසු SBI_EINVAL;
	}

	ආපසු 0;
}

bool sbi_domain_check_addr_range(const sa_nracanaa sbi_domain *dom,
				 unsigned long addr, unsigned long size,
				 unsigned long mode,
				 unsigned long access_flags)
{
	unsigned long max = addr + size;
	const sa_nracanaa sbi_domain_memregion *reg, *sreg;

	නම් (!dom)
		ආපසු false;

	නම් (size && max <= addr)
		ආපසු false;

	අතරතුර (addr < max) {
		reg = find_region(dom, addr);
		නම් (!reg)
			ආපසු false;

		නම් (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			ආපසු false;

		sreg = find_next_subset_region(dom, reg, addr);
		නම් (sreg)
			addr = sreg->base;
		නැතහොත් නම් (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		නැතහොත්
			bha_nga;
	}

	ආපසු true;
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
		නම් (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		නම් (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		නම් (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		නම් (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		නම් (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		නම් (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		නම් (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		නම් (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
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

පූර්ණ_සංඛ්‍යා sbi_domain_register(sa_nracanaa sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	sa_nracanaa sbi_domain *tdom;
	පූර්ණ_සංඛ්‍යා rc;

	/* Sanity checks */
	නම් (!dom || domain_finalized)
		ආපසු SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		නම් (tdom == dom ||
		    tdom->init_order == dom->init_order)
			ආපසු SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	නම් (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		ආපසු rc;
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
		නම් (tdom) {
			නම් (tdom->init_order > dom->init_order)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			නැතහොත්
				continue;
		}

		sbi_update_hartindex_to_domain(i, dom);
		sbi_hartmask_set_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		නම් (sbi_hartindex_to_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->index, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	නම් (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->node);
		ආපසු rc;
	}

	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා root_add_memregion(const sa_nracanaa sbi_domain_memregion *reg)
{
	පූර්ණ_සංඛ්‍යා rc;
	bool reg_merged;
	sa_nracanaa sbi_domain_memregion *nreg, *nreg1;
	පූර්ණ_සංඛ්‍යා root_memregs_count = sbi_domain_used_memregions(&root);

	/* Sanity checks */
	නම් (!reg || domain_finalized || !root.regions ||
	    (ROOT_REGION_MAX <= root_memregs_count))
		ආපසු SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&root, nreg) {
		නම් (is_region_compatible(reg, nreg))
			ආපසු 0;
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
		නම් (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   root.name, rc);
			ආපසු rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&root, nreg) {
			nreg1 = nreg + 1;
			නම් (!nreg1->order)
				continue;

			නම් (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				පූර්ණ_සංඛ්‍යා i1 = nreg1 - root.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    parimaa_na(*nreg1) * (root_memregs_count - i1));
				reg_merged = true;
				root_memregs_count--;
			}
		}
	} අතරතුර (reg_merged);

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා sbi_domain_root_add_memrange(unsigned long addr, unsigned long size,
			   unsigned long align, unsigned long region_flags)
{
	පූර්ණ_සංඛ්‍යා rc;
	unsigned long pos, end, rsize;
	sa_nracanaa sbi_domain_memregion reg;

	pos = addr;
	end = addr + size;
	අතරතුර (pos < end) {
		rsize = pos & (align - 1);
		නම් (rsize)
			rsize = 1UL << sbi_ffs(pos);
		නැතහොත්
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = root_add_memregion(&reg);
		නම් (rc)
			ආපසු rc;
		pos += rsize;
	}

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා sbi_domain_startup(sa_nracanaa sbi_scratch *scratch, u32 cold_hartid)
{
	පූර්ණ_සංඛ්‍යා rc;
	u32 dhart;
	sa_nracanaa sbi_domain *dom;

	/* Sanity checks */
	නම් (!domain_finalized)
		ආපසු SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		නම් (!sbi_hartindex_valid(dhart))
			continue;

		/* Ignore if boot HART not possible for this domain */
		නම් (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			continue;

		/* Ignore if boot HART assigned different domain */
		නම් (sbi_hartindex_to_domain(dhart) != dom)
			continue;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		නම් (!rc)
			continue;

		/* Startup boot HART of domain */
		නම් (dom->boot_hartid == cold_hartid) {
			scratch->next_addr = dom->next_addr;
			scratch->next_mode = dom->next_mode;
			scratch->next_arg1 = dom->next_arg1;
		} නැතහොත් {
			rc = sbi_hsm_hart_start(scratch, NULL,
						dom->boot_hartid,
						dom->next_addr,
						dom->next_mode,
						dom->next_arg1);
			නම් (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				ආපසු rc;
			}
		}
	}

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා sbi_domain_finalize(sa_nracanaa sbi_scratch *scratch)
{
	පූර්ණ_සංඛ්‍යා rc;
	const sa_nracanaa sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	නම් (domain_finalized)
		ආපසු SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	නම් (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		ආපසු rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා sbi_domain_init(sa_nracanaa sbi_scratch *scratch, u32 cold_hartid)
{
	පූර්ණ_සංඛ්‍යා rc;
	sa_nracanaa sbi_hartmask *root_hmask;
	sa_nracanaa sbi_domain_memregion *root_memregs;
	පූර්ණ_සංඛ්‍යා root_memregs_count = 0;
	const sa_nracanaa sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_HEAD(&domain_list);

	නම් (!fw_single_region) {
		නම් (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			ආපසු SBI_EINVAL;
		}
		නම් ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			ආපසු SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(void *);
	නම් (!domain_hart_ptr_offset)
		ආපසු SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	නම් (rc)
		goto fail_free_domain_hart_ptr_offset;

	root_memregs = sbi_calloc(parimaa_na(*root_memregs), ROOT_REGION_MAX + 1);
	නම් (!root_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		goto fail_deinit_context;
	}
	root.regions = root_memregs;

	root_hmask = sbi_zalloc(parimaa_na(*root_hmask));
	නම් (!root_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		goto fail_free_root_memregs;
	}
	root.possible_harts = root_hmask;

	/* Root domain firmware memory region */
	නම් (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_size,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &root_memregs[root_memregs_count++]);
	} නැතහොත් {
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
	නම් (rc)
		goto fail_free_root_hmask;

	ආපසු 0;

fail_free_root_hmask:
	sbi_free(root_hmask);
fail_free_root_memregs:
	sbi_free(root_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	ආපසු rc;
}
