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
	पूर्णांक (*supported)(संरचना fwft_config *conf);
	पूर्णांक (*set)(संरचना fwft_config *conf, अचिन्हित दीर्घ मूल्य);
	पूर्णांक (*get)(संरचना fwft_config *conf, अचिन्हित दीर्घ *मूल्य);
};

संरचना fwft_config {
	स्थिरांक संरचना fwft_feature *feature;
	अचिन्हित दीर्घ flags;
};

संरचना fwft_hart_state {
	अचिन्हित पूर्णांक config_मोजणी;
	संरचना fwft_config configs[];
};

स्थिर स्थिरांक अचिन्हित दीर्घ fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_चळत,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

स्थिर bool fwft_is_defined_feature(गणना sbi_fwft_feature_t feature)
{
	पूर्णांक i;

	साठी (i = 0; i < array_size(fwft_defined_features); i++) {
		जर (fwft_defined_features[i] == feature)
			परत true;
	}

	परत false;
}

स्थिर पूर्णांक fwft_menvcfg_set_bit(अचिन्हित दीर्घ मूल्य, अचिन्हित दीर्घ bit)
{
	जर (मूल्य == 1) {
		जर (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		अन्यथा
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} अन्यथा जर (मूल्य == 0) {
		जर (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		अन्यथा
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} अन्यथा {
		परत SBI_EINVAL;
	}

	परत SBI_OK;
}

स्थिर पूर्णांक fwft_menvcfg_read_bit(अचिन्हित दीर्घ *मूल्य, अचिन्हित दीर्घ bit)
{
	अचिन्हित दीर्घ cfg;

	जर (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	अन्यथा
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*मूल्य = cfg != 0;

	परत SBI_OK;
}

स्थिर पूर्णांक fwft_misaligned_delegation_supported(संरचना fwft_config *conf)
{
	जर (!misa_extension('S'))
		परत SBI_ENOTSUPP;

	परत SBI_OK;
}

स्थिर पूर्णांक fwft_set_misaligned_delegation(संरचना fwft_config *conf,
					 अचिन्हित दीर्घ मूल्य)
{
	जर (मूल्य == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	अन्यथा जर (मूल्य == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	अन्यथा
		परत SBI_EINVAL;

	परत SBI_OK;
}

स्थिर पूर्णांक fwft_get_misaligned_delegation(संरचना fwft_config *conf,
					 अचिन्हित दीर्घ *मूल्य)
{
	*मूल्य = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	परत SBI_OK;
}

स्थिर पूर्णांक fwft_double_trap_supported(संरचना fwft_config *conf)
{
	जर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		परत SBI_ENOTSUPP;

	परत SBI_OK;
}

स्थिर पूर्णांक fwft_set_double_trap(संरचना fwft_config *conf, अचिन्हित दीर्घ मूल्य)
{
	परत fwft_menvcfg_set_bit(मूल्य, ENVCFG_DTE_SHIFT);
}

स्थिर पूर्णांक fwft_get_double_trap(संरचना fwft_config *conf, अचिन्हित दीर्घ *मूल्य)
{
	परत fwft_menvcfg_read_bit(मूल्य, ENVCFG_DTE_SHIFT);
}

स्थिर पूर्णांक fwft_adue_supported(संरचना fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	जर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		परत SBI_ENOTSUPP;

	परत SBI_OK;
}

स्थिर पूर्णांक fwft_set_adue(संरचना fwft_config *conf, अचिन्हित दीर्घ मूल्य)
{
	पूर्णांक res = fwft_menvcfg_set_bit(मूल्य, ENVCFG_ADUE_SHIFT);

	जर (res == SBI_OK) {
		__sbi_sfence_vma_all();

		जर (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	परत res;
}

स्थिर पूर्णांक fwft_get_adue(संरचना fwft_config *conf, अचिन्हित दीर्घ *मूल्य)
{
	परत fwft_menvcfg_read_bit(मूल्य, ENVCFG_ADUE_SHIFT);
}

स्थिर पूर्णांक fwft_lpad_supported(संरचना fwft_config *conf)
{
	जर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		परत SBI_ENOTSUPP;

	परत SBI_OK;
}

स्थिर पूर्णांक fwft_enable_lpad(संरचना fwft_config *conf, अचिन्हित दीर्घ मूल्य)
{
	परत fwft_menvcfg_set_bit(मूल्य, ENVCFG_LPE_SHIFT);
}

स्थिर पूर्णांक fwft_get_lpad(संरचना fwft_config *conf, अचिन्हित दीर्घ *मूल्य)
{
	परत fwft_menvcfg_read_bit(मूल्य, ENVCFG_LPE_SHIFT);
}

स्थिर पूर्णांक fwft_sstack_supported(संरचना fwft_config *conf)
{
	जर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		परत SBI_ENOTSUPP;

	परत SBI_OK;
}

स्थिर पूर्णांक fwft_enable_sstack(संरचना fwft_config *conf, अचिन्हित दीर्घ मूल्य)
{
	परत fwft_menvcfg_set_bit(मूल्य, ENVCFG_SSE_SHIFT);
}

स्थिर पूर्णांक fwft_get_sstack(संरचना fwft_config *conf, अचिन्हित दीर्घ *मूल्य)
{
	परत fwft_menvcfg_read_bit(मूल्य, ENVCFG_SSE_SHIFT);
}

#जर __riscv_xlen > 32
स्थिर पूर्णांक fwft_pmlen_supported(संरचना fwft_config *conf)
{
	जर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		परत SBI_ENOTSUPP;

	परत SBI_OK;
}

स्थिर पूर्णांक fwft_set_pmlen(संरचना fwft_config *conf, अचिन्हित दीर्घ मूल्य)
{
	अचिन्हित दीर्घ pmm, prev;

	निवड (मूल्य) {
	स्थिती 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		थांबा;
	स्थिती 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		थांबा;
	स्थिती 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		थांबा;
	पूर्वनिर्धारित:
		परत SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	जर ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		परत SBI_EINVAL;
	}

	परत SBI_OK;
}

स्थिर पूर्णांक fwft_get_pmlen(संरचना fwft_config *conf, अचिन्हित दीर्घ *मूल्य)
{
	निवड (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	स्थिती ENVCFG_PMM_PMLEN_0:
		*मूल्य = 0;
		थांबा;
	स्थिती ENVCFG_PMM_PMLEN_7:
		*मूल्य = 7;
		थांबा;
	स्थिती ENVCFG_PMM_PMLEN_16:
		*मूल्य = 16;
		थांबा;
	पूर्वनिर्धारित:
		परत SBI_EFAIL;
	}

	परत SBI_OK;
}
#endif

स्थिर संरचना fwft_config* get_feature_config(गणना sbi_fwft_feature_t feature)
{
	पूर्णांक i;
	संरचना fwft_hart_state *fhs = fwft_thishart_state_ptr();

	जर (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		परत रिक्त;

	साठी (i = 0; i < fhs->config_मोजणी; i++){
		जर (feature == fhs->configs[i].feature->id)
			परत &fhs->configs[i];
	}

	परत रिक्त;
}

स्थिर पूर्णांक fwft_get_feature(गणना sbi_fwft_feature_t feature,
			    संरचना fwft_config **conf)
{
	पूर्णांक ret;
	संरचना fwft_config *tconf;

	tconf = get_feature_config(feature);
	जर (!tconf) {
		जर (fwft_is_defined_feature(feature))
			परत SBI_ENOTSUPP;

		परत SBI_EDENIED;
	}

	जर (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		जर (ret)
			परत ret;
	}
	*conf = tconf;

	परत SBI_SUCCESS;
}

स्थिर शून्य fwft_clear_config_lock(गणना sbi_fwft_feature_t feature)
{
	पूर्णांक ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	जर (ret)
		परत;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

पूर्णांक sbi_fwft_set(गणना sbi_fwft_feature_t feature, अचिन्हित दीर्घ मूल्य,
		 अचिन्हित दीर्घ flags)
{
	पूर्णांक ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	जर (ret)
		परत ret;

	जर ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		परत SBI_EINVAL;

	जर (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		परत SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, मूल्य);
	जर (ret)
		परत ret;

	conf->flags = flags;

	परत SBI_OK;
}

पूर्णांक sbi_fwft_get(गणना sbi_fwft_feature_t feature, अचिन्हित दीर्घ *out_val)
{
	पूर्णांक ret;
	संरचना fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	जर (ret)
		परत ret;

	परत conf->feature->get(conf, out_val);
}

स्थिर स्थिरांक संरचना fwft_feature features[] =
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
		.id = SBI_FWFT_SHADOW_चळत,
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
#जर __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

पूर्णांक sbi_fwft_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	पूर्णांक i;
	संरचना fwft_hart_state *fhs;

	जर (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(शून्य *);
		जर (!fwft_ptr_offset)
			परत SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	जर (!fhs) {
		fhs = sbi_zalloc(आकार(*fhs) + array_size(features) * आकार(संरचना fwft_config));
		जर (!fhs)
			परत SBI_ENOMEM;

		fhs->config_मोजणी = array_size(features);
		साठी (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	साठी (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	परत 0;
}
