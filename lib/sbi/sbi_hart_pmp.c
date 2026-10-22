/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitmap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart_protection.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hfence.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmp.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_tlb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>

ਸਥਿਰ ਪੂਰਨ hart_pmp_read(pmp_t *pmp, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ n)
{
	ਪੂਰਨ pmpcfg_csr, pmpcfg_ਖਿਸਕਾਓ, pmpaddr_csr;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cfgmask;

	/* check parameters */
	ਜੇ (n >= PMP_ਗਿਣਤੀ)
		ਵਾਪਸ SBI_EINVAL;

	/* calculate PMP register and offset */
#ਜੇ __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_ਖਿਸਕਾਓ = (n & 3) << 3;
#ਜਾਂਜੇ __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_ਖਿਸਕਾਓ = (n & 7) << 3;
#ਨਹੀਂਤਾਂ
# error "Unexpected __riscv_xlen"
#ਜੇ_ਅੰਤ
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_ਖਿਸਕਾਓ);
	pmp->cfg = (csr_read_ਅੰਕ(pmpcfg_csr) & cfgmask) >> pmpcfg_ਖਿਸਕਾਓ;
	pmp->addr = csr_read_ਅੰਕ(pmpaddr_csr);

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ hart_pmp_write(pmp_t *pmp, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ n)
{
	ਪੂਰਨ pmpcfg_csr, pmpcfg_ਖਿਸਕਾਓ, pmpaddr_csr;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cfgmask, pmpcfg;

	/* check parameters */
	ਜੇ (n >= PMP_ਗਿਣਤੀ)
		ਵਾਪਸ SBI_EINVAL;

	/* calculate PMP register and offset */
#ਜੇ __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_ਖਿਸਕਾਓ = (n & 3) << 3;
#ਜਾਂਜੇ __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_ਖਿਸਕਾਓ = (n & 7) << 3;
#ਨਹੀਂਤਾਂ
# error "Unexpected __riscv_xlen"
#ਜੇ_ਅੰਤ
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_ਅੰਕ(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_ਖਿਸਕਾਓ);
	pmpcfg  = (csr_read_ਅੰਕ(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)pmp->cfg << pmpcfg_ਖਿਸਕਾਓ) & ~cfgmask);
	csr_write_ਅੰਕ(pmpcfg_csr, pmpcfg);

	ਵਾਪਸ SBI_OK;
}

ਪੂਰਨ sbi_hart_pmp_disable(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ n)
{
	pmp_t pmp;
	ਪੂਰਨ rc;

	rc = hart_pmp_read(&pmp, n);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	pmp.cfg = 0;

	ਵਾਪਸ hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ n)
{
	pmp_t pmp;

	ਜੇ (hart_pmp_read(&pmp, n) != SBI_OK)
		ਵਾਪਸ false;

	ਵਾਪਸ sbi_pmp_is_enabled(&pmp);
}

ਪੂਰਨ sbi_hart_pmp_ਸੈੱਟ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ n, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ prot, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ log2len)
{
	pmp_t pmp;
	ਪੂਰਨ rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ hart_pmp_write(&pmp, n);
}

ਪੂਰਨ sbi_hart_pmp_get(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ n, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *prot_ਬਾਹਰਲਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *addr_ਬਾਹਰਲਾ,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *log2len)
{
	pmp_t pmp;
	ਪੂਰਨ rc;

	rc = hart_pmp_read(&pmp, n);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ sbi_pmp_decode(&pmp, prot_ਬਾਹਰਲਾ, addr_ਬਾਹਰਲਾ, log2len);
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBI_SMEPMP_RESV_ਇੰਦਰਾਜ		0

ਸਥਿਰ DECLARE_BITMAP(fw_smepmp_ids, PMP_ਗਿਣਤੀ);
ਸਥਿਰ bool fw_smepmp_ids_inited;

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ sbi_hart_pmp_ਗਿਣਤੀ(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	ਵਾਪਸ hfeatures->pmp_ਗਿਣਤੀ;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ sbi_hart_pmp_log2gran(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	ਵਾਪਸ hfeatures->pmp_log2gran;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ sbi_hart_pmp_addrbits(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	ਵਾਪਸ hfeatures->pmp_addr_ਬਿੱਟ;
}

bool sbi_hart_smepmp_is_fw_region(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_idx)
{
	ਜੇ (!fw_smepmp_ids_inited)
		ਵਾਪਸ false;

	ਵਾਪਸ bitmap_test(fw_smepmp_ids, pmp_idx) ? true : false;
}

ਖਾਲੀ sbi_hart_pmp_fence(ਖਾਲੀ)
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
	ਜੇ (misa_extension('S')) {
		__sbi_sfence_vma_all();

		/*
		 * If hypervisor mode is supported, flush caching
		 * structures in guest mode too.
		 */
		ਜੇ (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}
}

ਸਥਿਰ ਖਾਲੀ sbi_hart_smepmp_ਸੈੱਟ(ਬਣਤਰ sbi_scratch *scratch,
				ਬਣਤਰ sbi_domain *dom,
				ਬਣਤਰ sbi_domain_memregion *reg,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_idx,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_flags,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_log2gran,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ pmp_addr_max)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ pmp_addr = reg->ਅਧਾਰ_2 >> PMP_ਖਿਸਕਾਓ;

	ਜੇ (pmp_log2gran <= reg->ਕ੍ਰਮ && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_ਸੈੱਟ(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->ਅਧਾਰ_2, reg->ਕ੍ਰਮ);
		sbi_hart_pmp_ਸੈੱਟ(pmp_idx, pmp_flags, reg->ਅਧਾਰ_2, reg->ਕ੍ਰਮ);
	} ਨਹੀਂਤਾਂ {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->ਅਧਾਰ_2,
			   reg->ਕ੍ਰਮ);
	}
}

ਸਥਿਰ bool is_valid_pmp_idx(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_ਗਿਣਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_idx)
{
	ਜੇ (pmp_ਗਿਣਤੀ > pmp_idx)
		ਵਾਪਸ true;

	sbi_printf("error: insufficient PMP entries\n");
	ਵਾਪਸ false;
}

ਸਥਿਰ ਪੂਰਨ sbi_hart_smepmp_configure(ਬਣਤਰ sbi_scratch *scratch,
				     ਬਣਤਰ sbi_domain *dom)
{
	ਬਣਤਰ sbi_domain_memregion *reg;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_log2gran, pmp_ਬਿੱਟ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_idx, pmp_ਗਿਣਤੀ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ pmp_addr_max;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_flags;

	pmp_ਗਿਣਤੀ = sbi_hart_pmp_ਗਿਣਤੀ(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_ਬਿੱਟ = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_ਬਿੱਟ) | ((1UL << pmp_ਬਿੱਟ) - 1);

	/*
	 * Set the RLB so that, we can write to PMP entries without
	 * enforcement even if some entries are locked.
	 */
	csr_ਸੈੱਟ(CSR_MSECCFG, MSECCFG_RLB);

	/* Disable the reserved entry */
	sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ਇੰਦਰਾਜ);

	/* Program M-only regions when MML is not set. */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		ਜੇ (pmp_idx == SBI_SMEPMP_RESV_ਇੰਦਰਾਜ)
			pmp_idx++;
		ਜੇ (!is_valid_pmp_idx(pmp_ਗਿਣਤੀ, pmp_idx))
			ਵਾਪਸ SBI_EFAIL;

		/* Skip shared and SU-only regions */
		ਜੇ (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			ਜਾਰੀ;
		}

		/*
		 * Track firmware PMP entries to preserve them during
		 * domain switches. Under SmePMP, M-mode requires
		 * explicit PMP entries to access firmware code/data.
		 * These entries must remain enabled across domain
		 * context switches to prevent M-mode access faults.
		 */
		ਜੇ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags)) {
			ਜੇ (fw_smepmp_ids_inited) {
				/* Check inconsistent firmware region */
				ਜੇ (!sbi_hart_smepmp_is_fw_region(pmp_idx))
					ਵਾਪਸ SBI_EINVAL;
			} ਨਹੀਂਤਾਂ {
				bitmap_ਸੈੱਟ(fw_smepmp_ids, pmp_idx, 1);
			}
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_ਸੈੱਟ(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}

	fw_smepmp_ids_inited = true;

	/* Set the MML to enforce new encoding */
	csr_ਸੈੱਟ(CSR_MSECCFG, MSECCFG_MML);

	/* Program shared and SU-only regions */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		ਜੇ (pmp_idx == SBI_SMEPMP_RESV_ਇੰਦਰਾਜ)
			pmp_idx++;
		ਜੇ (!is_valid_pmp_idx(pmp_ਗਿਣਤੀ, pmp_idx))
			ਵਾਪਸ SBI_EFAIL;

		/* Skip M-only regions */
		ਜੇ (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			ਜਾਰੀ;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_ਸੈੱਟ(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	ਲਈ(; pmp_idx < pmp_ਗਿਣਤੀ; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ sbi_hart_smepmp_ਆਰਜ਼ੀ_map_range(ਬਣਤਰ sbi_scratch *scratch,
				     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ)
{
	/* shared R/W access for M and S/U mode */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_flags = (PMP_W | PMP_X);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਕ੍ਰਮ, ਅਧਾਰ_2 = 0;

	ਜੇ (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_ਇੰਦਰਾਜ))
		ਵਾਪਸ SBI_ENOSPC;

	ਲਈ (ਕ੍ਰਮ = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(ਮਾਪ));
	     ਕ੍ਰਮ <= __riscv_xlen; ਕ੍ਰਮ++) {
		ਜੇ (ਕ੍ਰਮ < __riscv_xlen) {
			ਅਧਾਰ_2 = addr & ~((1UL << ਕ੍ਰਮ) - 1UL);
			ਜੇ ((ਅਧਾਰ_2 <= addr) &&
			    (addr < (ਅਧਾਰ_2 + (1UL << ਕ੍ਰਮ))) &&
			    (ਅਧਾਰ_2 <= (addr + ਮਾਪ - 1UL)) &&
			    ((addr + ਮਾਪ - 1UL) < (ਅਧਾਰ_2 + (1UL << ਕ੍ਰਮ))))
				ਰੋਕੋ;
		} ਨਹੀਂਤਾਂ {
			ਵਾਪਸ SBI_EFAIL;
		}
	}

	sbi_platform_pmp_ਸੈੱਟ(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ਇੰਦਰਾਜ,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, ਅਧਾਰ_2, ਕ੍ਰਮ);
	sbi_hart_pmp_ਸੈੱਟ(SBI_SMEPMP_RESV_ਇੰਦਰਾਜ, pmp_flags, ਅਧਾਰ_2, ਕ੍ਰਮ);

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ sbi_hart_smepmp_ਆਰਜ਼ੀ_unmap_range(ਬਣਤਰ sbi_scratch *scratch,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ਇੰਦਰਾਜ);
	ਵਾਪਸ sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ਇੰਦਰਾਜ);
}

ਸਥਿਰ ਪੂਰਨ sbi_hart_oldpmp_configure(ਬਣਤਰ sbi_scratch *scratch,
				     ਬਣਤਰ sbi_domain *dom)
{
	ਬਣਤਰ sbi_domain_memregion *reg;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ pmp_addr, pmp_addr_max;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_log2gran, pmp_ਬਿੱਟ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_idx, pmp_ਗਿਣਤੀ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pmp_flags;

	pmp_ਗਿਣਤੀ = sbi_hart_pmp_ਗਿਣਤੀ(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_ਬਿੱਟ = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_ਬਿੱਟ) | ((1UL << pmp_ਬਿੱਟ) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		ਜੇ (!is_valid_pmp_idx(pmp_ਗਿਣਤੀ, pmp_idx))
			ਵਾਪਸ SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->ਅਧਾਰ_2 >> PMP_ਖਿਸਕਾਓ;
		ਜੇ (pmp_log2gran <= reg->ਕ੍ਰਮ && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_ਸੈੱਟ(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->ਅਧਾਰ_2, reg->ਕ੍ਰਮ);
			sbi_hart_pmp_ਸੈੱਟ(pmp_idx++, pmp_flags, reg->ਅਧਾਰ_2, reg->ਕ੍ਰਮ);
		} ਨਹੀਂਤਾਂ {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->ਅਧਾਰ_2,
				   reg->ਕ੍ਰਮ);
		}
	}
	/* Disable remaining PMP entries */
	ਲਈ(; pmp_idx < pmp_ਗਿਣਤੀ; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ sbi_hart_pmp_unconfigure(ਬਣਤਰ sbi_scratch *scratch,
				     ਬਣਤਰ sbi_domain *dom)
{
	ਪੂਰਨ i, pmp_ਗਿਣਤੀ = sbi_hart_pmp_ਗਿਣਤੀ(scratch);

	ਲਈ (i = 0; i < pmp_ਗਿਣਤੀ; i++) {
		/* Don't revoke firmware access permissions */
		ਜੇ (sbi_hart_smepmp_is_fw_region(i))
			ਜਾਰੀ;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

ਸਥਿਰ ਬਣਤਰ sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_ਸਮ੍ਰਿਤੀ,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

ਸਥਿਰ ਬਣਤਰ sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_ਸਮ੍ਰਿਤੀ,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.ਆਰਜ਼ੀ_map_range = sbi_hart_smepmp_ਆਰਜ਼ੀ_map_range,
	.ਆਰਜ਼ੀ_unmap_range = sbi_hart_smepmp_ਆਰਜ਼ੀ_unmap_range,
};

ਪੂਰਨ sbi_hart_pmp_init(ਬਣਤਰ sbi_scratch *scratch)
{
	ਪੂਰਨ rc;

	ਜੇ (sbi_hart_pmp_ਗਿਣਤੀ(scratch)) {
		ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_ਰਜਿਸਟਰ(&epmp_protection);
			ਜੇ (rc)
				ਵਾਪਸ rc;
		} ਨਹੀਂਤਾਂ {
			rc = sbi_hart_protection_ਰਜਿਸਟਰ(&pmp_protection);
			ਜੇ (rc)
				ਵਾਪਸ rc;
		}
	}

	ਵਾਪਸ 0;
}
