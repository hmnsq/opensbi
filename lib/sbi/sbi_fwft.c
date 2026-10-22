/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_bitmap.h>
#অন্তর্ভাৱ <sbi/sbi_ecall_interface.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_hart.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi/sbi_hfence.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi/sbi_string.h>
#অন্তর্ভাৱ <sbi/sbi_tlb.h>
#অন্তর্ভাৱ <sbi/sbi_types.h>

#অন্তর্ভাৱ <sbi/riscv_asm.h>
#অন্তর্ভাৱ <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
স্থির unsigned long fwft_ptr_offset;

#define fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), শূন্য *, fwft_ptr_offset)

#define fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define fwft_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), শূন্য *, fwft_ptr_offset, (__phs))

#define MIS_DELEG (1UL << CAUSE_MISALIGNED_LOAD | 1UL << CAUSE_MISALIGNED_STORE)

সংরচনা fwft_config;

সংরচনা fwft_feature {
	গণনা sbi_fwft_feature_t id;
	পূর্ণাঙ্ক (*supported)(সংরচনা fwft_config *conf);
	পূর্ণাঙ্ক (*set)(সংরচনা fwft_config *conf, unsigned long মান);
	পূর্ণাঙ্ক (*get)(সংরচনা fwft_config *conf, unsigned long *মান);
};

সংরচনা fwft_config {
	ধ্রুৱ সংরচনা fwft_feature *feature;
	unsigned long flags;
};

সংরচনা fwft_hart_state {
	unsigned পূর্ণাঙ্ক config_গুনতি;
	সংরচনা fwft_config configs[];
};

স্থির ধ্রুৱ unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_স্তূপ,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

স্থির bool fwft_is_defined_feature(গণনা sbi_fwft_feature_t feature)
{
	পূর্ণাঙ্ক i;

	বাবে (i = 0; i < array_size(fwft_defined_features); i++) {
		যদি (fwft_defined_features[i] == feature)
			ঘূৰাই true;
	}

	ঘূৰাই false;
}

স্থির পূর্ণাঙ্ক fwft_menvcfg_set_bit(unsigned long মান, unsigned long bit)
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
		ঘূৰাই SBI_EINVAL;
	}

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক fwft_menvcfg_read_bit(unsigned long *মান, unsigned long bit)
{
	unsigned long cfg;

	যদি (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	অন্যথা
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*মান = cfg != 0;

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক fwft_misaligned_delegation_supported(সংরচনা fwft_config *conf)
{
	যদি (!misa_extension('S'))
		ঘূৰাই SBI_ENOTSUPP;

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক fwft_set_misaligned_delegation(সংরচনা fwft_config *conf,
					 unsigned long মান)
{
	যদি (মান == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	অন্যথা যদি (মান == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	অন্যথা
		ঘূৰাই SBI_EINVAL;

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক fwft_get_misaligned_delegation(সংরচনা fwft_config *conf,
					 unsigned long *মান)
{
	*মান = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক fwft_double_trap_supported(সংরচনা fwft_config *conf)
{
	যদি (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		ঘূৰাই SBI_ENOTSUPP;

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক fwft_set_double_trap(সংরচনা fwft_config *conf, unsigned long মান)
{
	ঘূৰাই fwft_menvcfg_set_bit(মান, ENVCFG_DTE_SHIFT);
}

স্থির পূর্ণাঙ্ক fwft_get_double_trap(সংরচনা fwft_config *conf, unsigned long *মান)
{
	ঘূৰাই fwft_menvcfg_read_bit(মান, ENVCFG_DTE_SHIFT);
}

স্থির পূর্ণাঙ্ক fwft_adue_supported(সংরচনা fwft_config *conf)
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
		ঘূৰাই SBI_ENOTSUPP;

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক fwft_set_adue(সংরচনা fwft_config *conf, unsigned long মান)
{
	পূর্ণাঙ্ক res = fwft_menvcfg_set_bit(মান, ENVCFG_ADUE_SHIFT);

	যদি (res == SBI_OK) {
		__sbi_sfence_vma_all();

		যদি (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	ঘূৰাই res;
}

স্থির পূর্ণাঙ্ক fwft_get_adue(সংরচনা fwft_config *conf, unsigned long *মান)
{
	ঘূৰাই fwft_menvcfg_read_bit(মান, ENVCFG_ADUE_SHIFT);
}

স্থির পূর্ণাঙ্ক fwft_lpad_supported(সংরচনা fwft_config *conf)
{
	যদি (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		ঘূৰাই SBI_ENOTSUPP;

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক fwft_enable_lpad(সংরচনা fwft_config *conf, unsigned long মান)
{
	ঘূৰাই fwft_menvcfg_set_bit(মান, ENVCFG_LPE_SHIFT);
}

স্থির পূর্ণাঙ্ক fwft_get_lpad(সংরচনা fwft_config *conf, unsigned long *মান)
{
	ঘূৰাই fwft_menvcfg_read_bit(মান, ENVCFG_LPE_SHIFT);
}

স্থির পূর্ণাঙ্ক fwft_sstack_supported(সংরচনা fwft_config *conf)
{
	যদি (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		ঘূৰাই SBI_ENOTSUPP;

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক fwft_enable_sstack(সংরচনা fwft_config *conf, unsigned long মান)
{
	ঘূৰাই fwft_menvcfg_set_bit(মান, ENVCFG_SSE_SHIFT);
}

স্থির পূর্ণাঙ্ক fwft_get_sstack(সংরচনা fwft_config *conf, unsigned long *মান)
{
	ঘূৰাই fwft_menvcfg_read_bit(মান, ENVCFG_SSE_SHIFT);
}

#যদি __riscv_xlen > 32
স্থির পূর্ণাঙ্ক fwft_pmlen_supported(সংরচনা fwft_config *conf)
{
	যদি (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		ঘূৰাই SBI_ENOTSUPP;

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক fwft_set_pmlen(সংরচনা fwft_config *conf, unsigned long মান)
{
	unsigned long pmm, prev;

	ৱিকল্প (মান) {
	পক্ষ 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		ভঙ্গ;
	পক্ষ 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		ভঙ্গ;
	পক্ষ 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		ভঙ্গ;
	স্ৱভাৱ:
		ঘূৰাই SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	যদি ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		ঘূৰাই SBI_EINVAL;
	}

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক fwft_get_pmlen(সংরচনা fwft_config *conf, unsigned long *মান)
{
	ৱিকল্প (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	পক্ষ ENVCFG_PMM_PMLEN_0:
		*মান = 0;
		ভঙ্গ;
	পক্ষ ENVCFG_PMM_PMLEN_7:
		*মান = 7;
		ভঙ্গ;
	পক্ষ ENVCFG_PMM_PMLEN_16:
		*মান = 16;
		ভঙ্গ;
	স্ৱভাৱ:
		ঘূৰাই SBI_EFAIL;
	}

	ঘূৰাই SBI_OK;
}
#endif

স্থির সংরচনা fwft_config* get_feature_config(গণনা sbi_fwft_feature_t feature)
{
	পূর্ণাঙ্ক i;
	সংরচনা fwft_hart_state *fhs = fwft_thishart_state_ptr();

	যদি (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		ঘূৰাই রিক্ত;

	বাবে (i = 0; i < fhs->config_গুনতি; i++){
		যদি (feature == fhs->configs[i].feature->id)
			ঘূৰাই &fhs->configs[i];
	}

	ঘূৰাই রিক্ত;
}

স্থির পূর্ণাঙ্ক fwft_get_feature(গণনা sbi_fwft_feature_t feature,
			    সংরচনা fwft_config **conf)
{
	পূর্ণাঙ্ক ret;
	সংরচনা fwft_config *tconf;

	tconf = get_feature_config(feature);
	যদি (!tconf) {
		যদি (fwft_is_defined_feature(feature))
			ঘূৰাই SBI_ENOTSUPP;

		ঘূৰাই SBI_EDENIED;
	}

	যদি (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		যদি (ret)
			ঘূৰাই ret;
	}
	*conf = tconf;

	ঘূৰাই SBI_SUCCESS;
}

স্থির শূন্য fwft_clear_config_lock(গণনা sbi_fwft_feature_t feature)
{
	পূর্ণাঙ্ক ret;
	সংরচনা fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	যদি (ret)
		ঘূৰাই;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

পূর্ণাঙ্ক sbi_fwft_set(গণনা sbi_fwft_feature_t feature, unsigned long মান,
		 unsigned long flags)
{
	পূর্ণাঙ্ক ret;
	সংরচনা fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	যদি (ret)
		ঘূৰাই ret;

	যদি ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		ঘূৰাই SBI_EINVAL;

	যদি (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		ঘূৰাই SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, মান);
	যদি (ret)
		ঘূৰাই ret;

	conf->flags = flags;

	ঘূৰাই SBI_OK;
}

পূর্ণাঙ্ক sbi_fwft_get(গণনা sbi_fwft_feature_t feature, unsigned long *out_val)
{
	পূর্ণাঙ্ক ret;
	সংরচনা fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	যদি (ret)
		ঘূৰাই ret;

	ঘূৰাই conf->feature->get(conf, out_val);
}

স্থির ধ্রুৱ সংরচনা fwft_feature features[] =
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

পূর্ণাঙ্ক sbi_fwft_init(সংরচনা sbi_scratch *scratch, bool cold_boot)
{
	পূর্ণাঙ্ক i;
	সংরচনা fwft_hart_state *fhs;

	যদি (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(শূন্য *);
		যদি (!fwft_ptr_offset)
			ঘূৰাই SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	যদি (!fhs) {
		fhs = sbi_zalloc(পরিমাণ(*fhs) + array_size(features) * পরিমাণ(সংরচনা fwft_config));
		যদি (!fhs)
			ঘূৰাই SBI_ENOMEM;

		fhs->config_গুনতি = array_size(features);
		বাবে (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	বাবে (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	ঘূৰাই 0;
}
