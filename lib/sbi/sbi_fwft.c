/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_bitmap.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_hfence.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_tlb.h>
#अन्तर्भाव <sbi/sbi_types.h>

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
स्थिर unsigned long fwft_ptr_offset;

#define fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), शून्य *, fwft_ptr_offset)

#define fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define fwft_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), शून्य *, fwft_ptr_offset, (__phs))

#define MIS_DELEG (1UL << CAUSE_MISALIGNED_LOAD | 1UL << CAUSE_MISALIGNED_STORE)

संरचना fwft_config;

संरचना fwft_feature {
	गणना sbi_fwft_feature_t id;
	पूर्णाङ्क (*supported)(संरचना fwft_config *conf);
	पूर्णाङ्क (*set)(संरचना fwft_config *conf, unsigned long मानम्);
	पूर्णाङ्क (*get)(संरचना fwft_config *conf, unsigned long *मानम्);
};

संरचना fwft_config {
	ध्रुव संरचना fwft_feature *feature;
	unsigned long flags;
};

संरचना fwft_hart_state {
	unsigned पूर्णाङ्क config_गणनम्;
	संरचना fwft_config configs[];
};

स्थिर ध्रुव unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_राशिः,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

स्थिर bool fwft_is_defined_feature(गणना sbi_fwft_feature_t feature)
{
	पूर्णाङ्क i;

	प्रति (i = 0; i < array_size(fwft_defined_features); i++) {
		जइ (fwft_defined_features[i] == feature)
			प्रत्यागम true;
	}

	प्रत्यागम false;
}

स्थिर पूर्णाङ्क fwft_menvcfg_set_bit(unsigned long मानम्, unsigned long bit)
{
	जइ (मानम् == 1) {
		जइ (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		अण्णहा
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} अण्णहा जइ (मानम् == 0) {
		जइ (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		अण्णहा
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} अण्णहा {
		प्रत्यागम SBI_EINVAL;
	}

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_menvcfg_read_bit(unsigned long *मानम्, unsigned long bit)
{
	unsigned long cfg;

	जइ (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	अण्णहा
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*मानम् = cfg != 0;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_misaligned_delegation_supported(संरचना fwft_config *conf)
{
	जइ (!misa_extension('S'))
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_set_misaligned_delegation(संरचना fwft_config *conf,
					 unsigned long मानम्)
{
	जइ (मानम् == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	अण्णहा जइ (मानम् == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	अण्णहा
		प्रत्यागम SBI_EINVAL;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_get_misaligned_delegation(संरचना fwft_config *conf,
					 unsigned long *मानम्)
{
	*मानम् = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_double_trap_supported(संरचना fwft_config *conf)
{
	जइ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_set_double_trap(संरचना fwft_config *conf, unsigned long मानम्)
{
	प्रत्यागम fwft_menvcfg_set_bit(मानम्, ENVCFG_DTE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_get_double_trap(संरचना fwft_config *conf, unsigned long *मानम्)
{
	प्रत्यागम fwft_menvcfg_read_bit(मानम्, ENVCFG_DTE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_adue_supported(संरचना fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	जइ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_set_adue(संरचना fwft_config *conf, unsigned long मानम्)
{
	पूर्णाङ्क res = fwft_menvcfg_set_bit(मानम्, ENVCFG_ADUE_SHIFT);

	जइ (res == SBI_OK) {
		__sbi_sfence_vma_all();

		जइ (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	प्रत्यागम res;
}

स्थिर पूर्णाङ्क fwft_get_adue(संरचना fwft_config *conf, unsigned long *मानम्)
{
	प्रत्यागम fwft_menvcfg_read_bit(मानम्, ENVCFG_ADUE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_lpad_supported(संरचना fwft_config *conf)
{
	जइ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_enable_lpad(संरचना fwft_config *conf, unsigned long मानम्)
{
	प्रत्यागम fwft_menvcfg_set_bit(मानम्, ENVCFG_LPE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_get_lpad(संरचना fwft_config *conf, unsigned long *मानम्)
{
	प्रत्यागम fwft_menvcfg_read_bit(मानम्, ENVCFG_LPE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_sstack_supported(संरचना fwft_config *conf)
{
	जइ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_enable_sstack(संरचना fwft_config *conf, unsigned long मानम्)
{
	प्रत्यागम fwft_menvcfg_set_bit(मानम्, ENVCFG_SSE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_get_sstack(संरचना fwft_config *conf, unsigned long *मानम्)
{
	प्रत्यागम fwft_menvcfg_read_bit(मानम्, ENVCFG_SSE_SHIFT);
}

#जइ __riscv_xlen > 32
स्थिर पूर्णाङ्क fwft_pmlen_supported(संरचना fwft_config *conf)
{
	जइ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_set_pmlen(संरचना fwft_config *conf, unsigned long मानम्)
{
	unsigned long pmm, prev;

	विकल्प (मानम्) {
	पक्ष 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		भङ्ग;
	पक्ष 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		भङ्ग;
	पक्ष 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		भङ्ग;
	स्वभाव:
		प्रत्यागम SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	जइ ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		प्रत्यागम SBI_EINVAL;
	}

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_get_pmlen(संरचना fwft_config *conf, unsigned long *मानम्)
{
	विकल्प (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	पक्ष ENVCFG_PMM_PMLEN_0:
		*मानम् = 0;
		भङ्ग;
	पक्ष ENVCFG_PMM_PMLEN_7:
		*मानम् = 7;
		भङ्ग;
	पक्ष ENVCFG_PMM_PMLEN_16:
		*मानम् = 16;
		भङ्ग;
	स्वभाव:
		प्रत्यागम SBI_EFAIL;
	}

	प्रत्यागम SBI_OK;
}
#endif

स्थिर संरचना fwft_config* get_feature_config(गणना sbi_fwft_feature_t feature)
{
	पूर्णाङ्क i;
	संरचना fwft_hart_state *fhs = fwft_thishart_state_ptr();

	जइ (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		प्रत्यागम रिक्त;

	प्रति (i = 0; i < fhs->config_गणनम्; i++){
		जइ (feature == fhs->configs[i].feature->id)
			प्रत्यागम &fhs->configs[i];
	}

	प्रत्यागम रिक्त;
}

स्थिर पूर्णाङ्क fwft_get_feature(गणना sbi_fwft_feature_t feature,
			    संरचना fwft_config **conf)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *tconf;

	tconf = get_feature_config(feature);
	जइ (!tconf) {
		जइ (fwft_is_defined_feature(feature))
			प्रत्यागम SBI_ENOTSUPP;

		प्रत्यागम SBI_EDENIED;
	}

	जइ (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		जइ (ret)
			प्रत्यागम ret;
	}
	*conf = tconf;

	प्रत्यागम SBI_SUCCESS;
}

स्थिर शून्य fwft_clear_config_lock(गणना sbi_fwft_feature_t feature)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	जइ (ret)
		प्रत्यागम;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

पूर्णाङ्क sbi_fwft_set(गणना sbi_fwft_feature_t feature, unsigned long मानम्,
		 unsigned long flags)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	जइ (ret)
		प्रत्यागम ret;

	जइ ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		प्रत्यागम SBI_EINVAL;

	जइ (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		प्रत्यागम SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, मानम्);
	जइ (ret)
		प्रत्यागम ret;

	conf->flags = flags;

	प्रत्यागम SBI_OK;
}

पूर्णाङ्क sbi_fwft_get(गणना sbi_fwft_feature_t feature, unsigned long *out_val)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	जइ (ret)
		प्रत्यागम ret;

	प्रत्यागम conf->feature->get(conf, out_val);
}

स्थिर ध्रुव संरचना fwft_feature features[] =
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
		.id = SBI_FWFT_SHADOW_राशिः,
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
#जइ __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

पूर्णाङ्क sbi_fwft_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	पूर्णाङ्क i;
	संरचना fwft_hart_state *fhs;

	जइ (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(शून्य *);
		जइ (!fwft_ptr_offset)
			प्रत्यागम SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	जइ (!fhs) {
		fhs = sbi_zalloc(परिमाण(*fhs) + array_size(features) * परिमाण(संरचना fwft_config));
		जइ (!fhs)
			प्रत्यागम SBI_ENOMEM;

		fhs->config_गणनम् = array_size(features);
		प्रति (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	प्रति (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	प्रत्यागम 0;
}
