/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitmap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hfence.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_tlb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_types.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fwft_ptr_ਵਿੱਥ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ਖਾਲੀ *, fwft_ptr_ਵਿੱਥ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ fwft_ਸੈੱਟ_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), ਖਾਲੀ *, fwft_ptr_ਵਿੱਥ, (__phs))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIS_DELEG (1UL << CAUSE_MISALIGNED_ਲੱਦੋ | 1UL << CAUSE_MISALIGNED_STORE)

ਬਣਤਰ fwft_config;

ਬਣਤਰ fwft_feature {
	ਗਿਣਤੀ sbi_fwft_feature_t id;
	ਪੂਰਨ (*supported)(ਬਣਤਰ fwft_config *conf);
	ਪੂਰਨ (*ਸੈੱਟ)(ਬਣਤਰ fwft_config *conf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ);
	ਪੂਰਨ (*get)(ਬਣਤਰ fwft_config *conf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਮੁੱਲ);
};

ਬਣਤਰ fwft_config {
	ਅਚਲ ਬਣਤਰ fwft_feature *feature;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags;
};

ਬਣਤਰ fwft_hart_state {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ config_ਗਿਣਤੀ;
	ਬਣਤਰ fwft_config configs[];
};

ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_ਢੇਰ,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_ਸੂਚਕ_MASKING_PMLEN,
};

ਸਥਿਰ bool fwft_is_defined_feature(ਗਿਣਤੀ sbi_fwft_feature_t feature)
{
	ਪੂਰਨ i;

	ਲਈ (i = 0; i < array_ਮਾਪ(fwft_defined_features); i++) {
		ਜੇ (fwft_defined_features[i] == feature)
			ਵਾਪਸ true;
	}

	ਵਾਪਸ false;
}

ਸਥਿਰ ਪੂਰਨ fwft_menvcfg_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਦੋਆਧਾਰੀ_ਅੰਕ_2)
{
	ਜੇ (ਮੁੱਲ == 1) {
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ_2 >= 32 && __riscv_xlen == 32)
			csr_ਸੈੱਟ(CSR_MENVCFGH, _ULL(1) << (ਦੋਆਧਾਰੀ_ਅੰਕ_2 - 32));
		ਨਹੀਂਤਾਂ
			csr_ਸੈੱਟ(CSR_MENVCFG, _ULL(1) << ਦੋਆਧਾਰੀ_ਅੰਕ_2);

	} ਨਹੀਂਤਾਂ ਜੇ (ਮੁੱਲ == 0) {
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ_2 >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (ਦੋਆਧਾਰੀ_ਅੰਕ_2 - 32));
		ਨਹੀਂਤਾਂ
			csr_clear(CSR_MENVCFG, _ULL(1) << ਦੋਆਧਾਰੀ_ਅੰਕ_2);
	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ SBI_EINVAL;
	}

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ fwft_menvcfg_read_ਦੋਆਧਾਰੀ_ਅੰਕ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਦੋਆਧਾਰੀ_ਅੰਕ_2)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cfg;

	ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ_2 >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (ਦੋਆਧਾਰੀ_ਅੰਕ_2 - 32));
	ਨਹੀਂਤਾਂ
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << ਦੋਆਧਾਰੀ_ਅੰਕ_2);

	*ਮੁੱਲ = cfg != 0;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ fwft_misaligned_delegation_supported(ਬਣਤਰ fwft_config *conf)
{
	ਜੇ (!misa_extension('S'))
		ਵਾਪਸ SBI_ENOTSUPP;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ fwft_ਸੈੱਟ_misaligned_delegation(ਬਣਤਰ fwft_config *conf,
					 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ)
{
	ਜੇ (ਮੁੱਲ == 1)
		csr_ਸੈੱਟ(CSR_MEDELEG, MIS_DELEG);
	ਨਹੀਂਤਾਂ ਜੇ (ਮੁੱਲ == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ SBI_EINVAL;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ fwft_get_misaligned_delegation(ਬਣਤਰ fwft_config *conf,
					 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਮੁੱਲ)
{
	*ਮੁੱਲ = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ fwft_double_trap_supported(ਬਣਤਰ fwft_config *conf)
{
	ਜੇ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		ਵਾਪਸ SBI_ENOTSUPP;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ fwft_ਸੈੱਟ_double_trap(ਬਣਤਰ fwft_config *conf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ)
{
	ਵਾਪਸ fwft_menvcfg_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਮੁੱਲ, ENVCFG_DTE_ਖਿਸਕਾਓ);
}

ਸਥਿਰ ਪੂਰਨ fwft_get_double_trap(ਬਣਤਰ fwft_config *conf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਮੁੱਲ)
{
	ਵਾਪਸ fwft_menvcfg_read_ਦੋਆਧਾਰੀ_ਅੰਕ(ਮੁੱਲ, ENVCFG_DTE_ਖਿਸਕਾਓ);
}

ਸਥਿਰ ਪੂਰਨ fwft_adue_supported(ਬਣਤਰ fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	ਜੇ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		ਵਾਪਸ SBI_ENOTSUPP;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ fwft_ਸੈੱਟ_adue(ਬਣਤਰ fwft_config *conf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ)
{
	ਪੂਰਨ res = fwft_menvcfg_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਮੁੱਲ, ENVCFG_ADUE_ਖਿਸਕਾਓ);

	ਜੇ (res == SBI_OK) {
		__sbi_sfence_vma_all();

		ਜੇ (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	ਵਾਪਸ res;
}

ਸਥਿਰ ਪੂਰਨ fwft_get_adue(ਬਣਤਰ fwft_config *conf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਮੁੱਲ)
{
	ਵਾਪਸ fwft_menvcfg_read_ਦੋਆਧਾਰੀ_ਅੰਕ(ਮੁੱਲ, ENVCFG_ADUE_ਖਿਸਕਾਓ);
}

ਸਥਿਰ ਪੂਰਨ fwft_lpad_supported(ਬਣਤਰ fwft_config *conf)
{
	ਜੇ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		ਵਾਪਸ SBI_ENOTSUPP;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ fwft_enable_lpad(ਬਣਤਰ fwft_config *conf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ)
{
	ਵਾਪਸ fwft_menvcfg_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਮੁੱਲ, ENVCFG_LPE_ਖਿਸਕਾਓ);
}

ਸਥਿਰ ਪੂਰਨ fwft_get_lpad(ਬਣਤਰ fwft_config *conf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਮੁੱਲ)
{
	ਵਾਪਸ fwft_menvcfg_read_ਦੋਆਧਾਰੀ_ਅੰਕ(ਮੁੱਲ, ENVCFG_LPE_ਖਿਸਕਾਓ);
}

ਸਥਿਰ ਪੂਰਨ fwft_sstack_supported(ਬਣਤਰ fwft_config *conf)
{
	ਜੇ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		ਵਾਪਸ SBI_ENOTSUPP;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ fwft_enable_sstack(ਬਣਤਰ fwft_config *conf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ)
{
	ਵਾਪਸ fwft_menvcfg_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਮੁੱਲ, ENVCFG_SSE_ਖਿਸਕਾਓ);
}

ਸਥਿਰ ਪੂਰਨ fwft_get_sstack(ਬਣਤਰ fwft_config *conf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਮੁੱਲ)
{
	ਵਾਪਸ fwft_menvcfg_read_ਦੋਆਧਾਰੀ_ਅੰਕ(ਮੁੱਲ, ENVCFG_SSE_ਖਿਸਕਾਓ);
}

#ਜੇ __riscv_xlen > 32
ਸਥਿਰ ਪੂਰਨ fwft_pmlen_supported(ਬਣਤਰ fwft_config *conf)
{
	ਜੇ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		ਵਾਪਸ SBI_ENOTSUPP;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ fwft_ਸੈੱਟ_pmlen(ਬਣਤਰ fwft_config *conf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ pmm, prev;

	ਚੋਣ (ਮੁੱਲ) {
	ਹਾਲਤ 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		ਰੋਕੋ;
	ਹਾਲਤ 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		ਰੋਕੋ;
	ਹਾਲਤ 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		ਰੋਕੋ;
	ਮੂਲ:
		ਵਾਪਸ SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_ਸੈੱਟ(CSR_MENVCFG, pmm);
	ਜੇ ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		ਵਾਪਸ SBI_EINVAL;
	}

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ fwft_get_pmlen(ਬਣਤਰ fwft_config *conf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਮੁੱਲ)
{
	ਚੋਣ (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	ਹਾਲਤ ENVCFG_PMM_PMLEN_0:
		*ਮੁੱਲ = 0;
		ਰੋਕੋ;
	ਹਾਲਤ ENVCFG_PMM_PMLEN_7:
		*ਮੁੱਲ = 7;
		ਰੋਕੋ;
	ਹਾਲਤ ENVCFG_PMM_PMLEN_16:
		*ਮੁੱਲ = 16;
		ਰੋਕੋ;
	ਮੂਲ:
		ਵਾਪਸ SBI_EFAIL;
	}

	ਵਾਪਸ SBI_OK;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਬਣਤਰ fwft_config* get_feature_config(ਗਿਣਤੀ sbi_fwft_feature_t feature)
{
	ਪੂਰਨ i;
	ਬਣਤਰ fwft_hart_state *fhs = fwft_thishart_state_ptr();

	ਜੇ (feature & SBI_FWFT_GLOBAL_FEATURE_ਦੋਆਧਾਰੀ_ਅੰਕ)
		ਵਾਪਸ ਨਿਰਾ;

	ਲਈ (i = 0; i < fhs->config_ਗਿਣਤੀ; i++){
		ਜੇ (feature == fhs->configs[i].feature->id)
			ਵਾਪਸ &fhs->configs[i];
	}

	ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਪੂਰਨ fwft_get_feature(ਗਿਣਤੀ sbi_fwft_feature_t feature,
			    ਬਣਤਰ fwft_config **conf)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ fwft_config *tconf;

	tconf = get_feature_config(feature);
	ਜੇ (!tconf) {
		ਜੇ (fwft_is_defined_feature(feature))
			ਵਾਪਸ SBI_ENOTSUPP;

		ਵਾਪਸ SBI_EDENIED;
	}

	ਜੇ (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}
	*conf = tconf;

	ਵਾਪਸ SBI_SUCCESS;
}

ਸਥਿਰ ਖਾਲੀ fwft_clear_config_ਤਾਲਾ(ਗਿਣਤੀ sbi_fwft_feature_t feature)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ਜੇ (ret)
		ਵਾਪਸ;

	conf->flags &= ~SBI_FWFT_ਸੈੱਟ_FLAG_ਤਾਲਾ;
}

ਪੂਰਨ sbi_fwft_ਸੈੱਟ(ਗਿਣਤੀ sbi_fwft_feature_t feature, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ,
		 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ਜੇ ((flags & ~SBI_FWFT_ਸੈੱਟ_FLAG_ਤਾਲਾ) != 0)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (conf->flags & SBI_FWFT_ਸੈੱਟ_FLAG_ਤਾਲਾ)
		ਵਾਪਸ SBI_EDENIED_LOCKED;

	ret = conf->feature->ਸੈੱਟ(conf, ਮੁੱਲ);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	conf->flags = flags;

	ਵਾਪਸ SBI_OK;
}

ਪੂਰਨ sbi_fwft_get(ਗਿਣਤੀ sbi_fwft_feature_t feature, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_val)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ਵਾਪਸ conf->feature->get(conf, ਬਾਹਰਲਾ_val);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fwft_feature features[] =
{
	{
		.id = SBI_FWFT_MISALIGNED_EXC_DELEG,
		.supported = fwft_misaligned_delegation_supported,
		.ਸੈੱਟ = fwft_ਸੈੱਟ_misaligned_delegation,
		.get = fwft_get_misaligned_delegation,
	},
	{
		.id = SBI_FWFT_LANDING_PAD,
		.supported = fwft_lpad_supported,
		.ਸੈੱਟ = fwft_enable_lpad,
		.get = fwft_get_lpad,
	},
	{
		.id = SBI_FWFT_SHADOW_ਢੇਰ,
		.supported = fwft_sstack_supported,
		.ਸੈੱਟ = fwft_enable_sstack,
		.get = fwft_get_sstack,
	},
	{
		.id = SBI_FWFT_DOUBLE_TRAP,
		.supported = fwft_double_trap_supported,
		.ਸੈੱਟ = fwft_ਸੈੱਟ_double_trap,
		.get = fwft_get_double_trap,
	},
	{
		.id = SBI_FWFT_PTE_AD_HW_UPDATING,
		.supported = fwft_adue_supported,
		.ਸੈੱਟ = fwft_ਸੈੱਟ_adue,
		.get = fwft_get_adue,
	},
#ਜੇ __riscv_xlen > 32
	{
		.id = SBI_FWFT_ਸੂਚਕ_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.ਸੈੱਟ = fwft_ਸੈੱਟ_pmlen,
		.get = fwft_get_pmlen,
	},
#ਜੇ_ਅੰਤ
};

ਪੂਰਨ sbi_fwft_init(ਬਣਤਰ sbi_scratch *scratch, bool cold_boot)
{
	ਪੂਰਨ i;
	ਬਣਤਰ fwft_hart_state *fhs;

	ਜੇ (cold_boot) {
		fwft_ptr_ਵਿੱਥ = sbi_scratch_alloc_type_ਵਿੱਥ(ਖਾਲੀ *);
		ਜੇ (!fwft_ptr_ਵਿੱਥ)
			ਵਾਪਸ SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	ਜੇ (!fhs) {
		fhs = sbi_zalloc(ਆਕਾਰ(*fhs) + array_ਮਾਪ(features) * ਆਕਾਰ(ਬਣਤਰ fwft_config));
		ਜੇ (!fhs)
			ਵਾਪਸ SBI_ENOMEM;

		fhs->config_ਗਿਣਤੀ = array_ਮਾਪ(features);
		ਲਈ (i = 0; i < array_ਮਾਪ(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_ਸੈੱਟ_hart_state_ptr(scratch, fhs);
	}

	ਲਈ (i = 0; i < array_ਮਾਪ(features); i++)
		fwft_clear_config_ਤਾਲਾ(features[i].id);

	ਵਾਪਸ 0;
}
