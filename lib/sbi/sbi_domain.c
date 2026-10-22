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

ساكن u32 domain_ڳڻپ = 0;
ساكن bool domain_finalized = false;

#وصف_ڪريو پاڙ_REGION_MAX	32

بنية sbi_domain پاڙ = {
	.name = "root",
	.init_ترتيب = -1U,
	.possible_harts = عدم,
	.regions = عدم,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

ساكن بنانشان ڊگهو domain_hart_ptr_وٿي;

بنية sbi_domain *sbi_hartindex_منزل_domain(u32 hartindex)
{
	بنية sbi_scratch *scratch;

	scratch = sbi_hartindex_منزل_scratch(hartindex);
	إذا (!scratch || !domain_hart_ptr_وٿي)
		أرجع عدم;

	أرجع sbi_scratch_read_type(scratch, فراغ *, domain_hart_ptr_وٿي);
}

فراغ sbi_update_hartindex_منزل_domain(u32 hartindex, بنية sbi_domain *dom)
{
	بنية sbi_scratch *scratch;

	scratch = sbi_hartindex_منزل_scratch(hartindex);
	إذا (!scratch)
		أرجع;

	sbi_scratch_write_type(scratch, فراغ *, domain_hart_ptr_وٿي, dom);
}

bool sbi_domain_is_assigned_hart(ثابت بنية sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_تالو)
{
	bool ret;
	بنية sbi_domain *tdom = (بنية sbi_domain *)dom;

	إذا (!dom)
		أرجع false;

	spin_تالو(&tdom->assigned_harts_تالو);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_تالو);

	أرجع ret;
}

صحيح sbi_domain_get_assigned_hartmask(ثابت بنية sbi_domain *dom,
				     بنية sbi_hartmask *نقاب)
	MUST_NOT_HOLD(&dom->assigned_harts_تالو)
{
	ulong ret = 0;
	بنية sbi_domain *tdom = (بنية sbi_domain *)dom;

	إذا (!dom) {
		sbi_hartmask_clear_all(نقاب);
		أرجع 0;
	}

	spin_تالو(&tdom->assigned_harts_تالو);
	sbi_hartmask_copy(نقاب, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_تالو);

	أرجع ret;
}

فراغ sbi_domain_memregion_init(بنانشان ڊگهو addr,
				بنانشان ڊگهو سائيز,
				بنانشان ڊگهو flags,
				بنية sbi_domain_memregion *reg)
{
	بنانشان ڊگهو بنياد_2 = 0, ترتيب;

	لكل (ترتيب = log2roundup(سائيز) ; ترتيب <= __riscv_xlen; ترتيب++) {
		إذا (ترتيب < __riscv_xlen) {
			بنياد_2 = addr & ~((1UL << ترتيب) - 1UL);
			إذا ((بنياد_2 <= addr) &&
			    (addr < (بنياد_2 + (1UL << ترتيب))) &&
			    (بنياد_2 <= (addr + سائيز - 1UL)) &&
			    ((addr + سائيز - 1UL) < (بنياد_2 + (1UL << ترتيب))))
				اكسر;
		} وإلا {
			بنياد_2 = 0;
			اكسر;
		}

	}

	إذا (reg) {
		reg->بنياد_2 = بنياد_2;
		reg->ترتيب = ترتيب;
		reg->flags = flags;
	}
}

بنانشان صحيح sbi_domain_get_oldpmp_flags(بنية sbi_domain_memregion *reg)
{

	بنانشان صحيح pmp_flags = 0;

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

بنانشان صحيح sbi_domain_get_smepmp_flags(بنية sbi_domain_memregion *reg)
{
	بنانشان صحيح pmp_flags = 0;
	بنانشان ڊگهو rstart, rend;

	إذا ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_نقاب) == 0) {
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
		إذا ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_نقاب)
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
		rstart = reg->بنياد_2;
		rend = (reg->ترتيب < __riscv_xlen) ? rstart + ((1UL << reg->ترتيب) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	أرجع pmp_flags;
}

bool sbi_domain_check_addr(ثابت بنية sbi_domain *dom,
			   بنانشان ڊگهو addr, بنانشان ڊگهو گهڻائي,
			   بنانشان ڊگهو access_flags)
{
	bool rmmio, mmio = false;
	بنية sbi_domain_memregion *reg;
	بنانشان ڊگهو rstart, rend, rflags, rwx = 0, rrwx = 0;

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
		rrwx = (گهڻائي == PRV_M ?
			(rflags & SBI_DOMAIN_MEMREGION_M_ACCESS_نقاب) :
			(rflags & SBI_DOMAIN_MEMREGION_SU_ACCESS_نقاب)
			>> SBI_DOMAIN_MEMREGION_SU_ACCESS_سرڪايو);

		rstart = reg->بنياد_2;
		rend = (reg->ترتيب < __riscv_xlen) ?
			rstart + ((1UL << reg->ترتيب) - 1) : -1UL;
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

	أرجع (گهڻائي == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
ساكن bool is_region_valid(ثابت بنية sbi_domain_memregion *reg)
{
	إذا (reg->ترتيب < 3 || __riscv_xlen < reg->ترتيب)
		أرجع false;

	إذا (reg->ترتيب == __riscv_xlen && reg->بنياد_2 != 0)
		أرجع false;

	إذا (reg->ترتيب < __riscv_xlen && (reg->بنياد_2 & (ٻٽو_انگ(reg->ترتيب) - 1)))
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

	إذا (regA->ترتيب < regB->ترتيب)
		أرجع true;

	إذا ((regA->ترتيب == regB->ترتيب) &&
	    (regA->بنياد_2 < regB->بنياد_2))
		أرجع true;

	أرجع false;
}

ساكن ثابت بنية sbi_domain_memregion *ڳوليو_region(
						ثابت بنية sbi_domain *dom,
						بنانشان ڊگهو addr)
{
	بنانشان ڊگهو rstart, rend;
	بنية sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->بنياد_2;
		rend = (reg->ترتيب < __riscv_xlen) ?
			rstart + ((1UL << reg->ترتيب) - 1) : -1UL;
		إذا (rstart <= addr && addr <= rend)
			أرجع reg;
	}

	أرجع عدم;
}

ساكن ثابت بنية sbi_domain_memregion *ڳوليو_ايندڙ_subset_region(
				ثابت بنية sbi_domain *dom,
				ثابت بنية sbi_domain_memregion *reg,
				بنانشان ڊگهو addr)
{
	بنية sbi_domain_memregion *sreg, *ret = عدم;

	sbi_domain_for_each_memregion(dom, sreg) {
		إذا (sreg == reg || (sreg->بنياد_2 <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			استمر;

		إذا (!ret || (sreg->بنياد_2 < ret->بنياد_2) ||
		    ((sreg->بنياد_2 == ret->بنياد_2) && (sreg->ترتيب < ret->ترتيب)))
			ret = sreg;
	}

	أرجع ret;
}

ساكن فراغ مٽا_سٽا_region(بنية sbi_domain_memregion* reg1,
			بنية sbi_domain_memregion* reg2)
{
	بنية sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, حجم(treg));
	sbi_memcpy(reg1, reg2, حجم(treg));
	sbi_memcpy(reg2, &treg, حجم(treg));
}

ساكن صحيح sbi_domain_استعمال_ٿيل_memregions(ثابت بنية sbi_domain *dom)
{
	صحيح ڳڻپ = 0;
	بنية sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		ڳڻپ++;

	أرجع ڳڻپ;
}

ساكن صحيح sanitize_domain(بنية sbi_domain *dom)
{
	u32 i, j, ڳڻپ;
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
				   dom->name, sbi_hartindex_منزل_hartid(i));
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
				   dom->name, reg->بنياد_2, reg->ترتيب,
				   reg->flags);
			أرجع SBI_EINVAL;
		}
	}

	/* Count memory regions */
	ڳڻپ = sbi_domain_استعمال_ٿيل_memregions(dom);

	/* Check presence of firmware regions */
	إذا (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		أرجع SBI_EINVAL;
	}

	/* Sort the memory regions */
	لكل (i = 0; i < (ڳڻپ - 1); i++) {
		reg = &dom->regions[i];
		لكل (j = i + 1; j < ڳڻپ; j++) {
			reg1 = &dom->regions[j];

			إذا (!is_region_before(reg1, reg))
				استمر;

			مٽا_سٽا_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	لكل (i = 0; i < (ڳڻپ - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		لكل (j = i + 1; j < ڳڻپ; j++) {
			reg1 = &dom->regions[j];

			إذا (is_region_compatible(reg, reg1)) {
				is_covered = true;
				اكسر;
			}
		}

		/* find a region is superset of reg, remove reg */
		إذا (is_covered) {
			sbi_memmove(reg, reg + 1, حجم(*reg) * (ڳڻپ - i));
			ڳڻپ--;
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
	إذا (dom->ايندڙ_گهڻائي != PRV_S &&
	    dom->ايندڙ_گهڻائي != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->ايندڙ_گهڻائي);
		أرجع SBI_EINVAL;
	}

	/* Check next address and next mode */
	إذا (!sbi_domain_check_addr(dom, dom->ايندڙ_addr, dom->ايندڙ_گهڻائي,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->ايندڙ_addr);
		أرجع SBI_EINVAL;
	}

	أرجع 0;
}

bool sbi_domain_check_addr_range(ثابت بنية sbi_domain *dom,
				 بنانشان ڊگهو addr, بنانشان ڊگهو سائيز,
				 بنانشان ڊگهو گهڻائي,
				 بنانشان ڊگهو access_flags)
{
	بنانشان ڊگهو max = addr + سائيز;
	ثابت بنية sbi_domain_memregion *reg, *sreg;

	إذا (!dom)
		أرجع false;

	إذا (سائيز && max <= addr)
		أرجع false;

	طالما (addr < max) {
		reg = ڳوليو_region(dom, addr);
		إذا (!reg)
			أرجع false;

		إذا (!sbi_domain_check_addr(dom, addr, گهڻائي, access_flags))
			أرجع false;

		sreg = ڳوليو_ايندڙ_subset_region(dom, reg, addr);
		إذا (sreg)
			addr = sreg->بنياد_2;
		وإلا إذا (reg->ترتيب < __riscv_xlen)
			addr = reg->بنياد_2 + (1UL << reg->ترتيب);
		وإلا
			اكسر;
	}

	أرجع true;
}

فراغ sbi_domain_dump(ثابت بنية sbi_domain *dom, ثابت حرف *suffix)
{
	u32 i, j, k;
	بنانشان ڊگهو rstart, rend;
	بنية sbi_domain_memregion *reg;

	sbi_printf("Domain%d Name        %s: %s\n",
		   dom->اشاريو_2, suffix, dom->name);

	sbi_printf("Domain%d Init Order  %s: 0x%x\n",
		   dom->اشاريو_2, suffix, dom->init_ترتيب);

	sbi_printf("Domain%d Boot HART   %s: %d\n",
		   dom->اشاريو_2, suffix, dom->boot_hartid);

	k = 0;
	sbi_printf("Domain%d HARTs       %s: ", dom->اشاريو_2, suffix);
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		j = sbi_hartindex_منزل_hartid(i);
		sbi_printf("%s0x%x%s", (k++) ? "," : "",
			   j, sbi_domain_is_assigned_hart(dom, i) ? "*" : "");
	}
	sbi_printf("\n");

	i = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->بنياد_2;
		rend = (reg->ترتيب < __riscv_xlen) ?
			rstart + ((1UL << reg->ترتيب) - 1) : -1UL;

		sbi_printf("Domain%d Region%02d    %s: 0x%" PRILX "-0x%" PRILX " ",
			   dom->اشاريو_2, i, suffix, rstart, rend);

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
		   dom->اشاريو_2, suffix, dom->ايندڙ_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->اشاريو_2, suffix, dom->ايندڙ_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->اشاريو_2, suffix);
	بدل (dom->ايندڙ_گهڻائي) {
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
		   dom->اشاريو_2, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->اشاريو_2, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

فراغ sbi_domain_dump_all(ثابت حرف *suffix)
{
	ثابت بنية sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

صحيح sbi_domain_رجسٽر(بنية sbi_domain *dom)
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
		    tdom->init_ترتيب == dom->init_ترتيب)
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

	sbi_list_جوڙيو_پڇ(&dom->ڳنڍ, &domain_list);

	/* Assign index to domain */
	dom->اشاريو_2 = domain_ڳڻپ++;

	/* Initialize spinlock for dom->assigned_harts */
	SPIN_تالو_INIT(dom->assigned_harts_تالو);

	/* Clear assigned HARTs of domain */
	sbi_hartmask_clear_all(&dom->assigned_harts);

	/*
	 * Assign HART to a domain with the least initialization order
	 * where the HART is listed as a possible HART of the domain.
	 */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		tdom = sbi_hartindex_منزل_domain(i);
		إذا (tdom) {
			إذا (tdom->init_ترتيب > dom->init_ترتيب)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			وإلا
				استمر;
		}

		sbi_update_hartindex_منزل_domain(i, dom);
		sbi_hartmask_سيٽ_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		إذا (sbi_hartindex_منزل_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->اشاريو_2, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_تياري_state(dom);
	إذا (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->ڳنڍ);
		أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح پاڙ_جوڙيو_memregion(ثابت بنية sbi_domain_memregion *reg)
{
	صحيح rc;
	bool reg_merged;
	بنية sbi_domain_memregion *nreg, *nreg1;
	صحيح پاڙ_memregs_ڳڻپ = sbi_domain_استعمال_ٿيل_memregions(&پاڙ);

	/* Sanity checks */
	إذا (!reg || domain_finalized || !پاڙ.regions ||
	    (پاڙ_REGION_MAX <= پاڙ_memregs_ڳڻپ))
		أرجع SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&پاڙ, nreg) {
		إذا (is_region_compatible(reg, nreg))
			أرجع 0;
	}

	/* Append the memregion to root memregions */
	nreg = &پاڙ.regions[پاڙ_memregs_ڳڻپ];
	sbi_memcpy(nreg, reg, حجم(*reg));
	پاڙ_memregs_ڳڻپ++;
	پاڙ.regions[پاڙ_memregs_ڳڻپ].ترتيب = 0;

	/* Sort and optimize root regions */
	افعل {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&پاڙ);
		إذا (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   پاڙ.name, rc);
			أرجع rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&پاڙ, nreg) {
			nreg1 = nreg + 1;
			إذا (!nreg1->ترتيب)
				استمر;

			إذا (!(nreg->بنياد_2 & (ٻٽو_انگ(nreg->ترتيب + 1) - 1)) &&
			    (nreg->بنياد_2 + ٻٽو_انگ(nreg->ترتيب)) == nreg1->بنياد_2 &&
			    nreg->ترتيب == nreg1->ترتيب &&
			    nreg->flags == nreg1->flags) {
				صحيح i1 = nreg1 - پاڙ.regions;
				nreg->ترتيب++;
				sbi_memmove(nreg1, nreg1 + 1,
					    حجم(*nreg1) * (پاڙ_memregs_ڳڻپ - i1));
				reg_merged = true;
				پاڙ_memregs_ڳڻپ--;
			}
		}
	} طالما (reg_merged);

	أرجع 0;
}

صحيح sbi_domain_پاڙ_جوڙيو_memrange(بنانشان ڊگهو addr, بنانشان ڊگهو سائيز,
			   بنانشان ڊگهو align, بنانشان ڊگهو region_flags)
{
	صحيح rc;
	بنانشان ڊگهو pos, پڇاڙي, rsize;
	بنية sbi_domain_memregion reg;

	pos = addr;
	پڇاڙي = addr + سائيز;
	طالما (pos < پڇاڙي) {
		rsize = pos & (align - 1);
		إذا (rsize)
			rsize = 1UL << sbi_ffs(pos);
		وإلا
			rsize = ((پڇاڙي - pos) < align) ?
				(پڇاڙي - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = پاڙ_جوڙيو_memregion(&reg);
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
		dhart = sbi_hartid_منزل_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		إذا (!sbi_hartindex_valid(dhart))
			استمر;

		/* Ignore if boot HART not possible for this domain */
		إذا (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			استمر;

		/* Ignore if boot HART assigned different domain */
		إذا (sbi_hartindex_منزل_domain(dhart) != dom)
			استمر;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_تالو(&dom->assigned_harts_تالو);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_تالو);
		إذا (!rc)
			استمر;

		/* Startup boot HART of domain */
		إذا (dom->boot_hartid == cold_hartid) {
			scratch->ايندڙ_addr = dom->ايندڙ_addr;
			scratch->ايندڙ_گهڻائي = dom->ايندڙ_گهڻائي;
			scratch->ايندڙ_arg1 = dom->ايندڙ_arg1;
		} وإلا {
			rc = sbi_hsm_hart_شروعات(scratch, عدم,
						dom->boot_hartid,
						dom->ايندڙ_addr,
						dom->ايندڙ_گهڻائي,
						dom->ايندڙ_arg1);
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
	بنية sbi_hartmask *پاڙ_hmask;
	بنية sbi_domain_memregion *پاڙ_memregs;
	صحيح پاڙ_memregs_ڳڻپ = 0;
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_سر(&domain_list);

	إذا (!fw_single_region) {
		إذا (scratch->fw_rw_وٿي == 0 ||
		    (scratch->fw_rw_وٿي & (scratch->fw_rw_وٿي - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_وٿي);
			أرجع SBI_EINVAL;
		}
		إذا ((scratch->fw_شروعات & (scratch->fw_rw_وٿي - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			أرجع SBI_EINVAL;
		}
	}

	domain_hart_ptr_وٿي = sbi_scratch_alloc_type_وٿي(فراغ *);
	إذا (!domain_hart_ptr_وٿي)
		أرجع SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	إذا (rc)
		اذهب fail_free_domain_hart_ptr_وٿي;

	پاڙ_memregs = sbi_calloc(حجم(*پاڙ_memregs), پاڙ_REGION_MAX + 1);
	إذا (!پاڙ_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		اذهب fail_deinit_context;
	}
	پاڙ.regions = پاڙ_memregs;

	پاڙ_hmask = sbi_zalloc(حجم(*پاڙ_hmask));
	إذا (!پاڙ_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		اذهب fail_free_پاڙ_memregs;
	}
	پاڙ.possible_harts = پاڙ_hmask;

	/* Root domain firmware memory region */
	إذا (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_شروعات, scratch->fw_سائيز,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &پاڙ_memregs[پاڙ_memregs_ڳڻپ++]);
	} وإلا {
		sbi_domain_memregion_init(scratch->fw_شروعات,
					  scratch->fw_rw_وٿي,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &پاڙ_memregs[پاڙ_memregs_ڳڻپ++]);

		sbi_domain_memregion_init((scratch->fw_شروعات +
					   scratch->fw_rw_وٿي),
					  (scratch->fw_سائيز -
					   scratch->fw_rw_وٿي),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &پاڙ_memregs[پاڙ_memregs_ڳڻپ++]);
	}

	پاڙ.fw_region_inited = true;

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
				  &پاڙ_memregs[پاڙ_memregs_ڳڻپ++]);

	/* Root domain memory region end */
	پاڙ_memregs[پاڙ_memregs_ڳڻپ].ترتيب = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	پاڙ.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	پاڙ.ايندڙ_arg1 = scratch->ايندڙ_arg1;
	پاڙ.ايندڙ_addr = scratch->ايندڙ_addr;
	پاڙ.ايندڙ_گهڻائي = scratch->ايندڙ_گهڻائي;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_سيٽ_hartindex(i, پاڙ_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_رجسٽر(&پاڙ);
	إذا (rc)
		اذهب fail_free_پاڙ_hmask;

	أرجع 0;

fail_free_پاڙ_hmask:
	sbi_free(پاڙ_hmask);
fail_free_پاڙ_memregs:
	sbi_free(پاڙ_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_وٿي:
	sbi_scratch_free_وٿي(domain_hart_ptr_وٿي);
	أرجع rc;
}
