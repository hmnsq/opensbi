/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_hartmask.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_hsm.h>
#અન્તર્ભાવ <sbi/sbi_list.h>
#અન્તર્ભાવ <sbi/sbi_math.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_string.h>

SBI_LIST_HEAD(domain_list);

સ્થિર u32 domain_ગણતરી = 0;
સ્થિર bool domain_finalized = false;

#define મૂળ_REGION_MAX	32

સંરચના sbi_domain મૂળ = {
	.name = "root",
	.init_order = -1U,
	.possible_harts = ખાલી,
	.regions = ખાલી,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

સ્થિર unsigned long domain_hart_ptr_offset;

સંરચના sbi_domain *sbi_hartindex_to_domain(u32 hartindex)
{
	સંરચના sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	જો (!scratch || !domain_hart_ptr_offset)
		પાછા ખાલી;

	પાછા sbi_scratch_read_type(scratch, શૂન્ય *, domain_hart_ptr_offset);
}

શૂન્ય sbi_update_hartindex_to_domain(u32 hartindex, સંરચના sbi_domain *dom)
{
	સંરચના sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	જો (!scratch)
		પાછા;

	sbi_scratch_write_type(scratch, શૂન્ય *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(અચલ સંરચના sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	સંરચના sbi_domain *tdom = (સંરચના sbi_domain *)dom;

	જો (!dom)
		પાછા false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	પાછા ret;
}

પૂર્ણાંક sbi_domain_get_assigned_hartmask(અચલ સંરચના sbi_domain *dom,
				     સંરચના sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	સંરચના sbi_domain *tdom = (સંરચના sbi_domain *)dom;

	જો (!dom) {
		sbi_hartmask_clear_all(mask);
		પાછા 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	પાછા ret;
}

શૂન્ય sbi_domain_memregion_init(unsigned long addr,
				unsigned long size,
				unsigned long flags,
				સંરચના sbi_domain_memregion *reg)
{
	unsigned long base = 0, order;

	માટે (order = log2roundup(size) ; order <= __riscv_xlen; order++) {
		જો (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			જો ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				તોડો;
		} નહીંતર {
			base = 0;
			તોડો;
		}

	}

	જો (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

unsigned પૂર્ણાંક sbi_domain_get_oldpmp_flags(સંરચના sbi_domain_memregion *reg)
{

	unsigned પૂર્ણાંક pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	જો (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	જો (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	જો (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	જો (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	પાછા pmp_flags;
}

unsigned પૂર્ણાંક sbi_domain_get_smepmp_flags(સંરચના sbi_domain_memregion *reg)
{
	unsigned પૂર્ણાંક pmp_flags = 0;
	unsigned long rstart, rend;

	જો ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_MASK) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		પાછા 0;
	} નહીંતર જો (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		જો (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		નહીંતર જો (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		નહીંતર જો (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		નહીંતર જો (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		નહીંતર જો (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} નહીંતર જો (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		જો ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			પાછા 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		જો (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		જો (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		જો (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} નહીંતર જો (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		જો (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		જો (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		જો (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} નહીંતર {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ? rstart + ((1UL << reg->order) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	પાછા pmp_flags;
}

bool sbi_domain_check_addr(અચલ સંરચના sbi_domain *dom,
			   unsigned long addr, unsigned long mode,
			   unsigned long access_flags)
{
	bool rmmio, mmio = false;
	સંરચના sbi_domain_memregion *reg;
	unsigned long rstart, rend, rflags, rwx = 0, rrwx = 0;

	જો (!dom)
		પાછા false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	જો (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	જો (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	જો (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	જો (access_flags & SBI_DOMAIN_MMIO)
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
		જો (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			જો (!mmio && rmmio)
				પાછા false;
			પાછા ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	પાછા (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
સ્થિર bool is_region_valid(અચલ સંરચના sbi_domain_memregion *reg)
{
	જો (reg->order < 3 || __riscv_xlen < reg->order)
		પાછા false;

	જો (reg->order == __riscv_xlen && reg->base != 0)
		પાછા false;

	જો (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		પાછા false;

	પાછા true;
}

/** Check if regionA can be replaced by regionB */
સ્થિર bool is_region_compatible(અચલ સંરચના sbi_domain_memregion *regA,
				 અચલ સંરચના sbi_domain_memregion *regB)
{
	જો (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		પાછા true;

	પાછા false;
}

/** Check if regionA should be placed before regionB */
સ્થિર bool is_region_before(અચલ સંરચના sbi_domain_memregion *regA,
			     અચલ સંરચના sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	જો (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		પાછા true;
	જો (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		પાછા false;

	જો (regA->order < regB->order)
		પાછા true;

	જો ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		પાછા true;

	પાછા false;
}

સ્થિર અચલ સંરચના sbi_domain_memregion *find_region(
						અચલ સંરચના sbi_domain *dom,
						unsigned long addr)
{
	unsigned long rstart, rend;
	સંરચના sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		જો (rstart <= addr && addr <= rend)
			પાછા reg;
	}

	પાછા ખાલી;
}

સ્થિર અચલ સંરચના sbi_domain_memregion *find_next_subset_region(
				અચલ સંરચના sbi_domain *dom,
				અચલ સંરચના sbi_domain_memregion *reg,
				unsigned long addr)
{
	સંરચના sbi_domain_memregion *sreg, *ret = ખાલી;

	sbi_domain_for_each_memregion(dom, sreg) {
		જો (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			ચાલુ;

		જો (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	પાછા ret;
}

સ્થિર શૂન્ય swap_region(સંરચના sbi_domain_memregion* reg1,
			સંરચના sbi_domain_memregion* reg2)
{
	સંરચના sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, કદ(treg));
	sbi_memcpy(reg1, reg2, કદ(treg));
	sbi_memcpy(reg2, &treg, કદ(treg));
}

સ્થિર પૂર્ણાંક sbi_domain_used_memregions(અચલ સંરચના sbi_domain *dom)
{
	પૂર્ણાંક ગણતરી = 0;
	સંરચના sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		ગણતરી++;

	પાછા ગણતરી;
}

સ્થિર પૂર્ણાંક sanitize_domain(સંરચના sbi_domain *dom)
{
	u32 i, j, ગણતરી;
	bool is_covered;
	સંરચના sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	જો (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		પાછા SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		જો (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			પાછા SBI_EINVAL;
		}
	}

	/* Check memory regions */
	જો (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		પાછા SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		જો (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			પાછા SBI_EINVAL;
		}
	}

	/* Count memory regions */
	ગણતરી = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	જો (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		પાછા SBI_EINVAL;
	}

	/* Sort the memory regions */
	માટે (i = 0; i < (ગણતરી - 1); i++) {
		reg = &dom->regions[i];
		માટે (j = i + 1; j < ગણતરી; j++) {
			reg1 = &dom->regions[j];

			જો (!is_region_before(reg1, reg))
				ચાલુ;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	માટે (i = 0; i < (ગણતરી - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		માટે (j = i + 1; j < ગણતરી; j++) {
			reg1 = &dom->regions[j];

			જો (is_region_compatible(reg, reg1)) {
				is_covered = true;
				તોડો;
			}
		}

		/* find a region is superset of reg, remove reg */
		જો (is_covered) {
			sbi_memmove(reg, reg + 1, કદ(*reg) * (ગણતરી - i));
			ગણતરી--;
		} નહીંતર
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
	જો (dom->next_mode != PRV_S &&
	    dom->next_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->next_mode);
		પાછા SBI_EINVAL;
	}

	/* Check next address and next mode */
	જો (!sbi_domain_check_addr(dom, dom->next_addr, dom->next_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->next_addr);
		પાછા SBI_EINVAL;
	}

	પાછા 0;
}

bool sbi_domain_check_addr_range(અચલ સંરચના sbi_domain *dom,
				 unsigned long addr, unsigned long size,
				 unsigned long mode,
				 unsigned long access_flags)
{
	unsigned long max = addr + size;
	અચલ સંરચના sbi_domain_memregion *reg, *sreg;

	જો (!dom)
		પાછા false;

	જો (size && max <= addr)
		પાછા false;

	જ્યારે (addr < max) {
		reg = find_region(dom, addr);
		જો (!reg)
			પાછા false;

		જો (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			પાછા false;

		sreg = find_next_subset_region(dom, reg, addr);
		જો (sreg)
			addr = sreg->base;
		નહીંતર જો (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		નહીંતર
			તોડો;
	}

	પાછા true;
}

શૂન્ય sbi_domain_dump(અચલ સંરચના sbi_domain *dom, અચલ અક્ષર *suffix)
{
	u32 i, j, k;
	unsigned long rstart, rend;
	સંરચના sbi_domain_memregion *reg;

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
		જો (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		જો (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		જો (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		જો (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		જો (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		જો (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		જો (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		જો (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->index, suffix);
	પસંદગી (dom->next_mode) {
	સ્થિતિ PRV_M:
		sbi_printf("M-mode\n");
		તોડો;
	સ્થિતિ PRV_S:
		sbi_printf("S-mode\n");
		તોડો;
	સ્થિતિ PRV_U:
		sbi_printf("U-mode\n");
		તોડો;
	મૂળભૂત:
		sbi_printf("Unknown\n");
		તોડો;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->index, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->index, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

શૂન્ય sbi_domain_dump_all(અચલ અક્ષર *suffix)
{
	અચલ સંરચના sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

પૂર્ણાંક sbi_domain_register(સંરચના sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	સંરચના sbi_domain *tdom;
	પૂર્ણાંક rc;

	/* Sanity checks */
	જો (!dom || domain_finalized)
		પાછા SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		જો (tdom == dom ||
		    tdom->init_order == dom->init_order)
			પાછા SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	જો (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		પાછા rc;
	}

	sbi_list_add_tail(&dom->ગાંઠ, &domain_list);

	/* Assign index to domain */
	dom->index = domain_ગણતરી++;

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
		જો (tdom) {
			જો (tdom->init_order > dom->init_order)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			નહીંતર
				ચાલુ;
		}

		sbi_update_hartindex_to_domain(i, dom);
		sbi_hartmask_set_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		જો (sbi_hartindex_to_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->index, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	જો (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->ગાંઠ);
		પાછા rc;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક મૂળ_add_memregion(અચલ સંરચના sbi_domain_memregion *reg)
{
	પૂર્ણાંક rc;
	bool reg_merged;
	સંરચના sbi_domain_memregion *nreg, *nreg1;
	પૂર્ણાંક મૂળ_memregs_ગણતરી = sbi_domain_used_memregions(&મૂળ);

	/* Sanity checks */
	જો (!reg || domain_finalized || !મૂળ.regions ||
	    (મૂળ_REGION_MAX <= મૂળ_memregs_ગણતરી))
		પાછા SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&મૂળ, nreg) {
		જો (is_region_compatible(reg, nreg))
			પાછા 0;
	}

	/* Append the memregion to root memregions */
	nreg = &મૂળ.regions[મૂળ_memregs_ગણતરી];
	sbi_memcpy(nreg, reg, કદ(*reg));
	મૂળ_memregs_ગણતરી++;
	મૂળ.regions[મૂળ_memregs_ગણતરી].order = 0;

	/* Sort and optimize root regions */
	કરો {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&મૂળ);
		જો (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   મૂળ.name, rc);
			પાછા rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&મૂળ, nreg) {
			nreg1 = nreg + 1;
			જો (!nreg1->order)
				ચાલુ;

			જો (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				પૂર્ણાંક i1 = nreg1 - મૂળ.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    કદ(*nreg1) * (મૂળ_memregs_ગણતરી - i1));
				reg_merged = true;
				મૂળ_memregs_ગણતરી--;
			}
		}
	} જ્યારે (reg_merged);

	પાછા 0;
}

પૂર્ણાંક sbi_domain_મૂળ_add_memrange(unsigned long addr, unsigned long size,
			   unsigned long align, unsigned long region_flags)
{
	પૂર્ણાંક rc;
	unsigned long pos, end, rsize;
	સંરચના sbi_domain_memregion reg;

	pos = addr;
	end = addr + size;
	જ્યારે (pos < end) {
		rsize = pos & (align - 1);
		જો (rsize)
			rsize = 1UL << sbi_ffs(pos);
		નહીંતર
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = મૂળ_add_memregion(&reg);
		જો (rc)
			પાછા rc;
		pos += rsize;
	}

	પાછા 0;
}

પૂર્ણાંક sbi_domain_startup(સંરચના sbi_scratch *scratch, u32 cold_hartid)
{
	પૂર્ણાંક rc;
	u32 dhart;
	સંરચના sbi_domain *dom;

	/* Sanity checks */
	જો (!domain_finalized)
		પાછા SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		જો (!sbi_hartindex_valid(dhart))
			ચાલુ;

		/* Ignore if boot HART not possible for this domain */
		જો (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			ચાલુ;

		/* Ignore if boot HART assigned different domain */
		જો (sbi_hartindex_to_domain(dhart) != dom)
			ચાલુ;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		જો (!rc)
			ચાલુ;

		/* Startup boot HART of domain */
		જો (dom->boot_hartid == cold_hartid) {
			scratch->next_addr = dom->next_addr;
			scratch->next_mode = dom->next_mode;
			scratch->next_arg1 = dom->next_arg1;
		} નહીંતર {
			rc = sbi_hsm_hart_start(scratch, ખાલી,
						dom->boot_hartid,
						dom->next_addr,
						dom->next_mode,
						dom->next_arg1);
			જો (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				પાછા rc;
			}
		}
	}

	પાછા 0;
}

પૂર્ણાંક sbi_domain_finalize(સંરચના sbi_scratch *scratch)
{
	પૂર્ણાંક rc;
	અચલ સંરચના sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	જો (domain_finalized)
		પાછા SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	જો (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		પાછા rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	પાછા 0;
}

પૂર્ણાંક sbi_domain_init(સંરચના sbi_scratch *scratch, u32 cold_hartid)
{
	પૂર્ણાંક rc;
	સંરચના sbi_hartmask *મૂળ_hmask;
	સંરચના sbi_domain_memregion *મૂળ_memregs;
	પૂર્ણાંક મૂળ_memregs_ગણતરી = 0;
	અચલ સંરચના sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_HEAD(&domain_list);

	જો (!fw_single_region) {
		જો (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			પાછા SBI_EINVAL;
		}
		જો ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			પાછા SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(શૂન્ય *);
	જો (!domain_hart_ptr_offset)
		પાછા SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	જો (rc)
		જાઓ fail_free_domain_hart_ptr_offset;

	મૂળ_memregs = sbi_calloc(કદ(*મૂળ_memregs), મૂળ_REGION_MAX + 1);
	જો (!મૂળ_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		જાઓ fail_deinit_context;
	}
	મૂળ.regions = મૂળ_memregs;

	મૂળ_hmask = sbi_zalloc(કદ(*મૂળ_hmask));
	જો (!મૂળ_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		જાઓ fail_free_મૂળ_memregs;
	}
	મૂળ.possible_harts = મૂળ_hmask;

	/* Root domain firmware memory region */
	જો (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_size,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &મૂળ_memregs[મૂળ_memregs_ગણતરી++]);
	} નહીંતર {
		sbi_domain_memregion_init(scratch->fw_start,
					  scratch->fw_rw_offset,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &મૂળ_memregs[મૂળ_memregs_ગણતરી++]);

		sbi_domain_memregion_init((scratch->fw_start +
					   scratch->fw_rw_offset),
					  (scratch->fw_size -
					   scratch->fw_rw_offset),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &મૂળ_memregs[મૂળ_memregs_ગણતરી++]);
	}

	મૂળ.fw_region_inited = true;

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
				  &મૂળ_memregs[મૂળ_memregs_ગણતરી++]);

	/* Root domain memory region end */
	મૂળ_memregs[મૂળ_memregs_ગણતરી].order = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	મૂળ.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	મૂળ.next_arg1 = scratch->next_arg1;
	મૂળ.next_addr = scratch->next_addr;
	મૂળ.next_mode = scratch->next_mode;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_set_hartindex(i, મૂળ_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_register(&મૂળ);
	જો (rc)
		જાઓ fail_free_મૂળ_hmask;

	પાછા 0;

fail_free_મૂળ_hmask:
	sbi_free(મૂળ_hmask);
fail_free_મૂળ_memregs:
	sbi_free(મૂળ_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	પાછા rc;
}
