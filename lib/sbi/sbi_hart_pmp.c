/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#شامل <sbi/sbi_bitmap.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hart_protection.h>
#شامل <sbi/sbi_hfence.h>
#شامل <sbi/sbi_math.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_pmp.h>
#شامل <sbi/sbi_tlb.h>
#شامل <sbi/riscv_asm.h>

ایستا صحیح hart_pmp_read(pmp_t *pmp, unsigned صحیح n)
{
	صحیح pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask;

	/* check parameters */
	اگر (n >= PMP_شمار)
		بازگشت SBI_EINVAL;

	/* calculate PMP register and offset */
#اگر __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#وگرنهاگر __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#وگرنه
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_shift);
	pmp->cfg = (csr_read_رقم(pmpcfg_csr) & cfgmask) >> pmpcfg_shift;
	pmp->addr = csr_read_رقم(pmpaddr_csr);

	بازگشت SBI_OK;
}

ایستا صحیح hart_pmp_write(pmp_t *pmp, unsigned صحیح n)
{
	صحیح pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask, pmpcfg;

	/* check parameters */
	اگر (n >= PMP_شمار)
		بازگشت SBI_EINVAL;

	/* calculate PMP register and offset */
#اگر __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#وگرنهاگر __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#وگرنه
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_رقم(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_shift);
	pmpcfg  = (csr_read_رقم(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((unsigned long)pmp->cfg << pmpcfg_shift) & ~cfgmask);
	csr_write_رقم(pmpcfg_csr, pmpcfg);

	بازگشت SBI_OK;
}

صحیح sbi_hart_pmp_disable(unsigned صحیح n)
{
	pmp_t pmp;
	صحیح rc;

	rc = hart_pmp_read(&pmp, n);
	اگر (rc)
		بازگشت rc;

	pmp.cfg = 0;

	بازگشت hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(unsigned صحیح n)
{
	pmp_t pmp;

	اگر (hart_pmp_read(&pmp, n) != SBI_OK)
		بازگشت false;

	بازگشت sbi_pmp_is_enabled(&pmp);
}

صحیح sbi_hart_pmp_set(unsigned صحیح n, unsigned long prot, unsigned long addr,
		     unsigned long log2len)
{
	pmp_t pmp;
	صحیح rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	اگر (rc)
		بازگشت rc;

	بازگشت hart_pmp_write(&pmp, n);
}

صحیح sbi_hart_pmp_get(unsigned صحیح n, unsigned long *prot_out, unsigned long *addr_out,
		     unsigned long *log2len)
{
	pmp_t pmp;
	صحیح rc;

	rc = hart_pmp_read(&pmp, n);
	اگر (rc)
		بازگشت rc;

	بازگشت sbi_pmp_decode(&pmp, prot_out, addr_out, log2len);
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

ایستا DECLARE_BITMAP(fw_smepmp_ids, PMP_شمار);
ایستا bool fw_smepmp_ids_inited;

unsigned صحیح sbi_hart_pmp_شمار(ساختار sbi_scratch *scratch)
{
	ساختار sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	بازگشت hfeatures->pmp_شمار;
}

unsigned صحیح sbi_hart_pmp_log2gran(ساختار sbi_scratch *scratch)
{
	ساختار sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	بازگشت hfeatures->pmp_log2gran;
}

unsigned صحیح sbi_hart_pmp_addrbits(ساختار sbi_scratch *scratch)
{
	ساختار sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	بازگشت hfeatures->pmp_addr_bits;
}

bool sbi_hart_smepmp_is_fw_region(unsigned صحیح pmp_idx)
{
	اگر (!fw_smepmp_ids_inited)
		بازگشت false;

	بازگشت bitmap_test(fw_smepmp_ids, pmp_idx) ? true : false;
}

تهی sbi_hart_pmp_fence(تهی)
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
	اگر (misa_extension('S')) {
		__sbi_sfence_vma_all();

		/*
		 * If hypervisor mode is supported, flush caching
		 * structures in guest mode too.
		 */
		اگر (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}
}

ایستا تهی sbi_hart_smepmp_set(ساختار sbi_scratch *scratch,
				ساختار sbi_domain *dom,
				ساختار sbi_domain_memregion *reg,
				unsigned صحیح pmp_idx,
				unsigned صحیح pmp_flags,
				unsigned صحیح pmp_log2gran,
				unsigned long pmp_addr_max)
{
	unsigned long pmp_addr = reg->base >> PMP_SHIFT;

	اگر (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_set(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->base, reg->order);
		sbi_hart_pmp_set(pmp_idx, pmp_flags, reg->base, reg->order);
	} وگرنه {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->base,
			   reg->order);
	}
}

ایستا bool is_valid_pmp_idx(unsigned صحیح pmp_شمار, unsigned صحیح pmp_idx)
{
	اگر (pmp_شمار > pmp_idx)
		بازگشت true;

	sbi_printf("error: insufficient PMP entries\n");
	بازگشت false;
}

ایستا صحیح sbi_hart_smepmp_configure(ساختار sbi_scratch *scratch,
				     ساختار sbi_domain *dom)
{
	ساختار sbi_domain_memregion *reg;
	unsigned صحیح pmp_log2gran, pmp_bits;
	unsigned صحیح pmp_idx, pmp_شمار;
	unsigned long pmp_addr_max;
	unsigned صحیح pmp_flags;

	pmp_شمار = sbi_hart_pmp_شمار(scratch);
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
		اگر (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		اگر (!is_valid_pmp_idx(pmp_شمار, pmp_idx))
			بازگشت SBI_EFAIL;

		/* Skip shared and SU-only regions */
		اگر (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			ادامه;
		}

		/*
		 * Track firmware PMP entries to preserve them during
		 * domain switches. Under SmePMP, M-mode requires
		 * explicit PMP entries to access firmware code/data.
		 * These entries must remain enabled across domain
		 * context switches to prevent M-mode access faults.
		 */
		اگر (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags)) {
			اگر (fw_smepmp_ids_inited) {
				/* Check inconsistent firmware region */
				اگر (!sbi_hart_smepmp_is_fw_region(pmp_idx))
					بازگشت SBI_EINVAL;
			} وگرنه {
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
		اگر (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		اگر (!is_valid_pmp_idx(pmp_شمار, pmp_idx))
			بازگشت SBI_EFAIL;

		/* Skip M-only regions */
		اگر (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			ادامه;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_set(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	برای(; pmp_idx < pmp_شمار; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	بازگشت 0;
}

ایستا صحیح sbi_hart_smepmp_temp_map_range(ساختار sbi_scratch *scratch,
				     unsigned long addr, unsigned long size)
{
	/* shared R/W access for M and S/U mode */
	unsigned صحیح pmp_flags = (PMP_W | PMP_X);
	unsigned long order, base = 0;

	اگر (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_ENTRY))
		بازگشت SBI_ENOSPC;

	برای (order = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(size));
	     order <= __riscv_xlen; order++) {
		اگر (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			اگر ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				بشکن;
		} وگرنه {
			بازگشت SBI_EFAIL;
		}
	}

	sbi_platform_pmp_set(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, base, order);
	sbi_hart_pmp_set(SBI_SMEPMP_RESV_ENTRY, pmp_flags, base, order);

	بازگشت SBI_OK;
}

ایستا صحیح sbi_hart_smepmp_temp_unmap_range(ساختار sbi_scratch *scratch,
				       unsigned long addr, unsigned long size)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY);
	بازگشت sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ENTRY);
}

ایستا صحیح sbi_hart_oldpmp_configure(ساختار sbi_scratch *scratch,
				     ساختار sbi_domain *dom)
{
	ساختار sbi_domain_memregion *reg;
	unsigned long pmp_addr, pmp_addr_max;
	unsigned صحیح pmp_log2gran, pmp_bits;
	unsigned صحیح pmp_idx, pmp_شمار;
	unsigned صحیح pmp_flags;

	pmp_شمار = sbi_hart_pmp_شمار(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_bits = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_bits) | ((1UL << pmp_bits) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		اگر (!is_valid_pmp_idx(pmp_شمار, pmp_idx))
			بازگشت SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->base >> PMP_SHIFT;
		اگر (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_set(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->base, reg->order);
			sbi_hart_pmp_set(pmp_idx++, pmp_flags, reg->base, reg->order);
		} وگرنه {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->base,
				   reg->order);
		}
	}
	/* Disable remaining PMP entries */
	برای(; pmp_idx < pmp_شمار; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	بازگشت 0;
}

ایستا تهی sbi_hart_pmp_unconfigure(ساختار sbi_scratch *scratch,
				     ساختار sbi_domain *dom)
{
	صحیح i, pmp_شمار = sbi_hart_pmp_شمار(scratch);

	برای (i = 0; i < pmp_شمار; i++) {
		/* Don't revoke firmware access permissions */
		اگر (sbi_hart_smepmp_is_fw_region(i))
			ادامه;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

ایستا ساختار sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

ایستا ساختار sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.temp_map_range = sbi_hart_smepmp_temp_map_range,
	.temp_unmap_range = sbi_hart_smepmp_temp_unmap_range,
};

صحیح sbi_hart_pmp_init(ساختار sbi_scratch *scratch)
{
	صحیح rc;

	اگر (sbi_hart_pmp_شمار(scratch)) {
		اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_register(&epmp_protection);
			اگر (rc)
				بازگشت rc;
		} وگرنه {
			rc = sbi_hart_protection_register(&pmp_protection);
			اگر (rc)
				بازگشت rc;
		}
	}

	بازگشت 0;
}
