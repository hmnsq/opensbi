/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_bitmap.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_hfence.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_tlb.h>
#شامل <sbi/sbi_types.h>

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
ساکن غیرعلامتی طویل fwft_ptr_انحراف;

#تعریف_کرو fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), خالی *, fwft_ptr_انحراف)

#تعریف_کرو fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#تعریف_کرو fwft_سیٹ_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), خالی *, fwft_ptr_انحراف, (__phs))

#تعریف_کرو MIS_DELEG (1UL << CAUSE_MISALIGNED_بھرو | 1UL << CAUSE_MISALIGNED_STORE)

ساخت fwft_config;

ساخت fwft_feature {
	شمار sbi_fwft_feature_t id;
	صحیح (*supported)(ساخت fwft_config *conf);
	صحیح (*سیٹ)(ساخت fwft_config *conf, غیرعلامتی طویل قدر);
	صحیح (*get)(ساخت fwft_config *conf, غیرعلامتی طویل *قدر);
};

ساخت fwft_config {
	ثابت ساخت fwft_feature *feature;
	غیرعلامتی طویل flags;
};

ساخت fwft_hart_state {
	غیرعلامتی صحیح config_گنتی;
	ساخت fwft_config configs[];
};

ساکن ثابت غیرعلامتی طویل fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_ڈھیر,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_اشارہ_گر_MASKING_PMLEN,
};

ساکن bool fwft_is_defined_feature(شمار sbi_fwft_feature_t feature)
{
	صحیح i;

	برائے (i = 0; i < array_سائز(fwft_defined_features); i++) {
		اگر (fwft_defined_features[i] == feature)
			واپس true;
	}

	واپس false;
}

ساکن صحیح fwft_menvcfg_سیٹ_ثنائی_ہندسہ(غیرعلامتی طویل قدر, غیرعلامتی طویل ثنائی_ہندسہ_2)
{
	اگر (قدر == 1) {
		اگر (ثنائی_ہندسہ_2 >= 32 && __riscv_xlen == 32)
			csr_سیٹ(CSR_MENVCFGH, _ULL(1) << (ثنائی_ہندسہ_2 - 32));
		ورنہ
			csr_سیٹ(CSR_MENVCFG, _ULL(1) << ثنائی_ہندسہ_2);

	} ورنہ اگر (قدر == 0) {
		اگر (ثنائی_ہندسہ_2 >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (ثنائی_ہندسہ_2 - 32));
		ورنہ
			csr_clear(CSR_MENVCFG, _ULL(1) << ثنائی_ہندسہ_2);
	} ورنہ {
		واپس SBI_EINVAL;
	}

	واپس SBI_OK;
}

ساکن صحیح fwft_menvcfg_read_ثنائی_ہندسہ(غیرعلامتی طویل *قدر, غیرعلامتی طویل ثنائی_ہندسہ_2)
{
	غیرعلامتی طویل cfg;

	اگر (ثنائی_ہندسہ_2 >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (ثنائی_ہندسہ_2 - 32));
	ورنہ
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << ثنائی_ہندسہ_2);

	*قدر = cfg != 0;

	واپس SBI_OK;
}

ساکن صحیح fwft_misaligned_delegation_supported(ساخت fwft_config *conf)
{
	اگر (!misa_extension('S'))
		واپس SBI_ENOTSUPP;

	واپس SBI_OK;
}

ساکن صحیح fwft_سیٹ_misaligned_delegation(ساخت fwft_config *conf,
					 غیرعلامتی طویل قدر)
{
	اگر (قدر == 1)
		csr_سیٹ(CSR_MEDELEG, MIS_DELEG);
	ورنہ اگر (قدر == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	ورنہ
		واپس SBI_EINVAL;

	واپس SBI_OK;
}

ساکن صحیح fwft_get_misaligned_delegation(ساخت fwft_config *conf,
					 غیرعلامتی طویل *قدر)
{
	*قدر = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	واپس SBI_OK;
}

ساکن صحیح fwft_double_trap_supported(ساخت fwft_config *conf)
{
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		واپس SBI_ENOTSUPP;

	واپس SBI_OK;
}

ساکن صحیح fwft_سیٹ_double_trap(ساخت fwft_config *conf, غیرعلامتی طویل قدر)
{
	واپس fwft_menvcfg_سیٹ_ثنائی_ہندسہ(قدر, ENVCFG_DTE_سرکاؤ);
}

ساکن صحیح fwft_get_double_trap(ساخت fwft_config *conf, غیرعلامتی طویل *قدر)
{
	واپس fwft_menvcfg_read_ثنائی_ہندسہ(قدر, ENVCFG_DTE_سرکاؤ);
}

ساکن صحیح fwft_adue_supported(ساخت fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		واپس SBI_ENOTSUPP;

	واپس SBI_OK;
}

ساکن صحیح fwft_سیٹ_adue(ساخت fwft_config *conf, غیرعلامتی طویل قدر)
{
	صحیح res = fwft_menvcfg_سیٹ_ثنائی_ہندسہ(قدر, ENVCFG_ADUE_سرکاؤ);

	اگر (res == SBI_OK) {
		__sbi_sfence_vma_all();

		اگر (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	واپس res;
}

ساکن صحیح fwft_get_adue(ساخت fwft_config *conf, غیرعلامتی طویل *قدر)
{
	واپس fwft_menvcfg_read_ثنائی_ہندسہ(قدر, ENVCFG_ADUE_سرکاؤ);
}

ساکن صحیح fwft_lpad_supported(ساخت fwft_config *conf)
{
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		واپس SBI_ENOTSUPP;

	واپس SBI_OK;
}

ساکن صحیح fwft_enable_lpad(ساخت fwft_config *conf, غیرعلامتی طویل قدر)
{
	واپس fwft_menvcfg_سیٹ_ثنائی_ہندسہ(قدر, ENVCFG_LPE_سرکاؤ);
}

ساکن صحیح fwft_get_lpad(ساخت fwft_config *conf, غیرعلامتی طویل *قدر)
{
	واپس fwft_menvcfg_read_ثنائی_ہندسہ(قدر, ENVCFG_LPE_سرکاؤ);
}

ساکن صحیح fwft_sstack_supported(ساخت fwft_config *conf)
{
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		واپس SBI_ENOTSUPP;

	واپس SBI_OK;
}

ساکن صحیح fwft_enable_sstack(ساخت fwft_config *conf, غیرعلامتی طویل قدر)
{
	واپس fwft_menvcfg_سیٹ_ثنائی_ہندسہ(قدر, ENVCFG_SSE_سرکاؤ);
}

ساکن صحیح fwft_get_sstack(ساخت fwft_config *conf, غیرعلامتی طویل *قدر)
{
	واپس fwft_menvcfg_read_ثنائی_ہندسہ(قدر, ENVCFG_SSE_سرکاؤ);
}

#اگر __riscv_xlen > 32
ساکن صحیح fwft_pmlen_supported(ساخت fwft_config *conf)
{
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		واپس SBI_ENOTSUPP;

	واپس SBI_OK;
}

ساکن صحیح fwft_سیٹ_pmlen(ساخت fwft_config *conf, غیرعلامتی طویل قدر)
{
	غیرعلامتی طویل pmm, پچھلا;

	انتخاب (قدر) {
	حالت 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		توڑو;
	حالت 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		توڑو;
	حالت 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		توڑو;
	طےشدہ:
		واپس SBI_EINVAL;
	}

	پچھلا = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_سیٹ(CSR_MENVCFG, pmm);
	اگر ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, پچھلا);
		واپس SBI_EINVAL;
	}

	واپس SBI_OK;
}

ساکن صحیح fwft_get_pmlen(ساخت fwft_config *conf, غیرعلامتی طویل *قدر)
{
	انتخاب (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	حالت ENVCFG_PMM_PMLEN_0:
		*قدر = 0;
		توڑو;
	حالت ENVCFG_PMM_PMLEN_7:
		*قدر = 7;
		توڑو;
	حالت ENVCFG_PMM_PMLEN_16:
		*قدر = 16;
		توڑو;
	طےشدہ:
		واپس SBI_EFAIL;
	}

	واپس SBI_OK;
}
#اگر_ختم

ساکن ساخت fwft_config* get_feature_config(شمار sbi_fwft_feature_t feature)
{
	صحیح i;
	ساخت fwft_hart_state *fhs = fwft_thishart_state_ptr();

	اگر (feature & SBI_FWFT_GLOBAL_FEATURE_ثنائی_ہندسہ)
		واپس تہی;

	برائے (i = 0; i < fhs->config_گنتی; i++){
		اگر (feature == fhs->configs[i].feature->id)
			واپس &fhs->configs[i];
	}

	واپس تہی;
}

ساکن صحیح fwft_get_feature(شمار sbi_fwft_feature_t feature,
			    ساخت fwft_config **conf)
{
	صحیح ret;
	ساخت fwft_config *tconf;

	tconf = get_feature_config(feature);
	اگر (!tconf) {
		اگر (fwft_is_defined_feature(feature))
			واپس SBI_ENOTSUPP;

		واپس SBI_EDENIED;
	}

	اگر (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		اگر (ret)
			واپس ret;
	}
	*conf = tconf;

	واپس SBI_SUCCESS;
}

ساکن خالی fwft_clear_config_تالا(شمار sbi_fwft_feature_t feature)
{
	صحیح ret;
	ساخت fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	اگر (ret)
		واپس;

	conf->flags &= ~SBI_FWFT_سیٹ_FLAG_تالا;
}

صحیح sbi_fwft_سیٹ(شمار sbi_fwft_feature_t feature, غیرعلامتی طویل قدر,
		 غیرعلامتی طویل flags)
{
	صحیح ret;
	ساخت fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	اگر (ret)
		واپس ret;

	اگر ((flags & ~SBI_FWFT_سیٹ_FLAG_تالا) != 0)
		واپس SBI_EINVAL;

	اگر (conf->flags & SBI_FWFT_سیٹ_FLAG_تالا)
		واپس SBI_EDENIED_LOCKED;

	ret = conf->feature->سیٹ(conf, قدر);
	اگر (ret)
		واپس ret;

	conf->flags = flags;

	واپس SBI_OK;
}

صحیح sbi_fwft_get(شمار sbi_fwft_feature_t feature, غیرعلامتی طویل *برآمد_val)
{
	صحیح ret;
	ساخت fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	اگر (ret)
		واپس ret;

	واپس conf->feature->get(conf, برآمد_val);
}

ساکن ثابت ساخت fwft_feature features[] =
{
	{
		.id = SBI_FWFT_MISALIGNED_EXC_DELEG,
		.supported = fwft_misaligned_delegation_supported,
		.سیٹ = fwft_سیٹ_misaligned_delegation,
		.get = fwft_get_misaligned_delegation,
	},
	{
		.id = SBI_FWFT_LANDING_PAD,
		.supported = fwft_lpad_supported,
		.سیٹ = fwft_enable_lpad,
		.get = fwft_get_lpad,
	},
	{
		.id = SBI_FWFT_SHADOW_ڈھیر,
		.supported = fwft_sstack_supported,
		.سیٹ = fwft_enable_sstack,
		.get = fwft_get_sstack,
	},
	{
		.id = SBI_FWFT_DOUBLE_TRAP,
		.supported = fwft_double_trap_supported,
		.سیٹ = fwft_سیٹ_double_trap,
		.get = fwft_get_double_trap,
	},
	{
		.id = SBI_FWFT_PTE_AD_HW_UPDATING,
		.supported = fwft_adue_supported,
		.سیٹ = fwft_سیٹ_adue,
		.get = fwft_get_adue,
	},
#اگر __riscv_xlen > 32
	{
		.id = SBI_FWFT_اشارہ_گر_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.سیٹ = fwft_سیٹ_pmlen,
		.get = fwft_get_pmlen,
	},
#اگر_ختم
};

صحیح sbi_fwft_init(ساخت sbi_scratch *scratch, bool cold_boot)
{
	صحیح i;
	ساخت fwft_hart_state *fhs;

	اگر (cold_boot) {
		fwft_ptr_انحراف = sbi_scratch_alloc_type_انحراف(خالی *);
		اگر (!fwft_ptr_انحراف)
			واپس SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	اگر (!fhs) {
		fhs = sbi_zalloc(حجم(*fhs) + array_سائز(features) * حجم(ساخت fwft_config));
		اگر (!fhs)
			واپس SBI_ENOMEM;

		fhs->config_گنتی = array_سائز(features);
		برائے (i = 0; i < array_سائز(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_سیٹ_hart_state_ptr(scratch, fhs);
	}

	برائے (i = 0; i < array_سائز(features); i++)
		fwft_clear_config_تالا(features[i].id);

	واپس 0;
}
