/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_hartmask.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_hsm.h>
#അന്തര്ഭാവ <sbi/sbi_list.h>
#അന്തര്ഭാവ <sbi/sbi_math.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>

SBI_LIST_HEAD(domain_list);

സ്ഥിതം u32 domain_എണ്ണം = 0;
സ്ഥിതം bool domain_finalized = false;

#define വേര്_REGION_MAX	32

ഘടന sbi_domain വേര് = {
	.name = "root",
	.init_order = -1U,
	.possible_harts = ശൂന്യ,
	.regions = ശൂന്യ,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

സ്ഥിതം unsigned long domain_hart_ptr_offset;

ഘടന sbi_domain *sbi_hartindex_to_domain(u32 hartindex)
{
	ഘടന sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	എങ്കിൽ (!scratch || !domain_hart_ptr_offset)
		തിരികെ ശൂന്യ;

	തിരികെ sbi_scratch_read_type(scratch, ശൂന്യം *, domain_hart_ptr_offset);
}

ശൂന്യം sbi_update_hartindex_to_domain(u32 hartindex, ഘടന sbi_domain *dom)
{
	ഘടന sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	എങ്കിൽ (!scratch)
		തിരികെ;

	sbi_scratch_write_type(scratch, ശൂന്യം *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(സ്ഥിരം ഘടന sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	ഘടന sbi_domain *tdom = (ഘടന sbi_domain *)dom;

	എങ്കിൽ (!dom)
		തിരികെ false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ sbi_domain_get_assigned_hartmask(സ്ഥിരം ഘടന sbi_domain *dom,
				     ഘടന sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	ഘടന sbi_domain *tdom = (ഘടന sbi_domain *)dom;

	എങ്കിൽ (!dom) {
		sbi_hartmask_clear_all(mask);
		തിരികെ 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	തിരികെ ret;
}

ശൂന്യം sbi_domain_memregion_init(unsigned long addr,
				unsigned long size,
				unsigned long flags,
				ഘടന sbi_domain_memregion *reg)
{
	unsigned long base = 0, order;

	വേണ്ടി (order = log2roundup(size) ; order <= __riscv_xlen; order++) {
		എങ്കിൽ (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			എങ്കിൽ ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				നിർത്തുക;
		} അല്ലെങ്കിൽ {
			base = 0;
			നിർത്തുക;
		}

	}

	എങ്കിൽ (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

unsigned പൂർണ്ണസംഖ്യ sbi_domain_get_oldpmp_flags(ഘടന sbi_domain_memregion *reg)
{

	unsigned പൂർണ്ണസംഖ്യ pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	തിരികെ pmp_flags;
}

unsigned പൂർണ്ണസംഖ്യ sbi_domain_get_smepmp_flags(ഘടന sbi_domain_memregion *reg)
{
	unsigned പൂർണ്ണസംഖ്യ pmp_flags = 0;
	unsigned long rstart, rend;

	എങ്കിൽ ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_MASK) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		തിരികെ 0;
	} അല്ലെങ്കിൽ എങ്കിൽ (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		എങ്കിൽ (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		അല്ലെങ്കിൽ എങ്കിൽ (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		അല്ലെങ്കിൽ എങ്കിൽ (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		അല്ലെങ്കിൽ എങ്കിൽ (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		അല്ലെങ്കിൽ എങ്കിൽ (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} അല്ലെങ്കിൽ എങ്കിൽ (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		എങ്കിൽ ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			തിരികെ 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} അല്ലെങ്കിൽ എങ്കിൽ (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} അല്ലെങ്കിൽ {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ? rstart + ((1UL << reg->order) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	തിരികെ pmp_flags;
}

bool sbi_domain_check_addr(സ്ഥിരം ഘടന sbi_domain *dom,
			   unsigned long addr, unsigned long mode,
			   unsigned long access_flags)
{
	bool rmmio, mmio = false;
	ഘടന sbi_domain_memregion *reg;
	unsigned long rstart, rend, rflags, rwx = 0, rrwx = 0;

	എങ്കിൽ (!dom)
		തിരികെ false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	എങ്കിൽ (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	എങ്കിൽ (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	എങ്കിൽ (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	എങ്കിൽ (access_flags & SBI_DOMAIN_MMIO)
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
		എങ്കിൽ (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			എങ്കിൽ (!mmio && rmmio)
				തിരികെ false;
			തിരികെ ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	തിരികെ (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
സ്ഥിതം bool is_region_valid(സ്ഥിരം ഘടന sbi_domain_memregion *reg)
{
	എങ്കിൽ (reg->order < 3 || __riscv_xlen < reg->order)
		തിരികെ false;

	എങ്കിൽ (reg->order == __riscv_xlen && reg->base != 0)
		തിരികെ false;

	എങ്കിൽ (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		തിരികെ false;

	തിരികെ true;
}

/** Check if regionA can be replaced by regionB */
സ്ഥിതം bool is_region_compatible(സ്ഥിരം ഘടന sbi_domain_memregion *regA,
				 സ്ഥിരം ഘടന sbi_domain_memregion *regB)
{
	എങ്കിൽ (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		തിരികെ true;

	തിരികെ false;
}

/** Check if regionA should be placed before regionB */
സ്ഥിതം bool is_region_before(സ്ഥിരം ഘടന sbi_domain_memregion *regA,
			     സ്ഥിരം ഘടന sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	എങ്കിൽ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		തിരികെ true;
	എങ്കിൽ (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		തിരികെ false;

	എങ്കിൽ (regA->order < regB->order)
		തിരികെ true;

	എങ്കിൽ ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		തിരികെ true;

	തിരികെ false;
}

സ്ഥിതം സ്ഥിരം ഘടന sbi_domain_memregion *find_region(
						സ്ഥിരം ഘടന sbi_domain *dom,
						unsigned long addr)
{
	unsigned long rstart, rend;
	ഘടന sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		എങ്കിൽ (rstart <= addr && addr <= rend)
			തിരികെ reg;
	}

	തിരികെ ശൂന്യ;
}

സ്ഥിതം സ്ഥിരം ഘടന sbi_domain_memregion *find_next_subset_region(
				സ്ഥിരം ഘടന sbi_domain *dom,
				സ്ഥിരം ഘടന sbi_domain_memregion *reg,
				unsigned long addr)
{
	ഘടന sbi_domain_memregion *sreg, *ret = ശൂന്യ;

	sbi_domain_for_each_memregion(dom, sreg) {
		എങ്കിൽ (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			തുടരുക;

		എങ്കിൽ (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	തിരികെ ret;
}

സ്ഥിതം ശൂന്യം swap_region(ഘടന sbi_domain_memregion* reg1,
			ഘടന sbi_domain_memregion* reg2)
{
	ഘടന sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, വലുപ്പം(treg));
	sbi_memcpy(reg1, reg2, വലുപ്പം(treg));
	sbi_memcpy(reg2, &treg, വലുപ്പം(treg));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_domain_used_memregions(സ്ഥിരം ഘടന sbi_domain *dom)
{
	പൂർണ്ണസംഖ്യ എണ്ണം = 0;
	ഘടന sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		എണ്ണം++;

	തിരികെ എണ്ണം;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sanitize_domain(ഘടന sbi_domain *dom)
{
	u32 i, j, എണ്ണം;
	bool is_covered;
	ഘടന sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	എങ്കിൽ (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		തിരികെ SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		എങ്കിൽ (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			തിരികെ SBI_EINVAL;
		}
	}

	/* Check memory regions */
	എങ്കിൽ (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		തിരികെ SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		എങ്കിൽ (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			തിരികെ SBI_EINVAL;
		}
	}

	/* Count memory regions */
	എണ്ണം = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	എങ്കിൽ (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		തിരികെ SBI_EINVAL;
	}

	/* Sort the memory regions */
	വേണ്ടി (i = 0; i < (എണ്ണം - 1); i++) {
		reg = &dom->regions[i];
		വേണ്ടി (j = i + 1; j < എണ്ണം; j++) {
			reg1 = &dom->regions[j];

			എങ്കിൽ (!is_region_before(reg1, reg))
				തുടരുക;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	വേണ്ടി (i = 0; i < (എണ്ണം - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		വേണ്ടി (j = i + 1; j < എണ്ണം; j++) {
			reg1 = &dom->regions[j];

			എങ്കിൽ (is_region_compatible(reg, reg1)) {
				is_covered = true;
				നിർത്തുക;
			}
		}

		/* find a region is superset of reg, remove reg */
		എങ്കിൽ (is_covered) {
			sbi_memmove(reg, reg + 1, വലുപ്പം(*reg) * (എണ്ണം - i));
			എണ്ണം--;
		} അല്ലെങ്കിൽ
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
	എങ്കിൽ (dom->next_mode != PRV_S &&
	    dom->next_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->next_mode);
		തിരികെ SBI_EINVAL;
	}

	/* Check next address and next mode */
	എങ്കിൽ (!sbi_domain_check_addr(dom, dom->next_addr, dom->next_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->next_addr);
		തിരികെ SBI_EINVAL;
	}

	തിരികെ 0;
}

bool sbi_domain_check_addr_range(സ്ഥിരം ഘടന sbi_domain *dom,
				 unsigned long addr, unsigned long size,
				 unsigned long mode,
				 unsigned long access_flags)
{
	unsigned long max = addr + size;
	സ്ഥിരം ഘടന sbi_domain_memregion *reg, *sreg;

	എങ്കിൽ (!dom)
		തിരികെ false;

	എങ്കിൽ (size && max <= addr)
		തിരികെ false;

	ആകുമ്പോൾ (addr < max) {
		reg = find_region(dom, addr);
		എങ്കിൽ (!reg)
			തിരികെ false;

		എങ്കിൽ (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			തിരികെ false;

		sreg = find_next_subset_region(dom, reg, addr);
		എങ്കിൽ (sreg)
			addr = sreg->base;
		അല്ലെങ്കിൽ എങ്കിൽ (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		അല്ലെങ്കിൽ
			നിർത്തുക;
	}

	തിരികെ true;
}

ശൂന്യം sbi_domain_dump(സ്ഥിരം ഘടന sbi_domain *dom, സ്ഥിരം അക്ഷരം *suffix)
{
	u32 i, j, k;
	unsigned long rstart, rend;
	ഘടന sbi_domain_memregion *reg;

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
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->index, suffix);
	തിരഞ്ഞെടുപ്പ് (dom->next_mode) {
	അവസ്ഥ PRV_M:
		sbi_printf("M-mode\n");
		നിർത്തുക;
	അവസ്ഥ PRV_S:
		sbi_printf("S-mode\n");
		നിർത്തുക;
	അവസ്ഥ PRV_U:
		sbi_printf("U-mode\n");
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		sbi_printf("Unknown\n");
		നിർത്തുക;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->index, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->index, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

ശൂന്യം sbi_domain_dump_all(സ്ഥിരം അക്ഷരം *suffix)
{
	സ്ഥിരം ഘടന sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

പൂർണ്ണസംഖ്യ sbi_domain_register(ഘടന sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	ഘടന sbi_domain *tdom;
	പൂർണ്ണസംഖ്യ rc;

	/* Sanity checks */
	എങ്കിൽ (!dom || domain_finalized)
		തിരികെ SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		എങ്കിൽ (tdom == dom ||
		    tdom->init_order == dom->init_order)
			തിരികെ SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	എങ്കിൽ (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		തിരികെ rc;
	}

	sbi_list_add_tail(&dom->കണ്ണി, &domain_list);

	/* Assign index to domain */
	dom->index = domain_എണ്ണം++;

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
		എങ്കിൽ (tdom) {
			എങ്കിൽ (tdom->init_order > dom->init_order)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			അല്ലെങ്കിൽ
				തുടരുക;
		}

		sbi_update_hartindex_to_domain(i, dom);
		sbi_hartmask_set_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		എങ്കിൽ (sbi_hartindex_to_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->index, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	എങ്കിൽ (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->കണ്ണി);
		തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ വേര്_add_memregion(സ്ഥിരം ഘടന sbi_domain_memregion *reg)
{
	പൂർണ്ണസംഖ്യ rc;
	bool reg_merged;
	ഘടന sbi_domain_memregion *nreg, *nreg1;
	പൂർണ്ണസംഖ്യ വേര്_memregs_എണ്ണം = sbi_domain_used_memregions(&വേര്);

	/* Sanity checks */
	എങ്കിൽ (!reg || domain_finalized || !വേര്.regions ||
	    (വേര്_REGION_MAX <= വേര്_memregs_എണ്ണം))
		തിരികെ SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&വേര്, nreg) {
		എങ്കിൽ (is_region_compatible(reg, nreg))
			തിരികെ 0;
	}

	/* Append the memregion to root memregions */
	nreg = &വേര്.regions[വേര്_memregs_എണ്ണം];
	sbi_memcpy(nreg, reg, വലുപ്പം(*reg));
	വേര്_memregs_എണ്ണം++;
	വേര്.regions[വേര്_memregs_എണ്ണം].order = 0;

	/* Sort and optimize root regions */
	ചെയ്യുക {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&വേര്);
		എങ്കിൽ (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   വേര്.name, rc);
			തിരികെ rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&വേര്, nreg) {
			nreg1 = nreg + 1;
			എങ്കിൽ (!nreg1->order)
				തുടരുക;

			എങ്കിൽ (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				പൂർണ്ണസംഖ്യ i1 = nreg1 - വേര്.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    വലുപ്പം(*nreg1) * (വേര്_memregs_എണ്ണം - i1));
				reg_merged = true;
				വേര്_memregs_എണ്ണം--;
			}
		}
	} ആകുമ്പോൾ (reg_merged);

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_domain_വേര്_add_memrange(unsigned long addr, unsigned long size,
			   unsigned long align, unsigned long region_flags)
{
	പൂർണ്ണസംഖ്യ rc;
	unsigned long pos, end, rsize;
	ഘടന sbi_domain_memregion reg;

	pos = addr;
	end = addr + size;
	ആകുമ്പോൾ (pos < end) {
		rsize = pos & (align - 1);
		എങ്കിൽ (rsize)
			rsize = 1UL << sbi_ffs(pos);
		അല്ലെങ്കിൽ
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = വേര്_add_memregion(&reg);
		എങ്കിൽ (rc)
			തിരികെ rc;
		pos += rsize;
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_domain_startup(ഘടന sbi_scratch *scratch, u32 cold_hartid)
{
	പൂർണ്ണസംഖ്യ rc;
	u32 dhart;
	ഘടന sbi_domain *dom;

	/* Sanity checks */
	എങ്കിൽ (!domain_finalized)
		തിരികെ SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		എങ്കിൽ (!sbi_hartindex_valid(dhart))
			തുടരുക;

		/* Ignore if boot HART not possible for this domain */
		എങ്കിൽ (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			തുടരുക;

		/* Ignore if boot HART assigned different domain */
		എങ്കിൽ (sbi_hartindex_to_domain(dhart) != dom)
			തുടരുക;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		എങ്കിൽ (!rc)
			തുടരുക;

		/* Startup boot HART of domain */
		എങ്കിൽ (dom->boot_hartid == cold_hartid) {
			scratch->next_addr = dom->next_addr;
			scratch->next_mode = dom->next_mode;
			scratch->next_arg1 = dom->next_arg1;
		} അല്ലെങ്കിൽ {
			rc = sbi_hsm_hart_start(scratch, ശൂന്യ,
						dom->boot_hartid,
						dom->next_addr,
						dom->next_mode,
						dom->next_arg1);
			എങ്കിൽ (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				തിരികെ rc;
			}
		}
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_domain_finalize(ഘടന sbi_scratch *scratch)
{
	പൂർണ്ണസംഖ്യ rc;
	സ്ഥിരം ഘടന sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	എങ്കിൽ (domain_finalized)
		തിരികെ SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	എങ്കിൽ (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		തിരികെ rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_domain_init(ഘടന sbi_scratch *scratch, u32 cold_hartid)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന sbi_hartmask *വേര്_hmask;
	ഘടന sbi_domain_memregion *വേര്_memregs;
	പൂർണ്ണസംഖ്യ വേര്_memregs_എണ്ണം = 0;
	സ്ഥിരം ഘടന sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_HEAD(&domain_list);

	എങ്കിൽ (!fw_single_region) {
		എങ്കിൽ (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			തിരികെ SBI_EINVAL;
		}
		എങ്കിൽ ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			തിരികെ SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(ശൂന്യം *);
	എങ്കിൽ (!domain_hart_ptr_offset)
		തിരികെ SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	എങ്കിൽ (rc)
		പോകുക fail_free_domain_hart_ptr_offset;

	വേര്_memregs = sbi_calloc(വലുപ്പം(*വേര്_memregs), വേര്_REGION_MAX + 1);
	എങ്കിൽ (!വേര്_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		പോകുക fail_deinit_context;
	}
	വേര്.regions = വേര്_memregs;

	വേര്_hmask = sbi_zalloc(വലുപ്പം(*വേര്_hmask));
	എങ്കിൽ (!വേര്_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		പോകുക fail_free_വേര്_memregs;
	}
	വേര്.possible_harts = വേര്_hmask;

	/* Root domain firmware memory region */
	എങ്കിൽ (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_size,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &വേര്_memregs[വേര്_memregs_എണ്ണം++]);
	} അല്ലെങ്കിൽ {
		sbi_domain_memregion_init(scratch->fw_start,
					  scratch->fw_rw_offset,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &വേര്_memregs[വേര്_memregs_എണ്ണം++]);

		sbi_domain_memregion_init((scratch->fw_start +
					   scratch->fw_rw_offset),
					  (scratch->fw_size -
					   scratch->fw_rw_offset),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &വേര്_memregs[വേര്_memregs_എണ്ണം++]);
	}

	വേര്.fw_region_inited = true;

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
				  &വേര്_memregs[വേര്_memregs_എണ്ണം++]);

	/* Root domain memory region end */
	വേര്_memregs[വേര്_memregs_എണ്ണം].order = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	വേര്.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	വേര്.next_arg1 = scratch->next_arg1;
	വേര്.next_addr = scratch->next_addr;
	വേര്.next_mode = scratch->next_mode;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_set_hartindex(i, വേര്_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_register(&വേര്);
	എങ്കിൽ (rc)
		പോകുക fail_free_വേര്_hmask;

	തിരികെ 0;

fail_free_വേര്_hmask:
	sbi_free(വേര്_hmask);
fail_free_വേര്_memregs:
	sbi_free(വേര്_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	തിരികെ rc;
}
