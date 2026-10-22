/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_bitmap.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_hfence.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi/sbi_tlb.h>
#অন্তর্ভাব <sbi/sbi_types.h>

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
স্থির unsigned long fwft_ptr_offset;

#define fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), শূন্য *, fwft_ptr_offset)

#define fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define fwft_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), শূন্য *, fwft_ptr_offset, (__phs))

#define MIS_DELEG (1UL << CAUSE_MISALIGNED_LOAD | 1UL << CAUSE_MISALIGNED_STORE)

কাঠামো fwft_config;

কাঠামো fwft_feature {
	গণনা sbi_fwft_feature_t id;
	পূর্ণসংখ্যা (*supported)(কাঠামো fwft_config *conf);
	পূর্ণসংখ্যা (*set)(কাঠামো fwft_config *conf, unsigned long মান);
	পূর্ণসংখ্যা (*get)(কাঠামো fwft_config *conf, unsigned long *মান);
};

কাঠামো fwft_config {
	ধ্রুবক কাঠামো fwft_feature *feature;
	unsigned long flags;
};

কাঠামো fwft_hart_state {
	unsigned পূর্ণসংখ্যা config_গুনতি;
	কাঠামো fwft_config configs[];
};

স্থির ধ্রুবক unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_স্তূপ,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

স্থির bool fwft_is_defined_feature(গণনা sbi_fwft_feature_t feature)
{
	পূর্ণসংখ্যা i;

	জন্য (i = 0; i < array_আকার(fwft_defined_features); i++) {
		যদি (fwft_defined_features[i] == feature)
			ফেরত true;
	}

	ফেরত false;
}

স্থির পূর্ণসংখ্যা fwft_menvcfg_set_bit(unsigned long মান, unsigned long bit)
{
	যদি (মান == 1) {
		যদি (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		অন্যথা
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} অন্যথা যদি (মান == 0) {
		যদি (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		অন্যথা
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} অন্যথা {
		ফেরত SBI_EINVAL;
	}

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা fwft_menvcfg_read_bit(unsigned long *মান, unsigned long bit)
{
	unsigned long cfg;

	যদি (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	অন্যথা
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*মান = cfg != 0;

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা fwft_misaligned_delegation_supported(কাঠামো fwft_config *conf)
{
	যদি (!misa_extension('S'))
		ফেরত SBI_ENOTSUPP;

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা fwft_set_misaligned_delegation(কাঠামো fwft_config *conf,
					 unsigned long মান)
{
	যদি (মান == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	অন্যথা যদি (মান == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	অন্যথা
		ফেরত SBI_EINVAL;

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা fwft_get_misaligned_delegation(কাঠামো fwft_config *conf,
					 unsigned long *মান)
{
	*মান = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা fwft_double_trap_supported(কাঠামো fwft_config *conf)
{
	যদি (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		ফেরত SBI_ENOTSUPP;

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা fwft_set_double_trap(কাঠামো fwft_config *conf, unsigned long মান)
{
	ফেরত fwft_menvcfg_set_bit(মান, ENVCFG_DTE_SHIFT);
}

স্থির পূর্ণসংখ্যা fwft_get_double_trap(কাঠামো fwft_config *conf, unsigned long *মান)
{
	ফেরত fwft_menvcfg_read_bit(মান, ENVCFG_DTE_SHIFT);
}

স্থির পূর্ণসংখ্যা fwft_adue_supported(কাঠামো fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	যদি (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		ফেরত SBI_ENOTSUPP;

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা fwft_set_adue(কাঠামো fwft_config *conf, unsigned long মান)
{
	পূর্ণসংখ্যা res = fwft_menvcfg_set_bit(মান, ENVCFG_ADUE_SHIFT);

	যদি (res == SBI_OK) {
		__sbi_sfence_vma_all();

		যদি (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	ফেরত res;
}

স্থির পূর্ণসংখ্যা fwft_get_adue(কাঠামো fwft_config *conf, unsigned long *মান)
{
	ফেরত fwft_menvcfg_read_bit(মান, ENVCFG_ADUE_SHIFT);
}

স্থির পূর্ণসংখ্যা fwft_lpad_supported(কাঠামো fwft_config *conf)
{
	যদি (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		ফেরত SBI_ENOTSUPP;

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা fwft_enable_lpad(কাঠামো fwft_config *conf, unsigned long মান)
{
	ফেরত fwft_menvcfg_set_bit(মান, ENVCFG_LPE_SHIFT);
}

স্থির পূর্ণসংখ্যা fwft_get_lpad(কাঠামো fwft_config *conf, unsigned long *মান)
{
	ফেরত fwft_menvcfg_read_bit(মান, ENVCFG_LPE_SHIFT);
}

স্থির পূর্ণসংখ্যা fwft_sstack_supported(কাঠামো fwft_config *conf)
{
	যদি (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		ফেরত SBI_ENOTSUPP;

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা fwft_enable_sstack(কাঠামো fwft_config *conf, unsigned long মান)
{
	ফেরত fwft_menvcfg_set_bit(মান, ENVCFG_SSE_SHIFT);
}

স্থির পূর্ণসংখ্যা fwft_get_sstack(কাঠামো fwft_config *conf, unsigned long *মান)
{
	ফেরত fwft_menvcfg_read_bit(মান, ENVCFG_SSE_SHIFT);
}

#যদি __riscv_xlen > 32
স্থির পূর্ণসংখ্যা fwft_pmlen_supported(কাঠামো fwft_config *conf)
{
	যদি (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		ফেরত SBI_ENOTSUPP;

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা fwft_set_pmlen(কাঠামো fwft_config *conf, unsigned long মান)
{
	unsigned long pmm, পূর্ব;

	নির্বাচন (মান) {
	ক্ষেত্র 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		বিরতি;
	ক্ষেত্র 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		বিরতি;
	ক্ষেত্র 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		বিরতি;
	পূর্বনির্ধারিত:
		ফেরত SBI_EINVAL;
	}

	পূর্ব = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	যদি ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, পূর্ব);
		ফেরত SBI_EINVAL;
	}

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা fwft_get_pmlen(কাঠামো fwft_config *conf, unsigned long *মান)
{
	নির্বাচন (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	ক্ষেত্র ENVCFG_PMM_PMLEN_0:
		*মান = 0;
		বিরতি;
	ক্ষেত্র ENVCFG_PMM_PMLEN_7:
		*মান = 7;
		বিরতি;
	ক্ষেত্র ENVCFG_PMM_PMLEN_16:
		*মান = 16;
		বিরতি;
	পূর্বনির্ধারিত:
		ফেরত SBI_EFAIL;
	}

	ফেরত SBI_OK;
}
#endif

স্থির কাঠামো fwft_config* get_feature_config(গণনা sbi_fwft_feature_t feature)
{
	পূর্ণসংখ্যা i;
	কাঠামো fwft_hart_state *fhs = fwft_thishart_state_ptr();

	যদি (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		ফেরত ফাঁকা;

	জন্য (i = 0; i < fhs->config_গুনতি; i++){
		যদি (feature == fhs->configs[i].feature->id)
			ফেরত &fhs->configs[i];
	}

	ফেরত ফাঁকা;
}

স্থির পূর্ণসংখ্যা fwft_get_feature(গণনা sbi_fwft_feature_t feature,
			    কাঠামো fwft_config **conf)
{
	পূর্ণসংখ্যা ret;
	কাঠামো fwft_config *tconf;

	tconf = get_feature_config(feature);
	যদি (!tconf) {
		যদি (fwft_is_defined_feature(feature))
			ফেরত SBI_ENOTSUPP;

		ফেরত SBI_EDENIED;
	}

	যদি (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		যদি (ret)
			ফেরত ret;
	}
	*conf = tconf;

	ফেরত SBI_SUCCESS;
}

স্থির শূন্য fwft_clear_config_lock(গণনা sbi_fwft_feature_t feature)
{
	পূর্ণসংখ্যা ret;
	কাঠামো fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	যদি (ret)
		ফেরত;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

পূর্ণসংখ্যা sbi_fwft_set(গণনা sbi_fwft_feature_t feature, unsigned long মান,
		 unsigned long flags)
{
	পূর্ণসংখ্যা ret;
	কাঠামো fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	যদি (ret)
		ফেরত ret;

	যদি ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		ফেরত SBI_EINVAL;

	যদি (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		ফেরত SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, মান);
	যদি (ret)
		ফেরত ret;

	conf->flags = flags;

	ফেরত SBI_OK;
}

পূর্ণসংখ্যা sbi_fwft_get(গণনা sbi_fwft_feature_t feature, unsigned long *out_val)
{
	পূর্ণসংখ্যা ret;
	কাঠামো fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	যদি (ret)
		ফেরত ret;

	ফেরত conf->feature->get(conf, out_val);
}

স্থির ধ্রুবক কাঠামো fwft_feature features[] =
{
	{
		.id = SBI_FWFT_MISALIGNED_EXC_DELEG,
		.supported = fwft_misaligned_delegation_supported,
		.set = fwft_set_misaligned_delegation,
		.get = fwft_get_misaligned_delegation,
	},
	{
		.id = SBI_FWFT_LANDING_PAD,
		.supported = fwft_lpad_supported,
		.set = fwft_enable_lpad,
		.get = fwft_get_lpad,
	},
	{
		.id = SBI_FWFT_SHADOW_স্তূপ,
		.supported = fwft_sstack_supported,
		.set = fwft_enable_sstack,
		.get = fwft_get_sstack,
	},
	{
		.id = SBI_FWFT_DOUBLE_TRAP,
		.supported = fwft_double_trap_supported,
		.set = fwft_set_double_trap,
		.get = fwft_get_double_trap,
	},
	{
		.id = SBI_FWFT_PTE_AD_HW_UPDATING,
		.supported = fwft_adue_supported,
		.set = fwft_set_adue,
		.get = fwft_get_adue,
	},
#যদি __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

পূর্ণসংখ্যা sbi_fwft_init(কাঠামো sbi_scratch *scratch, bool cold_boot)
{
	পূর্ণসংখ্যা i;
	কাঠামো fwft_hart_state *fhs;

	যদি (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(শূন্য *);
		যদি (!fwft_ptr_offset)
			ফেরত SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	যদি (!fhs) {
		fhs = sbi_zalloc(আকার(*fhs) + array_আকার(features) * আকার(কাঠামো fwft_config));
		যদি (!fhs)
			ফেরত SBI_ENOMEM;

		fhs->config_গুনতি = array_আকার(features);
		জন্য (i = 0; i < array_আকার(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	জন্য (i = 0; i < array_আকার(features); i++)
		fwft_clear_config_lock(features[i].id);

	ফেরত 0;
}
