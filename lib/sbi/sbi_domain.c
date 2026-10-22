/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_hartmask.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_hsm.h>
#अन्तर्भाव <sbi/sbi_list.h>
#अन्तर्भाव <sbi/sbi_math.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_string.h>

SBI_LIST_HEAD(domain_list);

स्थिर u32 domain_गणनम् = 0;
स्थिर bool domain_finalized = false;

#define मूलम्_REGION_MAX	32

संरचना sbi_domain मूलम् = {
	.name = "root",
	.init_order = -1U,
	.possible_harts = रिक्त,
	.regions = रिक्त,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

स्थिर unsigned long domain_hart_ptr_offset;

संरचना sbi_domain *sbi_hartindex_to_domain(u32 hartindex)
{
	संरचना sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	सचे (!scratch || !domain_hart_ptr_offset)
		पटिनिवत्त रिक्त;

	पटिनिवत्त sbi_scratch_read_type(scratch, शून्य *, domain_hart_ptr_offset);
}

शून्य sbi_update_hartindex_to_domain(u32 hartindex, संरचना sbi_domain *dom)
{
	संरचना sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	सचे (!scratch)
		पटिनिवत्त;

	sbi_scratch_write_type(scratch, शून्य *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(ध्रुव संरचना sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	संरचना sbi_domain *tdom = (संरचना sbi_domain *)dom;

	सचे (!dom)
		पटिनिवत्त false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	पटिनिवत्त ret;
}

पूर्णाङ्क sbi_domain_get_assigned_hartmask(ध्रुव संरचना sbi_domain *dom,
				     संरचना sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	संरचना sbi_domain *tdom = (संरचना sbi_domain *)dom;

	सचे (!dom) {
		sbi_hartmask_clear_all(mask);
		पटिनिवत्त 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	पटिनिवत्त ret;
}

शून्य sbi_domain_memregion_init(unsigned long addr,
				unsigned long size,
				unsigned long flags,
				संरचना sbi_domain_memregion *reg)
{
	unsigned long base = 0, order;

	अत्थाय (order = log2roundup(size) ; order <= __riscv_xlen; order++) {
		सचे (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			सचे ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				भङ्ग;
		} अञ्ञथा {
			base = 0;
			भङ्ग;
		}

	}

	सचे (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

unsigned पूर्णाङ्क sbi_domain_get_oldpmp_flags(संरचना sbi_domain_memregion *reg)
{

	unsigned पूर्णाङ्क pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	सचे (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	सचे (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	सचे (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	सचे (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	पटिनिवत्त pmp_flags;
}

unsigned पूर्णाङ्क sbi_domain_get_smepmp_flags(संरचना sbi_domain_memregion *reg)
{
	unsigned पूर्णाङ्क pmp_flags = 0;
	unsigned long rstart, rend;

	सचे ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_MASK) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		पटिनिवत्त 0;
	} अञ्ञथा सचे (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		सचे (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		अञ्ञथा सचे (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		अञ्ञथा सचे (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		अञ्ञथा सचे (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		अञ्ञथा सचे (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} अञ्ञथा सचे (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		सचे ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			पटिनिवत्त 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		सचे (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		सचे (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		सचे (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} अञ्ञथा सचे (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		सचे (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		सचे (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		सचे (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} अञ्ञथा {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ? rstart + ((1UL << reg->order) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	पटिनिवत्त pmp_flags;
}

bool sbi_domain_check_addr(ध्रुव संरचना sbi_domain *dom,
			   unsigned long addr, unsigned long mode,
			   unsigned long access_flags)
{
	bool rmmio, mmio = false;
	संरचना sbi_domain_memregion *reg;
	unsigned long rstart, rend, rflags, rwx = 0, rrwx = 0;

	सचे (!dom)
		पटिनिवत्त false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	सचे (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	सचे (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	सचे (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	सचे (access_flags & SBI_DOMAIN_MMIO)
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
		सचे (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			सचे (!mmio && rmmio)
				पटिनिवत्त false;
			पटिनिवत्त ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	पटिनिवत्त (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
स्थिर bool is_region_valid(ध्रुव संरचना sbi_domain_memregion *reg)
{
	सचे (reg->order < 3 || __riscv_xlen < reg->order)
		पटिनिवत्त false;

	सचे (reg->order == __riscv_xlen && reg->base != 0)
		पटिनिवत्त false;

	सचे (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		पटिनिवत्त false;

	पटिनिवत्त true;
}

/** Check if regionA can be replaced by regionB */
स्थिर bool is_region_compatible(ध्रुव संरचना sbi_domain_memregion *regA,
				 ध्रुव संरचना sbi_domain_memregion *regB)
{
	सचे (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		पटिनिवत्त true;

	पटिनिवत्त false;
}

/** Check if regionA should be placed before regionB */
स्थिर bool is_region_before(ध्रुव संरचना sbi_domain_memregion *regA,
			     ध्रुव संरचना sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	सचे (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		पटिनिवत्त true;
	सचे (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		पटिनिवत्त false;

	सचे (regA->order < regB->order)
		पटिनिवत्त true;

	सचे ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		पटिनिवत्त true;

	पटिनिवत्त false;
}

स्थिर ध्रुव संरचना sbi_domain_memregion *find_region(
						ध्रुव संरचना sbi_domain *dom,
						unsigned long addr)
{
	unsigned long rstart, rend;
	संरचना sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		सचे (rstart <= addr && addr <= rend)
			पटिनिवत्त reg;
	}

	पटिनिवत्त रिक्त;
}

स्थिर ध्रुव संरचना sbi_domain_memregion *find_next_subset_region(
				ध्रुव संरचना sbi_domain *dom,
				ध्रुव संरचना sbi_domain_memregion *reg,
				unsigned long addr)
{
	संरचना sbi_domain_memregion *sreg, *ret = रिक्त;

	sbi_domain_for_each_memregion(dom, sreg) {
		सचे (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			अनुवर्त;

		सचे (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	पटिनिवत्त ret;
}

स्थिर शून्य swap_region(संरचना sbi_domain_memregion* reg1,
			संरचना sbi_domain_memregion* reg2)
{
	संरचना sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, परिमाण(treg));
	sbi_memcpy(reg1, reg2, परिमाण(treg));
	sbi_memcpy(reg2, &treg, परिमाण(treg));
}

स्थिर पूर्णाङ्क sbi_domain_used_memregions(ध्रुव संरचना sbi_domain *dom)
{
	पूर्णाङ्क गणनम् = 0;
	संरचना sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		गणनम्++;

	पटिनिवत्त गणनम्;
}

स्थिर पूर्णाङ्क sanitize_domain(संरचना sbi_domain *dom)
{
	u32 i, j, गणनम्;
	bool is_covered;
	संरचना sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	सचे (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		पटिनिवत्त SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		सचे (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			पटिनिवत्त SBI_EINVAL;
		}
	}

	/* Check memory regions */
	सचे (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		पटिनिवत्त SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		सचे (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			पटिनिवत्त SBI_EINVAL;
		}
	}

	/* Count memory regions */
	गणनम् = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	सचे (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		पटिनिवत्त SBI_EINVAL;
	}

	/* Sort the memory regions */
	अत्थाय (i = 0; i < (गणनम् - 1); i++) {
		reg = &dom->regions[i];
		अत्थाय (j = i + 1; j < गणनम्; j++) {
			reg1 = &dom->regions[j];

			सचे (!is_region_before(reg1, reg))
				अनुवर्त;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	अत्थाय (i = 0; i < (गणनम् - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		अत्थाय (j = i + 1; j < गणनम्; j++) {
			reg1 = &dom->regions[j];

			सचे (is_region_compatible(reg, reg1)) {
				is_covered = true;
				भङ्ग;
			}
		}

		/* find a region is superset of reg, remove reg */
		सचे (is_covered) {
			sbi_memmove(reg, reg + 1, परिमाण(*reg) * (गणनम् - i));
			गणनम्--;
		} अञ्ञथा
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
	सचे (dom->next_mode != PRV_S &&
	    dom->next_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->next_mode);
		पटिनिवत्त SBI_EINVAL;
	}

	/* Check next address and next mode */
	सचे (!sbi_domain_check_addr(dom, dom->next_addr, dom->next_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->next_addr);
		पटिनिवत्त SBI_EINVAL;
	}

	पटिनिवत्त 0;
}

bool sbi_domain_check_addr_range(ध्रुव संरचना sbi_domain *dom,
				 unsigned long addr, unsigned long size,
				 unsigned long mode,
				 unsigned long access_flags)
{
	unsigned long max = addr + size;
	ध्रुव संरचना sbi_domain_memregion *reg, *sreg;

	सचे (!dom)
		पटिनिवत्त false;

	सचे (size && max <= addr)
		पटिनिवत्त false;

	याव (addr < max) {
		reg = find_region(dom, addr);
		सचे (!reg)
			पटिनिवत्त false;

		सचे (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			पटिनिवत्त false;

		sreg = find_next_subset_region(dom, reg, addr);
		सचे (sreg)
			addr = sreg->base;
		अञ्ञथा सचे (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		अञ्ञथा
			भङ्ग;
	}

	पटिनिवत्त true;
}

शून्य sbi_domain_dump(ध्रुव संरचना sbi_domain *dom, ध्रुव वर्ण *suffix)
{
	u32 i, j, k;
	unsigned long rstart, rend;
	संरचना sbi_domain_memregion *reg;

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
		सचे (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		सचे (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		सचे (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		सचे (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		सचे (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		सचे (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		सचे (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		सचे (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->index, suffix);
	विकल्प (dom->next_mode) {
	पक्ष PRV_M:
		sbi_printf("M-mode\n");
		भङ्ग;
	पक्ष PRV_S:
		sbi_printf("S-mode\n");
		भङ्ग;
	पक्ष PRV_U:
		sbi_printf("U-mode\n");
		भङ्ग;
	स्वभाव:
		sbi_printf("Unknown\n");
		भङ्ग;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->index, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->index, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

शून्य sbi_domain_dump_all(ध्रुव वर्ण *suffix)
{
	ध्रुव संरचना sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

पूर्णाङ्क sbi_domain_register(संरचना sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	संरचना sbi_domain *tdom;
	पूर्णाङ्क rc;

	/* Sanity checks */
	सचे (!dom || domain_finalized)
		पटिनिवत्त SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		सचे (tdom == dom ||
		    tdom->init_order == dom->init_order)
			पटिनिवत्त SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	सचे (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		पटिनिवत्त rc;
	}

	sbi_list_add_tail(&dom->ग्रन्थिः, &domain_list);

	/* Assign index to domain */
	dom->index = domain_गणनम्++;

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
		सचे (tdom) {
			सचे (tdom->init_order > dom->init_order)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			अञ्ञथा
				अनुवर्त;
		}

		sbi_update_hartindex_to_domain(i, dom);
		sbi_hartmask_set_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		सचे (sbi_hartindex_to_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->index, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	सचे (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->ग्रन्थिः);
		पटिनिवत्त rc;
	}

	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क मूलम्_add_memregion(ध्रुव संरचना sbi_domain_memregion *reg)
{
	पूर्णाङ्क rc;
	bool reg_merged;
	संरचना sbi_domain_memregion *nreg, *nreg1;
	पूर्णाङ्क मूलम्_memregs_गणनम् = sbi_domain_used_memregions(&मूलम्);

	/* Sanity checks */
	सचे (!reg || domain_finalized || !मूलम्.regions ||
	    (मूलम्_REGION_MAX <= मूलम्_memregs_गणनम्))
		पटिनिवत्त SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&मूलम्, nreg) {
		सचे (is_region_compatible(reg, nreg))
			पटिनिवत्त 0;
	}

	/* Append the memregion to root memregions */
	nreg = &मूलम्.regions[मूलम्_memregs_गणनम्];
	sbi_memcpy(nreg, reg, परिमाण(*reg));
	मूलम्_memregs_गणनम्++;
	मूलम्.regions[मूलम्_memregs_गणनम्].order = 0;

	/* Sort and optimize root regions */
	करोहि {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&मूलम्);
		सचे (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   मूलम्.name, rc);
			पटिनिवत्त rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&मूलम्, nreg) {
			nreg1 = nreg + 1;
			सचे (!nreg1->order)
				अनुवर्त;

			सचे (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				पूर्णाङ्क i1 = nreg1 - मूलम्.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    परिमाण(*nreg1) * (मूलम्_memregs_गणनम् - i1));
				reg_merged = true;
				मूलम्_memregs_गणनम्--;
			}
		}
	} याव (reg_merged);

	पटिनिवत्त 0;
}

पूर्णाङ्क sbi_domain_मूलम्_add_memrange(unsigned long addr, unsigned long size,
			   unsigned long align, unsigned long region_flags)
{
	पूर्णाङ्क rc;
	unsigned long pos, end, rsize;
	संरचना sbi_domain_memregion reg;

	pos = addr;
	end = addr + size;
	याव (pos < end) {
		rsize = pos & (align - 1);
		सचे (rsize)
			rsize = 1UL << sbi_ffs(pos);
		अञ्ञथा
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = मूलम्_add_memregion(&reg);
		सचे (rc)
			पटिनिवत्त rc;
		pos += rsize;
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क sbi_domain_startup(संरचना sbi_scratch *scratch, u32 cold_hartid)
{
	पूर्णाङ्क rc;
	u32 dhart;
	संरचना sbi_domain *dom;

	/* Sanity checks */
	सचे (!domain_finalized)
		पटिनिवत्त SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		सचे (!sbi_hartindex_valid(dhart))
			अनुवर्त;

		/* Ignore if boot HART not possible for this domain */
		सचे (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			अनुवर्त;

		/* Ignore if boot HART assigned different domain */
		सचे (sbi_hartindex_to_domain(dhart) != dom)
			अनुवर्त;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		सचे (!rc)
			अनुवर्त;

		/* Startup boot HART of domain */
		सचे (dom->boot_hartid == cold_hartid) {
			scratch->next_addr = dom->next_addr;
			scratch->next_mode = dom->next_mode;
			scratch->next_arg1 = dom->next_arg1;
		} अञ्ञथा {
			rc = sbi_hsm_hart_start(scratch, रिक्त,
						dom->boot_hartid,
						dom->next_addr,
						dom->next_mode,
						dom->next_arg1);
			सचे (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				पटिनिवत्त rc;
			}
		}
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क sbi_domain_finalize(संरचना sbi_scratch *scratch)
{
	पूर्णाङ्क rc;
	ध्रुव संरचना sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	सचे (domain_finalized)
		पटिनिवत्त SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	सचे (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		पटिनिवत्त rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	पटिनिवत्त 0;
}

पूर्णाङ्क sbi_domain_init(संरचना sbi_scratch *scratch, u32 cold_hartid)
{
	पूर्णाङ्क rc;
	संरचना sbi_hartmask *मूलम्_hmask;
	संरचना sbi_domain_memregion *मूलम्_memregs;
	पूर्णाङ्क मूलम्_memregs_गणनम् = 0;
	ध्रुव संरचना sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_HEAD(&domain_list);

	सचे (!fw_single_region) {
		सचे (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			पटिनिवत्त SBI_EINVAL;
		}
		सचे ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			पटिनिवत्त SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(शून्य *);
	सचे (!domain_hart_ptr_offset)
		पटिनिवत्त SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	सचे (rc)
		गच्छ fail_free_domain_hart_ptr_offset;

	मूलम्_memregs = sbi_calloc(परिमाण(*मूलम्_memregs), मूलम्_REGION_MAX + 1);
	सचे (!मूलम्_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		गच्छ fail_deinit_context;
	}
	मूलम्.regions = मूलम्_memregs;

	मूलम्_hmask = sbi_zalloc(परिमाण(*मूलम्_hmask));
	सचे (!मूलम्_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		गच्छ fail_free_मूलम्_memregs;
	}
	मूलम्.possible_harts = मूलम्_hmask;

	/* Root domain firmware memory region */
	सचे (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_size,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &मूलम्_memregs[मूलम्_memregs_गणनम्++]);
	} अञ्ञथा {
		sbi_domain_memregion_init(scratch->fw_start,
					  scratch->fw_rw_offset,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &मूलम्_memregs[मूलम्_memregs_गणनम्++]);

		sbi_domain_memregion_init((scratch->fw_start +
					   scratch->fw_rw_offset),
					  (scratch->fw_size -
					   scratch->fw_rw_offset),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &मूलम्_memregs[मूलम्_memregs_गणनम्++]);
	}

	मूलम्.fw_region_inited = true;

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
				  &मूलम्_memregs[मूलम्_memregs_गणनम्++]);

	/* Root domain memory region end */
	मूलम्_memregs[मूलम्_memregs_गणनम्].order = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	मूलम्.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	मूलम्.next_arg1 = scratch->next_arg1;
	मूलम्.next_addr = scratch->next_addr;
	मूलम्.next_mode = scratch->next_mode;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_set_hartindex(i, मूलम्_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_register(&मूलम्);
	सचे (rc)
		गच्छ fail_free_मूलम्_hmask;

	पटिनिवत्त 0;

fail_free_मूलम्_hmask:
	sbi_free(मूलम्_hmask);
fail_free_मूलम्_memregs:
	sbi_free(मूलम्_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	पटिनिवत्त rc;
}
