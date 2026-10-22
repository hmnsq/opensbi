/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_hartmask.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <sbi/sbi_list.h>
#అన్తర్భావ <sbi/sbi_math.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_string.h>

SBI_LIST_తల(domain_list);

స్థిర u32 domain_లెక్క = 0;
స్థిర bool domain_finalized = false;

#నిర్వచించు మూలం_REGION_MAX	32

నిర్మాణం sbi_domain మూలం = {
	.name = "root",
	.init_క్రమం = -1U,
	.possible_harts = ఖాళీ,
	.regions = ఖాళీ,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

స్థిర గుర్తులేని దీర్ఘ domain_hart_ptr_స్థానభ్రంశం;

నిర్మాణం sbi_domain *sbi_hartindex_గమ్యం_domain(u32 hartindex)
{
	నిర్మాణం sbi_scratch *scratch;

	scratch = sbi_hartindex_గమ్యం_scratch(hartindex);
	ఒకవేళ (!scratch || !domain_hart_ptr_స్థానభ్రంశం)
		తిరిగి ఖాళీ;

	తిరిగి sbi_scratch_read_type(scratch, శూన్య *, domain_hart_ptr_స్థానభ్రంశం);
}

శూన్య sbi_update_hartindex_గమ్యం_domain(u32 hartindex, నిర్మాణం sbi_domain *dom)
{
	నిర్మాణం sbi_scratch *scratch;

	scratch = sbi_hartindex_గమ్యం_scratch(hartindex);
	ఒకవేళ (!scratch)
		తిరిగి;

	sbi_scratch_write_type(scratch, శూన్య *, domain_hart_ptr_స్థానభ్రంశం, dom);
}

bool sbi_domain_is_assigned_hart(స్థిరరాశి నిర్మాణం sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_తాళం)
{
	bool ret;
	నిర్మాణం sbi_domain *tdom = (నిర్మాణం sbi_domain *)dom;

	ఒకవేళ (!dom)
		తిరిగి false;

	spin_తాళం(&tdom->assigned_harts_తాళం);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_తాళం);

	తిరిగి ret;
}

పూర్ణాంకం sbi_domain_get_assigned_hartmask(స్థిరరాశి నిర్మాణం sbi_domain *dom,
				     నిర్మాణం sbi_hartmask *ముసుగు)
	MUST_NOT_HOLD(&dom->assigned_harts_తాళం)
{
	ulong ret = 0;
	నిర్మాణం sbi_domain *tdom = (నిర్మాణం sbi_domain *)dom;

	ఒకవేళ (!dom) {
		sbi_hartmask_clear_all(ముసుగు);
		తిరిగి 0;
	}

	spin_తాళం(&tdom->assigned_harts_తాళం);
	sbi_hartmask_copy(ముసుగు, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_తాళం);

	తిరిగి ret;
}

శూన్య sbi_domain_memregion_init(గుర్తులేని దీర్ఘ addr,
				గుర్తులేని దీర్ఘ కొలత,
				గుర్తులేని దీర్ఘ flags,
				నిర్మాణం sbi_domain_memregion *reg)
{
	గుర్తులేని దీర్ఘ పునాది_2 = 0, క్రమం;

	కొరకు (క్రమం = log2roundup(కొలత) ; క్రమం <= __riscv_xlen; క్రమం++) {
		ఒకవేళ (క్రమం < __riscv_xlen) {
			పునాది_2 = addr & ~((1UL << క్రమం) - 1UL);
			ఒకవేళ ((పునాది_2 <= addr) &&
			    (addr < (పునాది_2 + (1UL << క్రమం))) &&
			    (పునాది_2 <= (addr + కొలత - 1UL)) &&
			    ((addr + కొలత - 1UL) < (పునాది_2 + (1UL << క్రమం))))
				ఆపు;
		} లేకపోతే {
			పునాది_2 = 0;
			ఆపు;
		}

	}

	ఒకవేళ (reg) {
		reg->పునాది_2 = పునాది_2;
		reg->క్రమం = క్రమం;
		reg->flags = flags;
	}
}

గుర్తులేని పూర్ణాంకం sbi_domain_get_oldpmp_flags(నిర్మాణం sbi_domain_memregion *reg)
{

	గుర్తులేని పూర్ణాంకం pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	తిరిగి pmp_flags;
}

గుర్తులేని పూర్ణాంకం sbi_domain_get_smepmp_flags(నిర్మాణం sbi_domain_memregion *reg)
{
	గుర్తులేని పూర్ణాంకం pmp_flags = 0;
	గుర్తులేని దీర్ఘ rstart, rend;

	ఒకవేళ ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_ముసుగు) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		తిరిగి 0;
	} లేకపోతే ఒకవేళ (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		ఒకవేళ (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		లేకపోతే ఒకవేళ (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		లేకపోతే ఒకవేళ (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		లేకపోతే ఒకవేళ (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		లేకపోతే ఒకవేళ (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} లేకపోతే ఒకవేళ (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		ఒకవేళ ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_ముసుగు)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			తిరిగి 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} లేకపోతే ఒకవేళ (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} లేకపోతే {
		rstart = reg->పునాది_2;
		rend = (reg->క్రమం < __riscv_xlen) ? rstart + ((1UL << reg->క్రమం) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	తిరిగి pmp_flags;
}

bool sbi_domain_check_addr(స్థిరరాశి నిర్మాణం sbi_domain *dom,
			   గుర్తులేని దీర్ఘ addr, గుర్తులేని దీర్ఘ బహుళకం,
			   గుర్తులేని దీర్ఘ access_flags)
{
	bool rmmio, mmio = false;
	నిర్మాణం sbi_domain_memregion *reg;
	గుర్తులేని దీర్ఘ rstart, rend, rflags, rwx = 0, rrwx = 0;

	ఒకవేళ (!dom)
		తిరిగి false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	ఒకవేళ (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	ఒకవేళ (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	ఒకవేళ (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	ఒకవేళ (access_flags & SBI_DOMAIN_MMIO)
		mmio = true;

	sbi_domain_for_each_memregion(dom, reg) {
		rflags = reg->flags;
		rrwx = (బహుళకం == PRV_M ?
			(rflags & SBI_DOMAIN_MEMREGION_M_ACCESS_ముసుగు) :
			(rflags & SBI_DOMAIN_MEMREGION_SU_ACCESS_ముసుగు)
			>> SBI_DOMAIN_MEMREGION_SU_ACCESS_జరుపు);

		rstart = reg->పునాది_2;
		rend = (reg->క్రమం < __riscv_xlen) ?
			rstart + ((1UL << reg->క్రమం) - 1) : -1UL;
		ఒకవేళ (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			ఒకవేళ (!mmio && rmmio)
				తిరిగి false;
			తిరిగి ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	తిరిగి (బహుళకం == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
స్థిర bool is_region_valid(స్థిరరాశి నిర్మాణం sbi_domain_memregion *reg)
{
	ఒకవేళ (reg->క్రమం < 3 || __riscv_xlen < reg->క్రమం)
		తిరిగి false;

	ఒకవేళ (reg->క్రమం == __riscv_xlen && reg->పునాది_2 != 0)
		తిరిగి false;

	ఒకవేళ (reg->క్రమం < __riscv_xlen && (reg->పునాది_2 & (బిట్టు(reg->క్రమం) - 1)))
		తిరిగి false;

	తిరిగి true;
}

/** Check if regionA can be replaced by regionB */
స్థిర bool is_region_compatible(స్థిరరాశి నిర్మాణం sbi_domain_memregion *regA,
				 స్థిరరాశి నిర్మాణం sbi_domain_memregion *regB)
{
	ఒకవేళ (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		తిరిగి true;

	తిరిగి false;
}

/** Check if regionA should be placed before regionB */
స్థిర bool is_region_before(స్థిరరాశి నిర్మాణం sbi_domain_memregion *regA,
			     స్థిరరాశి నిర్మాణం sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	ఒకవేళ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		తిరిగి true;
	ఒకవేళ (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		తిరిగి false;

	ఒకవేళ (regA->క్రమం < regB->క్రమం)
		తిరిగి true;

	ఒకవేళ ((regA->క్రమం == regB->క్రమం) &&
	    (regA->పునాది_2 < regB->పునాది_2))
		తిరిగి true;

	తిరిగి false;
}

స్థిర స్థిరరాశి నిర్మాణం sbi_domain_memregion *కనుగొను_region(
						స్థిరరాశి నిర్మాణం sbi_domain *dom,
						గుర్తులేని దీర్ఘ addr)
{
	గుర్తులేని దీర్ఘ rstart, rend;
	నిర్మాణం sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->పునాది_2;
		rend = (reg->క్రమం < __riscv_xlen) ?
			rstart + ((1UL << reg->క్రమం) - 1) : -1UL;
		ఒకవేళ (rstart <= addr && addr <= rend)
			తిరిగి reg;
	}

	తిరిగి ఖాళీ;
}

స్థిర స్థిరరాశి నిర్మాణం sbi_domain_memregion *కనుగొను_తదుపరి_subset_region(
				స్థిరరాశి నిర్మాణం sbi_domain *dom,
				స్థిరరాశి నిర్మాణం sbi_domain_memregion *reg,
				గుర్తులేని దీర్ఘ addr)
{
	నిర్మాణం sbi_domain_memregion *sreg, *ret = ఖాళీ;

	sbi_domain_for_each_memregion(dom, sreg) {
		ఒకవేళ (sreg == reg || (sreg->పునాది_2 <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			కొనసాగు;

		ఒకవేళ (!ret || (sreg->పునాది_2 < ret->పునాది_2) ||
		    ((sreg->పునాది_2 == ret->పునాది_2) && (sreg->క్రమం < ret->క్రమం)))
			ret = sreg;
	}

	తిరిగి ret;
}

స్థిర శూన్య మార్పిడి_region(నిర్మాణం sbi_domain_memregion* reg1,
			నిర్మాణం sbi_domain_memregion* reg2)
{
	నిర్మాణం sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, పరిమాణం(treg));
	sbi_memcpy(reg1, reg2, పరిమాణం(treg));
	sbi_memcpy(reg2, &treg, పరిమాణం(treg));
}

స్థిర పూర్ణాంకం sbi_domain_వాడిన_memregions(స్థిరరాశి నిర్మాణం sbi_domain *dom)
{
	పూర్ణాంకం లెక్క = 0;
	నిర్మాణం sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		లెక్క++;

	తిరిగి లెక్క;
}

స్థిర పూర్ణాంకం sanitize_domain(నిర్మాణం sbi_domain *dom)
{
	u32 i, j, లెక్క;
	bool is_covered;
	నిర్మాణం sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	ఒకవేళ (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		తిరిగి SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ఒకవేళ (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_గమ్యం_hartid(i));
			తిరిగి SBI_EINVAL;
		}
	}

	/* Check memory regions */
	ఒకవేళ (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		తిరిగి SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		ఒకవేళ (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->పునాది_2, reg->క్రమం,
				   reg->flags);
			తిరిగి SBI_EINVAL;
		}
	}

	/* Count memory regions */
	లెక్క = sbi_domain_వాడిన_memregions(dom);

	/* Check presence of firmware regions */
	ఒకవేళ (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		తిరిగి SBI_EINVAL;
	}

	/* Sort the memory regions */
	కొరకు (i = 0; i < (లెక్క - 1); i++) {
		reg = &dom->regions[i];
		కొరకు (j = i + 1; j < లెక్క; j++) {
			reg1 = &dom->regions[j];

			ఒకవేళ (!is_region_before(reg1, reg))
				కొనసాగు;

			మార్పిడి_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	కొరకు (i = 0; i < (లెక్క - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		కొరకు (j = i + 1; j < లెక్క; j++) {
			reg1 = &dom->regions[j];

			ఒకవేళ (is_region_compatible(reg, reg1)) {
				is_covered = true;
				ఆపు;
			}
		}

		/* find a region is superset of reg, remove reg */
		ఒకవేళ (is_covered) {
			sbi_memmove(reg, reg + 1, పరిమాణం(*reg) * (లెక్క - i));
			లెక్క--;
		} లేకపోతే
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
	ఒకవేళ (dom->తదుపరి_బహుళకం != PRV_S &&
	    dom->తదుపరి_బహుళకం != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->తదుపరి_బహుళకం);
		తిరిగి SBI_EINVAL;
	}

	/* Check next address and next mode */
	ఒకవేళ (!sbi_domain_check_addr(dom, dom->తదుపరి_addr, dom->తదుపరి_బహుళకం,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->తదుపరి_addr);
		తిరిగి SBI_EINVAL;
	}

	తిరిగి 0;
}

bool sbi_domain_check_addr_range(స్థిరరాశి నిర్మాణం sbi_domain *dom,
				 గుర్తులేని దీర్ఘ addr, గుర్తులేని దీర్ఘ కొలత,
				 గుర్తులేని దీర్ఘ బహుళకం,
				 గుర్తులేని దీర్ఘ access_flags)
{
	గుర్తులేని దీర్ఘ max = addr + కొలత;
	స్థిరరాశి నిర్మాణం sbi_domain_memregion *reg, *sreg;

	ఒకవేళ (!dom)
		తిరిగి false;

	ఒకవేళ (కొలత && max <= addr)
		తిరిగి false;

	అయితే (addr < max) {
		reg = కనుగొను_region(dom, addr);
		ఒకవేళ (!reg)
			తిరిగి false;

		ఒకవేళ (!sbi_domain_check_addr(dom, addr, బహుళకం, access_flags))
			తిరిగి false;

		sreg = కనుగొను_తదుపరి_subset_region(dom, reg, addr);
		ఒకవేళ (sreg)
			addr = sreg->పునాది_2;
		లేకపోతే ఒకవేళ (reg->క్రమం < __riscv_xlen)
			addr = reg->పునాది_2 + (1UL << reg->క్రమం);
		లేకపోతే
			ఆపు;
	}

	తిరిగి true;
}

శూన్య sbi_domain_dump(స్థిరరాశి నిర్మాణం sbi_domain *dom, స్థిరరాశి అక్షరం *suffix)
{
	u32 i, j, k;
	గుర్తులేని దీర్ఘ rstart, rend;
	నిర్మాణం sbi_domain_memregion *reg;

	sbi_printf("Domain%d Name        %s: %s\n",
		   dom->సూచీ_2, suffix, dom->name);

	sbi_printf("Domain%d Init Order  %s: 0x%x\n",
		   dom->సూచీ_2, suffix, dom->init_క్రమం);

	sbi_printf("Domain%d Boot HART   %s: %d\n",
		   dom->సూచీ_2, suffix, dom->boot_hartid);

	k = 0;
	sbi_printf("Domain%d HARTs       %s: ", dom->సూచీ_2, suffix);
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		j = sbi_hartindex_గమ్యం_hartid(i);
		sbi_printf("%s0x%x%s", (k++) ? "," : "",
			   j, sbi_domain_is_assigned_hart(dom, i) ? "*" : "");
	}
	sbi_printf("\n");

	i = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->పునాది_2;
		rend = (reg->క్రమం < __riscv_xlen) ?
			rstart + ((1UL << reg->క్రమం) - 1) : -1UL;

		sbi_printf("Domain%d Region%02d    %s: 0x%" PRILX "-0x%" PRILX " ",
			   dom->సూచీ_2, i, suffix, rstart, rend);

		k = 0;

		sbi_printf("M: ");
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->సూచీ_2, suffix, dom->తదుపరి_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->సూచీ_2, suffix, dom->తదుపరి_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->సూచీ_2, suffix);
	ఎంపిక (dom->తదుపరి_బహుళకం) {
	స్థితి PRV_M:
		sbi_printf("M-mode\n");
		ఆపు;
	స్థితి PRV_S:
		sbi_printf("S-mode\n");
		ఆపు;
	స్థితి PRV_U:
		sbi_printf("U-mode\n");
		ఆపు;
	అప్రమేయ:
		sbi_printf("Unknown\n");
		ఆపు;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->సూచీ_2, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->సూచీ_2, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

శూన్య sbi_domain_dump_all(స్థిరరాశి అక్షరం *suffix)
{
	స్థిరరాశి నిర్మాణం sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

పూర్ణాంకం sbi_domain_రిజిస్టరు(నిర్మాణం sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	నిర్మాణం sbi_domain *tdom;
	పూర్ణాంకం rc;

	/* Sanity checks */
	ఒకవేళ (!dom || domain_finalized)
		తిరిగి SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		ఒకవేళ (tdom == dom ||
		    tdom->init_క్రమం == dom->init_క్రమం)
			తిరిగి SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	ఒకవేళ (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		తిరిగి rc;
	}

	sbi_list_కూడు_తోక(&dom->కణుపు, &domain_list);

	/* Assign index to domain */
	dom->సూచీ_2 = domain_లెక్క++;

	/* Initialize spinlock for dom->assigned_harts */
	SPIN_తాళం_INIT(dom->assigned_harts_తాళం);

	/* Clear assigned HARTs of domain */
	sbi_hartmask_clear_all(&dom->assigned_harts);

	/*
	 * Assign HART to a domain with the least initialization order
	 * where the HART is listed as a possible HART of the domain.
	 */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		tdom = sbi_hartindex_గమ్యం_domain(i);
		ఒకవేళ (tdom) {
			ఒకవేళ (tdom->init_క్రమం > dom->init_క్రమం)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			లేకపోతే
				కొనసాగు;
		}

		sbi_update_hartindex_గమ్యం_domain(i, dom);
		sbi_hartmask_సమితి_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		ఒకవేళ (sbi_hartindex_గమ్యం_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->సూచీ_2, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_అమరిక_state(dom);
	ఒకవేళ (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->కణుపు);
		తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం మూలం_కూడు_memregion(స్థిరరాశి నిర్మాణం sbi_domain_memregion *reg)
{
	పూర్ణాంకం rc;
	bool reg_merged;
	నిర్మాణం sbi_domain_memregion *nreg, *nreg1;
	పూర్ణాంకం మూలం_memregs_లెక్క = sbi_domain_వాడిన_memregions(&మూలం);

	/* Sanity checks */
	ఒకవేళ (!reg || domain_finalized || !మూలం.regions ||
	    (మూలం_REGION_MAX <= మూలం_memregs_లెక్క))
		తిరిగి SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&మూలం, nreg) {
		ఒకవేళ (is_region_compatible(reg, nreg))
			తిరిగి 0;
	}

	/* Append the memregion to root memregions */
	nreg = &మూలం.regions[మూలం_memregs_లెక్క];
	sbi_memcpy(nreg, reg, పరిమాణం(*reg));
	మూలం_memregs_లెక్క++;
	మూలం.regions[మూలం_memregs_లెక్క].క్రమం = 0;

	/* Sort and optimize root regions */
	చేయి {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&మూలం);
		ఒకవేళ (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   మూలం.name, rc);
			తిరిగి rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&మూలం, nreg) {
			nreg1 = nreg + 1;
			ఒకవేళ (!nreg1->క్రమం)
				కొనసాగు;

			ఒకవేళ (!(nreg->పునాది_2 & (బిట్టు(nreg->క్రమం + 1) - 1)) &&
			    (nreg->పునాది_2 + బిట్టు(nreg->క్రమం)) == nreg1->పునాది_2 &&
			    nreg->క్రమం == nreg1->క్రమం &&
			    nreg->flags == nreg1->flags) {
				పూర్ణాంకం i1 = nreg1 - మూలం.regions;
				nreg->క్రమం++;
				sbi_memmove(nreg1, nreg1 + 1,
					    పరిమాణం(*nreg1) * (మూలం_memregs_లెక్క - i1));
				reg_merged = true;
				మూలం_memregs_లెక్క--;
			}
		}
	} అయితే (reg_merged);

	తిరిగి 0;
}

పూర్ణాంకం sbi_domain_మూలం_కూడు_memrange(గుర్తులేని దీర్ఘ addr, గుర్తులేని దీర్ఘ కొలత,
			   గుర్తులేని దీర్ఘ align, గుర్తులేని దీర్ఘ region_flags)
{
	పూర్ణాంకం rc;
	గుర్తులేని దీర్ఘ pos, చివర, rsize;
	నిర్మాణం sbi_domain_memregion reg;

	pos = addr;
	చివర = addr + కొలత;
	అయితే (pos < చివర) {
		rsize = pos & (align - 1);
		ఒకవేళ (rsize)
			rsize = 1UL << sbi_ffs(pos);
		లేకపోతే
			rsize = ((చివర - pos) < align) ?
				(చివర - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = మూలం_కూడు_memregion(&reg);
		ఒకవేళ (rc)
			తిరిగి rc;
		pos += rsize;
	}

	తిరిగి 0;
}

పూర్ణాంకం sbi_domain_startup(నిర్మాణం sbi_scratch *scratch, u32 cold_hartid)
{
	పూర్ణాంకం rc;
	u32 dhart;
	నిర్మాణం sbi_domain *dom;

	/* Sanity checks */
	ఒకవేళ (!domain_finalized)
		తిరిగి SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_గమ్యం_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		ఒకవేళ (!sbi_hartindex_valid(dhart))
			కొనసాగు;

		/* Ignore if boot HART not possible for this domain */
		ఒకవేళ (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			కొనసాగు;

		/* Ignore if boot HART assigned different domain */
		ఒకవేళ (sbi_hartindex_గమ్యం_domain(dhart) != dom)
			కొనసాగు;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_తాళం(&dom->assigned_harts_తాళం);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_తాళం);
		ఒకవేళ (!rc)
			కొనసాగు;

		/* Startup boot HART of domain */
		ఒకవేళ (dom->boot_hartid == cold_hartid) {
			scratch->తదుపరి_addr = dom->తదుపరి_addr;
			scratch->తదుపరి_బహుళకం = dom->తదుపరి_బహుళకం;
			scratch->తదుపరి_arg1 = dom->తదుపరి_arg1;
		} లేకపోతే {
			rc = sbi_hsm_hart_ప్రారంభం(scratch, ఖాళీ,
						dom->boot_hartid,
						dom->తదుపరి_addr,
						dom->తదుపరి_బహుళకం,
						dom->తదుపరి_arg1);
			ఒకవేళ (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				తిరిగి rc;
			}
		}
	}

	తిరిగి 0;
}

పూర్ణాంకం sbi_domain_finalize(నిర్మాణం sbi_scratch *scratch)
{
	పూర్ణాంకం rc;
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	ఒకవేళ (domain_finalized)
		తిరిగి SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	ఒకవేళ (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		తిరిగి rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	తిరిగి 0;
}

పూర్ణాంకం sbi_domain_init(నిర్మాణం sbi_scratch *scratch, u32 cold_hartid)
{
	పూర్ణాంకం rc;
	నిర్మాణం sbi_hartmask *మూలం_hmask;
	నిర్మాణం sbi_domain_memregion *మూలం_memregs;
	పూర్ణాంకం మూలం_memregs_లెక్క = 0;
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_తల(&domain_list);

	ఒకవేళ (!fw_single_region) {
		ఒకవేళ (scratch->fw_rw_స్థానభ్రంశం == 0 ||
		    (scratch->fw_rw_స్థానభ్రంశం & (scratch->fw_rw_స్థానభ్రంశం - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_స్థానభ్రంశం);
			తిరిగి SBI_EINVAL;
		}
		ఒకవేళ ((scratch->fw_ప్రారంభం & (scratch->fw_rw_స్థానభ్రంశం - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			తిరిగి SBI_EINVAL;
		}
	}

	domain_hart_ptr_స్థానభ్రంశం = sbi_scratch_alloc_type_స్థానభ్రంశం(శూన్య *);
	ఒకవేళ (!domain_hart_ptr_స్థానభ్రంశం)
		తిరిగి SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	ఒకవేళ (rc)
		వెళ్ళు fail_free_domain_hart_ptr_స్థానభ్రంశం;

	మూలం_memregs = sbi_calloc(పరిమాణం(*మూలం_memregs), మూలం_REGION_MAX + 1);
	ఒకవేళ (!మూలం_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		వెళ్ళు fail_deinit_context;
	}
	మూలం.regions = మూలం_memregs;

	మూలం_hmask = sbi_zalloc(పరిమాణం(*మూలం_hmask));
	ఒకవేళ (!మూలం_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		వెళ్ళు fail_free_మూలం_memregs;
	}
	మూలం.possible_harts = మూలం_hmask;

	/* Root domain firmware memory region */
	ఒకవేళ (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_ప్రారంభం, scratch->fw_కొలత,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &మూలం_memregs[మూలం_memregs_లెక్క++]);
	} లేకపోతే {
		sbi_domain_memregion_init(scratch->fw_ప్రారంభం,
					  scratch->fw_rw_స్థానభ్రంశం,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &మూలం_memregs[మూలం_memregs_లెక్క++]);

		sbi_domain_memregion_init((scratch->fw_ప్రారంభం +
					   scratch->fw_rw_స్థానభ్రంశం),
					  (scratch->fw_కొలత -
					   scratch->fw_rw_స్థానభ్రంశం),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &మూలం_memregs[మూలం_memregs_లెక్క++]);
	}

	మూలం.fw_region_inited = true;

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
				  &మూలం_memregs[మూలం_memregs_లెక్క++]);

	/* Root domain memory region end */
	మూలం_memregs[మూలం_memregs_లెక్క].క్రమం = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	మూలం.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	మూలం.తదుపరి_arg1 = scratch->తదుపరి_arg1;
	మూలం.తదుపరి_addr = scratch->తదుపరి_addr;
	మూలం.తదుపరి_బహుళకం = scratch->తదుపరి_బహుళకం;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_సమితి_hartindex(i, మూలం_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_రిజిస్టరు(&మూలం);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_మూలం_hmask;

	తిరిగి 0;

fail_free_మూలం_hmask:
	sbi_free(మూలం_hmask);
fail_free_మూలం_memregs:
	sbi_free(మూలం_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_స్థానభ్రంశం:
	sbi_scratch_free_స్థానభ్రంశం(domain_hart_ptr_స్థానభ్రంశం);
	తిరిగి rc;
}
