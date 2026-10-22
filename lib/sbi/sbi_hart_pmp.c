/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitmap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart_protection.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hfence.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_math.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_pmp.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_tlb.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hart_pmp_read(pmp_t *pmp, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask;

	/* check parameters */
	ଯଦି (n >= PMP_ଗଣନା)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* calculate PMP register and offset */
#ଯଦି __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#ନଚେତଯଦି __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#ଅନ୍ୟଥା
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_shift);
	pmp->cfg = (csr_read_ଅଙ୍କ(pmpcfg_csr) & cfgmask) >> pmpcfg_shift;
	pmp->addr = csr_read_ଅଙ୍କ(pmpaddr_csr);

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hart_pmp_write(pmp_t *pmp, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask, pmpcfg;

	/* check parameters */
	ଯଦି (n >= PMP_ଗଣନା)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* calculate PMP register and offset */
#ଯଦି __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#ନଚେତଯଦି __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#ଅନ୍ୟଥା
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_ଅଙ୍କ(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_shift);
	pmpcfg  = (csr_read_ଅଙ୍କ(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((unsigned long)pmp->cfg << pmpcfg_shift) & ~cfgmask);
	csr_write_ଅଙ୍କ(pmpcfg_csr, pmpcfg);

	ଫେରନ୍ତୁ SBI_OK;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_pmp_disable(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
	pmp_t pmp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = hart_pmp_read(&pmp, n);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	pmp.cfg = 0;

	ଫେରନ୍ତୁ hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
	pmp_t pmp;

	ଯଦି (hart_pmp_read(&pmp, n) != SBI_OK)
		ଫେରନ୍ତୁ false;

	ଫେରନ୍ତୁ sbi_pmp_is_enabled(&pmp);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_pmp_set(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, unsigned long prot, unsigned long addr,
		     unsigned long log2len)
{
	pmp_t pmp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଫେରନ୍ତୁ hart_pmp_write(&pmp, n);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_pmp_get(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, unsigned long *prot_out, unsigned long *addr_out,
		     unsigned long *log2len)
{
	pmp_t pmp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = hart_pmp_read(&pmp, n);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଫେରନ୍ତୁ sbi_pmp_decode(&pmp, prot_out, addr_out, log2len);
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
#define SBI_SMEPMP_RESV_ENTRY		0

ସ୍ଥିର DECLARE_BITMAP(fw_smepmp_ids, PMP_ଗଣନା);
ସ୍ଥିର bool fw_smepmp_ids_inited;

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_pmp_ଗଣନା(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	ଫେରନ୍ତୁ hfeatures->pmp_ଗଣନା;
}

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_pmp_log2gran(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	ଫେରନ୍ତୁ hfeatures->pmp_log2gran;
}

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_pmp_addrbits(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	ଫେରନ୍ତୁ hfeatures->pmp_addr_bits;
}

bool sbi_hart_smepmp_is_fw_region(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_idx)
{
	ଯଦି (!fw_smepmp_ids_inited)
		ଫେରନ୍ତୁ false;

	ଫେରନ୍ତୁ bitmap_test(fw_smepmp_ids, pmp_idx) ? true : false;
}

ଶୂନ୍ୟ sbi_hart_pmp_fence(ଶୂନ୍ୟ)
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
	ଯଦି (misa_extension('S')) {
		__sbi_sfence_vma_all();

		/*
		 * If hypervisor mode is supported, flush caching
		 * structures in guest mode too.
		 */
		ଯଦି (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_hart_smepmp_set(ସଂରଚନା sbi_scratch *scratch,
				ସଂରଚନା sbi_domain *dom,
				ସଂରଚନା sbi_domain_memregion *reg,
				unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_idx,
				unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_flags,
				unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_log2gran,
				unsigned long pmp_addr_max)
{
	unsigned long pmp_addr = reg->base >> PMP_SHIFT;

	ଯଦି (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_set(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->base, reg->order);
		sbi_hart_pmp_set(pmp_idx, pmp_flags, reg->base, reg->order);
	} ଅନ୍ୟଥା {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->base,
			   reg->order);
	}
}

ସ୍ଥିର bool is_valid_pmp_idx(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_ଗଣନା, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_idx)
{
	ଯଦି (pmp_ଗଣନା > pmp_idx)
		ଫେରନ୍ତୁ true;

	sbi_printf("error: insufficient PMP entries\n");
	ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_smepmp_configure(ସଂରଚନା sbi_scratch *scratch,
				     ସଂରଚନା sbi_domain *dom)
{
	ସଂରଚନା sbi_domain_memregion *reg;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_log2gran, pmp_bits;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_idx, pmp_ଗଣନା;
	unsigned long pmp_addr_max;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_flags;

	pmp_ଗଣନା = sbi_hart_pmp_ଗଣନା(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_bits = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_bits) | ((1UL << pmp_bits) - 1);

	/*
	 * Set the RLB so that, we can write to PMP entries without
	 * enforcement even if some entries are locked.
	 */
	csr_set(CSR_MSECCFG, MSECCFG_RLB);

	/* Disable the reserved entry */
	sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ENTRY);

	/* Program M-only regions when MML is not set. */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		ଯଦି (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		ଯଦି (!is_valid_pmp_idx(pmp_ଗଣନା, pmp_idx))
			ଫେରନ୍ତୁ SBI_EFAIL;

		/* Skip shared and SU-only regions */
		ଯଦି (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			ଜାରି;
		}

		/*
		 * Track firmware PMP entries to preserve them during
		 * domain switches. Under SmePMP, M-mode requires
		 * explicit PMP entries to access firmware code/data.
		 * These entries must remain enabled across domain
		 * context switches to prevent M-mode access faults.
		 */
		ଯଦି (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags)) {
			ଯଦି (fw_smepmp_ids_inited) {
				/* Check inconsistent firmware region */
				ଯଦି (!sbi_hart_smepmp_is_fw_region(pmp_idx))
					ଫେରନ୍ତୁ SBI_EINVAL;
			} ଅନ୍ୟଥା {
				bitmap_set(fw_smepmp_ids, pmp_idx, 1);
			}
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_set(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}

	fw_smepmp_ids_inited = true;

	/* Set the MML to enforce new encoding */
	csr_set(CSR_MSECCFG, MSECCFG_MML);

	/* Program shared and SU-only regions */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		ଯଦି (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		ଯଦି (!is_valid_pmp_idx(pmp_ଗଣନା, pmp_idx))
			ଫେରନ୍ତୁ SBI_EFAIL;

		/* Skip M-only regions */
		ଯଦି (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			ଜାରି;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_set(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	ପାଇଁ(; pmp_idx < pmp_ଗଣନା; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_smepmp_temp_map_range(ସଂରଚନା sbi_scratch *scratch,
				     unsigned long addr, unsigned long size)
{
	/* shared R/W access for M and S/U mode */
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_flags = (PMP_W | PMP_X);
	unsigned long order, base = 0;

	ଯଦି (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_ENTRY))
		ଫେରନ୍ତୁ SBI_ENOSPC;

	ପାଇଁ (order = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(size));
	     order <= __riscv_xlen; order++) {
		ଯଦି (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			ଯଦି ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				ଭାଙ୍ଗ;
		} ଅନ୍ୟଥା {
			ଫେରନ୍ତୁ SBI_EFAIL;
		}
	}

	sbi_platform_pmp_set(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, base, order);
	sbi_hart_pmp_set(SBI_SMEPMP_RESV_ENTRY, pmp_flags, base, order);

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_smepmp_temp_unmap_range(ସଂରଚନା sbi_scratch *scratch,
				       unsigned long addr, unsigned long size)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY);
	ଫେରନ୍ତୁ sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ENTRY);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_oldpmp_configure(ସଂରଚନା sbi_scratch *scratch,
				     ସଂରଚନା sbi_domain *dom)
{
	ସଂରଚନା sbi_domain_memregion *reg;
	unsigned long pmp_addr, pmp_addr_max;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_log2gran, pmp_bits;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_idx, pmp_ଗଣନା;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmp_flags;

	pmp_ଗଣନା = sbi_hart_pmp_ଗଣନା(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_bits = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_bits) | ((1UL << pmp_bits) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		ଯଦି (!is_valid_pmp_idx(pmp_ଗଣନା, pmp_idx))
			ଫେରନ୍ତୁ SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->base >> PMP_SHIFT;
		ଯଦି (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_set(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->base, reg->order);
			sbi_hart_pmp_set(pmp_idx++, pmp_flags, reg->base, reg->order);
		} ଅନ୍ୟଥା {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->base,
				   reg->order);
		}
	}
	/* Disable remaining PMP entries */
	ପାଇଁ(; pmp_idx < pmp_ଗଣନା; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_hart_pmp_unconfigure(ସଂରଚନା sbi_scratch *scratch,
				     ସଂରଚନା sbi_domain *dom)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, pmp_ଗଣନା = sbi_hart_pmp_ଗଣନା(scratch);

	ପାଇଁ (i = 0; i < pmp_ଗଣନା; i++) {
		/* Don't revoke firmware access permissions */
		ଯଦି (sbi_hart_smepmp_is_fw_region(i))
			ଜାରି;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

ସ୍ଥିର ସଂରଚନା sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

ସ୍ଥିର ସଂରଚନା sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.temp_map_range = sbi_hart_smepmp_temp_map_range,
	.temp_unmap_range = sbi_hart_smepmp_temp_unmap_range,
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_pmp_init(ସଂରଚନା sbi_scratch *scratch)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (sbi_hart_pmp_ଗଣନା(scratch)) {
		ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_register(&epmp_protection);
			ଯଦି (rc)
				ଫେରନ୍ତୁ rc;
		} ଅନ୍ୟଥା {
			rc = sbi_hart_protection_register(&pmp_protection);
			ଯଦି (rc)
				ଫେରନ୍ତୁ rc;
		}
	}

	ଫେରନ୍ତୁ 0;
}
