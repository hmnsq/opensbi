/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_bitmap.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_hfence.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi/sbi_tlb.h>
#അന്തര്ഭാവ <sbi/sbi_types.h>

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
സ്ഥിതം unsigned long fwft_ptr_offset;

#define fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ശൂന്യം *, fwft_ptr_offset)

#define fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define fwft_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), ശൂന്യം *, fwft_ptr_offset, (__phs))

#define MIS_DELEG (1UL << CAUSE_MISALIGNED_LOAD | 1UL << CAUSE_MISALIGNED_STORE)

ഘടന fwft_config;

ഘടന fwft_feature {
	എണ്ണൽ sbi_fwft_feature_t id;
	പൂർണ്ണസംഖ്യ (*supported)(ഘടന fwft_config *conf);
	പൂർണ്ണസംഖ്യ (*set)(ഘടന fwft_config *conf, unsigned long മൂല്യം);
	പൂർണ്ണസംഖ്യ (*get)(ഘടന fwft_config *conf, unsigned long *മൂല്യം);
};

ഘടന fwft_config {
	സ്ഥിരം ഘടന fwft_feature *feature;
	unsigned long flags;
};

ഘടന fwft_hart_state {
	unsigned പൂർണ്ണസംഖ്യ config_എണ്ണം;
	ഘടന fwft_config configs[];
};

സ്ഥിതം സ്ഥിരം unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_അട്ടി,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

സ്ഥിതം bool fwft_is_defined_feature(എണ്ണൽ sbi_fwft_feature_t feature)
{
	പൂർണ്ണസംഖ്യ i;

	വേണ്ടി (i = 0; i < array_size(fwft_defined_features); i++) {
		എങ്കിൽ (fwft_defined_features[i] == feature)
			തിരികെ true;
	}

	തിരികെ false;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_menvcfg_set_bit(unsigned long മൂല്യം, unsigned long bit)
{
	എങ്കിൽ (മൂല്യം == 1) {
		എങ്കിൽ (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		അല്ലെങ്കിൽ
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} അല്ലെങ്കിൽ എങ്കിൽ (മൂല്യം == 0) {
		എങ്കിൽ (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		അല്ലെങ്കിൽ
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} അല്ലെങ്കിൽ {
		തിരികെ SBI_EINVAL;
	}

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_menvcfg_read_bit(unsigned long *മൂല്യം, unsigned long bit)
{
	unsigned long cfg;

	എങ്കിൽ (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	അല്ലെങ്കിൽ
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*മൂല്യം = cfg != 0;

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_misaligned_delegation_supported(ഘടന fwft_config *conf)
{
	എങ്കിൽ (!misa_extension('S'))
		തിരികെ SBI_ENOTSUPP;

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_set_misaligned_delegation(ഘടന fwft_config *conf,
					 unsigned long മൂല്യം)
{
	എങ്കിൽ (മൂല്യം == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	അല്ലെങ്കിൽ എങ്കിൽ (മൂല്യം == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	അല്ലെങ്കിൽ
		തിരികെ SBI_EINVAL;

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_get_misaligned_delegation(ഘടന fwft_config *conf,
					 unsigned long *മൂല്യം)
{
	*മൂല്യം = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_double_trap_supported(ഘടന fwft_config *conf)
{
	എങ്കിൽ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		തിരികെ SBI_ENOTSUPP;

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_set_double_trap(ഘടന fwft_config *conf, unsigned long മൂല്യം)
{
	തിരികെ fwft_menvcfg_set_bit(മൂല്യം, ENVCFG_DTE_SHIFT);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_get_double_trap(ഘടന fwft_config *conf, unsigned long *മൂല്യം)
{
	തിരികെ fwft_menvcfg_read_bit(മൂല്യം, ENVCFG_DTE_SHIFT);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_adue_supported(ഘടന fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	എങ്കിൽ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		തിരികെ SBI_ENOTSUPP;

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_set_adue(ഘടന fwft_config *conf, unsigned long മൂല്യം)
{
	പൂർണ്ണസംഖ്യ res = fwft_menvcfg_set_bit(മൂല്യം, ENVCFG_ADUE_SHIFT);

	എങ്കിൽ (res == SBI_OK) {
		__sbi_sfence_vma_all();

		എങ്കിൽ (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	തിരികെ res;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_get_adue(ഘടന fwft_config *conf, unsigned long *മൂല്യം)
{
	തിരികെ fwft_menvcfg_read_bit(മൂല്യം, ENVCFG_ADUE_SHIFT);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_lpad_supported(ഘടന fwft_config *conf)
{
	എങ്കിൽ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		തിരികെ SBI_ENOTSUPP;

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_enable_lpad(ഘടന fwft_config *conf, unsigned long മൂല്യം)
{
	തിരികെ fwft_menvcfg_set_bit(മൂല്യം, ENVCFG_LPE_SHIFT);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_get_lpad(ഘടന fwft_config *conf, unsigned long *മൂല്യം)
{
	തിരികെ fwft_menvcfg_read_bit(മൂല്യം, ENVCFG_LPE_SHIFT);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_sstack_supported(ഘടന fwft_config *conf)
{
	എങ്കിൽ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		തിരികെ SBI_ENOTSUPP;

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_enable_sstack(ഘടന fwft_config *conf, unsigned long മൂല്യം)
{
	തിരികെ fwft_menvcfg_set_bit(മൂല്യം, ENVCFG_SSE_SHIFT);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_get_sstack(ഘടന fwft_config *conf, unsigned long *മൂല്യം)
{
	തിരികെ fwft_menvcfg_read_bit(മൂല്യം, ENVCFG_SSE_SHIFT);
}

#എങ്കിൽ __riscv_xlen > 32
സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_pmlen_supported(ഘടന fwft_config *conf)
{
	എങ്കിൽ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		തിരികെ SBI_ENOTSUPP;

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_set_pmlen(ഘടന fwft_config *conf, unsigned long മൂല്യം)
{
	unsigned long pmm, prev;

	തിരഞ്ഞെടുപ്പ് (മൂല്യം) {
	അവസ്ഥ 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		നിർത്തുക;
	അവസ്ഥ 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		നിർത്തുക;
	അവസ്ഥ 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		തിരികെ SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	എങ്കിൽ ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		തിരികെ SBI_EINVAL;
	}

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_get_pmlen(ഘടന fwft_config *conf, unsigned long *മൂല്യം)
{
	തിരഞ്ഞെടുപ്പ് (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	അവസ്ഥ ENVCFG_PMM_PMLEN_0:
		*മൂല്യം = 0;
		നിർത്തുക;
	അവസ്ഥ ENVCFG_PMM_PMLEN_7:
		*മൂല്യം = 7;
		നിർത്തുക;
	അവസ്ഥ ENVCFG_PMM_PMLEN_16:
		*മൂല്യം = 16;
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		തിരികെ SBI_EFAIL;
	}

	തിരികെ SBI_OK;
}
#endif

സ്ഥിതം ഘടന fwft_config* get_feature_config(എണ്ണൽ sbi_fwft_feature_t feature)
{
	പൂർണ്ണസംഖ്യ i;
	ഘടന fwft_hart_state *fhs = fwft_thishart_state_ptr();

	എങ്കിൽ (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		തിരികെ ശൂന്യ;

	വേണ്ടി (i = 0; i < fhs->config_എണ്ണം; i++){
		എങ്കിൽ (feature == fhs->configs[i].feature->id)
			തിരികെ &fhs->configs[i];
	}

	തിരികെ ശൂന്യ;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fwft_get_feature(എണ്ണൽ sbi_fwft_feature_t feature,
			    ഘടന fwft_config **conf)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന fwft_config *tconf;

	tconf = get_feature_config(feature);
	എങ്കിൽ (!tconf) {
		എങ്കിൽ (fwft_is_defined_feature(feature))
			തിരികെ SBI_ENOTSUPP;

		തിരികെ SBI_EDENIED;
	}

	എങ്കിൽ (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}
	*conf = tconf;

	തിരികെ SBI_SUCCESS;
}

സ്ഥിതം ശൂന്യം fwft_clear_config_lock(എണ്ണൽ sbi_fwft_feature_t feature)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	എങ്കിൽ (ret)
		തിരികെ;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

പൂർണ്ണസംഖ്യ sbi_fwft_set(എണ്ണൽ sbi_fwft_feature_t feature, unsigned long മൂല്യം,
		 unsigned long flags)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	എങ്കിൽ (ret)
		തിരികെ ret;

	എങ്കിൽ ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		തിരികെ SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, മൂല്യം);
	എങ്കിൽ (ret)
		തിരികെ ret;

	conf->flags = flags;

	തിരികെ SBI_OK;
}

പൂർണ്ണസംഖ്യ sbi_fwft_get(എണ്ണൽ sbi_fwft_feature_t feature, unsigned long *out_val)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	എങ്കിൽ (ret)
		തിരികെ ret;

	തിരികെ conf->feature->get(conf, out_val);
}

സ്ഥിതം സ്ഥിരം ഘടന fwft_feature features[] =
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
		.id = SBI_FWFT_SHADOW_അട്ടി,
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
#എങ്കിൽ __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

പൂർണ്ണസംഖ്യ sbi_fwft_init(ഘടന sbi_scratch *scratch, bool cold_boot)
{
	പൂർണ്ണസംഖ്യ i;
	ഘടന fwft_hart_state *fhs;

	എങ്കിൽ (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(ശൂന്യം *);
		എങ്കിൽ (!fwft_ptr_offset)
			തിരികെ SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	എങ്കിൽ (!fhs) {
		fhs = sbi_zalloc(വലുപ്പം(*fhs) + array_size(features) * വലുപ്പം(ഘടന fwft_config));
		എങ്കിൽ (!fhs)
			തിരികെ SBI_ENOMEM;

		fhs->config_എണ്ണം = array_size(features);
		വേണ്ടി (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	വേണ്ടി (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	തിരികെ 0;
}
