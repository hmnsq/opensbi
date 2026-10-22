/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hartmask.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hsm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_list.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_math.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>

SBI_LIST_HEAD(domain_list);

ସ୍ଥିର u32 domain_ଗଣନା = 0;
ସ୍ଥିର bool domain_finalized = false;

#define ମୂଳ_REGION_MAX	32

ସଂରଚନା sbi_domain ମୂଳ = {
	.name = "root",
	.init_order = -1U,
	.possible_harts = ଖାଲି,
	.regions = ଖାଲି,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

ସ୍ଥିର unsigned long domain_hart_ptr_offset;

ସଂରଚନା sbi_domain *sbi_hartindex_to_domain(u32 hartindex)
{
	ସଂରଚନା sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ଯଦି (!scratch || !domain_hart_ptr_offset)
		ଫେରନ୍ତୁ ଖାଲି;

	ଫେରନ୍ତୁ sbi_scratch_read_type(scratch, ଶୂନ୍ୟ *, domain_hart_ptr_offset);
}

ଶୂନ୍ୟ sbi_update_hartindex_to_domain(u32 hartindex, ସଂରଚନା sbi_domain *dom)
{
	ସଂରଚନା sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ଯଦି (!scratch)
		ଫେରନ୍ତୁ;

	sbi_scratch_write_type(scratch, ଶୂନ୍ୟ *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(ଅଚଳ ସଂରଚନା sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	ସଂରଚନା sbi_domain *tdom = (ସଂରଚନା sbi_domain *)dom;

	ଯଦି (!dom)
		ଫେରନ୍ତୁ false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_get_assigned_hartmask(ଅଚଳ ସଂରଚନା sbi_domain *dom,
				     ସଂରଚନା sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	ସଂରଚନା sbi_domain *tdom = (ସଂରଚନା sbi_domain *)dom;

	ଯଦି (!dom) {
		sbi_hartmask_clear_all(mask);
		ଫେରନ୍ତୁ 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	ଫେରନ୍ତୁ ret;
}

ଶୂନ୍ୟ sbi_domain_memregion_init(unsigned long addr,
				unsigned long size,
				unsigned long flags,
				ସଂରଚନା sbi_domain_memregion *reg)
{
	unsigned long base = 0, order;

	ପାଇଁ (order = log2roundup(size) ; order <= __riscv_xlen; order++) {
		ଯଦି (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			ଯଦି ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				ଭାଙ୍ଗ;
		} ଅନ୍ୟଥା {
			base = 0;
			ଭାଙ୍ଗ;
		}

	}

	ଯଦି (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_get_oldpmp_flags(ସଂରଚନା sbi_domain_memregion *reg)
{

	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	ଫେରନ୍ତୁ pmp_flags;
}

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_get_smepmp_flags(ସଂରଚନା sbi_domain_memregion *reg)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_flags = 0;
	unsigned long rstart, rend;

	ଯଦି ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_MASK) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		ଫେରନ୍ତୁ 0;
	} ଅନ୍ୟଥା ଯଦି (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		ଯଦି (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		ଅନ୍ୟଥା ଯଦି (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		ଅନ୍ୟଥା ଯଦି (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		ଅନ୍ୟଥା ଯଦି (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		ଅନ୍ୟଥା ଯଦି (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} ଅନ୍ୟଥା ଯଦି (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		ଯଦି ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			ଫେରନ୍ତୁ 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} ଅନ୍ୟଥା ଯଦି (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} ଅନ୍ୟଥା {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ? rstart + ((1UL << reg->order) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	ଫେରନ୍ତୁ pmp_flags;
}

bool sbi_domain_check_addr(ଅଚଳ ସଂରଚନା sbi_domain *dom,
			   unsigned long addr, unsigned long mode,
			   unsigned long access_flags)
{
	bool rmmio, mmio = false;
	ସଂରଚନା sbi_domain_memregion *reg;
	unsigned long rstart, rend, rflags, rwx = 0, rrwx = 0;

	ଯଦି (!dom)
		ଫେରନ୍ତୁ false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	ଯଦି (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	ଯଦି (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	ଯଦି (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	ଯଦି (access_flags & SBI_DOMAIN_MMIO)
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
		ଯଦି (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			ଯଦି (!mmio && rmmio)
				ଫେରନ୍ତୁ false;
			ଫେରନ୍ତୁ ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	ଫେରନ୍ତୁ (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
ସ୍ଥିର bool is_region_valid(ଅଚଳ ସଂରଚନା sbi_domain_memregion *reg)
{
	ଯଦି (reg->order < 3 || __riscv_xlen < reg->order)
		ଫେରନ୍ତୁ false;

	ଯଦି (reg->order == __riscv_xlen && reg->base != 0)
		ଫେରନ୍ତୁ false;

	ଯଦି (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		ଫେରନ୍ତୁ false;

	ଫେରନ୍ତୁ true;
}

/** Check if regionA can be replaced by regionB */
ସ୍ଥିର bool is_region_compatible(ଅଚଳ ସଂରଚନା sbi_domain_memregion *regA,
				 ଅଚଳ ସଂରଚନା sbi_domain_memregion *regB)
{
	ଯଦି (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		ଫେରନ୍ତୁ true;

	ଫେରନ୍ତୁ false;
}

/** Check if regionA should be placed before regionB */
ସ୍ଥିର bool is_region_before(ଅଚଳ ସଂରଚନା sbi_domain_memregion *regA,
			     ଅଚଳ ସଂରଚନା sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	ଯଦି (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		ଫେରନ୍ତୁ true;
	ଯଦି (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		ଫେରନ୍ତୁ false;

	ଯଦି (regA->order < regB->order)
		ଫେରନ୍ତୁ true;

	ଯଦି ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		ଫେରନ୍ତୁ true;

	ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_domain_memregion *find_region(
						ଅଚଳ ସଂରଚନା sbi_domain *dom,
						unsigned long addr)
{
	unsigned long rstart, rend;
	ସଂରଚନା sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		ଯଦି (rstart <= addr && addr <= rend)
			ଫେରନ୍ତୁ reg;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_domain_memregion *find_next_subset_region(
				ଅଚଳ ସଂରଚନା sbi_domain *dom,
				ଅଚଳ ସଂରଚନା sbi_domain_memregion *reg,
				unsigned long addr)
{
	ସଂରଚନା sbi_domain_memregion *sreg, *ret = ଖାଲି;

	sbi_domain_for_each_memregion(dom, sreg) {
		ଯଦି (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			ଜାରି;

		ଯଦି (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ଶୂନ୍ୟ swap_region(ସଂରଚନା sbi_domain_memregion* reg1,
			ସଂରଚନା sbi_domain_memregion* reg2)
{
	ସଂରଚନା sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, ଆକାର(treg));
	sbi_memcpy(reg1, reg2, ଆକାର(treg));
	sbi_memcpy(reg2, &treg, ଆକାର(treg));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_used_memregions(ଅଚଳ ସଂରଚନା sbi_domain *dom)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0;
	ସଂରଚନା sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		ଗଣନା_2++;

	ଫେରନ୍ତୁ ଗଣନା_2;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sanitize_domain(ସଂରଚନା sbi_domain *dom)
{
	u32 i, j, ଗଣନା_2;
	bool is_covered;
	ସଂରଚନା sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	ଯଦି (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		ଫେରନ୍ତୁ SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ଯଦି (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			ଫେରନ୍ତୁ SBI_EINVAL;
		}
	}

	/* Check memory regions */
	ଯଦି (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		ଫେରନ୍ତୁ SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		ଯଦି (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			ଫେରନ୍ତୁ SBI_EINVAL;
		}
	}

	/* Count memory regions */
	ଗଣନା_2 = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	ଯଦି (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		ଫେରନ୍ତୁ SBI_EINVAL;
	}

	/* Sort the memory regions */
	ପାଇଁ (i = 0; i < (ଗଣନା_2 - 1); i++) {
		reg = &dom->regions[i];
		ପାଇଁ (j = i + 1; j < ଗଣନା_2; j++) {
			reg1 = &dom->regions[j];

			ଯଦି (!is_region_before(reg1, reg))
				ଜାରି;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	ପାଇଁ (i = 0; i < (ଗଣନା_2 - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		ପାଇଁ (j = i + 1; j < ଗଣନା_2; j++) {
			reg1 = &dom->regions[j];

			ଯଦି (is_region_compatible(reg, reg1)) {
				is_covered = true;
				ଭାଙ୍ଗ;
			}
		}

		/* find a region is superset of reg, remove reg */
		ଯଦି (is_covered) {
			sbi_memmove(reg, reg + 1, ଆକାର(*reg) * (ଗଣନା_2 - i));
			ଗଣନା_2--;
		} ଅନ୍ୟଥା
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
	ଯଦି (dom->next_mode != PRV_S &&
	    dom->next_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->next_mode);
		ଫେରନ୍ତୁ SBI_EINVAL;
	}

	/* Check next address and next mode */
	ଯଦି (!sbi_domain_check_addr(dom, dom->next_addr, dom->next_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->next_addr);
		ଫେରନ୍ତୁ SBI_EINVAL;
	}

	ଫେରନ୍ତୁ 0;
}

bool sbi_domain_check_addr_range(ଅଚଳ ସଂରଚନା sbi_domain *dom,
				 unsigned long addr, unsigned long size,
				 unsigned long mode,
				 unsigned long access_flags)
{
	unsigned long max = addr + size;
	ଅଚଳ ସଂରଚନା sbi_domain_memregion *reg, *sreg;

	ଯଦି (!dom)
		ଫେରନ୍ତୁ false;

	ଯଦି (size && max <= addr)
		ଫେରନ୍ତୁ false;

	ଯେତେବେଳେ (addr < max) {
		reg = find_region(dom, addr);
		ଯଦି (!reg)
			ଫେରନ୍ତୁ false;

		ଯଦି (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			ଫେରନ୍ତୁ false;

		sreg = find_next_subset_region(dom, reg, addr);
		ଯଦି (sreg)
			addr = sreg->base;
		ଅନ୍ୟଥା ଯଦି (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		ଅନ୍ୟଥା
			ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ true;
}

ଶୂନ୍ୟ sbi_domain_dump(ଅଚଳ ସଂରଚନା sbi_domain *dom, ଅଚଳ ଅକ୍ଷର *suffix)
{
	u32 i, j, k;
	unsigned long rstart, rend;
	ସଂରଚନା sbi_domain_memregion *reg;

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
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->index, suffix);
	ବାଛ (dom->next_mode) {
	ସ୍ଥିତି PRV_M:
		sbi_printf("M-mode\n");
		ଭାଙ୍ଗ;
	ସ୍ଥିତି PRV_S:
		sbi_printf("S-mode\n");
		ଭାଙ୍ଗ;
	ସ୍ଥିତି PRV_U:
		sbi_printf("U-mode\n");
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		sbi_printf("Unknown\n");
		ଭାଙ୍ଗ;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->index, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->index, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

ଶୂନ୍ୟ sbi_domain_dump_all(ଅଚଳ ଅକ୍ଷର *suffix)
{
	ଅଚଳ ସଂରଚନା sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_register(ସଂରଚନା sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	ସଂରଚନା sbi_domain *tdom;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	/* Sanity checks */
	ଯଦି (!dom || domain_finalized)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		ଯଦି (tdom == dom ||
		    tdom->init_order == dom->init_order)
			ଫେରନ୍ତୁ SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	ଯଦି (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		ଫେରନ୍ତୁ rc;
	}

	sbi_list_add_tail(&dom->ଗଣ୍ଠି, &domain_list);

	/* Assign index to domain */
	dom->index = domain_ଗଣନା++;

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
		ଯଦି (tdom) {
			ଯଦି (tdom->init_order > dom->init_order)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			ଅନ୍ୟଥା
				ଜାରି;
		}

		sbi_update_hartindex_to_domain(i, dom);
		sbi_hartmask_set_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		ଯଦି (sbi_hartindex_to_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->index, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	ଯଦି (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->ଗଣ୍ଠି);
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_add_memregion(ଅଚଳ ସଂରଚନା sbi_domain_memregion *reg)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	bool reg_merged;
	ସଂରଚନା sbi_domain_memregion *nreg, *nreg1;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_memregs_ଗଣନା = sbi_domain_used_memregions(&ମୂଳ);

	/* Sanity checks */
	ଯଦି (!reg || domain_finalized || !ମୂଳ.regions ||
	    (ମୂଳ_REGION_MAX <= ମୂଳ_memregs_ଗଣନା))
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&ମୂଳ, nreg) {
		ଯଦି (is_region_compatible(reg, nreg))
			ଫେରନ୍ତୁ 0;
	}

	/* Append the memregion to root memregions */
	nreg = &ମୂଳ.regions[ମୂଳ_memregs_ଗଣନା];
	sbi_memcpy(nreg, reg, ଆକାର(*reg));
	ମୂଳ_memregs_ଗଣନା++;
	ମୂଳ.regions[ମୂଳ_memregs_ଗଣନା].order = 0;

	/* Sort and optimize root regions */
	କର {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&ମୂଳ);
		ଯଦି (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   ମୂଳ.name, rc);
			ଫେରନ୍ତୁ rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&ମୂଳ, nreg) {
			nreg1 = nreg + 1;
			ଯଦି (!nreg1->order)
				ଜାରି;

			ଯଦି (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା i1 = nreg1 - ମୂଳ.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    ଆକାର(*nreg1) * (ମୂଳ_memregs_ଗଣନା - i1));
				reg_merged = true;
				ମୂଳ_memregs_ଗଣନା--;
			}
		}
	} ଯେତେବେଳେ (reg_merged);

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_ମୂଳ_add_memrange(unsigned long addr, unsigned long size,
			   unsigned long align, unsigned long region_flags)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	unsigned long pos, end, rsize;
	ସଂରଚନା sbi_domain_memregion reg;

	pos = addr;
	end = addr + size;
	ଯେତେବେଳେ (pos < end) {
		rsize = pos & (align - 1);
		ଯଦି (rsize)
			rsize = 1UL << sbi_ffs(pos);
		ଅନ୍ୟଥା
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = ମୂଳ_add_memregion(&reg);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
		pos += rsize;
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_startup(ସଂରଚନା sbi_scratch *scratch, u32 cold_hartid)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	u32 dhart;
	ସଂରଚନା sbi_domain *dom;

	/* Sanity checks */
	ଯଦି (!domain_finalized)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		ଯଦି (!sbi_hartindex_valid(dhart))
			ଜାରି;

		/* Ignore if boot HART not possible for this domain */
		ଯଦି (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			ଜାରି;

		/* Ignore if boot HART assigned different domain */
		ଯଦି (sbi_hartindex_to_domain(dhart) != dom)
			ଜାରି;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		ଯଦି (!rc)
			ଜାରି;

		/* Startup boot HART of domain */
		ଯଦି (dom->boot_hartid == cold_hartid) {
			scratch->next_addr = dom->next_addr;
			scratch->next_mode = dom->next_mode;
			scratch->next_arg1 = dom->next_arg1;
		} ଅନ୍ୟଥା {
			rc = sbi_hsm_hart_start(scratch, ଖାଲି,
						dom->boot_hartid,
						dom->next_addr,
						dom->next_mode,
						dom->next_arg1);
			ଯଦି (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				ଫେରନ୍ତୁ rc;
			}
		}
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_finalize(ସଂରଚନା sbi_scratch *scratch)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ଅଚଳ ସଂରଚନା sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	ଯଦି (domain_finalized)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	ଯଦି (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		ଫେରନ୍ତୁ rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_init(ସଂରଚନା sbi_scratch *scratch, u32 cold_hartid)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା sbi_hartmask *ମୂଳ_hmask;
	ସଂରଚନା sbi_domain_memregion *ମୂଳ_memregs;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_memregs_ଗଣନା = 0;
	ଅଚଳ ସଂରଚନା sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_HEAD(&domain_list);

	ଯଦି (!fw_single_region) {
		ଯଦି (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			ଫେରନ୍ତୁ SBI_EINVAL;
		}
		ଯଦି ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			ଫେରନ୍ତୁ SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(ଶୂନ୍ୟ *);
	ଯଦି (!domain_hart_ptr_offset)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	ଯଦି (rc)
		ଯାଅ fail_free_domain_hart_ptr_offset;

	ମୂଳ_memregs = sbi_calloc(ଆକାର(*ମୂଳ_memregs), ମୂଳ_REGION_MAX + 1);
	ଯଦି (!ମୂଳ_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		ଯାଅ fail_deinit_context;
	}
	ମୂଳ.regions = ମୂଳ_memregs;

	ମୂଳ_hmask = sbi_zalloc(ଆକାର(*ମୂଳ_hmask));
	ଯଦି (!ମୂଳ_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		ଯାଅ fail_free_ମୂଳ_memregs;
	}
	ମୂଳ.possible_harts = ମୂଳ_hmask;

	/* Root domain firmware memory region */
	ଯଦି (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_size,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ମୂଳ_memregs[ମୂଳ_memregs_ଗଣନା++]);
	} ଅନ୍ୟଥା {
		sbi_domain_memregion_init(scratch->fw_start,
					  scratch->fw_rw_offset,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ମୂଳ_memregs[ମୂଳ_memregs_ଗଣନା++]);

		sbi_domain_memregion_init((scratch->fw_start +
					   scratch->fw_rw_offset),
					  (scratch->fw_size -
					   scratch->fw_rw_offset),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ମୂଳ_memregs[ମୂଳ_memregs_ଗଣନା++]);
	}

	ମୂଳ.fw_region_inited = true;

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
				  &ମୂଳ_memregs[ମୂଳ_memregs_ଗଣନା++]);

	/* Root domain memory region end */
	ମୂଳ_memregs[ମୂଳ_memregs_ଗଣନା].order = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	ମୂଳ.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	ମୂଳ.next_arg1 = scratch->next_arg1;
	ମୂଳ.next_addr = scratch->next_addr;
	ମୂଳ.next_mode = scratch->next_mode;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_set_hartindex(i, ମୂଳ_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_register(&ମୂଳ);
	ଯଦି (rc)
		ଯାଅ fail_free_ମୂଳ_hmask;

	ଫେରନ୍ତୁ 0;

fail_free_ମୂଳ_hmask:
	sbi_free(ମୂଳ_hmask);
fail_free_ମୂଳ_memregs:
	sbi_free(ମୂଳ_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	ଫେରନ୍ତୁ rc;
}
