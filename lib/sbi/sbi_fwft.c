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
स्थिर अचिह्नित दीर्घ fwft_ptr_विस्थापनम्;

#परिभाषय fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), शून्य *, fwft_ptr_विस्थापनम्)

#परिभाषय fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#परिभाषय fwft_समुच्चयः_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), शून्य *, fwft_ptr_विस्थापनम्, (__phs))

#परिभाषय MIS_DELEG (1UL << CAUSE_MISALIGNED_आरोपय | 1UL << CAUSE_MISALIGNED_STORE)

संरचना fwft_config;

संरचना fwft_feature {
	गणना sbi_fwft_feature_t id;
	पूर्णाङ्क (*supported)(संरचना fwft_config *conf);
	पूर्णाङ्क (*समुच्चयः)(संरचना fwft_config *conf, अचिह्नित दीर्घ मानम्);
	पूर्णाङ्क (*get)(संरचना fwft_config *conf, अचिह्नित दीर्घ *मानम्);
};

संरचना fwft_config {
	ध्रुव संरचना fwft_feature *feature;
	अचिह्नित दीर्घ flags;
};

संरचना fwft_hart_state {
	अचिह्नित पूर्णाङ्क config_गणनम्;
	संरचना fwft_config configs[];
};

स्थिर ध्रुव अचिह्नित दीर्घ fwft_defined_features[] = {
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

	प्रति (i = 0; i < array_आकार(fwft_defined_features); i++) {
		यदि (fwft_defined_features[i] == feature)
			प्रत्यागम true;
	}

	प्रत्यागम false;
}

स्थिर पूर्णाङ्क fwft_menvcfg_समुच्चयः_bit(अचिह्नित दीर्घ मानम्, अचिह्नित दीर्घ bit)
{
	यदि (मानम् == 1) {
		यदि (bit >= 32 && __riscv_xlen == 32)
			csr_समुच्चयः(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		अन्यथा
			csr_समुच्चयः(CSR_MENVCFG, _ULL(1) << bit);

	} अन्यथा यदि (मानम् == 0) {
		यदि (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		अन्यथा
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} अन्यथा {
		प्रत्यागम SBI_EINVAL;
	}

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_menvcfg_read_bit(अचिह्नित दीर्घ *मानम्, अचिह्नित दीर्घ bit)
{
	अचिह्नित दीर्घ cfg;

	यदि (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	अन्यथा
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*मानम् = cfg != 0;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_misaligned_delegation_supported(संरचना fwft_config *conf)
{
	यदि (!misa_extension('S'))
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_समुच्चयः_misaligned_delegation(संरचना fwft_config *conf,
					 अचिह्नित दीर्घ मानम्)
{
	यदि (मानम् == 1)
		csr_समुच्चयः(CSR_MEDELEG, MIS_DELEG);
	अन्यथा यदि (मानम् == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	अन्यथा
		प्रत्यागम SBI_EINVAL;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_get_misaligned_delegation(संरचना fwft_config *conf,
					 अचिह्नित दीर्घ *मानम्)
{
	*मानम् = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_double_trap_supported(संरचना fwft_config *conf)
{
	यदि (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_समुच्चयः_double_trap(संरचना fwft_config *conf, अचिह्नित दीर्घ मानम्)
{
	प्रत्यागम fwft_menvcfg_समुच्चयः_bit(मानम्, ENVCFG_DTE_सरणम्);
}

स्थिर पूर्णाङ्क fwft_get_double_trap(संरचना fwft_config *conf, अचिह्नित दीर्घ *मानम्)
{
	प्रत्यागम fwft_menvcfg_read_bit(मानम्, ENVCFG_DTE_सरणम्);
}

स्थिर पूर्णाङ्क fwft_adue_supported(संरचना fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	यदि (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_समुच्चयः_adue(संरचना fwft_config *conf, अचिह्नित दीर्घ मानम्)
{
	पूर्णाङ्क res = fwft_menvcfg_समुच्चयः_bit(मानम्, ENVCFG_ADUE_सरणम्);

	यदि (res == SBI_OK) {
		__sbi_sfence_vma_all();

		यदि (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	प्रत्यागम res;
}

स्थिर पूर्णाङ्क fwft_get_adue(संरचना fwft_config *conf, अचिह्नित दीर्घ *मानम्)
{
	प्रत्यागम fwft_menvcfg_read_bit(मानम्, ENVCFG_ADUE_सरणम्);
}

स्थिर पूर्णाङ्क fwft_lpad_supported(संरचना fwft_config *conf)
{
	यदि (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_enable_lpad(संरचना fwft_config *conf, अचिह्नित दीर्घ मानम्)
{
	प्रत्यागम fwft_menvcfg_समुच्चयः_bit(मानम्, ENVCFG_LPE_सरणम्);
}

स्थिर पूर्णाङ्क fwft_get_lpad(संरचना fwft_config *conf, अचिह्नित दीर्घ *मानम्)
{
	प्रत्यागम fwft_menvcfg_read_bit(मानम्, ENVCFG_LPE_सरणम्);
}

स्थिर पूर्णाङ्क fwft_sstack_supported(संरचना fwft_config *conf)
{
	यदि (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_enable_sstack(संरचना fwft_config *conf, अचिह्नित दीर्घ मानम्)
{
	प्रत्यागम fwft_menvcfg_समुच्चयः_bit(मानम्, ENVCFG_SSE_सरणम्);
}

स्थिर पूर्णाङ्क fwft_get_sstack(संरचना fwft_config *conf, अचिह्नित दीर्घ *मानम्)
{
	प्रत्यागम fwft_menvcfg_read_bit(मानम्, ENVCFG_SSE_सरणम्);
}

#यदि __riscv_xlen > 32
स्थिर पूर्णाङ्क fwft_pmlen_supported(संरचना fwft_config *conf)
{
	यदि (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_समुच्चयः_pmlen(संरचना fwft_config *conf, अचिह्नित दीर्घ मानम्)
{
	अचिह्नित दीर्घ pmm, पूर्व;

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

	पूर्व = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_समुच्चयः(CSR_MENVCFG, pmm);
	यदि ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, पूर्व);
		प्रत्यागम SBI_EINVAL;
	}

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क fwft_get_pmlen(संरचना fwft_config *conf, अचिह्नित दीर्घ *मानम्)
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
#यदि_अन्तः

स्थिर संरचना fwft_config* get_feature_config(गणना sbi_fwft_feature_t feature)
{
	पूर्णाङ्क i;
	संरचना fwft_hart_state *fhs = fwft_thishart_state_ptr();

	यदि (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		प्रत्यागम रिक्त;

	प्रति (i = 0; i < fhs->config_गणनम्; i++){
		यदि (feature == fhs->configs[i].feature->id)
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
	यदि (!tconf) {
		यदि (fwft_is_defined_feature(feature))
			प्रत्यागम SBI_ENOTSUPP;

		प्रत्यागम SBI_EDENIED;
	}

	यदि (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		यदि (ret)
			प्रत्यागम ret;
	}
	*conf = tconf;

	प्रत्यागम SBI_SUCCESS;
}

स्थिर शून्य fwft_clear_config_तालकम्(गणना sbi_fwft_feature_t feature)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	यदि (ret)
		प्रत्यागम;

	conf->flags &= ~SBI_FWFT_समुच्चयः_FLAG_तालकम्;
}

पूर्णाङ्क sbi_fwft_समुच्चयः(गणना sbi_fwft_feature_t feature, अचिह्नित दीर्घ मानम्,
		 अचिह्नित दीर्घ flags)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	यदि (ret)
		प्रत्यागम ret;

	यदि ((flags & ~SBI_FWFT_समुच्चयः_FLAG_तालकम्) != 0)
		प्रत्यागम SBI_EINVAL;

	यदि (conf->flags & SBI_FWFT_समुच्चयः_FLAG_तालकम्)
		प्रत्यागम SBI_EDENIED_LOCKED;

	ret = conf->feature->समुच्चयः(conf, मानम्);
	यदि (ret)
		प्रत्यागम ret;

	conf->flags = flags;

	प्रत्यागम SBI_OK;
}

पूर्णाङ्क sbi_fwft_get(गणना sbi_fwft_feature_t feature, अचिह्नित दीर्घ *बहिर्गमनम्_val)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	यदि (ret)
		प्रत्यागम ret;

	प्रत्यागम conf->feature->get(conf, बहिर्गमनम्_val);
}

स्थिर ध्रुव संरचना fwft_feature features[] =
{
	{
		.id = SBI_FWFT_MISALIGNED_EXC_DELEG,
		.supported = fwft_misaligned_delegation_supported,
		.समुच्चयः = fwft_समुच्चयः_misaligned_delegation,
		.get = fwft_get_misaligned_delegation,
	},
	{
		.id = SBI_FWFT_LANDING_PAD,
		.supported = fwft_lpad_supported,
		.समुच्चयः = fwft_enable_lpad,
		.get = fwft_get_lpad,
	},
	{
		.id = SBI_FWFT_SHADOW_राशिः,
		.supported = fwft_sstack_supported,
		.समुच्चयः = fwft_enable_sstack,
		.get = fwft_get_sstack,
	},
	{
		.id = SBI_FWFT_DOUBLE_TRAP,
		.supported = fwft_double_trap_supported,
		.समुच्चयः = fwft_समुच्चयः_double_trap,
		.get = fwft_get_double_trap,
	},
	{
		.id = SBI_FWFT_PTE_AD_HW_UPDATING,
		.supported = fwft_adue_supported,
		.समुच्चयः = fwft_समुच्चयः_adue,
		.get = fwft_get_adue,
	},
#यदि __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.समुच्चयः = fwft_समुच्चयः_pmlen,
		.get = fwft_get_pmlen,
	},
#यदि_अन्तः
};

पूर्णाङ्क sbi_fwft_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	पूर्णाङ्क i;
	संरचना fwft_hart_state *fhs;

	यदि (cold_boot) {
		fwft_ptr_विस्थापनम् = sbi_scratch_alloc_type_विस्थापनम्(शून्य *);
		यदि (!fwft_ptr_विस्थापनम्)
			प्रत्यागम SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	यदि (!fhs) {
		fhs = sbi_zalloc(परिमाण(*fhs) + array_आकार(features) * परिमाण(संरचना fwft_config));
		यदि (!fhs)
			प्रत्यागम SBI_ENOMEM;

		fhs->config_गणनम् = array_आकार(features);
		प्रति (i = 0; i < array_आकार(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_समुच्चयः_hart_state_ptr(scratch, fhs);
	}

	प्रति (i = 0; i < array_आकार(features); i++)
		fwft_clear_config_तालकम्(features[i].id);

	प्रत्यागम 0;
}
