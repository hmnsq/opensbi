/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#ضمن <sbi/sbi_bitmap.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_hart_protection.h>
#ضمن <sbi/sbi_hfence.h>
#ضمن <sbi/sbi_math.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_pmp.h>
#ضمن <sbi/sbi_tlb.h>
#ضمن <sbi/riscv_asm.h>

ساكن صحيح hart_pmp_read(pmp_t *pmp, unsigned صحيح n)
{
	صحيح pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask;

	/* check parameters */
	𐡄𐡍 (n >= PMP_COUNT)
		أرجع SBI_EINVAL;

	/* calculate PMP register and offset */
#𐡄𐡍 __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#وإلاإذا __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#وإلا
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_shift);
	pmp->cfg = (csr_read_num(pmpcfg_csr) & cfgmask) >> pmpcfg_shift;
	pmp->addr = csr_read_num(pmpaddr_csr);

	أرجع SBI_OK;
}

ساكن صحيح hart_pmp_write(pmp_t *pmp, unsigned صحيح n)
{
	صحيح pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask, pmpcfg;

	/* check parameters */
	𐡄𐡍 (n >= PMP_COUNT)
		أرجع SBI_EINVAL;

	/* calculate PMP register and offset */
#𐡄𐡍 __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#وإلاإذا __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#وإلا
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_num(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_shift);
	pmpcfg  = (csr_read_num(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((unsigned long)pmp->cfg << pmpcfg_shift) & ~cfgmask);
	csr_write_num(pmpcfg_csr, pmpcfg);

	أرجع SBI_OK;
}

صحيح sbi_hart_pmp_disable(unsigned صحيح n)
{
	pmp_t pmp;
	صحيح rc;

	rc = hart_pmp_read(&pmp, n);
	𐡄𐡍 (rc)
		أرجع rc;

	pmp.cfg = 0;

	أرجع hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(unsigned صحيح n)
{
	pmp_t pmp;

	𐡄𐡍 (hart_pmp_read(&pmp, n) != SBI_OK)
		أرجع false;

	أرجع sbi_pmp_is_enabled(&pmp);
}

صحيح sbi_hart_pmp_set(unsigned صحيح n, unsigned long prot, unsigned long addr,
		     unsigned long log2len)
{
	pmp_t pmp;
	صحيح rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	𐡄𐡍 (rc)
		أرجع rc;

	أرجع hart_pmp_write(&pmp, n);
}

صحيح sbi_hart_pmp_get(unsigned صحيح n, unsigned long *prot_out, unsigned long *addr_out,
		     unsigned long *log2len)
{
	pmp_t pmp;
	صحيح rc;

	rc = hart_pmp_read(&pmp, n);
	𐡄𐡍 (rc)
		أرجع rc;

	أرجع sbi_pmp_decode(&pmp, prot_out, addr_out, log2len);
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

ساكن DECLARE_BITMAP(fw_smepmp_ids, PMP_COUNT);
ساكن bool fw_smepmp_ids_inited;

unsigned صحيح sbi_hart_pmp_count(بنية sbi_scratch *scratch)
{
	بنية sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	أرجع hfeatures->pmp_count;
}

unsigned صحيح sbi_hart_pmp_log2gran(بنية sbi_scratch *scratch)
{
	بنية sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	أرجع hfeatures->pmp_log2gran;
}

unsigned صحيح sbi_hart_pmp_addrbits(بنية sbi_scratch *scratch)
{
	بنية sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	أرجع hfeatures->pmp_addr_bits;
}

bool sbi_hart_smepmp_is_fw_region(unsigned صحيح pmp_idx)
{
	𐡄𐡍 (!fw_smepmp_ids_inited)
		أرجع false;

	أرجع bitmap_test(fw_smepmp_ids, pmp_idx) ? true : false;
}

فراغ sbi_hart_pmp_fence(فراغ)
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
	𐡄𐡍 (misa_extension('S')) {
		__sbi_sfence_vma_all();

		/*
		 * If hypervisor mode is supported, flush caching
		 * structures in guest mode too.
		 */
		𐡄𐡍 (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}
}

ساكن فراغ sbi_hart_smepmp_set(بنية sbi_scratch *scratch,
				بنية sbi_domain *dom,
				بنية sbi_domain_memregion *reg,
				unsigned صحيح pmp_idx,
				unsigned صحيح pmp_flags,
				unsigned صحيح pmp_log2gran,
				unsigned long pmp_addr_max)
{
	unsigned long pmp_addr = reg->base >> PMP_SHIFT;

	𐡄𐡍 (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_set(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->base, reg->order);
		sbi_hart_pmp_set(pmp_idx, pmp_flags, reg->base, reg->order);
	} وإلا {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->base,
			   reg->order);
	}
}

ساكن bool is_valid_pmp_idx(unsigned صحيح pmp_count, unsigned صحيح pmp_idx)
{
	𐡄𐡍 (pmp_count > pmp_idx)
		أرجع true;

	sbi_printf("error: insufficient PMP entries\n");
	أرجع false;
}

ساكن صحيح sbi_hart_smepmp_configure(بنية sbi_scratch *scratch,
				     بنية sbi_domain *dom)
{
	بنية sbi_domain_memregion *reg;
	unsigned صحيح pmp_log2gran, pmp_bits;
	unsigned صحيح pmp_idx, pmp_count;
	unsigned long pmp_addr_max;
	unsigned صحيح pmp_flags;

	pmp_count = sbi_hart_pmp_count(scratch);
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
		𐡄𐡍 (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		𐡄𐡍 (!is_valid_pmp_idx(pmp_count, pmp_idx))
			أرجع SBI_EFAIL;

		/* Skip shared and SU-only regions */
		𐡄𐡍 (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			استمر;
		}

		/*
		 * Track firmware PMP entries to preserve them during
		 * domain switches. Under SmePMP, M-mode requires
		 * explicit PMP entries to access firmware code/data.
		 * These entries must remain enabled across domain
		 * context switches to prevent M-mode access faults.
		 */
		𐡄𐡍 (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags)) {
			𐡄𐡍 (fw_smepmp_ids_inited) {
				/* Check inconsistent firmware region */
				𐡄𐡍 (!sbi_hart_smepmp_is_fw_region(pmp_idx))
					أرجع SBI_EINVAL;
			} وإلا {
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
		𐡄𐡍 (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		𐡄𐡍 (!is_valid_pmp_idx(pmp_count, pmp_idx))
			أرجع SBI_EFAIL;

		/* Skip M-only regions */
		𐡄𐡍 (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			استمر;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_set(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	لكل(; pmp_idx < pmp_count; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	أرجع 0;
}

ساكن صحيح sbi_hart_smepmp_temp_map_range(بنية sbi_scratch *scratch,
				     unsigned long addr, unsigned long size)
{
	/* shared R/W access for M and S/U mode */
	unsigned صحيح pmp_flags = (PMP_W | PMP_X);
	unsigned long order, base = 0;

	𐡄𐡍 (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_ENTRY))
		أرجع SBI_ENOSPC;

	لكل (order = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(size));
	     order <= __riscv_xlen; order++) {
		𐡄𐡍 (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			𐡄𐡍 ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				اكسر;
		} وإلا {
			أرجع SBI_EFAIL;
		}
	}

	sbi_platform_pmp_set(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, base, order);
	sbi_hart_pmp_set(SBI_SMEPMP_RESV_ENTRY, pmp_flags, base, order);

	أرجع SBI_OK;
}

ساكن صحيح sbi_hart_smepmp_temp_unmap_range(بنية sbi_scratch *scratch,
				       unsigned long addr, unsigned long size)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY);
	أرجع sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ENTRY);
}

ساكن صحيح sbi_hart_oldpmp_configure(بنية sbi_scratch *scratch,
				     بنية sbi_domain *dom)
{
	بنية sbi_domain_memregion *reg;
	unsigned long pmp_addr, pmp_addr_max;
	unsigned صحيح pmp_log2gran, pmp_bits;
	unsigned صحيح pmp_idx, pmp_count;
	unsigned صحيح pmp_flags;

	pmp_count = sbi_hart_pmp_count(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_bits = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_bits) | ((1UL << pmp_bits) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		𐡄𐡍 (!is_valid_pmp_idx(pmp_count, pmp_idx))
			أرجع SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->base >> PMP_SHIFT;
		𐡄𐡍 (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_set(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->base, reg->order);
			sbi_hart_pmp_set(pmp_idx++, pmp_flags, reg->base, reg->order);
		} وإلا {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->base,
				   reg->order);
		}
	}
	/* Disable remaining PMP entries */
	لكل(; pmp_idx < pmp_count; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	أرجع 0;
}

ساكن فراغ sbi_hart_pmp_unconfigure(بنية sbi_scratch *scratch,
				     بنية sbi_domain *dom)
{
	صحيح i, pmp_count = sbi_hart_pmp_count(scratch);

	لكل (i = 0; i < pmp_count; i++) {
		/* Don't revoke firmware access permissions */
		𐡄𐡍 (sbi_hart_smepmp_is_fw_region(i))
			استمر;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

ساكن بنية sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

ساكن بنية sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.temp_map_range = sbi_hart_smepmp_temp_map_range,
	.temp_unmap_range = sbi_hart_smepmp_temp_unmap_range,
};

صحيح sbi_hart_pmp_init(بنية sbi_scratch *scratch)
{
	صحيح rc;

	𐡄𐡍 (sbi_hart_pmp_count(scratch)) {
		𐡄𐡍 (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_register(&epmp_protection);
			𐡄𐡍 (rc)
				أرجع rc;
		} وإلا {
			rc = sbi_hart_protection_register(&pmp_protection);
			𐡄𐡍 (rc)
				أرجع rc;
		}
	}

	أرجع 0;
}
