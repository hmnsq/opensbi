/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_hartmask.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_list.h>
#شامل <sbi/sbi_math.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>

SBI_LIST_سر(domain_list);

ساکن u32 domain_گنتی = 0;
ساکن bool domain_finalized = false;

#تعریف_کرو جڑ_REGION_MAX	32

ساخت sbi_domain جڑ = {
	.name = "root",
	.init_ترتیب = -1U,
	.possible_harts = تہی,
	.regions = تہی,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

ساکن غیرعلامتی طویل domain_hart_ptr_انحراف;

ساخت sbi_domain *sbi_hartindex_مقصد_domain(u32 hartindex)
{
	ساخت sbi_scratch *scratch;

	scratch = sbi_hartindex_مقصد_scratch(hartindex);
	اگر (!scratch || !domain_hart_ptr_انحراف)
		واپس تہی;

	واپس sbi_scratch_read_type(scratch, خالی *, domain_hart_ptr_انحراف);
}

خالی sbi_update_hartindex_مقصد_domain(u32 hartindex, ساخت sbi_domain *dom)
{
	ساخت sbi_scratch *scratch;

	scratch = sbi_hartindex_مقصد_scratch(hartindex);
	اگر (!scratch)
		واپس;

	sbi_scratch_write_type(scratch, خالی *, domain_hart_ptr_انحراف, dom);
}

bool sbi_domain_is_assigned_hart(ثابت ساخت sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_تالا)
{
	bool ret;
	ساخت sbi_domain *tdom = (ساخت sbi_domain *)dom;

	اگر (!dom)
		واپس false;

	spin_تالا(&tdom->assigned_harts_تالا);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_تالا);

	واپس ret;
}

صحیح sbi_domain_get_assigned_hartmask(ثابت ساخت sbi_domain *dom,
				     ساخت sbi_hartmask *نقاب)
	MUST_NOT_HOLD(&dom->assigned_harts_تالا)
{
	ulong ret = 0;
	ساخت sbi_domain *tdom = (ساخت sbi_domain *)dom;

	اگر (!dom) {
		sbi_hartmask_clear_all(نقاب);
		واپس 0;
	}

	spin_تالا(&tdom->assigned_harts_تالا);
	sbi_hartmask_copy(نقاب, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_تالا);

	واپس ret;
}

خالی sbi_domain_memregion_init(غیرعلامتی طویل addr,
				غیرعلامتی طویل سائز,
				غیرعلامتی طویل flags,
				ساخت sbi_domain_memregion *reg)
{
	غیرعلامتی طویل بنیاد_2 = 0, ترتیب;

	برائے (ترتیب = log2roundup(سائز) ; ترتیب <= __riscv_xlen; ترتیب++) {
		اگر (ترتیب < __riscv_xlen) {
			بنیاد_2 = addr & ~((1UL << ترتیب) - 1UL);
			اگر ((بنیاد_2 <= addr) &&
			    (addr < (بنیاد_2 + (1UL << ترتیب))) &&
			    (بنیاد_2 <= (addr + سائز - 1UL)) &&
			    ((addr + سائز - 1UL) < (بنیاد_2 + (1UL << ترتیب))))
				توڑو;
		} ورنہ {
			بنیاد_2 = 0;
			توڑو;
		}

	}

	اگر (reg) {
		reg->بنیاد_2 = بنیاد_2;
		reg->ترتیب = ترتیب;
		reg->flags = flags;
	}
}

غیرعلامتی صحیح sbi_domain_get_oldpmp_flags(ساخت sbi_domain_memregion *reg)
{

	غیرعلامتی صحیح pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	اگر (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	واپس pmp_flags;
}

غیرعلامتی صحیح sbi_domain_get_smepmp_flags(ساخت sbi_domain_memregion *reg)
{
	غیرعلامتی صحیح pmp_flags = 0;
	غیرعلامتی طویل rstart, rend;

	اگر ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_نقاب) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		واپس 0;
	} ورنہ اگر (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		اگر (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		ورنہ اگر (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		ورنہ اگر (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		ورنہ اگر (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		ورنہ اگر (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} ورنہ اگر (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		اگر ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_نقاب)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			واپس 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		اگر (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} ورنہ اگر (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} ورنہ {
		rstart = reg->بنیاد_2;
		rend = (reg->ترتیب < __riscv_xlen) ? rstart + ((1UL << reg->ترتیب) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	واپس pmp_flags;
}

bool sbi_domain_check_addr(ثابت ساخت sbi_domain *dom,
			   غیرعلامتی طویل addr, غیرعلامتی طویل منوال,
			   غیرعلامتی طویل access_flags)
{
	bool rmmio, mmio = false;
	ساخت sbi_domain_memregion *reg;
	غیرعلامتی طویل rstart, rend, rflags, rwx = 0, rrwx = 0;

	اگر (!dom)
		واپس false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	اگر (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	اگر (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	اگر (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	اگر (access_flags & SBI_DOMAIN_MMIO)
		mmio = true;

	sbi_domain_for_each_memregion(dom, reg) {
		rflags = reg->flags;
		rrwx = (منوال == PRV_M ?
			(rflags & SBI_DOMAIN_MEMREGION_M_ACCESS_نقاب) :
			(rflags & SBI_DOMAIN_MEMREGION_SU_ACCESS_نقاب)
			>> SBI_DOMAIN_MEMREGION_SU_ACCESS_سرکاؤ);

		rstart = reg->بنیاد_2;
		rend = (reg->ترتیب < __riscv_xlen) ?
			rstart + ((1UL << reg->ترتیب) - 1) : -1UL;
		اگر (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			اگر (!mmio && rmmio)
				واپس false;
			واپس ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	واپس (منوال == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
ساکن bool is_region_valid(ثابت ساخت sbi_domain_memregion *reg)
{
	اگر (reg->ترتیب < 3 || __riscv_xlen < reg->ترتیب)
		واپس false;

	اگر (reg->ترتیب == __riscv_xlen && reg->بنیاد_2 != 0)
		واپس false;

	اگر (reg->ترتیب < __riscv_xlen && (reg->بنیاد_2 & (ثنائی_ہندسہ(reg->ترتیب) - 1)))
		واپس false;

	واپس true;
}

/** Check if regionA can be replaced by regionB */
ساکن bool is_region_compatible(ثابت ساخت sbi_domain_memregion *regA,
				 ثابت ساخت sbi_domain_memregion *regB)
{
	اگر (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		واپس true;

	واپس false;
}

/** Check if regionA should be placed before regionB */
ساکن bool is_region_before(ثابت ساخت sbi_domain_memregion *regA,
			     ثابت ساخت sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	اگر (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		واپس true;
	اگر (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		واپس false;

	اگر (regA->ترتیب < regB->ترتیب)
		واپس true;

	اگر ((regA->ترتیب == regB->ترتیب) &&
	    (regA->بنیاد_2 < regB->بنیاد_2))
		واپس true;

	واپس false;
}

ساکن ثابت ساخت sbi_domain_memregion *ڈھونڈو_region(
						ثابت ساخت sbi_domain *dom,
						غیرعلامتی طویل addr)
{
	غیرعلامتی طویل rstart, rend;
	ساخت sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->بنیاد_2;
		rend = (reg->ترتیب < __riscv_xlen) ?
			rstart + ((1UL << reg->ترتیب) - 1) : -1UL;
		اگر (rstart <= addr && addr <= rend)
			واپس reg;
	}

	واپس تہی;
}

ساکن ثابت ساخت sbi_domain_memregion *ڈھونڈو_اگلا_subset_region(
				ثابت ساخت sbi_domain *dom,
				ثابت ساخت sbi_domain_memregion *reg,
				غیرعلامتی طویل addr)
{
	ساخت sbi_domain_memregion *sreg, *ret = تہی;

	sbi_domain_for_each_memregion(dom, sreg) {
		اگر (sreg == reg || (sreg->بنیاد_2 <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			جاری;

		اگر (!ret || (sreg->بنیاد_2 < ret->بنیاد_2) ||
		    ((sreg->بنیاد_2 == ret->بنیاد_2) && (sreg->ترتیب < ret->ترتیب)))
			ret = sreg;
	}

	واپس ret;
}

ساکن خالی تبادلہ_region(ساخت sbi_domain_memregion* reg1,
			ساخت sbi_domain_memregion* reg2)
{
	ساخت sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, حجم(treg));
	sbi_memcpy(reg1, reg2, حجم(treg));
	sbi_memcpy(reg2, &treg, حجم(treg));
}

ساکن صحیح sbi_domain_مستعمل_memregions(ثابت ساخت sbi_domain *dom)
{
	صحیح گنتی = 0;
	ساخت sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		گنتی++;

	واپس گنتی;
}

ساکن صحیح sanitize_domain(ساخت sbi_domain *dom)
{
	u32 i, j, گنتی;
	bool is_covered;
	ساخت sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	اگر (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		واپس SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		اگر (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_مقصد_hartid(i));
			واپس SBI_EINVAL;
		}
	}

	/* Check memory regions */
	اگر (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		واپس SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		اگر (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->بنیاد_2, reg->ترتیب,
				   reg->flags);
			واپس SBI_EINVAL;
		}
	}

	/* Count memory regions */
	گنتی = sbi_domain_مستعمل_memregions(dom);

	/* Check presence of firmware regions */
	اگر (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		واپس SBI_EINVAL;
	}

	/* Sort the memory regions */
	برائے (i = 0; i < (گنتی - 1); i++) {
		reg = &dom->regions[i];
		برائے (j = i + 1; j < گنتی; j++) {
			reg1 = &dom->regions[j];

			اگر (!is_region_before(reg1, reg))
				جاری;

			تبادلہ_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	برائے (i = 0; i < (گنتی - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		برائے (j = i + 1; j < گنتی; j++) {
			reg1 = &dom->regions[j];

			اگر (is_region_compatible(reg, reg1)) {
				is_covered = true;
				توڑو;
			}
		}

		/* find a region is superset of reg, remove reg */
		اگر (is_covered) {
			sbi_memmove(reg, reg + 1, حجم(*reg) * (گنتی - i));
			گنتی--;
		} ورنہ
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
	اگر (dom->اگلا_منوال != PRV_S &&
	    dom->اگلا_منوال != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->اگلا_منوال);
		واپس SBI_EINVAL;
	}

	/* Check next address and next mode */
	اگر (!sbi_domain_check_addr(dom, dom->اگلا_addr, dom->اگلا_منوال,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->اگلا_addr);
		واپس SBI_EINVAL;
	}

	واپس 0;
}

bool sbi_domain_check_addr_range(ثابت ساخت sbi_domain *dom,
				 غیرعلامتی طویل addr, غیرعلامتی طویل سائز,
				 غیرعلامتی طویل منوال,
				 غیرعلامتی طویل access_flags)
{
	غیرعلامتی طویل max = addr + سائز;
	ثابت ساخت sbi_domain_memregion *reg, *sreg;

	اگر (!dom)
		واپس false;

	اگر (سائز && max <= addr)
		واپس false;

	جب (addr < max) {
		reg = ڈھونڈو_region(dom, addr);
		اگر (!reg)
			واپس false;

		اگر (!sbi_domain_check_addr(dom, addr, منوال, access_flags))
			واپس false;

		sreg = ڈھونڈو_اگلا_subset_region(dom, reg, addr);
		اگر (sreg)
			addr = sreg->بنیاد_2;
		ورنہ اگر (reg->ترتیب < __riscv_xlen)
			addr = reg->بنیاد_2 + (1UL << reg->ترتیب);
		ورنہ
			توڑو;
	}

	واپس true;
}

خالی sbi_domain_dump(ثابت ساخت sbi_domain *dom, ثابت حرف *suffix)
{
	u32 i, j, k;
	غیرعلامتی طویل rstart, rend;
	ساخت sbi_domain_memregion *reg;

	sbi_printf("Domain%d Name        %s: %s\n",
		   dom->اشاریہ_2, suffix, dom->name);

	sbi_printf("Domain%d Init Order  %s: 0x%x\n",
		   dom->اشاریہ_2, suffix, dom->init_ترتیب);

	sbi_printf("Domain%d Boot HART   %s: %d\n",
		   dom->اشاریہ_2, suffix, dom->boot_hartid);

	k = 0;
	sbi_printf("Domain%d HARTs       %s: ", dom->اشاریہ_2, suffix);
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		j = sbi_hartindex_مقصد_hartid(i);
		sbi_printf("%s0x%x%s", (k++) ? "," : "",
			   j, sbi_domain_is_assigned_hart(dom, i) ? "*" : "");
	}
	sbi_printf("\n");

	i = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->بنیاد_2;
		rend = (reg->ترتیب < __riscv_xlen) ?
			rstart + ((1UL << reg->ترتیب) - 1) : -1UL;

		sbi_printf("Domain%d Region%02d    %s: 0x%" PRILX "-0x%" PRILX " ",
			   dom->اشاریہ_2, i, suffix, rstart, rend);

		k = 0;

		sbi_printf("M: ");
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->اشاریہ_2, suffix, dom->اگلا_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->اشاریہ_2, suffix, dom->اگلا_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->اشاریہ_2, suffix);
	انتخاب (dom->اگلا_منوال) {
	حالت PRV_M:
		sbi_printf("M-mode\n");
		توڑو;
	حالت PRV_S:
		sbi_printf("S-mode\n");
		توڑو;
	حالت PRV_U:
		sbi_printf("U-mode\n");
		توڑو;
	طےشدہ:
		sbi_printf("Unknown\n");
		توڑو;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->اشاریہ_2, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->اشاریہ_2, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

خالی sbi_domain_dump_all(ثابت حرف *suffix)
{
	ثابت ساخت sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

صحیح sbi_domain_رجسٹر(ساخت sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	ساخت sbi_domain *tdom;
	صحیح rc;

	/* Sanity checks */
	اگر (!dom || domain_finalized)
		واپس SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		اگر (tdom == dom ||
		    tdom->init_ترتیب == dom->init_ترتیب)
			واپس SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	اگر (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		واپس rc;
	}

	sbi_list_جوڑو_دم(&dom->گرہ, &domain_list);

	/* Assign index to domain */
	dom->اشاریہ_2 = domain_گنتی++;

	/* Initialize spinlock for dom->assigned_harts */
	SPIN_تالا_INIT(dom->assigned_harts_تالا);

	/* Clear assigned HARTs of domain */
	sbi_hartmask_clear_all(&dom->assigned_harts);

	/*
	 * Assign HART to a domain with the least initialization order
	 * where the HART is listed as a possible HART of the domain.
	 */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		tdom = sbi_hartindex_مقصد_domain(i);
		اگر (tdom) {
			اگر (tdom->init_ترتیب > dom->init_ترتیب)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			ورنہ
				جاری;
		}

		sbi_update_hartindex_مقصد_domain(i, dom);
		sbi_hartmask_سیٹ_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		اگر (sbi_hartindex_مقصد_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->اشاریہ_2, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_تیاری_state(dom);
	اگر (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->گرہ);
		واپس rc;
	}

	واپس 0;
}

ساکن صحیح جڑ_جوڑو_memregion(ثابت ساخت sbi_domain_memregion *reg)
{
	صحیح rc;
	bool reg_merged;
	ساخت sbi_domain_memregion *nreg, *nreg1;
	صحیح جڑ_memregs_گنتی = sbi_domain_مستعمل_memregions(&جڑ);

	/* Sanity checks */
	اگر (!reg || domain_finalized || !جڑ.regions ||
	    (جڑ_REGION_MAX <= جڑ_memregs_گنتی))
		واپس SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&جڑ, nreg) {
		اگر (is_region_compatible(reg, nreg))
			واپس 0;
	}

	/* Append the memregion to root memregions */
	nreg = &جڑ.regions[جڑ_memregs_گنتی];
	sbi_memcpy(nreg, reg, حجم(*reg));
	جڑ_memregs_گنتی++;
	جڑ.regions[جڑ_memregs_گنتی].ترتیب = 0;

	/* Sort and optimize root regions */
	کرو {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&جڑ);
		اگر (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   جڑ.name, rc);
			واپس rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&جڑ, nreg) {
			nreg1 = nreg + 1;
			اگر (!nreg1->ترتیب)
				جاری;

			اگر (!(nreg->بنیاد_2 & (ثنائی_ہندسہ(nreg->ترتیب + 1) - 1)) &&
			    (nreg->بنیاد_2 + ثنائی_ہندسہ(nreg->ترتیب)) == nreg1->بنیاد_2 &&
			    nreg->ترتیب == nreg1->ترتیب &&
			    nreg->flags == nreg1->flags) {
				صحیح i1 = nreg1 - جڑ.regions;
				nreg->ترتیب++;
				sbi_memmove(nreg1, nreg1 + 1,
					    حجم(*nreg1) * (جڑ_memregs_گنتی - i1));
				reg_merged = true;
				جڑ_memregs_گنتی--;
			}
		}
	} جب (reg_merged);

	واپس 0;
}

صحیح sbi_domain_جڑ_جوڑو_memrange(غیرعلامتی طویل addr, غیرعلامتی طویل سائز,
			   غیرعلامتی طویل align, غیرعلامتی طویل region_flags)
{
	صحیح rc;
	غیرعلامتی طویل pos, انتہا, rsize;
	ساخت sbi_domain_memregion reg;

	pos = addr;
	انتہا = addr + سائز;
	جب (pos < انتہا) {
		rsize = pos & (align - 1);
		اگر (rsize)
			rsize = 1UL << sbi_ffs(pos);
		ورنہ
			rsize = ((انتہا - pos) < align) ?
				(انتہا - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = جڑ_جوڑو_memregion(&reg);
		اگر (rc)
			واپس rc;
		pos += rsize;
	}

	واپس 0;
}

صحیح sbi_domain_startup(ساخت sbi_scratch *scratch, u32 cold_hartid)
{
	صحیح rc;
	u32 dhart;
	ساخت sbi_domain *dom;

	/* Sanity checks */
	اگر (!domain_finalized)
		واپس SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_مقصد_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		اگر (!sbi_hartindex_valid(dhart))
			جاری;

		/* Ignore if boot HART not possible for this domain */
		اگر (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			جاری;

		/* Ignore if boot HART assigned different domain */
		اگر (sbi_hartindex_مقصد_domain(dhart) != dom)
			جاری;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_تالا(&dom->assigned_harts_تالا);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_تالا);
		اگر (!rc)
			جاری;

		/* Startup boot HART of domain */
		اگر (dom->boot_hartid == cold_hartid) {
			scratch->اگلا_addr = dom->اگلا_addr;
			scratch->اگلا_منوال = dom->اگلا_منوال;
			scratch->اگلا_arg1 = dom->اگلا_arg1;
		} ورنہ {
			rc = sbi_hsm_hart_آغاز(scratch, تہی,
						dom->boot_hartid,
						dom->اگلا_addr,
						dom->اگلا_منوال,
						dom->اگلا_arg1);
			اگر (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				واپس rc;
			}
		}
	}

	واپس 0;
}

صحیح sbi_domain_finalize(ساخت sbi_scratch *scratch)
{
	صحیح rc;
	ثابت ساخت sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	اگر (domain_finalized)
		واپس SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	اگر (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		واپس rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	واپس 0;
}

صحیح sbi_domain_init(ساخت sbi_scratch *scratch, u32 cold_hartid)
{
	صحیح rc;
	ساخت sbi_hartmask *جڑ_hmask;
	ساخت sbi_domain_memregion *جڑ_memregs;
	صحیح جڑ_memregs_گنتی = 0;
	ثابت ساخت sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_سر(&domain_list);

	اگر (!fw_single_region) {
		اگر (scratch->fw_rw_انحراف == 0 ||
		    (scratch->fw_rw_انحراف & (scratch->fw_rw_انحراف - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_انحراف);
			واپس SBI_EINVAL;
		}
		اگر ((scratch->fw_آغاز & (scratch->fw_rw_انحراف - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			واپس SBI_EINVAL;
		}
	}

	domain_hart_ptr_انحراف = sbi_scratch_alloc_type_انحراف(خالی *);
	اگر (!domain_hart_ptr_انحراف)
		واپس SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	اگر (rc)
		جاؤ fail_free_domain_hart_ptr_انحراف;

	جڑ_memregs = sbi_calloc(حجم(*جڑ_memregs), جڑ_REGION_MAX + 1);
	اگر (!جڑ_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		جاؤ fail_deinit_context;
	}
	جڑ.regions = جڑ_memregs;

	جڑ_hmask = sbi_zalloc(حجم(*جڑ_hmask));
	اگر (!جڑ_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		جاؤ fail_free_جڑ_memregs;
	}
	جڑ.possible_harts = جڑ_hmask;

	/* Root domain firmware memory region */
	اگر (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_آغاز, scratch->fw_سائز,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &جڑ_memregs[جڑ_memregs_گنتی++]);
	} ورنہ {
		sbi_domain_memregion_init(scratch->fw_آغاز,
					  scratch->fw_rw_انحراف,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &جڑ_memregs[جڑ_memregs_گنتی++]);

		sbi_domain_memregion_init((scratch->fw_آغاز +
					   scratch->fw_rw_انحراف),
					  (scratch->fw_سائز -
					   scratch->fw_rw_انحراف),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &جڑ_memregs[جڑ_memregs_گنتی++]);
	}

	جڑ.fw_region_inited = true;

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
				  &جڑ_memregs[جڑ_memregs_گنتی++]);

	/* Root domain memory region end */
	جڑ_memregs[جڑ_memregs_گنتی].ترتیب = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	جڑ.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	جڑ.اگلا_arg1 = scratch->اگلا_arg1;
	جڑ.اگلا_addr = scratch->اگلا_addr;
	جڑ.اگلا_منوال = scratch->اگلا_منوال;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_سیٹ_hartindex(i, جڑ_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_رجسٹر(&جڑ);
	اگر (rc)
		جاؤ fail_free_جڑ_hmask;

	واپس 0;

fail_free_جڑ_hmask:
	sbi_free(جڑ_hmask);
fail_free_جڑ_memregs:
	sbi_free(جڑ_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_انحراف:
	sbi_scratch_free_انحراف(domain_hart_ptr_انحراف);
	واپس rc;
}
