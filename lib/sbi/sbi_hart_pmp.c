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

ساکن صحیح hart_pmp_read(pmp_t *pmp, غیرعلامتی صحیح n)
{
	صحیح pmpcfg_csr, pmpcfg_سرکاؤ, pmpaddr_csr;
	غیرعلامتی طویل cfgmask;

	/* check parameters */
	اگر (n >= PMP_گنتی)
		واپس SBI_EINVAL;

	/* calculate PMP register and offset */
#اگر __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_سرکاؤ = (n & 3) << 3;
#وگرنہ_اگر __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_سرکاؤ = (n & 7) << 3;
#ورنہ
# error "Unexpected __riscv_xlen"
#اگر_ختم
	pmpaddr_csr = CSR_PMPADDR0 + n;

	cfgmask = (0xffUL << pmpcfg_سرکاؤ);
	pmp->cfg = (csr_read_عدد(pmpcfg_csr) & cfgmask) >> pmpcfg_سرکاؤ;
	pmp->addr = csr_read_عدد(pmpaddr_csr);

	واپس SBI_OK;
}

ساکن صحیح hart_pmp_write(pmp_t *pmp, غیرعلامتی صحیح n)
{
	صحیح pmpcfg_csr, pmpcfg_سرکاؤ, pmpaddr_csr;
	غیرعلامتی طویل cfgmask, pmpcfg;

	/* check parameters */
	اگر (n >= PMP_گنتی)
		واپس SBI_EINVAL;

	/* calculate PMP register and offset */
#اگر __riscv_xlen == 32
	pmpcfg_csr   = CSR_PMPCFG0 + (n >> 2);
	pmpcfg_سرکاؤ = (n & 3) << 3;
#وگرنہ_اگر __riscv_xlen == 64
	pmpcfg_csr   = (CSR_PMPCFG0 + (n >> 2)) & ~1;
	pmpcfg_سرکاؤ = (n & 7) << 3;
#ورنہ
# error "Unexpected __riscv_xlen"
#اگر_ختم
	pmpaddr_csr = CSR_PMPADDR0 + n;

	/* write csrs */
	csr_write_عدد(pmpaddr_csr, pmp->addr);
	cfgmask = ~(0xffUL << pmpcfg_سرکاؤ);
	pmpcfg  = (csr_read_عدد(pmpcfg_csr) & cfgmask);
	pmpcfg |= (((غیرعلامتی طویل)pmp->cfg << pmpcfg_سرکاؤ) & ~cfgmask);
	csr_write_عدد(pmpcfg_csr, pmpcfg);

	واپس SBI_OK;
}

صحیح sbi_hart_pmp_disable(غیرعلامتی صحیح n)
{
	pmp_t pmp;
	صحیح rc;

	rc = hart_pmp_read(&pmp, n);
	اگر (rc)
		واپس rc;

	pmp.cfg = 0;

	واپس hart_pmp_write(&pmp, n);
}

bool sbi_hart_is_pmp_enabled(غیرعلامتی صحیح n)
{
	pmp_t pmp;

	اگر (hart_pmp_read(&pmp, n) != SBI_OK)
		واپس false;

	واپس sbi_pmp_is_enabled(&pmp);
}

صحیح sbi_hart_pmp_سیٹ(غیرعلامتی صحیح n, غیرعلامتی طویل prot, غیرعلامتی طویل addr,
		     غیرعلامتی طویل log2len)
{
	pmp_t pmp;
	صحیح rc;

	rc = sbi_pmp_encode(&pmp, prot, addr, log2len);
	اگر (rc)
		واپس rc;

	واپس hart_pmp_write(&pmp, n);
}

صحیح sbi_hart_pmp_get(غیرعلامتی صحیح n, غیرعلامتی طویل *prot_برآمد, غیرعلامتی طویل *addr_برآمد,
		     غیرعلامتی طویل *log2len)
{
	pmp_t pmp;
	صحیح rc;

	rc = hart_pmp_read(&pmp, n);
	اگر (rc)
		واپس rc;

	واپس sbi_pmp_decode(&pmp, prot_برآمد, addr_برآمد, log2len);
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
#تعریف_کرو SBI_SMEPMP_RESV_اندراج		0

ساکن DECLARE_BITMAP(fw_smepmp_ids, PMP_گنتی);
ساکن bool fw_smepmp_ids_inited;

غیرعلامتی صحیح sbi_hart_pmp_گنتی(ساخت sbi_scratch *scratch)
{
	ساخت sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	واپس hfeatures->pmp_گنتی;
}

غیرعلامتی صحیح sbi_hart_pmp_log2gran(ساخت sbi_scratch *scratch)
{
	ساخت sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	واپس hfeatures->pmp_log2gran;
}

غیرعلامتی صحیح sbi_hart_pmp_addrbits(ساخت sbi_scratch *scratch)
{
	ساخت sbi_hart_features *hfeatures = sbi_hart_features_ptr(scratch);

	واپس hfeatures->pmp_addr_بٹ;
}

bool sbi_hart_smepmp_is_fw_region(غیرعلامتی صحیح pmp_idx)
{
	اگر (!fw_smepmp_ids_inited)
		واپس false;

	واپس bitmap_test(fw_smepmp_ids, pmp_idx) ? true : false;
}

خالی sbi_hart_pmp_fence(خالی)
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

ساکن خالی sbi_hart_smepmp_سیٹ(ساخت sbi_scratch *scratch,
				ساخت sbi_domain *dom,
				ساخت sbi_domain_memregion *reg,
				غیرعلامتی صحیح pmp_idx,
				غیرعلامتی صحیح pmp_flags,
				غیرعلامتی صحیح pmp_log2gran,
				غیرعلامتی طویل pmp_addr_max)
{
	غیرعلامتی طویل pmp_addr = reg->بنیاد_2 >> PMP_سرکاؤ;

	اگر (pmp_log2gran <= reg->ترتیب && pmp_addr < pmp_addr_max) {
		sbi_platform_pmp_سیٹ(sbi_platform_ptr(scratch),
				     pmp_idx, reg->flags, pmp_flags,
				     reg->بنیاد_2, reg->ترتیب);
		sbi_hart_pmp_سیٹ(pmp_idx, pmp_flags, reg->بنیاد_2, reg->ترتیب);
	} ورنہ {
		sbi_printf("Can not configure pmp for domain %s because"
			   " memory region address 0x%lx or size 0x%lx "
			   "is not in range.\n", dom->name, reg->بنیاد_2,
			   reg->ترتیب);
	}
}

ساکن bool is_valid_pmp_idx(غیرعلامتی صحیح pmp_گنتی, غیرعلامتی صحیح pmp_idx)
{
	اگر (pmp_گنتی > pmp_idx)
		واپس true;

	sbi_printf("error: insufficient PMP entries\n");
	واپس false;
}

ساکن صحیح sbi_hart_smepmp_configure(ساخت sbi_scratch *scratch,
				     ساخت sbi_domain *dom)
{
	ساخت sbi_domain_memregion *reg;
	غیرعلامتی صحیح pmp_log2gran, pmp_بٹ;
	غیرعلامتی صحیح pmp_idx, pmp_گنتی;
	غیرعلامتی طویل pmp_addr_max;
	غیرعلامتی صحیح pmp_flags;

	pmp_گنتی = sbi_hart_pmp_گنتی(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_بٹ = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_بٹ) | ((1UL << pmp_بٹ) - 1);

	/*
	 * Set the RLB so that, we can write to PMP entries without
	 * enforcement even if some entries are locked.
	 */
	csr_سیٹ(CSR_MSECCFG, MSECCFG_RLB);

	/* Disable the reserved entry */
	sbi_hart_pmp_disable(SBI_SMEPMP_RESV_اندراج);

	/* Program M-only regions when MML is not set. */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		اگر (pmp_idx == SBI_SMEPMP_RESV_اندراج)
			pmp_idx++;
		اگر (!is_valid_pmp_idx(pmp_گنتی, pmp_idx))
			واپس SBI_EFAIL;

		/* Skip shared and SU-only regions */
		اگر (!SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			جاری;
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
					واپس SBI_EINVAL;
			} ورنہ {
				bitmap_سیٹ(fw_smepmp_ids, pmp_idx, 1);
			}
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_سیٹ(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}

	fw_smepmp_ids_inited = true;

	/* Set the MML to enforce new encoding */
	csr_سیٹ(CSR_MSECCFG, MSECCFG_MML);

	/* Program shared and SU-only regions */
	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Skip reserved entry */
		اگر (pmp_idx == SBI_SMEPMP_RESV_اندراج)
			pmp_idx++;
		اگر (!is_valid_pmp_idx(pmp_گنتی, pmp_idx))
			واپس SBI_EFAIL;

		/* Skip M-only regions */
		اگر (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
			pmp_idx++;
			جاری;
		}

		pmp_flags = sbi_domain_get_smepmp_flags(reg);

		sbi_hart_smepmp_سیٹ(scratch, dom, reg, pmp_idx++, pmp_flags,
				    pmp_log2gran, pmp_addr_max);
	}
	/* Disable remaining PMP entries */
	برائے(; pmp_idx < pmp_گنتی; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	/*
	 * All entries are programmed.
	 * Keep the RLB bit so that dynamic mappings can be done.
	 */

	sbi_hart_pmp_fence();
	واپس 0;
}

ساکن صحیح sbi_hart_smepmp_عارضی_map_range(ساخت sbi_scratch *scratch,
				     غیرعلامتی طویل addr, غیرعلامتی طویل سائز)
{
	/* shared R/W access for M and S/U mode */
	غیرعلامتی صحیح pmp_flags = (PMP_W | PMP_X);
	غیرعلامتی طویل ترتیب, بنیاد_2 = 0;

	اگر (sbi_hart_is_pmp_enabled(SBI_SMEPMP_RESV_اندراج))
		واپس SBI_ENOSPC;

	برائے (ترتیب = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(سائز));
	     ترتیب <= __riscv_xlen; ترتیب++) {
		اگر (ترتیب < __riscv_xlen) {
			بنیاد_2 = addr & ~((1UL << ترتیب) - 1UL);
			اگر ((بنیاد_2 <= addr) &&
			    (addr < (بنیاد_2 + (1UL << ترتیب))) &&
			    (بنیاد_2 <= (addr + سائز - 1UL)) &&
			    ((addr + سائز - 1UL) < (بنیاد_2 + (1UL << ترتیب))))
				توڑو;
		} ورنہ {
			واپس SBI_EFAIL;
		}
	}

	sbi_platform_pmp_سیٹ(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_اندراج,
			     SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW,
			     pmp_flags, بنیاد_2, ترتیب);
	sbi_hart_pmp_سیٹ(SBI_SMEPMP_RESV_اندراج, pmp_flags, بنیاد_2, ترتیب);

	واپس SBI_OK;
}

ساکن صحیح sbi_hart_smepmp_عارضی_unmap_range(ساخت sbi_scratch *scratch,
				       غیرعلامتی طویل addr, غیرعلامتی طویل سائز)
{
	sbi_platform_pmp_disable(sbi_platform_ptr(scratch), SBI_SMEPMP_RESV_اندراج);
	واپس sbi_hart_pmp_disable(SBI_SMEPMP_RESV_اندراج);
}

ساکن صحیح sbi_hart_oldpmp_configure(ساخت sbi_scratch *scratch,
				     ساخت sbi_domain *dom)
{
	ساخت sbi_domain_memregion *reg;
	غیرعلامتی طویل pmp_addr, pmp_addr_max;
	غیرعلامتی صحیح pmp_log2gran, pmp_بٹ;
	غیرعلامتی صحیح pmp_idx, pmp_گنتی;
	غیرعلامتی صحیح pmp_flags;

	pmp_گنتی = sbi_hart_pmp_گنتی(scratch);
	pmp_log2gran = sbi_hart_pmp_log2gran(scratch);
	pmp_بٹ = sbi_hart_pmp_addrbits(scratch) - 1;
	pmp_addr_max = (1UL << pmp_بٹ) | ((1UL << pmp_بٹ) - 1);

	pmp_idx = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		اگر (!is_valid_pmp_idx(pmp_گنتی, pmp_idx))
			واپس SBI_EFAIL;

		pmp_flags = sbi_domain_get_oldpmp_flags(reg);
		pmp_addr = reg->بنیاد_2 >> PMP_سرکاؤ;
		اگر (pmp_log2gran <= reg->ترتیب && pmp_addr < pmp_addr_max) {
			sbi_platform_pmp_سیٹ(sbi_platform_ptr(scratch),
					     pmp_idx, reg->flags, pmp_flags,
					     reg->بنیاد_2, reg->ترتیب);
			sbi_hart_pmp_سیٹ(pmp_idx++, pmp_flags, reg->بنیاد_2, reg->ترتیب);
		} ورنہ {
			sbi_printf("Can not configure pmp for domain %s because"
				   " memory region address 0x%lx or size 0x%lx "
				   "is not in range.\n", dom->name, reg->بنیاد_2,
				   reg->ترتیب);
		}
	}
	/* Disable remaining PMP entries */
	برائے(; pmp_idx < pmp_گنتی; pmp_idx++)
		sbi_hart_pmp_disable(pmp_idx);

	sbi_hart_pmp_fence();
	واپس 0;
}

ساکن خالی sbi_hart_pmp_unconfigure(ساخت sbi_scratch *scratch,
				     ساخت sbi_domain *dom)
{
	صحیح i, pmp_گنتی = sbi_hart_pmp_گنتی(scratch);

	برائے (i = 0; i < pmp_گنتی; i++) {
		/* Don't revoke firmware access permissions */
		اگر (sbi_hart_smepmp_is_fw_region(i))
			جاری;

		sbi_platform_pmp_disable(sbi_platform_ptr(scratch), i);
		sbi_hart_pmp_disable(i);
	}
}

ساکن ساخت sbi_hart_protection pmp_protection = {
	.name = "pmp",
	.rating = 100,
	.type = SBI_HART_PROTECTION_TYPE_حافظہ,
	.configure = sbi_hart_oldpmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
};

ساکن ساخت sbi_hart_protection epmp_protection = {
	.name = "epmp",
	.rating = 200,
	.type = SBI_HART_PROTECTION_TYPE_حافظہ,
	.configure = sbi_hart_smepmp_configure,
	.unconfigure = sbi_hart_pmp_unconfigure,
	.عارضی_map_range = sbi_hart_smepmp_عارضی_map_range,
	.عارضی_unmap_range = sbi_hart_smepmp_عارضی_unmap_range,
};

صحیح sbi_hart_pmp_init(ساخت sbi_scratch *scratch)
{
	صحیح rc;

	اگر (sbi_hart_pmp_گنتی(scratch)) {
		اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP)) {
			rc = sbi_hart_protection_رجسٹر(&epmp_protection);
			اگر (rc)
				واپس rc;
		} ورنہ {
			rc = sbi_hart_protection_رجسٹر(&pmp_protection);
			اگر (rc)
				واپس rc;
		}
	}

	واپس 0;
}
