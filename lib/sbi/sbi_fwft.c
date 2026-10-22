/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_bitmap.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_hfence.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_tlb.h>
#समावेश <sbi/sbi_types.h>

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
अटल अचिन्हित दीर्घ fwft_ptr_offset;

#घोषणा fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), व्योम *, fwft_ptr_offset)

#घोषणा fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#घोषणा fwft_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), व्योम *, fwft_ptr_offset, (__phs))

#घोषणा MIS_DELEG (1UL << CAUSE_MISALIGNED_LOAD | 1UL << CAUSE_MISALIGNED_STORE)

काष्ठा fwft_config;

काष्ठा fwft_feature {
	क्रमागत sbi_fwft_feature_t id;
	पूर्णांक (*supported)(काष्ठा fwft_config *conf);
	पूर्णांक (*set)(काष्ठा fwft_config *conf, अचिन्हित दीर्घ मूल्य);
	पूर्णांक (*get)(काष्ठा fwft_config *conf, अचिन्हित दीर्घ *मूल्य);
};

काष्ठा fwft_config {
	स्थिर काष्ठा fwft_feature *feature;
	अचिन्हित दीर्घ flags;
};

काष्ठा fwft_hart_state {
	अचिन्हित पूर्णांक config_गिनती;
	काष्ठा fwft_config configs[];
};

अटल स्थिर अचिन्हित दीर्घ fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_ढेर,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

अटल bool fwft_is_defined_feature(क्रमागत sbi_fwft_feature_t feature)
{
	पूर्णांक i;

	क्रम (i = 0; i < array_आकार(fwft_defined_features); i++) {
		अगर (fwft_defined_features[i] == feature)
			वापस true;
	}

	वापस false;
}

अटल पूर्णांक fwft_menvcfg_set_bit(अचिन्हित दीर्घ मूल्य, अचिन्हित दीर्घ bit)
{
	अगर (मूल्य == 1) {
		अगर (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		अन्यथा
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} अन्यथा अगर (मूल्य == 0) {
		अगर (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		अन्यथा
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} अन्यथा {
		वापस SBI_EINVAL;
	}

	वापस SBI_OK;
}

अटल पूर्णांक fwft_menvcfg_read_bit(अचिन्हित दीर्घ *मूल्य, अचिन्हित दीर्घ bit)
{
	अचिन्हित दीर्घ cfg;

	अगर (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	अन्यथा
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*मूल्य = cfg != 0;

	वापस SBI_OK;
}

अटल पूर्णांक fwft_misaligned_delegation_supported(काष्ठा fwft_config *conf)
{
	अगर (!misa_extension('S'))
		वापस SBI_ENOTSUPP;

	वापस SBI_OK;
}

अटल पूर्णांक fwft_set_misaligned_delegation(काष्ठा fwft_config *conf,
					 अचिन्हित दीर्घ मूल्य)
{
	अगर (मूल्य == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	अन्यथा अगर (मूल्य == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	अन्यथा
		वापस SBI_EINVAL;

	वापस SBI_OK;
}

अटल पूर्णांक fwft_get_misaligned_delegation(काष्ठा fwft_config *conf,
					 अचिन्हित दीर्घ *मूल्य)
{
	*मूल्य = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	वापस SBI_OK;
}

अटल पूर्णांक fwft_double_trap_supported(काष्ठा fwft_config *conf)
{
	अगर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		वापस SBI_ENOTSUPP;

	वापस SBI_OK;
}

अटल पूर्णांक fwft_set_double_trap(काष्ठा fwft_config *conf, अचिन्हित दीर्घ मूल्य)
{
	वापस fwft_menvcfg_set_bit(मूल्य, ENVCFG_DTE_SHIFT);
}

अटल पूर्णांक fwft_get_double_trap(काष्ठा fwft_config *conf, अचिन्हित दीर्घ *मूल्य)
{
	वापस fwft_menvcfg_read_bit(मूल्य, ENVCFG_DTE_SHIFT);
}

अटल पूर्णांक fwft_adue_supported(काष्ठा fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	अगर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		वापस SBI_ENOTSUPP;

	वापस SBI_OK;
}

अटल पूर्णांक fwft_set_adue(काष्ठा fwft_config *conf, अचिन्हित दीर्घ मूल्य)
{
	पूर्णांक res = fwft_menvcfg_set_bit(मूल्य, ENVCFG_ADUE_SHIFT);

	अगर (res == SBI_OK) {
		__sbi_sfence_vma_all();

		अगर (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	वापस res;
}

अटल पूर्णांक fwft_get_adue(काष्ठा fwft_config *conf, अचिन्हित दीर्घ *मूल्य)
{
	वापस fwft_menvcfg_read_bit(मूल्य, ENVCFG_ADUE_SHIFT);
}

अटल पूर्णांक fwft_lpad_supported(काष्ठा fwft_config *conf)
{
	अगर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		वापस SBI_ENOTSUPP;

	वापस SBI_OK;
}

अटल पूर्णांक fwft_enable_lpad(काष्ठा fwft_config *conf, अचिन्हित दीर्घ मूल्य)
{
	वापस fwft_menvcfg_set_bit(मूल्य, ENVCFG_LPE_SHIFT);
}

अटल पूर्णांक fwft_get_lpad(काष्ठा fwft_config *conf, अचिन्हित दीर्घ *मूल्य)
{
	वापस fwft_menvcfg_read_bit(मूल्य, ENVCFG_LPE_SHIFT);
}

अटल पूर्णांक fwft_sstack_supported(काष्ठा fwft_config *conf)
{
	अगर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		वापस SBI_ENOTSUPP;

	वापस SBI_OK;
}

अटल पूर्णांक fwft_enable_sstack(काष्ठा fwft_config *conf, अचिन्हित दीर्घ मूल्य)
{
	वापस fwft_menvcfg_set_bit(मूल्य, ENVCFG_SSE_SHIFT);
}

अटल पूर्णांक fwft_get_sstack(काष्ठा fwft_config *conf, अचिन्हित दीर्घ *मूल्य)
{
	वापस fwft_menvcfg_read_bit(मूल्य, ENVCFG_SSE_SHIFT);
}

#अगर __riscv_xlen > 32
अटल पूर्णांक fwft_pmlen_supported(काष्ठा fwft_config *conf)
{
	अगर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		वापस SBI_ENOTSUPP;

	वापस SBI_OK;
}

अटल पूर्णांक fwft_set_pmlen(काष्ठा fwft_config *conf, अचिन्हित दीर्घ मूल्य)
{
	अचिन्हित दीर्घ pmm, पूर्व;

	चयन (मूल्य) {
	हाल 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		अवरोध;
	हाल 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		अवरोध;
	हाल 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		अवरोध;
	शेष:
		वापस SBI_EINVAL;
	}

	पूर्व = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	अगर ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, पूर्व);
		वापस SBI_EINVAL;
	}

	वापस SBI_OK;
}

अटल पूर्णांक fwft_get_pmlen(काष्ठा fwft_config *conf, अचिन्हित दीर्घ *मूल्य)
{
	चयन (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	हाल ENVCFG_PMM_PMLEN_0:
		*मूल्य = 0;
		अवरोध;
	हाल ENVCFG_PMM_PMLEN_7:
		*मूल्य = 7;
		अवरोध;
	हाल ENVCFG_PMM_PMLEN_16:
		*मूल्य = 16;
		अवरोध;
	शेष:
		वापस SBI_EFAIL;
	}

	वापस SBI_OK;
}
#पूर्ण_अगर

अटल काष्ठा fwft_config* get_feature_config(क्रमागत sbi_fwft_feature_t feature)
{
	पूर्णांक i;
	काष्ठा fwft_hart_state *fhs = fwft_thishart_state_ptr();

	अगर (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		वापस शून्य;

	क्रम (i = 0; i < fhs->config_गिनती; i++){
		अगर (feature == fhs->configs[i].feature->id)
			वापस &fhs->configs[i];
	}

	वापस शून्य;
}

अटल पूर्णांक fwft_get_feature(क्रमागत sbi_fwft_feature_t feature,
			    काष्ठा fwft_config **conf)
{
	पूर्णांक ret;
	काष्ठा fwft_config *tconf;

	tconf = get_feature_config(feature);
	अगर (!tconf) {
		अगर (fwft_is_defined_feature(feature))
			वापस SBI_ENOTSUPP;

		वापस SBI_EDENIED;
	}

	अगर (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		अगर (ret)
			वापस ret;
	}
	*conf = tconf;

	वापस SBI_SUCCESS;
}

अटल व्योम fwft_clear_config_lock(क्रमागत sbi_fwft_feature_t feature)
{
	पूर्णांक ret;
	काष्ठा fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	अगर (ret)
		वापस;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

पूर्णांक sbi_fwft_set(क्रमागत sbi_fwft_feature_t feature, अचिन्हित दीर्घ मूल्य,
		 अचिन्हित दीर्घ flags)
{
	पूर्णांक ret;
	काष्ठा fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	अगर (ret)
		वापस ret;

	अगर ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		वापस SBI_EINVAL;

	अगर (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		वापस SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, मूल्य);
	अगर (ret)
		वापस ret;

	conf->flags = flags;

	वापस SBI_OK;
}

पूर्णांक sbi_fwft_get(क्रमागत sbi_fwft_feature_t feature, अचिन्हित दीर्घ *out_val)
{
	पूर्णांक ret;
	काष्ठा fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	अगर (ret)
		वापस ret;

	वापस conf->feature->get(conf, out_val);
}

अटल स्थिर काष्ठा fwft_feature features[] =
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
		.id = SBI_FWFT_SHADOW_ढेर,
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
#अगर __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#पूर्ण_अगर
};

पूर्णांक sbi_fwft_init(काष्ठा sbi_scratch *scratch, bool cold_boot)
{
	पूर्णांक i;
	काष्ठा fwft_hart_state *fhs;

	अगर (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(व्योम *);
		अगर (!fwft_ptr_offset)
			वापस SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	अगर (!fhs) {
		fhs = sbi_zalloc(माप(*fhs) + array_आकार(features) * माप(काष्ठा fwft_config));
		अगर (!fhs)
			वापस SBI_ENOMEM;

		fhs->config_गिनती = array_आकार(features);
		क्रम (i = 0; i < array_आकार(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	क्रम (i = 0; i < array_आकार(features); i++)
		fwft_clear_config_lock(features[i].id);

	वापस 0;
}
