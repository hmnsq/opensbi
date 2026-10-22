/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#અન્તર્ભાવ <sbi/sbi_bitmap.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_hart_protection.h>
#અન્તર્ભાવ <sbi/sbi_hfence.h>
#અન્તર્ભાવ <sbi/sbi_math.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_pmp.h>
#અન્તર્ભાવ <sbi/sbi_tlb.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>

સ્થિર પૂર્ણાંક hart_pmp_read(pmp_t *pmp, unsigned પૂર્ણાંક n)
{
	પૂર્ણાંક pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask;

	/* check parameters */
	જો (n >= PMP_ગણતરી)
		પાછા SBI_EINVAL;

	/* calculate PMP register and offset */
#જો __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#અથવાજો __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#નહીંતર
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_shift);
	pmp->cfg = (csr_read_અંક(pmpcfg_csr) & cfgmask) >> pmpcfg_shift;
	pmp->addr = csr_read_અંક(pmpaddr_csr);

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક hart_pmp_write(pmp_t *pmp, unsigned પૂર્ણાંક n)
{
	પૂર્ણાંક pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask, pmpcfg;

	/* check parameters */
	જો (n >= PMP_ગણતરી)
		પાછા SBI_EINVAL;

	/* calculate PMP register and offset */
#જો __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#અથવાજો __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#નહીંતર
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_અંક(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_shift);
	pmpcfg  = (csr_read_અંક(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((unsigned long)pmp->cfg << pmpcfg_shift) & ~cfgmask);
	csr_write_અંક(pmpcfg_csr, pmpcfg);

	પાછા SBI_OK;
}

પૂર્ણાંક sbi_hart_pmp_disable(unsigned પૂર્ણાંક n)
{
	pmp_t pmp;
	પૂર્ણાંક rc;

	rc = hart_pmp_read(&pmp, n);
	જો (rc)
		પાછા rc;

	pmp.cfg = 0;

	પાછા hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(unsigned પૂર્ણાંક n)
{
	pmp_t pmp;

	જો (hart_pmp_read(&pmp, n) != SBI_OK)
		પાછા false;

	પાછા sbi_pmp_is_enabled(&pmp);
}

પૂર્ણાંક sbi_hart_pmp_set(unsigned પૂર્ણાંક n, unsigned long prot, unsigned long addr,
		     unsigned long log2len)
{
	pmp_t pmp;
	પૂર્ણાંક rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	જો (rc)
		પાછા rc;

	પાછા hart_pmp_write(&pmp, n);
}

પૂર્ણાંક sbi_hart_pmp_get(unsigned પૂર્ણાંક n, unsigned long *prot_out, unsigned long *addr_out,
		     unsigned long *log2len)
{
	pmp_t pmp;
	પૂર્ણાંક rc;

	rc = hart_pmp_read(&pmp, n);
	જો (rc)
		પાછા rc;

	પાછા sbi_pmp_decode(&pmp, prot_out, addr_out, log2len);
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

સ્થિર DECLARE_BITMAP(fw_smepmp_ids, PMP_ગણતરી);
સ્થિર bool fw_smepmp_ids_inited;

unsigned પૂર્ણાંક sbi_hart_pmp_ગણતરી(સંરચના sbi_scratch *scratch)
{
	સંરચના sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	પાછા hfeatures->pmp_ગણતરી;
}

unsigned પૂર્ણાંક sbi_hart_pmp_log2gran(સંરચના sbi_scratch *scratch)
{
	સંરચના sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	પાછા hfeatures->pmp_log2gran;
}

unsigned પૂર્ણાંક sbi_hart_pmp_addrbits(સંરચના sbi_scratch *scratch)
{
	સંરચના sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	પાછા hfeatures->pmp_addr_bits;
}

bool sbi_hart_smepmp_is_fw_region(unsigned પૂર્ણાંક pmp_idx)
{
	જો (!fw_smepmp_ids_inited)
		પાછા false;

	પાછા bitmap_test(fw_smepmp_ids, pmp_idx) ? true : false;
}

શૂન્ય sbi_hart_pmp_fence(શૂન્ય)
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
	જો (misa_extension('S')) {
		__sbi_sfence_vma_all();

		/*
		 * If hypervisor mode is supported, flush caching
		 * structures in guest mode too.
		 */
		જો (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}
}

સ્થિર શૂન્ય sbi_hart_smepmp_set(સંરચના sbi_scratch *scratch,
				સંરચના sbi_domain *dom,
				સંરચના sbi_domain_memregion *reg,
				unsigned પૂર્ણાંક pmp_idx,
				unsigned પૂર્ણાંક pmp_flags,
				unsigned પૂર્ણાંક pmp_log2gran,
				unsigned long pmp_addr_max)
{
	unsigned long pmp_addr = reg->base >> PMP_SHIFT;

	જો (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_set(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->base, reg->order);
		sbi_hart_pmp_set(pmp_idx, pmp_flags, reg->base, reg->order);
	} નહીંતર {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->base,
			   reg->order);
	}
}

સ્થિર bool is_valid_pmp_idx(unsigned પૂર્ણાંક pmp_ગણતરી, unsigned પૂર્ણાંક pmp_idx)
{
	જો (pmp_ગણતરી > pmp_idx)
		પાછા true;

	sbi_printf("error: insufficient PMP entries\n");
	પાછા false;
}

સ્થિર પૂર્ણાંક sbi_hart_smepmp_configure(સંરચના sbi_scratch *scratch,
				     સંરચના sbi_domain *dom)
{
	સંરચના sbi_domain_memregion *reg;
	unsigned પૂર્ણાંક pmp_log2gran, pmp_bits;
	unsigned પૂર્ણાંક pmp_idx, pmp_ગણતરી;
	unsigned long pmp_addr_max;
	unsigned પૂર્ણાંક pmp_flags;

	pmp_ગણતરી = sbi_hart_pmp_ગણતરી(scratch);
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
		જો (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		જો (!is_valid_pmp_idx(pmp_ગણતરી, pmp_idx))
			પાછા SBI_EFAIL;

		/* Skip shared and SU-only regions */
		જો (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			ચાલુ;
		}

		/*
		 * Track firmware PMP entries to preserve them during
		 * domain switches. Under SmePMP, M-mode requires
		 * explicit PMP entries to access firmware code/data.
		 * These entries must remain enabled across domain
		 * context switches to prevent M-mode access faults.
		 */
		જો (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags)) {
			જો (fw_smepmp_ids_inited) {
				/* Check inconsistent firmware region */
				જો (!sbi_hart_smepmp_is_fw_region(pmp_idx))
					પાછા SBI_EINVAL;
			} નહીંતર {
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
		જો (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		જો (!is_valid_pmp_idx(pmp_ગણતરી, pmp_idx))
			પાછા SBI_EFAIL;

		/* Skip M-only regions */
		જો (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			ચાલુ;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_set(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	માટે(; pmp_idx < pmp_ગણતરી; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	પાછા 0;
}

સ્થિર પૂર્ણાંક sbi_hart_smepmp_temp_map_range(સંરચના sbi_scratch *scratch,
				     unsigned long addr, unsigned long size)
{
	/* shared R/W access for M and S/U mode */
	unsigned પૂર્ણાંક pmp_flags = (PMP_W | PMP_X);
	unsigned long order, base = 0;

	જો (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_ENTRY))
		પાછા SBI_ENOSPC;

	માટે (order = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(size));
	     order <= __riscv_xlen; order++) {
		જો (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			જો ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				તોડો;
		} નહીંતર {
			પાછા SBI_EFAIL;
		}
	}

	sbi_platform_pmp_set(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, base, order);
	sbi_hart_pmp_set(SBI_SMEPMP_RESV_ENTRY, pmp_flags, base, order);

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક sbi_hart_smepmp_temp_unmap_range(સંરચના sbi_scratch *scratch,
				       unsigned long addr, unsigned long size)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY);
	પાછા sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ENTRY);
}

સ્થિર પૂર્ણાંક sbi_hart_oldpmp_configure(સંરચના sbi_scratch *scratch,
				     સંરચના sbi_domain *dom)
{
	સંરચના sbi_domain_memregion *reg;
	unsigned long pmp_addr, pmp_addr_max;
	unsigned પૂર્ણાંક pmp_log2gran, pmp_bits;
	unsigned પૂર્ણાંક pmp_idx, pmp_ગણતરી;
	unsigned પૂર્ણાંક pmp_flags;

	pmp_ગણતરી = sbi_hart_pmp_ગણતરી(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_bits = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_bits) | ((1UL << pmp_bits) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		જો (!is_valid_pmp_idx(pmp_ગણતરી, pmp_idx))
			પાછા SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->base >> PMP_SHIFT;
		જો (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_set(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->base, reg->order);
			sbi_hart_pmp_set(pmp_idx++, pmp_flags, reg->base, reg->order);
		} નહીંતર {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->base,
				   reg->order);
		}
	}
	/* Disable remaining PMP entries */
	માટે(; pmp_idx < pmp_ગણતરી; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	પાછા 0;
}

સ્થિર શૂન્ય sbi_hart_pmp_unconfigure(સંરચના sbi_scratch *scratch,
				     સંરચના sbi_domain *dom)
{
	પૂર્ણાંક i, pmp_ગણતરી = sbi_hart_pmp_ગણતરી(scratch);

	માટે (i = 0; i < pmp_ગણતરી; i++) {
		/* Don't revoke firmware access permissions */
		જો (sbi_hart_smepmp_is_fw_region(i))
			ચાલુ;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

સ્થિર સંરચના sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

સ્થિર સંરચના sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.temp_map_range = sbi_hart_smepmp_temp_map_range,
	.temp_unmap_range = sbi_hart_smepmp_temp_unmap_range,
};

પૂર્ણાંક sbi_hart_pmp_init(સંરચના sbi_scratch *scratch)
{
	પૂર્ણાંક rc;

	જો (sbi_hart_pmp_ગણતરી(scratch)) {
		જો (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_register(&epmp_protection);
			જો (rc)
				પાછા rc;
		} નહીંતર {
			rc = sbi_hart_protection_register(&pmp_protection);
			જો (rc)
				પાછા rc;
		}
	}

	પાછા 0;
}
