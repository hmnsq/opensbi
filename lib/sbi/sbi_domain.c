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

SBI_LIST_HEAD(domain_list);

ایستا u32 domain_شمار = 0;
ایستا bool domain_finalized = false;

#define ریشه_REGION_MAX	32

ساختار sbi_domain ریشه = {
	.name = "root",
	.init_order = -1U,
	.possible_harts = پوچ,
	.regions = پوچ,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

ایستا unsigned long domain_hart_ptr_offset;

ساختار sbi_domain *sbi_hartindex_to_domain(u32 hartindex)
{
	ساختار sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	اگر (!scratch || !domain_hart_ptr_offset)
		بازگشت پوچ;

	بازگشت sbi_scratch_read_type(scratch, تهی *, domain_hart_ptr_offset);
}

تهی sbi_update_hartindex_to_domain(u32 hartindex, ساختار sbi_domain *dom)
{
	ساختار sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	اگر (!scratch)
		بازگشت;

	sbi_scratch_write_type(scratch, تهی *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(ثابت ساختار sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	ساختار sbi_domain *tdom = (ساختار sbi_domain *)dom;

	اگر (!dom)
		بازگشت false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	بازگشت ret;
}

صحیح sbi_domain_get_assigned_hartmask(ثابت ساختار sbi_domain *dom,
				     ساختار sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	ساختار sbi_domain *tdom = (ساختار sbi_domain *)dom;

	اگر (!dom) {
		sbi_hartmask_clear_all(mask);
		بازگشت 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	بازگشت ret;
}

تهی sbi_domain_memregion_init(unsigned long addr,
				unsigned long size,
				unsigned long flags,
				ساختار sbi_domain_memregion *reg)
{
	unsigned long base = 0, order;

	برای (order = log2roundup(size) ; order <= __riscv_xlen; order++) {
		اگر (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			اگر ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				بشکن;
		} وگرنه {
			base = 0;
			بشکن;
		}

	}

	اگر (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

unsigned صحیح sbi_domain_get_oldpmp_flags(ساختار sbi_domain_memregion *reg)
{

	unsigned صحیح pmp_flags = 0;

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

	بازگشت pmp_flags;
}

unsigned صحیح sbi_domain_get_smepmp_flags(ساختار sbi_domain_memregion *reg)
{
	unsigned صحیح pmp_flags = 0;
	unsigned long rstart, rend;

	اگر ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_MASK) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		بازگشت 0;
	} وگرنه اگر (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		اگر (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		وگرنه اگر (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		وگرنه اگر (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		وگرنه اگر (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		وگرنه اگر (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} وگرنه اگر (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		اگر ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			بازگشت 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		اگر (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} وگرنه اگر (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} وگرنه {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ? rstart + ((1UL << reg->order) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	بازگشت pmp_flags;
}

bool sbi_domain_check_addr(ثابت ساختار sbi_domain *dom,
			   unsigned long addr, unsigned long mode,
			   unsigned long access_flags)
{
	bool rmmio, mmio = false;
	ساختار sbi_domain_memregion *reg;
	unsigned long rstart, rend, rflags, rwx = 0, rrwx = 0;

	اگر (!dom)
		بازگشت false;

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
		rrwx = (mode == PRV_M ?
			(rflags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK) :
			(rflags & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
			>> SBI_DOMAIN_MEMREGION_SU_ACCESS_SHIFT);

		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		اگر (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			اگر (!mmio && rmmio)
				بازگشت false;
			بازگشت ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	بازگشت (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
ایستا bool is_region_valid(ثابت ساختار sbi_domain_memregion *reg)
{
	اگر (reg->order < 3 || __riscv_xlen < reg->order)
		بازگشت false;

	اگر (reg->order == __riscv_xlen && reg->base != 0)
		بازگشت false;

	اگر (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		بازگشت false;

	بازگشت true;
}

/** Check if regionA can be replaced by regionB */
ایستا bool is_region_compatible(ثابت ساختار sbi_domain_memregion *regA,
				 ثابت ساختار sbi_domain_memregion *regB)
{
	اگر (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		بازگشت true;

	بازگشت false;
}

/** Check if regionA should be placed before regionB */
ایستا bool is_region_before(ثابت ساختار sbi_domain_memregion *regA,
			     ثابت ساختار sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	اگر (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		بازگشت true;
	اگر (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		بازگشت false;

	اگر (regA->order < regB->order)
		بازگشت true;

	اگر ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		بازگشت true;

	بازگشت false;
}

ایستا ثابت ساختار sbi_domain_memregion *find_region(
						ثابت ساختار sbi_domain *dom,
						unsigned long addr)
{
	unsigned long rstart, rend;
	ساختار sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		اگر (rstart <= addr && addr <= rend)
			بازگشت reg;
	}

	بازگشت پوچ;
}

ایستا ثابت ساختار sbi_domain_memregion *find_next_subset_region(
				ثابت ساختار sbi_domain *dom,
				ثابت ساختار sbi_domain_memregion *reg,
				unsigned long addr)
{
	ساختار sbi_domain_memregion *sreg, *ret = پوچ;

	sbi_domain_for_each_memregion(dom, sreg) {
		اگر (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			ادامه;

		اگر (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	بازگشت ret;
}

ایستا تهی swap_region(ساختار sbi_domain_memregion* reg1,
			ساختار sbi_domain_memregion* reg2)
{
	ساختار sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, اندازه(treg));
	sbi_memcpy(reg1, reg2, اندازه(treg));
	sbi_memcpy(reg2, &treg, اندازه(treg));
}

ایستا صحیح sbi_domain_used_memregions(ثابت ساختار sbi_domain *dom)
{
	صحیح شمار = 0;
	ساختار sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		شمار++;

	بازگشت شمار;
}

ایستا صحیح sanitize_domain(ساختار sbi_domain *dom)
{
	u32 i, j, شمار;
	bool is_covered;
	ساختار sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	اگر (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		بازگشت SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		اگر (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			بازگشت SBI_EINVAL;
		}
	}

	/* Check memory regions */
	اگر (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		بازگشت SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		اگر (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			بازگشت SBI_EINVAL;
		}
	}

	/* Count memory regions */
	شمار = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	اگر (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		بازگشت SBI_EINVAL;
	}

	/* Sort the memory regions */
	برای (i = 0; i < (شمار - 1); i++) {
		reg = &dom->regions[i];
		برای (j = i + 1; j < شمار; j++) {
			reg1 = &dom->regions[j];

			اگر (!is_region_before(reg1, reg))
				ادامه;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	برای (i = 0; i < (شمار - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		برای (j = i + 1; j < شمار; j++) {
			reg1 = &dom->regions[j];

			اگر (is_region_compatible(reg, reg1)) {
				is_covered = true;
				بشکن;
			}
		}

		/* find a region is superset of reg, remove reg */
		اگر (is_covered) {
			sbi_memmove(reg, reg + 1, اندازه(*reg) * (شمار - i));
			شمار--;
		} وگرنه
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
	اگر (dom->next_mode != PRV_S &&
	    dom->next_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->next_mode);
		بازگشت SBI_EINVAL;
	}

	/* Check next address and next mode */
	اگر (!sbi_domain_check_addr(dom, dom->next_addr, dom->next_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->next_addr);
		بازگشت SBI_EINVAL;
	}

	بازگشت 0;
}

bool sbi_domain_check_addr_range(ثابت ساختار sbi_domain *dom,
				 unsigned long addr, unsigned long size,
				 unsigned long mode,
				 unsigned long access_flags)
{
	unsigned long max = addr + size;
	ثابت ساختار sbi_domain_memregion *reg, *sreg;

	اگر (!dom)
		بازگشت false;

	اگر (size && max <= addr)
		بازگشت false;

	وقتی (addr < max) {
		reg = find_region(dom, addr);
		اگر (!reg)
			بازگشت false;

		اگر (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			بازگشت false;

		sreg = find_next_subset_region(dom, reg, addr);
		اگر (sreg)
			addr = sreg->base;
		وگرنه اگر (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		وگرنه
			بشکن;
	}

	بازگشت true;
}

تهی sbi_domain_dump(ثابت ساختار sbi_domain *dom, ثابت نویسه *suffix)
{
	u32 i, j, k;
	unsigned long rstart, rend;
	ساختار sbi_domain_memregion *reg;

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
		   dom->index, suffix, dom->next_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->index, suffix);
	انتخاب (dom->next_mode) {
	حالت PRV_M:
		sbi_printf("M-mode\n");
		بشکن;
	حالت PRV_S:
		sbi_printf("S-mode\n");
		بشکن;
	حالت PRV_U:
		sbi_printf("U-mode\n");
		بشکن;
	پیشفرض:
		sbi_printf("Unknown\n");
		بشکن;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->index, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->index, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

تهی sbi_domain_dump_all(ثابت نویسه *suffix)
{
	ثابت ساختار sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

صحیح sbi_domain_register(ساختار sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	ساختار sbi_domain *tdom;
	صحیح rc;

	/* Sanity checks */
	اگر (!dom || domain_finalized)
		بازگشت SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		اگر (tdom == dom ||
		    tdom->init_order == dom->init_order)
			بازگشت SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	اگر (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		بازگشت rc;
	}

	sbi_list_add_tail(&dom->گره, &domain_list);

	/* Assign index to domain */
	dom->index = domain_شمار++;

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
		اگر (tdom) {
			اگر (tdom->init_order > dom->init_order)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			وگرنه
				ادامه;
		}

		sbi_update_hartindex_to_domain(i, dom);
		sbi_hartmask_set_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		اگر (sbi_hartindex_to_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->index, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	اگر (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->گره);
		بازگشت rc;
	}

	بازگشت 0;
}

ایستا صحیح ریشه_add_memregion(ثابت ساختار sbi_domain_memregion *reg)
{
	صحیح rc;
	bool reg_merged;
	ساختار sbi_domain_memregion *nreg, *nreg1;
	صحیح ریشه_memregs_شمار = sbi_domain_used_memregions(&ریشه);

	/* Sanity checks */
	اگر (!reg || domain_finalized || !ریشه.regions ||
	    (ریشه_REGION_MAX <= ریشه_memregs_شمار))
		بازگشت SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&ریشه, nreg) {
		اگر (is_region_compatible(reg, nreg))
			بازگشت 0;
	}

	/* Append the memregion to root memregions */
	nreg = &ریشه.regions[ریشه_memregs_شمار];
	sbi_memcpy(nreg, reg, اندازه(*reg));
	ریشه_memregs_شمار++;
	ریشه.regions[ریشه_memregs_شمار].order = 0;

	/* Sort and optimize root regions */
	انجامبده {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&ریشه);
		اگر (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   ریشه.name, rc);
			بازگشت rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&ریشه, nreg) {
			nreg1 = nreg + 1;
			اگر (!nreg1->order)
				ادامه;

			اگر (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				صحیح i1 = nreg1 - ریشه.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    اندازه(*nreg1) * (ریشه_memregs_شمار - i1));
				reg_merged = true;
				ریشه_memregs_شمار--;
			}
		}
	} وقتی (reg_merged);

	بازگشت 0;
}

صحیح sbi_domain_ریشه_add_memrange(unsigned long addr, unsigned long size,
			   unsigned long align, unsigned long region_flags)
{
	صحیح rc;
	unsigned long pos, end, rsize;
	ساختار sbi_domain_memregion reg;

	pos = addr;
	end = addr + size;
	وقتی (pos < end) {
		rsize = pos & (align - 1);
		اگر (rsize)
			rsize = 1UL << sbi_ffs(pos);
		وگرنه
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = ریشه_add_memregion(&reg);
		اگر (rc)
			بازگشت rc;
		pos += rsize;
	}

	بازگشت 0;
}

صحیح sbi_domain_startup(ساختار sbi_scratch *scratch, u32 cold_hartid)
{
	صحیح rc;
	u32 dhart;
	ساختار sbi_domain *dom;

	/* Sanity checks */
	اگر (!domain_finalized)
		بازگشت SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		اگر (!sbi_hartindex_valid(dhart))
			ادامه;

		/* Ignore if boot HART not possible for this domain */
		اگر (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			ادامه;

		/* Ignore if boot HART assigned different domain */
		اگر (sbi_hartindex_to_domain(dhart) != dom)
			ادامه;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		اگر (!rc)
			ادامه;

		/* Startup boot HART of domain */
		اگر (dom->boot_hartid == cold_hartid) {
			scratch->next_addr = dom->next_addr;
			scratch->next_mode = dom->next_mode;
			scratch->next_arg1 = dom->next_arg1;
		} وگرنه {
			rc = sbi_hsm_hart_start(scratch, پوچ,
						dom->boot_hartid,
						dom->next_addr,
						dom->next_mode,
						dom->next_arg1);
			اگر (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				بازگشت rc;
			}
		}
	}

	بازگشت 0;
}

صحیح sbi_domain_finalize(ساختار sbi_scratch *scratch)
{
	صحیح rc;
	ثابت ساختار sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	اگر (domain_finalized)
		بازگشت SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	اگر (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		بازگشت rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	بازگشت 0;
}

صحیح sbi_domain_init(ساختار sbi_scratch *scratch, u32 cold_hartid)
{
	صحیح rc;
	ساختار sbi_hartmask *ریشه_hmask;
	ساختار sbi_domain_memregion *ریشه_memregs;
	صحیح ریشه_memregs_شمار = 0;
	ثابت ساختار sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_HEAD(&domain_list);

	اگر (!fw_single_region) {
		اگر (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			بازگشت SBI_EINVAL;
		}
		اگر ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			بازگشت SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(تهی *);
	اگر (!domain_hart_ptr_offset)
		بازگشت SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	اگر (rc)
		برو fail_free_domain_hart_ptr_offset;

	ریشه_memregs = sbi_calloc(اندازه(*ریشه_memregs), ریشه_REGION_MAX + 1);
	اگر (!ریشه_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		برو fail_deinit_context;
	}
	ریشه.regions = ریشه_memregs;

	ریشه_hmask = sbi_zalloc(اندازه(*ریشه_hmask));
	اگر (!ریشه_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		برو fail_free_ریشه_memregs;
	}
	ریشه.possible_harts = ریشه_hmask;

	/* Root domain firmware memory region */
	اگر (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_size,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ریشه_memregs[ریشه_memregs_شمار++]);
	} وگرنه {
		sbi_domain_memregion_init(scratch->fw_start,
					  scratch->fw_rw_offset,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ریشه_memregs[ریشه_memregs_شمار++]);

		sbi_domain_memregion_init((scratch->fw_start +
					   scratch->fw_rw_offset),
					  (scratch->fw_size -
					   scratch->fw_rw_offset),
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
	ریشه_memregs[ریشه_memregs_شمار].order = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	ریشه.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	ریشه.next_arg1 = scratch->next_arg1;
	ریشه.next_addr = scratch->next_addr;
	ریشه.next_mode = scratch->next_mode;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_set_hartindex(i, ریشه_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_register(&ریشه);
	اگر (rc)
		برو fail_free_ریشه_hmask;

	بازگشت 0;

fail_free_ریشه_hmask:
	sbi_free(ریشه_hmask);
fail_free_ریشه_memregs:
	sbi_free(ریشه_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	بازگشت rc;
}
