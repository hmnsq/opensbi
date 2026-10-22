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

SBI_LIST_HEAD(domain_list);

ساكن u32 domain_count = 0;
ساكن bool domain_finalized = false;

#define ROOT_REGION_MAX	32

بنية sbi_domain root = {
	.name = "root",
	.init_order = -1U,
	.possible_harts = عدم,
	.regions = عدم,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

ساكن unsigned long domain_hart_ptr_offset;

بنية sbi_domain *sbi_hartindex_to_domain(u32 hartindex)
{
	بنية sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	𐡄𐡍 (!scratch || !domain_hart_ptr_offset)
		أرجع عدم;

	أرجع sbi_scratch_read_type(scratch, فراغ *, domain_hart_ptr_offset);
}

فراغ sbi_update_hartindex_to_domain(u32 hartindex, بنية sbi_domain *dom)
{
	بنية sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	𐡄𐡍 (!scratch)
		أرجع;

	sbi_scratch_write_type(scratch, فراغ *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(ثابت بنية sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	بنية sbi_domain *tdom = (بنية sbi_domain *)dom;

	𐡄𐡍 (!dom)
		أرجع false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	أرجع ret;
}

صحيح sbi_domain_get_assigned_hartmask(ثابت بنية sbi_domain *dom,
				     بنية sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	بنية sbi_domain *tdom = (بنية sbi_domain *)dom;

	𐡄𐡍 (!dom) {
		sbi_hartmask_clear_all(mask);
		أرجع 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	أرجع ret;
}

فراغ sbi_domain_memregion_init(unsigned long addr,
				unsigned long size,
				unsigned long flags,
				بنية sbi_domain_memregion *reg)
{
	unsigned long base = 0, order;

	لكل (order = log2roundup(size) ; order <= __riscv_xlen; order++) {
		𐡄𐡍 (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			𐡄𐡍 ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				اكسر;
		} وإلا {
			base = 0;
			اكسر;
		}

	}

	𐡄𐡍 (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

unsigned صحيح sbi_domain_get_oldpmp_flags(بنية sbi_domain_memregion *reg)
{

	unsigned صحيح pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	أرجع pmp_flags;
}

unsigned صحيح sbi_domain_get_smepmp_flags(بنية sbi_domain_memregion *reg)
{
	unsigned صحيح pmp_flags = 0;
	unsigned long rstart, rend;

	𐡄𐡍 ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_MASK) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		أرجع 0;
	} وإلا 𐡄𐡍 (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		𐡄𐡍 (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		وإلا 𐡄𐡍 (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		وإلا 𐡄𐡍 (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		وإلا 𐡄𐡍 (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		وإلا 𐡄𐡍 (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} وإلا 𐡄𐡍 (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		𐡄𐡍 ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			أرجع 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} وإلا 𐡄𐡍 (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} وإلا {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ? rstart + ((1UL << reg->order) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	أرجع pmp_flags;
}

bool sbi_domain_check_addr(ثابت بنية sbi_domain *dom,
			   unsigned long addr, unsigned long mode,
			   unsigned long access_flags)
{
	bool rmmio, mmio = false;
	بنية sbi_domain_memregion *reg;
	unsigned long rstart, rend, rflags, rwx = 0, rrwx = 0;

	𐡄𐡍 (!dom)
		أرجع false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	𐡄𐡍 (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	𐡄𐡍 (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	𐡄𐡍 (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	𐡄𐡍 (access_flags & SBI_DOMAIN_MMIO)
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
		𐡄𐡍 (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			𐡄𐡍 (!mmio && rmmio)
				أرجع false;
			أرجع ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	أرجع (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
ساكن bool is_region_valid(ثابت بنية sbi_domain_memregion *reg)
{
	𐡄𐡍 (reg->order < 3 || __riscv_xlen < reg->order)
		أرجع false;

	𐡄𐡍 (reg->order == __riscv_xlen && reg->base != 0)
		أرجع false;

	𐡄𐡍 (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		أرجع false;

	أرجع true;
}

/** Check if regionA can be replaced by regionB */
ساكن bool is_region_compatible(ثابت بنية sbi_domain_memregion *regA,
				 ثابت بنية sbi_domain_memregion *regB)
{
	𐡄𐡍 (sbi_domain_memregion_is_subset(regA, regB) &&
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
	𐡄𐡍 (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		أرجع true;
	𐡄𐡍 (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		أرجع false;

	𐡄𐡍 (regA->order < regB->order)
		أرجع true;

	𐡄𐡍 ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		أرجع true;

	أرجع false;
}

ساكن ثابت بنية sbi_domain_memregion *find_region(
						ثابت بنية sbi_domain *dom,
						unsigned long addr)
{
	unsigned long rstart, rend;
	بنية sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		𐡄𐡍 (rstart <= addr && addr <= rend)
			أرجع reg;
	}

	أرجع عدم;
}

ساكن ثابت بنية sbi_domain_memregion *find_next_subset_region(
				ثابت بنية sbi_domain *dom,
				ثابت بنية sbi_domain_memregion *reg,
				unsigned long addr)
{
	بنية sbi_domain_memregion *sreg, *ret = عدم;

	sbi_domain_for_each_memregion(dom, sreg) {
		𐡄𐡍 (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			استمر;

		𐡄𐡍 (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	أرجع ret;
}

ساكن فراغ swap_region(بنية sbi_domain_memregion* reg1,
			بنية sbi_domain_memregion* reg2)
{
	بنية sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, حجم(treg));
	sbi_memcpy(reg1, reg2, حجم(treg));
	sbi_memcpy(reg2, &treg, حجم(treg));
}

ساكن صحيح sbi_domain_used_memregions(ثابت بنية sbi_domain *dom)
{
	صحيح count = 0;
	بنية sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		count++;

	أرجع count;
}

ساكن صحيح sanitize_domain(بنية sbi_domain *dom)
{
	u32 i, j, count;
	bool is_covered;
	بنية sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	𐡄𐡍 (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		أرجع SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		𐡄𐡍 (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			أرجع SBI_EINVAL;
		}
	}

	/* Check memory regions */
	𐡄𐡍 (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		أرجع SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		𐡄𐡍 (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			أرجع SBI_EINVAL;
		}
	}

	/* Count memory regions */
	count = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	𐡄𐡍 (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		أرجع SBI_EINVAL;
	}

	/* Sort the memory regions */
	لكل (i = 0; i < (count - 1); i++) {
		reg = &dom->regions[i];
		لكل (j = i + 1; j < count; j++) {
			reg1 = &dom->regions[j];

			𐡄𐡍 (!is_region_before(reg1, reg))
				استمر;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	لكل (i = 0; i < (count - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		لكل (j = i + 1; j < count; j++) {
			reg1 = &dom->regions[j];

			𐡄𐡍 (is_region_compatible(reg, reg1)) {
				is_covered = true;
				اكسر;
			}
		}

		/* find a region is superset of reg, remove reg */
		𐡄𐡍 (is_covered) {
			sbi_memmove(reg, reg + 1, حجم(*reg) * (count - i));
			count--;
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
	𐡄𐡍 (dom->next_mode != PRV_S &&
	    dom->next_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->next_mode);
		أرجع SBI_EINVAL;
	}

	/* Check next address and next mode */
	𐡄𐡍 (!sbi_domain_check_addr(dom, dom->next_addr, dom->next_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->next_addr);
		أرجع SBI_EINVAL;
	}

	أرجع 0;
}

bool sbi_domain_check_addr_range(ثابت بنية sbi_domain *dom,
				 unsigned long addr, unsigned long size,
				 unsigned long mode,
				 unsigned long access_flags)
{
	unsigned long max = addr + size;
	ثابت بنية sbi_domain_memregion *reg, *sreg;

	𐡄𐡍 (!dom)
		أرجع false;

	𐡄𐡍 (size && max <= addr)
		أرجع false;

	طالما (addr < max) {
		reg = find_region(dom, addr);
		𐡄𐡍 (!reg)
			أرجع false;

		𐡄𐡍 (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			أرجع false;

		sreg = find_next_subset_region(dom, reg, addr);
		𐡄𐡍 (sreg)
			addr = sreg->base;
		وإلا 𐡄𐡍 (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		وإلا
			اكسر;
	}

	أرجع true;
}

فراغ sbi_domain_dump(ثابت بنية sbi_domain *dom, ثابت حرف *suffix)
{
	u32 i, j, k;
	unsigned long rstart, rend;
	بنية sbi_domain_memregion *reg;

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
		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		𐡄𐡍 (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->index, suffix);
	بدل (dom->next_mode) {
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
		   dom->index, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->index, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

فراغ sbi_domain_dump_all(ثابت حرف *suffix)
{
	ثابت بنية sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

صحيح sbi_domain_register(بنية sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	بنية sbi_domain *tdom;
	صحيح rc;

	/* Sanity checks */
	𐡄𐡍 (!dom || domain_finalized)
		أرجع SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		𐡄𐡍 (tdom == dom ||
		    tdom->init_order == dom->init_order)
			أرجع SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	𐡄𐡍 (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		أرجع rc;
	}

	sbi_list_add_tail(&dom->node, &domain_list);

	/* Assign index to domain */
	dom->index = domain_count++;

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
		𐡄𐡍 (tdom) {
			𐡄𐡍 (tdom->init_order > dom->init_order)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			وإلا
				استمر;
		}

		sbi_update_hartindex_to_domain(i, dom);
		sbi_hartmask_set_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		𐡄𐡍 (sbi_hartindex_to_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->index, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	𐡄𐡍 (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->node);
		أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح root_add_memregion(ثابت بنية sbi_domain_memregion *reg)
{
	صحيح rc;
	bool reg_merged;
	بنية sbi_domain_memregion *nreg, *nreg1;
	صحيح root_memregs_count = sbi_domain_used_memregions(&root);

	/* Sanity checks */
	𐡄𐡍 (!reg || domain_finalized || !root.regions ||
	    (ROOT_REGION_MAX <= root_memregs_count))
		أرجع SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&root, nreg) {
		𐡄𐡍 (is_region_compatible(reg, nreg))
			أرجع 0;
	}

	/* Append the memregion to root memregions */
	nreg = &root.regions[root_memregs_count];
	sbi_memcpy(nreg, reg, حجم(*reg));
	root_memregs_count++;
	root.regions[root_memregs_count].order = 0;

	/* Sort and optimize root regions */
	𐡏𐡁𐡃 {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&root);
		𐡄𐡍 (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   root.name, rc);
			أرجع rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&root, nreg) {
			nreg1 = nreg + 1;
			𐡄𐡍 (!nreg1->order)
				استمر;

			𐡄𐡍 (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				صحيح i1 = nreg1 - root.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    حجم(*nreg1) * (root_memregs_count - i1));
				reg_merged = true;
				root_memregs_count--;
			}
		}
	} طالما (reg_merged);

	أرجع 0;
}

صحيح sbi_domain_root_add_memrange(unsigned long addr, unsigned long size,
			   unsigned long align, unsigned long region_flags)
{
	صحيح rc;
	unsigned long pos, end, rsize;
	بنية sbi_domain_memregion reg;

	pos = addr;
	end = addr + size;
	طالما (pos < end) {
		rsize = pos & (align - 1);
		𐡄𐡍 (rsize)
			rsize = 1UL << sbi_ffs(pos);
		وإلا
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = root_add_memregion(&reg);
		𐡄𐡍 (rc)
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
	𐡄𐡍 (!domain_finalized)
		أرجع SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		𐡄𐡍 (!sbi_hartindex_valid(dhart))
			استمر;

		/* Ignore if boot HART not possible for this domain */
		𐡄𐡍 (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			استمر;

		/* Ignore if boot HART assigned different domain */
		𐡄𐡍 (sbi_hartindex_to_domain(dhart) != dom)
			استمر;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		𐡄𐡍 (!rc)
			استمر;

		/* Startup boot HART of domain */
		𐡄𐡍 (dom->boot_hartid == cold_hartid) {
			scratch->next_addr = dom->next_addr;
			scratch->next_mode = dom->next_mode;
			scratch->next_arg1 = dom->next_arg1;
		} وإلا {
			rc = sbi_hsm_hart_start(scratch, عدم,
						dom->boot_hartid,
						dom->next_addr,
						dom->next_mode,
						dom->next_arg1);
			𐡄𐡍 (rc) {
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
	𐡄𐡍 (domain_finalized)
		أرجع SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	𐡄𐡍 (rc) {
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
	بنية sbi_hartmask *root_hmask;
	بنية sbi_domain_memregion *root_memregs;
	صحيح root_memregs_count = 0;
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_HEAD(&domain_list);

	𐡄𐡍 (!fw_single_region) {
		𐡄𐡍 (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			أرجع SBI_EINVAL;
		}
		𐡄𐡍 ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			أرجع SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(فراغ *);
	𐡄𐡍 (!domain_hart_ptr_offset)
		أرجع SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	𐡄𐡍 (rc)
		اذهب fail_free_domain_hart_ptr_offset;

	root_memregs = sbi_calloc(حجم(*root_memregs), ROOT_REGION_MAX + 1);
	𐡄𐡍 (!root_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		اذهب fail_deinit_context;
	}
	root.regions = root_memregs;

	root_hmask = sbi_zalloc(حجم(*root_hmask));
	𐡄𐡍 (!root_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		اذهب fail_free_root_memregs;
	}
	root.possible_harts = root_hmask;

	/* Root domain firmware memory region */
	𐡄𐡍 (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_size,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &root_memregs[root_memregs_count++]);
	} وإلا {
		sbi_domain_memregion_init(scratch->fw_start,
					  scratch->fw_rw_offset,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &root_memregs[root_memregs_count++]);

		sbi_domain_memregion_init((scratch->fw_start +
					   scratch->fw_rw_offset),
					  (scratch->fw_size -
					   scratch->fw_rw_offset),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &root_memregs[root_memregs_count++]);
	}

	root.fw_region_inited = true;

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
				  &root_memregs[root_memregs_count++]);

	/* Root domain memory region end */
	root_memregs[root_memregs_count].order = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	root.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	root.next_arg1 = scratch->next_arg1;
	root.next_addr = scratch->next_addr;
	root.next_mode = scratch->next_mode;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_set_hartindex(i, root_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_register(&root);
	𐡄𐡍 (rc)
		اذهب fail_free_root_hmask;

	أرجع 0;

fail_free_root_hmask:
	sbi_free(root_hmask);
fail_free_root_memregs:
	sbi_free(root_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	أرجع rc;
}
