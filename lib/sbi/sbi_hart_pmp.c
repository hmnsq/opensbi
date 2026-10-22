/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#समावेश <sbi/sbi_bitmap.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_hart_protection.h>
#समावेश <sbi/sbi_hfence.h>
#समावेश <sbi/sbi_math.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_pmp.h>
#समावेश <sbi/sbi_tlb.h>
#समावेश <sbi/riscv_asm.h>

अटल पूर्णांक hart_pmp_read(pmp_t *pmp, अचिन्हित पूर्णांक n)
{
	पूर्णांक pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	अचिन्हित दीर्घ cfgmask;

	/* check parameters */
	अगर (n >= PMP_गिनती)
		वापस SBI_EINVAL;

	/* calculate PMP register and offset */
#अगर __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#या_अगर __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#अन्यथा
# error "Unexpected __riscv_xlen"
#पूर्ण_अगर
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_shift);
	pmp->cfg = (csr_read_अंक(pmpcfg_csr) & cfgmask) >> pmpcfg_shift;
	pmp->addr = csr_read_अंक(pmpaddr_csr);

	वापस SBI_OK;
}

अटल पूर्णांक hart_pmp_write(pmp_t *pmp, अचिन्हित पूर्णांक n)
{
	पूर्णांक pmpcfg_csr, pmpcfg_shift, pmpaddr_csr;
	अचिन्हित दीर्घ cfgmask, pmpcfg;

	/* check parameters */
	अगर (n >= PMP_गिनती)
		वापस SBI_EINVAL;

	/* calculate PMP register and offset */
#अगर __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_shift = (n & 3) << 3;
#या_अगर __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#अन्यथा
# error "Unexpected __riscv_xlen"
#पूर्ण_अगर
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_अंक(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_shift);
	pmpcfg  = (csr_read_अंक(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((अचिन्हित दीर्घ)pmp->cfg << pmpcfg_shift) & ~cfgmask);
	csr_write_अंक(pmpcfg_csr, pmpcfg);

	वापस SBI_OK;
}

पूर्णांक sbi_hart_pmp_disable(अचिन्हित पूर्णांक n)
{
	pmp_t pmp;
	पूर्णांक rc;

	rc = hart_pmp_read(&pmp, n);
	अगर (rc)
		वापस rc;

	pmp.cfg = 0;

	वापस hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(अचिन्हित पूर्णांक n)
{
	pmp_t pmp;

	अगर (hart_pmp_read(&pmp, n) != SBI_OK)
		वापस false;

	वापस sbi_pmp_is_enabled(&pmp);
}

पूर्णांक sbi_hart_pmp_set(अचिन्हित पूर्णांक n, अचिन्हित दीर्घ prot, अचिन्हित दीर्घ addr,
		     अचिन्हित दीर्घ log2len)
{
	pmp_t pmp;
	पूर्णांक rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	अगर (rc)
		वापस rc;

	वापस hart_pmp_write(&pmp, n);
}

पूर्णांक sbi_hart_pmp_get(अचिन्हित पूर्णांक n, अचिन्हित दीर्घ *prot_out, अचिन्हित दीर्घ *addr_out,
		     अचिन्हित दीर्घ *log2len)
{
	pmp_t pmp;
	पूर्णांक rc;

	rc = hart_pmp_read(&pmp, n);
	अगर (rc)
		वापस rc;

	वापस sbi_pmp_decode(&pmp, prot_out, addr_out, log2len);
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
#घोषणा SBI_SMEPMP_RESV_ENTRY		0

अटल DECLARE_BITMAP(fw_smepmp_ids, PMP_गिनती);
अटल bool fw_smepmp_ids_inited;

अचिन्हित पूर्णांक sbi_hart_pmp_गिनती(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	वापस hfeatures->pmp_गिनती;
}

अचिन्हित पूर्णांक sbi_hart_pmp_log2gran(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	वापस hfeatures->pmp_log2gran;
}

अचिन्हित पूर्णांक sbi_hart_pmp_addrbits(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	वापस hfeatures->pmp_addr_bits;
}

bool sbi_hart_smepmp_is_fw_region(अचिन्हित पूर्णांक pmp_idx)
{
	अगर (!fw_smepmp_ids_inited)
		वापस false;

	वापस bitmap_test(fw_smepmp_ids, pmp_idx) ? true : false;
}

व्योम sbi_hart_pmp_fence(व्योम)
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
	अगर (misa_extension('S')) {
		__sbi_sfence_vma_all();

		/*
		 * If hypervisor mode is supported, flush caching
		 * structures in guest mode too.
		 */
		अगर (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}
}

अटल व्योम sbi_hart_smepmp_set(काष्ठा sbi_scratch *scratch,
				काष्ठा sbi_domain *dom,
				काष्ठा sbi_domain_memregion *reg,
				अचिन्हित पूर्णांक pmp_idx,
				अचिन्हित पूर्णांक pmp_flags,
				अचिन्हित पूर्णांक pmp_log2gran,
				अचिन्हित दीर्घ pmp_addr_max)
{
	अचिन्हित दीर्घ pmp_addr = reg->base >> PMP_SHIFT;

	अगर (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_set(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->base, reg->order);
		sbi_hart_pmp_set(pmp_idx, pmp_flags, reg->base, reg->order);
	} अन्यथा {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->base,
			   reg->order);
	}
}

अटल bool is_valid_pmp_idx(अचिन्हित पूर्णांक pmp_गिनती, अचिन्हित पूर्णांक pmp_idx)
{
	अगर (pmp_गिनती > pmp_idx)
		वापस true;

	sbi_printf("error: insufficient PMP entries\n");
	वापस false;
}

अटल पूर्णांक sbi_hart_smepmp_configure(काष्ठा sbi_scratch *scratch,
				     काष्ठा sbi_domain *dom)
{
	काष्ठा sbi_domain_memregion *reg;
	अचिन्हित पूर्णांक pmp_log2gran, pmp_bits;
	अचिन्हित पूर्णांक pmp_idx, pmp_गिनती;
	अचिन्हित दीर्घ pmp_addr_max;
	अचिन्हित पूर्णांक pmp_flags;

	pmp_गिनती = sbi_hart_pmp_गिनती(scratch);
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
		अगर (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		अगर (!is_valid_pmp_idx(pmp_गिनती, pmp_idx))
			वापस SBI_EFAIL;

		/* Skip shared and SU-only regions */
		अगर (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			जारी;
		}

		/*
		 * Track firmware PMP entries to preserve them during
		 * domain switches. Under SmePMP, M-mode requires
		 * explicit PMP entries to access firmware code/data.
		 * These entries must remain enabled across domain
		 * context switches to prevent M-mode access faults.
		 */
		अगर (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(reg->flags)) {
			अगर (fw_smepmp_ids_inited) {
				/* Check inconsistent firmware region */
				अगर (!sbi_hart_smepmp_is_fw_region(pmp_idx))
					वापस SBI_EINVAL;
			} अन्यथा {
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
		अगर (pmp_idx == SBI_SMEPMP_RESV_ENTRY)
			pmp_idx++;
		अगर (!is_valid_pmp_idx(pmp_गिनती, pmp_idx))
			वापस SBI_EFAIL;

		/* Skip M-only regions */
		अगर (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			जारी;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_set(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	क्रम(; pmp_idx < pmp_गिनती; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	वापस 0;
}

अटल पूर्णांक sbi_hart_smepmp_अस्थायी_map_range(काष्ठा sbi_scratch *scratch,
				     अचिन्हित दीर्घ addr, अचिन्हित दीर्घ आकार)
{
	/* shared R/W access for M and S/U mode */
	अचिन्हित पूर्णांक pmp_flags = (PMP_W | PMP_X);
	अचिन्हित दीर्घ order, base = 0;

	अगर (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_ENTRY))
		वापस SBI_ENOSPC;

	क्रम (order = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(आकार));
	     order <= __riscv_xlen; order++) {
		अगर (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			अगर ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + आकार - 1UL)) &&
			    ((addr + आकार - 1UL) < (base + (1UL << order))))
				अवरोध;
		} अन्यथा {
			वापस SBI_EFAIL;
		}
	}

	sbi_platform_pmp_set(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, base, order);
	sbi_hart_pmp_set(SBI_SMEPMP_RESV_ENTRY, pmp_flags, base, order);

	वापस SBI_OK;
}

अटल पूर्णांक sbi_hart_smepmp_अस्थायी_unmap_range(काष्ठा sbi_scratch *scratch,
				       अचिन्हित दीर्घ addr, अचिन्हित दीर्घ आकार)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_ENTRY);
	वापस sbi_hart_pmp_disable(SBI_SMEPMP_RESV_ENTRY);
}

अटल पूर्णांक sbi_hart_oldpmp_configure(काष्ठा sbi_scratch *scratch,
				     काष्ठा sbi_domain *dom)
{
	काष्ठा sbi_domain_memregion *reg;
	अचिन्हित दीर्घ pmp_addr, pmp_addr_max;
	अचिन्हित पूर्णांक pmp_log2gran, pmp_bits;
	अचिन्हित पूर्णांक pmp_idx, pmp_गिनती;
	अचिन्हित पूर्णांक pmp_flags;

	pmp_गिनती = sbi_hart_pmp_गिनती(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_bits = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_bits) | ((1UL << pmp_bits) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		अगर (!is_valid_pmp_idx(pmp_गिनती, pmp_idx))
			वापस SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->base >> PMP_SHIFT;
		अगर (pmp_log2gran <= reg->order && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_set(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->base, reg->order);
			sbi_hart_pmp_set(pmp_idx++, pmp_flags, reg->base, reg->order);
		} अन्यथा {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->base,
				   reg->order);
		}
	}
	/* Disable remaining PMP entries */
	क्रम(; pmp_idx < pmp_गिनती; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	वापस 0;
}

अटल व्योम sbi_hart_pmp_unconfigure(काष्ठा sbi_scratch *scratch,
				     काष्ठा sbi_domain *dom)
{
	पूर्णांक i, pmp_गिनती = sbi_hart_pmp_गिनती(scratch);

	क्रम (i = 0; i < pmp_गिनती; i++) {
		/* Don't revoke firmware access permissions */
		अगर (sbi_hart_smepmp_is_fw_region(i))
			जारी;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

अटल काष्ठा sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

अटल काष्ठा sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_MEMORY,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.अस्थायी_map_range = sbi_hart_smepmp_अस्थायी_map_range,
	.अस्थायी_unmap_range = sbi_hart_smepmp_अस्थायी_unmap_range,
};

पूर्णांक sbi_hart_pmp_init(काष्ठा sbi_scratch *scratch)
{
	पूर्णांक rc;

	अगर (sbi_hart_pmp_गिनती(scratch)) {
		अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_register(&epmp_protection);
			अगर (rc)
				वापस rc;
		} अन्यथा {
			rc = sbi_hart_protection_register(&pmp_protection);
			अगर (rc)
				वापस rc;
		}
	}

	वापस 0;
}
