/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#అన్తర్భావ <sbi/sbi_bitmap.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_hart_protection.h>
#అన్తర్భావ <sbi/sbi_hfence.h>
#అన్తర్భావ <sbi/sbi_math.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_pmp.h>
#అన్తర్భావ <sbi/sbi_tlb.h>
#అన్తర్భావ <sbi/riscv_asm.h>

స్థిర పూర్ణాంకం hart_pmp_read(pmp_t *pmp, గుర్తులేని పూర్ణాంకం n)
{
	పూర్ణాంకం pmpcfg_csr, pmpcfg_జరుపు, pmpaddr_csr;
	గుర్తులేని దీర్ఘ cfgmask;

	/* check parameters */
	ఒకవేళ (n >= PMP_లెక్క)
		తిరిగి SBI_EINVAL;

	/* calculate PMP register and offset */
#ఒకవేళ __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_జరుపు = (n & 3) << 3;
#లేదావేళ __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_జరుపు = (n & 7) << 3;
#లేకపోతే
# error "Unexpected __riscv_xlen"
#అయితే_ముగింపు
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_జరుపు);
	pmp->cfg = (csr_read_అంకె(pmpcfg_csr) & cfgmask) >> pmpcfg_జరుపు;
	pmp->addr = csr_read_అంకె(pmpaddr_csr);

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం hart_pmp_write(pmp_t *pmp, గుర్తులేని పూర్ణాంకం n)
{
	పూర్ణాంకం pmpcfg_csr, pmpcfg_జరుపు, pmpaddr_csr;
	గుర్తులేని దీర్ఘ cfgmask, pmpcfg;

	/* check parameters */
	ఒకవేళ (n >= PMP_లెక్క)
		తిరిగి SBI_EINVAL;

	/* calculate PMP register and offset */
#ఒకవేళ __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_జరుపు = (n & 3) << 3;
#లేదావేళ __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_జరుపు = (n & 7) << 3;
#లేకపోతే
# error "Unexpected __riscv_xlen"
#అయితే_ముగింపు
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_అంకె(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_జరుపు);
	pmpcfg  = (csr_read_అంకె(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((గుర్తులేని దీర్ఘ)pmp->cfg << pmpcfg_జరుపు) & ~cfgmask);
	csr_write_అంకె(pmpcfg_csr, pmpcfg);

	తిరిగి SBI_OK;
}

పూర్ణాంకం sbi_hart_pmp_disable(గుర్తులేని పూర్ణాంకం n)
{
	pmp_t pmp;
	పూర్ణాంకం rc;

	rc = hart_pmp_read(&pmp, n);
	ఒకవేళ (rc)
		తిరిగి rc;

	pmp.cfg = 0;

	తిరిగి hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(గుర్తులేని పూర్ణాంకం n)
{
	pmp_t pmp;

	ఒకవేళ (hart_pmp_read(&pmp, n) != SBI_OK)
		తిరిగి false;

	తిరిగి sbi_pmp_is_enabled(&pmp);
}

పూర్ణాంకం sbi_hart_pmp_సమితి(గుర్తులేని పూర్ణాంకం n, గుర్తులేని దీర్ఘ prot, గుర్తులేని దీర్ఘ addr,
		     గుర్తులేని దీర్ఘ log2len)
{
	pmp_t pmp;
	పూర్ణాంకం rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి hart_pmp_write(&pmp, n);
}

పూర్ణాంకం sbi_hart_pmp_get(గుర్తులేని పూర్ణాంకం n, గుర్తులేని దీర్ఘ *prot_బహిర్గతం, గుర్తులేని దీర్ఘ *addr_బహిర్గతం,
		     గుర్తులేని దీర్ఘ *log2len)
{
	pmp_t pmp;
	పూర్ణాంకం rc;

	rc = hart_pmp_read(&pmp, n);
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి sbi_pmp_decode(&pmp, prot_బహిర్గతం, addr_బహిర్గతం, log2len);
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
#నిర్వచించు SBI_SMEPMP_RESV_నమోదు		0

స్థిర DECLARE_BITMAP(fw_smepmp_ids, PMP_లెక్క);
స్థిర bool fw_smepmp_ids_inited;

గుర్తులేని పూర్ణాంకం sbi_hart_pmp_లెక్క(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	తిరిగి hfeatures->pmp_లెక్క;
}

గుర్తులేని పూర్ణాంకం sbi_hart_pmp_log2gran(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	తిరిగి hfeatures->pmp_log2gran;
}

గుర్తులేని పూర్ణాంకం sbi_hart_pmp_addrbits(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	తిరిగి hfeatures->pmp_addr_బిట్లు;
}

bool sbi_hart_smepmp_is_fw_region(గుర్తులేని పూర్ణాంకం pmp_idx)
{
	ఒకవేళ (!fw_smepmp_ids_inited)
		తిరిగి false;

	తిరిగి bitmap_test(fw_smepmp_ids, pmp_idx) ? true : false;
}

శూన్య sbi_hart_pmp_fence(శూన్య)
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
	ఒకవేళ (misa_extension('S')) {
		__sbi_sfence_vma_all();

		/*
		 * If hypervisor mode is supported, flush caching
		 * structures in guest mode too.
		 */
		ఒకవేళ (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}
}

స్థిర శూన్య sbi_hart_smepmp_సమితి(నిర్మాణం sbi_scratch *scratch,
				నిర్మాణం sbi_domain *dom,
				నిర్మాణం sbi_domain_memregion *reg,
				గుర్తులేని పూర్ణాంకం pmp_idx,
				గుర్తులేని పూర్ణాంకం pmp_flags,
				గుర్తులేని పూర్ణాంకం pmp_log2gran,
				గుర్తులేని దీర్ఘ pmp_addr_max)
{
	గుర్తులేని దీర్ఘ pmp_addr = reg->పునాది_2 >> PMP_జరుపు;

	ఒకవేళ (pmp_log2gran <= reg->క్రమం && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_సమితి(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->పునాది_2, reg->క్రమం);
		sbi_hart_pmp_సమితి(pmp_idx, pmp_flags, reg->పునాది_2, reg->క్రమం);
	} లేకపోతే {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->పునాది_2,
			   reg->క్రమం);
	}
}

స్థిర bool is_valid_pmp_idx(గుర్తులేని పూర్ణాంకం pmp_లెక్క, గుర్తులేని పూర్ణాంకం pmp_idx)
{
	ఒకవేళ (pmp_లెక్క > pmp_idx)
		తిరిగి true;

	sbi_printf("error: insufficient PMP entries\n");
	తిరిగి false;
}

స్థిర పూర్ణాంకం sbi_hart_smepmp_configure(నిర్మాణం sbi_scratch *scratch,
				     నిర్మాణం sbi_domain *dom)
{
	నిర్మాణం sbi_domain_memregion *reg;
	గుర్తులేని పూర్ణాంకం pmp_log2gran, pmp_బిట్లు;
	గుర్తులేని పూర్ణాంకం pmp_idx, pmp_లెక్క;
	గుర్తులేని దీర్ఘ pmp_addr_max;
	గుర్తులేని పూర్ణాంకం pmp_flags;

	pmp_లెక్క = sbi_hart_pmp_లెక్క(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_బిట్లు = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_బిట్లు) | ((1UL << pmp_బిట్లు) - 1);

	/*
	 * Set the RLB so that, we can write to PMP entries without
	 * enforcement even if some entries are locked.
	 */
	csr_సమితి(CSR_MSECCFG, MSECCFG_RLB);

	/* Disable the reserved entry */
	sbi_hart_pmp_disable(SBI_SMEPMP_RESV_నమోదు);

	/* Program M-only regions when MML is not set. */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		ఒకవేళ (pmp_idx == SBI_SMEPMP_RESV_నమోదు)
			pmp_idx++;
		ఒకవేళ (!is_valid_pmp_idx(pmp_లెక్క, pmp_idx))
			తిరిగి SBI_EFAIL;

		/* Skip shared and SU-only regions */
		ఒకవేళ (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			కొనసాగు;
		}

		/*
		 * Track firmware PMP entries to preserve them during
		 * domain switches. Under SmePMP, M-mode requires
		 * explicit PMP entries to access firmware code/data.
		 * These entries must remain enabled across domain
		 * context switches to prevent M-mode access faults.
		 */
		ఒకవేళ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags)) {
			ఒకవేళ (fw_smepmp_ids_inited) {
				/* Check inconsistent firmware region */
				ఒకవేళ (!sbi_hart_smepmp_is_fw_region(pmp_idx))
					తిరిగి SBI_EINVAL;
			} లేకపోతే {
				bitmap_సమితి(fw_smepmp_ids, pmp_idx, 1);
			}
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_సమితి(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}

	fw_smepmp_ids_inited = true;

	/* Set the MML to enforce new encoding */
	csr_సమితి(CSR_MSECCFG, MSECCFG_MML);

	/* Program shared and SU-only regions */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		ఒకవేళ (pmp_idx == SBI_SMEPMP_RESV_నమోదు)
			pmp_idx++;
		ఒకవేళ (!is_valid_pmp_idx(pmp_లెక్క, pmp_idx))
			తిరిగి SBI_EFAIL;

		/* Skip M-only regions */
		ఒకవేళ (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			కొనసాగు;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_సమితి(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	కొరకు(; pmp_idx < pmp_లెక్క; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	తిరిగి 0;
}

స్థిర పూర్ణాంకం sbi_hart_smepmp_తాత్కాలిక_map_range(నిర్మాణం sbi_scratch *scratch,
				     గుర్తులేని దీర్ఘ addr, గుర్తులేని దీర్ఘ కొలత)
{
	/* shared R/W access for M and S/U mode */
	గుర్తులేని పూర్ణాంకం pmp_flags = (PMP_W | PMP_X);
	గుర్తులేని దీర్ఘ క్రమం, పునాది_2 = 0;

	ఒకవేళ (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_నమోదు))
		తిరిగి SBI_ENOSPC;

	కొరకు (క్రమం = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(కొలత));
	     క్రమం <= __riscv_xlen; క్రమం++) {
		ఒకవేళ (క్రమం < __riscv_xlen) {
			పునాది_2 = addr & ~((1UL << క్రమం) - 1UL);
			ఒకవేళ ((పునాది_2 <= addr) &&
			    (addr < (పునాది_2 + (1UL << క్రమం))) &&
			    (పునాది_2 <= (addr + కొలత - 1UL)) &&
			    ((addr + కొలత - 1UL) < (పునాది_2 + (1UL << క్రమం))))
				ఆపు;
		} లేకపోతే {
			తిరిగి SBI_EFAIL;
		}
	}

	sbi_platform_pmp_సమితి(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_నమోదు,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, పునాది_2, క్రమం);
	sbi_hart_pmp_సమితి(SBI_SMEPMP_RESV_నమోదు, pmp_flags, పునాది_2, క్రమం);

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం sbi_hart_smepmp_తాత్కాలిక_unmap_range(నిర్మాణం sbi_scratch *scratch,
				       గుర్తులేని దీర్ఘ addr, గుర్తులేని దీర్ఘ కొలత)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_నమోదు);
	తిరిగి sbi_hart_pmp_disable(SBI_SMEPMP_RESV_నమోదు);
}

స్థిర పూర్ణాంకం sbi_hart_oldpmp_configure(నిర్మాణం sbi_scratch *scratch,
				     నిర్మాణం sbi_domain *dom)
{
	నిర్మాణం sbi_domain_memregion *reg;
	గుర్తులేని దీర్ఘ pmp_addr, pmp_addr_max;
	గుర్తులేని పూర్ణాంకం pmp_log2gran, pmp_బిట్లు;
	గుర్తులేని పూర్ణాంకం pmp_idx, pmp_లెక్క;
	గుర్తులేని పూర్ణాంకం pmp_flags;

	pmp_లెక్క = sbi_hart_pmp_లెక్క(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_బిట్లు = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_బిట్లు) | ((1UL << pmp_బిట్లు) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		ఒకవేళ (!is_valid_pmp_idx(pmp_లెక్క, pmp_idx))
			తిరిగి SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->పునాది_2 >> PMP_జరుపు;
		ఒకవేళ (pmp_log2gran <= reg->క్రమం && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_సమితి(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->పునాది_2, reg->క్రమం);
			sbi_hart_pmp_సమితి(pmp_idx++, pmp_flags, reg->పునాది_2, reg->క్రమం);
		} లేకపోతే {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->పునాది_2,
				   reg->క్రమం);
		}
	}
	/* Disable remaining PMP entries */
	కొరకు(; pmp_idx < pmp_లెక్క; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	తిరిగి 0;
}

స్థిర శూన్య sbi_hart_pmp_unconfigure(నిర్మాణం sbi_scratch *scratch,
				     నిర్మాణం sbi_domain *dom)
{
	పూర్ణాంకం i, pmp_లెక్క = sbi_hart_pmp_లెక్క(scratch);

	కొరకు (i = 0; i < pmp_లెక్క; i++) {
		/* Don't revoke firmware access permissions */
		ఒకవేళ (sbi_hart_smepmp_is_fw_region(i))
			కొనసాగు;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

స్థిర నిర్మాణం sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_జ్ఞాపకం,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

స్థిర నిర్మాణం sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_జ్ఞాపకం,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.తాత్కాలిక_map_range = sbi_hart_smepmp_తాత్కాలిక_map_range,
	.తాత్కాలిక_unmap_range = sbi_hart_smepmp_తాత్కాలిక_unmap_range,
};

పూర్ణాంకం sbi_hart_pmp_init(నిర్మాణం sbi_scratch *scratch)
{
	పూర్ణాంకం rc;

	ఒకవేళ (sbi_hart_pmp_లెక్క(scratch)) {
		ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_రిజిస్టరు(&epmp_protection);
			ఒకవేళ (rc)
				తిరిగి rc;
		} లేకపోతే {
			rc = sbi_hart_protection_రిజిస్టరు(&pmp_protection);
			ఒకవేళ (rc)
				తిరిగి rc;
		}
	}

	తిరిగి 0;
}
