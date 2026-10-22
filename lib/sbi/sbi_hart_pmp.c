/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#अन्तर्भाव <sbi/sbi_bitmap.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_hart_protection.h>
#अन्तर्भाव <sbi/sbi_hfence.h>
#अन्तर्भाव <sbi/sbi_math.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_pmp.h>
#अन्तर्भाव <sbi/sbi_tlb.h>
#अन्तर्भाव <sbi/riscv_asm.h>

स्थिर पूर्णाङ्क hart_pmp_read(pmp_t *pmp, अचिह्नित पूर्णाङ्क n)
{
	पूर्णाङ्क pmpcfg_csr, pmpcfg_सरणम्, pmpaddr_csr;
	अचिह्नित दीर्घ cfgmask;

	/* check parameters */
	यदि (n >= PMP_गणनम्)
		प्रत्यागम SBI_EINVAL;

	/* calculate PMP register and offset */
#यदि __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_सरणम् = (n & 3) << 3;
#नोचेद्यदि __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_सरणम् = (n & 7) << 3;
#अन्यथा
# error "Unexpected __riscv_xlen"
#यदि_अन्तः
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_सरणम्);
	pmp->cfg = (csr_read_अङ्कः(pmpcfg_csr) & cfgmask) >> pmpcfg_सरणम्;
	pmp->addr = csr_read_अङ्कः(pmpaddr_csr);

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क hart_pmp_write(pmp_t *pmp, अचिह्नित पूर्णाङ्क n)
{
	पूर्णाङ्क pmpcfg_csr, pmpcfg_सरणम्, pmpaddr_csr;
	अचिह्नित दीर्घ cfgmask, pmpcfg;

	/* check parameters */
	यदि (n >= PMP_गणनम्)
		प्रत्यागम SBI_EINVAL;

	/* calculate PMP register and offset */
#यदि __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_सरणम् = (n & 3) << 3;
#नोचेद्यदि __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_सरणम् = (n & 7) << 3;
#अन्यथा
# error "Unexpected __riscv_xlen"
#यदि_अन्तः
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_अङ्कः(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_सरणम्);
	pmpcfg  = (csr_read_अङ्कः(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((अचिह्नित दीर्घ)pmp->cfg << pmpcfg_सरणम्) & ~cfgmask);
	csr_write_अङ्कः(pmpcfg_csr, pmpcfg);

	प्रत्यागम SBI_OK;
}

पूर्णाङ्क sbi_hart_pmp_disable(अचिह्नित पूर्णाङ्क n)
{
	pmp_t pmp;
	पूर्णाङ्क rc;

	rc = hart_pmp_read(&pmp, n);
	यदि (rc)
		प्रत्यागम rc;

	pmp.cfg = 0;

	प्रत्यागम hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(अचिह्नित पूर्णाङ्क n)
{
	pmp_t pmp;

	यदि (hart_pmp_read(&pmp, n) != SBI_OK)
		प्रत्यागम false;

	प्रत्यागम sbi_pmp_is_enabled(&pmp);
}

पूर्णाङ्क sbi_hart_pmp_समुच्चयः(अचिह्नित पूर्णाङ्क n, अचिह्नित दीर्घ prot, अचिह्नित दीर्घ addr,
		     अचिह्नित दीर्घ log2len)
{
	pmp_t pmp;
	पूर्णाङ्क rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम hart_pmp_write(&pmp, n);
}

पूर्णाङ्क sbi_hart_pmp_get(अचिह्नित पूर्णाङ्क n, अचिह्नित दीर्घ *prot_बहिर्गमनम्, अचिह्नित दीर्घ *addr_बहिर्गमनम्,
		     अचिह्नित दीर्घ *log2len)
{
	pmp_t pmp;
	पूर्णाङ्क rc;

	rc = hart_pmp_read(&pmp, n);
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम sbi_pmp_decode(&pmp, prot_बहिर्गमनम्, addr_बहिर्गमनम्, log2len);
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
#परिभाषय SBI_SMEPMP_RESV_प्रविष्टिः		0

स्थिर DECLARE_BITMAP(fw_smepmp_ids, PMP_गणनम्);
स्थिर bool fw_smepmp_ids_inited;

अचिह्नित पूर्णाङ्क sbi_hart_pmp_गणनम्(संरचना sbi_scratch *scratch)
{
	संरचना sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	प्रत्यागम hfeatures->pmp_गणनम्;
}

अचिह्नित पूर्णाङ्क sbi_hart_pmp_log2gran(संरचना sbi_scratch *scratch)
{
	संरचना sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	प्रत्यागम hfeatures->pmp_log2gran;
}

अचिह्नित पूर्णाङ्क sbi_hart_pmp_addrbits(संरचना sbi_scratch *scratch)
{
	संरचना sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	प्रत्यागम hfeatures->pmp_addr_द्वयङ्काः;
}

bool sbi_hart_smepmp_is_fw_region(अचिह्नित पूर्णाङ्क pmp_idx)
{
	यदि (!fw_smepmp_ids_inited)
		प्रत्यागम false;

	प्रत्यागम bitmap_test(fw_smepmp_ids, pmp_idx) ? true : false;
}

शून्य sbi_hart_pmp_fence(शून्य)
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
	यदि (misa_extension('S')) {
		__sbi_sfence_vma_all();

		/*
		 * If hypervisor mode is supported, flush caching
		 * structures in guest mode too.
		 */
		यदि (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}
}

स्थिर शून्य sbi_hart_smepmp_समुच्चयः(संरचना sbi_scratch *scratch,
				संरचना sbi_domain *dom,
				संरचना sbi_domain_memregion *reg,
				अचिह्नित पूर्णाङ्क pmp_idx,
				अचिह्नित पूर्णाङ्क pmp_flags,
				अचिह्नित पूर्णाङ्क pmp_log2gran,
				अचिह्नित दीर्घ pmp_addr_max)
{
	अचिह्नित दीर्घ pmp_addr = reg->आधारः_2 >> PMP_सरणम्;

	यदि (pmp_log2gran <= reg->क्रमः && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_समुच्चयः(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->आधारः_2, reg->क्रमः);
		sbi_hart_pmp_समुच्चयः(pmp_idx, pmp_flags, reg->आधारः_2, reg->क्रमः);
	} अन्यथा {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->आधारः_2,
			   reg->क्रमः);
	}
}

स्थिर bool is_valid_pmp_idx(अचिह्नित पूर्णाङ्क pmp_गणनम्, अचिह्नित पूर्णाङ्क pmp_idx)
{
	यदि (pmp_गणनम् > pmp_idx)
		प्रत्यागम true;

	sbi_printf("error: insufficient PMP entries\n");
	प्रत्यागम false;
}

स्थिर पूर्णाङ्क sbi_hart_smepmp_configure(संरचना sbi_scratch *scratch,
				     संरचना sbi_domain *dom)
{
	संरचना sbi_domain_memregion *reg;
	अचिह्नित पूर्णाङ्क pmp_log2gran, pmp_द्वयङ्काः;
	अचिह्नित पूर्णाङ्क pmp_idx, pmp_गणनम्;
	अचिह्नित दीर्घ pmp_addr_max;
	अचिह्नित पूर्णाङ्क pmp_flags;

	pmp_गणनम् = sbi_hart_pmp_गणनम्(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_द्वयङ्काः = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_द्वयङ्काः) | ((1UL << pmp_द्वयङ्काः) - 1);

	/*
	 * Set the RLB so that, we can write to PMP entries without
	 * enforcement even if some entries are locked.
	 */
	csr_समुच्चयः(CSR_MSECCFG, MSECCFG_RLB);

	/* Disable the reserved entry */
	sbi_hart_pmp_disable(SBI_SMEPMP_RESV_प्रविष्टिः);

	/* Program M-only regions when MML is not set. */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		यदि (pmp_idx == SBI_SMEPMP_RESV_प्रविष्टिः)
			pmp_idx++;
		यदि (!is_valid_pmp_idx(pmp_गणनम्, pmp_idx))
			प्रत्यागम SBI_EFAIL;

		/* Skip shared and SU-only regions */
		यदि (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			अनुवर्त;
		}

		/*
		 * Track firmware PMP entries to preserve them during
		 * domain switches. Under SmePMP, M-mode requires
		 * explicit PMP entries to access firmware code/data.
		 * These entries must remain enabled across domain
		 * context switches to prevent M-mode access faults.
		 */
		यदि (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags)) {
			यदि (fw_smepmp_ids_inited) {
				/* Check inconsistent firmware region */
				यदि (!sbi_hart_smepmp_is_fw_region(pmp_idx))
					प्रत्यागम SBI_EINVAL;
			} अन्यथा {
				bitmap_समुच्चयः(fw_smepmp_ids, pmp_idx, 1);
			}
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_समुच्चयः(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}

	fw_smepmp_ids_inited = true;

	/* Set the MML to enforce new encoding */
	csr_समुच्चयः(CSR_MSECCFG, MSECCFG_MML);

	/* Program shared and SU-only regions */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		यदि (pmp_idx == SBI_SMEPMP_RESV_प्रविष्टिः)
			pmp_idx++;
		यदि (!is_valid_pmp_idx(pmp_गणनम्, pmp_idx))
			प्रत्यागम SBI_EFAIL;

		/* Skip M-only regions */
		यदि (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			अनुवर्त;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_समुच्चयः(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	प्रति(; pmp_idx < pmp_गणनम्; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क sbi_hart_smepmp_अस्थायी_map_range(संरचना sbi_scratch *scratch,
				     अचिह्नित दीर्घ addr, अचिह्नित दीर्घ आकार)
{
	/* shared R/W access for M and S/U mode */
	अचिह्नित पूर्णाङ्क pmp_flags = (PMP_W | PMP_X);
	अचिह्नित दीर्घ क्रमः, आधारः_2 = 0;

	यदि (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_प्रविष्टिः))
		प्रत्यागम SBI_ENOSPC;

	प्रति (क्रमः = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(आकार));
	     क्रमः <= __riscv_xlen; क्रमः++) {
		यदि (क्रमः < __riscv_xlen) {
			आधारः_2 = addr & ~((1UL << क्रमः) - 1UL);
			यदि ((आधारः_2 <= addr) &&
			    (addr < (आधारः_2 + (1UL << क्रमः))) &&
			    (आधारः_2 <= (addr + आकार - 1UL)) &&
			    ((addr + आकार - 1UL) < (आधारः_2 + (1UL << क्रमः))))
				भङ्ग;
		} अन्यथा {
			प्रत्यागम SBI_EFAIL;
		}
	}

	sbi_platform_pmp_समुच्चयः(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_प्रविष्टिः,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, आधारः_2, क्रमः);
	sbi_hart_pmp_समुच्चयः(SBI_SMEPMP_RESV_प्रविष्टिः, pmp_flags, आधारः_2, क्रमः);

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क sbi_hart_smepmp_अस्थायी_unmap_range(संरचना sbi_scratch *scratch,
				       अचिह्नित दीर्घ addr, अचिह्नित दीर्घ आकार)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_प्रविष्टिः);
	प्रत्यागम sbi_hart_pmp_disable(SBI_SMEPMP_RESV_प्रविष्टिः);
}

स्थिर पूर्णाङ्क sbi_hart_oldpmp_configure(संरचना sbi_scratch *scratch,
				     संरचना sbi_domain *dom)
{
	संरचना sbi_domain_memregion *reg;
	अचिह्नित दीर्घ pmp_addr, pmp_addr_max;
	अचिह्नित पूर्णाङ्क pmp_log2gran, pmp_द्वयङ्काः;
	अचिह्नित पूर्णाङ्क pmp_idx, pmp_गणनम्;
	अचिह्नित पूर्णाङ्क pmp_flags;

	pmp_गणनम् = sbi_hart_pmp_गणनम्(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_द्वयङ्काः = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_द्वयङ्काः) | ((1UL << pmp_द्वयङ्काः) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		यदि (!is_valid_pmp_idx(pmp_गणनम्, pmp_idx))
			प्रत्यागम SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->आधारः_2 >> PMP_सरणम्;
		यदि (pmp_log2gran <= reg->क्रमः && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_समुच्चयः(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->आधारः_2, reg->क्रमः);
			sbi_hart_pmp_समुच्चयः(pmp_idx++, pmp_flags, reg->आधारः_2, reg->क्रमः);
		} अन्यथा {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->आधारः_2,
				   reg->क्रमः);
		}
	}
	/* Disable remaining PMP entries */
	प्रति(; pmp_idx < pmp_गणनम्; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	प्रत्यागम 0;
}

स्थिर शून्य sbi_hart_pmp_unconfigure(संरचना sbi_scratch *scratch,
				     संरचना sbi_domain *dom)
{
	पूर्णाङ्क i, pmp_गणनम् = sbi_hart_pmp_गणनम्(scratch);

	प्रति (i = 0; i < pmp_गणनम्; i++) {
		/* Don't revoke firmware access permissions */
		यदि (sbi_hart_smepmp_is_fw_region(i))
			अनुवर्त;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

स्थिर संरचना sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_स्मृतिः,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

स्थिर संरचना sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_स्मृतिः,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.अस्थायी_map_range = sbi_hart_smepmp_अस्थायी_map_range,
	.अस्थायी_unmap_range = sbi_hart_smepmp_अस्थायी_unmap_range,
};

पूर्णाङ्क sbi_hart_pmp_init(संरचना sbi_scratch *scratch)
{
	पूर्णाङ्क rc;

	यदि (sbi_hart_pmp_गणनम्(scratch)) {
		यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_पञ्जिकाकोशः(&epmp_protection);
			यदि (rc)
				प्रत्यागम rc;
		} अन्यथा {
			rc = sbi_hart_protection_पञ्जिकाकोशः(&pmp_protection);
			यदि (rc)
				प्रत्यागम rc;
		}
	}

	प्रत्यागम 0;
}
