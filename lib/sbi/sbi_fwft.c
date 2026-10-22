/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitmap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hfence.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_tlb.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_types.h>

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
ସ୍ଥିର unsigned long fwft_ptr_offset;

#define fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ଶୂନ୍ୟ *, fwft_ptr_offset)

#define fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define fwft_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), ଶୂନ୍ୟ *, fwft_ptr_offset, (__phs))

#define MIS_DELEG (1UL << CAUSE_MISALIGNED_LOAD | 1UL << CAUSE_MISALIGNED_STORE)

ସଂରଚନା fwft_config;

ସଂରଚନା fwft_feature {
	ଗଣନା sbi_fwft_feature_t id;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*supported)(ସଂରଚନା fwft_config *conf);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*set)(ସଂରଚନା fwft_config *conf, unsigned long ମୂଲ୍ୟ);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*get)(ସଂରଚନା fwft_config *conf, unsigned long *ମୂଲ୍ୟ);
};

ସଂରଚନା fwft_config {
	ଅଚଳ ସଂରଚନା fwft_feature *feature;
	unsigned long flags;
};

ସଂରଚନା fwft_hart_state {
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା config_ଗଣନା;
	ସଂରଚନା fwft_config configs[];
};

ସ୍ଥିର ଅଚଳ unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_ଗଦା,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

ସ୍ଥିର bool fwft_is_defined_feature(ଗଣନା sbi_fwft_feature_t feature)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପାଇଁ (i = 0; i < array_size(fwft_defined_features); i++) {
		ଯଦି (fwft_defined_features[i] == feature)
			ଫେରନ୍ତୁ true;
	}

	ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_menvcfg_set_bit(unsigned long ମୂଲ୍ୟ, unsigned long bit)
{
	ଯଦି (ମୂଲ୍ୟ == 1) {
		ଯଦି (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		ଅନ୍ୟଥା
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} ଅନ୍ୟଥା ଯଦି (ମୂଲ୍ୟ == 0) {
		ଯଦି (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		ଅନ୍ୟଥା
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ SBI_EINVAL;
	}

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_menvcfg_read_bit(unsigned long *ମୂଲ୍ୟ, unsigned long bit)
{
	unsigned long cfg;

	ଯଦି (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	ଅନ୍ୟଥା
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*ମୂଲ୍ୟ = cfg != 0;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_misaligned_delegation_supported(ସଂରଚନା fwft_config *conf)
{
	ଯଦି (!misa_extension('S'))
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_set_misaligned_delegation(ସଂରଚନା fwft_config *conf,
					 unsigned long ମୂଲ୍ୟ)
{
	ଯଦି (ମୂଲ୍ୟ == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	ଅନ୍ୟଥା ଯଦି (ମୂଲ୍ୟ == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_get_misaligned_delegation(ସଂରଚନା fwft_config *conf,
					 unsigned long *ମୂଲ୍ୟ)
{
	*ମୂଲ୍ୟ = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_double_trap_supported(ସଂରଚନା fwft_config *conf)
{
	ଯଦି (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_set_double_trap(ସଂରଚନା fwft_config *conf, unsigned long ମୂଲ୍ୟ)
{
	ଫେରନ୍ତୁ fwft_menvcfg_set_bit(ମୂଲ୍ୟ, ENVCFG_DTE_SHIFT);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_get_double_trap(ସଂରଚନା fwft_config *conf, unsigned long *ମୂଲ୍ୟ)
{
	ଫେରନ୍ତୁ fwft_menvcfg_read_bit(ମୂଲ୍ୟ, ENVCFG_DTE_SHIFT);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_adue_supported(ସଂରଚନା fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	ଯଦି (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_set_adue(ସଂରଚନା fwft_config *conf, unsigned long ମୂଲ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = fwft_menvcfg_set_bit(ମୂଲ୍ୟ, ENVCFG_ADUE_SHIFT);

	ଯଦି (res == SBI_OK) {
		__sbi_sfence_vma_all();

		ଯଦି (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	ଫେରନ୍ତୁ res;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_get_adue(ସଂରଚନା fwft_config *conf, unsigned long *ମୂଲ୍ୟ)
{
	ଫେରନ୍ତୁ fwft_menvcfg_read_bit(ମୂଲ୍ୟ, ENVCFG_ADUE_SHIFT);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_lpad_supported(ସଂରଚନା fwft_config *conf)
{
	ଯଦି (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_enable_lpad(ସଂରଚନା fwft_config *conf, unsigned long ମୂଲ୍ୟ)
{
	ଫେରନ୍ତୁ fwft_menvcfg_set_bit(ମୂଲ୍ୟ, ENVCFG_LPE_SHIFT);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_get_lpad(ସଂରଚନା fwft_config *conf, unsigned long *ମୂଲ୍ୟ)
{
	ଫେରନ୍ତୁ fwft_menvcfg_read_bit(ମୂଲ୍ୟ, ENVCFG_LPE_SHIFT);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_sstack_supported(ସଂରଚନା fwft_config *conf)
{
	ଯଦି (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_enable_sstack(ସଂରଚନା fwft_config *conf, unsigned long ମୂଲ୍ୟ)
{
	ଫେରନ୍ତୁ fwft_menvcfg_set_bit(ମୂଲ୍ୟ, ENVCFG_SSE_SHIFT);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_get_sstack(ସଂରଚନା fwft_config *conf, unsigned long *ମୂଲ୍ୟ)
{
	ଫେରନ୍ତୁ fwft_menvcfg_read_bit(ମୂଲ୍ୟ, ENVCFG_SSE_SHIFT);
}

#ଯଦି __riscv_xlen > 32
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_pmlen_supported(ସଂରଚନା fwft_config *conf)
{
	ଯଦି (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_set_pmlen(ସଂରଚନା fwft_config *conf, unsigned long ମୂଲ୍ୟ)
{
	unsigned long pmm, prev;

	ବାଛ (ମୂଲ୍ୟ) {
	ସ୍ଥିତି 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	ଯଦି ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		ଫେରନ୍ତୁ SBI_EINVAL;
	}

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_get_pmlen(ସଂରଚନା fwft_config *conf, unsigned long *ମୂଲ୍ୟ)
{
	ବାଛ (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	ସ୍ଥିତି ENVCFG_PMM_PMLEN_0:
		*ମୂଲ୍ୟ = 0;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି ENVCFG_PMM_PMLEN_7:
		*ମୂଲ୍ୟ = 7;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି ENVCFG_PMM_PMLEN_16:
		*ମୂଲ୍ୟ = 16;
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ SBI_EFAIL;
	}

	ଫେରନ୍ତୁ SBI_OK;
}
#endif

ସ୍ଥିର ସଂରଚନା fwft_config* get_feature_config(ଗଣନା sbi_fwft_feature_t feature)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ସଂରଚନା fwft_hart_state *fhs = fwft_thishart_state_ptr();

	ଯଦି (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		ଫେରନ୍ତୁ ଖାଲି;

	ପାଇଁ (i = 0; i < fhs->config_ଗଣନା; i++){
		ଯଦି (feature == fhs->configs[i].feature->id)
			ଫେରନ୍ତୁ &fhs->configs[i];
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fwft_get_feature(ଗଣନା sbi_fwft_feature_t feature,
			    ସଂରଚନା fwft_config **conf)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା fwft_config *tconf;

	tconf = get_feature_config(feature);
	ଯଦି (!tconf) {
		ଯଦି (fwft_is_defined_feature(feature))
			ଫେରନ୍ତୁ SBI_ENOTSUPP;

		ଫେରନ୍ତୁ SBI_EDENIED;
	}

	ଯଦି (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}
	*conf = tconf;

	ଫେରନ୍ତୁ SBI_SUCCESS;
}

ସ୍ଥିର ଶୂନ୍ୟ fwft_clear_config_lock(ଗଣନା sbi_fwft_feature_t feature)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ଯଦି (ret)
		ଫେରନ୍ତୁ;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_fwft_set(ଗଣନା sbi_fwft_feature_t feature, unsigned long ମୂଲ୍ୟ,
		 unsigned long flags)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	ଯଦି ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		ଫେରନ୍ତୁ SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, ମୂଲ୍ୟ);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	conf->flags = flags;

	ଫେରନ୍ତୁ SBI_OK;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_fwft_get(ଗଣନା sbi_fwft_feature_t feature, unsigned long *out_val)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	ଫେରନ୍ତୁ conf->feature->get(conf, out_val);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fwft_feature features[] =
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
		.id = SBI_FWFT_SHADOW_ଗଦା,
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
#ଯଦି __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_fwft_init(ସଂରଚନା sbi_scratch *scratch, bool cold_boot)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ସଂରଚନା fwft_hart_state *fhs;

	ଯଦି (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(ଶୂନ୍ୟ *);
		ଯଦି (!fwft_ptr_offset)
			ଫେରନ୍ତୁ SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	ଯଦି (!fhs) {
		fhs = sbi_zalloc(ଆକାର(*fhs) + array_size(features) * ଆକାର(ସଂରଚନା fwft_config));
		ଯଦି (!fhs)
			ଫେରନ୍ତୁ SBI_ENOMEM;

		fhs->config_ଗଣନା = array_size(features);
		ପାଇଁ (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	ପାଇଁ (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	ଫେରନ୍ତୁ 0;
}
