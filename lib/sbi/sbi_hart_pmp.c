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

ساكن صحيح hart_pmp_read(pmp_t *pmp, بینښه صحيح n)
{
	صحيح pmpcfg_csr, pmpcfg_ښويدنه, pmpaddr_csr;
	بینښه اوږد cfgmask;

	/* check parameters */
	إذا (n >= PMP_شمېر)
		أرجع SBI_EINVAL;

	/* calculate PMP register and offset */
#إذا __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_ښويدنه = (n & 3) << 3;
#وإلاإذا __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_ښويدنه = (n & 7) << 3;
#وإلا
# error "Unexpected __riscv_xlen"
#د_که_پای
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_ښويدنه);
	pmp->cfg = (csr_read_عدد(pmpcfg_csr) & cfgmask) >> pmpcfg_ښويدنه;
	pmp->addr = csr_read_عدد(pmpaddr_csr);

	أرجع SBI_OK;
}

ساكن صحيح hart_pmp_write(pmp_t *pmp, بینښه صحيح n)
{
	صحيح pmpcfg_csr, pmpcfg_ښويدنه, pmpaddr_csr;
	بینښه اوږد cfgmask, pmpcfg;

	/* check parameters */
	إذا (n >= PMP_شمېر)
		أرجع SBI_EINVAL;

	/* calculate PMP register and offset */
#إذا __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_ښويدنه = (n & 3) << 3;
#وإلاإذا __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_ښويدنه = (n & 7) << 3;
#وإلا
# error "Unexpected __riscv_xlen"
#د_که_پای
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_عدد(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_ښويدنه);
	pmpcfg  = (csr_read_عدد(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((بینښه اوږد)pmp->cfg << pmpcfg_ښويدنه) & ~cfgmask);
	csr_write_عدد(pmpcfg_csr, pmpcfg);

	أرجع SBI_OK;
}

صحيح sbi_hart_pmp_disable(بینښه صحيح n)
{
	pmp_t pmp;
	صحيح rc;

	rc = hart_pmp_read(&pmp, n);
	إذا (rc)
		أرجع rc;

	pmp.cfg = 0;

	أرجع hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(بینښه صحيح n)
{
	pmp_t pmp;

	إذا (hart_pmp_read(&pmp, n) != SBI_OK)
		أرجع false;

	أرجع sbi_pmp_is_enabled(&pmp);
}

صحيح sbi_hart_pmp_سېټ(بینښه صحيح n, بینښه اوږد prot, بینښه اوږد addr,
		     بینښه اوږد log2len)
{
	pmp_t pmp;
	صحيح rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	إذا (rc)
		أرجع rc;

	أرجع hart_pmp_write(&pmp, n);
}

صحيح sbi_hart_pmp_get(بینښه صحيح n, بینښه اوږد *prot_بهر, بینښه اوږد *addr_بهر,
		     بینښه اوږد *log2len)
{
	pmp_t pmp;
	صحيح rc;

	rc = hart_pmp_read(&pmp, n);
	إذا (rc)
		أرجع rc;

	أرجع sbi_pmp_decode(&pmp, prot_بهر, addr_بهر, log2len);
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
#تعریف_کړه SBI_SMEPMP_RESV_ثبت		0

ساكن DECLARE_BITMAP(fw_smepmp_ids, PMP_شمېر);
ساكن bool fw_smepmp_ids_inited;

بینښه صحيح sbi_hart_pmp_شمېر(بنية sbi_scratch *scratch)
{
	بنية sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	أرجع hfeatures->pmp_شمېر;
}

بینښه صحيح sbi_hart_pmp_log2gran(بنية sbi_scratch *scratch)
{
	بنية sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	أرجع hfeatures->pmp_log2gran;
}

بینښه صحيح sbi_hart_pmp_addrbits(بنية sbi_scratch *scratch)
{
	بنية sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	أرجع hfeatures->pmp_addr_بټونه;
}

bool sbi_hart_smepmp_is_fw_region(بینښه صحيح pmp_idx)
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

ساكن فراغ sbi_hart_smepmp_سېټ(بنية sbi_scratch *scratch,
				بنية sbi_domain *dom,
				بنية sbi_domain_memregion *reg,
				بینښه صحيح pmp_idx,
				بینښه صحيح pmp_flags,
				بینښه صحيح pmp_log2gran,
				بینښه اوږد pmp_addr_max)
{
	بینښه اوږد pmp_addr = reg->بنسټ_2 >> PMP_ښويدنه;

	إذا (pmp_log2gran <= reg->ترتیب && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_سېټ(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->بنسټ_2, reg->ترتیب);
		sbi_hart_pmp_سېټ(pmp_idx, pmp_flags, reg->بنسټ_2, reg->ترتیب);
	} وإلا {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->بنسټ_2,
			   reg->ترتیب);
	}
}

ساكن bool is_valid_pmp_idx(بینښه صحيح pmp_شمېر, بینښه صحيح pmp_idx)
{
	إذا (pmp_شمېر > pmp_idx)
		أرجع true;

	sbi_printf("error: insufficient PMP entries\n");
	أرجع false;
}

ساكن صحيح sbi_hart_smepmp_configure(بنية sbi_scratch *scratch,
				     بنية sbi_domain *dom)
{
	بنية sbi_domain_memregion *reg;
	بینښه صحيح pmp_log2gran, pmp_بټونه;
	بینښه صحيح pmp_idx, pmp_شمېر;
	بینښه اوږد pmp_addr_max;
	بینښه صحيح pmp_flags;

	pmp_شمېر = sbi_hart_pmp_شمېر(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_بټونه = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_بټونه) | ((1UL << pmp_بټونه) - 1);

	/*
	 * Set the RLB so that, we can write to PMP entries without
	 * enforcement even if some entries are locked.
	 */
	csr_سېټ(CSR_MSECCFG, MSECCFG_RLB);

	/* Disable the reserved entry */
	sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ثبت);

	/* Program M-only regions when MML is not set. */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		إذا (pmp_idx == SBI_SMEPMP_RESV_ثبت)
			pmp_idx++;
		إذا (!is_valid_pmp_idx(pmp_شمېر, pmp_idx))
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
				bitmap_سېټ(fw_smepmp_ids, pmp_idx, 1);
			}
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_سېټ(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}

	fw_smepmp_ids_inited = true;

	/* Set the MML to enforce new encoding */
	csr_سېټ(CSR_MSECCFG, MSECCFG_MML);

	/* Program shared and SU-only regions */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		إذا (pmp_idx == SBI_SMEPMP_RESV_ثبت)
			pmp_idx++;
		إذا (!is_valid_pmp_idx(pmp_شمېر, pmp_idx))
			أرجع SBI_EFAIL;

		/* Skip M-only regions */
		إذا (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			استمر;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_سېټ(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	لكل(; pmp_idx < pmp_شمېر; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	أرجع 0;
}

ساكن صحيح sbi_hart_smepmp_لنډمهاله_map_range(بنية sbi_scratch *scratch,
				     بینښه اوږد addr, بینښه اوږد اندازه)
{
	/* shared R/W access for M and S/U mode */
	بینښه صحيح pmp_flags = (PMP_W | PMP_X);
	بینښه اوږد ترتیب, بنسټ_2 = 0;

	إذا (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_ثبت))
		أرجع SBI_ENOSPC;

	لكل (ترتیب = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(اندازه));
	     ترتیب <= __riscv_xlen; ترتیب++) {
		إذا (ترتیب < __riscv_xlen) {
			بنسټ_2 = addr & ~((1UL << ترتیب) - 1UL);
			إذا ((بنسټ_2 <= addr) &&
			    (addr < (بنسټ_2 + (1UL << ترتیب))) &&
			    (بنسټ_2 <= (addr + اندازه - 1UL)) &&
			    ((addr + اندازه - 1UL) < (بنسټ_2 + (1UL << ترتیب))))
				اكسر;
		} وإلا {
			أرجع SBI_EFAIL;
		}
	}

	sbi_platform_pmp_سېټ(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ثبت,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, بنسټ_2, ترتیب);
	sbi_hart_pmp_سېټ(SBI_SMEPMP_RESV_ثبت, pmp_flags, بنسټ_2, ترتیب);

	أرجع SBI_OK;
}

ساكن صحيح sbi_hart_smepmp_لنډمهاله_unmap_range(بنية sbi_scratch *scratch,
				       بینښه اوږد addr, بینښه اوږد اندازه)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ثبت);
	أرجع sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ثبت);
}

ساكن صحيح sbi_hart_oldpmp_configure(بنية sbi_scratch *scratch,
				     بنية sbi_domain *dom)
{
	بنية sbi_domain_memregion *reg;
	بینښه اوږد pmp_addr, pmp_addr_max;
	بینښه صحيح pmp_log2gran, pmp_بټونه;
	بینښه صحيح pmp_idx, pmp_شمېر;
	بینښه صحيح pmp_flags;

	pmp_شمېر = sbi_hart_pmp_شمېر(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_بټونه = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_بټونه) | ((1UL << pmp_بټونه) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		إذا (!is_valid_pmp_idx(pmp_شمېر, pmp_idx))
			أرجع SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->بنسټ_2 >> PMP_ښويدنه;
		إذا (pmp_log2gran <= reg->ترتیب && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_سېټ(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->بنسټ_2, reg->ترتیب);
			sbi_hart_pmp_سېټ(pmp_idx++, pmp_flags, reg->بنسټ_2, reg->ترتیب);
		} وإلا {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->بنسټ_2,
				   reg->ترتیب);
		}
	}
	/* Disable remaining PMP entries */
	لكل(; pmp_idx < pmp_شمېر; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	أرجع 0;
}

ساكن فراغ sbi_hart_pmp_unconfigure(بنية sbi_scratch *scratch,
				     بنية sbi_domain *dom)
{
	صحيح i, pmp_شمېر = sbi_hart_pmp_شمېر(scratch);

	لكل (i = 0; i < pmp_شمېر; i++) {
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
	.type = SBI_HART_PROTECTION_TYPE_حافظه,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

ساكن بنية sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_حافظه,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.لنډمهاله_map_range = sbi_hart_smepmp_لنډمهاله_map_range,
	.لنډمهاله_unmap_range = sbi_hart_smepmp_لنډمهاله_unmap_range,
};

صحيح sbi_hart_pmp_init(بنية sbi_scratch *scratch)
{
	صحيح rc;

	إذا (sbi_hart_pmp_شمېر(scratch)) {
		إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_ثبتوونکی(&epmp_protection);
			إذا (rc)
				أرجع rc;
		} وإلا {
			rc = sbi_hart_protection_ثبتوونکی(&pmp_protection);
			إذا (rc)
				أرجع rc;
		}
	}

	أرجع 0;
}
