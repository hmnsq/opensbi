/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_bitmap.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_hfence.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_tlb.h>
#అన్తర్భావ <sbi/sbi_types.h>

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
స్థిర గుర్తులేని దీర్ఘ fwft_ptr_స్థానభ్రంశం;

#నిర్వచించు fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), శూన్య *, fwft_ptr_స్థానభ్రంశం)

#నిర్వచించు fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#నిర్వచించు fwft_సమితి_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), శూన్య *, fwft_ptr_స్థానభ్రంశం, (__phs))

#నిర్వచించు MIS_DELEG (1UL << CAUSE_MISALIGNED_ఎక్కించు | 1UL << CAUSE_MISALIGNED_STORE)

నిర్మాణం fwft_config;

నిర్మాణం fwft_feature {
	గణన sbi_fwft_feature_t id;
	పూర్ణాంకం (*supported)(నిర్మాణం fwft_config *conf);
	పూర్ణాంకం (*సమితి)(నిర్మాణం fwft_config *conf, గుర్తులేని దీర్ఘ విలువ);
	పూర్ణాంకం (*get)(నిర్మాణం fwft_config *conf, గుర్తులేని దీర్ఘ *విలువ);
};

నిర్మాణం fwft_config {
	స్థిరరాశి నిర్మాణం fwft_feature *feature;
	గుర్తులేని దీర్ఘ flags;
};

నిర్మాణం fwft_hart_state {
	గుర్తులేని పూర్ణాంకం config_లెక్క;
	నిర్మాణం fwft_config configs[];
};

స్థిర స్థిరరాశి గుర్తులేని దీర్ఘ fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_దొంతర,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_సూచిక_MASKING_PMLEN,
};

స్థిర bool fwft_is_defined_feature(గణన sbi_fwft_feature_t feature)
{
	పూర్ణాంకం i;

	కొరకు (i = 0; i < array_కొలత(fwft_defined_features); i++) {
		ఒకవేళ (fwft_defined_features[i] == feature)
			తిరిగి true;
	}

	తిరిగి false;
}

స్థిర పూర్ణాంకం fwft_menvcfg_సమితి_బిట్టు(గుర్తులేని దీర్ఘ విలువ, గుర్తులేని దీర్ఘ బిట్టు_2)
{
	ఒకవేళ (విలువ == 1) {
		ఒకవేళ (బిట్టు_2 >= 32 && __riscv_xlen == 32)
			csr_సమితి(CSR_MENVCFGH, _ULL(1) << (బిట్టు_2 - 32));
		లేకపోతే
			csr_సమితి(CSR_MENVCFG, _ULL(1) << బిట్టు_2);

	} లేకపోతే ఒకవేళ (విలువ == 0) {
		ఒకవేళ (బిట్టు_2 >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (బిట్టు_2 - 32));
		లేకపోతే
			csr_clear(CSR_MENVCFG, _ULL(1) << బిట్టు_2);
	} లేకపోతే {
		తిరిగి SBI_EINVAL;
	}

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం fwft_menvcfg_read_బిట్టు(గుర్తులేని దీర్ఘ *విలువ, గుర్తులేని దీర్ఘ బిట్టు_2)
{
	గుర్తులేని దీర్ఘ cfg;

	ఒకవేళ (బిట్టు_2 >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (బిట్టు_2 - 32));
	లేకపోతే
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << బిట్టు_2);

	*విలువ = cfg != 0;

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం fwft_misaligned_delegation_supported(నిర్మాణం fwft_config *conf)
{
	ఒకవేళ (!misa_extension('S'))
		తిరిగి SBI_ENOTSUPP;

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం fwft_సమితి_misaligned_delegation(నిర్మాణం fwft_config *conf,
					 గుర్తులేని దీర్ఘ విలువ)
{
	ఒకవేళ (విలువ == 1)
		csr_సమితి(CSR_MEDELEG, MIS_DELEG);
	లేకపోతే ఒకవేళ (విలువ == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	లేకపోతే
		తిరిగి SBI_EINVAL;

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం fwft_get_misaligned_delegation(నిర్మాణం fwft_config *conf,
					 గుర్తులేని దీర్ఘ *విలువ)
{
	*విలువ = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం fwft_double_trap_supported(నిర్మాణం fwft_config *conf)
{
	ఒకవేళ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		తిరిగి SBI_ENOTSUPP;

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం fwft_సమితి_double_trap(నిర్మాణం fwft_config *conf, గుర్తులేని దీర్ఘ విలువ)
{
	తిరిగి fwft_menvcfg_సమితి_బిట్టు(విలువ, ENVCFG_DTE_జరుపు);
}

స్థిర పూర్ణాంకం fwft_get_double_trap(నిర్మాణం fwft_config *conf, గుర్తులేని దీర్ఘ *విలువ)
{
	తిరిగి fwft_menvcfg_read_బిట్టు(విలువ, ENVCFG_DTE_జరుపు);
}

స్థిర పూర్ణాంకం fwft_adue_supported(నిర్మాణం fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	ఒకవేళ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		తిరిగి SBI_ENOTSUPP;

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం fwft_సమితి_adue(నిర్మాణం fwft_config *conf, గుర్తులేని దీర్ఘ విలువ)
{
	పూర్ణాంకం res = fwft_menvcfg_సమితి_బిట్టు(విలువ, ENVCFG_ADUE_జరుపు);

	ఒకవేళ (res == SBI_OK) {
		__sbi_sfence_vma_all();

		ఒకవేళ (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	తిరిగి res;
}

స్థిర పూర్ణాంకం fwft_get_adue(నిర్మాణం fwft_config *conf, గుర్తులేని దీర్ఘ *విలువ)
{
	తిరిగి fwft_menvcfg_read_బిట్టు(విలువ, ENVCFG_ADUE_జరుపు);
}

స్థిర పూర్ణాంకం fwft_lpad_supported(నిర్మాణం fwft_config *conf)
{
	ఒకవేళ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		తిరిగి SBI_ENOTSUPP;

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం fwft_enable_lpad(నిర్మాణం fwft_config *conf, గుర్తులేని దీర్ఘ విలువ)
{
	తిరిగి fwft_menvcfg_సమితి_బిట్టు(విలువ, ENVCFG_LPE_జరుపు);
}

స్థిర పూర్ణాంకం fwft_get_lpad(నిర్మాణం fwft_config *conf, గుర్తులేని దీర్ఘ *విలువ)
{
	తిరిగి fwft_menvcfg_read_బిట్టు(విలువ, ENVCFG_LPE_జరుపు);
}

స్థిర పూర్ణాంకం fwft_sstack_supported(నిర్మాణం fwft_config *conf)
{
	ఒకవేళ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		తిరిగి SBI_ENOTSUPP;

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం fwft_enable_sstack(నిర్మాణం fwft_config *conf, గుర్తులేని దీర్ఘ విలువ)
{
	తిరిగి fwft_menvcfg_సమితి_బిట్టు(విలువ, ENVCFG_SSE_జరుపు);
}

స్థిర పూర్ణాంకం fwft_get_sstack(నిర్మాణం fwft_config *conf, గుర్తులేని దీర్ఘ *విలువ)
{
	తిరిగి fwft_menvcfg_read_బిట్టు(విలువ, ENVCFG_SSE_జరుపు);
}

#ఒకవేళ __riscv_xlen > 32
స్థిర పూర్ణాంకం fwft_pmlen_supported(నిర్మాణం fwft_config *conf)
{
	ఒకవేళ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		తిరిగి SBI_ENOTSUPP;

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం fwft_సమితి_pmlen(నిర్మాణం fwft_config *conf, గుర్తులేని దీర్ఘ విలువ)
{
	గుర్తులేని దీర్ఘ pmm, prev;

	ఎంపిక (విలువ) {
	స్థితి 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		ఆపు;
	స్థితి 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		ఆపు;
	స్థితి 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		ఆపు;
	అప్రమేయ:
		తిరిగి SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_సమితి(CSR_MENVCFG, pmm);
	ఒకవేళ ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		తిరిగి SBI_EINVAL;
	}

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం fwft_get_pmlen(నిర్మాణం fwft_config *conf, గుర్తులేని దీర్ఘ *విలువ)
{
	ఎంపిక (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	స్థితి ENVCFG_PMM_PMLEN_0:
		*విలువ = 0;
		ఆపు;
	స్థితి ENVCFG_PMM_PMLEN_7:
		*విలువ = 7;
		ఆపు;
	స్థితి ENVCFG_PMM_PMLEN_16:
		*విలువ = 16;
		ఆపు;
	అప్రమేయ:
		తిరిగి SBI_EFAIL;
	}

	తిరిగి SBI_OK;
}
#అయితే_ముగింపు

స్థిర నిర్మాణం fwft_config* get_feature_config(గణన sbi_fwft_feature_t feature)
{
	పూర్ణాంకం i;
	నిర్మాణం fwft_hart_state *fhs = fwft_thishart_state_ptr();

	ఒకవేళ (feature & SBI_FWFT_GLOBAL_FEATURE_బిట్టు)
		తిరిగి ఖాళీ;

	కొరకు (i = 0; i < fhs->config_లెక్క; i++){
		ఒకవేళ (feature == fhs->configs[i].feature->id)
			తిరిగి &fhs->configs[i];
	}

	తిరిగి ఖాళీ;
}

స్థిర పూర్ణాంకం fwft_get_feature(గణన sbi_fwft_feature_t feature,
			    నిర్మాణం fwft_config **conf)
{
	పూర్ణాంకం ret;
	నిర్మాణం fwft_config *tconf;

	tconf = get_feature_config(feature);
	ఒకవేళ (!tconf) {
		ఒకవేళ (fwft_is_defined_feature(feature))
			తిరిగి SBI_ENOTSUPP;

		తిరిగి SBI_EDENIED;
	}

	ఒకవేళ (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		ఒకవేళ (ret)
			తిరిగి ret;
	}
	*conf = tconf;

	తిరిగి SBI_SUCCESS;
}

స్థిర శూన్య fwft_clear_config_తాళం(గణన sbi_fwft_feature_t feature)
{
	పూర్ణాంకం ret;
	నిర్మాణం fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ఒకవేళ (ret)
		తిరిగి;

	conf->flags &= ~SBI_FWFT_సమితి_FLAG_తాళం;
}

పూర్ణాంకం sbi_fwft_సమితి(గణన sbi_fwft_feature_t feature, గుర్తులేని దీర్ఘ విలువ,
		 గుర్తులేని దీర్ఘ flags)
{
	పూర్ణాంకం ret;
	నిర్మాణం fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ఒకవేళ (ret)
		తిరిగి ret;

	ఒకవేళ ((flags & ~SBI_FWFT_సమితి_FLAG_తాళం) != 0)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (conf->flags & SBI_FWFT_సమితి_FLAG_తాళం)
		తిరిగి SBI_EDENIED_LOCKED;

	ret = conf->feature->సమితి(conf, విలువ);
	ఒకవేళ (ret)
		తిరిగి ret;

	conf->flags = flags;

	తిరిగి SBI_OK;
}

పూర్ణాంకం sbi_fwft_get(గణన sbi_fwft_feature_t feature, గుర్తులేని దీర్ఘ *బహిర్గతం_val)
{
	పూర్ణాంకం ret;
	నిర్మాణం fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ఒకవేళ (ret)
		తిరిగి ret;

	తిరిగి conf->feature->get(conf, బహిర్గతం_val);
}

స్థిర స్థిరరాశి నిర్మాణం fwft_feature features[] =
{
	{
		.id = SBI_FWFT_MISALIGNED_EXC_DELEG,
		.supported = fwft_misaligned_delegation_supported,
		.సమితి = fwft_సమితి_misaligned_delegation,
		.get = fwft_get_misaligned_delegation,
	},
	{
		.id = SBI_FWFT_LANDING_PAD,
		.supported = fwft_lpad_supported,
		.సమితి = fwft_enable_lpad,
		.get = fwft_get_lpad,
	},
	{
		.id = SBI_FWFT_SHADOW_దొంతర,
		.supported = fwft_sstack_supported,
		.సమితి = fwft_enable_sstack,
		.get = fwft_get_sstack,
	},
	{
		.id = SBI_FWFT_DOUBLE_TRAP,
		.supported = fwft_double_trap_supported,
		.సమితి = fwft_సమితి_double_trap,
		.get = fwft_get_double_trap,
	},
	{
		.id = SBI_FWFT_PTE_AD_HW_UPDATING,
		.supported = fwft_adue_supported,
		.సమితి = fwft_సమితి_adue,
		.get = fwft_get_adue,
	},
#ఒకవేళ __riscv_xlen > 32
	{
		.id = SBI_FWFT_సూచిక_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.సమితి = fwft_సమితి_pmlen,
		.get = fwft_get_pmlen,
	},
#అయితే_ముగింపు
};

పూర్ణాంకం sbi_fwft_init(నిర్మాణం sbi_scratch *scratch, bool cold_boot)
{
	పూర్ణాంకం i;
	నిర్మాణం fwft_hart_state *fhs;

	ఒకవేళ (cold_boot) {
		fwft_ptr_స్థానభ్రంశం = sbi_scratch_alloc_type_స్థానభ్రంశం(శూన్య *);
		ఒకవేళ (!fwft_ptr_స్థానభ్రంశం)
			తిరిగి SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	ఒకవేళ (!fhs) {
		fhs = sbi_zalloc(పరిమాణం(*fhs) + array_కొలత(features) * పరిమాణం(నిర్మాణం fwft_config));
		ఒకవేళ (!fhs)
			తిరిగి SBI_ENOMEM;

		fhs->config_లెక్క = array_కొలత(features);
		కొరకు (i = 0; i < array_కొలత(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_సమితి_hart_state_ptr(scratch, fhs);
	}

	కొరకు (i = 0; i < array_కొలత(features); i++)
		fwft_clear_config_తాళం(features[i].id);

	తిరిగి 0;
}
