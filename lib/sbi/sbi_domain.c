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

स्थिर u32 domain_मोजणी = 0;
स्थिर bool domain_finalized = false;

#define मूळ_REGION_MAX	32

संरचना sbi_domain मूळ = {
	.name = "root",
	.init_order = -1U,
	.possible_harts = रिक्त,
	.regions = रिक्त,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

स्थिर अचिन्हित दीर्घ domain_hart_ptr_offset;

संरचना sbi_domain *sbi_hartindex_to_domain(u32 hartindex)
{
	संरचना sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	जर (!scratch || !domain_hart_ptr_offset)
		परत रिक्त;

	परत sbi_scratch_read_type(scratch, शून्य *, domain_hart_ptr_offset);
}

शून्य sbi_update_hartindex_to_domain(u32 hartindex, संरचना sbi_domain *dom)
{
	संरचना sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	जर (!scratch)
		परत;

	sbi_scratch_write_type(scratch, शून्य *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(स्थिरांक संरचना sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	संरचना sbi_domain *tdom = (संरचना sbi_domain *)dom;

	जर (!dom)
		परत false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	परत ret;
}

पूर्णांक sbi_domain_get_assigned_hartmask(स्थिरांक संरचना sbi_domain *dom,
				     संरचना sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	संरचना sbi_domain *tdom = (संरचना sbi_domain *)dom;

	जर (!dom) {
		sbi_hartmask_clear_all(mask);
		परत 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	परत ret;
}

शून्य sbi_domain_memregion_init(अचिन्हित दीर्घ addr,
				अचिन्हित दीर्घ size,
				अचिन्हित दीर्घ flags,
				संरचना sbi_domain_memregion *reg)
{
	अचिन्हित दीर्घ base = 0, order;

	साठी (order = log2roundup(size) ; order <= __riscv_xlen; order++) {
		जर (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			जर ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				थांबा;
		} अन्यथा {
			base = 0;
			थांबा;
		}

	}

	जर (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

अचिन्हित पूर्णांक sbi_domain_get_oldpmp_flags(संरचना sbi_domain_memregion *reg)
{

	अचिन्हित पूर्णांक pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	जर (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	जर (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	जर (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	जर (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	परत pmp_flags;
}

अचिन्हित पूर्णांक sbi_domain_get_smepmp_flags(संरचना sbi_domain_memregion *reg)
{
	अचिन्हित पूर्णांक pmp_flags = 0;
	अचिन्हित दीर्घ rstart, rend;

	जर ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_MASK) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		परत 0;
	} अन्यथा जर (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		जर (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		अन्यथा जर (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		अन्यथा जर (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		अन्यथा जर (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		अन्यथा जर (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} अन्यथा जर (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		जर ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			परत 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		जर (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		जर (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		जर (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} अन्यथा जर (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		जर (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		जर (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		जर (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} अन्यथा {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ? rstart + ((1UL << reg->order) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	परत pmp_flags;
}

bool sbi_domain_check_addr(स्थिरांक संरचना sbi_domain *dom,
			   अचिन्हित दीर्घ addr, अचिन्हित दीर्घ mode,
			   अचिन्हित दीर्घ access_flags)
{
	bool rmmio, mmio = false;
	संरचना sbi_domain_memregion *reg;
	अचिन्हित दीर्घ rstart, rend, rflags, rwx = 0, rrwx = 0;

	जर (!dom)
		परत false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	जर (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	जर (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	जर (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	जर (access_flags & SBI_DOMAIN_MMIO)
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
		जर (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			जर (!mmio && rmmio)
				परत false;
			परत ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	परत (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
स्थिर bool is_region_valid(स्थिरांक संरचना sbi_domain_memregion *reg)
{
	जर (reg->order < 3 || __riscv_xlen < reg->order)
		परत false;

	जर (reg->order == __riscv_xlen && reg->base != 0)
		परत false;

	जर (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		परत false;

	परत true;
}

/** Check if regionA can be replaced by regionB */
स्थिर bool is_region_compatible(स्थिरांक संरचना sbi_domain_memregion *regA,
				 स्थिरांक संरचना sbi_domain_memregion *regB)
{
	जर (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		परत true;

	परत false;
}

/** Check if regionA should be placed before regionB */
स्थिर bool is_region_before(स्थिरांक संरचना sbi_domain_memregion *regA,
			     स्थिरांक संरचना sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	जर (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		परत true;
	जर (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		परत false;

	जर (regA->order < regB->order)
		परत true;

	जर ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		परत true;

	परत false;
}

स्थिर स्थिरांक संरचना sbi_domain_memregion *find_region(
						स्थिरांक संरचना sbi_domain *dom,
						अचिन्हित दीर्घ addr)
{
	अचिन्हित दीर्घ rstart, rend;
	संरचना sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		जर (rstart <= addr && addr <= rend)
			परत reg;
	}

	परत रिक्त;
}

स्थिर स्थिरांक संरचना sbi_domain_memregion *find_next_subset_region(
				स्थिरांक संरचना sbi_domain *dom,
				स्थिरांक संरचना sbi_domain_memregion *reg,
				अचिन्हित दीर्घ addr)
{
	संरचना sbi_domain_memregion *sreg, *ret = रिक्त;

	sbi_domain_for_each_memregion(dom, sreg) {
		जर (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			सुरूठेवा;

		जर (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	परत ret;
}

स्थिर शून्य swap_region(संरचना sbi_domain_memregion* reg1,
			संरचना sbi_domain_memregion* reg2)
{
	संरचना sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, आकार(treg));
	sbi_memcpy(reg1, reg2, आकार(treg));
	sbi_memcpy(reg2, &treg, आकार(treg));
}

स्थिर पूर्णांक sbi_domain_used_memregions(स्थिरांक संरचना sbi_domain *dom)
{
	पूर्णांक मोजणी = 0;
	संरचना sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		मोजणी++;

	परत मोजणी;
}

स्थिर पूर्णांक sanitize_domain(संरचना sbi_domain *dom)
{
	u32 i, j, मोजणी;
	bool is_covered;
	संरचना sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	जर (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		परत SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		जर (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			परत SBI_EINVAL;
		}
	}

	/* Check memory regions */
	जर (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		परत SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		जर (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			परत SBI_EINVAL;
		}
	}

	/* Count memory regions */
	मोजणी = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	जर (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		परत SBI_EINVAL;
	}

	/* Sort the memory regions */
	साठी (i = 0; i < (मोजणी - 1); i++) {
		reg = &dom->regions[i];
		साठी (j = i + 1; j < मोजणी; j++) {
			reg1 = &dom->regions[j];

			जर (!is_region_before(reg1, reg))
				सुरूठेवा;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	साठी (i = 0; i < (मोजणी - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		साठी (j = i + 1; j < मोजणी; j++) {
			reg1 = &dom->regions[j];

			जर (is_region_compatible(reg, reg1)) {
				is_covered = true;
				थांबा;
			}
		}

		/* find a region is superset of reg, remove reg */
		जर (is_covered) {
			sbi_memmove(reg, reg + 1, आकार(*reg) * (मोजणी - i));
			मोजणी--;
		} अन्यथा
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
	जर (dom->next_mode != PRV_S &&
	    dom->next_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->next_mode);
		परत SBI_EINVAL;
	}

	/* Check next address and next mode */
	जर (!sbi_domain_check_addr(dom, dom->next_addr, dom->next_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->next_addr);
		परत SBI_EINVAL;
	}

	परत 0;
}

bool sbi_domain_check_addr_range(स्थिरांक संरचना sbi_domain *dom,
				 अचिन्हित दीर्घ addr, अचिन्हित दीर्घ size,
				 अचिन्हित दीर्घ mode,
				 अचिन्हित दीर्घ access_flags)
{
	अचिन्हित दीर्घ max = addr + size;
	स्थिरांक संरचना sbi_domain_memregion *reg, *sreg;

	जर (!dom)
		परत false;

	जर (size && max <= addr)
		परत false;

	पर्यंत (addr < max) {
		reg = find_region(dom, addr);
		जर (!reg)
			परत false;

		जर (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			परत false;

		sreg = find_next_subset_region(dom, reg, addr);
		जर (sreg)
			addr = sreg->base;
		अन्यथा जर (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		अन्यथा
			थांबा;
	}

	परत true;
}

शून्य sbi_domain_dump(स्थिरांक संरचना sbi_domain *dom, स्थिरांक अक्षर *suffix)
{
	u32 i, j, k;
	अचिन्हित दीर्घ rstart, rend;
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
		जर (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		जर (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		जर (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		जर (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		जर (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		जर (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		जर (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		जर (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->index, suffix);
	निवड (dom->next_mode) {
	स्थिती PRV_M:
		sbi_printf("M-mode\n");
		थांबा;
	स्थिती PRV_S:
		sbi_printf("S-mode\n");
		थांबा;
	स्थिती PRV_U:
		sbi_printf("U-mode\n");
		थांबा;
	पूर्वनिर्धारित:
		sbi_printf("Unknown\n");
		थांबा;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->index, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->index, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

शून्य sbi_domain_dump_all(स्थिरांक अक्षर *suffix)
{
	स्थिरांक संरचना sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

पूर्णांक sbi_domain_register(संरचना sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	संरचना sbi_domain *tdom;
	पूर्णांक rc;

	/* Sanity checks */
	जर (!dom || domain_finalized)
		परत SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		जर (tdom == dom ||
		    tdom->init_order == dom->init_order)
			परत SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	जर (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		परत rc;
	}

	sbi_list_add_tail(&dom->गाठ, &domain_list);

	/* Assign index to domain */
	dom->index = domain_मोजणी++;

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
		जर (tdom) {
			जर (tdom->init_order > dom->init_order)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			अन्यथा
				सुरूठेवा;
		}

		sbi_update_hartindex_to_domain(i, dom);
		sbi_hartmask_set_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		जर (sbi_hartindex_to_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->index, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	जर (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->गाठ);
		परत rc;
	}

	परत 0;
}

स्थिर पूर्णांक मूळ_add_memregion(स्थिरांक संरचना sbi_domain_memregion *reg)
{
	पूर्णांक rc;
	bool reg_merged;
	संरचना sbi_domain_memregion *nreg, *nreg1;
	पूर्णांक मूळ_memregs_मोजणी = sbi_domain_used_memregions(&मूळ);

	/* Sanity checks */
	जर (!reg || domain_finalized || !मूळ.regions ||
	    (मूळ_REGION_MAX <= मूळ_memregs_मोजणी))
		परत SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&मूळ, nreg) {
		जर (is_region_compatible(reg, nreg))
			परत 0;
	}

	/* Append the memregion to root memregions */
	nreg = &मूळ.regions[मूळ_memregs_मोजणी];
	sbi_memcpy(nreg, reg, आकार(*reg));
	मूळ_memregs_मोजणी++;
	मूळ.regions[मूळ_memregs_मोजणी].order = 0;

	/* Sort and optimize root regions */
	करा {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&मूळ);
		जर (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   मूळ.name, rc);
			परत rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&मूळ, nreg) {
			nreg1 = nreg + 1;
			जर (!nreg1->order)
				सुरूठेवा;

			जर (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				पूर्णांक i1 = nreg1 - मूळ.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    आकार(*nreg1) * (मूळ_memregs_मोजणी - i1));
				reg_merged = true;
				मूळ_memregs_मोजणी--;
			}
		}
	} पर्यंत (reg_merged);

	परत 0;
}

पूर्णांक sbi_domain_मूळ_add_memrange(अचिन्हित दीर्घ addr, अचिन्हित दीर्घ size,
			   अचिन्हित दीर्घ align, अचिन्हित दीर्घ region_flags)
{
	पूर्णांक rc;
	अचिन्हित दीर्घ pos, end, rsize;
	संरचना sbi_domain_memregion reg;

	pos = addr;
	end = addr + size;
	पर्यंत (pos < end) {
		rsize = pos & (align - 1);
		जर (rsize)
			rsize = 1UL << sbi_ffs(pos);
		अन्यथा
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = मूळ_add_memregion(&reg);
		जर (rc)
			परत rc;
		pos += rsize;
	}

	परत 0;
}

पूर्णांक sbi_domain_startup(संरचना sbi_scratch *scratch, u32 cold_hartid)
{
	पूर्णांक rc;
	u32 dhart;
	संरचना sbi_domain *dom;

	/* Sanity checks */
	जर (!domain_finalized)
		परत SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		जर (!sbi_hartindex_valid(dhart))
			सुरूठेवा;

		/* Ignore if boot HART not possible for this domain */
		जर (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			सुरूठेवा;

		/* Ignore if boot HART assigned different domain */
		जर (sbi_hartindex_to_domain(dhart) != dom)
			सुरूठेवा;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		जर (!rc)
			सुरूठेवा;

		/* Startup boot HART of domain */
		जर (dom->boot_hartid == cold_hartid) {
			scratch->next_addr = dom->next_addr;
			scratch->next_mode = dom->next_mode;
			scratch->next_arg1 = dom->next_arg1;
		} अन्यथा {
			rc = sbi_hsm_hart_start(scratch, रिक्त,
						dom->boot_hartid,
						dom->next_addr,
						dom->next_mode,
						dom->next_arg1);
			जर (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				परत rc;
			}
		}
	}

	परत 0;
}

पूर्णांक sbi_domain_finalize(संरचना sbi_scratch *scratch)
{
	पूर्णांक rc;
	स्थिरांक संरचना sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	जर (domain_finalized)
		परत SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	जर (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		परत rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	परत 0;
}

पूर्णांक sbi_domain_init(संरचना sbi_scratch *scratch, u32 cold_hartid)
{
	पूर्णांक rc;
	संरचना sbi_hartmask *मूळ_hmask;
	संरचना sbi_domain_memregion *मूळ_memregs;
	पूर्णांक मूळ_memregs_मोजणी = 0;
	स्थिरांक संरचना sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_HEAD(&domain_list);

	जर (!fw_single_region) {
		जर (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			परत SBI_EINVAL;
		}
		जर ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			परत SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(शून्य *);
	जर (!domain_hart_ptr_offset)
		परत SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	जर (rc)
		जा fail_free_domain_hart_ptr_offset;

	मूळ_memregs = sbi_calloc(आकार(*मूळ_memregs), मूळ_REGION_MAX + 1);
	जर (!मूळ_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		जा fail_deinit_context;
	}
	मूळ.regions = मूळ_memregs;

	मूळ_hmask = sbi_zalloc(आकार(*मूळ_hmask));
	जर (!मूळ_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		जा fail_free_मूळ_memregs;
	}
	मूळ.possible_harts = मूळ_hmask;

	/* Root domain firmware memory region */
	जर (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_size,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &मूळ_memregs[मूळ_memregs_मोजणी++]);
	} अन्यथा {
		sbi_domain_memregion_init(scratch->fw_start,
					  scratch->fw_rw_offset,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &मूळ_memregs[मूळ_memregs_मोजणी++]);

		sbi_domain_memregion_init((scratch->fw_start +
					   scratch->fw_rw_offset),
					  (scratch->fw_size -
					   scratch->fw_rw_offset),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &मूळ_memregs[मूळ_memregs_मोजणी++]);
	}

	मूळ.fw_region_inited = true;

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
				  &मूळ_memregs[मूळ_memregs_मोजणी++]);

	/* Root domain memory region end */
	मूळ_memregs[मूळ_memregs_मोजणी].order = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	मूळ.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	मूळ.next_arg1 = scratch->next_arg1;
	मूळ.next_addr = scratch->next_addr;
	मूळ.next_mode = scratch->next_mode;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_set_hartindex(i, मूळ_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_register(&मूळ);
	जर (rc)
		जा fail_free_मूळ_hmask;

	परत 0;

fail_free_मूळ_hmask:
	sbi_free(मूळ_hmask);
fail_free_मूळ_memregs:
	sbi_free(मूळ_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	परत rc;
}
