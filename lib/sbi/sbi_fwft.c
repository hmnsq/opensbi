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

	अत्थाय (i = 0; i < array_size(fwft_defined_features); i++) {
		सचे (fwft_defined_features[i] == feature)
			पटिनिवत्त true;
	}

	पटिनिवत्त false;
}

स्थिर पूर्णाङ्क fwft_menvcfg_set_bit(unsigned long मानम्, unsigned long bit)
{
	सचे (मानम् == 1) {
		सचे (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		अञ्ञथा
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} अञ्ञथा सचे (मानम् == 0) {
		सचे (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		अञ्ञथा
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} अञ्ञथा {
		पटिनिवत्त SBI_EINVAL;
	}

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क fwft_menvcfg_read_bit(unsigned long *मानम्, unsigned long bit)
{
	unsigned long cfg;

	सचे (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	अञ्ञथा
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*मानम् = cfg != 0;

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क fwft_misaligned_delegation_supported(संरचना fwft_config *conf)
{
	सचे (!misa_extension('S'))
		पटिनिवत्त SBI_ENOTSUPP;

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क fwft_set_misaligned_delegation(संरचना fwft_config *conf,
					 unsigned long मानम्)
{
	सचे (मानम् == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	अञ्ञथा सचे (मानम् == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	अञ्ञथा
		पटिनिवत्त SBI_EINVAL;

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क fwft_get_misaligned_delegation(संरचना fwft_config *conf,
					 unsigned long *मानम्)
{
	*मानम् = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क fwft_double_trap_supported(संरचना fwft_config *conf)
{
	सचे (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		पटिनिवत्त SBI_ENOTSUPP;

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क fwft_set_double_trap(संरचना fwft_config *conf, unsigned long मानम्)
{
	पटिनिवत्त fwft_menvcfg_set_bit(मानम्, ENVCFG_DTE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_get_double_trap(संरचना fwft_config *conf, unsigned long *मानम्)
{
	पटिनिवत्त fwft_menvcfg_read_bit(मानम्, ENVCFG_DTE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_adue_supported(संरचना fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	सचे (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		पटिनिवत्त SBI_ENOTSUPP;

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क fwft_set_adue(संरचना fwft_config *conf, unsigned long मानम्)
{
	पूर्णाङ्क res = fwft_menvcfg_set_bit(मानम्, ENVCFG_ADUE_SHIFT);

	सचे (res == SBI_OK) {
		__sbi_sfence_vma_all();

		सचे (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	पटिनिवत्त res;
}

स्थिर पूर्णाङ्क fwft_get_adue(संरचना fwft_config *conf, unsigned long *मानम्)
{
	पटिनिवत्त fwft_menvcfg_read_bit(मानम्, ENVCFG_ADUE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_lpad_supported(संरचना fwft_config *conf)
{
	सचे (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		पटिनिवत्त SBI_ENOTSUPP;

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क fwft_enable_lpad(संरचना fwft_config *conf, unsigned long मानम्)
{
	पटिनिवत्त fwft_menvcfg_set_bit(मानम्, ENVCFG_LPE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_get_lpad(संरचना fwft_config *conf, unsigned long *मानम्)
{
	पटिनिवत्त fwft_menvcfg_read_bit(मानम्, ENVCFG_LPE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_sstack_supported(संरचना fwft_config *conf)
{
	सचे (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		पटिनिवत्त SBI_ENOTSUPP;

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क fwft_enable_sstack(संरचना fwft_config *conf, unsigned long मानम्)
{
	पटिनिवत्त fwft_menvcfg_set_bit(मानम्, ENVCFG_SSE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_get_sstack(संरचना fwft_config *conf, unsigned long *मानम्)
{
	पटिनिवत्त fwft_menvcfg_read_bit(मानम्, ENVCFG_SSE_SHIFT);
}

#सचे __riscv_xlen > 32
स्थिर पूर्णाङ्क fwft_pmlen_supported(संरचना fwft_config *conf)
{
	सचे (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		पटिनिवत्त SBI_ENOTSUPP;

	पटिनिवत्त SBI_OK;
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
		पटिनिवत्त SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	सचे ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		पटिनिवत्त SBI_EINVAL;
	}

	पटिनिवत्त SBI_OK;
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
		पटिनिवत्त SBI_EFAIL;
	}

	पटिनिवत्त SBI_OK;
}
#endif

स्थिर संरचना fwft_config* get_feature_config(गणना sbi_fwft_feature_t feature)
{
	पूर्णाङ्क i;
	संरचना fwft_hart_state *fhs = fwft_thishart_state_ptr();

	सचे (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		पटिनिवत्त रिक्त;

	अत्थाय (i = 0; i < fhs->config_गणनम्; i++){
		सचे (feature == fhs->configs[i].feature->id)
			पटिनिवत्त &fhs->configs[i];
	}

	पटिनिवत्त रिक्त;
}

स्थिर पूर्णाङ्क fwft_get_feature(गणना sbi_fwft_feature_t feature,
			    संरचना fwft_config **conf)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *tconf;

	tconf = get_feature_config(feature);
	सचे (!tconf) {
		सचे (fwft_is_defined_feature(feature))
			पटिनिवत्त SBI_ENOTSUPP;

		पटिनिवत्त SBI_EDENIED;
	}

	सचे (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		सचे (ret)
			पटिनिवत्त ret;
	}
	*conf = tconf;

	पटिनिवत्त SBI_SUCCESS;
}

स्थिर शून्य fwft_clear_config_lock(गणना sbi_fwft_feature_t feature)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	सचे (ret)
		पटिनिवत्त;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

पूर्णाङ्क sbi_fwft_set(गणना sbi_fwft_feature_t feature, unsigned long मानम्,
		 unsigned long flags)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	सचे (ret)
		पटिनिवत्त ret;

	सचे ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		पटिनिवत्त SBI_EINVAL;

	सचे (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		पटिनिवत्त SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, मानम्);
	सचे (ret)
		पटिनिवत्त ret;

	conf->flags = flags;

	पटिनिवत्त SBI_OK;
}

पूर्णाङ्क sbi_fwft_get(गणना sbi_fwft_feature_t feature, unsigned long *out_val)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	सचे (ret)
		पटिनिवत्त ret;

	पटिनिवत्त conf->feature->get(conf, out_val);
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
#सचे __riscv_xlen > 32
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

	सचे (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(शून्य *);
		सचे (!fwft_ptr_offset)
			पटिनिवत्त SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	सचे (!fhs) {
		fhs = sbi_zalloc(परिमाण(*fhs) + array_size(features) * परिमाण(संरचना fwft_config));
		सचे (!fhs)
			पटिनिवत्त SBI_ENOMEM;

		fhs->config_गणनम् = array_size(features);
		अत्थाय (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	अत्थाय (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	पटिनिवत्त 0;
}
