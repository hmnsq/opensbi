/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <sbi/riscv_asm.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_domain.h>
#অন্তর্ভাৱ <sbi/sbi_hartmask.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi/sbi_hsm.h>
#অন্তর্ভাৱ <sbi/sbi_list.h>
#অন্তর্ভাৱ <sbi/sbi_math.h>
#অন্তর্ভাৱ <sbi/sbi_platform.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi/sbi_string.h>

SBI_LIST_HEAD(domain_list);

স্থির u32 domain_গুনতি = 0;
স্থির bool domain_finalized = false;

#define মূল_REGION_MAX	32

সংরচনা sbi_domain মূল = {
	.name = "root",
	.init_order = -1U,
	.possible_harts = রিক্ত,
	.regions = রিক্ত,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

স্থির unsigned long domain_hart_ptr_offset;

সংরচনা sbi_domain *sbi_hartindex_to_domain(u32 hartindex)
{
	সংরচনা sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!scratch || !domain_hart_ptr_offset)
		ঘূৰাই রিক্ত;

	ঘূৰাই sbi_scratch_read_type(scratch, শূন্য *, domain_hart_ptr_offset);
}

শূন্য sbi_update_hartindex_to_domain(u32 hartindex, সংরচনা sbi_domain *dom)
{
	সংরচনা sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!scratch)
		ঘূৰাই;

	sbi_scratch_write_type(scratch, শূন্য *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(ধ্রুৱ সংরচনা sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	সংরচনা sbi_domain *tdom = (সংরচনা sbi_domain *)dom;

	যদি (!dom)
		ঘূৰাই false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	ঘূৰাই ret;
}

পূর্ণাঙ্ক sbi_domain_get_assigned_hartmask(ধ্রুৱ সংরচনা sbi_domain *dom,
				     সংরচনা sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	সংরচনা sbi_domain *tdom = (সংরচনা sbi_domain *)dom;

	যদি (!dom) {
		sbi_hartmask_clear_all(mask);
		ঘূৰাই 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	ঘূৰাই ret;
}

শূন্য sbi_domain_memregion_init(unsigned long addr,
				unsigned long size,
				unsigned long flags,
				সংরচনা sbi_domain_memregion *reg)
{
	unsigned long base = 0, order;

	বাবে (order = log2roundup(size) ; order <= __riscv_xlen; order++) {
		যদি (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			যদি ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				ভঙ্গ;
		} অন্যথা {
			base = 0;
			ভঙ্গ;
		}

	}

	যদি (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

unsigned পূর্ণাঙ্ক sbi_domain_get_oldpmp_flags(সংরচনা sbi_domain_memregion *reg)
{

	unsigned পূর্ণাঙ্ক pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	যদি (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	যদি (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	যদি (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	যদি (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	ঘূৰাই pmp_flags;
}

unsigned পূর্ণাঙ্ক sbi_domain_get_smepmp_flags(সংরচনা sbi_domain_memregion *reg)
{
	unsigned পূর্ণাঙ্ক pmp_flags = 0;
	unsigned long rstart, rend;

	যদি ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_MASK) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		ঘূৰাই 0;
	} অন্যথা যদি (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		যদি (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		অন্যথা যদি (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		অন্যথা যদি (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		অন্যথা যদি (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		অন্যথা যদি (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} অন্যথা যদি (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		যদি ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			ঘূৰাই 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		যদি (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} অন্যথা যদি (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} অন্যথা {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ? rstart + ((1UL << reg->order) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	ঘূৰাই pmp_flags;
}

bool sbi_domain_check_addr(ধ্রুৱ সংরচনা sbi_domain *dom,
			   unsigned long addr, unsigned long mode,
			   unsigned long access_flags)
{
	bool rmmio, mmio = false;
	সংরচনা sbi_domain_memregion *reg;
	unsigned long rstart, rend, rflags, rwx = 0, rrwx = 0;

	যদি (!dom)
		ঘূৰাই false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	যদি (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	যদি (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	যদি (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	যদি (access_flags & SBI_DOMAIN_MMIO)
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
		যদি (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			যদি (!mmio && rmmio)
				ঘূৰাই false;
			ঘূৰাই ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	ঘূৰাই (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
স্থির bool is_region_valid(ধ্রুৱ সংরচনা sbi_domain_memregion *reg)
{
	যদি (reg->order < 3 || __riscv_xlen < reg->order)
		ঘূৰাই false;

	যদি (reg->order == __riscv_xlen && reg->base != 0)
		ঘূৰাই false;

	যদি (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		ঘূৰাই false;

	ঘূৰাই true;
}

/** Check if regionA can be replaced by regionB */
স্থির bool is_region_compatible(ধ্রুৱ সংরচনা sbi_domain_memregion *regA,
				 ধ্রুৱ সংরচনা sbi_domain_memregion *regB)
{
	যদি (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		ঘূৰাই true;

	ঘূৰাই false;
}

/** Check if regionA should be placed before regionB */
স্থির bool is_region_before(ধ্রুৱ সংরচনা sbi_domain_memregion *regA,
			     ধ্রুৱ সংরচনা sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	যদি (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		ঘূৰাই true;
	যদি (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		ঘূৰাই false;

	যদি (regA->order < regB->order)
		ঘূৰাই true;

	যদি ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		ঘূৰাই true;

	ঘূৰাই false;
}

স্থির ধ্রুৱ সংরচনা sbi_domain_memregion *find_region(
						ধ্রুৱ সংরচনা sbi_domain *dom,
						unsigned long addr)
{
	unsigned long rstart, rend;
	সংরচনা sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		যদি (rstart <= addr && addr <= rend)
			ঘূৰাই reg;
	}

	ঘূৰাই রিক্ত;
}

স্থির ধ্রুৱ সংরচনা sbi_domain_memregion *find_next_subset_region(
				ধ্রুৱ সংরচনা sbi_domain *dom,
				ধ্রুৱ সংরচনা sbi_domain_memregion *reg,
				unsigned long addr)
{
	সংরচনা sbi_domain_memregion *sreg, *ret = রিক্ত;

	sbi_domain_for_each_memregion(dom, sreg) {
		যদি (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			অনুৱর্ত;

		যদি (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	ঘূৰাই ret;
}

স্থির শূন্য swap_region(সংরচনা sbi_domain_memregion* reg1,
			সংরচনা sbi_domain_memregion* reg2)
{
	সংরচনা sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, পরিমাণ(treg));
	sbi_memcpy(reg1, reg2, পরিমাণ(treg));
	sbi_memcpy(reg2, &treg, পরিমাণ(treg));
}

স্থির পূর্ণাঙ্ক sbi_domain_used_memregions(ধ্রুৱ সংরচনা sbi_domain *dom)
{
	পূর্ণাঙ্ক গুনতি = 0;
	সংরচনা sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		গুনতি++;

	ঘূৰাই গুনতি;
}

স্থির পূর্ণাঙ্ক sanitize_domain(সংরচনা sbi_domain *dom)
{
	u32 i, j, গুনতি;
	bool is_covered;
	সংরচনা sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	যদি (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		ঘূৰাই SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		যদি (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			ঘূৰাই SBI_EINVAL;
		}
	}

	/* Check memory regions */
	যদি (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		ঘূৰাই SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		যদি (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			ঘূৰাই SBI_EINVAL;
		}
	}

	/* Count memory regions */
	গুনতি = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	যদি (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		ঘূৰাই SBI_EINVAL;
	}

	/* Sort the memory regions */
	বাবে (i = 0; i < (গুনতি - 1); i++) {
		reg = &dom->regions[i];
		বাবে (j = i + 1; j < গুনতি; j++) {
			reg1 = &dom->regions[j];

			যদি (!is_region_before(reg1, reg))
				অনুৱর্ত;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	বাবে (i = 0; i < (গুনতি - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		বাবে (j = i + 1; j < গুনতি; j++) {
			reg1 = &dom->regions[j];

			যদি (is_region_compatible(reg, reg1)) {
				is_covered = true;
				ভঙ্গ;
			}
		}

		/* find a region is superset of reg, remove reg */
		যদি (is_covered) {
			sbi_memmove(reg, reg + 1, পরিমাণ(*reg) * (গুনতি - i));
			গুনতি--;
		} অন্যথা
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
	যদি (dom->next_mode != PRV_S &&
	    dom->next_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->next_mode);
		ঘূৰাই SBI_EINVAL;
	}

	/* Check next address and next mode */
	যদি (!sbi_domain_check_addr(dom, dom->next_addr, dom->next_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->next_addr);
		ঘূৰাই SBI_EINVAL;
	}

	ঘূৰাই 0;
}

bool sbi_domain_check_addr_range(ধ্রুৱ সংরচনা sbi_domain *dom,
				 unsigned long addr, unsigned long size,
				 unsigned long mode,
				 unsigned long access_flags)
{
	unsigned long max = addr + size;
	ধ্রুৱ সংরচনা sbi_domain_memregion *reg, *sreg;

	যদি (!dom)
		ঘূৰাই false;

	যদি (size && max <= addr)
		ঘূৰাই false;

	যেতিয়ালৈ (addr < max) {
		reg = find_region(dom, addr);
		যদি (!reg)
			ঘূৰাই false;

		যদি (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			ঘূৰাই false;

		sreg = find_next_subset_region(dom, reg, addr);
		যদি (sreg)
			addr = sreg->base;
		অন্যথা যদি (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		অন্যথা
			ভঙ্গ;
	}

	ঘূৰাই true;
}

শূন্য sbi_domain_dump(ধ্রুৱ সংরচনা sbi_domain *dom, ধ্রুৱ ৱর্ণ *suffix)
{
	u32 i, j, k;
	unsigned long rstart, rend;
	সংরচনা sbi_domain_memregion *reg;

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
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->index, suffix);
	ৱিকল্প (dom->next_mode) {
	পক্ষ PRV_M:
		sbi_printf("M-mode\n");
		ভঙ্গ;
	পক্ষ PRV_S:
		sbi_printf("S-mode\n");
		ভঙ্গ;
	পক্ষ PRV_U:
		sbi_printf("U-mode\n");
		ভঙ্গ;
	স্ৱভাৱ:
		sbi_printf("Unknown\n");
		ভঙ্গ;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->index, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->index, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

শূন্য sbi_domain_dump_all(ধ্রুৱ ৱর্ণ *suffix)
{
	ধ্রুৱ সংরচনা sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

পূর্ণাঙ্ক sbi_domain_register(সংরচনা sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	সংরচনা sbi_domain *tdom;
	পূর্ণাঙ্ক rc;

	/* Sanity checks */
	যদি (!dom || domain_finalized)
		ঘূৰাই SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		যদি (tdom == dom ||
		    tdom->init_order == dom->init_order)
			ঘূৰাই SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	যদি (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		ঘূৰাই rc;
	}

	sbi_list_add_tail(&dom->গ্ৰন্থি, &domain_list);

	/* Assign index to domain */
	dom->index = domain_গুনতি++;

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
		যদি (tdom) {
			যদি (tdom->init_order > dom->init_order)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			অন্যথা
				অনুৱর্ত;
		}

		sbi_update_hartindex_to_domain(i, dom);
		sbi_hartmask_set_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		যদি (sbi_hartindex_to_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->index, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	যদি (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->গ্ৰন্থি);
		ঘূৰাই rc;
	}

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক মূল_add_memregion(ধ্রুৱ সংরচনা sbi_domain_memregion *reg)
{
	পূর্ণাঙ্ক rc;
	bool reg_merged;
	সংরচনা sbi_domain_memregion *nreg, *nreg1;
	পূর্ণাঙ্ক মূল_memregs_গুনতি = sbi_domain_used_memregions(&মূল);

	/* Sanity checks */
	যদি (!reg || domain_finalized || !মূল.regions ||
	    (মূল_REGION_MAX <= মূল_memregs_গুনতি))
		ঘূৰাই SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&মূল, nreg) {
		যদি (is_region_compatible(reg, nreg))
			ঘূৰাই 0;
	}

	/* Append the memregion to root memregions */
	nreg = &মূল.regions[মূল_memregs_গুনতি];
	sbi_memcpy(nreg, reg, পরিমাণ(*reg));
	মূল_memregs_গুনতি++;
	মূল.regions[মূল_memregs_গুনতি].order = 0;

	/* Sort and optimize root regions */
	কৰা {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&মূল);
		যদি (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   মূল.name, rc);
			ঘূৰাই rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&মূল, nreg) {
			nreg1 = nreg + 1;
			যদি (!nreg1->order)
				অনুৱর্ত;

			যদি (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				পূর্ণাঙ্ক i1 = nreg1 - মূল.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    পরিমাণ(*nreg1) * (মূল_memregs_গুনতি - i1));
				reg_merged = true;
				মূল_memregs_গুনতি--;
			}
		}
	} যেতিয়ালৈ (reg_merged);

	ঘূৰাই 0;
}

পূর্ণাঙ্ক sbi_domain_মূল_add_memrange(unsigned long addr, unsigned long size,
			   unsigned long align, unsigned long region_flags)
{
	পূর্ণাঙ্ক rc;
	unsigned long pos, end, rsize;
	সংরচনা sbi_domain_memregion reg;

	pos = addr;
	end = addr + size;
	যেতিয়ালৈ (pos < end) {
		rsize = pos & (align - 1);
		যদি (rsize)
			rsize = 1UL << sbi_ffs(pos);
		অন্যথা
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = মূল_add_memregion(&reg);
		যদি (rc)
			ঘূৰাই rc;
		pos += rsize;
	}

	ঘূৰাই 0;
}

পূর্ণাঙ্ক sbi_domain_startup(সংরচনা sbi_scratch *scratch, u32 cold_hartid)
{
	পূর্ণাঙ্ক rc;
	u32 dhart;
	সংরচনা sbi_domain *dom;

	/* Sanity checks */
	যদি (!domain_finalized)
		ঘূৰাই SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		যদি (!sbi_hartindex_valid(dhart))
			অনুৱর্ত;

		/* Ignore if boot HART not possible for this domain */
		যদি (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			অনুৱর্ত;

		/* Ignore if boot HART assigned different domain */
		যদি (sbi_hartindex_to_domain(dhart) != dom)
			অনুৱর্ত;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		যদি (!rc)
			অনুৱর্ত;

		/* Startup boot HART of domain */
		যদি (dom->boot_hartid == cold_hartid) {
			scratch->next_addr = dom->next_addr;
			scratch->next_mode = dom->next_mode;
			scratch->next_arg1 = dom->next_arg1;
		} অন্যথা {
			rc = sbi_hsm_hart_start(scratch, রিক্ত,
						dom->boot_hartid,
						dom->next_addr,
						dom->next_mode,
						dom->next_arg1);
			যদি (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				ঘূৰাই rc;
			}
		}
	}

	ঘূৰাই 0;
}

পূর্ণাঙ্ক sbi_domain_finalize(সংরচনা sbi_scratch *scratch)
{
	পূর্ণাঙ্ক rc;
	ধ্রুৱ সংরচনা sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	যদি (domain_finalized)
		ঘূৰাই SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	যদি (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		ঘূৰাই rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	ঘূৰাই 0;
}

পূর্ণাঙ্ক sbi_domain_init(সংরচনা sbi_scratch *scratch, u32 cold_hartid)
{
	পূর্ণাঙ্ক rc;
	সংরচনা sbi_hartmask *মূল_hmask;
	সংরচনা sbi_domain_memregion *মূল_memregs;
	পূর্ণাঙ্ক মূল_memregs_গুনতি = 0;
	ধ্রুৱ সংরচনা sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_HEAD(&domain_list);

	যদি (!fw_single_region) {
		যদি (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			ঘূৰাই SBI_EINVAL;
		}
		যদি ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			ঘূৰাই SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(শূন্য *);
	যদি (!domain_hart_ptr_offset)
		ঘূৰাই SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	যদি (rc)
		গচ্ছ fail_free_domain_hart_ptr_offset;

	মূল_memregs = sbi_calloc(পরিমাণ(*মূল_memregs), মূল_REGION_MAX + 1);
	যদি (!মূল_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		গচ্ছ fail_deinit_context;
	}
	মূল.regions = মূল_memregs;

	মূল_hmask = sbi_zalloc(পরিমাণ(*মূল_hmask));
	যদি (!মূল_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		গচ্ছ fail_free_মূল_memregs;
	}
	মূল.possible_harts = মূল_hmask;

	/* Root domain firmware memory region */
	যদি (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_size,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &মূল_memregs[মূল_memregs_গুনতি++]);
	} অন্যথা {
		sbi_domain_memregion_init(scratch->fw_start,
					  scratch->fw_rw_offset,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &মূল_memregs[মূল_memregs_গুনতি++]);

		sbi_domain_memregion_init((scratch->fw_start +
					   scratch->fw_rw_offset),
					  (scratch->fw_size -
					   scratch->fw_rw_offset),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &মূল_memregs[মূল_memregs_গুনতি++]);
	}

	মূল.fw_region_inited = true;

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
				  &মূল_memregs[মূল_memregs_গুনতি++]);

	/* Root domain memory region end */
	মূল_memregs[মূল_memregs_গুনতি].order = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	মূল.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	মূল.next_arg1 = scratch->next_arg1;
	মূল.next_addr = scratch->next_addr;
	মূল.next_mode = scratch->next_mode;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_set_hartindex(i, মূল_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_register(&মূল);
	যদি (rc)
		গচ্ছ fail_free_মূল_hmask;

	ঘূৰাই 0;

fail_free_মূল_hmask:
	sbi_free(মূল_hmask);
fail_free_মূল_memregs:
	sbi_free(মূল_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	ঘূৰাই rc;
}
