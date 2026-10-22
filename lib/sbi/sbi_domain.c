/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hartmask.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_list.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>

SBI_LIST_ਸਿਰ(domain_list);

ਸਥਿਰ u32 domain_ਗਿਣਤੀ = 0;
ਸਥਿਰ bool domain_finalized = false;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਜੜ੍ਹ_REGION_MAX	32

ਬਣਤਰ sbi_domain ਜੜ੍ਹ = {
	.name = "root",
	.init_ਕ੍ਰਮ = -1U,
	.possible_harts = ਨਿਰਾ,
	.regions = ਨਿਰਾ,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ domain_hart_ptr_ਵਿੱਥ;

ਬਣਤਰ sbi_domain *sbi_hartindex_ਮੰਜ਼ਿਲ_domain(u32 hartindex)
{
	ਬਣਤਰ sbi_scratch *scratch;

	scratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(hartindex);
	ਜੇ (!scratch || !domain_hart_ptr_ਵਿੱਥ)
		ਵਾਪਸ ਨਿਰਾ;

	ਵਾਪਸ sbi_scratch_read_type(scratch, ਖਾਲੀ *, domain_hart_ptr_ਵਿੱਥ);
}

ਖਾਲੀ sbi_update_hartindex_ਮੰਜ਼ਿਲ_domain(u32 hartindex, ਬਣਤਰ sbi_domain *dom)
{
	ਬਣਤਰ sbi_scratch *scratch;

	scratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(hartindex);
	ਜੇ (!scratch)
		ਵਾਪਸ;

	sbi_scratch_write_type(scratch, ਖਾਲੀ *, domain_hart_ptr_ਵਿੱਥ, dom);
}

bool sbi_domain_is_assigned_hart(ਅਚਲ ਬਣਤਰ sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_ਤਾਲਾ)
{
	bool ret;
	ਬਣਤਰ sbi_domain *tdom = (ਬਣਤਰ sbi_domain *)dom;

	ਜੇ (!dom)
		ਵਾਪਸ false;

	spin_ਤਾਲਾ(&tdom->assigned_harts_ਤਾਲਾ);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_ਤਾਲਾ);

	ਵਾਪਸ ret;
}

ਪੂਰਨ sbi_domain_get_assigned_hartmask(ਅਚਲ ਬਣਤਰ sbi_domain *dom,
				     ਬਣਤਰ sbi_hartmask *ਨਕਾਬ)
	MUST_NOT_HOLD(&dom->assigned_harts_ਤਾਲਾ)
{
	ulong ret = 0;
	ਬਣਤਰ sbi_domain *tdom = (ਬਣਤਰ sbi_domain *)dom;

	ਜੇ (!dom) {
		sbi_hartmask_clear_all(ਨਕਾਬ);
		ਵਾਪਸ 0;
	}

	spin_ਤਾਲਾ(&tdom->assigned_harts_ਤਾਲਾ);
	sbi_hartmask_copy(ਨਕਾਬ, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_ਤਾਲਾ);

	ਵਾਪਸ ret;
}

ਖਾਲੀ sbi_domain_memregion_init(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags,
				ਬਣਤਰ sbi_domain_memregion *reg)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2 = 0, ਕ੍ਰਮ;

	ਲਈ (ਕ੍ਰਮ = log2roundup(ਮਾਪ) ; ਕ੍ਰਮ <= __riscv_xlen; ਕ੍ਰਮ++) {
		ਜੇ (ਕ੍ਰਮ < __riscv_xlen) {
			ਅਧਾਰ_2 = addr & ~((1UL << ਕ੍ਰਮ) - 1UL);
			ਜੇ ((ਅਧਾਰ_2 <= addr) &&
			    (addr < (ਅਧਾਰ_2 + (1UL << ਕ੍ਰਮ))) &&
			    (ਅਧਾਰ_2 <= (addr + ਮਾਪ - 1UL)) &&
			    ((addr + ਮਾਪ - 1UL) < (ਅਧਾਰ_2 + (1UL << ਕ੍ਰਮ))))
				ਰੋਕੋ;
		} ਨਹੀਂਤਾਂ {
			ਅਧਾਰ_2 = 0;
			ਰੋਕੋ;
		}

	}

	ਜੇ (reg) {
		reg->ਅਧਾਰ_2 = ਅਧਾਰ_2;
		reg->ਕ੍ਰਮ = ਕ੍ਰਮ;
		reg->flags = flags;
	}
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ sbi_domain_get_oldpmp_flags(ਬਣਤਰ sbi_domain_memregion *reg)
{

	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	ਵਾਪਸ pmp_flags;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ sbi_domain_get_smepmp_flags(ਬਣਤਰ sbi_domain_memregion *reg)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_flags = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ rstart, rend;

	ਜੇ ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_ਨਕਾਬ) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		ਵਾਪਸ 0;
	} ਨਹੀਂਤਾਂ ਜੇ (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		ਜੇ (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		ਨਹੀਂਤਾਂ ਜੇ (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		ਨਹੀਂਤਾਂ ਜੇ (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		ਨਹੀਂਤਾਂ ਜੇ (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		ਨਹੀਂਤਾਂ ਜੇ (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} ਨਹੀਂਤਾਂ ਜੇ (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		ਜੇ ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_ਨਕਾਬ)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			ਵਾਪਸ 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} ਨਹੀਂਤਾਂ ਜੇ (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} ਨਹੀਂਤਾਂ {
		rstart = reg->ਅਧਾਰ_2;
		rend = (reg->ਕ੍ਰਮ < __riscv_xlen) ? rstart + ((1UL << reg->ਕ੍ਰਮ) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	ਵਾਪਸ pmp_flags;
}

bool sbi_domain_check_addr(ਅਚਲ ਬਣਤਰ sbi_domain *dom,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਬਹੁਲਕ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ access_flags)
{
	bool rmmio, mmio = false;
	ਬਣਤਰ sbi_domain_memregion *reg;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ rstart, rend, rflags, rwx = 0, rrwx = 0;

	ਜੇ (!dom)
		ਵਾਪਸ false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	ਜੇ (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	ਜੇ (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	ਜੇ (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	ਜੇ (access_flags & SBI_DOMAIN_MMIO)
		mmio = true;

	sbi_domain_for_each_memregion(dom, reg) {
		rflags = reg->flags;
		rrwx = (ਬਹੁਲਕ == PRV_M ?
			(rflags & SBI_DOMAIN_MEMREGION_M_ACCESS_ਨਕਾਬ) :
			(rflags & SBI_DOMAIN_MEMREGION_SU_ACCESS_ਨਕਾਬ)
			>> SBI_DOMAIN_MEMREGION_SU_ACCESS_ਖਿਸਕਾਓ);

		rstart = reg->ਅਧਾਰ_2;
		rend = (reg->ਕ੍ਰਮ < __riscv_xlen) ?
			rstart + ((1UL << reg->ਕ੍ਰਮ) - 1) : -1UL;
		ਜੇ (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			ਜੇ (!mmio && rmmio)
				ਵਾਪਸ false;
			ਵਾਪਸ ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	ਵਾਪਸ (ਬਹੁਲਕ == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
ਸਥਿਰ bool is_region_valid(ਅਚਲ ਬਣਤਰ sbi_domain_memregion *reg)
{
	ਜੇ (reg->ਕ੍ਰਮ < 3 || __riscv_xlen < reg->ਕ੍ਰਮ)
		ਵਾਪਸ false;

	ਜੇ (reg->ਕ੍ਰਮ == __riscv_xlen && reg->ਅਧਾਰ_2 != 0)
		ਵਾਪਸ false;

	ਜੇ (reg->ਕ੍ਰਮ < __riscv_xlen && (reg->ਅਧਾਰ_2 & (ਦੋਆਧਾਰੀ_ਅੰਕ(reg->ਕ੍ਰਮ) - 1)))
		ਵਾਪਸ false;

	ਵਾਪਸ true;
}

/** Check if regionA can be replaced by regionB */
ਸਥਿਰ bool is_region_compatible(ਅਚਲ ਬਣਤਰ sbi_domain_memregion *regA,
				 ਅਚਲ ਬਣਤਰ sbi_domain_memregion *regB)
{
	ਜੇ (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		ਵਾਪਸ true;

	ਵਾਪਸ false;
}

/** Check if regionA should be placed before regionB */
ਸਥਿਰ bool is_region_before(ਅਚਲ ਬਣਤਰ sbi_domain_memregion *regA,
			     ਅਚਲ ਬਣਤਰ sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	ਜੇ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		ਵਾਪਸ true;
	ਜੇ (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		ਵਾਪਸ false;

	ਜੇ (regA->ਕ੍ਰਮ < regB->ਕ੍ਰਮ)
		ਵਾਪਸ true;

	ਜੇ ((regA->ਕ੍ਰਮ == regB->ਕ੍ਰਮ) &&
	    (regA->ਅਧਾਰ_2 < regB->ਅਧਾਰ_2))
		ਵਾਪਸ true;

	ਵਾਪਸ false;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_domain_memregion *ਲੱਭੋ_region(
						ਅਚਲ ਬਣਤਰ sbi_domain *dom,
						ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ rstart, rend;
	ਬਣਤਰ sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->ਅਧਾਰ_2;
		rend = (reg->ਕ੍ਰਮ < __riscv_xlen) ?
			rstart + ((1UL << reg->ਕ੍ਰਮ) - 1) : -1UL;
		ਜੇ (rstart <= addr && addr <= rend)
			ਵਾਪਸ reg;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_domain_memregion *ਲੱਭੋ_ਅਗਲਾ_subset_region(
				ਅਚਲ ਬਣਤਰ sbi_domain *dom,
				ਅਚਲ ਬਣਤਰ sbi_domain_memregion *reg,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr)
{
	ਬਣਤਰ sbi_domain_memregion *sreg, *ret = ਨਿਰਾ;

	sbi_domain_for_each_memregion(dom, sreg) {
		ਜੇ (sreg == reg || (sreg->ਅਧਾਰ_2 <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			ਜਾਰੀ;

		ਜੇ (!ret || (sreg->ਅਧਾਰ_2 < ret->ਅਧਾਰ_2) ||
		    ((sreg->ਅਧਾਰ_2 == ret->ਅਧਾਰ_2) && (sreg->ਕ੍ਰਮ < ret->ਕ੍ਰਮ)))
			ret = sreg;
	}

	ਵਾਪਸ ret;
}

ਸਥਿਰ ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ_region(ਬਣਤਰ sbi_domain_memregion* reg1,
			ਬਣਤਰ sbi_domain_memregion* reg2)
{
	ਬਣਤਰ sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, ਆਕਾਰ(treg));
	sbi_memcpy(reg1, reg2, ਆਕਾਰ(treg));
	sbi_memcpy(reg2, &treg, ਆਕਾਰ(treg));
}

ਸਥਿਰ ਪੂਰਨ sbi_domain_ਵਰਤਿਆ_memregions(ਅਚਲ ਬਣਤਰ sbi_domain *dom)
{
	ਪੂਰਨ ਗਿਣਤੀ_2 = 0;
	ਬਣਤਰ sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		ਗਿਣਤੀ_2++;

	ਵਾਪਸ ਗਿਣਤੀ_2;
}

ਸਥਿਰ ਪੂਰਨ sanitize_domain(ਬਣਤਰ sbi_domain *dom)
{
	u32 i, j, ਗਿਣਤੀ_2;
	bool is_covered;
	ਬਣਤਰ sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	ਜੇ (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		ਵਾਪਸ SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ਜੇ (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_ਮੰਜ਼ਿਲ_hartid(i));
			ਵਾਪਸ SBI_EINVAL;
		}
	}

	/* Check memory regions */
	ਜੇ (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		ਵਾਪਸ SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		ਜੇ (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->ਅਧਾਰ_2, reg->ਕ੍ਰਮ,
				   reg->flags);
			ਵਾਪਸ SBI_EINVAL;
		}
	}

	/* Count memory regions */
	ਗਿਣਤੀ_2 = sbi_domain_ਵਰਤਿਆ_memregions(dom);

	/* Check presence of firmware regions */
	ਜੇ (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		ਵਾਪਸ SBI_EINVAL;
	}

	/* Sort the memory regions */
	ਲਈ (i = 0; i < (ਗਿਣਤੀ_2 - 1); i++) {
		reg = &dom->regions[i];
		ਲਈ (j = i + 1; j < ਗਿਣਤੀ_2; j++) {
			reg1 = &dom->regions[j];

			ਜੇ (!is_region_before(reg1, reg))
				ਜਾਰੀ;

			ਅਦਲਾ_ਬਦਲੀ_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	ਲਈ (i = 0; i < (ਗਿਣਤੀ_2 - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		ਲਈ (j = i + 1; j < ਗਿਣਤੀ_2; j++) {
			reg1 = &dom->regions[j];

			ਜੇ (is_region_compatible(reg, reg1)) {
				is_covered = true;
				ਰੋਕੋ;
			}
		}

		/* find a region is superset of reg, remove reg */
		ਜੇ (is_covered) {
			sbi_memmove(reg, reg + 1, ਆਕਾਰ(*reg) * (ਗਿਣਤੀ_2 - i));
			ਗਿਣਤੀ_2--;
		} ਨਹੀਂਤਾਂ
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
	ਜੇ (dom->ਅਗਲਾ_ਬਹੁਲਕ != PRV_S &&
	    dom->ਅਗਲਾ_ਬਹੁਲਕ != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->ਅਗਲਾ_ਬਹੁਲਕ);
		ਵਾਪਸ SBI_EINVAL;
	}

	/* Check next address and next mode */
	ਜੇ (!sbi_domain_check_addr(dom, dom->ਅਗਲਾ_addr, dom->ਅਗਲਾ_ਬਹੁਲਕ,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->ਅਗਲਾ_addr);
		ਵਾਪਸ SBI_EINVAL;
	}

	ਵਾਪਸ 0;
}

bool sbi_domain_check_addr_range(ਅਚਲ ਬਣਤਰ sbi_domain *dom,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਬਹੁਲਕ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ access_flags)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ max = addr + ਮਾਪ;
	ਅਚਲ ਬਣਤਰ sbi_domain_memregion *reg, *sreg;

	ਜੇ (!dom)
		ਵਾਪਸ false;

	ਜੇ (ਮਾਪ && max <= addr)
		ਵਾਪਸ false;

	ਜਦੋਂ (addr < max) {
		reg = ਲੱਭੋ_region(dom, addr);
		ਜੇ (!reg)
			ਵਾਪਸ false;

		ਜੇ (!sbi_domain_check_addr(dom, addr, ਬਹੁਲਕ, access_flags))
			ਵਾਪਸ false;

		sreg = ਲੱਭੋ_ਅਗਲਾ_subset_region(dom, reg, addr);
		ਜੇ (sreg)
			addr = sreg->ਅਧਾਰ_2;
		ਨਹੀਂਤਾਂ ਜੇ (reg->ਕ੍ਰਮ < __riscv_xlen)
			addr = reg->ਅਧਾਰ_2 + (1UL << reg->ਕ੍ਰਮ);
		ਨਹੀਂਤਾਂ
			ਰੋਕੋ;
	}

	ਵਾਪਸ true;
}

ਖਾਲੀ sbi_domain_dump(ਅਚਲ ਬਣਤਰ sbi_domain *dom, ਅਚਲ ਅੱਖਰ *suffix)
{
	u32 i, j, k;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ rstart, rend;
	ਬਣਤਰ sbi_domain_memregion *reg;

	sbi_printf("Domain%d Name        %s: %s\n",
		   dom->ਸੂਚਕਾਂਕ_2, suffix, dom->name);

	sbi_printf("Domain%d Init Order  %s: 0x%x\n",
		   dom->ਸੂਚਕਾਂਕ_2, suffix, dom->init_ਕ੍ਰਮ);

	sbi_printf("Domain%d Boot HART   %s: %d\n",
		   dom->ਸੂਚਕਾਂਕ_2, suffix, dom->boot_hartid);

	k = 0;
	sbi_printf("Domain%d HARTs       %s: ", dom->ਸੂਚਕਾਂਕ_2, suffix);
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		j = sbi_hartindex_ਮੰਜ਼ਿਲ_hartid(i);
		sbi_printf("%s0x%x%s", (k++) ? "," : "",
			   j, sbi_domain_is_assigned_hart(dom, i) ? "*" : "");
	}
	sbi_printf("\n");

	i = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->ਅਧਾਰ_2;
		rend = (reg->ਕ੍ਰਮ < __riscv_xlen) ?
			rstart + ((1UL << reg->ਕ੍ਰਮ) - 1) : -1UL;

		sbi_printf("Domain%d Region%02d    %s: 0x%" PRILX "-0x%" PRILX " ",
			   dom->ਸੂਚਕਾਂਕ_2, i, suffix, rstart, rend);

		k = 0;

		sbi_printf("M: ");
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->ਸੂਚਕਾਂਕ_2, suffix, dom->ਅਗਲਾ_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->ਸੂਚਕਾਂਕ_2, suffix, dom->ਅਗਲਾ_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->ਸੂਚਕਾਂਕ_2, suffix);
	ਚੋਣ (dom->ਅਗਲਾ_ਬਹੁਲਕ) {
	ਹਾਲਤ PRV_M:
		sbi_printf("M-mode\n");
		ਰੋਕੋ;
	ਹਾਲਤ PRV_S:
		sbi_printf("S-mode\n");
		ਰੋਕੋ;
	ਹਾਲਤ PRV_U:
		sbi_printf("U-mode\n");
		ਰੋਕੋ;
	ਮੂਲ:
		sbi_printf("Unknown\n");
		ਰੋਕੋ;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->ਸੂਚਕਾਂਕ_2, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->ਸੂਚਕਾਂਕ_2, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

ਖਾਲੀ sbi_domain_dump_all(ਅਚਲ ਅੱਖਰ *suffix)
{
	ਅਚਲ ਬਣਤਰ sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

ਪੂਰਨ sbi_domain_ਰਜਿਸਟਰ(ਬਣਤਰ sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	ਬਣਤਰ sbi_domain *tdom;
	ਪੂਰਨ rc;

	/* Sanity checks */
	ਜੇ (!dom || domain_finalized)
		ਵਾਪਸ SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		ਜੇ (tdom == dom ||
		    tdom->init_ਕ੍ਰਮ == dom->init_ਕ੍ਰਮ)
			ਵਾਪਸ SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	ਜੇ (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		ਵਾਪਸ rc;
	}

	sbi_list_ਜੋੜੋ_ਪੂਛ(&dom->ਗੰਢ, &domain_list);

	/* Assign index to domain */
	dom->ਸੂਚਕਾਂਕ_2 = domain_ਗਿਣਤੀ++;

	/* Initialize spinlock for dom->assigned_harts */
	SPIN_ਤਾਲਾ_INIT(dom->assigned_harts_ਤਾਲਾ);

	/* Clear assigned HARTs of domain */
	sbi_hartmask_clear_all(&dom->assigned_harts);

	/*
	 * Assign HART to a domain with the least initialization order
	 * where the HART is listed as a possible HART of the domain.
	 */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		tdom = sbi_hartindex_ਮੰਜ਼ਿਲ_domain(i);
		ਜੇ (tdom) {
			ਜੇ (tdom->init_ਕ੍ਰਮ > dom->init_ਕ੍ਰਮ)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			ਨਹੀਂਤਾਂ
				ਜਾਰੀ;
		}

		sbi_update_hartindex_ਮੰਜ਼ਿਲ_domain(i, dom);
		sbi_hartmask_ਸੈੱਟ_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		ਜੇ (sbi_hartindex_ਮੰਜ਼ਿਲ_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->ਸੂਚਕਾਂਕ_2, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_ਤਿਆਰੀ_state(dom);
	ਜੇ (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->ਗੰਢ);
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ ਜੜ੍ਹ_ਜੋੜੋ_memregion(ਅਚਲ ਬਣਤਰ sbi_domain_memregion *reg)
{
	ਪੂਰਨ rc;
	bool reg_merged;
	ਬਣਤਰ sbi_domain_memregion *nreg, *nreg1;
	ਪੂਰਨ ਜੜ੍ਹ_memregs_ਗਿਣਤੀ = sbi_domain_ਵਰਤਿਆ_memregions(&ਜੜ੍ਹ);

	/* Sanity checks */
	ਜੇ (!reg || domain_finalized || !ਜੜ੍ਹ.regions ||
	    (ਜੜ੍ਹ_REGION_MAX <= ਜੜ੍ਹ_memregs_ਗਿਣਤੀ))
		ਵਾਪਸ SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&ਜੜ੍ਹ, nreg) {
		ਜੇ (is_region_compatible(reg, nreg))
			ਵਾਪਸ 0;
	}

	/* Append the memregion to root memregions */
	nreg = &ਜੜ੍ਹ.regions[ਜੜ੍ਹ_memregs_ਗਿਣਤੀ];
	sbi_memcpy(nreg, reg, ਆਕਾਰ(*reg));
	ਜੜ੍ਹ_memregs_ਗਿਣਤੀ++;
	ਜੜ੍ਹ.regions[ਜੜ੍ਹ_memregs_ਗਿਣਤੀ].ਕ੍ਰਮ = 0;

	/* Sort and optimize root regions */
	ਕਰੋ {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&ਜੜ੍ਹ);
		ਜੇ (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   ਜੜ੍ਹ.name, rc);
			ਵਾਪਸ rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&ਜੜ੍ਹ, nreg) {
			nreg1 = nreg + 1;
			ਜੇ (!nreg1->ਕ੍ਰਮ)
				ਜਾਰੀ;

			ਜੇ (!(nreg->ਅਧਾਰ_2 & (ਦੋਆਧਾਰੀ_ਅੰਕ(nreg->ਕ੍ਰਮ + 1) - 1)) &&
			    (nreg->ਅਧਾਰ_2 + ਦੋਆਧਾਰੀ_ਅੰਕ(nreg->ਕ੍ਰਮ)) == nreg1->ਅਧਾਰ_2 &&
			    nreg->ਕ੍ਰਮ == nreg1->ਕ੍ਰਮ &&
			    nreg->flags == nreg1->flags) {
				ਪੂਰਨ i1 = nreg1 - ਜੜ੍ਹ.regions;
				nreg->ਕ੍ਰਮ++;
				sbi_memmove(nreg1, nreg1 + 1,
					    ਆਕਾਰ(*nreg1) * (ਜੜ੍ਹ_memregs_ਗਿਣਤੀ - i1));
				reg_merged = true;
				ਜੜ੍ਹ_memregs_ਗਿਣਤੀ--;
			}
		}
	} ਜਦੋਂ (reg_merged);

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ align, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ region_flags)
{
	ਪੂਰਨ rc;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ pos, ਸਿਰਾ, rsize;
	ਬਣਤਰ sbi_domain_memregion reg;

	pos = addr;
	ਸਿਰਾ = addr + ਮਾਪ;
	ਜਦੋਂ (pos < ਸਿਰਾ) {
		rsize = pos & (align - 1);
		ਜੇ (rsize)
			rsize = 1UL << sbi_ffs(pos);
		ਨਹੀਂਤਾਂ
			rsize = ((ਸਿਰਾ - pos) < align) ?
				(ਸਿਰਾ - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = ਜੜ੍ਹ_ਜੋੜੋ_memregion(&reg);
		ਜੇ (rc)
			ਵਾਪਸ rc;
		pos += rsize;
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_domain_startup(ਬਣਤਰ sbi_scratch *scratch, u32 cold_hartid)
{
	ਪੂਰਨ rc;
	u32 dhart;
	ਬਣਤਰ sbi_domain *dom;

	/* Sanity checks */
	ਜੇ (!domain_finalized)
		ਵਾਪਸ SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_ਮੰਜ਼ਿਲ_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		ਜੇ (!sbi_hartindex_valid(dhart))
			ਜਾਰੀ;

		/* Ignore if boot HART not possible for this domain */
		ਜੇ (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			ਜਾਰੀ;

		/* Ignore if boot HART assigned different domain */
		ਜੇ (sbi_hartindex_ਮੰਜ਼ਿਲ_domain(dhart) != dom)
			ਜਾਰੀ;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_ਤਾਲਾ(&dom->assigned_harts_ਤਾਲਾ);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_ਤਾਲਾ);
		ਜੇ (!rc)
			ਜਾਰੀ;

		/* Startup boot HART of domain */
		ਜੇ (dom->boot_hartid == cold_hartid) {
			scratch->ਅਗਲਾ_addr = dom->ਅਗਲਾ_addr;
			scratch->ਅਗਲਾ_ਬਹੁਲਕ = dom->ਅਗਲਾ_ਬਹੁਲਕ;
			scratch->ਅਗਲਾ_arg1 = dom->ਅਗਲਾ_arg1;
		} ਨਹੀਂਤਾਂ {
			rc = sbi_hsm_hart_ਸ਼ੁਰੂ(scratch, ਨਿਰਾ,
						dom->boot_hartid,
						dom->ਅਗਲਾ_addr,
						dom->ਅਗਲਾ_ਬਹੁਲਕ,
						dom->ਅਗਲਾ_arg1);
			ਜੇ (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				ਵਾਪਸ rc;
			}
		}
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_domain_finalize(ਬਣਤਰ sbi_scratch *scratch)
{
	ਪੂਰਨ rc;
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	ਜੇ (domain_finalized)
		ਵਾਪਸ SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	ਜੇ (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		ਵਾਪਸ rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_domain_init(ਬਣਤਰ sbi_scratch *scratch, u32 cold_hartid)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ sbi_hartmask *ਜੜ੍ਹ_hmask;
	ਬਣਤਰ sbi_domain_memregion *ਜੜ੍ਹ_memregs;
	ਪੂਰਨ ਜੜ੍ਹ_memregs_ਗਿਣਤੀ = 0;
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_ਸਿਰ(&domain_list);

	ਜੇ (!fw_single_region) {
		ਜੇ (scratch->fw_rw_ਵਿੱਥ == 0 ||
		    (scratch->fw_rw_ਵਿੱਥ & (scratch->fw_rw_ਵਿੱਥ - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_ਵਿੱਥ);
			ਵਾਪਸ SBI_EINVAL;
		}
		ਜੇ ((scratch->fw_ਸ਼ੁਰੂ & (scratch->fw_rw_ਵਿੱਥ - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			ਵਾਪਸ SBI_EINVAL;
		}
	}

	domain_hart_ptr_ਵਿੱਥ = sbi_scratch_alloc_type_ਵਿੱਥ(ਖਾਲੀ *);
	ਜੇ (!domain_hart_ptr_ਵਿੱਥ)
		ਵਾਪਸ SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	ਜੇ (rc)
		ਜਾਓ fail_free_domain_hart_ptr_ਵਿੱਥ;

	ਜੜ੍ਹ_memregs = sbi_calloc(ਆਕਾਰ(*ਜੜ੍ਹ_memregs), ਜੜ੍ਹ_REGION_MAX + 1);
	ਜੇ (!ਜੜ੍ਹ_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		ਜਾਓ fail_deinit_context;
	}
	ਜੜ੍ਹ.regions = ਜੜ੍ਹ_memregs;

	ਜੜ੍ਹ_hmask = sbi_zalloc(ਆਕਾਰ(*ਜੜ੍ਹ_hmask));
	ਜੇ (!ਜੜ੍ਹ_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		ਜਾਓ fail_free_ਜੜ੍ਹ_memregs;
	}
	ਜੜ੍ਹ.possible_harts = ਜੜ੍ਹ_hmask;

	/* Root domain firmware memory region */
	ਜੇ (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_ਸ਼ੁਰੂ, scratch->fw_ਮਾਪ,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ਜੜ੍ਹ_memregs[ਜੜ੍ਹ_memregs_ਗਿਣਤੀ++]);
	} ਨਹੀਂਤਾਂ {
		sbi_domain_memregion_init(scratch->fw_ਸ਼ੁਰੂ,
					  scratch->fw_rw_ਵਿੱਥ,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ਜੜ੍ਹ_memregs[ਜੜ੍ਹ_memregs_ਗਿਣਤੀ++]);

		sbi_domain_memregion_init((scratch->fw_ਸ਼ੁਰੂ +
					   scratch->fw_rw_ਵਿੱਥ),
					  (scratch->fw_ਮਾਪ -
					   scratch->fw_rw_ਵਿੱਥ),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ਜੜ੍ਹ_memregs[ਜੜ੍ਹ_memregs_ਗਿਣਤੀ++]);
	}

	ਜੜ੍ਹ.fw_region_inited = true;

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
				  &ਜੜ੍ਹ_memregs[ਜੜ੍ਹ_memregs_ਗਿਣਤੀ++]);

	/* Root domain memory region end */
	ਜੜ੍ਹ_memregs[ਜੜ੍ਹ_memregs_ਗਿਣਤੀ].ਕ੍ਰਮ = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	ਜੜ੍ਹ.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	ਜੜ੍ਹ.ਅਗਲਾ_arg1 = scratch->ਅਗਲਾ_arg1;
	ਜੜ੍ਹ.ਅਗਲਾ_addr = scratch->ਅਗਲਾ_addr;
	ਜੜ੍ਹ.ਅਗਲਾ_ਬਹੁਲਕ = scratch->ਅਗਲਾ_ਬਹੁਲਕ;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_ਸੈੱਟ_hartindex(i, ਜੜ੍ਹ_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_ਰਜਿਸਟਰ(&ਜੜ੍ਹ);
	ਜੇ (rc)
		ਜਾਓ fail_free_ਜੜ੍ਹ_hmask;

	ਵਾਪਸ 0;

fail_free_ਜੜ੍ਹ_hmask:
	sbi_free(ਜੜ੍ਹ_hmask);
fail_free_ਜੜ੍ਹ_memregs:
	sbi_free(ਜੜ੍ਹ_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_ਵਿੱਥ:
	sbi_scratch_free_ਵਿੱਥ(domain_hart_ptr_ਵਿੱਥ);
	ਵਾਪਸ rc;
}
