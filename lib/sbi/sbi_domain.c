/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hartmask.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_list.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_math.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>

SBI_LIST_ತಲೆ(domain_list);

ಸ್ಥಿರ u32 domain_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ bool domain_finalized = false;

#ವ್ಯಾಖ್ಯಾನಿಸು ಬೇರು_REGION_MAX	32

ರಚನೆ sbi_domain ಬೇರು = {
	.name = "root",
	.init_ಕ್ರಮ = -1U,
	.possible_harts = ಖಾಲಿ,
	.regions = ಖಾಲಿ,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ domain_hart_ptr_ಸ್ಥಾನಾಂತರ;

ರಚನೆ sbi_domain *sbi_hartindex_ಗಮ್ಯ_domain(u32 hartindex)
{
	ರಚನೆ sbi_scratch *scratch;

	scratch = sbi_hartindex_ಗಮ್ಯ_scratch(hartindex);
	ಆದರೆ (!scratch || !domain_hart_ptr_ಸ್ಥಾನಾಂತರ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ sbi_scratch_read_type(scratch, ಶೂನ್ಯ *, domain_hart_ptr_ಸ್ಥಾನಾಂತರ);
}

ಶೂನ್ಯ sbi_update_hartindex_ಗಮ್ಯ_domain(u32 hartindex, ರಚನೆ sbi_domain *dom)
{
	ರಚನೆ sbi_scratch *scratch;

	scratch = sbi_hartindex_ಗಮ್ಯ_scratch(hartindex);
	ಆದರೆ (!scratch)
		ಹಿಂತಿರುಗಿ;

	sbi_scratch_write_type(scratch, ಶೂನ್ಯ *, domain_hart_ptr_ಸ್ಥಾನಾಂತರ, dom);
}

bool sbi_domain_is_assigned_hart(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_ಬೀಗ)
{
	bool ret;
	ರಚನೆ sbi_domain *tdom = (ರಚನೆ sbi_domain *)dom;

	ಆದರೆ (!dom)
		ಹಿಂತಿರುಗಿ false;

	spin_ಬೀಗ(&tdom->assigned_harts_ಬೀಗ);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_ಬೀಗ);

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ sbi_domain_get_assigned_hartmask(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom,
				     ರಚನೆ sbi_hartmask *ಮುಖವಾಡ)
	MUST_NOT_HOLD(&dom->assigned_harts_ಬೀಗ)
{
	ulong ret = 0;
	ರಚನೆ sbi_domain *tdom = (ರಚನೆ sbi_domain *)dom;

	ಆದರೆ (!dom) {
		sbi_hartmask_clear_all(ಮುಖವಾಡ);
		ಹಿಂತಿರುಗಿ 0;
	}

	spin_ಬೀಗ(&tdom->assigned_harts_ಬೀಗ);
	sbi_hartmask_copy(ಮುಖವಾಡ, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_ಬೀಗ);

	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ sbi_domain_memregion_init(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr,
				ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2,
				ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags,
				ರಚನೆ sbi_domain_memregion *reg)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2 = 0, ಕ್ರಮ;

	ಗಾಗಿ (ಕ್ರಮ = log2roundup(ಗಾತ್ರ_2) ; ಕ್ರಮ <= __riscv_xlen; ಕ್ರಮ++) {
		ಆದರೆ (ಕ್ರಮ < __riscv_xlen) {
			ತಳ_2 = addr & ~((1UL << ಕ್ರಮ) - 1UL);
			ಆದರೆ ((ತಳ_2 <= addr) &&
			    (addr < (ತಳ_2 + (1UL << ಕ್ರಮ))) &&
			    (ತಳ_2 <= (addr + ಗಾತ್ರ_2 - 1UL)) &&
			    ((addr + ಗಾತ್ರ_2 - 1UL) < (ತಳ_2 + (1UL << ಕ್ರಮ))))
				ನಿಲ್ಲಿಸು;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ತಳ_2 = 0;
			ನಿಲ್ಲಿಸು;
		}

	}

	ಆದರೆ (reg) {
		reg->ತಳ_2 = ತಳ_2;
		reg->ಕ್ರಮ = ಕ್ರಮ;
		reg->flags = flags;
	}
}

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ sbi_domain_get_oldpmp_flags(ರಚನೆ sbi_domain_memregion *reg)
{

	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	ಹಿಂತಿರುಗಿ pmp_flags;
}

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ sbi_domain_get_smepmp_flags(ರಚನೆ sbi_domain_memregion *reg)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_flags = 0;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ rstart, rend;

	ಆದರೆ ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_ಮುಖವಾಡ) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		ಹಿಂತಿರುಗಿ 0;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		ಆದರೆ (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		ಆದರೆ ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_ಮುಖವಾಡ)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			ಹಿಂತಿರುಗಿ 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} ಇಲ್ಲದಿದ್ದರೆ {
		rstart = reg->ತಳ_2;
		rend = (reg->ಕ್ರಮ < __riscv_xlen) ? rstart + ((1UL << reg->ಕ್ರಮ) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	ಹಿಂತಿರುಗಿ pmp_flags;
}

bool sbi_domain_check_addr(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಬಹುಲಕ,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ access_flags)
{
	bool rmmio, mmio = false;
	ರಚನೆ sbi_domain_memregion *reg;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ rstart, rend, rflags, rwx = 0, rrwx = 0;

	ಆದರೆ (!dom)
		ಹಿಂತಿರುಗಿ false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	ಆದರೆ (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	ಆದರೆ (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	ಆದರೆ (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	ಆದರೆ (access_flags & SBI_DOMAIN_MMIO)
		mmio = true;

	sbi_domain_for_each_memregion(dom, reg) {
		rflags = reg->flags;
		rrwx = (ಬಹುಲಕ == PRV_M ?
			(rflags & SBI_DOMAIN_MEMREGION_M_ACCESS_ಮುಖವಾಡ) :
			(rflags & SBI_DOMAIN_MEMREGION_SU_ACCESS_ಮುಖವಾಡ)
			>> SBI_DOMAIN_MEMREGION_SU_ACCESS_ಸರಿಸು);

		rstart = reg->ತಳ_2;
		rend = (reg->ಕ್ರಮ < __riscv_xlen) ?
			rstart + ((1UL << reg->ಕ್ರಮ) - 1) : -1UL;
		ಆದರೆ (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			ಆದರೆ (!mmio && rmmio)
				ಹಿಂತಿರುಗಿ false;
			ಹಿಂತಿರುಗಿ ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	ಹಿಂತಿರುಗಿ (ಬಹುಲಕ == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
ಸ್ಥಿರ bool is_region_valid(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain_memregion *reg)
{
	ಆದರೆ (reg->ಕ್ರಮ < 3 || __riscv_xlen < reg->ಕ್ರಮ)
		ಹಿಂತಿರುಗಿ false;

	ಆದರೆ (reg->ಕ್ರಮ == __riscv_xlen && reg->ತಳ_2 != 0)
		ಹಿಂತಿರುಗಿ false;

	ಆದರೆ (reg->ಕ್ರಮ < __riscv_xlen && (reg->ತಳ_2 & (ಬಿಟ್(reg->ಕ್ರಮ) - 1)))
		ಹಿಂತಿರುಗಿ false;

	ಹಿಂತಿರುಗಿ true;
}

/** Check if regionA can be replaced by regionB */
ಸ್ಥಿರ bool is_region_compatible(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain_memregion *regA,
				 ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain_memregion *regB)
{
	ಆದರೆ (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		ಹಿಂತಿರುಗಿ true;

	ಹಿಂತಿರುಗಿ false;
}

/** Check if regionA should be placed before regionB */
ಸ್ಥಿರ bool is_region_before(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain_memregion *regA,
			     ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	ಆದರೆ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		ಹಿಂತಿರುಗಿ true;
	ಆದರೆ (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		ಹಿಂತಿರುಗಿ false;

	ಆದರೆ (regA->ಕ್ರಮ < regB->ಕ್ರಮ)
		ಹಿಂತಿರುಗಿ true;

	ಆದರೆ ((regA->ಕ್ರಮ == regB->ಕ್ರಮ) &&
	    (regA->ತಳ_2 < regB->ತಳ_2))
		ಹಿಂತಿರುಗಿ true;

	ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain_memregion *ಕಂಡುಹಿಡಿ_region(
						ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom,
						ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ rstart, rend;
	ರಚನೆ sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->ತಳ_2;
		rend = (reg->ಕ್ರಮ < __riscv_xlen) ?
			rstart + ((1UL << reg->ಕ್ರಮ) - 1) : -1UL;
		ಆದರೆ (rstart <= addr && addr <= rend)
			ಹಿಂತಿರುಗಿ reg;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain_memregion *ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_subset_region(
				ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom,
				ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain_memregion *reg,
				ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr)
{
	ರಚನೆ sbi_domain_memregion *sreg, *ret = ಖಾಲಿ;

	sbi_domain_for_each_memregion(dom, sreg) {
		ಆದರೆ (sreg == reg || (sreg->ತಳ_2 <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			ಮುಂದುವರಿ;

		ಆದರೆ (!ret || (sreg->ತಳ_2 < ret->ತಳ_2) ||
		    ((sreg->ತಳ_2 == ret->ತಳ_2) && (sreg->ಕ್ರಮ < ret->ಕ್ರಮ)))
			ret = sreg;
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಶೂನ್ಯ ಅದಲುಬದಲು_region(ರಚನೆ sbi_domain_memregion* reg1,
			ರಚನೆ sbi_domain_memregion* reg2)
{
	ರಚನೆ sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, ಗಾತ್ರ(treg));
	sbi_memcpy(reg1, reg2, ಗಾತ್ರ(treg));
	sbi_memcpy(reg2, &treg, ಗಾತ್ರ(treg));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_domain_ಬಳಸಿದ_memregions(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom)
{
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;
	ರಚನೆ sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		ಎಣಿಕೆ_2++;

	ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sanitize_domain(ರಚನೆ sbi_domain *dom)
{
	u32 i, j, ಎಣಿಕೆ_2;
	bool is_covered;
	ರಚನೆ sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	ಆದರೆ (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ಆದರೆ (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_ಗಮ್ಯ_hartid(i));
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		}
	}

	/* Check memory regions */
	ಆದರೆ (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		ಆದರೆ (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->ತಳ_2, reg->ಕ್ರಮ,
				   reg->flags);
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		}
	}

	/* Count memory regions */
	ಎಣಿಕೆ_2 = sbi_domain_ಬಳಸಿದ_memregions(dom);

	/* Check presence of firmware regions */
	ಆದರೆ (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	/* Sort the memory regions */
	ಗಾಗಿ (i = 0; i < (ಎಣಿಕೆ_2 - 1); i++) {
		reg = &dom->regions[i];
		ಗಾಗಿ (j = i + 1; j < ಎಣಿಕೆ_2; j++) {
			reg1 = &dom->regions[j];

			ಆದರೆ (!is_region_before(reg1, reg))
				ಮುಂದುವರಿ;

			ಅದಲುಬದಲು_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	ಗಾಗಿ (i = 0; i < (ಎಣಿಕೆ_2 - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		ಗಾಗಿ (j = i + 1; j < ಎಣಿಕೆ_2; j++) {
			reg1 = &dom->regions[j];

			ಆದರೆ (is_region_compatible(reg, reg1)) {
				is_covered = true;
				ನಿಲ್ಲಿಸು;
			}
		}

		/* find a region is superset of reg, remove reg */
		ಆದರೆ (is_covered) {
			sbi_memmove(reg, reg + 1, ಗಾತ್ರ(*reg) * (ಎಣಿಕೆ_2 - i));
			ಎಣಿಕೆ_2--;
		} ಇಲ್ಲದಿದ್ದರೆ
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
	ಆದರೆ (dom->ಮುಂದಿನ_ಬಹುಲಕ != PRV_S &&
	    dom->ಮುಂದಿನ_ಬಹುಲಕ != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->ಮುಂದಿನ_ಬಹುಲಕ);
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	/* Check next address and next mode */
	ಆದರೆ (!sbi_domain_check_addr(dom, dom->ಮುಂದಿನ_addr, dom->ಮುಂದಿನ_ಬಹುಲಕ,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->ಮುಂದಿನ_addr);
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	ಹಿಂತಿರುಗಿ 0;
}

bool sbi_domain_check_addr_range(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom,
				 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2,
				 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಬಹುಲಕ,
				 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ access_flags)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ max = addr + ಗಾತ್ರ_2;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain_memregion *reg, *sreg;

	ಆದರೆ (!dom)
		ಹಿಂತಿರುಗಿ false;

	ಆದರೆ (ಗಾತ್ರ_2 && max <= addr)
		ಹಿಂತಿರುಗಿ false;

	ಆಗ (addr < max) {
		reg = ಕಂಡುಹಿಡಿ_region(dom, addr);
		ಆದರೆ (!reg)
			ಹಿಂತಿರುಗಿ false;

		ಆದರೆ (!sbi_domain_check_addr(dom, addr, ಬಹುಲಕ, access_flags))
			ಹಿಂತಿರುಗಿ false;

		sreg = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_subset_region(dom, reg, addr);
		ಆದರೆ (sreg)
			addr = sreg->ತಳ_2;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reg->ಕ್ರಮ < __riscv_xlen)
			addr = reg->ತಳ_2 + (1UL << reg->ಕ್ರಮ);
		ಇಲ್ಲದಿದ್ದರೆ
			ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ true;
}

ಶೂನ್ಯ sbi_domain_dump(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix)
{
	u32 i, j, k;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ rstart, rend;
	ರಚನೆ sbi_domain_memregion *reg;

	sbi_printf("Domain%d Name        %s: %s\n",
		   dom->ಸೂಚ್ಯಂಕ_2, suffix, dom->name);

	sbi_printf("Domain%d Init Order  %s: 0x%x\n",
		   dom->ಸೂಚ್ಯಂಕ_2, suffix, dom->init_ಕ್ರಮ);

	sbi_printf("Domain%d Boot HART   %s: %d\n",
		   dom->ಸೂಚ್ಯಂಕ_2, suffix, dom->boot_hartid);

	k = 0;
	sbi_printf("Domain%d HARTs       %s: ", dom->ಸೂಚ್ಯಂಕ_2, suffix);
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		j = sbi_hartindex_ಗಮ್ಯ_hartid(i);
		sbi_printf("%s0x%x%s", (k++) ? "," : "",
			   j, sbi_domain_is_assigned_hart(dom, i) ? "*" : "");
	}
	sbi_printf("\n");

	i = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->ತಳ_2;
		rend = (reg->ಕ್ರಮ < __riscv_xlen) ?
			rstart + ((1UL << reg->ಕ್ರಮ) - 1) : -1UL;

		sbi_printf("Domain%d Region%02d    %s: 0x%" PRILX "-0x%" PRILX " ",
			   dom->ಸೂಚ್ಯಂಕ_2, i, suffix, rstart, rend);

		k = 0;

		sbi_printf("M: ");
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->ಸೂಚ್ಯಂಕ_2, suffix, dom->ಮುಂದಿನ_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->ಸೂಚ್ಯಂಕ_2, suffix, dom->ಮುಂದಿನ_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->ಸೂಚ್ಯಂಕ_2, suffix);
	ಆಯ್ಕೆ (dom->ಮುಂದಿನ_ಬಹುಲಕ) {
	ಸ್ಥಿತಿ PRV_M:
		sbi_printf("M-mode\n");
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRV_S:
		sbi_printf("S-mode\n");
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRV_U:
		sbi_printf("U-mode\n");
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		sbi_printf("Unknown\n");
		ನಿಲ್ಲಿಸು;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->ಸೂಚ್ಯಂಕ_2, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->ಸೂಚ್ಯಂಕ_2, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

ಶೂನ್ಯ sbi_domain_dump_all(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

ಪೂರ್ಣಾಂಕ sbi_domain_ರಿಜಿಸ್ಟರ್(ರಚನೆ sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	ರಚನೆ sbi_domain *tdom;
	ಪೂರ್ಣಾಂಕ rc;

	/* Sanity checks */
	ಆದರೆ (!dom || domain_finalized)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		ಆದರೆ (tdom == dom ||
		    tdom->init_ಕ್ರಮ == dom->init_ಕ್ರಮ)
			ಹಿಂತಿರುಗಿ SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	ಆದರೆ (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		ಹಿಂತಿರುಗಿ rc;
	}

	sbi_list_ಕೂಡಿಸು_ಬಾಲ(&dom->ಗಂಟು, &domain_list);

	/* Assign index to domain */
	dom->ಸೂಚ್ಯಂಕ_2 = domain_ಎಣಿಕೆ++;

	/* Initialize spinlock for dom->assigned_harts */
	SPIN_ಬೀಗ_INIT(dom->assigned_harts_ಬೀಗ);

	/* Clear assigned HARTs of domain */
	sbi_hartmask_clear_all(&dom->assigned_harts);

	/*
	 * Assign HART to a domain with the least initialization order
	 * where the HART is listed as a possible HART of the domain.
	 */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		tdom = sbi_hartindex_ಗಮ್ಯ_domain(i);
		ಆದರೆ (tdom) {
			ಆದರೆ (tdom->init_ಕ್ರಮ > dom->init_ಕ್ರಮ)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			ಇಲ್ಲದಿದ್ದರೆ
				ಮುಂದುವರಿ;
		}

		sbi_update_hartindex_ಗಮ್ಯ_domain(i, dom);
		sbi_hartmask_ಗಣ_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		ಆದರೆ (sbi_hartindex_ಗಮ್ಯ_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->ಸೂಚ್ಯಂಕ_2, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_ಸಿದ್ಧತೆ_state(dom);
	ಆದರೆ (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->ಗಂಟು);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬೇರು_ಕೂಡಿಸು_memregion(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain_memregion *reg)
{
	ಪೂರ್ಣಾಂಕ rc;
	bool reg_merged;
	ರಚನೆ sbi_domain_memregion *nreg, *nreg1;
	ಪೂರ್ಣಾಂಕ ಬೇರು_memregs_ಎಣಿಕೆ = sbi_domain_ಬಳಸಿದ_memregions(&ಬೇರು);

	/* Sanity checks */
	ಆದರೆ (!reg || domain_finalized || !ಬೇರು.regions ||
	    (ಬೇರು_REGION_MAX <= ಬೇರು_memregs_ಎಣಿಕೆ))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&ಬೇರು, nreg) {
		ಆದರೆ (is_region_compatible(reg, nreg))
			ಹಿಂತಿರುಗಿ 0;
	}

	/* Append the memregion to root memregions */
	nreg = &ಬೇರು.regions[ಬೇರು_memregs_ಎಣಿಕೆ];
	sbi_memcpy(nreg, reg, ಗಾತ್ರ(*reg));
	ಬೇರು_memregs_ಎಣಿಕೆ++;
	ಬೇರು.regions[ಬೇರು_memregs_ಎಣಿಕೆ].ಕ್ರಮ = 0;

	/* Sort and optimize root regions */
	ಮಾಡು {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&ಬೇರು);
		ಆದರೆ (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   ಬೇರು.name, rc);
			ಹಿಂತಿರುಗಿ rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&ಬೇರು, nreg) {
			nreg1 = nreg + 1;
			ಆದರೆ (!nreg1->ಕ್ರಮ)
				ಮುಂದುವರಿ;

			ಆದರೆ (!(nreg->ತಳ_2 & (ಬಿಟ್(nreg->ಕ್ರಮ + 1) - 1)) &&
			    (nreg->ತಳ_2 + ಬಿಟ್(nreg->ಕ್ರಮ)) == nreg1->ತಳ_2 &&
			    nreg->ಕ್ರಮ == nreg1->ಕ್ರಮ &&
			    nreg->flags == nreg1->flags) {
				ಪೂರ್ಣಾಂಕ i1 = nreg1 - ಬೇರು.regions;
				nreg->ಕ್ರಮ++;
				sbi_memmove(nreg1, nreg1 + 1,
					    ಗಾತ್ರ(*nreg1) * (ಬೇರು_memregs_ಎಣಿಕೆ - i1));
				reg_merged = true;
				ಬೇರು_memregs_ಎಣಿಕೆ--;
			}
		}
	} ಆಗ (reg_merged);

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ align, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ region_flags)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ pos, ಕೊನೆ, rsize;
	ರಚನೆ sbi_domain_memregion reg;

	pos = addr;
	ಕೊನೆ = addr + ಗಾತ್ರ_2;
	ಆಗ (pos < ಕೊನೆ) {
		rsize = pos & (align - 1);
		ಆದರೆ (rsize)
			rsize = 1UL << sbi_ffs(pos);
		ಇಲ್ಲದಿದ್ದರೆ
			rsize = ((ಕೊನೆ - pos) < align) ?
				(ಕೊನೆ - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = ಬೇರು_ಕೂಡಿಸು_memregion(&reg);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
		pos += rsize;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_domain_startup(ರಚನೆ sbi_scratch *scratch, u32 cold_hartid)
{
	ಪೂರ್ಣಾಂಕ rc;
	u32 dhart;
	ರಚನೆ sbi_domain *dom;

	/* Sanity checks */
	ಆದರೆ (!domain_finalized)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_ಗಮ್ಯ_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		ಆದರೆ (!sbi_hartindex_valid(dhart))
			ಮುಂದುವರಿ;

		/* Ignore if boot HART not possible for this domain */
		ಆದರೆ (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			ಮುಂದುವರಿ;

		/* Ignore if boot HART assigned different domain */
		ಆದರೆ (sbi_hartindex_ಗಮ್ಯ_domain(dhart) != dom)
			ಮುಂದುವರಿ;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_ಬೀಗ(&dom->assigned_harts_ಬೀಗ);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_ಬೀಗ);
		ಆದರೆ (!rc)
			ಮುಂದುವರಿ;

		/* Startup boot HART of domain */
		ಆದರೆ (dom->boot_hartid == cold_hartid) {
			scratch->ಮುಂದಿನ_addr = dom->ಮುಂದಿನ_addr;
			scratch->ಮುಂದಿನ_ಬಹುಲಕ = dom->ಮುಂದಿನ_ಬಹುಲಕ;
			scratch->ಮುಂದಿನ_arg1 = dom->ಮುಂದಿನ_arg1;
		} ಇಲ್ಲದಿದ್ದರೆ {
			rc = sbi_hsm_hart_ಆರಂಭ(scratch, ಖಾಲಿ,
						dom->boot_hartid,
						dom->ಮುಂದಿನ_addr,
						dom->ಮುಂದಿನ_ಬಹುಲಕ,
						dom->ಮುಂದಿನ_arg1);
			ಆದರೆ (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				ಹಿಂತಿರುಗಿ rc;
			}
		}
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_domain_finalize(ರಚನೆ sbi_scratch *scratch)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	ಆದರೆ (domain_finalized)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	ಆದರೆ (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		ಹಿಂತಿರುಗಿ rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_domain_init(ರಚನೆ sbi_scratch *scratch, u32 cold_hartid)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ sbi_hartmask *ಬೇರು_hmask;
	ರಚನೆ sbi_domain_memregion *ಬೇರು_memregs;
	ಪೂರ್ಣಾಂಕ ಬೇರು_memregs_ಎಣಿಕೆ = 0;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_ತಲೆ(&domain_list);

	ಆದರೆ (!fw_single_region) {
		ಆದರೆ (scratch->fw_rw_ಸ್ಥಾನಾಂತರ == 0 ||
		    (scratch->fw_rw_ಸ್ಥಾನಾಂತರ & (scratch->fw_rw_ಸ್ಥಾನಾಂತರ - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_ಸ್ಥಾನಾಂತರ);
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		}
		ಆದರೆ ((scratch->fw_ಆರಂಭ & (scratch->fw_rw_ಸ್ಥಾನಾಂತರ - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		}
	}

	domain_hart_ptr_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_type_ಸ್ಥಾನಾಂತರ(ಶೂನ್ಯ *);
	ಆದರೆ (!domain_hart_ptr_ಸ್ಥಾನಾಂತರ)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	ಆದರೆ (rc)
		ಹೋಗು fail_free_domain_hart_ptr_ಸ್ಥಾನಾಂತರ;

	ಬೇರು_memregs = sbi_calloc(ಗಾತ್ರ(*ಬೇರು_memregs), ಬೇರು_REGION_MAX + 1);
	ಆದರೆ (!ಬೇರು_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		ಹೋಗು fail_deinit_context;
	}
	ಬೇರು.regions = ಬೇರು_memregs;

	ಬೇರು_hmask = sbi_zalloc(ಗಾತ್ರ(*ಬೇರು_hmask));
	ಆದರೆ (!ಬೇರು_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		ಹೋಗು fail_free_ಬೇರು_memregs;
	}
	ಬೇರು.possible_harts = ಬೇರು_hmask;

	/* Root domain firmware memory region */
	ಆದರೆ (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_ಆರಂಭ, scratch->fw_ಗಾತ್ರ,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ಬೇರು_memregs[ಬೇರು_memregs_ಎಣಿಕೆ++]);
	} ಇಲ್ಲದಿದ್ದರೆ {
		sbi_domain_memregion_init(scratch->fw_ಆರಂಭ,
					  scratch->fw_rw_ಸ್ಥಾನಾಂತರ,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ಬೇರು_memregs[ಬೇರು_memregs_ಎಣಿಕೆ++]);

		sbi_domain_memregion_init((scratch->fw_ಆರಂಭ +
					   scratch->fw_rw_ಸ್ಥಾನಾಂತರ),
					  (scratch->fw_ಗಾತ್ರ -
					   scratch->fw_rw_ಸ್ಥಾನಾಂತರ),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &ಬೇರು_memregs[ಬೇರು_memregs_ಎಣಿಕೆ++]);
	}

	ಬೇರು.fw_region_inited = true;

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
				  &ಬೇರು_memregs[ಬೇರು_memregs_ಎಣಿಕೆ++]);

	/* Root domain memory region end */
	ಬೇರು_memregs[ಬೇರು_memregs_ಎಣಿಕೆ].ಕ್ರಮ = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	ಬೇರು.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	ಬೇರು.ಮುಂದಿನ_arg1 = scratch->ಮುಂದಿನ_arg1;
	ಬೇರು.ಮುಂದಿನ_addr = scratch->ಮುಂದಿನ_addr;
	ಬೇರು.ಮುಂದಿನ_ಬಹುಲಕ = scratch->ಮುಂದಿನ_ಬಹುಲಕ;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_ಗಣ_hartindex(i, ಬೇರು_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_ರಿಜಿಸ್ಟರ್(&ಬೇರು);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_ಬೇರು_hmask;

	ಹಿಂತಿರುಗಿ 0;

fail_free_ಬೇರು_hmask:
	sbi_free(ಬೇರು_hmask);
fail_free_ಬೇರು_memregs:
	sbi_free(ಬೇರು_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_ಸ್ಥಾನಾಂತರ:
	sbi_scratch_free_ಸ್ಥಾನಾಂತರ(domain_hart_ptr_ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ rc;
}
