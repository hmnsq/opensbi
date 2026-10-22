/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_hartmask.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_hsm.h>
#அந்தர்பாவ <sbi/sbi_list.h>
#அந்தர்பாவ <sbi/sbi_math.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_string.h>

SBI_LIST_HEAD(domain_list);

நிலையான u32 domain_மொத்தம் = 0;
நிலையான bool domain_finalized = false;

#define வேர்_REGION_MAX	32

கட்டமைப்பு sbi_domain வேர் = {
	.name = "root",
	.init_order = -1U,
	.possible_harts = வெற்று,
	.regions = வெற்று,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

நிலையான unsigned long domain_hart_ptr_offset;

கட்டமைப்பு sbi_domain *sbi_hartindex_to_domain(u32 hartindex)
{
	கட்டமைப்பு sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	எனில் (!scratch || !domain_hart_ptr_offset)
		திரும்பு வெற்று;

	திரும்பு sbi_scratch_read_type(scratch, வெறுமை *, domain_hart_ptr_offset);
}

வெறுமை sbi_update_hartindex_to_domain(u32 hartindex, கட்டமைப்பு sbi_domain *dom)
{
	கட்டமைப்பு sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	எனில் (!scratch)
		திரும்பு;

	sbi_scratch_write_type(scratch, வெறுமை *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(மாறிலி கட்டமைப்பு sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	கட்டமைப்பு sbi_domain *tdom = (கட்டமைப்பு sbi_domain *)dom;

	எனில் (!dom)
		திரும்பு false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	திரும்பு ret;
}

முழு sbi_domain_get_assigned_hartmask(மாறிலி கட்டமைப்பு sbi_domain *dom,
				     கட்டமைப்பு sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	கட்டமைப்பு sbi_domain *tdom = (கட்டமைப்பு sbi_domain *)dom;

	எனில் (!dom) {
		sbi_hartmask_clear_all(mask);
		திரும்பு 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	திரும்பு ret;
}

வெறுமை sbi_domain_memregion_init(unsigned long addr,
				unsigned long size,
				unsigned long flags,
				கட்டமைப்பு sbi_domain_memregion *reg)
{
	unsigned long base = 0, order;

	க்கு (order = log2roundup(size) ; order <= __riscv_xlen; order++) {
		எனில் (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			எனில் ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				நிறுத்து;
		} இல்லையெனில் {
			base = 0;
			நிறுத்து;
		}

	}

	எனில் (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

unsigned முழு sbi_domain_get_oldpmp_flags(கட்டமைப்பு sbi_domain_memregion *reg)
{

	unsigned முழு pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	எனில் (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	எனில் (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	எனில் (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	எனில் (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	திரும்பு pmp_flags;
}

unsigned முழு sbi_domain_get_smepmp_flags(கட்டமைப்பு sbi_domain_memregion *reg)
{
	unsigned முழு pmp_flags = 0;
	unsigned long rstart, rend;

	எனில் ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_MASK) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		திரும்பு 0;
	} இல்லையெனில் எனில் (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		எனில் (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		இல்லையெனில் எனில் (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		இல்லையெனில் எனில் (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		இல்லையெனில் எனில் (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		இல்லையெனில் எனில் (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} இல்லையெனில் எனில் (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		எனில் ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			திரும்பு 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} இல்லையெனில் எனில் (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} இல்லையெனில் {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ? rstart + ((1UL << reg->order) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	திரும்பு pmp_flags;
}

bool sbi_domain_check_addr(மாறிலி கட்டமைப்பு sbi_domain *dom,
			   unsigned long addr, unsigned long mode,
			   unsigned long access_flags)
{
	bool rmmio, mmio = false;
	கட்டமைப்பு sbi_domain_memregion *reg;
	unsigned long rstart, rend, rflags, rwx = 0, rrwx = 0;

	எனில் (!dom)
		திரும்பு false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	எனில் (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	எனில் (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	எனில் (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	எனில் (access_flags & SBI_DOMAIN_MMIO)
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
		எனில் (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			எனில் (!mmio && rmmio)
				திரும்பு false;
			திரும்பு ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	திரும்பு (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
நிலையான bool is_region_valid(மாறிலி கட்டமைப்பு sbi_domain_memregion *reg)
{
	எனில் (reg->order < 3 || __riscv_xlen < reg->order)
		திரும்பு false;

	எனில் (reg->order == __riscv_xlen && reg->base != 0)
		திரும்பு false;

	எனில் (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		திரும்பு false;

	திரும்பு true;
}

/** Check if regionA can be replaced by regionB */
நிலையான bool is_region_compatible(மாறிலி கட்டமைப்பு sbi_domain_memregion *regA,
				 மாறிலி கட்டமைப்பு sbi_domain_memregion *regB)
{
	எனில் (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		திரும்பு true;

	திரும்பு false;
}

/** Check if regionA should be placed before regionB */
நிலையான bool is_region_before(மாறிலி கட்டமைப்பு sbi_domain_memregion *regA,
			     மாறிலி கட்டமைப்பு sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	எனில் (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		திரும்பு true;
	எனில் (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		திரும்பு false;

	எனில் (regA->order < regB->order)
		திரும்பு true;

	எனில் ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		திரும்பு true;

	திரும்பு false;
}

நிலையான மாறிலி கட்டமைப்பு sbi_domain_memregion *find_region(
						மாறிலி கட்டமைப்பு sbi_domain *dom,
						unsigned long addr)
{
	unsigned long rstart, rend;
	கட்டமைப்பு sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		எனில் (rstart <= addr && addr <= rend)
			திரும்பு reg;
	}

	திரும்பு வெற்று;
}

நிலையான மாறிலி கட்டமைப்பு sbi_domain_memregion *find_next_subset_region(
				மாறிலி கட்டமைப்பு sbi_domain *dom,
				மாறிலி கட்டமைப்பு sbi_domain_memregion *reg,
				unsigned long addr)
{
	கட்டமைப்பு sbi_domain_memregion *sreg, *ret = வெற்று;

	sbi_domain_for_each_memregion(dom, sreg) {
		எனில் (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			தொடர்;

		எனில் (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	திரும்பு ret;
}

நிலையான வெறுமை swap_region(கட்டமைப்பு sbi_domain_memregion* reg1,
			கட்டமைப்பு sbi_domain_memregion* reg2)
{
	கட்டமைப்பு sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, அளவு(treg));
	sbi_memcpy(reg1, reg2, அளவு(treg));
	sbi_memcpy(reg2, &treg, அளவு(treg));
}

நிலையான முழு sbi_domain_used_memregions(மாறிலி கட்டமைப்பு sbi_domain *dom)
{
	முழு மொத்தம் = 0;
	கட்டமைப்பு sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		மொத்தம்++;

	திரும்பு மொத்தம்;
}

நிலையான முழு sanitize_domain(கட்டமைப்பு sbi_domain *dom)
{
	u32 i, j, மொத்தம்;
	bool is_covered;
	கட்டமைப்பு sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	எனில் (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		திரும்பு SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		எனில் (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			திரும்பு SBI_EINVAL;
		}
	}

	/* Check memory regions */
	எனில் (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		திரும்பு SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		எனில் (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			திரும்பு SBI_EINVAL;
		}
	}

	/* Count memory regions */
	மொத்தம் = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	எனில் (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		திரும்பு SBI_EINVAL;
	}

	/* Sort the memory regions */
	க்கு (i = 0; i < (மொத்தம் - 1); i++) {
		reg = &dom->regions[i];
		க்கு (j = i + 1; j < மொத்தம்; j++) {
			reg1 = &dom->regions[j];

			எனில் (!is_region_before(reg1, reg))
				தொடர்;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	க்கு (i = 0; i < (மொத்தம் - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		க்கு (j = i + 1; j < மொத்தம்; j++) {
			reg1 = &dom->regions[j];

			எனில் (is_region_compatible(reg, reg1)) {
				is_covered = true;
				நிறுத்து;
			}
		}

		/* find a region is superset of reg, remove reg */
		எனில் (is_covered) {
			sbi_memmove(reg, reg + 1, அளவு(*reg) * (மொத்தம் - i));
			மொத்தம்--;
		} இல்லையெனில்
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
	எனில் (dom->next_mode != PRV_S &&
	    dom->next_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->next_mode);
		திரும்பு SBI_EINVAL;
	}

	/* Check next address and next mode */
	எனில் (!sbi_domain_check_addr(dom, dom->next_addr, dom->next_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->next_addr);
		திரும்பு SBI_EINVAL;
	}

	திரும்பு 0;
}

bool sbi_domain_check_addr_range(மாறிலி கட்டமைப்பு sbi_domain *dom,
				 unsigned long addr, unsigned long size,
				 unsigned long mode,
				 unsigned long access_flags)
{
	unsigned long max = addr + size;
	மாறிலி கட்டமைப்பு sbi_domain_memregion *reg, *sreg;

	எனில் (!dom)
		திரும்பு false;

	எனில் (size && max <= addr)
		திரும்பு false;

	போது (addr < max) {
		reg = find_region(dom, addr);
		எனில் (!reg)
			திரும்பு false;

		எனில் (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			திரும்பு false;

		sreg = find_next_subset_region(dom, reg, addr);
		எனில் (sreg)
			addr = sreg->base;
		இல்லையெனில் எனில் (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		இல்லையெனில்
			நிறுத்து;
	}

	திரும்பு true;
}

வெறுமை sbi_domain_dump(மாறிலி கட்டமைப்பு sbi_domain *dom, மாறிலி எழுத்து *suffix)
{
	u32 i, j, k;
	unsigned long rstart, rend;
	கட்டமைப்பு sbi_domain_memregion *reg;

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
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->index, suffix);
	தேர்வு (dom->next_mode) {
	நிலை PRV_M:
		sbi_printf("M-mode\n");
		நிறுத்து;
	நிலை PRV_S:
		sbi_printf("S-mode\n");
		நிறுத்து;
	நிலை PRV_U:
		sbi_printf("U-mode\n");
		நிறுத்து;
	இயல்பு:
		sbi_printf("Unknown\n");
		நிறுத்து;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->index, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->index, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

வெறுமை sbi_domain_dump_all(மாறிலி எழுத்து *suffix)
{
	மாறிலி கட்டமைப்பு sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

முழு sbi_domain_register(கட்டமைப்பு sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	கட்டமைப்பு sbi_domain *tdom;
	முழு rc;

	/* Sanity checks */
	எனில் (!dom || domain_finalized)
		திரும்பு SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		எனில் (tdom == dom ||
		    tdom->init_order == dom->init_order)
			திரும்பு SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	எனில் (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		திரும்பு rc;
	}

	sbi_list_add_tail(&dom->முனை, &domain_list);

	/* Assign index to domain */
	dom->index = domain_மொத்தம்++;

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
		எனில் (tdom) {
			எனில் (tdom->init_order > dom->init_order)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			இல்லையெனில்
				தொடர்;
		}

		sbi_update_hartindex_to_domain(i, dom);
		sbi_hartmask_set_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		எனில் (sbi_hartindex_to_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->index, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	எனில் (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->முனை);
		திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான முழு வேர்_add_memregion(மாறிலி கட்டமைப்பு sbi_domain_memregion *reg)
{
	முழு rc;
	bool reg_merged;
	கட்டமைப்பு sbi_domain_memregion *nreg, *nreg1;
	முழு வேர்_memregs_மொத்தம் = sbi_domain_used_memregions(&வேர்);

	/* Sanity checks */
	எனில் (!reg || domain_finalized || !வேர்.regions ||
	    (வேர்_REGION_MAX <= வேர்_memregs_மொத்தம்))
		திரும்பு SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&வேர், nreg) {
		எனில் (is_region_compatible(reg, nreg))
			திரும்பு 0;
	}

	/* Append the memregion to root memregions */
	nreg = &வேர்.regions[வேர்_memregs_மொத்தம்];
	sbi_memcpy(nreg, reg, அளவு(*reg));
	வேர்_memregs_மொத்தம்++;
	வேர்.regions[வேர்_memregs_மொத்தம்].order = 0;

	/* Sort and optimize root regions */
	செய் {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&வேர்);
		எனில் (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   வேர்.name, rc);
			திரும்பு rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&வேர், nreg) {
			nreg1 = nreg + 1;
			எனில் (!nreg1->order)
				தொடர்;

			எனில் (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				முழு i1 = nreg1 - வேர்.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    அளவு(*nreg1) * (வேர்_memregs_மொத்தம் - i1));
				reg_merged = true;
				வேர்_memregs_மொத்தம்--;
			}
		}
	} போது (reg_merged);

	திரும்பு 0;
}

முழு sbi_domain_வேர்_add_memrange(unsigned long addr, unsigned long size,
			   unsigned long align, unsigned long region_flags)
{
	முழு rc;
	unsigned long pos, end, rsize;
	கட்டமைப்பு sbi_domain_memregion reg;

	pos = addr;
	end = addr + size;
	போது (pos < end) {
		rsize = pos & (align - 1);
		எனில் (rsize)
			rsize = 1UL << sbi_ffs(pos);
		இல்லையெனில்
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = வேர்_add_memregion(&reg);
		எனில் (rc)
			திரும்பு rc;
		pos += rsize;
	}

	திரும்பு 0;
}

முழு sbi_domain_startup(கட்டமைப்பு sbi_scratch *scratch, u32 cold_hartid)
{
	முழு rc;
	u32 dhart;
	கட்டமைப்பு sbi_domain *dom;

	/* Sanity checks */
	எனில் (!domain_finalized)
		திரும்பு SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		எனில் (!sbi_hartindex_valid(dhart))
			தொடர்;

		/* Ignore if boot HART not possible for this domain */
		எனில் (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			தொடர்;

		/* Ignore if boot HART assigned different domain */
		எனில் (sbi_hartindex_to_domain(dhart) != dom)
			தொடர்;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		எனில் (!rc)
			தொடர்;

		/* Startup boot HART of domain */
		எனில் (dom->boot_hartid == cold_hartid) {
			scratch->next_addr = dom->next_addr;
			scratch->next_mode = dom->next_mode;
			scratch->next_arg1 = dom->next_arg1;
		} இல்லையெனில் {
			rc = sbi_hsm_hart_start(scratch, வெற்று,
						dom->boot_hartid,
						dom->next_addr,
						dom->next_mode,
						dom->next_arg1);
			எனில் (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				திரும்பு rc;
			}
		}
	}

	திரும்பு 0;
}

முழு sbi_domain_finalize(கட்டமைப்பு sbi_scratch *scratch)
{
	முழு rc;
	மாறிலி கட்டமைப்பு sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	எனில் (domain_finalized)
		திரும்பு SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	எனில் (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		திரும்பு rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	திரும்பு 0;
}

முழு sbi_domain_init(கட்டமைப்பு sbi_scratch *scratch, u32 cold_hartid)
{
	முழு rc;
	கட்டமைப்பு sbi_hartmask *வேர்_hmask;
	கட்டமைப்பு sbi_domain_memregion *வேர்_memregs;
	முழு வேர்_memregs_மொத்தம் = 0;
	மாறிலி கட்டமைப்பு sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_HEAD(&domain_list);

	எனில் (!fw_single_region) {
		எனில் (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			திரும்பு SBI_EINVAL;
		}
		எனில் ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			திரும்பு SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(வெறுமை *);
	எனில் (!domain_hart_ptr_offset)
		திரும்பு SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	எனில் (rc)
		செல் fail_free_domain_hart_ptr_offset;

	வேர்_memregs = sbi_calloc(அளவு(*வேர்_memregs), வேர்_REGION_MAX + 1);
	எனில் (!வேர்_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		செல் fail_deinit_context;
	}
	வேர்.regions = வேர்_memregs;

	வேர்_hmask = sbi_zalloc(அளவு(*வேர்_hmask));
	எனில் (!வேர்_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		செல் fail_free_வேர்_memregs;
	}
	வேர்.possible_harts = வேர்_hmask;

	/* Root domain firmware memory region */
	எனில் (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_size,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &வேர்_memregs[வேர்_memregs_மொத்தம்++]);
	} இல்லையெனில் {
		sbi_domain_memregion_init(scratch->fw_start,
					  scratch->fw_rw_offset,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &வேர்_memregs[வேர்_memregs_மொத்தம்++]);

		sbi_domain_memregion_init((scratch->fw_start +
					   scratch->fw_rw_offset),
					  (scratch->fw_size -
					   scratch->fw_rw_offset),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &வேர்_memregs[வேர்_memregs_மொத்தம்++]);
	}

	வேர்.fw_region_inited = true;

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
				  &வேர்_memregs[வேர்_memregs_மொத்தம்++]);

	/* Root domain memory region end */
	வேர்_memregs[வேர்_memregs_மொத்தம்].order = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	வேர்.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	வேர்.next_arg1 = scratch->next_arg1;
	வேர்.next_addr = scratch->next_addr;
	வேர்.next_mode = scratch->next_mode;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_set_hartindex(i, வேர்_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_register(&வேர்);
	எனில் (rc)
		செல் fail_free_வேர்_hmask;

	திரும்பு 0;

fail_free_வேர்_hmask:
	sbi_free(வேர்_hmask);
fail_free_வேர்_memregs:
	sbi_free(வேர்_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	திரும்பு rc;
}
