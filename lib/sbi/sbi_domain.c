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

SBI_LIST_शीर्ष(domain_list);

स्थिर u32 domain_गणनम् = 0;
स्थिर bool domain_finalized = false;

#परिभाषय मूलम्_REGION_MAX	32

संरचना sbi_domain मूलम् = {
	.name = "root",
	.init_क्रमः = -1U,
	.possible_harts = रिक्त,
	.regions = रिक्त,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

स्थिर अचिह्नित दीर्घ domain_hart_ptr_विस्थापनम्;

संरचना sbi_domain *sbi_hartindex_गन्तव्यम्_domain(u32 hartindex)
{
	संरचना sbi_scratch *scratch;

	scratch = sbi_hartindex_गन्तव्यम्_scratch(hartindex);
	यदि (!scratch || !domain_hart_ptr_विस्थापनम्)
		प्रत्यागम रिक्त;

	प्रत्यागम sbi_scratch_read_type(scratch, शून्य *, domain_hart_ptr_विस्थापनम्);
}

शून्य sbi_update_hartindex_गन्तव्यम्_domain(u32 hartindex, संरचना sbi_domain *dom)
{
	संरचना sbi_scratch *scratch;

	scratch = sbi_hartindex_गन्तव्यम्_scratch(hartindex);
	यदि (!scratch)
		प्रत्यागम;

	sbi_scratch_write_type(scratch, शून्य *, domain_hart_ptr_विस्थापनम्, dom);
}

bool sbi_domain_is_assigned_hart(ध्रुव संरचना sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_तालकम्)
{
	bool ret;
	संरचना sbi_domain *tdom = (संरचना sbi_domain *)dom;

	यदि (!dom)
		प्रत्यागम false;

	spin_तालकम्(&tdom->assigned_harts_तालकम्);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_तालकम्);

	प्रत्यागम ret;
}

पूर्णाङ्क sbi_domain_get_assigned_hartmask(ध्रुव संरचना sbi_domain *dom,
				     संरचना sbi_hartmask *आवरणम्)
	MUST_NOT_HOLD(&dom->assigned_harts_तालकम्)
{
	ulong ret = 0;
	संरचना sbi_domain *tdom = (संरचना sbi_domain *)dom;

	यदि (!dom) {
		sbi_hartmask_clear_all(आवरणम्);
		प्रत्यागम 0;
	}

	spin_तालकम्(&tdom->assigned_harts_तालकम्);
	sbi_hartmask_copy(आवरणम्, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_तालकम्);

	प्रत्यागम ret;
}

शून्य sbi_domain_memregion_init(अचिह्नित दीर्घ addr,
				अचिह्नित दीर्घ आकार,
				अचिह्नित दीर्घ flags,
				संरचना sbi_domain_memregion *reg)
{
	अचिह्नित दीर्घ आधारः_2 = 0, क्रमः;

	प्रति (क्रमः = log2roundup(आकार) ; क्रमः <= __riscv_xlen; क्रमः++) {
		यदि (क्रमः < __riscv_xlen) {
			आधारः_2 = addr & ~((1UL << क्रमः) - 1UL);
			यदि ((आधारः_2 <= addr) &&
			    (addr < (आधारः_2 + (1UL << क्रमः))) &&
			    (आधारः_2 <= (addr + आकार - 1UL)) &&
			    ((addr + आकार - 1UL) < (आधारः_2 + (1UL << क्रमः))))
				भङ्ग;
		} अन्यथा {
			आधारः_2 = 0;
			भङ्ग;
		}

	}

	यदि (reg) {
		reg->आधारः_2 = आधारः_2;
		reg->क्रमः = क्रमः;
		reg->flags = flags;
	}
}

अचिह्नित पूर्णाङ्क sbi_domain_get_oldpmp_flags(संरचना sbi_domain_memregion *reg)
{

	अचिह्नित पूर्णाङ्क pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	यदि (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	प्रत्यागम pmp_flags;
}

अचिह्नित पूर्णाङ्क sbi_domain_get_smepmp_flags(संरचना sbi_domain_memregion *reg)
{
	अचिह्नित पूर्णाङ्क pmp_flags = 0;
	अचिह्नित दीर्घ rstart, rend;

	यदि ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_आवरणम्) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		प्रत्यागम 0;
	} अन्यथा यदि (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		यदि (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		अन्यथा यदि (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		अन्यथा यदि (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		अन्यथा यदि (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		अन्यथा यदि (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} अन्यथा यदि (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		यदि ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_आवरणम्)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			प्रत्यागम 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		यदि (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} अन्यथा यदि (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} अन्यथा {
		rstart = reg->आधारः_2;
		rend = (reg->क्रमः < __riscv_xlen) ? rstart + ((1UL << reg->क्रमः) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	प्रत्यागम pmp_flags;
}

bool sbi_domain_check_addr(ध्रुव संरचना sbi_domain *dom,
			   अचिह्नित दीर्घ addr, अचिह्नित दीर्घ बहुलकम्,
			   अचिह्नित दीर्घ access_flags)
{
	bool rmmio, mmio = false;
	संरचना sbi_domain_memregion *reg;
	अचिह्नित दीर्घ rstart, rend, rflags, rwx = 0, rrwx = 0;

	यदि (!dom)
		प्रत्यागम false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	यदि (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	यदि (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	यदि (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	यदि (access_flags & SBI_DOMAIN_MMIO)
		mmio = true;

	sbi_domain_for_each_memregion(dom, reg) {
		rflags = reg->flags;
		rrwx = (बहुलकम् == PRV_M ?
			(rflags & SBI_DOMAIN_MEMREGION_M_ACCESS_आवरणम्) :
			(rflags & SBI_DOMAIN_MEMREGION_SU_ACCESS_आवरणम्)
			>> SBI_DOMAIN_MEMREGION_SU_ACCESS_सरणम्);

		rstart = reg->आधारः_2;
		rend = (reg->क्रमः < __riscv_xlen) ?
			rstart + ((1UL << reg->क्रमः) - 1) : -1UL;
		यदि (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			यदि (!mmio && rmmio)
				प्रत्यागम false;
			प्रत्यागम ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	प्रत्यागम (बहुलकम् == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
स्थिर bool is_region_valid(ध्रुव संरचना sbi_domain_memregion *reg)
{
	यदि (reg->क्रमः < 3 || __riscv_xlen < reg->क्रमः)
		प्रत्यागम false;

	यदि (reg->क्रमः == __riscv_xlen && reg->आधारः_2 != 0)
		प्रत्यागम false;

	यदि (reg->क्रमः < __riscv_xlen && (reg->आधारः_2 & (BIT(reg->क्रमः) - 1)))
		प्रत्यागम false;

	प्रत्यागम true;
}

/** Check if regionA can be replaced by regionB */
स्थिर bool is_region_compatible(ध्रुव संरचना sbi_domain_memregion *regA,
				 ध्रुव संरचना sbi_domain_memregion *regB)
{
	यदि (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		प्रत्यागम true;

	प्रत्यागम false;
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
	यदि (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		प्रत्यागम true;
	यदि (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		प्रत्यागम false;

	यदि (regA->क्रमः < regB->क्रमः)
		प्रत्यागम true;

	यदि ((regA->क्रमः == regB->क्रमः) &&
	    (regA->आधारः_2 < regB->आधारः_2))
		प्रत्यागम true;

	प्रत्यागम false;
}

स्थिर ध्रुव संरचना sbi_domain_memregion *find_region(
						ध्रुव संरचना sbi_domain *dom,
						अचिह्नित दीर्घ addr)
{
	अचिह्नित दीर्घ rstart, rend;
	संरचना sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->आधारः_2;
		rend = (reg->क्रमः < __riscv_xlen) ?
			rstart + ((1UL << reg->क्रमः) - 1) : -1UL;
		यदि (rstart <= addr && addr <= rend)
			प्रत्यागम reg;
	}

	प्रत्यागम रिक्त;
}

स्थिर ध्रुव संरचना sbi_domain_memregion *find_अग्र_subset_region(
				ध्रुव संरचना sbi_domain *dom,
				ध्रुव संरचना sbi_domain_memregion *reg,
				अचिह्नित दीर्घ addr)
{
	संरचना sbi_domain_memregion *sreg, *ret = रिक्त;

	sbi_domain_for_each_memregion(dom, sreg) {
		यदि (sreg == reg || (sreg->आधारः_2 <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			अनुवर्त;

		यदि (!ret || (sreg->आधारः_2 < ret->आधारः_2) ||
		    ((sreg->आधारः_2 == ret->आधारः_2) && (sreg->क्रमः < ret->क्रमः)))
			ret = sreg;
	}

	प्रत्यागम ret;
}

स्थिर शून्य swap_region(संरचना sbi_domain_memregion* reg1,
			संरचना sbi_domain_memregion* reg2)
{
	संरचना sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, परिमाण(treg));
	sbi_memcpy(reg1, reg2, परिमाण(treg));
	sbi_memcpy(reg2, &treg, परिमाण(treg));
}

स्थिर पूर्णाङ्क sbi_domain_प्रयुक्तम्_memregions(ध्रुव संरचना sbi_domain *dom)
{
	पूर्णाङ्क गणनम् = 0;
	संरचना sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		गणनम्++;

	प्रत्यागम गणनम्;
}

स्थिर पूर्णाङ्क sanitize_domain(संरचना sbi_domain *dom)
{
	u32 i, j, गणनम्;
	bool is_covered;
	संरचना sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	यदि (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		प्रत्यागम SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		यदि (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_गन्तव्यम्_hartid(i));
			प्रत्यागम SBI_EINVAL;
		}
	}

	/* Check memory regions */
	यदि (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		प्रत्यागम SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		यदि (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->आधारः_2, reg->क्रमः,
				   reg->flags);
			प्रत्यागम SBI_EINVAL;
		}
	}

	/* Count memory regions */
	गणनम् = sbi_domain_प्रयुक्तम्_memregions(dom);

	/* Check presence of firmware regions */
	यदि (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		प्रत्यागम SBI_EINVAL;
	}

	/* Sort the memory regions */
	प्रति (i = 0; i < (गणनम् - 1); i++) {
		reg = &dom->regions[i];
		प्रति (j = i + 1; j < गणनम्; j++) {
			reg1 = &dom->regions[j];

			यदि (!is_region_before(reg1, reg))
				अनुवर्त;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	प्रति (i = 0; i < (गणनम् - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		प्रति (j = i + 1; j < गणनम्; j++) {
			reg1 = &dom->regions[j];

			यदि (is_region_compatible(reg, reg1)) {
				is_covered = true;
				भङ्ग;
			}
		}

		/* find a region is superset of reg, remove reg */
		यदि (is_covered) {
			sbi_memmove(reg, reg + 1, परिमाण(*reg) * (गणनम् - i));
			गणनम्--;
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
	यदि (dom->अग्र_बहुलकम् != PRV_S &&
	    dom->अग्र_बहुलकम् != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->अग्र_बहुलकम्);
		प्रत्यागम SBI_EINVAL;
	}

	/* Check next address and next mode */
	यदि (!sbi_domain_check_addr(dom, dom->अग्र_addr, dom->अग्र_बहुलकम्,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->अग्र_addr);
		प्रत्यागम SBI_EINVAL;
	}

	प्रत्यागम 0;
}

bool sbi_domain_check_addr_range(ध्रुव संरचना sbi_domain *dom,
				 अचिह्नित दीर्घ addr, अचिह्नित दीर्घ आकार,
				 अचिह्नित दीर्घ बहुलकम्,
				 अचिह्नित दीर्घ access_flags)
{
	अचिह्नित दीर्घ max = addr + आकार;
	ध्रुव संरचना sbi_domain_memregion *reg, *sreg;

	यदि (!dom)
		प्रत्यागम false;

	यदि (आकार && max <= addr)
		प्रत्यागम false;

	यावत् (addr < max) {
		reg = find_region(dom, addr);
		यदि (!reg)
			प्रत्यागम false;

		यदि (!sbi_domain_check_addr(dom, addr, बहुलकम्, access_flags))
			प्रत्यागम false;

		sreg = find_अग्र_subset_region(dom, reg, addr);
		यदि (sreg)
			addr = sreg->आधारः_2;
		अन्यथा यदि (reg->क्रमः < __riscv_xlen)
			addr = reg->आधारः_2 + (1UL << reg->क्रमः);
		अन्यथा
			भङ्ग;
	}

	प्रत्यागम true;
}

शून्य sbi_domain_dump(ध्रुव संरचना sbi_domain *dom, ध्रुव वर्ण *suffix)
{
	u32 i, j, k;
	अचिह्नित दीर्घ rstart, rend;
	संरचना sbi_domain_memregion *reg;

	sbi_printf("Domain%d Name        %s: %s\n",
		   dom->सूचकांक_2, suffix, dom->name);

	sbi_printf("Domain%d Init Order  %s: 0x%x\n",
		   dom->सूचकांक_2, suffix, dom->init_क्रमः);

	sbi_printf("Domain%d Boot HART   %s: %d\n",
		   dom->सूचकांक_2, suffix, dom->boot_hartid);

	k = 0;
	sbi_printf("Domain%d HARTs       %s: ", dom->सूचकांक_2, suffix);
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		j = sbi_hartindex_गन्तव्यम्_hartid(i);
		sbi_printf("%s0x%x%s", (k++) ? "," : "",
			   j, sbi_domain_is_assigned_hart(dom, i) ? "*" : "");
	}
	sbi_printf("\n");

	i = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->आधारः_2;
		rend = (reg->क्रमः < __riscv_xlen) ?
			rstart + ((1UL << reg->क्रमः) - 1) : -1UL;

		sbi_printf("Domain%d Region%02d    %s: 0x%" PRILX "-0x%" PRILX " ",
			   dom->सूचकांक_2, i, suffix, rstart, rend);

		k = 0;

		sbi_printf("M: ");
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->सूचकांक_2, suffix, dom->अग्र_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->सूचकांक_2, suffix, dom->अग्र_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->सूचकांक_2, suffix);
	विकल्प (dom->अग्र_बहुलकम्) {
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
		   dom->सूचकांक_2, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->सूचकांक_2, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

शून्य sbi_domain_dump_all(ध्रुव वर्ण *suffix)
{
	ध्रुव संरचना sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

पूर्णाङ्क sbi_domain_पञ्जिकाकोशः(संरचना sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	संरचना sbi_domain *tdom;
	पूर्णाङ्क rc;

	/* Sanity checks */
	यदि (!dom || domain_finalized)
		प्रत्यागम SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		यदि (tdom == dom ||
		    tdom->init_क्रमः == dom->init_क्रमः)
			प्रत्यागम SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	यदि (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		प्रत्यागम rc;
	}

	sbi_list_सङ्कलय_पुच्छ(&dom->ग्रन्थिः, &domain_list);

	/* Assign index to domain */
	dom->सूचकांक_2 = domain_गणनम्++;

	/* Initialize spinlock for dom->assigned_harts */
	SPIN_तालकम्_INIT(dom->assigned_harts_तालकम्);

	/* Clear assigned HARTs of domain */
	sbi_hartmask_clear_all(&dom->assigned_harts);

	/*
	 * Assign HART to a domain with the least initialization order
	 * where the HART is listed as a possible HART of the domain.
	 */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		tdom = sbi_hartindex_गन्तव्यम्_domain(i);
		यदि (tdom) {
			यदि (tdom->init_क्रमः > dom->init_क्रमः)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			अन्यथा
				अनुवर्त;
		}

		sbi_update_hartindex_गन्तव्यम्_domain(i, dom);
		sbi_hartmask_समुच्चयः_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		यदि (sbi_hartindex_गन्तव्यम्_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->सूचकांक_2, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_सज्जता_state(dom);
	यदि (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->ग्रन्थिः);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क मूलम्_सङ्कलय_memregion(ध्रुव संरचना sbi_domain_memregion *reg)
{
	पूर्णाङ्क rc;
	bool reg_merged;
	संरचना sbi_domain_memregion *nreg, *nreg1;
	पूर्णाङ्क मूलम्_memregs_गणनम् = sbi_domain_प्रयुक्तम्_memregions(&मूलम्);

	/* Sanity checks */
	यदि (!reg || domain_finalized || !मूलम्.regions ||
	    (मूलम्_REGION_MAX <= मूलम्_memregs_गणनम्))
		प्रत्यागम SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&मूलम्, nreg) {
		यदि (is_region_compatible(reg, nreg))
			प्रत्यागम 0;
	}

	/* Append the memregion to root memregions */
	nreg = &मूलम्.regions[मूलम्_memregs_गणनम्];
	sbi_memcpy(nreg, reg, परिमाण(*reg));
	मूलम्_memregs_गणनम्++;
	मूलम्.regions[मूलम्_memregs_गणनम्].क्रमः = 0;

	/* Sort and optimize root regions */
	कुरु {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&मूलम्);
		यदि (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   मूलम्.name, rc);
			प्रत्यागम rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&मूलम्, nreg) {
			nreg1 = nreg + 1;
			यदि (!nreg1->क्रमः)
				अनुवर्त;

			यदि (!(nreg->आधारः_2 & (BIT(nreg->क्रमः + 1) - 1)) &&
			    (nreg->आधारः_2 + BIT(nreg->क्रमः)) == nreg1->आधारः_2 &&
			    nreg->क्रमः == nreg1->क्रमः &&
			    nreg->flags == nreg1->flags) {
				पूर्णाङ्क i1 = nreg1 - मूलम्.regions;
				nreg->क्रमः++;
				sbi_memmove(nreg1, nreg1 + 1,
					    परिमाण(*nreg1) * (मूलम्_memregs_गणनम् - i1));
				reg_merged = true;
				मूलम्_memregs_गणनम्--;
			}
		}
	} यावत् (reg_merged);

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_domain_मूलम्_सङ्कलय_memrange(अचिह्नित दीर्घ addr, अचिह्नित दीर्घ आकार,
			   अचिह्नित दीर्घ align, अचिह्नित दीर्घ region_flags)
{
	पूर्णाङ्क rc;
	अचिह्नित दीर्घ pos, अन्तः_2, rsize;
	संरचना sbi_domain_memregion reg;

	pos = addr;
	अन्तः_2 = addr + आकार;
	यावत् (pos < अन्तः_2) {
		rsize = pos & (align - 1);
		यदि (rsize)
			rsize = 1UL << sbi_ffs(pos);
		अन्यथा
			rsize = ((अन्तः_2 - pos) < align) ?
				(अन्तः_2 - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = मूलम्_सङ्कलय_memregion(&reg);
		यदि (rc)
			प्रत्यागम rc;
		pos += rsize;
	}

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_domain_startup(संरचना sbi_scratch *scratch, u32 cold_hartid)
{
	पूर्णाङ्क rc;
	u32 dhart;
	संरचना sbi_domain *dom;

	/* Sanity checks */
	यदि (!domain_finalized)
		प्रत्यागम SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_गन्तव्यम्_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		यदि (!sbi_hartindex_valid(dhart))
			अनुवर्त;

		/* Ignore if boot HART not possible for this domain */
		यदि (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			अनुवर्त;

		/* Ignore if boot HART assigned different domain */
		यदि (sbi_hartindex_गन्तव्यम्_domain(dhart) != dom)
			अनुवर्त;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_तालकम्(&dom->assigned_harts_तालकम्);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_तालकम्);
		यदि (!rc)
			अनुवर्त;

		/* Startup boot HART of domain */
		यदि (dom->boot_hartid == cold_hartid) {
			scratch->अग्र_addr = dom->अग्र_addr;
			scratch->अग्र_बहुलकम् = dom->अग्र_बहुलकम्;
			scratch->अग्र_arg1 = dom->अग्र_arg1;
		} अन्यथा {
			rc = sbi_hsm_hart_start(scratch, रिक्त,
						dom->boot_hartid,
						dom->अग्र_addr,
						dom->अग्र_बहुलकम्,
						dom->अग्र_arg1);
			यदि (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				प्रत्यागम rc;
			}
		}
	}

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_domain_finalize(संरचना sbi_scratch *scratch)
{
	पूर्णाङ्क rc;
	ध्रुव संरचना sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	यदि (domain_finalized)
		प्रत्यागम SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	यदि (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		प्रत्यागम rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_domain_init(संरचना sbi_scratch *scratch, u32 cold_hartid)
{
	पूर्णाङ्क rc;
	संरचना sbi_hartmask *मूलम्_hmask;
	संरचना sbi_domain_memregion *मूलम्_memregs;
	पूर्णाङ्क मूलम्_memregs_गणनम् = 0;
	ध्रुव संरचना sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_शीर्ष(&domain_list);

	यदि (!fw_single_region) {
		यदि (scratch->fw_rw_विस्थापनम् == 0 ||
		    (scratch->fw_rw_विस्थापनम् & (scratch->fw_rw_विस्थापनम् - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_विस्थापनम्);
			प्रत्यागम SBI_EINVAL;
		}
		यदि ((scratch->fw_start & (scratch->fw_rw_विस्थापनम् - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			प्रत्यागम SBI_EINVAL;
		}
	}

	domain_hart_ptr_विस्थापनम् = sbi_scratch_alloc_type_विस्थापनम्(शून्य *);
	यदि (!domain_hart_ptr_विस्थापनम्)
		प्रत्यागम SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	यदि (rc)
		गच्छ fail_free_domain_hart_ptr_विस्थापनम्;

	मूलम्_memregs = sbi_calloc(परिमाण(*मूलम्_memregs), मूलम्_REGION_MAX + 1);
	यदि (!मूलम्_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		गच्छ fail_deinit_context;
	}
	मूलम्.regions = मूलम्_memregs;

	मूलम्_hmask = sbi_zalloc(परिमाण(*मूलम्_hmask));
	यदि (!मूलम्_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		गच्छ fail_free_मूलम्_memregs;
	}
	मूलम्.possible_harts = मूलम्_hmask;

	/* Root domain firmware memory region */
	यदि (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_आकार,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &मूलम्_memregs[मूलम्_memregs_गणनम्++]);
	} अन्यथा {
		sbi_domain_memregion_init(scratch->fw_start,
					  scratch->fw_rw_विस्थापनम्,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &मूलम्_memregs[मूलम्_memregs_गणनम्++]);

		sbi_domain_memregion_init((scratch->fw_start +
					   scratch->fw_rw_विस्थापनम्),
					  (scratch->fw_आकार -
					   scratch->fw_rw_विस्थापनम्),
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
	मूलम्_memregs[मूलम्_memregs_गणनम्].क्रमः = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	मूलम्.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	मूलम्.अग्र_arg1 = scratch->अग्र_arg1;
	मूलम्.अग्र_addr = scratch->अग्र_addr;
	मूलम्.अग्र_बहुलकम् = scratch->अग्र_बहुलकम्;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_समुच्चयः_hartindex(i, मूलम्_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_पञ्जिकाकोशः(&मूलम्);
	यदि (rc)
		गच्छ fail_free_मूलम्_hmask;

	प्रत्यागम 0;

fail_free_मूलम्_hmask:
	sbi_free(मूलम्_hmask);
fail_free_मूलम्_memregs:
	sbi_free(मूलम्_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_विस्थापनम्:
	sbi_scratch_free_विस्थापनम्(domain_hart_ptr_विस्थापनम्);
	प्रत्यागम rc;
}
