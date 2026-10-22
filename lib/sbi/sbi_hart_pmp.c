/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#include <sbi/sbi_bitmap.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_hart_protection.h>
#include <sbi/sbi_hfence.h>
#include <sbi/sbi_math.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_pmp.h>
#include <sbi/sbi_tlb.h>
#include <sbi/riscv_asm.h>

static số_nguyên hart_pmp_read(pmp_t *pmp, unsigned số_nguyên n)
{
	số_nguyên pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask;

	/* check parameters */
	nếu (n >= PMP_COUNT)
		trả_về SBI_EINVAL;

	/* calculate PMP register and offset */
#nếu __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#elif __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#không_thì
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_shift);
	pmp->cfg = (csr_read_num(pmpcfg_csr) & cfgmask) >> pmpcfg_shift;
	pmp->addr = csr_read_num(pmpaddr_csr);

	trả_về SBI_OK;
}

static số_nguyên hart_pmp_write(pmp_t *pmp, unsigned số_nguyên n)
{
	số_nguyên pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask, pmpcfg;

	/* check parameters */
	nếu (n >= PMP_COUNT)
		trả_về SBI_EINVAL;

	/* calculate PMP register and offset */
#nếu __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#elif __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#không_thì
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_num(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_shift);
	pmpcfg  = (csr_read_num(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((unsigned long)pmp->cfg << pmpcfg_shift) & ~cfgmask);
	csr_write_num(pmpcfg_csr, pmpcfg);

	trả_về SBI_OK;
}

số_nguyên sbi_hart_pmp_disable(unsigned số_nguyên n)
{
	pmp_t pmp;
	số_nguyên rc;

	rc = hart_pmp_read(&pmp, n);
	nếu (rc)
		trả_về rc;

	pmp.cfg = 0;

	trả_về hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(unsigned số_nguyên n)
{
	pmp_t pmp;

	nếu (hart_pmp_read(&pmp, n) != SBI_OK)
		trả_về false;

	trả_về sbi_pmp_is_enabled(&pmp);
}

số_nguyên sbi_hart_pmp_set(unsigned số_nguyên n, unsigned long prot, unsigned long addr,
		     unsigned long log2len)
{
	pmp_t pmp;
	số_nguyên rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	nếu (rc)
		trả_về rc;

	trả_về hart_pmp_write(&pmp, n);
}

số_nguyên sbi_hart_pmp_get(unsigned số_nguyên n, unsigned long *prot_out, unsigned long *addr_out,
		     unsigned long *log2len)
{
	pmp_t pmp;
	số_nguyên rc;

	rc = hart_pmp_read(&pmp, n);
	nếu (rc)
		trả_về rc;

	trả_về sbi_pmp_decode(&pmp, prot_out, addr_out, log2len);
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

static DECLARE_BITMAP(fw_smepmp_ids, PMP_COUNT);
static bool fw_smepmp_ids_inited;

unsigned số_nguyên sbi_hart_pmp_count(struct sbi_scratch *scratch)
{
	struct sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	trả_về hfeatures->pmp_count;
}

unsigned số_nguyên sbi_hart_pmp_log2gran(struct sbi_scratch *scratch)
{
	struct sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	trả_về hfeatures->pmp_log2gran;
}

unsigned số_nguyên sbi_hart_pmp_addrbits(struct sbi_scratch *scratch)
{
	struct sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	trả_về hfeatures->pmp_addr_bits;
}

bool sbi_hart_smepmp_is_fw_region(unsigned số_nguyên pmp_idx)
{
	nếu (!fw_smepmp_ids_inited)
		trả_về false;

	trả_về bitmap_test(fw_smepmp_ids, pmp_idx) ? true : false;
}

void sbi_hart_pmp_fence(void)
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
	nếu (misa_extension('S')) {
		__sbi_sfence_vma_all();

		/*
		 * If hypervisor mode is supported, flush caching
		 * structures in guest mode too.
		 */
		nếu (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}
}

static void sbi_hart_smepmp_set(struct sbi_scratch *scratch,
				struct sbi_domain *dom,
				struct sbi_domain_memregion *reg,
				unsigned số_nguyên pmp_idx,
				unsigned số_nguyên pmp_flags,
				unsigned số_nguyên pmp_log2gran,
				unsigned long pmp_addr_max)
{
	unsigned long pmp_addr = reg->base >> PMP_SHIFT;

	nếu (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_set(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->base, reg->order);
		sbi_hart_pmp_set(pmp_idx, pmp_flags, reg->base, reg->order);
	} không_thì {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->base,
			   reg->order);
	}
}

static bool is_valid_pmp_idx(unsigned số_nguyên pmp_count, unsigned số_nguyên pmp_idx)
{
	nếu (pmp_count > pmp_idx)
		trả_về true;

	sbi_printf("error: insufficient PMP entries\n");
	trả_về false;
}

static số_nguyên sbi_hart_smepmp_configure(struct sbi_scratch *scratch,
				     struct sbi_domain *dom)
{
	struct sbi_domain_memregion *reg;
	unsigned số_nguyên pmp_log2gran, pmp_bits;
	unsigned số_nguyên pmp_idx, pmp_count;
	unsigned long pmp_addr_max;
	unsigned số_nguyên pmp_flags;

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
		nếu (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		nếu (!is_valid_pmp_idx(pmp_count, pmp_idx))
			trả_về SBI_EFAIL;

		/* Skip shared and SU-only regions */
		nếu (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			continue;
		}

		/*
		 * Track firmware PMP entries to preserve them during
		 * domain switches. Under SmePMP, M-mode requires
		 * explicit PMP entries to access firmware code/data.
		 * These entries must remain enabled across domain
		 * context switches to prevent M-mode access faults.
		 */
		nếu (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags)) {
			nếu (fw_smepmp_ids_inited) {
				/* Check inconsistent firmware region */
				nếu (!sbi_hart_smepmp_is_fw_region(pmp_idx))
					trả_về SBI_EINVAL;
			} không_thì {
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
		nếu (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		nếu (!is_valid_pmp_idx(pmp_count, pmp_idx))
			trả_về SBI_EFAIL;

		/* Skip M-only regions */
		nếu (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			continue;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_set(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	cho(; pmp_idx < pmp_count; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	trả_về 0;
}

static số_nguyên sbi_hart_smepmp_temp_map_range(struct sbi_scratch *scratch,
				     unsigned long addr, unsigned long size)
{
	/* shared R/W access for M and S/U mode */
	unsigned số_nguyên pmp_flags = (PMP_W | PMP_X);
	unsigned long order, base = 0;

	nếu (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_ENTRY))
		trả_về SBI_ENOSPC;

	cho (order = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(size));
	     order <= __riscv_xlen; order++) {
		nếu (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			nếu ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				break;
		} không_thì {
			trả_về SBI_EFAIL;
		}
	}

	sbi_platform_pmp_set(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, base, order);
	sbi_hart_pmp_set(SBI_SMEPMP_RESV_ENTRY, pmp_flags, base, order);

	trả_về SBI_OK;
}

static số_nguyên sbi_hart_smepmp_temp_unmap_range(struct sbi_scratch *scratch,
				       unsigned long addr, unsigned long size)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY);
	trả_về sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ENTRY);
}

static số_nguyên sbi_hart_oldpmp_configure(struct sbi_scratch *scratch,
				     struct sbi_domain *dom)
{
	struct sbi_domain_memregion *reg;
	unsigned long pmp_addr, pmp_addr_max;
	unsigned số_nguyên pmp_log2gran, pmp_bits;
	unsigned số_nguyên pmp_idx, pmp_count;
	unsigned số_nguyên pmp_flags;

	pmp_count = sbi_hart_pmp_count(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_bits = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_bits) | ((1UL << pmp_bits) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		nếu (!is_valid_pmp_idx(pmp_count, pmp_idx))
			trả_về SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->base >> PMP_SHIFT;
		nếu (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_set(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->base, reg->order);
			sbi_hart_pmp_set(pmp_idx++, pmp_flags, reg->base, reg->order);
		} không_thì {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->base,
				   reg->order);
		}
	}
	/* Disable remaining PMP entries */
	cho(; pmp_idx < pmp_count; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	trả_về 0;
}

static void sbi_hart_pmp_unconfigure(struct sbi_scratch *scratch,
				     struct sbi_domain *dom)
{
	số_nguyên i, pmp_count = sbi_hart_pmp_count(scratch);

	cho (i = 0; i < pmp_count; i++) {
		/* Don't revoke firmware access permissions */
		nếu (sbi_hart_smepmp_is_fw_region(i))
			continue;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

static struct sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

static struct sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.temp_map_range = sbi_hart_smepmp_temp_map_range,
	.temp_unmap_range = sbi_hart_smepmp_temp_unmap_range,
};

số_nguyên sbi_hart_pmp_init(struct sbi_scratch *scratch)
{
	số_nguyên rc;

	nếu (sbi_hart_pmp_count(scratch)) {
		nếu (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_register(&epmp_protection);
			nếu (rc)
				trả_về rc;
		} không_thì {
			rc = sbi_hart_protection_register(&pmp_protection);
			nếu (rc)
				trả_về rc;
		}
	}

	trả_về 0;
}
