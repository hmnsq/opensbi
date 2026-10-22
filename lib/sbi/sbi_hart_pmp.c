/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#অন্তর্ভাব <sbi/sbi_bitmap.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_hart_protection.h>
#অন্তর্ভাব <sbi/sbi_hfence.h>
#অন্তর্ভাব <sbi/sbi_math.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_pmp.h>
#অন্তর্ভাব <sbi/sbi_tlb.h>
#অন্তর্ভাব <sbi/riscv_asm.h>

স্থির পূর্ণসংখ্যা hart_pmp_read(pmp_t *pmp, unsigned পূর্ণসংখ্যা n)
{
	পূর্ণসংখ্যা pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask;

	/* check parameters */
	যদি (n >= PMP_গুনতি)
		ফেরত SBI_EINVAL;

	/* calculate PMP register and offset */
#যদি __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#নতুবা_যদি __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#অন্যথা
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_shift);
	pmp->cfg = (csr_read_অঙ্ক(pmpcfg_csr) & cfgmask) >> pmpcfg_shift;
	pmp->addr = csr_read_অঙ্ক(pmpaddr_csr);

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা hart_pmp_write(pmp_t *pmp, unsigned পূর্ণসংখ্যা n)
{
	পূর্ণসংখ্যা pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask, pmpcfg;

	/* check parameters */
	যদি (n >= PMP_গুনতি)
		ফেরত SBI_EINVAL;

	/* calculate PMP register and offset */
#যদি __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#নতুবা_যদি __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#অন্যথা
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_অঙ্ক(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_shift);
	pmpcfg  = (csr_read_অঙ্ক(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((unsigned long)pmp->cfg << pmpcfg_shift) & ~cfgmask);
	csr_write_অঙ্ক(pmpcfg_csr, pmpcfg);

	ফেরত SBI_OK;
}

পূর্ণসংখ্যা sbi_hart_pmp_disable(unsigned পূর্ণসংখ্যা n)
{
	pmp_t pmp;
	পূর্ণসংখ্যা rc;

	rc = hart_pmp_read(&pmp, n);
	যদি (rc)
		ফেরত rc;

	pmp.cfg = 0;

	ফেরত hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(unsigned পূর্ণসংখ্যা n)
{
	pmp_t pmp;

	যদি (hart_pmp_read(&pmp, n) != SBI_OK)
		ফেরত false;

	ফেরত sbi_pmp_is_enabled(&pmp);
}

পূর্ণসংখ্যা sbi_hart_pmp_set(unsigned পূর্ণসংখ্যা n, unsigned long prot, unsigned long addr,
		     unsigned long log2len)
{
	pmp_t pmp;
	পূর্ণসংখ্যা rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	যদি (rc)
		ফেরত rc;

	ফেরত hart_pmp_write(&pmp, n);
}

পূর্ণসংখ্যা sbi_hart_pmp_get(unsigned পূর্ণসংখ্যা n, unsigned long *prot_out, unsigned long *addr_out,
		     unsigned long *log2len)
{
	pmp_t pmp;
	পূর্ণসংখ্যা rc;

	rc = hart_pmp_read(&pmp, n);
	যদি (rc)
		ফেরত rc;

	ফেরত sbi_pmp_decode(&pmp, prot_out, addr_out, log2len);
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

স্থির DECLARE_BITMAP(fw_smepmp_ids, PMP_গুনতি);
স্থির bool fw_smepmp_ids_inited;

unsigned পূর্ণসংখ্যা sbi_hart_pmp_গুনতি(কাঠামো sbi_scratch *scratch)
{
	কাঠামো sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	ফেরত hfeatures->pmp_গুনতি;
}

unsigned পূর্ণসংখ্যা sbi_hart_pmp_log2gran(কাঠামো sbi_scratch *scratch)
{
	কাঠামো sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	ফেরত hfeatures->pmp_log2gran;
}

unsigned পূর্ণসংখ্যা sbi_hart_pmp_addrbits(কাঠামো sbi_scratch *scratch)
{
	কাঠামো sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	ফেরত hfeatures->pmp_addr_bits;
}

bool sbi_hart_smepmp_is_fw_region(unsigned পূর্ণসংখ্যা pmp_idx)
{
	যদি (!fw_smepmp_ids_inited)
		ফেরত false;

	ফেরত bitmap_test(fw_smepmp_ids, pmp_idx) ? true : false;
}

শূন্য sbi_hart_pmp_fence(শূন্য)
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
	যদি (misa_extension('S')) {
		__sbi_sfence_vma_all();

		/*
		 * If hypervisor mode is supported, flush caching
		 * structures in guest mode too.
		 */
		যদি (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}
}

স্থির শূন্য sbi_hart_smepmp_set(কাঠামো sbi_scratch *scratch,
				কাঠামো sbi_domain *dom,
				কাঠামো sbi_domain_memregion *reg,
				unsigned পূর্ণসংখ্যা pmp_idx,
				unsigned পূর্ণসংখ্যা pmp_flags,
				unsigned পূর্ণসংখ্যা pmp_log2gran,
				unsigned long pmp_addr_max)
{
	unsigned long pmp_addr = reg->base >> PMP_SHIFT;

	যদি (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_set(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->base, reg->order);
		sbi_hart_pmp_set(pmp_idx, pmp_flags, reg->base, reg->order);
	} অন্যথা {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->base,
			   reg->order);
	}
}

স্থির bool is_valid_pmp_idx(unsigned পূর্ণসংখ্যা pmp_গুনতি, unsigned পূর্ণসংখ্যা pmp_idx)
{
	যদি (pmp_গুনতি > pmp_idx)
		ফেরত true;

	sbi_printf("error: insufficient PMP entries\n");
	ফেরত false;
}

স্থির পূর্ণসংখ্যা sbi_hart_smepmp_configure(কাঠামো sbi_scratch *scratch,
				     কাঠামো sbi_domain *dom)
{
	কাঠামো sbi_domain_memregion *reg;
	unsigned পূর্ণসংখ্যা pmp_log2gran, pmp_bits;
	unsigned পূর্ণসংখ্যা pmp_idx, pmp_গুনতি;
	unsigned long pmp_addr_max;
	unsigned পূর্ণসংখ্যা pmp_flags;

	pmp_গুনতি = sbi_hart_pmp_গুনতি(scratch);
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
		যদি (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		যদি (!is_valid_pmp_idx(pmp_গুনতি, pmp_idx))
			ফেরত SBI_EFAIL;

		/* Skip shared and SU-only regions */
		যদি (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			চালু;
		}

		/*
		 * Track firmware PMP entries to preserve them during
		 * domain switches. Under SmePMP, M-mode requires
		 * explicit PMP entries to access firmware code/data.
		 * These entries must remain enabled across domain
		 * context switches to prevent M-mode access faults.
		 */
		যদি (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags)) {
			যদি (fw_smepmp_ids_inited) {
				/* Check inconsistent firmware region */
				যদি (!sbi_hart_smepmp_is_fw_region(pmp_idx))
					ফেরত SBI_EINVAL;
			} অন্যথা {
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
		যদি (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		যদি (!is_valid_pmp_idx(pmp_গুনতি, pmp_idx))
			ফেরত SBI_EFAIL;

		/* Skip M-only regions */
		যদি (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			চালু;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_set(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	জন্য(; pmp_idx < pmp_গুনতি; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা sbi_hart_smepmp_অস্থায়ী_map_range(কাঠামো sbi_scratch *scratch,
				     unsigned long addr, unsigned long আকার_2)
{
	/* shared R/W access for M and S/U mode */
	unsigned পূর্ণসংখ্যা pmp_flags = (PMP_W | PMP_X);
	unsigned long order, base = 0;

	যদি (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_ENTRY))
		ফেরত SBI_ENOSPC;

	জন্য (order = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(আকার_2));
	     order <= __riscv_xlen; order++) {
		যদি (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			যদি ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + আকার_2 - 1UL)) &&
			    ((addr + আকার_2 - 1UL) < (base + (1UL << order))))
				বিরতি;
		} অন্যথা {
			ফেরত SBI_EFAIL;
		}
	}

	sbi_platform_pmp_set(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, base, order);
	sbi_hart_pmp_set(SBI_SMEPMP_RESV_ENTRY, pmp_flags, base, order);

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা sbi_hart_smepmp_অস্থায়ী_unmap_range(কাঠামো sbi_scratch *scratch,
				       unsigned long addr, unsigned long আকার_2)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY);
	ফেরত sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ENTRY);
}

স্থির পূর্ণসংখ্যা sbi_hart_oldpmp_configure(কাঠামো sbi_scratch *scratch,
				     কাঠামো sbi_domain *dom)
{
	কাঠামো sbi_domain_memregion *reg;
	unsigned long pmp_addr, pmp_addr_max;
	unsigned পূর্ণসংখ্যা pmp_log2gran, pmp_bits;
	unsigned পূর্ণসংখ্যা pmp_idx, pmp_গুনতি;
	unsigned পূর্ণসংখ্যা pmp_flags;

	pmp_গুনতি = sbi_hart_pmp_গুনতি(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_bits = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_bits) | ((1UL << pmp_bits) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		যদি (!is_valid_pmp_idx(pmp_গুনতি, pmp_idx))
			ফেরত SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->base >> PMP_SHIFT;
		যদি (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_set(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->base, reg->order);
			sbi_hart_pmp_set(pmp_idx++, pmp_flags, reg->base, reg->order);
		} অন্যথা {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->base,
				   reg->order);
		}
	}
	/* Disable remaining PMP entries */
	জন্য(; pmp_idx < pmp_গুনতি; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	ফেরত 0;
}

স্থির শূন্য sbi_hart_pmp_unconfigure(কাঠামো sbi_scratch *scratch,
				     কাঠামো sbi_domain *dom)
{
	পূর্ণসংখ্যা i, pmp_গুনতি = sbi_hart_pmp_গুনতি(scratch);

	জন্য (i = 0; i < pmp_গুনতি; i++) {
		/* Don't revoke firmware access permissions */
		যদি (sbi_hart_smepmp_is_fw_region(i))
			চালু;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

স্থির কাঠামো sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

স্থির কাঠামো sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.অস্থায়ী_map_range = sbi_hart_smepmp_অস্থায়ী_map_range,
	.অস্থায়ী_unmap_range = sbi_hart_smepmp_অস্থায়ী_unmap_range,
};

পূর্ণসংখ্যা sbi_hart_pmp_init(কাঠামো sbi_scratch *scratch)
{
	পূর্ণসংখ্যা rc;

	যদি (sbi_hart_pmp_গুনতি(scratch)) {
		যদি (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_register(&epmp_protection);
			যদি (rc)
				ফেরত rc;
		} অন্যথা {
			rc = sbi_hart_protection_register(&pmp_protection);
			যদি (rc)
				ফেরত rc;
		}
	}

	ফেরত 0;
}
