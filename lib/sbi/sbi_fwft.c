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
ساكن بینښه اوږد fwft_ptr_دننه_واټن;

#تعریف_کړه fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, fwft_ptr_دننه_واټن)

#تعریف_کړه fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#تعریف_کړه fwft_سېټ_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), فراغ *, fwft_ptr_دننه_واټن, (__phs))

#تعریف_کړه MIS_DELEG (1UL << CAUSE_MISALIGNED_بارول | 1UL << CAUSE_MISALIGNED_STORE)

بنية fwft_config;

بنية fwft_feature {
	تعداد sbi_fwft_feature_t id;
	صحيح (*supported)(بنية fwft_config *conf);
	صحيح (*سېټ)(بنية fwft_config *conf, بینښه اوږد ارزښت);
	صحيح (*get)(بنية fwft_config *conf, بینښه اوږد *ارزښت);
};

بنية fwft_config {
	ثابت بنية fwft_feature *feature;
	بینښه اوږد flags;
};

بنية fwft_hart_state {
	بینښه صحيح config_شمېر;
	بنية fwft_config configs[];
};

ساكن ثابت بینښه اوږد fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_ډېری,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_اشاره_ګر_MASKING_PMLEN,
};

ساكن bool fwft_is_defined_feature(تعداد sbi_fwft_feature_t feature)
{
	صحيح i;

	لكل (i = 0; i < array_اندازه(fwft_defined_features); i++) {
		إذا (fwft_defined_features[i] == feature)
			أرجع true;
	}

	أرجع false;
}

ساكن صحيح fwft_menvcfg_سېټ_بټ(بینښه اوږد ارزښت, بینښه اوږد بټ_2)
{
	إذا (ارزښت == 1) {
		إذا (بټ_2 >= 32 && __riscv_xlen == 32)
			csr_سېټ(CSR_MENVCFGH, _ULL(1) << (بټ_2 - 32));
		وإلا
			csr_سېټ(CSR_MENVCFG, _ULL(1) << بټ_2);

	} وإلا إذا (ارزښت == 0) {
		إذا (بټ_2 >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (بټ_2 - 32));
		وإلا
			csr_clear(CSR_MENVCFG, _ULL(1) << بټ_2);
	} وإلا {
		أرجع SBI_EINVAL;
	}

	أرجع SBI_OK;
}

ساكن صحيح fwft_menvcfg_read_بټ(بینښه اوږد *ارزښت, بینښه اوږد بټ_2)
{
	بینښه اوږد cfg;

	إذا (بټ_2 >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (بټ_2 - 32));
	وإلا
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << بټ_2);

	*ارزښت = cfg != 0;

	أرجع SBI_OK;
}

ساكن صحيح fwft_misaligned_delegation_supported(بنية fwft_config *conf)
{
	إذا (!misa_extension('S'))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_سېټ_misaligned_delegation(بنية fwft_config *conf,
					 بینښه اوږد ارزښت)
{
	إذا (ارزښت == 1)
		csr_سېټ(CSR_MEDELEG, MIS_DELEG);
	وإلا إذا (ارزښت == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	وإلا
		أرجع SBI_EINVAL;

	أرجع SBI_OK;
}

ساكن صحيح fwft_get_misaligned_delegation(بنية fwft_config *conf,
					 بینښه اوږد *ارزښت)
{
	*ارزښت = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	أرجع SBI_OK;
}

ساكن صحيح fwft_double_trap_supported(بنية fwft_config *conf)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_سېټ_double_trap(بنية fwft_config *conf, بینښه اوږد ارزښت)
{
	أرجع fwft_menvcfg_سېټ_بټ(ارزښت, ENVCFG_DTE_ښويدنه);
}

ساكن صحيح fwft_get_double_trap(بنية fwft_config *conf, بینښه اوږد *ارزښت)
{
	أرجع fwft_menvcfg_read_بټ(ارزښت, ENVCFG_DTE_ښويدنه);
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

ساكن صحيح fwft_سېټ_adue(بنية fwft_config *conf, بینښه اوږد ارزښت)
{
	صحيح res = fwft_menvcfg_سېټ_بټ(ارزښت, ENVCFG_ADUE_ښويدنه);

	إذا (res == SBI_OK) {
		__sbi_sfence_vma_all();

		إذا (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	أرجع res;
}

ساكن صحيح fwft_get_adue(بنية fwft_config *conf, بینښه اوږد *ارزښت)
{
	أرجع fwft_menvcfg_read_بټ(ارزښت, ENVCFG_ADUE_ښويدنه);
}

ساكن صحيح fwft_lpad_supported(بنية fwft_config *conf)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_enable_lpad(بنية fwft_config *conf, بینښه اوږد ارزښت)
{
	أرجع fwft_menvcfg_سېټ_بټ(ارزښت, ENVCFG_LPE_ښويدنه);
}

ساكن صحيح fwft_get_lpad(بنية fwft_config *conf, بینښه اوږد *ارزښت)
{
	أرجع fwft_menvcfg_read_بټ(ارزښت, ENVCFG_LPE_ښويدنه);
}

ساكن صحيح fwft_sstack_supported(بنية fwft_config *conf)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_enable_sstack(بنية fwft_config *conf, بینښه اوږد ارزښت)
{
	أرجع fwft_menvcfg_سېټ_بټ(ارزښت, ENVCFG_SSE_ښويدنه);
}

ساكن صحيح fwft_get_sstack(بنية fwft_config *conf, بینښه اوږد *ارزښت)
{
	أرجع fwft_menvcfg_read_بټ(ارزښت, ENVCFG_SSE_ښويدنه);
}

#إذا __riscv_xlen > 32
ساكن صحيح fwft_pmlen_supported(بنية fwft_config *conf)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_سېټ_pmlen(بنية fwft_config *conf, بینښه اوږد ارزښت)
{
	بینښه اوږد pmm, prev;

	بدل (ارزښت) {
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
	csr_سېټ(CSR_MENVCFG, pmm);
	إذا ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		أرجع SBI_EINVAL;
	}

	أرجع SBI_OK;
}

ساكن صحيح fwft_get_pmlen(بنية fwft_config *conf, بینښه اوږد *ارزښت)
{
	بدل (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	حالة ENVCFG_PMM_PMLEN_0:
		*ارزښت = 0;
		اكسر;
	حالة ENVCFG_PMM_PMLEN_7:
		*ارزښت = 7;
		اكسر;
	حالة ENVCFG_PMM_PMLEN_16:
		*ارزښت = 16;
		اكسر;
	افتراضي:
		أرجع SBI_EFAIL;
	}

	أرجع SBI_OK;
}
#د_که_پای

ساكن بنية fwft_config* get_feature_config(تعداد sbi_fwft_feature_t feature)
{
	صحيح i;
	بنية fwft_hart_state *fhs = fwft_thishart_state_ptr();

	إذا (feature & SBI_FWFT_GLOBAL_FEATURE_بټ)
		أرجع عدم;

	لكل (i = 0; i < fhs->config_شمېر; i++){
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

ساكن فراغ fwft_clear_config_قلف(تعداد sbi_fwft_feature_t feature)
{
	صحيح ret;
	بنية fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	إذا (ret)
		أرجع;

	conf->flags &= ~SBI_FWFT_سېټ_FLAG_قلف;
}

صحيح sbi_fwft_سېټ(تعداد sbi_fwft_feature_t feature, بینښه اوږد ارزښت,
		 بینښه اوږد flags)
{
	صحيح ret;
	بنية fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	إذا (ret)
		أرجع ret;

	إذا ((flags & ~SBI_FWFT_سېټ_FLAG_قلف) != 0)
		أرجع SBI_EINVAL;

	إذا (conf->flags & SBI_FWFT_سېټ_FLAG_قلف)
		أرجع SBI_EDENIED_LOCKED;

	ret = conf->feature->سېټ(conf, ارزښت);
	إذا (ret)
		أرجع ret;

	conf->flags = flags;

	أرجع SBI_OK;
}

صحيح sbi_fwft_get(تعداد sbi_fwft_feature_t feature, بینښه اوږد *بهر_val)
{
	صحيح ret;
	بنية fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	إذا (ret)
		أرجع ret;

	أرجع conf->feature->get(conf, بهر_val);
}

ساكن ثابت بنية fwft_feature features[] =
{
	{
		.id = SBI_FWFT_MISALIGNED_EXC_DELEG,
		.supported = fwft_misaligned_delegation_supported,
		.سېټ = fwft_سېټ_misaligned_delegation,
		.get = fwft_get_misaligned_delegation,
	},
	{
		.id = SBI_FWFT_LANDING_PAD,
		.supported = fwft_lpad_supported,
		.سېټ = fwft_enable_lpad,
		.get = fwft_get_lpad,
	},
	{
		.id = SBI_FWFT_SHADOW_ډېری,
		.supported = fwft_sstack_supported,
		.سېټ = fwft_enable_sstack,
		.get = fwft_get_sstack,
	},
	{
		.id = SBI_FWFT_DOUBLE_TRAP,
		.supported = fwft_double_trap_supported,
		.سېټ = fwft_سېټ_double_trap,
		.get = fwft_get_double_trap,
	},
	{
		.id = SBI_FWFT_PTE_AD_HW_UPDATING,
		.supported = fwft_adue_supported,
		.سېټ = fwft_سېټ_adue,
		.get = fwft_get_adue,
	},
#إذا __riscv_xlen > 32
	{
		.id = SBI_FWFT_اشاره_ګر_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.سېټ = fwft_سېټ_pmlen,
		.get = fwft_get_pmlen,
	},
#د_که_پای
};

صحيح sbi_fwft_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	صحيح i;
	بنية fwft_hart_state *fhs;

	إذا (cold_boot) {
		fwft_ptr_دننه_واټن = sbi_scratch_alloc_type_دننه_واټن(فراغ *);
		إذا (!fwft_ptr_دننه_واټن)
			أرجع SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	إذا (!fhs) {
		fhs = sbi_zalloc(حجم(*fhs) + array_اندازه(features) * حجم(بنية fwft_config));
		إذا (!fhs)
			أرجع SBI_ENOMEM;

		fhs->config_شمېر = array_اندازه(features);
		لكل (i = 0; i < array_اندازه(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_سېټ_hart_state_ptr(scratch, fhs);
	}

	لكل (i = 0; i < array_اندازه(features); i++)
		fwft_clear_config_قلف(features[i].id);

	أرجع 0;
}
