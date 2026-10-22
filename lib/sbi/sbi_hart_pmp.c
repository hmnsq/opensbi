/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#അന്തര്ഭാവ <sbi/sbi_bitmap.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_hart_protection.h>
#അന്തര്ഭാവ <sbi/sbi_hfence.h>
#അന്തര്ഭാവ <sbi/sbi_math.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_pmp.h>
#അന്തര്ഭാവ <sbi/sbi_tlb.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ hart_pmp_read(pmp_t *pmp, unsigned പൂർണ്ണസംഖ്യ n)
{
	പൂർണ്ണസംഖ്യ pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask;

	/* check parameters */
	എങ്കിൽ (n >= PMP_എണ്ണം)
		തിരികെ SBI_EINVAL;

	/* calculate PMP register and offset */
#എങ്കിൽ __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#അല്ലെങ്കിലെങ്കിൽ __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#അല്ലെങ്കിൽ
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_shift);
	pmp->cfg = (csr_read_അക്കം(pmpcfg_csr) & cfgmask) >> pmpcfg_shift;
	pmp->addr = csr_read_അക്കം(pmpaddr_csr);

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ hart_pmp_write(pmp_t *pmp, unsigned പൂർണ്ണസംഖ്യ n)
{
	പൂർണ്ണസംഖ്യ pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	unsigned long cfgmask, pmpcfg;

	/* check parameters */
	എങ്കിൽ (n >= PMP_എണ്ണം)
		തിരികെ SBI_EINVAL;

	/* calculate PMP register and offset */
#എങ്കിൽ __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#അല്ലെങ്കിലെങ്കിൽ __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#അല്ലെങ്കിൽ
# error "Unexpected __riscv_xlen"
#endif
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_അക്കം(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_shift);
	pmpcfg  = (csr_read_അക്കം(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((unsigned long)pmp->cfg << pmpcfg_shift) & ~cfgmask);
	csr_write_അക്കം(pmpcfg_csr, pmpcfg);

	തിരികെ SBI_OK;
}

പൂർണ്ണസംഖ്യ sbi_hart_pmp_disable(unsigned പൂർണ്ണസംഖ്യ n)
{
	pmp_t pmp;
	പൂർണ്ണസംഖ്യ rc;

	rc = hart_pmp_read(&pmp, n);
	എങ്കിൽ (rc)
		തിരികെ rc;

	pmp.cfg = 0;

	തിരികെ hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(unsigned പൂർണ്ണസംഖ്യ n)
{
	pmp_t pmp;

	എങ്കിൽ (hart_pmp_read(&pmp, n) != SBI_OK)
		തിരികെ false;

	തിരികെ sbi_pmp_is_enabled(&pmp);
}

പൂർണ്ണസംഖ്യ sbi_hart_pmp_set(unsigned പൂർണ്ണസംഖ്യ n, unsigned long prot, unsigned long addr,
		     unsigned long log2len)
{
	pmp_t pmp;
	പൂർണ്ണസംഖ്യ rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ hart_pmp_write(&pmp, n);
}

പൂർണ്ണസംഖ്യ sbi_hart_pmp_get(unsigned പൂർണ്ണസംഖ്യ n, unsigned long *prot_out, unsigned long *addr_out,
		     unsigned long *log2len)
{
	pmp_t pmp;
	പൂർണ്ണസംഖ്യ rc;

	rc = hart_pmp_read(&pmp, n);
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ sbi_pmp_decode(&pmp, prot_out, addr_out, log2len);
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

സ്ഥിതം DECLARE_BITMAP(fw_smepmp_ids, PMP_എണ്ണം);
സ്ഥിതം bool fw_smepmp_ids_inited;

unsigned പൂർണ്ണസംഖ്യ sbi_hart_pmp_എണ്ണം(ഘടന sbi_scratch *scratch)
{
	ഘടന sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	തിരികെ hfeatures->pmp_എണ്ണം;
}

unsigned പൂർണ്ണസംഖ്യ sbi_hart_pmp_log2gran(ഘടന sbi_scratch *scratch)
{
	ഘടന sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	തിരികെ hfeatures->pmp_log2gran;
}

unsigned പൂർണ്ണസംഖ്യ sbi_hart_pmp_addrbits(ഘടന sbi_scratch *scratch)
{
	ഘടന sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	തിരികെ hfeatures->pmp_addr_bits;
}

bool sbi_hart_smepmp_is_fw_region(unsigned പൂർണ്ണസംഖ്യ pmp_idx)
{
	എങ്കിൽ (!fw_smepmp_ids_inited)
		തിരികെ false;

	തിരികെ bitmap_test(fw_smepmp_ids, pmp_idx) ? true : false;
}

ശൂന്യം sbi_hart_pmp_fence(ശൂന്യം)
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
	എങ്കിൽ (misa_extension('S')) {
		__sbi_sfence_vma_all();

		/*
		 * If hypervisor mode is supported, flush caching
		 * structures in guest mode too.
		 */
		എങ്കിൽ (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}
}

സ്ഥിതം ശൂന്യം sbi_hart_smepmp_set(ഘടന sbi_scratch *scratch,
				ഘടന sbi_domain *dom,
				ഘടന sbi_domain_memregion *reg,
				unsigned പൂർണ്ണസംഖ്യ pmp_idx,
				unsigned പൂർണ്ണസംഖ്യ pmp_flags,
				unsigned പൂർണ്ണസംഖ്യ pmp_log2gran,
				unsigned long pmp_addr_max)
{
	unsigned long pmp_addr = reg->base >> PMP_SHIFT;

	എങ്കിൽ (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_set(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->base, reg->order);
		sbi_hart_pmp_set(pmp_idx, pmp_flags, reg->base, reg->order);
	} അല്ലെങ്കിൽ {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->base,
			   reg->order);
	}
}

സ്ഥിതം bool is_valid_pmp_idx(unsigned പൂർണ്ണസംഖ്യ pmp_എണ്ണം, unsigned പൂർണ്ണസംഖ്യ pmp_idx)
{
	എങ്കിൽ (pmp_എണ്ണം > pmp_idx)
		തിരികെ true;

	sbi_printf("error: insufficient PMP entries\n");
	തിരികെ false;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_hart_smepmp_configure(ഘടന sbi_scratch *scratch,
				     ഘടന sbi_domain *dom)
{
	ഘടന sbi_domain_memregion *reg;
	unsigned പൂർണ്ണസംഖ്യ pmp_log2gran, pmp_bits;
	unsigned പൂർണ്ണസംഖ്യ pmp_idx, pmp_എണ്ണം;
	unsigned long pmp_addr_max;
	unsigned പൂർണ്ണസംഖ്യ pmp_flags;

	pmp_എണ്ണം = sbi_hart_pmp_എണ്ണം(scratch);
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
		എങ്കിൽ (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		എങ്കിൽ (!is_valid_pmp_idx(pmp_എണ്ണം, pmp_idx))
			തിരികെ SBI_EFAIL;

		/* Skip shared and SU-only regions */
		എങ്കിൽ (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			തുടരുക;
		}

		/*
		 * Track firmware PMP entries to preserve them during
		 * domain switches. Under SmePMP, M-mode requires
		 * explicit PMP entries to access firmware code/data.
		 * These entries must remain enabled across domain
		 * context switches to prevent M-mode access faults.
		 */
		എങ്കിൽ (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags)) {
			എങ്കിൽ (fw_smepmp_ids_inited) {
				/* Check inconsistent firmware region */
				എങ്കിൽ (!sbi_hart_smepmp_is_fw_region(pmp_idx))
					തിരികെ SBI_EINVAL;
			} അല്ലെങ്കിൽ {
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
		എങ്കിൽ (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		എങ്കിൽ (!is_valid_pmp_idx(pmp_എണ്ണം, pmp_idx))
			തിരികെ SBI_EFAIL;

		/* Skip M-only regions */
		എങ്കിൽ (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			തുടരുക;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_set(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	വേണ്ടി(; pmp_idx < pmp_എണ്ണം; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_hart_smepmp_temp_map_range(ഘടന sbi_scratch *scratch,
				     unsigned long addr, unsigned long size)
{
	/* shared R/W access for M and S/U mode */
	unsigned പൂർണ്ണസംഖ്യ pmp_flags = (PMP_W | PMP_X);
	unsigned long order, base = 0;

	എങ്കിൽ (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_ENTRY))
		തിരികെ SBI_ENOSPC;

	വേണ്ടി (order = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(size));
	     order <= __riscv_xlen; order++) {
		എങ്കിൽ (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			എങ്കിൽ ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				നിർത്തുക;
		} അല്ലെങ്കിൽ {
			തിരികെ SBI_EFAIL;
		}
	}

	sbi_platform_pmp_set(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, base, order);
	sbi_hart_pmp_set(SBI_SMEPMP_RESV_ENTRY, pmp_flags, base, order);

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_hart_smepmp_temp_unmap_range(ഘടന sbi_scratch *scratch,
				       unsigned long addr, unsigned long size)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY);
	തിരികെ sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ENTRY);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_hart_oldpmp_configure(ഘടന sbi_scratch *scratch,
				     ഘടന sbi_domain *dom)
{
	ഘടന sbi_domain_memregion *reg;
	unsigned long pmp_addr, pmp_addr_max;
	unsigned പൂർണ്ണസംഖ്യ pmp_log2gran, pmp_bits;
	unsigned പൂർണ്ണസംഖ്യ pmp_idx, pmp_എണ്ണം;
	unsigned പൂർണ്ണസംഖ്യ pmp_flags;

	pmp_എണ്ണം = sbi_hart_pmp_എണ്ണം(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_bits = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_bits) | ((1UL << pmp_bits) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		എങ്കിൽ (!is_valid_pmp_idx(pmp_എണ്ണം, pmp_idx))
			തിരികെ SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->base >> PMP_SHIFT;
		എങ്കിൽ (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_set(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->base, reg->order);
			sbi_hart_pmp_set(pmp_idx++, pmp_flags, reg->base, reg->order);
		} അല്ലെങ്കിൽ {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->base,
				   reg->order);
		}
	}
	/* Disable remaining PMP entries */
	വേണ്ടി(; pmp_idx < pmp_എണ്ണം; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം sbi_hart_pmp_unconfigure(ഘടന sbi_scratch *scratch,
				     ഘടന sbi_domain *dom)
{
	പൂർണ്ണസംഖ്യ i, pmp_എണ്ണം = sbi_hart_pmp_എണ്ണം(scratch);

	വേണ്ടി (i = 0; i < pmp_എണ്ണം; i++) {
		/* Don't revoke firmware access permissions */
		എങ്കിൽ (sbi_hart_smepmp_is_fw_region(i))
			തുടരുക;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

സ്ഥിതം ഘടന sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

സ്ഥിതം ഘടന sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.temp_map_range = sbi_hart_smepmp_temp_map_range,
	.temp_unmap_range = sbi_hart_smepmp_temp_unmap_range,
};

പൂർണ്ണസംഖ്യ sbi_hart_pmp_init(ഘടന sbi_scratch *scratch)
{
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (sbi_hart_pmp_എണ്ണം(scratch)) {
		എങ്കിൽ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_register(&epmp_protection);
			എങ്കിൽ (rc)
				തിരികെ rc;
		} അല്ലെങ്കിൽ {
			rc = sbi_hart_protection_register(&pmp_protection);
			എങ്കിൽ (rc)
				തിരികെ rc;
		}
	}

	തിരികെ 0;
}
