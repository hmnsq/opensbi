/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_hartmask.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_hsm.h>
#অন্তর্ভাব <sbi/sbi_list.h>
#অন্তর্ভাব <sbi/sbi_math.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_string.h>

SBI_LIST_শীর্ষ(domain_list);

স্থির u32 domain_গুনতি = 0;
স্থির bool domain_finalized = false;

#define মূল_REGION_MAX	32

কাঠামো sbi_domain মূল = {
	.name = "root",
	.init_order = -1U,
	.possible_harts = ফাঁকা,
	.regions = ফাঁকা,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

স্থির unsigned long domain_hart_ptr_offset;

কাঠামো sbi_domain *sbi_hartindex_to_domain(u32 hartindex)
{
	কাঠামো sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!scratch || !domain_hart_ptr_offset)
		ফেরত ফাঁকা;

	ফেরত sbi_scratch_read_type(scratch, শূন্য *, domain_hart_ptr_offset);
}

শূন্য sbi_update_hartindex_to_domain(u32 hartindex, কাঠামো sbi_domain *dom)
{
	কাঠামো sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!scratch)
		ফেরত;

	sbi_scratch_write_type(scratch, শূন্য *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(ধ্রুবক কাঠামো sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	কাঠামো sbi_domain *tdom = (কাঠামো sbi_domain *)dom;

	যদি (!dom)
		ফেরত false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	ফেরত ret;
}

পূর্ণসংখ্যা sbi_domain_get_assigned_hartmask(ধ্রুবক কাঠামো sbi_domain *dom,
				     কাঠামো sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	কাঠামো sbi_domain *tdom = (কাঠামো sbi_domain *)dom;

	যদি (!dom) {
		sbi_hartmask_clear_all(mask);
		ফেরত 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	ফেরত ret;
}

শূন্য sbi_domain_memregion_init(unsigned long addr,
				unsigned long আকার_2,
				unsigned long flags,
				কাঠামো sbi_domain_memregion *reg)
{
	unsigned long base = 0, order;

	জন্য (order = log2roundup(আকার_2) ; order <= __riscv_xlen; order++) {
		যদি (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			যদি ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + আকার_2 - 1UL)) &&
			    ((addr + আকার_2 - 1UL) < (base + (1UL << order))))
				বিরতি;
		} অন্যথা {
			base = 0;
			বিরতি;
		}

	}

	যদি (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

unsigned পূর্ণসংখ্যা sbi_domain_get_oldpmp_flags(কাঠামো sbi_domain_memregion *reg)
{

	unsigned পূর্ণসংখ্যা pmp_flags = 0;

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

	ফেরত pmp_flags;
}

unsigned পূর্ণসংখ্যা sbi_domain_get_smepmp_flags(কাঠামো sbi_domain_memregion *reg)
{
	unsigned পূর্ণসংখ্যা pmp_flags = 0;
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
		ফেরত 0;
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
			ফেরত 0;
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

	ফেরত pmp_flags;
}

bool sbi_domain_check_addr(ধ্রুবক কাঠামো sbi_domain *dom,
			   unsigned long addr, unsigned long mode,
			   unsigned long access_flags)
{
	bool rmmio, mmio = false;
	কাঠামো sbi_domain_memregion *reg;
	unsigned long rstart, rend, rflags, rwx = 0, rrwx = 0;

	যদি (!dom)
		ফেরত false;

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
				ফেরত false;
			ফেরত ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	ফেরত (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
স্থির bool is_region_valid(ধ্রুবক কাঠামো sbi_domain_memregion *reg)
{
	যদি (reg->order < 3 || __riscv_xlen < reg->order)
		ফেরত false;

	যদি (reg->order == __riscv_xlen && reg->base != 0)
		ফেরত false;

	যদি (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		ফেরত false;

	ফেরত true;
}

/** Check if regionA can be replaced by regionB */
স্থির bool is_region_compatible(ধ্রুবক কাঠামো sbi_domain_memregion *regA,
				 ধ্রুবক কাঠামো sbi_domain_memregion *regB)
{
	যদি (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		ফেরত true;

	ফেরত false;
}

/** Check if regionA should be placed before regionB */
স্থির bool is_region_before(ধ্রুবক কাঠামো sbi_domain_memregion *regA,
			     ধ্রুবক কাঠামো sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	যদি (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		ফেরত true;
	যদি (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		ফেরত false;

	যদি (regA->order < regB->order)
		ফেরত true;

	যদি ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		ফেরত true;

	ফেরত false;
}

স্থির ধ্রুবক কাঠামো sbi_domain_memregion *find_region(
						ধ্রুবক কাঠামো sbi_domain *dom,
						unsigned long addr)
{
	unsigned long rstart, rend;
	কাঠামো sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		যদি (rstart <= addr && addr <= rend)
			ফেরত reg;
	}

	ফেরত ফাঁকা;
}

স্থির ধ্রুবক কাঠামো sbi_domain_memregion *find_পরবর্তী_subset_region(
				ধ্রুবক কাঠামো sbi_domain *dom,
				ধ্রুবক কাঠামো sbi_domain_memregion *reg,
				unsigned long addr)
{
	কাঠামো sbi_domain_memregion *sreg, *ret = ফাঁকা;

	sbi_domain_for_each_memregion(dom, sreg) {
		যদি (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			চালু;

		যদি (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	ফেরত ret;
}

স্থির শূন্য swap_region(কাঠামো sbi_domain_memregion* reg1,
			কাঠামো sbi_domain_memregion* reg2)
{
	কাঠামো sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, আকার(treg));
	sbi_memcpy(reg1, reg2, আকার(treg));
	sbi_memcpy(reg2, &treg, আকার(treg));
}

স্থির পূর্ণসংখ্যা sbi_domain_used_memregions(ধ্রুবক কাঠামো sbi_domain *dom)
{
	পূর্ণসংখ্যা গুনতি = 0;
	কাঠামো sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		গুনতি++;

	ফেরত গুনতি;
}

স্থির পূর্ণসংখ্যা sanitize_domain(কাঠামো sbi_domain *dom)
{
	u32 i, j, গুনতি;
	bool is_covered;
	কাঠামো sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	যদি (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		ফেরত SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		যদি (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			ফেরত SBI_EINVAL;
		}
	}

	/* Check memory regions */
	যদি (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		ফেরত SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		যদি (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			ফেরত SBI_EINVAL;
		}
	}

	/* Count memory regions */
	গুনতি = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	যদি (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		ফেরত SBI_EINVAL;
	}

	/* Sort the memory regions */
	জন্য (i = 0; i < (গুনতি - 1); i++) {
		reg = &dom->regions[i];
		জন্য (j = i + 1; j < গুনতি; j++) {
			reg1 = &dom->regions[j];

			যদি (!is_region_before(reg1, reg))
				চালু;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	জন্য (i = 0; i < (গুনতি - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		জন্য (j = i + 1; j < গুনতি; j++) {
			reg1 = &dom->regions[j];

			যদি (is_region_compatible(reg, reg1)) {
				is_covered = true;
				বিরতি;
			}
		}

		/* find a region is superset of reg, remove reg */
		যদি (is_covered) {
			sbi_memmove(reg, reg + 1, আকার(*reg) * (গুনতি - i));
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
	যদি (dom->পরবর্তী_mode != PRV_S &&
	    dom->পরবর্তী_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->পরবর্তী_mode);
		ফেরত SBI_EINVAL;
	}

	/* Check next address and next mode */
	যদি (!sbi_domain_check_addr(dom, dom->পরবর্তী_addr, dom->পরবর্তী_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->পরবর্তী_addr);
		ফেরত SBI_EINVAL;
	}

	ফেরত 0;
}

bool sbi_domain_check_addr_range(ধ্রুবক কাঠামো sbi_domain *dom,
				 unsigned long addr, unsigned long আকার_2,
				 unsigned long mode,
				 unsigned long access_flags)
{
	unsigned long max = addr + আকার_2;
	ধ্রুবক কাঠামো sbi_domain_memregion *reg, *sreg;

	যদি (!dom)
		ফেরত false;

	যদি (আকার_2 && max <= addr)
		ফেরত false;

	যখন (addr < max) {
		reg = find_region(dom, addr);
		যদি (!reg)
			ফেরত false;

		যদি (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			ফেরত false;

		sreg = find_পরবর্তী_subset_region(dom, reg, addr);
		যদি (sreg)
			addr = sreg->base;
		অন্যথা যদি (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		অন্যথা
			বিরতি;
	}

	ফেরত true;
}

শূন্য sbi_domain_dump(ধ্রুবক কাঠামো sbi_domain *dom, ধ্রুবক অক্ষর *suffix)
{
	u32 i, j, k;
	unsigned long rstart, rend;
	কাঠামো sbi_domain_memregion *reg;

	sbi_printf("Domain%d Name        %s: %s\n",
		   dom->সূচক_2, suffix, dom->name);

	sbi_printf("Domain%d Init Order  %s: 0x%x\n",
		   dom->সূচক_2, suffix, dom->init_order);

	sbi_printf("Domain%d Boot HART   %s: %d\n",
		   dom->সূচক_2, suffix, dom->boot_hartid);

	k = 0;
	sbi_printf("Domain%d HARTs       %s: ", dom->সূচক_2, suffix);
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
			   dom->সূচক_2, i, suffix, rstart, rend);

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
		   dom->সূচক_2, suffix, dom->পরবর্তী_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->সূচক_2, suffix, dom->পরবর্তী_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->সূচক_2, suffix);
	নির্বাচন (dom->পরবর্তী_mode) {
	ক্ষেত্র PRV_M:
		sbi_printf("M-mode\n");
		বিরতি;
	ক্ষেত্র PRV_S:
		sbi_printf("S-mode\n");
		বিরতি;
	ক্ষেত্র PRV_U:
		sbi_printf("U-mode\n");
		বিরতি;
	পূর্বনির্ধারিত:
		sbi_printf("Unknown\n");
		বিরতি;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->সূচক_2, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->সূচক_2, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

শূন্য sbi_domain_dump_all(ধ্রুবক অক্ষর *suffix)
{
	ধ্রুবক কাঠামো sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

পূর্ণসংখ্যা sbi_domain_register(কাঠামো sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	কাঠামো sbi_domain *tdom;
	পূর্ণসংখ্যা rc;

	/* Sanity checks */
	যদি (!dom || domain_finalized)
		ফেরত SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		যদি (tdom == dom ||
		    tdom->init_order == dom->init_order)
			ফেরত SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	যদি (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		ফেরত rc;
	}

	sbi_list_add_লেজ(&dom->গ্রন্থি, &domain_list);

	/* Assign index to domain */
	dom->সূচক_2 = domain_গুনতি++;

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
				চালু;
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
				   " %d\n", dom->সূচক_2, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	যদি (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->গ্রন্থি);
		ফেরত rc;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা মূল_add_memregion(ধ্রুবক কাঠামো sbi_domain_memregion *reg)
{
	পূর্ণসংখ্যা rc;
	bool reg_merged;
	কাঠামো sbi_domain_memregion *nreg, *nreg1;
	পূর্ণসংখ্যা মূল_memregs_গুনতি = sbi_domain_used_memregions(&মূল);

	/* Sanity checks */
	যদি (!reg || domain_finalized || !মূল.regions ||
	    (মূল_REGION_MAX <= মূল_memregs_গুনতি))
		ফেরত SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&মূল, nreg) {
		যদি (is_region_compatible(reg, nreg))
			ফেরত 0;
	}

	/* Append the memregion to root memregions */
	nreg = &মূল.regions[মূল_memregs_গুনতি];
	sbi_memcpy(nreg, reg, আকার(*reg));
	মূল_memregs_গুনতি++;
	মূল.regions[মূল_memregs_গুনতি].order = 0;

	/* Sort and optimize root regions */
	করো {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&মূল);
		যদি (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   মূল.name, rc);
			ফেরত rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&মূল, nreg) {
			nreg1 = nreg + 1;
			যদি (!nreg1->order)
				চালু;

			যদি (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				পূর্ণসংখ্যা i1 = nreg1 - মূল.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    আকার(*nreg1) * (মূল_memregs_গুনতি - i1));
				reg_merged = true;
				মূল_memregs_গুনতি--;
			}
		}
	} যখন (reg_merged);

	ফেরত 0;
}

পূর্ণসংখ্যা sbi_domain_মূল_add_memrange(unsigned long addr, unsigned long আকার_2,
			   unsigned long align, unsigned long region_flags)
{
	পূর্ণসংখ্যা rc;
	unsigned long pos, end, rsize;
	কাঠামো sbi_domain_memregion reg;

	pos = addr;
	end = addr + আকার_2;
	যখন (pos < end) {
		rsize = pos & (align - 1);
		যদি (rsize)
			rsize = 1UL << sbi_ffs(pos);
		অন্যথা
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = মূল_add_memregion(&reg);
		যদি (rc)
			ফেরত rc;
		pos += rsize;
	}

	ফেরত 0;
}

পূর্ণসংখ্যা sbi_domain_startup(কাঠামো sbi_scratch *scratch, u32 cold_hartid)
{
	পূর্ণসংখ্যা rc;
	u32 dhart;
	কাঠামো sbi_domain *dom;

	/* Sanity checks */
	যদি (!domain_finalized)
		ফেরত SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		যদি (!sbi_hartindex_valid(dhart))
			চালু;

		/* Ignore if boot HART not possible for this domain */
		যদি (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			চালু;

		/* Ignore if boot HART assigned different domain */
		যদি (sbi_hartindex_to_domain(dhart) != dom)
			চালু;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		যদি (!rc)
			চালু;

		/* Startup boot HART of domain */
		যদি (dom->boot_hartid == cold_hartid) {
			scratch->পরবর্তী_addr = dom->পরবর্তী_addr;
			scratch->পরবর্তী_mode = dom->পরবর্তী_mode;
			scratch->পরবর্তী_arg1 = dom->পরবর্তী_arg1;
		} অন্যথা {
			rc = sbi_hsm_hart_start(scratch, ফাঁকা,
						dom->boot_hartid,
						dom->পরবর্তী_addr,
						dom->পরবর্তী_mode,
						dom->পরবর্তী_arg1);
			যদি (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				ফেরত rc;
			}
		}
	}

	ফেরত 0;
}

পূর্ণসংখ্যা sbi_domain_finalize(কাঠামো sbi_scratch *scratch)
{
	পূর্ণসংখ্যা rc;
	ধ্রুবক কাঠামো sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	যদি (domain_finalized)
		ফেরত SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	যদি (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		ফেরত rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	ফেরত 0;
}

পূর্ণসংখ্যা sbi_domain_init(কাঠামো sbi_scratch *scratch, u32 cold_hartid)
{
	পূর্ণসংখ্যা rc;
	কাঠামো sbi_hartmask *মূল_hmask;
	কাঠামো sbi_domain_memregion *মূল_memregs;
	পূর্ণসংখ্যা মূল_memregs_গুনতি = 0;
	ধ্রুবক কাঠামো sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_শীর্ষ(&domain_list);

	যদি (!fw_single_region) {
		যদি (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			ফেরত SBI_EINVAL;
		}
		যদি ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			ফেরত SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(শূন্য *);
	যদি (!domain_hart_ptr_offset)
		ফেরত SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	যদি (rc)
		যাও fail_free_domain_hart_ptr_offset;

	মূল_memregs = sbi_calloc(আকার(*মূল_memregs), মূল_REGION_MAX + 1);
	যদি (!মূল_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		যাও fail_deinit_context;
	}
	মূল.regions = মূল_memregs;

	মূল_hmask = sbi_zalloc(আকার(*মূল_hmask));
	যদি (!মূল_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		যাও fail_free_মূল_memregs;
	}
	মূল.possible_harts = মূল_hmask;

	/* Root domain firmware memory region */
	যদি (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_আকার,
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
					  (scratch->fw_আকার -
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
	মূল.পরবর্তী_arg1 = scratch->পরবর্তী_arg1;
	মূল.পরবর্তী_addr = scratch->পরবর্তী_addr;
	মূল.পরবর্তী_mode = scratch->পরবর্তী_mode;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_set_hartindex(i, মূল_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_register(&মূল);
	যদি (rc)
		যাও fail_free_মূল_hmask;

	ফেরত 0;

fail_free_মূল_hmask:
	sbi_free(মূল_hmask);
fail_free_মূল_memregs:
	sbi_free(মূল_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	ফেরত rc;
}
