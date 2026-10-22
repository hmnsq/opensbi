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

ساكن صحيح hart_pmp_read(pmp_t *pmp, بنانشان صحيح n)
{
	صحيح pmpcfg_csr, pmpcfg_سرڪايو, pmpaddr_csr;
	بنانشان ڊگهو cfgmask;

	/* check parameters */
	إذا (n >= PMP_ڳڻپ)
		أرجع SBI_EINVAL;

	/* calculate PMP register and offset */
#إذا __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_سرڪايو = (n & 3) << 3;
#وإلاإذا __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_سرڪايو = (n & 7) << 3;
#وإلا
# error "Unexpected __riscv_xlen"
#جيڪڏهن_پورو
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_سرڪايو);
	pmp->cfg = (csr_read_انگ(pmpcfg_csr) & cfgmask) >> pmpcfg_سرڪايو;
	pmp->addr = csr_read_انگ(pmpaddr_csr);

	أرجع SBI_OK;
}

ساكن صحيح hart_pmp_write(pmp_t *pmp, بنانشان صحيح n)
{
	صحيح pmpcfg_csr, pmpcfg_سرڪايو, pmpaddr_csr;
	بنانشان ڊگهو cfgmask, pmpcfg;

	/* check parameters */
	إذا (n >= PMP_ڳڻپ)
		أرجع SBI_EINVAL;

	/* calculate PMP register and offset */
#إذا __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_سرڪايو = (n & 3) << 3;
#وإلاإذا __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_سرڪايو = (n & 7) << 3;
#وإلا
# error "Unexpected __riscv_xlen"
#جيڪڏهن_پورو
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_انگ(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_سرڪايو);
	pmpcfg  = (csr_read_انگ(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((بنانشان ڊگهو)pmp->cfg << pmpcfg_سرڪايو) & ~cfgmask);
	csr_write_انگ(pmpcfg_csr, pmpcfg);

	أرجع SBI_OK;
}

صحيح sbi_hart_pmp_disable(بنانشان صحيح n)
{
	pmp_t pmp;
	صحيح rc;

	rc = hart_pmp_read(&pmp, n);
	إذا (rc)
		أرجع rc;

	pmp.cfg = 0;

	أرجع hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(بنانشان صحيح n)
{
	pmp_t pmp;

	إذا (hart_pmp_read(&pmp, n) != SBI_OK)
		أرجع false;

	أرجع sbi_pmp_is_enabled(&pmp);
}

صحيح sbi_hart_pmp_سيٽ(بنانشان صحيح n, بنانشان ڊگهو prot, بنانشان ڊگهو addr,
		     بنانشان ڊگهو log2len)
{
	pmp_t pmp;
	صحيح rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	إذا (rc)
		أرجع rc;

	أرجع hart_pmp_write(&pmp, n);
}

صحيح sbi_hart_pmp_get(بنانشان صحيح n, بنانشان ڊگهو *prot_ٻاهريون, بنانشان ڊگهو *addr_ٻاهريون,
		     بنانشان ڊگهو *log2len)
{
	pmp_t pmp;
	صحيح rc;

	rc = hart_pmp_read(&pmp, n);
	إذا (rc)
		أرجع rc;

	أرجع sbi_pmp_decode(&pmp, prot_ٻاهريون, addr_ٻاهريون, log2len);
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
#وصف_ڪريو SBI_SMEPMP_RESV_داخلا		0

ساكن DECLARE_BITMAP(fw_smepmp_ids, PMP_ڳڻپ);
ساكن bool fw_smepmp_ids_inited;

بنانشان صحيح sbi_hart_pmp_ڳڻپ(بنية sbi_scratch *scratch)
{
	بنية sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	أرجع hfeatures->pmp_ڳڻپ;
}

بنانشان صحيح sbi_hart_pmp_log2gran(بنية sbi_scratch *scratch)
{
	بنية sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	أرجع hfeatures->pmp_log2gran;
}

بنانشان صحيح sbi_hart_pmp_addrbits(بنية sbi_scratch *scratch)
{
	بنية sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	أرجع hfeatures->pmp_addr_بٽ;
}

bool sbi_hart_smepmp_is_fw_region(بنانشان صحيح pmp_idx)
{
	إذا (!fw_smepmp_ids_inited)
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
	إذا (misa_extension('S')) {
		__sbi_sfence_vma_all();

		/*
		 * If hypervisor mode is supported, flush caching
		 * structures in guest mode too.
		 */
		إذا (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}
}

ساكن فراغ sbi_hart_smepmp_سيٽ(بنية sbi_scratch *scratch,
				بنية sbi_domain *dom,
				بنية sbi_domain_memregion *reg,
				بنانشان صحيح pmp_idx,
				بنانشان صحيح pmp_flags,
				بنانشان صحيح pmp_log2gran,
				بنانشان ڊگهو pmp_addr_max)
{
	بنانشان ڊگهو pmp_addr = reg->بنياد_2 >> PMP_سرڪايو;

	إذا (pmp_log2gran <= reg->ترتيب && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_سيٽ(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->بنياد_2, reg->ترتيب);
		sbi_hart_pmp_سيٽ(pmp_idx, pmp_flags, reg->بنياد_2, reg->ترتيب);
	} وإلا {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->بنياد_2,
			   reg->ترتيب);
	}
}

ساكن bool is_valid_pmp_idx(بنانشان صحيح pmp_ڳڻپ, بنانشان صحيح pmp_idx)
{
	إذا (pmp_ڳڻپ > pmp_idx)
		أرجع true;

	sbi_printf("error: insufficient PMP entries\n");
	أرجع false;
}

ساكن صحيح sbi_hart_smepmp_configure(بنية sbi_scratch *scratch,
				     بنية sbi_domain *dom)
{
	بنية sbi_domain_memregion *reg;
	بنانشان صحيح pmp_log2gran, pmp_بٽ;
	بنانشان صحيح pmp_idx, pmp_ڳڻپ;
	بنانشان ڊگهو pmp_addr_max;
	بنانشان صحيح pmp_flags;

	pmp_ڳڻپ = sbi_hart_pmp_ڳڻپ(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_بٽ = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_بٽ) | ((1UL << pmp_بٽ) - 1);

	/*
	 * Set the RLB so that, we can write to PMP entries without
	 * enforcement even if some entries are locked.
	 */
	csr_سيٽ(CSR_MSECCFG, MSECCFG_RLB);

	/* Disable the reserved entry */
	sbi_hart_pmp_disable(SBI_SMEPMP_RESV_داخلا);

	/* Program M-only regions when MML is not set. */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		إذا (pmp_idx == SBI_SMEPMP_RESV_داخلا)
			pmp_idx++;
		إذا (!is_valid_pmp_idx(pmp_ڳڻپ, pmp_idx))
			أرجع SBI_EFAIL;

		/* Skip shared and SU-only regions */
		إذا (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
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
		إذا (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags)) {
			إذا (fw_smepmp_ids_inited) {
				/* Check inconsistent firmware region */
				إذا (!sbi_hart_smepmp_is_fw_region(pmp_idx))
					أرجع SBI_EINVAL;
			} وإلا {
				bitmap_سيٽ(fw_smepmp_ids, pmp_idx, 1);
			}
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_سيٽ(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}

	fw_smepmp_ids_inited = true;

	/* Set the MML to enforce new encoding */
	csr_سيٽ(CSR_MSECCFG, MSECCFG_MML);

	/* Program shared and SU-only regions */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		إذا (pmp_idx == SBI_SMEPMP_RESV_داخلا)
			pmp_idx++;
		إذا (!is_valid_pmp_idx(pmp_ڳڻپ, pmp_idx))
			أرجع SBI_EFAIL;

		/* Skip M-only regions */
		إذا (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			استمر;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_سيٽ(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	لكل(; pmp_idx < pmp_ڳڻپ; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	أرجع 0;
}

ساكن صحيح sbi_hart_smepmp_عارضي_map_range(بنية sbi_scratch *scratch,
				     بنانشان ڊگهو addr, بنانشان ڊگهو سائيز)
{
	/* shared R/W access for M and S/U mode */
	بنانشان صحيح pmp_flags = (PMP_W | PMP_X);
	بنانشان ڊگهو ترتيب, بنياد_2 = 0;

	إذا (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_داخلا))
		أرجع SBI_ENOSPC;

	لكل (ترتيب = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(سائيز));
	     ترتيب <= __riscv_xlen; ترتيب++) {
		إذا (ترتيب < __riscv_xlen) {
			بنياد_2 = addr & ~((1UL << ترتيب) - 1UL);
			إذا ((بنياد_2 <= addr) &&
			    (addr < (بنياد_2 + (1UL << ترتيب))) &&
			    (بنياد_2 <= (addr + سائيز - 1UL)) &&
			    ((addr + سائيز - 1UL) < (بنياد_2 + (1UL << ترتيب))))
				اكسر;
		} وإلا {
			أرجع SBI_EFAIL;
		}
	}

	sbi_platform_pmp_سيٽ(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_داخلا,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, بنياد_2, ترتيب);
	sbi_hart_pmp_سيٽ(SBI_SMEPMP_RESV_داخلا, pmp_flags, بنياد_2, ترتيب);

	أرجع SBI_OK;
}

ساكن صحيح sbi_hart_smepmp_عارضي_unmap_range(بنية sbi_scratch *scratch,
				       بنانشان ڊگهو addr, بنانشان ڊگهو سائيز)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_داخلا);
	أرجع sbi_hart_pmp_disable(SBI_SMEPMP_RESV_داخلا);
}

ساكن صحيح sbi_hart_oldpmp_configure(بنية sbi_scratch *scratch,
				     بنية sbi_domain *dom)
{
	بنية sbi_domain_memregion *reg;
	بنانشان ڊگهو pmp_addr, pmp_addr_max;
	بنانشان صحيح pmp_log2gran, pmp_بٽ;
	بنانشان صحيح pmp_idx, pmp_ڳڻپ;
	بنانشان صحيح pmp_flags;

	pmp_ڳڻپ = sbi_hart_pmp_ڳڻپ(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_بٽ = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_بٽ) | ((1UL << pmp_بٽ) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		إذا (!is_valid_pmp_idx(pmp_ڳڻپ, pmp_idx))
			أرجع SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->بنياد_2 >> PMP_سرڪايو;
		إذا (pmp_log2gran <= reg->ترتيب && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_سيٽ(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->بنياد_2, reg->ترتيب);
			sbi_hart_pmp_سيٽ(pmp_idx++, pmp_flags, reg->بنياد_2, reg->ترتيب);
		} وإلا {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->بنياد_2,
				   reg->ترتيب);
		}
	}
	/* Disable remaining PMP entries */
	لكل(; pmp_idx < pmp_ڳڻپ; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	أرجع 0;
}

ساكن فراغ sbi_hart_pmp_unconfigure(بنية sbi_scratch *scratch,
				     بنية sbi_domain *dom)
{
	صحيح i, pmp_ڳڻپ = sbi_hart_pmp_ڳڻپ(scratch);

	لكل (i = 0; i < pmp_ڳڻپ; i++) {
		/* Don't revoke firmware access permissions */
		إذا (sbi_hart_smepmp_is_fw_region(i))
			استمر;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

ساكن بنية sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_يادگيري,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

ساكن بنية sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_يادگيري,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.عارضي_map_range = sbi_hart_smepmp_عارضي_map_range,
	.عارضي_unmap_range = sbi_hart_smepmp_عارضي_unmap_range,
};

صحيح sbi_hart_pmp_init(بنية sbi_scratch *scratch)
{
	صحيح rc;

	إذا (sbi_hart_pmp_ڳڻپ(scratch)) {
		إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_رجسٽر(&epmp_protection);
			إذا (rc)
				أرجع rc;
		} وإلا {
			rc = sbi_hart_protection_رجسٽر(&pmp_protection);
			إذا (rc)
				أرجع rc;
		}
	}

	أرجع 0;
}
