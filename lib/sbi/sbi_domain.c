/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_hartmask.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <sbi/sbi_list.h>
#ضمن <sbi/sbi_math.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_string.h>

SBI_LIST_سر(domain_list);

ساكن u32 domain_شمار = 0;
ساكن bool domain_finalized = false;

#تعریف_کن ریشه_REGION_MAX	32

بنية sbi_domain ریشه = {
	.name = "root",
	.init_ترتیب = -1U,
	.possible_harts = عدم,
	.regions = عدم,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

ساكن بیعلامت دراز domain_hart_ptr_جابه_جایی;

بنية sbi_domain *sbi_hartindex_مقصد_domain(u32 hartindex)
{
	بنية sbi_scratch *scratch;

	scratch = sbi_hartindex_مقصد_scratch(hartindex);
	إذا (!scratch || !domain_hart_ptr_جابه_جایی)
		أرجع عدم;

	أرجع sbi_scratch_read_type(scratch, فراغ *, domain_hart_ptr_جابه_جایی);
}

فراغ sbi_update_hartindex_مقصد_domain(u32 hartindex, بنية sbi_domain *dom)
{
	بنية sbi_scratch *scratch;

	scratch = sbi_hartindex_مقصد_scratch(hartindex);
	إذا (!scratch)
		أرجع;

	sbi_scratch_write_type(scratch, فراغ *, domain_hart_ptr_جابه_جایی, dom);
}

bool sbi_domain_is_assigned_hart(ثابت بنية sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_قفل)
{
	bool ret;
	بنية sbi_domain *tdom = (بنية sbi_domain *)dom;

	إذا (!dom)
		أرجع false;

	spin_قفل(&tdom->assigned_harts_قفل);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_قفل);

	أرجع ret;
}

صحيح sbi_domain_get_assigned_hartmask(ثابت بنية sbi_domain *dom,
				     بنية sbi_hartmask *ماسک)
	MUST_NOT_HOLD(&dom->assigned_harts_قفل)
{
	ulong ret = 0;
	بنية sbi_domain *tdom = (بنية sbi_domain *)dom;

	إذا (!dom) {
		sbi_hartmask_clear_all(ماسک);
		أرجع 0;
	}

	spin_قفل(&tdom->assigned_harts_قفل);
	sbi_hartmask_copy(ماسک, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_قفل);

	أرجع ret;
}

فراغ sbi_domain_memregion_init(بیعلامت دراز addr,
				بیعلامت دراز اندازه,
				بیعلامت دراز flags,
				بنية sbi_domain_memregion *reg)
{
	بیعلامت دراز پایه_2 = 0, ترتیب;

	لكل (ترتیب = log2roundup(اندازه) ; ترتیب <= __riscv_xlen; ترتیب++) {
		إذا (ترتیب < __riscv_xlen) {
			پایه_2 = addr & ~((1UL << ترتیب) - 1UL);
			إذا ((پایه_2 <= addr) &&
			    (addr < (پایه_2 + (1UL << ترتیب))) &&
			    (پایه_2 <= (addr + اندازه - 1UL)) &&
			    ((addr + اندازه - 1UL) < (پایه_2 + (1UL << ترتیب))))
				اكسر;
		} وإلا {
			پایه_2 = 0;
			اكسر;
		}

	}

	إذا (reg) {
		reg->پایه_2 = پایه_2;
		reg->ترتیب = ترتیب;
		reg->flags = flags;
	}
}

بیعلامت صحيح sbi_domain_get_oldpmp_flags(بنية sbi_domain_memregion *reg)
{

	بیعلامت صحيح pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	إذا (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	إذا (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	إذا (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	إذا (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	أرجع pmp_flags;
}

بیعلامت صحيح sbi_domain_get_smepmp_flags(بنية sbi_domain_memregion *reg)
{
	بیعلامت صحيح pmp_flags = 0;
	بیعلامت دراز rstart, rend;

	إذا ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_ماسک) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		أرجع 0;
	} وإلا إذا (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		إذا (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		وإلا إذا (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		وإلا إذا (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		وإلا إذا (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		وإلا إذا (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} وإلا إذا (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		إذا ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_ماسک)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			أرجع 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		إذا (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} وإلا إذا (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} وإلا {
		rstart = reg->پایه_2;
		rend = (reg->ترتیب < __riscv_xlen) ? rstart + ((1UL << reg->ترتیب) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	أرجع pmp_flags;
}

bool sbi_domain_check_addr(ثابت بنية sbi_domain *dom,
			   بیعلامت دراز addr, بیعلامت دراز نما,
			   بیعلامت دراز access_flags)
{
	bool rmmio, mmio = false;
	بنية sbi_domain_memregion *reg;
	بیعلامت دراز rstart, rend, rflags, rwx = 0, rrwx = 0;

	إذا (!dom)
		أرجع false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	إذا (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	إذا (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	إذا (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	إذا (access_flags & SBI_DOMAIN_MMIO)
		mmio = true;

	sbi_domain_for_each_memregion(dom, reg) {
		rflags = reg->flags;
		rrwx = (نما == PRV_M ?
			(rflags & SBI_DOMAIN_MEMREGION_M_ACCESS_ماسک) :
			(rflags & SBI_DOMAIN_MEMREGION_SU_ACCESS_ماسک)
			>> SBI_DOMAIN_MEMREGION_SU_ACCESS_لغزش);

		rstart = reg->پایه_2;
		rend = (reg->ترتیب < __riscv_xlen) ?
			rstart + ((1UL << reg->ترتیب) - 1) : -1UL;
		إذا (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			إذا (!mmio && rmmio)
				أرجع false;
			أرجع ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	أرجع (نما == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
ساكن bool is_region_valid(ثابت بنية sbi_domain_memregion *reg)
{
	إذا (reg->ترتیب < 3 || __riscv_xlen < reg->ترتیب)
		أرجع false;

	إذا (reg->ترتیب == __riscv_xlen && reg->پایه_2 != 0)
		أرجع false;

	إذا (reg->ترتیب < __riscv_xlen && (reg->پایه_2 & (بیت(reg->ترتیب) - 1)))
		أرجع false;

	أرجع true;
}

/** Check if regionA can be replaced by regionB */
ساكن bool is_region_compatible(ثابت بنية sbi_domain_memregion *regA,
				 ثابت بنية sbi_domain_memregion *regB)
{
	إذا (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		أرجع true;

	أرجع false;
}

/** Check if regionA should be placed before regionB */
ساكن bool is_region_before(ثابت بنية sbi_domain_memregion *regA,
			     ثابت بنية sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	إذا (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		أرجع true;
	إذا (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		أرجع false;

	إذا (regA->ترتیب < regB->ترتیب)
		أرجع true;

	إذا ((regA->ترتیب == regB->ترتیب) &&
	    (regA->پایه_2 < regB->پایه_2))
		أرجع true;

	أرجع false;
}

ساكن ثابت بنية sbi_domain_memregion *پیدا_کن_region(
						ثابت بنية sbi_domain *dom,
						بیعلامت دراز addr)
{
	بیعلامت دراز rstart, rend;
	بنية sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->پایه_2;
		rend = (reg->ترتیب < __riscv_xlen) ?
			rstart + ((1UL << reg->ترتیب) - 1) : -1UL;
		إذا (rstart <= addr && addr <= rend)
			أرجع reg;
	}

	أرجع عدم;
}

ساكن ثابت بنية sbi_domain_memregion *پیدا_کن_بعدی_subset_region(
				ثابت بنية sbi_domain *dom,
				ثابت بنية sbi_domain_memregion *reg,
				بیعلامت دراز addr)
{
	بنية sbi_domain_memregion *sreg, *ret = عدم;

	sbi_domain_for_each_memregion(dom, sreg) {
		إذا (sreg == reg || (sreg->پایه_2 <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			استمر;

		إذا (!ret || (sreg->پایه_2 < ret->پایه_2) ||
		    ((sreg->پایه_2 == ret->پایه_2) && (sreg->ترتیب < ret->ترتیب)))
			ret = sreg;
	}

	أرجع ret;
}

ساكن فراغ جابجا_region(بنية sbi_domain_memregion* reg1,
			بنية sbi_domain_memregion* reg2)
{
	بنية sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, حجم(treg));
	sbi_memcpy(reg1, reg2, حجم(treg));
	sbi_memcpy(reg2, &treg, حجم(treg));
}

ساكن صحيح sbi_domain_استفاده_شده_memregions(ثابت بنية sbi_domain *dom)
{
	صحيح شمار = 0;
	بنية sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		شمار++;

	أرجع شمار;
}

ساكن صحيح sanitize_domain(بنية sbi_domain *dom)
{
	u32 i, j, شمار;
	bool is_covered;
	بنية sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	إذا (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		أرجع SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		إذا (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_مقصد_hartid(i));
			أرجع SBI_EINVAL;
		}
	}

	/* Check memory regions */
	إذا (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		أرجع SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		إذا (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->پایه_2, reg->ترتیب,
				   reg->flags);
			أرجع SBI_EINVAL;
		}
	}

	/* Count memory regions */
	شمار = sbi_domain_استفاده_شده_memregions(dom);

	/* Check presence of firmware regions */
	إذا (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		أرجع SBI_EINVAL;
	}

	/* Sort the memory regions */
	لكل (i = 0; i < (شمار - 1); i++) {
		reg = &dom->regions[i];
		لكل (j = i + 1; j < شمار; j++) {
			reg1 = &dom->regions[j];

			إذا (!is_region_before(reg1, reg))
				استمر;

			جابجا_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	لكل (i = 0; i < (شمار - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		لكل (j = i + 1; j < شمار; j++) {
			reg1 = &dom->regions[j];

			إذا (is_region_compatible(reg, reg1)) {
				is_covered = true;
				اكسر;
			}
		}

		/* find a region is superset of reg, remove reg */
		إذا (is_covered) {
			sbi_memmove(reg, reg + 1, حجم(*reg) * (شمار - i));
			شمار--;
		} وإلا
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
	إذا (dom->بعدی_نما != PRV_S &&
	    dom->بعدی_نما != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->بعدی_نما);
		أرجع SBI_EINVAL;
	}

	/* Check next address and next mode */
	إذا (!sbi_domain_check_addr(dom, dom->بعدی_addr, dom->بعدی_نما,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->بعدی_addr);
		أرجع SBI_EINVAL;
	}

	أرجع 0;
}

bool sbi_domain_check_addr_range(ثابت بنية sbi_domain *dom,
				 بیعلامت دراز addr, بیعلامت دراز اندازه,
				 بیعلامت دراز نما,
				 بیعلامت دراز access_flags)
{
	بیعلامت دراز max = addr + اندازه;
	ثابت بنية sbi_domain_memregion *reg, *sreg;

	إذا (!dom)
		أرجع false;

	إذا (اندازه && max <= addr)
		أرجع false;

	طالما (addr < max) {
		reg = پیدا_کن_region(dom, addr);
		إذا (!reg)
			أرجع false;

		إذا (!sbi_domain_check_addr(dom, addr, نما, access_flags))
			أرجع false;

		sreg = پیدا_کن_بعدی_subset_region(dom, reg, addr);
		إذا (sreg)
			addr = sreg->پایه_2;
		وإلا إذا (reg->ترتیب < __riscv_xlen)
			addr = reg->پایه_2 + (1UL << reg->ترتیب);
		وإلا
			اكسر;
	}

	أرجع true;
}

فراغ sbi_domain_dump(ثابت بنية sbi_domain *dom, ثابت حرف *suffix)
{
	u32 i, j, k;
	بیعلامت دراز rstart, rend;
	بنية sbi_domain_memregion *reg;

	sbi_printf("Domain%d Name        %s: %s\n",
		   dom->شاخص_2, suffix, dom->name);

	sbi_printf("Domain%d Init Order  %s: 0x%x\n",
		   dom->شاخص_2, suffix, dom->init_ترتیب);

	sbi_printf("Domain%d Boot HART   %s: %d\n",
		   dom->شاخص_2, suffix, dom->boot_hartid);

	k = 0;
	sbi_printf("Domain%d HARTs       %s: ", dom->شاخص_2, suffix);
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		j = sbi_hartindex_مقصد_hartid(i);
		sbi_printf("%s0x%x%s", (k++) ? "," : "",
			   j, sbi_domain_is_assigned_hart(dom, i) ? "*" : "");
	}
	sbi_printf("\n");

	i = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->پایه_2;
		rend = (reg->ترتیب < __riscv_xlen) ?
			rstart + ((1UL << reg->ترتیب) - 1) : -1UL;

		sbi_printf("Domain%d Region%02d    %s: 0x%" PRILX "-0x%" PRILX " ",
			   dom->شاخص_2, i, suffix, rstart, rend);

		k = 0;

		sbi_printf("M: ");
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->شاخص_2, suffix, dom->بعدی_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->شاخص_2, suffix, dom->بعدی_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->شاخص_2, suffix);
	بدل (dom->بعدی_نما) {
	حالة PRV_M:
		sbi_printf("M-mode\n");
		اكسر;
	حالة PRV_S:
		sbi_printf("S-mode\n");
		اكسر;
	حالة PRV_U:
		sbi_printf("U-mode\n");
		اكسر;
	افتراضي:
		sbi_printf("Unknown\n");
		اكسر;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->شاخص_2, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->شاخص_2, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

فراغ sbi_domain_dump_all(ثابت حرف *suffix)
{
	ثابت بنية sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

صحيح sbi_domain_ثبات(بنية sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	بنية sbi_domain *tdom;
	صحيح rc;

	/* Sanity checks */
	إذا (!dom || domain_finalized)
		أرجع SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		إذا (tdom == dom ||
		    tdom->init_ترتیب == dom->init_ترتیب)
			أرجع SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	إذا (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		أرجع rc;
	}

	sbi_list_جمع_کن_دم(&dom->گره, &domain_list);

	/* Assign index to domain */
	dom->شاخص_2 = domain_شمار++;

	/* Initialize spinlock for dom->assigned_harts */
	SPIN_قفل_INIT(dom->assigned_harts_قفل);

	/* Clear assigned HARTs of domain */
	sbi_hartmask_clear_all(&dom->assigned_harts);

	/*
	 * Assign HART to a domain with the least initialization order
	 * where the HART is listed as a possible HART of the domain.
	 */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		tdom = sbi_hartindex_مقصد_domain(i);
		إذا (tdom) {
			إذا (tdom->init_ترتیب > dom->init_ترتیب)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			وإلا
				استمر;
		}

		sbi_update_hartindex_مقصد_domain(i, dom);
		sbi_hartmask_مجموعه_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		إذا (sbi_hartindex_مقصد_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->شاخص_2, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_راهاندازی_state(dom);
	إذا (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->گره);
		أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح ریشه_جمع_کن_memregion(ثابت بنية sbi_domain_memregion *reg)
{
	صحيح rc;
	bool reg_merged;
	بنية sbi_domain_memregion *nreg, *nreg1;
	صحيح ریشه_memregs_شمار = sbi_domain_استفاده_شده_memregions(&ریشه);

	/* Sanity checks */
	إذا (!reg || domain_finalized || !ریشه.regions ||
	    (ریشه_REGION_MAX <= ریشه_memregs_شمار))
		أرجع SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&ریشه, nreg) {
		إذا (is_region_compatible(reg, nreg))
			أرجع 0;
	}

	/* Append the memregion to root memregions */
	nreg = &ریشه.regions[ریشه_memregs_شمار];
	sbi_memcpy(nreg, reg, حجم(*reg));
	ریشه_memregs_شمار++;
	ریشه.regions[ریشه_memregs_شمار].ترتیب = 0;

	/* Sort and optimize root regions */
	افعل {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&ریشه);
		إذا (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   ریشه.name, rc);
			أرجع rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&ریشه, nreg) {
			nreg1 = nreg + 1;
			إذا (!nreg1->ترتیب)
				استمر;

			إذا (!(nreg->پایه_2 & (بیت(nreg->ترتیب + 1) - 1)) &&
			    (nreg->پایه_2 + بیت(nreg->ترتیب)) == nreg1->پایه_2 &&
			    nreg->ترتیب == nreg1->ترتیب &&
			    nreg->flags == nreg1->flags) {
				صحيح i1 = nreg1 - ریشه.regions;
				nreg->ترتیب++;
				sbi_memmove(nreg1, nreg1 + 1,
					    حجم(*nreg1) * (ریشه_memregs_شمار - i1));
				reg_merged = true;
				ریشه_memregs_شمار--;
			}
		}
	} طالما (reg_merged);

	أرجع 0;
}

صحيح sbi_domain_ریشه_جمع_کن_memrange(بیعلامت دراز addr, بیعلامت دراز اندازه,
			   بیعلامت دراز align, بیعلامت دراز region_flags)
{
	صحيح rc;
	بیعلامت دراز pos, انتها, rsize;
	بنية sbi_domain_memregion reg;

	pos = addr;
	انتها = addr + اندازه;
	طالما (pos < انتها) {
		rsize = pos & (align - 1);
		إذا (rsize)
			rsize = 1UL << sbi_ffs(pos);
		وإلا
			rsize = ((انتها - pos) < align) ?
				(انتها - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = ریشه_جمع_کن_memregion(&reg);
		إذا (rc)
			أرجع rc;
		pos += rsize;
	}

	أرجع 0;
}

صحيح sbi_domain_startup(بنية sbi_scratch *scratch, u32 cold_hartid)
{
	صحيح rc;
	u32 dhart;
	بنية sbi_domain *dom;

	/* Sanity checks */
	إذا (!domain_finalized)
		أرجع SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_مقصد_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		إذا (!sbi_hartindex_valid(dhart))
			استمر;

		/* Ignore if boot HART not possible for this domain */
		إذا (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			استمر;

		/* Ignore if boot HART assigned different domain */
		إذا (sbi_hartindex_مقصد_domain(dhart) != dom)
			استمر;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_قفل(&dom->assigned_harts_قفل);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_قفل);
		إذا (!rc)
			استمر;

		/* Startup boot HART of domain */
		إذا (dom->boot_hartid == cold_hartid) {
			scratch->بعدی_addr = dom->بعدی_addr;
			scratch->بعدی_نما = dom->بعدی_نما;
			scratch->بعدی_arg1 = dom->بعدی_arg1;
		} وإلا {
			rc = sbi_hsm_hart_شروع(scratch, عدم,
						dom->boot_hartid,
						dom->بعدی_addr,
						dom->بعدی_نما,
						dom->بعدی_arg1);
			إذا (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				أرجع rc;
			}
		}
	}

	أرجع 0;
}

صحيح sbi_domain_finalize(بنية sbi_scratch *scratch)
{
	صحيح rc;
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	إذا (domain_finalized)
		أرجع SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	إذا (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		أرجع rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	أرجع 0;
}

صحيح sbi_domain_init(بنية sbi_scratch *scratch, u32 cold_hartid)
{
	صحيح rc;
	بنية sbi_hartmask *ریشه_hmask;
	بنية sbi_domain_memregion *ریشه_memregs;
	صحيح ریشه_memregs_شمار = 0;
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_سر(&domain_list);

	إذا (!fw_single_region) {
		إذا (scratch->fw_rw_جابه_جایی == 0 ||
		    (scratch->fw_rw_جابه_جایی & (scratch->fw_rw_جابه_جایی - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_جابه_جایی);
			أرجع SBI_EINVAL;
		}
		إذا ((scratch->fw_شروع & (scratch->fw_rw_جابه_جایی - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			أرجع SBI_EINVAL;
		}
	}

	domain_hart_ptr_جابه_جایی = sbi_scratch_alloc_type_جابه_جایی(فراغ *);
	إذا (!domain_hart_ptr_جابه_جایی)
		أرجع SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	إذا (rc)
		اذهب fail_free_domain_hart_ptr_جابه_جایی;

	ریشه_memregs = sbi_calloc(حجم(*ریشه_memregs), ریشه_REGION_MAX + 1);
	إذا (!ریشه_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		اذهب fail_deinit_context;
	}
	ریشه.regions = ریشه_memregs;

	ریشه_hmask = sbi_zalloc(حجم(*ریشه_hmask));
	إذا (!ریشه_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		اذهب fail_free_ریشه_memregs;
	}
	ریشه.possible_harts = ریشه_hmask;

	/* Root domain firmware memory region */
	إذا (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_شروع, scratch->fw_اندازه,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ریشه_memregs[ریشه_memregs_شمار++]);
	} وإلا {
		sbi_domain_memregion_init(scratch->fw_شروع,
					  scratch->fw_rw_جابه_جایی,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ریشه_memregs[ریشه_memregs_شمار++]);

		sbi_domain_memregion_init((scratch->fw_شروع +
					   scratch->fw_rw_جابه_جایی),
					  (scratch->fw_اندازه -
					   scratch->fw_rw_جابه_جایی),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ریشه_memregs[ریشه_memregs_شمار++]);
	}

	ریشه.fw_region_inited = true;

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
				  &ریشه_memregs[ریشه_memregs_شمار++]);

	/* Root domain memory region end */
	ریشه_memregs[ریشه_memregs_شمار].ترتیب = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	ریشه.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	ریشه.بعدی_arg1 = scratch->بعدی_arg1;
	ریشه.بعدی_addr = scratch->بعدی_addr;
	ریشه.بعدی_نما = scratch->بعدی_نما;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_مجموعه_hartindex(i, ریشه_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_ثبات(&ریشه);
	إذا (rc)
		اذهب fail_free_ریشه_hmask;

	أرجع 0;

fail_free_ریشه_hmask:
	sbi_free(ریشه_hmask);
fail_free_ریشه_memregs:
	sbi_free(ریشه_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_جابه_جایی:
	sbi_scratch_free_جابه_جایی(domain_hart_ptr_جابه_جایی);
	أرجع rc;
}
