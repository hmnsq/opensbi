/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_hartmask.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <sbi/sbi_list.h>
#समावेश <sbi/sbi_math.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_string.h>

SBI_LIST_शीर्ष(domain_list);

अटल u32 domain_गिनती = 0;
अटल bool domain_finalized = false;

#घोषणा मूल_REGION_MAX	32

काष्ठा sbi_domain मूल = {
	.name = "root",
	.init_order = -1U,
	.possible_harts = शून्य,
	.regions = शून्य,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

अटल अचिन्हित दीर्घ domain_hart_ptr_offset;

काष्ठा sbi_domain *sbi_hartindex_to_domain(u32 hartindex)
{
	काष्ठा sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	अगर (!scratch || !domain_hart_ptr_offset)
		वापस शून्य;

	वापस sbi_scratch_read_type(scratch, व्योम *, domain_hart_ptr_offset);
}

व्योम sbi_update_hartindex_to_domain(u32 hartindex, काष्ठा sbi_domain *dom)
{
	काष्ठा sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	अगर (!scratch)
		वापस;

	sbi_scratch_write_type(scratch, व्योम *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(स्थिर काष्ठा sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	काष्ठा sbi_domain *tdom = (काष्ठा sbi_domain *)dom;

	अगर (!dom)
		वापस false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	वापस ret;
}

पूर्णांक sbi_domain_get_assigned_hartmask(स्थिर काष्ठा sbi_domain *dom,
				     काष्ठा sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	काष्ठा sbi_domain *tdom = (काष्ठा sbi_domain *)dom;

	अगर (!dom) {
		sbi_hartmask_clear_all(mask);
		वापस 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	वापस ret;
}

व्योम sbi_domain_memregion_init(अचिन्हित दीर्घ addr,
				अचिन्हित दीर्घ आकार,
				अचिन्हित दीर्घ flags,
				काष्ठा sbi_domain_memregion *reg)
{
	अचिन्हित दीर्घ base = 0, order;

	क्रम (order = log2roundup(आकार) ; order <= __riscv_xlen; order++) {
		अगर (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			अगर ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + आकार - 1UL)) &&
			    ((addr + आकार - 1UL) < (base + (1UL << order))))
				अवरोध;
		} अन्यथा {
			base = 0;
			अवरोध;
		}

	}

	अगर (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

अचिन्हित पूर्णांक sbi_domain_get_oldpmp_flags(काष्ठा sbi_domain_memregion *reg)
{

	अचिन्हित पूर्णांक pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	अगर (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	अगर (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	अगर (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	अगर (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	वापस pmp_flags;
}

अचिन्हित पूर्णांक sbi_domain_get_smepmp_flags(काष्ठा sbi_domain_memregion *reg)
{
	अचिन्हित पूर्णांक pmp_flags = 0;
	अचिन्हित दीर्घ rstart, rend;

	अगर ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_MASK) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		वापस 0;
	} अन्यथा अगर (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		अगर (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		अन्यथा अगर (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		अन्यथा अगर (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		अन्यथा अगर (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		अन्यथा अगर (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} अन्यथा अगर (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		अगर ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			वापस 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		अगर (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} अन्यथा अगर (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} अन्यथा {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ? rstart + ((1UL << reg->order) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	वापस pmp_flags;
}

bool sbi_domain_check_addr(स्थिर काष्ठा sbi_domain *dom,
			   अचिन्हित दीर्घ addr, अचिन्हित दीर्घ mode,
			   अचिन्हित दीर्घ access_flags)
{
	bool rmmio, mmio = false;
	काष्ठा sbi_domain_memregion *reg;
	अचिन्हित दीर्घ rstart, rend, rflags, rwx = 0, rrwx = 0;

	अगर (!dom)
		वापस false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	अगर (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	अगर (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	अगर (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	अगर (access_flags & SBI_DOMAIN_MMIO)
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
		अगर (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			अगर (!mmio && rmmio)
				वापस false;
			वापस ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	वापस (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
अटल bool is_region_valid(स्थिर काष्ठा sbi_domain_memregion *reg)
{
	अगर (reg->order < 3 || __riscv_xlen < reg->order)
		वापस false;

	अगर (reg->order == __riscv_xlen && reg->base != 0)
		वापस false;

	अगर (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		वापस false;

	वापस true;
}

/** Check if regionA can be replaced by regionB */
अटल bool is_region_compatible(स्थिर काष्ठा sbi_domain_memregion *regA,
				 स्थिर काष्ठा sbi_domain_memregion *regB)
{
	अगर (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		वापस true;

	वापस false;
}

/** Check if regionA should be placed before regionB */
अटल bool is_region_before(स्थिर काष्ठा sbi_domain_memregion *regA,
			     स्थिर काष्ठा sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	अगर (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		वापस true;
	अगर (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		वापस false;

	अगर (regA->order < regB->order)
		वापस true;

	अगर ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		वापस true;

	वापस false;
}

अटल स्थिर काष्ठा sbi_domain_memregion *find_region(
						स्थिर काष्ठा sbi_domain *dom,
						अचिन्हित दीर्घ addr)
{
	अचिन्हित दीर्घ rstart, rend;
	काष्ठा sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		अगर (rstart <= addr && addr <= rend)
			वापस reg;
	}

	वापस शून्य;
}

अटल स्थिर काष्ठा sbi_domain_memregion *find_अग्र_subset_region(
				स्थिर काष्ठा sbi_domain *dom,
				स्थिर काष्ठा sbi_domain_memregion *reg,
				अचिन्हित दीर्घ addr)
{
	काष्ठा sbi_domain_memregion *sreg, *ret = शून्य;

	sbi_domain_for_each_memregion(dom, sreg) {
		अगर (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			जारी;

		अगर (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	वापस ret;
}

अटल व्योम swap_region(काष्ठा sbi_domain_memregion* reg1,
			काष्ठा sbi_domain_memregion* reg2)
{
	काष्ठा sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, माप(treg));
	sbi_memcpy(reg1, reg2, माप(treg));
	sbi_memcpy(reg2, &treg, माप(treg));
}

अटल पूर्णांक sbi_domain_used_memregions(स्थिर काष्ठा sbi_domain *dom)
{
	पूर्णांक गिनती = 0;
	काष्ठा sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		गिनती++;

	वापस गिनती;
}

अटल पूर्णांक sanitize_domain(काष्ठा sbi_domain *dom)
{
	u32 i, j, गिनती;
	bool is_covered;
	काष्ठा sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	अगर (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		वापस SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		अगर (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			वापस SBI_EINVAL;
		}
	}

	/* Check memory regions */
	अगर (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		वापस SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		अगर (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			वापस SBI_EINVAL;
		}
	}

	/* Count memory regions */
	गिनती = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	अगर (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		वापस SBI_EINVAL;
	}

	/* Sort the memory regions */
	क्रम (i = 0; i < (गिनती - 1); i++) {
		reg = &dom->regions[i];
		क्रम (j = i + 1; j < गिनती; j++) {
			reg1 = &dom->regions[j];

			अगर (!is_region_before(reg1, reg))
				जारी;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	क्रम (i = 0; i < (गिनती - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		क्रम (j = i + 1; j < गिनती; j++) {
			reg1 = &dom->regions[j];

			अगर (is_region_compatible(reg, reg1)) {
				is_covered = true;
				अवरोध;
			}
		}

		/* find a region is superset of reg, remove reg */
		अगर (is_covered) {
			sbi_memmove(reg, reg + 1, माप(*reg) * (गिनती - i));
			गिनती--;
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
	अगर (dom->अग्र_mode != PRV_S &&
	    dom->अग्र_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->अग्र_mode);
		वापस SBI_EINVAL;
	}

	/* Check next address and next mode */
	अगर (!sbi_domain_check_addr(dom, dom->अग्र_addr, dom->अग्र_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->अग्र_addr);
		वापस SBI_EINVAL;
	}

	वापस 0;
}

bool sbi_domain_check_addr_range(स्थिर काष्ठा sbi_domain *dom,
				 अचिन्हित दीर्घ addr, अचिन्हित दीर्घ आकार,
				 अचिन्हित दीर्घ mode,
				 अचिन्हित दीर्घ access_flags)
{
	अचिन्हित दीर्घ max = addr + आकार;
	स्थिर काष्ठा sbi_domain_memregion *reg, *sreg;

	अगर (!dom)
		वापस false;

	अगर (आकार && max <= addr)
		वापस false;

	जबतक (addr < max) {
		reg = find_region(dom, addr);
		अगर (!reg)
			वापस false;

		अगर (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			वापस false;

		sreg = find_अग्र_subset_region(dom, reg, addr);
		अगर (sreg)
			addr = sreg->base;
		अन्यथा अगर (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		अन्यथा
			अवरोध;
	}

	वापस true;
}

व्योम sbi_domain_dump(स्थिर काष्ठा sbi_domain *dom, स्थिर अक्षर *suffix)
{
	u32 i, j, k;
	अचिन्हित दीर्घ rstart, rend;
	काष्ठा sbi_domain_memregion *reg;

	sbi_printf("Domain%d Name        %s: %s\n",
		   dom->सूचकांक_2, suffix, dom->name);

	sbi_printf("Domain%d Init Order  %s: 0x%x\n",
		   dom->सूचकांक_2, suffix, dom->init_order);

	sbi_printf("Domain%d Boot HART   %s: %d\n",
		   dom->सूचकांक_2, suffix, dom->boot_hartid);

	k = 0;
	sbi_printf("Domain%d HARTs       %s: ", dom->सूचकांक_2, suffix);
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
			   dom->सूचकांक_2, i, suffix, rstart, rend);

		k = 0;

		sbi_printf("M: ");
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->सूचकांक_2, suffix, dom->अग्र_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->सूचकांक_2, suffix, dom->अग्र_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->सूचकांक_2, suffix);
	चयन (dom->अग्र_mode) {
	हाल PRV_M:
		sbi_printf("M-mode\n");
		अवरोध;
	हाल PRV_S:
		sbi_printf("S-mode\n");
		अवरोध;
	हाल PRV_U:
		sbi_printf("U-mode\n");
		अवरोध;
	शेष:
		sbi_printf("Unknown\n");
		अवरोध;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->सूचकांक_2, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->सूचकांक_2, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

व्योम sbi_domain_dump_all(स्थिर अक्षर *suffix)
{
	स्थिर काष्ठा sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

पूर्णांक sbi_domain_register(काष्ठा sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	काष्ठा sbi_domain *tdom;
	पूर्णांक rc;

	/* Sanity checks */
	अगर (!dom || domain_finalized)
		वापस SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		अगर (tdom == dom ||
		    tdom->init_order == dom->init_order)
			वापस SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	अगर (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		वापस rc;
	}

	sbi_list_add_पुच्छ(&dom->गाँठ, &domain_list);

	/* Assign index to domain */
	dom->सूचकांक_2 = domain_गिनती++;

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
		अगर (tdom) {
			अगर (tdom->init_order > dom->init_order)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			अन्यथा
				जारी;
		}

		sbi_update_hartindex_to_domain(i, dom);
		sbi_hartmask_set_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		अगर (sbi_hartindex_to_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->सूचकांक_2, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	अगर (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->गाँठ);
		वापस rc;
	}

	वापस 0;
}

अटल पूर्णांक मूल_add_memregion(स्थिर काष्ठा sbi_domain_memregion *reg)
{
	पूर्णांक rc;
	bool reg_merged;
	काष्ठा sbi_domain_memregion *nreg, *nreg1;
	पूर्णांक मूल_memregs_गिनती = sbi_domain_used_memregions(&मूल);

	/* Sanity checks */
	अगर (!reg || domain_finalized || !मूल.regions ||
	    (मूल_REGION_MAX <= मूल_memregs_गिनती))
		वापस SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&मूल, nreg) {
		अगर (is_region_compatible(reg, nreg))
			वापस 0;
	}

	/* Append the memregion to root memregions */
	nreg = &मूल.regions[मूल_memregs_गिनती];
	sbi_memcpy(nreg, reg, माप(*reg));
	मूल_memregs_गिनती++;
	मूल.regions[मूल_memregs_गिनती].order = 0;

	/* Sort and optimize root regions */
	करो {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&मूल);
		अगर (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   मूल.name, rc);
			वापस rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&मूल, nreg) {
			nreg1 = nreg + 1;
			अगर (!nreg1->order)
				जारी;

			अगर (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				पूर्णांक i1 = nreg1 - मूल.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    माप(*nreg1) * (मूल_memregs_गिनती - i1));
				reg_merged = true;
				मूल_memregs_गिनती--;
			}
		}
	} जबतक (reg_merged);

	वापस 0;
}

पूर्णांक sbi_domain_मूल_add_memrange(अचिन्हित दीर्घ addr, अचिन्हित दीर्घ आकार,
			   अचिन्हित दीर्घ align, अचिन्हित दीर्घ region_flags)
{
	पूर्णांक rc;
	अचिन्हित दीर्घ pos, end, rsize;
	काष्ठा sbi_domain_memregion reg;

	pos = addr;
	end = addr + आकार;
	जबतक (pos < end) {
		rsize = pos & (align - 1);
		अगर (rsize)
			rsize = 1UL << sbi_ffs(pos);
		अन्यथा
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = मूल_add_memregion(&reg);
		अगर (rc)
			वापस rc;
		pos += rsize;
	}

	वापस 0;
}

पूर्णांक sbi_domain_startup(काष्ठा sbi_scratch *scratch, u32 cold_hartid)
{
	पूर्णांक rc;
	u32 dhart;
	काष्ठा sbi_domain *dom;

	/* Sanity checks */
	अगर (!domain_finalized)
		वापस SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		अगर (!sbi_hartindex_valid(dhart))
			जारी;

		/* Ignore if boot HART not possible for this domain */
		अगर (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			जारी;

		/* Ignore if boot HART assigned different domain */
		अगर (sbi_hartindex_to_domain(dhart) != dom)
			जारी;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		अगर (!rc)
			जारी;

		/* Startup boot HART of domain */
		अगर (dom->boot_hartid == cold_hartid) {
			scratch->अग्र_addr = dom->अग्र_addr;
			scratch->अग्र_mode = dom->अग्र_mode;
			scratch->अग्र_arg1 = dom->अग्र_arg1;
		} अन्यथा {
			rc = sbi_hsm_hart_start(scratch, शून्य,
						dom->boot_hartid,
						dom->अग्र_addr,
						dom->अग्र_mode,
						dom->अग्र_arg1);
			अगर (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				वापस rc;
			}
		}
	}

	वापस 0;
}

पूर्णांक sbi_domain_finalize(काष्ठा sbi_scratch *scratch)
{
	पूर्णांक rc;
	स्थिर काष्ठा sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	अगर (domain_finalized)
		वापस SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	अगर (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		वापस rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	वापस 0;
}

पूर्णांक sbi_domain_init(काष्ठा sbi_scratch *scratch, u32 cold_hartid)
{
	पूर्णांक rc;
	काष्ठा sbi_hartmask *मूल_hmask;
	काष्ठा sbi_domain_memregion *मूल_memregs;
	पूर्णांक मूल_memregs_गिनती = 0;
	स्थिर काष्ठा sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_शीर्ष(&domain_list);

	अगर (!fw_single_region) {
		अगर (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			वापस SBI_EINVAL;
		}
		अगर ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			वापस SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(व्योम *);
	अगर (!domain_hart_ptr_offset)
		वापस SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	अगर (rc)
		जाओ fail_free_domain_hart_ptr_offset;

	मूल_memregs = sbi_calloc(माप(*मूल_memregs), मूल_REGION_MAX + 1);
	अगर (!मूल_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		जाओ fail_deinit_context;
	}
	मूल.regions = मूल_memregs;

	मूल_hmask = sbi_zalloc(माप(*मूल_hmask));
	अगर (!मूल_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		जाओ fail_free_मूल_memregs;
	}
	मूल.possible_harts = मूल_hmask;

	/* Root domain firmware memory region */
	अगर (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_आकार,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &मूल_memregs[मूल_memregs_गिनती++]);
	} अन्यथा {
		sbi_domain_memregion_init(scratch->fw_start,
					  scratch->fw_rw_offset,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &मूल_memregs[मूल_memregs_गिनती++]);

		sbi_domain_memregion_init((scratch->fw_start +
					   scratch->fw_rw_offset),
					  (scratch->fw_आकार -
					   scratch->fw_rw_offset),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &मूल_memregs[मूल_memregs_गिनती++]);
	}

	मूल.fw_region_inited = true;

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
				  &मूल_memregs[मूल_memregs_गिनती++]);

	/* Root domain memory region end */
	मूल_memregs[मूल_memregs_गिनती].order = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	मूल.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	मूल.अग्र_arg1 = scratch->अग्र_arg1;
	मूल.अग्र_addr = scratch->अग्र_addr;
	मूल.अग्र_mode = scratch->अग्र_mode;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_set_hartindex(i, मूल_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_register(&मूल);
	अगर (rc)
		जाओ fail_free_मूल_hmask;

	वापस 0;

fail_free_मूल_hmask:
	sbi_free(मूल_hmask);
fail_free_मूल_memregs:
	sbi_free(मूल_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	वापस rc;
}
