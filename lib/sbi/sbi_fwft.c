/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_bitmap.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_hfence.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_tlb.h>
#ضمن <sbi/sbi_types.h>

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
ساكن بنانشانی لما fwft_ptr_وتھ;

#تعریف_کرو fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, fwft_ptr_وتھ)

#تعریف_کرو fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#تعریف_کرو fwft_سیٹ_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), فراغ *, fwft_ptr_وتھ, (__phs))

#تعریف_کرو MIS_DELEG (1UL << CAUSE_MISALIGNED_لدو | 1UL << CAUSE_MISALIGNED_STORE)

بنية fwft_config;

بنية fwft_feature {
	تعداد sbi_fwft_feature_t id;
	صحيح (*supported)(بنية fwft_config *conf);
	صحيح (*سیٹ)(بنية fwft_config *conf, بنانشانی لما قدر);
	صحيح (*get)(بنية fwft_config *conf, بنانشانی لما *قدر);
};

بنية fwft_config {
	ثابت بنية fwft_feature *feature;
	بنانشانی لما flags;
};

بنية fwft_hart_state {
	بنانشانی صحيح config_گنتی;
	بنية fwft_config configs[];
};

ساكن ثابت بنانشانی لما fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_ڈھیر,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_اشارہ_گر_MASKING_PMLEN,
};

ساكن bool fwft_is_defined_feature(تعداد sbi_fwft_feature_t feature)
{
	صحيح i;

	لكل (i = 0; i < array_ناپ(fwft_defined_features); i++) {
		إذا (fwft_defined_features[i] == feature)
			أرجع true;
	}

	أرجع false;
}

ساكن صحيح fwft_menvcfg_سیٹ_ثنائی_ہندسہ(بنانشانی لما قدر, بنانشانی لما ثنائی_ہندسہ_2)
{
	إذا (قدر == 1) {
		إذا (ثنائی_ہندسہ_2 >= 32 && __riscv_xlen == 32)
			csr_سیٹ(CSR_MENVCFGH, _ULL(1) << (ثنائی_ہندسہ_2 - 32));
		وإلا
			csr_سیٹ(CSR_MENVCFG, _ULL(1) << ثنائی_ہندسہ_2);

	} وإلا إذا (قدر == 0) {
		إذا (ثنائی_ہندسہ_2 >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (ثنائی_ہندسہ_2 - 32));
		وإلا
			csr_clear(CSR_MENVCFG, _ULL(1) << ثنائی_ہندسہ_2);
	} وإلا {
		أرجع SBI_EINVAL;
	}

	أرجع SBI_OK;
}

ساكن صحيح fwft_menvcfg_read_ثنائی_ہندسہ(بنانشانی لما *قدر, بنانشانی لما ثنائی_ہندسہ_2)
{
	بنانشانی لما cfg;

	إذا (ثنائی_ہندسہ_2 >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (ثنائی_ہندسہ_2 - 32));
	وإلا
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << ثنائی_ہندسہ_2);

	*قدر = cfg != 0;

	أرجع SBI_OK;
}

ساكن صحيح fwft_misaligned_delegation_supported(بنية fwft_config *conf)
{
	إذا (!misa_extension('S'))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_سیٹ_misaligned_delegation(بنية fwft_config *conf,
					 بنانشانی لما قدر)
{
	إذا (قدر == 1)
		csr_سیٹ(CSR_MEDELEG, MIS_DELEG);
	وإلا إذا (قدر == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	وإلا
		أرجع SBI_EINVAL;

	أرجع SBI_OK;
}

ساكن صحيح fwft_get_misaligned_delegation(بنية fwft_config *conf,
					 بنانشانی لما *قدر)
{
	*قدر = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	أرجع SBI_OK;
}

ساكن صحيح fwft_double_trap_supported(بنية fwft_config *conf)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_سیٹ_double_trap(بنية fwft_config *conf, بنانشانی لما قدر)
{
	أرجع fwft_menvcfg_سیٹ_ثنائی_ہندسہ(قدر, ENVCFG_DTE_کھسکاؤ);
}

ساكن صحيح fwft_get_double_trap(بنية fwft_config *conf, بنانشانی لما *قدر)
{
	أرجع fwft_menvcfg_read_ثنائی_ہندسہ(قدر, ENVCFG_DTE_کھسکاؤ);
}

ساكن صحيح fwft_adue_supported(بنية fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_سیٹ_adue(بنية fwft_config *conf, بنانشانی لما قدر)
{
	صحيح res = fwft_menvcfg_سیٹ_ثنائی_ہندسہ(قدر, ENVCFG_ADUE_کھسکاؤ);

	إذا (res == SBI_OK) {
		__sbi_sfence_vma_all();

		إذا (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	أرجع res;
}

ساكن صحيح fwft_get_adue(بنية fwft_config *conf, بنانشانی لما *قدر)
{
	أرجع fwft_menvcfg_read_ثنائی_ہندسہ(قدر, ENVCFG_ADUE_کھسکاؤ);
}

ساكن صحيح fwft_lpad_supported(بنية fwft_config *conf)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_enable_lpad(بنية fwft_config *conf, بنانشانی لما قدر)
{
	أرجع fwft_menvcfg_سیٹ_ثنائی_ہندسہ(قدر, ENVCFG_LPE_کھسکاؤ);
}

ساكن صحيح fwft_get_lpad(بنية fwft_config *conf, بنانشانی لما *قدر)
{
	أرجع fwft_menvcfg_read_ثنائی_ہندسہ(قدر, ENVCFG_LPE_کھسکاؤ);
}

ساكن صحيح fwft_sstack_supported(بنية fwft_config *conf)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_enable_sstack(بنية fwft_config *conf, بنانشانی لما قدر)
{
	أرجع fwft_menvcfg_سیٹ_ثنائی_ہندسہ(قدر, ENVCFG_SSE_کھسکاؤ);
}

ساكن صحيح fwft_get_sstack(بنية fwft_config *conf, بنانشانی لما *قدر)
{
	أرجع fwft_menvcfg_read_ثنائی_ہندسہ(قدر, ENVCFG_SSE_کھسکاؤ);
}

#إذا __riscv_xlen > 32
ساكن صحيح fwft_pmlen_supported(بنية fwft_config *conf)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_سیٹ_pmlen(بنية fwft_config *conf, بنانشانی لما قدر)
{
	بنانشانی لما pmm, prev;

	بدل (قدر) {
	حالة 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		اكسر;
	حالة 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		اكسر;
	حالة 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		اكسر;
	افتراضي:
		أرجع SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_سیٹ(CSR_MENVCFG, pmm);
	إذا ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		أرجع SBI_EINVAL;
	}

	أرجع SBI_OK;
}

ساكن صحيح fwft_get_pmlen(بنية fwft_config *conf, بنانشانی لما *قدر)
{
	بدل (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	حالة ENVCFG_PMM_PMLEN_0:
		*قدر = 0;
		اكسر;
	حالة ENVCFG_PMM_PMLEN_7:
		*قدر = 7;
		اكسر;
	حالة ENVCFG_PMM_PMLEN_16:
		*قدر = 16;
		اكسر;
	افتراضي:
		أرجع SBI_EFAIL;
	}

	أرجع SBI_OK;
}
#جے_انت

ساكن بنية fwft_config* get_feature_config(تعداد sbi_fwft_feature_t feature)
{
	صحيح i;
	بنية fwft_hart_state *fhs = fwft_thishart_state_ptr();

	إذا (feature & SBI_FWFT_GLOBAL_FEATURE_ثنائی_ہندسہ)
		أرجع عدم;

	لكل (i = 0; i < fhs->config_گنتی; i++){
		إذا (feature == fhs->configs[i].feature->id)
			أرجع &fhs->configs[i];
	}

	أرجع عدم;
}

ساكن صحيح fwft_get_feature(تعداد sbi_fwft_feature_t feature,
			    بنية fwft_config **conf)
{
	صحيح ret;
	بنية fwft_config *tconf;

	tconf = get_feature_config(feature);
	إذا (!tconf) {
		إذا (fwft_is_defined_feature(feature))
			أرجع SBI_ENOTSUPP;

		أرجع SBI_EDENIED;
	}

	إذا (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		إذا (ret)
			أرجع ret;
	}
	*conf = tconf;

	أرجع SBI_SUCCESS;
}

ساكن فراغ fwft_clear_config_جندرا(تعداد sbi_fwft_feature_t feature)
{
	صحيح ret;
	بنية fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	إذا (ret)
		أرجع;

	conf->flags &= ~SBI_FWFT_سیٹ_FLAG_جندرا;
}

صحيح sbi_fwft_سیٹ(تعداد sbi_fwft_feature_t feature, بنانشانی لما قدر,
		 بنانشانی لما flags)
{
	صحيح ret;
	بنية fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	إذا (ret)
		أرجع ret;

	إذا ((flags & ~SBI_FWFT_سیٹ_FLAG_جندرا) != 0)
		أرجع SBI_EINVAL;

	إذا (conf->flags & SBI_FWFT_سیٹ_FLAG_جندرا)
		أرجع SBI_EDENIED_LOCKED;

	ret = conf->feature->سیٹ(conf, قدر);
	إذا (ret)
		أرجع ret;

	conf->flags = flags;

	أرجع SBI_OK;
}

صحيح sbi_fwft_get(تعداد sbi_fwft_feature_t feature, بنانشانی لما *باہرلا_val)
{
	صحيح ret;
	بنية fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	إذا (ret)
		أرجع ret;

	أرجع conf->feature->get(conf, باہرلا_val);
}

ساكن ثابت بنية fwft_feature features[] =
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
#إذا __riscv_xlen > 32
	{
		.id = SBI_FWFT_اشارہ_گر_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.سیٹ = fwft_سیٹ_pmlen,
		.get = fwft_get_pmlen,
	},
#جے_انت
};

صحيح sbi_fwft_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	صحيح i;
	بنية fwft_hart_state *fhs;

	إذا (cold_boot) {
		fwft_ptr_وتھ = sbi_scratch_alloc_type_وتھ(فراغ *);
		إذا (!fwft_ptr_وتھ)
			أرجع SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	إذا (!fhs) {
		fhs = sbi_zalloc(حجم(*fhs) + array_ناپ(features) * حجم(بنية fwft_config));
		إذا (!fhs)
			أرجع SBI_ENOMEM;

		fhs->config_گنتی = array_ناپ(features);
		لكل (i = 0; i < array_ناپ(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_سیٹ_hart_state_ptr(scratch, fhs);
	}

	لكل (i = 0; i < array_ناپ(features); i++)
		fwft_clear_config_جندرا(features[i].id);

	أرجع 0;
}
