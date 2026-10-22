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
स्थिर अचिन्हित दीर्घ fwft_ptr_offset;

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
	पूर्णाङ्क (*set)(संरचना fwft_config *conf, अचिन्हित दीर्घ मान);
	पूर्णाङ्क (*get)(संरचना fwft_config *conf, अचिन्हित दीर्घ *मान);
};

संरचना fwft_config {
	अचर संरचना fwft_feature *feature;
	अचिन्हित दीर्घ flags;
};

संरचना fwft_hart_state {
	अचिन्हित पूर्णाङ्क config_गन्ती;
	संरचना fwft_config configs[];
};

स्थिर अचर अचिन्हित दीर्घ fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_थाक,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

स्थिर bool fwft_is_defined_feature(गणना sbi_fwft_feature_t feature)
{
	पूर्णाङ्क i;

	लागि (i = 0; i < array_size(fwft_defined_features); i++) {
		यदि (fwft_defined_features[i] == feature)
			फर्काऊ true;
	}

	फर्काऊ false;
}

स्थिर पूर्णाङ्क fwft_menvcfg_set_bit(अचिन्हित दीर्घ मान, अचिन्हित दीर्घ bit)
{
	यदि (मान == 1) {
		यदि (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		अन्यथा
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} अन्यथा यदि (मान == 0) {
		यदि (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		अन्यथा
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} अन्यथा {
		फर्काऊ SBI_EINVAL;
	}

	फर्काऊ SBI_OK;
}

स्थिर पूर्णाङ्क fwft_menvcfg_read_bit(अचिन्हित दीर्घ *मान, अचिन्हित दीर्घ bit)
{
	अचिन्हित दीर्घ cfg;

	यदि (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	अन्यथा
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*मान = cfg != 0;

	फर्काऊ SBI_OK;
}

स्थिर पूर्णाङ्क fwft_misaligned_delegation_supported(संरचना fwft_config *conf)
{
	यदि (!misa_extension('S'))
		फर्काऊ SBI_ENOTSUPP;

	फर्काऊ SBI_OK;
}

स्थिर पूर्णाङ्क fwft_set_misaligned_delegation(संरचना fwft_config *conf,
					 अचिन्हित दीर्घ मान)
{
	यदि (मान == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	अन्यथा यदि (मान == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	अन्यथा
		फर्काऊ SBI_EINVAL;

	फर्काऊ SBI_OK;
}

स्थिर पूर्णाङ्क fwft_get_misaligned_delegation(संरचना fwft_config *conf,
					 अचिन्हित दीर्घ *मान)
{
	*मान = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	फर्काऊ SBI_OK;
}

स्थिर पूर्णाङ्क fwft_double_trap_supported(संरचना fwft_config *conf)
{
	यदि (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		फर्काऊ SBI_ENOTSUPP;

	फर्काऊ SBI_OK;
}

स्थिर पूर्णाङ्क fwft_set_double_trap(संरचना fwft_config *conf, अचिन्हित दीर्घ मान)
{
	फर्काऊ fwft_menvcfg_set_bit(मान, ENVCFG_DTE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_get_double_trap(संरचना fwft_config *conf, अचिन्हित दीर्घ *मान)
{
	फर्काऊ fwft_menvcfg_read_bit(मान, ENVCFG_DTE_SHIFT);
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
		फर्काऊ SBI_ENOTSUPP;

	फर्काऊ SBI_OK;
}

स्थिर पूर्णाङ्क fwft_set_adue(संरचना fwft_config *conf, अचिन्हित दीर्घ मान)
{
	पूर्णाङ्क res = fwft_menvcfg_set_bit(मान, ENVCFG_ADUE_SHIFT);

	यदि (res == SBI_OK) {
		__sbi_sfence_vma_all();

		यदि (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	फर्काऊ res;
}

स्थिर पूर्णाङ्क fwft_get_adue(संरचना fwft_config *conf, अचिन्हित दीर्घ *मान)
{
	फर्काऊ fwft_menvcfg_read_bit(मान, ENVCFG_ADUE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_lpad_supported(संरचना fwft_config *conf)
{
	यदि (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		फर्काऊ SBI_ENOTSUPP;

	फर्काऊ SBI_OK;
}

स्थिर पूर्णाङ्क fwft_enable_lpad(संरचना fwft_config *conf, अचिन्हित दीर्घ मान)
{
	फर्काऊ fwft_menvcfg_set_bit(मान, ENVCFG_LPE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_get_lpad(संरचना fwft_config *conf, अचिन्हित दीर्घ *मान)
{
	फर्काऊ fwft_menvcfg_read_bit(मान, ENVCFG_LPE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_sstack_supported(संरचना fwft_config *conf)
{
	यदि (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		फर्काऊ SBI_ENOTSUPP;

	फर्काऊ SBI_OK;
}

स्थिर पूर्णाङ्क fwft_enable_sstack(संरचना fwft_config *conf, अचिन्हित दीर्घ मान)
{
	फर्काऊ fwft_menvcfg_set_bit(मान, ENVCFG_SSE_SHIFT);
}

स्थिर पूर्णाङ्क fwft_get_sstack(संरचना fwft_config *conf, अचिन्हित दीर्घ *मान)
{
	फर्काऊ fwft_menvcfg_read_bit(मान, ENVCFG_SSE_SHIFT);
}

#यदि __riscv_xlen > 32
स्थिर पूर्णाङ्क fwft_pmlen_supported(संरचना fwft_config *conf)
{
	यदि (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		फर्काऊ SBI_ENOTSUPP;

	फर्काऊ SBI_OK;
}

स्थिर पूर्णाङ्क fwft_set_pmlen(संरचना fwft_config *conf, अचिन्हित दीर्घ मान)
{
	अचिन्हित दीर्घ pmm, prev;

	विकल्प (मान) {
	पक्ष 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		रोक;
	पक्ष 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		रोक;
	पक्ष 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		रोक;
	स्वभाव:
		फर्काऊ SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	यदि ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		फर्काऊ SBI_EINVAL;
	}

	फर्काऊ SBI_OK;
}

स्थिर पूर्णाङ्क fwft_get_pmlen(संरचना fwft_config *conf, अचिन्हित दीर्घ *मान)
{
	विकल्प (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	पक्ष ENVCFG_PMM_PMLEN_0:
		*मान = 0;
		रोक;
	पक्ष ENVCFG_PMM_PMLEN_7:
		*मान = 7;
		रोक;
	पक्ष ENVCFG_PMM_PMLEN_16:
		*मान = 16;
		रोक;
	स्वभाव:
		फर्काऊ SBI_EFAIL;
	}

	फर्काऊ SBI_OK;
}
#endif

स्थिर संरचना fwft_config* get_feature_config(गणना sbi_fwft_feature_t feature)
{
	पूर्णाङ्क i;
	संरचना fwft_hart_state *fhs = fwft_thishart_state_ptr();

	यदि (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		फर्काऊ रिक्त;

	लागि (i = 0; i < fhs->config_गन्ती; i++){
		यदि (feature == fhs->configs[i].feature->id)
			फर्काऊ &fhs->configs[i];
	}

	फर्काऊ रिक्त;
}

स्थिर पूर्णाङ्क fwft_get_feature(गणना sbi_fwft_feature_t feature,
			    संरचना fwft_config **conf)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *tconf;

	tconf = get_feature_config(feature);
	यदि (!tconf) {
		यदि (fwft_is_defined_feature(feature))
			फर्काऊ SBI_ENOTSUPP;

		फर्काऊ SBI_EDENIED;
	}

	यदि (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		यदि (ret)
			फर्काऊ ret;
	}
	*conf = tconf;

	फर्काऊ SBI_SUCCESS;
}

स्थिर शून्य fwft_clear_config_lock(गणना sbi_fwft_feature_t feature)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	यदि (ret)
		फर्काऊ;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

पूर्णाङ्क sbi_fwft_set(गणना sbi_fwft_feature_t feature, अचिन्हित दीर्घ मान,
		 अचिन्हित दीर्घ flags)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	यदि (ret)
		फर्काऊ ret;

	यदि ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		फर्काऊ SBI_EINVAL;

	यदि (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		फर्काऊ SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, मान);
	यदि (ret)
		फर्काऊ ret;

	conf->flags = flags;

	फर्काऊ SBI_OK;
}

पूर्णाङ्क sbi_fwft_get(गणना sbi_fwft_feature_t feature, अचिन्हित दीर्घ *out_val)
{
	पूर्णाङ्क ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	यदि (ret)
		फर्काऊ ret;

	फर्काऊ conf->feature->get(conf, out_val);
}

स्थिर अचर संरचना fwft_feature features[] =
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
		.id = SBI_FWFT_SHADOW_थाक,
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
#यदि __riscv_xlen > 32
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

	यदि (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(शून्य *);
		यदि (!fwft_ptr_offset)
			फर्काऊ SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	यदि (!fhs) {
		fhs = sbi_zalloc(परिमाण(*fhs) + array_size(features) * परिमाण(संरचना fwft_config));
		यदि (!fhs)
			फर्काऊ SBI_ENOMEM;

		fhs->config_गन्ती = array_size(features);
		लागि (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	लागि (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	फर्काऊ 0;
}
