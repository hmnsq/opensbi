/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitmap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart_protection.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hfence.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_math.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmp.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_tlb.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hart_pmp_read(pmp_t *pmp, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ n)
{
	ಪೂರ್ಣಾಂಕ pmpcfg_csr, pmpcfg_ಸರಿಸು, pmpaddr_csr;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cfgmask;

	/* check parameters */
	ಆದರೆ (n >= PMP_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* calculate PMP register and offset */
#ಆದರೆ __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_ಸರಿಸು = (n & 3) << 3;
#ಅಥವಾಆದರೆ __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_ಸರಿಸು = (n & 7) << 3;
#ಇಲ್ಲದಿದ್ದರೆ
# error "Unexpected __riscv_xlen"
#ಆದರೆ_ಅಂತ್ಯ
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_ಸರಿಸು);
	pmp->cfg = (csr_read_ಅಂಕೆ(pmpcfg_csr) & cfgmask) >> pmpcfg_ಸರಿಸು;
	pmp->addr = csr_read_ಅಂಕೆ(pmpaddr_csr);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hart_pmp_write(pmp_t *pmp, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ n)
{
	ಪೂರ್ಣಾಂಕ pmpcfg_csr, pmpcfg_ಸರಿಸು, pmpaddr_csr;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cfgmask, pmpcfg;

	/* check parameters */
	ಆದರೆ (n >= PMP_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* calculate PMP register and offset */
#ಆದರೆ __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_ಸರಿಸು = (n & 3) << 3;
#ಅಥವಾಆದರೆ __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_ಸರಿಸು = (n & 7) << 3;
#ಇಲ್ಲದಿದ್ದರೆ
# error "Unexpected __riscv_xlen"
#ಆದರೆ_ಅಂತ್ಯ
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_ಅಂಕೆ(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_ಸರಿಸು);
	pmpcfg  = (csr_read_ಅಂಕೆ(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)pmp->cfg << pmpcfg_ಸರಿಸು) & ~cfgmask);
	csr_write_ಅಂಕೆ(pmpcfg_csr, pmpcfg);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಪೂರ್ಣಾಂಕ sbi_hart_pmp_disable(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ n)
{
	pmp_t pmp;
	ಪೂರ್ಣಾಂಕ rc;

	rc = hart_pmp_read(&pmp, n);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	pmp.cfg = 0;

	ಹಿಂತಿರುಗಿ hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ n)
{
	pmp_t pmp;

	ಆದರೆ (hart_pmp_read(&pmp, n) != SBI_OK)
		ಹಿಂತಿರುಗಿ false;

	ಹಿಂತಿರುಗಿ sbi_pmp_is_enabled(&pmp);
}

ಪೂರ್ಣಾಂಕ sbi_hart_pmp_ಗಣ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ n, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ prot, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr,
		     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ log2len)
{
	pmp_t pmp;
	ಪೂರ್ಣಾಂಕ rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ hart_pmp_write(&pmp, n);
}

ಪೂರ್ಣಾಂಕ sbi_hart_pmp_get(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ n, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *prot_ಹೊರಹೋಗು, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *addr_ಹೊರಹೋಗು,
		     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *log2len)
{
	pmp_t pmp;
	ಪೂರ್ಣಾಂಕ rc;

	rc = hart_pmp_read(&pmp, n);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ sbi_pmp_decode(&pmp, prot_ಹೊರಹೋಗು, addr_ಹೊರಹೋಗು, log2len);
}

/*
 * Smepmp enforces access boundaries between M-mode and
 * S/U-mode. When it is enabled, the PMPs are programmed
 * such that M-mode doesn't have access to S/U-mode memory.
 *
 * To give M-mode R/W access to the shared memory between M and
 * S/U-mode, first entry is reserved. It is disabled at boot.
 * When shared memory access is required, the physical address
 * should be programmed into the first PMP entry with R/W
 * permissions to the M-mode. Once the work is done, it should be
 * unmapped. sbi_hart_protection_temp_map_range/sbi_hart_protection_temp_unmap_range
 * function pair should be used to map/unmap the shared memory.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು SBI_SMEPMP_RESV_ನಮೂದು		0

ಸ್ಥಿರ DECLARE_BITMAP(fw_smepmp_ids, PMP_ಎಣಿಕೆ);
ಸ್ಥಿರ bool fw_smepmp_ids_inited;

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ sbi_hart_pmp_ಎಣಿಕೆ(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	ಹಿಂತಿರುಗಿ hfeatures->pmp_ಎಣಿಕೆ;
}

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ sbi_hart_pmp_log2gran(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	ಹಿಂತಿರುಗಿ hfeatures->pmp_log2gran;
}

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ sbi_hart_pmp_addrbits(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	ಹಿಂತಿರುಗಿ hfeatures->pmp_addr_ಬಿಟ್ಟುಗಳು;
}

bool sbi_hart_smepmp_is_fw_region(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_idx)
{
	ಆದರೆ (!fw_smepmp_ids_inited)
		ಹಿಂತಿರುಗಿ false;

	ಹಿಂತಿರುಗಿ bitmap_test(fw_smepmp_ids, pmp_idx) ? true : false;
}

ಶೂನ್ಯ sbi_hart_pmp_fence(ಶೂನ್ಯ)
{
	/*
	 * As per section 3.7.2 of privileged specification v1.12,
	 * virtual address translations can be speculatively performed
	 * (even before actual access). These, along with PMP traslations,
	 * can be cached. This can pose a problem with CPU hotplug
	 * and non-retentive suspend scenario because PMP states are
	 * not preserved.
	 * It is advisable to flush the caching structures under such
	 * conditions.
	 */
	ಆದರೆ (misa_extension('S')) {
		__sbi_sfence_vma_all();

		/*
		 * If hypervisor mode is supported, flush caching
		 * structures in guest mode too.
		 */
		ಆದರೆ (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_hart_smepmp_ಗಣ(ರಚನೆ sbi_scratch *scratch,
				ರಚನೆ sbi_domain *dom,
				ರಚನೆ sbi_domain_memregion *reg,
				ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_idx,
				ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_flags,
				ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_log2gran,
				ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ pmp_addr_max)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ pmp_addr = reg->ತಳ_2 >> PMP_ಸರಿಸು;

	ಆದರೆ (pmp_log2gran <= reg->ಕ್ರಮ && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_ಗಣ(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->ತಳ_2, reg->ಕ್ರಮ);
		sbi_hart_pmp_ಗಣ(pmp_idx, pmp_flags, reg->ತಳ_2, reg->ಕ್ರಮ);
	} ಇಲ್ಲದಿದ್ದರೆ {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->ತಳ_2,
			   reg->ಕ್ರಮ);
	}
}

ಸ್ಥಿರ bool is_valid_pmp_idx(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_ಎಣಿಕೆ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_idx)
{
	ಆದರೆ (pmp_ಎಣಿಕೆ > pmp_idx)
		ಹಿಂತಿರುಗಿ true;

	sbi_printf("error: insufficient PMP entries\n");
	ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_hart_smepmp_configure(ರಚನೆ sbi_scratch *scratch,
				     ರಚನೆ sbi_domain *dom)
{
	ರಚನೆ sbi_domain_memregion *reg;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_log2gran, pmp_ಬಿಟ್ಟುಗಳು;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_idx, pmp_ಎಣಿಕೆ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ pmp_addr_max;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_flags;

	pmp_ಎಣಿಕೆ = sbi_hart_pmp_ಎಣಿಕೆ(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_ಬಿಟ್ಟುಗಳು = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_ಬಿಟ್ಟುಗಳು) | ((1UL << pmp_ಬಿಟ್ಟುಗಳು) - 1);

	/*
	 * Set the RLB so that, we can write to PMP entries without
	 * enforcement even if some entries are locked.
	 */
	csr_ಗಣ(CSR_MSECCFG, MSECCFG_RLB);

	/* Disable the reserved entry */
	sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ನಮೂದು);

	/* Program M-only regions when MML is not set. */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		ಆದರೆ (pmp_idx == SBI_SMEPMP_RESV_ನಮೂದು)
			pmp_idx++;
		ಆದರೆ (!is_valid_pmp_idx(pmp_ಎಣಿಕೆ, pmp_idx))
			ಹಿಂತಿರುಗಿ SBI_EFAIL;

		/* Skip shared and SU-only regions */
		ಆದರೆ (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			ಮುಂದುವರಿ;
		}

		/*
		 * Track firmware PMP entries to preserve them during
		 * domain switches. Under SmePMP, M-mode requires
		 * explicit PMP entries to access firmware code/data.
		 * These entries must remain enabled across domain
		 * context switches to prevent M-mode access faults.
		 */
		ಆದರೆ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags)) {
			ಆದರೆ (fw_smepmp_ids_inited) {
				/* Check inconsistent firmware region */
				ಆದರೆ (!sbi_hart_smepmp_is_fw_region(pmp_idx))
					ಹಿಂತಿರುಗಿ SBI_EINVAL;
			} ಇಲ್ಲದಿದ್ದರೆ {
				bitmap_ಗಣ(fw_smepmp_ids, pmp_idx, 1);
			}
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_ಗಣ(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}

	fw_smepmp_ids_inited = true;

	/* Set the MML to enforce new encoding */
	csr_ಗಣ(CSR_MSECCFG, MSECCFG_MML);

	/* Program shared and SU-only regions */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		ಆದರೆ (pmp_idx == SBI_SMEPMP_RESV_ನಮೂದು)
			pmp_idx++;
		ಆದರೆ (!is_valid_pmp_idx(pmp_ಎಣಿಕೆ, pmp_idx))
			ಹಿಂತಿರುಗಿ SBI_EFAIL;

		/* Skip M-only regions */
		ಆದರೆ (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			ಮುಂದುವರಿ;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_ಗಣ(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	ಗಾಗಿ(; pmp_idx < pmp_ಎಣಿಕೆ; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_hart_smepmp_ತಾತ್ಕಾಲಿಕ_map_range(ರಚನೆ sbi_scratch *scratch,
				     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2)
{
	/* shared R/W access for M and S/U mode */
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_flags = (PMP_W | PMP_X);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಕ್ರಮ, ತಳ_2 = 0;

	ಆದರೆ (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_ನಮೂದು))
		ಹಿಂತಿರುಗಿ SBI_ENOSPC;

	ಗಾಗಿ (ಕ್ರಮ = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(ಗಾತ್ರ_2));
	     ಕ್ರಮ <= __riscv_xlen; ಕ್ರಮ++) {
		ಆದರೆ (ಕ್ರಮ < __riscv_xlen) {
			ತಳ_2 = addr & ~((1UL << ಕ್ರಮ) - 1UL);
			ಆದರೆ ((ತಳ_2 <= addr) &&
			    (addr < (ತಳ_2 + (1UL << ಕ್ರಮ))) &&
			    (ತಳ_2 <= (addr + ಗಾತ್ರ_2 - 1UL)) &&
			    ((addr + ಗಾತ್ರ_2 - 1UL) < (ತಳ_2 + (1UL << ಕ್ರಮ))))
				ನಿಲ್ಲಿಸು;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಹಿಂತಿರುಗಿ SBI_EFAIL;
		}
	}

	sbi_platform_pmp_ಗಣ(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ನಮೂದು,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, ತಳ_2, ಕ್ರಮ);
	sbi_hart_pmp_ಗಣ(SBI_SMEPMP_RESV_ನಮೂದು, pmp_flags, ತಳ_2, ಕ್ರಮ);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_hart_smepmp_ತಾತ್ಕಾಲಿಕ_unmap_range(ರಚನೆ sbi_scratch *scratch,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ನಮೂದು);
	ಹಿಂತಿರುಗಿ sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ನಮೂದು);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_hart_oldpmp_configure(ರಚನೆ sbi_scratch *scratch,
				     ರಚನೆ sbi_domain *dom)
{
	ರಚನೆ sbi_domain_memregion *reg;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ pmp_addr, pmp_addr_max;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_log2gran, pmp_ಬಿಟ್ಟುಗಳು;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_idx, pmp_ಎಣಿಕೆ;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pmp_flags;

	pmp_ಎಣಿಕೆ = sbi_hart_pmp_ಎಣಿಕೆ(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_ಬಿಟ್ಟುಗಳು = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_ಬಿಟ್ಟುಗಳು) | ((1UL << pmp_ಬಿಟ್ಟುಗಳು) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		ಆದರೆ (!is_valid_pmp_idx(pmp_ಎಣಿಕೆ, pmp_idx))
			ಹಿಂತಿರುಗಿ SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->ತಳ_2 >> PMP_ಸರಿಸು;
		ಆದರೆ (pmp_log2gran <= reg->ಕ್ರಮ && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_ಗಣ(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->ತಳ_2, reg->ಕ್ರಮ);
			sbi_hart_pmp_ಗಣ(pmp_idx++, pmp_flags, reg->ತಳ_2, reg->ಕ್ರಮ);
		} ಇಲ್ಲದಿದ್ದರೆ {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->ತಳ_2,
				   reg->ಕ್ರಮ);
		}
	}
	/* Disable remaining PMP entries */
	ಗಾಗಿ(; pmp_idx < pmp_ಎಣಿಕೆ; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_hart_pmp_unconfigure(ರಚನೆ sbi_scratch *scratch,
				     ರಚನೆ sbi_domain *dom)
{
	ಪೂರ್ಣಾಂಕ i, pmp_ಎಣಿಕೆ = sbi_hart_pmp_ಎಣಿಕೆ(scratch);

	ಗಾಗಿ (i = 0; i < pmp_ಎಣಿಕೆ; i++) {
		/* Don't revoke firmware access permissions */
		ಆದರೆ (sbi_hart_smepmp_is_fw_region(i))
			ಮುಂದುವರಿ;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

ಸ್ಥಿರ ರಚನೆ sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_ಸ್ಮರಣೆ,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

ಸ್ಥಿರ ರಚನೆ sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_ಸ್ಮರಣೆ,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.ತಾತ್ಕಾಲಿಕ_map_range = sbi_hart_smepmp_ತಾತ್ಕಾಲಿಕ_map_range,
	.ತಾತ್ಕಾಲಿಕ_unmap_range = sbi_hart_smepmp_ತಾತ್ಕಾಲಿಕ_unmap_range,
};

ಪೂರ್ಣಾಂಕ sbi_hart_pmp_init(ರಚನೆ sbi_scratch *scratch)
{
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (sbi_hart_pmp_ಎಣಿಕೆ(scratch)) {
		ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_ರಿಜಿಸ್ಟರ್(&epmp_protection);
			ಆದರೆ (rc)
				ಹಿಂತಿರುಗಿ rc;
		} ಇಲ್ಲದಿದ್ದರೆ {
			rc = sbi_hart_protection_ರಿಜಿಸ್ಟರ್(&pmp_protection);
			ಆದರೆ (rc)
				ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಹಿಂತಿರುಗಿ 0;
}
