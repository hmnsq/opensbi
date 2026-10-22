/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitmap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hfence.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_tlb.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_types.h>

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
ಸ್ಥಿರ unsigned long fwft_ptr_offset;

#define fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, fwft_ptr_offset)

#define fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define fwft_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, fwft_ptr_offset, (__phs))

#define MIS_DELEG (1UL << CAUSE_MISALIGNED_LOAD | 1UL << CAUSE_MISALIGNED_STORE)

ಸಂರಚನಾ fwft_config;

ಸಂರಚನಾ fwft_feature {
	ಗಣನಾ sbi_fwft_feature_t id;
	ಪೂರ್ಣಾಙ್ಕ (*supported)(ಸಂರಚನಾ fwft_config *conf);
	ಪೂರ್ಣಾಙ್ಕ (*set)(ಸಂರಚನಾ fwft_config *conf, unsigned long ಮೌಲ್ಯ);
	ಪೂರ್ಣಾಙ್ಕ (*get)(ಸಂರಚನಾ fwft_config *conf, unsigned long *ಮೌಲ್ಯ);
};

ಸಂರಚನಾ fwft_config {
	ಧ್ರುವ ಸಂರಚನಾ fwft_feature *feature;
	unsigned long flags;
};

ಸಂರಚನಾ fwft_hart_state {
	unsigned ಪೂರ್ಣಾಙ್ಕ config_ಎಣಿಕೆ;
	ಸಂರಚನಾ fwft_config configs[];
};

ಸ್ಥಿರ ಧ್ರುವ unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_ರಾಶಿ,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

ಸ್ಥಿರ bool fwft_is_defined_feature(ಗಣನಾ sbi_fwft_feature_t feature)
{
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿ (i = 0; i < array_size(fwft_defined_features); i++) {
		ಯದಿ (fwft_defined_features[i] == feature)
			ಪ್ರತ್ಯಾಗಮ true;
	}

	ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_menvcfg_set_bit(unsigned long ಮೌಲ್ಯ, unsigned long bit)
{
	ಯದಿ (ಮೌಲ್ಯ == 1) {
		ಯದಿ (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		ಅನ್ಯಥಾ
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} ಅನ್ಯಥಾ ಯದಿ (ಮೌಲ್ಯ == 0) {
		ಯದಿ (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		ಅನ್ಯಥಾ
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} ಅನ್ಯಥಾ {
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	}

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_menvcfg_read_bit(unsigned long *ಮೌಲ್ಯ, unsigned long bit)
{
	unsigned long cfg;

	ಯದಿ (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	ಅನ್ಯಥಾ
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*ಮೌಲ್ಯ = cfg != 0;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_misaligned_delegation_supported(ಸಂರಚನಾ fwft_config *conf)
{
	ಯದಿ (!misa_extension('S'))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_set_misaligned_delegation(ಸಂರಚನಾ fwft_config *conf,
					 unsigned long ಮೌಲ್ಯ)
{
	ಯದಿ (ಮೌಲ್ಯ == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	ಅನ್ಯಥಾ ಯದಿ (ಮೌಲ್ಯ == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_get_misaligned_delegation(ಸಂರಚನಾ fwft_config *conf,
					 unsigned long *ಮೌಲ್ಯ)
{
	*ಮೌಲ್ಯ = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_double_trap_supported(ಸಂರಚನಾ fwft_config *conf)
{
	ಯದಿ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_set_double_trap(ಸಂರಚನಾ fwft_config *conf, unsigned long ಮೌಲ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ fwft_menvcfg_set_bit(ಮೌಲ್ಯ, ENVCFG_DTE_SHIFT);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_get_double_trap(ಸಂರಚನಾ fwft_config *conf, unsigned long *ಮೌಲ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ fwft_menvcfg_read_bit(ಮೌಲ್ಯ, ENVCFG_DTE_SHIFT);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_adue_supported(ಸಂರಚನಾ fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	ಯದಿ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_set_adue(ಸಂರಚನಾ fwft_config *conf, unsigned long ಮೌಲ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ res = fwft_menvcfg_set_bit(ಮೌಲ್ಯ, ENVCFG_ADUE_SHIFT);

	ಯದಿ (res == SBI_OK) {
		__sbi_sfence_vma_all();

		ಯದಿ (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	ಪ್ರತ್ಯಾಗಮ res;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_get_adue(ಸಂರಚನಾ fwft_config *conf, unsigned long *ಮೌಲ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ fwft_menvcfg_read_bit(ಮೌಲ್ಯ, ENVCFG_ADUE_SHIFT);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_lpad_supported(ಸಂರಚನಾ fwft_config *conf)
{
	ಯದಿ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_enable_lpad(ಸಂರಚನಾ fwft_config *conf, unsigned long ಮೌಲ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ fwft_menvcfg_set_bit(ಮೌಲ್ಯ, ENVCFG_LPE_SHIFT);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_get_lpad(ಸಂರಚನಾ fwft_config *conf, unsigned long *ಮೌಲ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ fwft_menvcfg_read_bit(ಮೌಲ್ಯ, ENVCFG_LPE_SHIFT);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_sstack_supported(ಸಂರಚನಾ fwft_config *conf)
{
	ಯದಿ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_enable_sstack(ಸಂರಚನಾ fwft_config *conf, unsigned long ಮೌಲ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ fwft_menvcfg_set_bit(ಮೌಲ್ಯ, ENVCFG_SSE_SHIFT);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_get_sstack(ಸಂರಚನಾ fwft_config *conf, unsigned long *ಮೌಲ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ fwft_menvcfg_read_bit(ಮೌಲ್ಯ, ENVCFG_SSE_SHIFT);
}

#ಯದಿ __riscv_xlen > 32
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_pmlen_supported(ಸಂರಚನಾ fwft_config *conf)
{
	ಯದಿ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_set_pmlen(ಸಂರಚನಾ fwft_config *conf, unsigned long ಮೌಲ್ಯ)
{
	unsigned long pmm, prev;

	ವಿಕಲ್ಪ (ಮೌಲ್ಯ) {
	ಪಕ್ಷ 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		ಭಙ್ಗ;
	ಪಕ್ಷ 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		ಭಙ್ಗ;
	ಪಕ್ಷ 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	ಯದಿ ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	}

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_get_pmlen(ಸಂರಚನಾ fwft_config *conf, unsigned long *ಮೌಲ್ಯ)
{
	ವಿಕಲ್ಪ (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	ಪಕ್ಷ ENVCFG_PMM_PMLEN_0:
		*ಮೌಲ್ಯ = 0;
		ಭಙ್ಗ;
	ಪಕ್ಷ ENVCFG_PMM_PMLEN_7:
		*ಮೌಲ್ಯ = 7;
		ಭಙ್ಗ;
	ಪಕ್ಷ ENVCFG_PMM_PMLEN_16:
		*ಮೌಲ್ಯ = 16;
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;
	}

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}
#endif

ಸ್ಥಿರ ಸಂರಚನಾ fwft_config* get_feature_config(ಗಣನಾ sbi_fwft_feature_t feature)
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ಸಂರಚನಾ fwft_hart_state *fhs = fwft_thishart_state_ptr();

	ಯದಿ (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಪ್ರತಿ (i = 0; i < fhs->config_ಎಣಿಕೆ; i++){
		ಯದಿ (feature == fhs->configs[i].feature->id)
			ಪ್ರತ್ಯಾಗಮ &fhs->configs[i];
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fwft_get_feature(ಗಣನಾ sbi_fwft_feature_t feature,
			    ಸಂರಚನಾ fwft_config **conf)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ fwft_config *tconf;

	tconf = get_feature_config(feature);
	ಯದಿ (!tconf) {
		ಯದಿ (fwft_is_defined_feature(feature))
			ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

		ಪ್ರತ್ಯಾಗಮ SBI_EDENIED;
	}

	ಯದಿ (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}
	*conf = tconf;

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಸ್ಥಿರ ಶೂನ್ಯ fwft_clear_config_lock(ಗಣನಾ sbi_fwft_feature_t feature)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

ಪೂರ್ಣಾಙ್ಕ sbi_fwft_set(ಗಣನಾ sbi_fwft_feature_t feature, unsigned long ಮೌಲ್ಯ,
		 unsigned long flags)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ಯದಿ ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		ಪ್ರತ್ಯಾಗಮ SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, ಮೌಲ್ಯ);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	conf->flags = flags;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಪೂರ್ಣಾಙ್ಕ sbi_fwft_get(ಗಣನಾ sbi_fwft_feature_t feature, unsigned long *out_val)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ಪ್ರತ್ಯಾಗಮ conf->feature->get(conf, out_val);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fwft_feature features[] =
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
		.id = SBI_FWFT_SHADOW_ರಾಶಿ,
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
#ಯದಿ __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

ಪೂರ್ಣಾಙ್ಕ sbi_fwft_init(ಸಂರಚನಾ sbi_scratch *scratch, bool cold_boot)
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ಸಂರಚನಾ fwft_hart_state *fhs;

	ಯದಿ (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(ಶೂನ್ಯ *);
		ಯದಿ (!fwft_ptr_offset)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	ಯದಿ (!fhs) {
		fhs = sbi_zalloc(ಪರಿಮಾಣ(*fhs) + array_size(features) * ಪರಿಮಾಣ(ಸಂರಚನಾ fwft_config));
		ಯದಿ (!fhs)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

		fhs->config_ಎಣಿಕೆ = array_size(features);
		ಪ್ರತಿ (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	ಪ್ರತಿ (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	ಪ್ರತ್ಯಾಗಮ 0;
}
