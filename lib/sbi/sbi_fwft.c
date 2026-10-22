/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitmap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hfence.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_tlb.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_types.h>

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fwft_ptr_ಸ್ಥಾನಾಂತರ;

#ವ್ಯಾಖ್ಯಾನಿಸು fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, fwft_ptr_ಸ್ಥಾನಾಂತರ)

#ವ್ಯಾಖ್ಯಾನಿಸು fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#ವ್ಯಾಖ್ಯಾನಿಸು fwft_ಗಣ_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, fwft_ptr_ಸ್ಥಾನಾಂತರ, (__phs))

#ವ್ಯಾಖ್ಯಾನಿಸು MIS_DELEG (1UL << CAUSE_MISALIGNED_ತುಂಬು | 1UL << CAUSE_MISALIGNED_STORE)

ರಚನೆ fwft_config;

ರಚನೆ fwft_feature {
	ಎಣಿಕೆ sbi_fwft_feature_t id;
	ಪೂರ್ಣಾಂಕ (*supported)(ರಚನೆ fwft_config *conf);
	ಪೂರ್ಣಾಂಕ (*ಗಣ)(ರಚನೆ fwft_config *conf, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ);
	ಪೂರ್ಣಾಂಕ (*get)(ರಚನೆ fwft_config *conf, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಮೌಲ್ಯ);
};

ರಚನೆ fwft_config {
	ಸ್ಥಿರಾಂಕ ರಚನೆ fwft_feature *feature;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags;
};

ರಚನೆ fwft_hart_state {
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ config_ಎಣಿಕೆ;
	ರಚನೆ fwft_config configs[];
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_ರಾಶಿ,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_ಸೂಚಕ_MASKING_PMLEN,
};

ಸ್ಥಿರ bool fwft_is_defined_feature(ಎಣಿಕೆ sbi_fwft_feature_t feature)
{
	ಪೂರ್ಣಾಂಕ i;

	ಗಾಗಿ (i = 0; i < array_ಗಾತ್ರ(fwft_defined_features); i++) {
		ಆದರೆ (fwft_defined_features[i] == feature)
			ಹಿಂತಿರುಗಿ true;
	}

	ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_menvcfg_ಗಣ_ಬಿಟ್(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಬಿಟ್_2)
{
	ಆದರೆ (ಮೌಲ್ಯ == 1) {
		ಆದರೆ (ಬಿಟ್_2 >= 32 && __riscv_xlen == 32)
			csr_ಗಣ(CSR_MENVCFGH, _ULL(1) << (ಬಿಟ್_2 - 32));
		ಇಲ್ಲದಿದ್ದರೆ
			csr_ಗಣ(CSR_MENVCFG, _ULL(1) << ಬಿಟ್_2);

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೌಲ್ಯ == 0) {
		ಆದರೆ (ಬಿಟ್_2 >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (ಬಿಟ್_2 - 32));
		ಇಲ್ಲದಿದ್ದರೆ
			csr_clear(CSR_MENVCFG, _ULL(1) << ಬಿಟ್_2);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_menvcfg_read_ಬಿಟ್(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಬಿಟ್_2)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cfg;

	ಆದರೆ (ಬಿಟ್_2 >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (ಬಿಟ್_2 - 32));
	ಇಲ್ಲದಿದ್ದರೆ
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << ಬಿಟ್_2);

	*ಮೌಲ್ಯ = cfg != 0;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_misaligned_delegation_supported(ರಚನೆ fwft_config *conf)
{
	ಆದರೆ (!misa_extension('S'))
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_ಗಣ_misaligned_delegation(ರಚನೆ fwft_config *conf,
					 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ)
{
	ಆದರೆ (ಮೌಲ್ಯ == 1)
		csr_ಗಣ(CSR_MEDELEG, MIS_DELEG);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೌಲ್ಯ == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_get_misaligned_delegation(ರಚನೆ fwft_config *conf,
					 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಮೌಲ್ಯ)
{
	*ಮೌಲ್ಯ = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_double_trap_supported(ರಚನೆ fwft_config *conf)
{
	ಆದರೆ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_ಗಣ_double_trap(ರಚನೆ fwft_config *conf, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ)
{
	ಹಿಂತಿರುಗಿ fwft_menvcfg_ಗಣ_ಬಿಟ್(ಮೌಲ್ಯ, ENVCFG_DTE_ಸರಿಸು);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_get_double_trap(ರಚನೆ fwft_config *conf, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಮೌಲ್ಯ)
{
	ಹಿಂತಿರುಗಿ fwft_menvcfg_read_ಬಿಟ್(ಮೌಲ್ಯ, ENVCFG_DTE_ಸರಿಸು);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_adue_supported(ರಚನೆ fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	ಆದರೆ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_ಗಣ_adue(ರಚನೆ fwft_config *conf, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ)
{
	ಪೂರ್ಣಾಂಕ res = fwft_menvcfg_ಗಣ_ಬಿಟ್(ಮೌಲ್ಯ, ENVCFG_ADUE_ಸರಿಸು);

	ಆದರೆ (res == SBI_OK) {
		__sbi_sfence_vma_all();

		ಆದರೆ (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	ಹಿಂತಿರುಗಿ res;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_get_adue(ರಚನೆ fwft_config *conf, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಮೌಲ್ಯ)
{
	ಹಿಂತಿರುಗಿ fwft_menvcfg_read_ಬಿಟ್(ಮೌಲ್ಯ, ENVCFG_ADUE_ಸರಿಸು);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_lpad_supported(ರಚನೆ fwft_config *conf)
{
	ಆದರೆ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_enable_lpad(ರಚನೆ fwft_config *conf, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ)
{
	ಹಿಂತಿರುಗಿ fwft_menvcfg_ಗಣ_ಬಿಟ್(ಮೌಲ್ಯ, ENVCFG_LPE_ಸರಿಸು);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_get_lpad(ರಚನೆ fwft_config *conf, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಮೌಲ್ಯ)
{
	ಹಿಂತಿರುಗಿ fwft_menvcfg_read_ಬಿಟ್(ಮೌಲ್ಯ, ENVCFG_LPE_ಸರಿಸು);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_sstack_supported(ರಚನೆ fwft_config *conf)
{
	ಆದರೆ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_enable_sstack(ರಚನೆ fwft_config *conf, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ)
{
	ಹಿಂತಿರುಗಿ fwft_menvcfg_ಗಣ_ಬಿಟ್(ಮೌಲ್ಯ, ENVCFG_SSE_ಸರಿಸು);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_get_sstack(ರಚನೆ fwft_config *conf, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಮೌಲ್ಯ)
{
	ಹಿಂತಿರುಗಿ fwft_menvcfg_read_ಬಿಟ್(ಮೌಲ್ಯ, ENVCFG_SSE_ಸರಿಸು);
}

#ಆದರೆ __riscv_xlen > 32
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_pmlen_supported(ರಚನೆ fwft_config *conf)
{
	ಆದರೆ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_ಗಣ_pmlen(ರಚನೆ fwft_config *conf, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ pmm, prev;

	ಆಯ್ಕೆ (ಮೌಲ್ಯ) {
	ಸ್ಥಿತಿ 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_ಗಣ(CSR_MENVCFG, pmm);
	ಆದರೆ ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_get_pmlen(ರಚನೆ fwft_config *conf, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಮೌಲ್ಯ)
{
	ಆಯ್ಕೆ (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	ಸ್ಥಿತಿ ENVCFG_PMM_PMLEN_0:
		*ಮೌಲ್ಯ = 0;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ENVCFG_PMM_PMLEN_7:
		*ಮೌಲ್ಯ = 7;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ENVCFG_PMM_PMLEN_16:
		*ಮೌಲ್ಯ = 16;
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ SBI_EFAIL;
	}

	ಹಿಂತಿರುಗಿ SBI_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ರಚನೆ fwft_config* get_feature_config(ಎಣಿಕೆ sbi_fwft_feature_t feature)
{
	ಪೂರ್ಣಾಂಕ i;
	ರಚನೆ fwft_hart_state *fhs = fwft_thishart_state_ptr();

	ಆದರೆ (feature & SBI_FWFT_GLOBAL_FEATURE_ಬಿಟ್)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಗಾಗಿ (i = 0; i < fhs->config_ಎಣಿಕೆ; i++){
		ಆದರೆ (feature == fhs->configs[i].feature->id)
			ಹಿಂತಿರುಗಿ &fhs->configs[i];
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fwft_get_feature(ಎಣಿಕೆ sbi_fwft_feature_t feature,
			    ರಚನೆ fwft_config **conf)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ fwft_config *tconf;

	tconf = get_feature_config(feature);
	ಆದರೆ (!tconf) {
		ಆದರೆ (fwft_is_defined_feature(feature))
			ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

		ಹಿಂತಿರುಗಿ SBI_EDENIED;
	}

	ಆದರೆ (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}
	*conf = tconf;

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಸ್ಥಿರ ಶೂನ್ಯ fwft_clear_config_ಬೀಗ(ಎಣಿಕೆ sbi_fwft_feature_t feature)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ;

	conf->flags &= ~SBI_FWFT_ಗಣ_FLAG_ಬೀಗ;
}

ಪೂರ್ಣಾಂಕ sbi_fwft_ಗಣ(ಎಣಿಕೆ sbi_fwft_feature_t feature, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ,
		 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ಆದರೆ ((flags & ~SBI_FWFT_ಗಣ_FLAG_ಬೀಗ) != 0)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (conf->flags & SBI_FWFT_ಗಣ_FLAG_ಬೀಗ)
		ಹಿಂತಿರುಗಿ SBI_EDENIED_LOCKED;

	ret = conf->feature->ಗಣ(conf, ಮೌಲ್ಯ);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	conf->flags = flags;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಪೂರ್ಣಾಂಕ sbi_fwft_get(ಎಣಿಕೆ sbi_fwft_feature_t feature, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_val)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ಹಿಂತಿರುಗಿ conf->feature->get(conf, ಹೊರಹೋಗು_val);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fwft_feature features[] =
{
	{
		.id = SBI_FWFT_MISALIGNED_EXC_DELEG,
		.supported = fwft_misaligned_delegation_supported,
		.ಗಣ = fwft_ಗಣ_misaligned_delegation,
		.get = fwft_get_misaligned_delegation,
	},
	{
		.id = SBI_FWFT_LANDING_PAD,
		.supported = fwft_lpad_supported,
		.ಗಣ = fwft_enable_lpad,
		.get = fwft_get_lpad,
	},
	{
		.id = SBI_FWFT_SHADOW_ರಾಶಿ,
		.supported = fwft_sstack_supported,
		.ಗಣ = fwft_enable_sstack,
		.get = fwft_get_sstack,
	},
	{
		.id = SBI_FWFT_DOUBLE_TRAP,
		.supported = fwft_double_trap_supported,
		.ಗಣ = fwft_ಗಣ_double_trap,
		.get = fwft_get_double_trap,
	},
	{
		.id = SBI_FWFT_PTE_AD_HW_UPDATING,
		.supported = fwft_adue_supported,
		.ಗಣ = fwft_ಗಣ_adue,
		.get = fwft_get_adue,
	},
#ಆದರೆ __riscv_xlen > 32
	{
		.id = SBI_FWFT_ಸೂಚಕ_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.ಗಣ = fwft_ಗಣ_pmlen,
		.get = fwft_get_pmlen,
	},
#ಆದರೆ_ಅಂತ್ಯ
};

ಪೂರ್ಣಾಂಕ sbi_fwft_init(ರಚನೆ sbi_scratch *scratch, bool cold_boot)
{
	ಪೂರ್ಣಾಂಕ i;
	ರಚನೆ fwft_hart_state *fhs;

	ಆದರೆ (cold_boot) {
		fwft_ptr_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_type_ಸ್ಥಾನಾಂತರ(ಶೂನ್ಯ *);
		ಆದರೆ (!fwft_ptr_ಸ್ಥಾನಾಂತರ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	ಆದರೆ (!fhs) {
		fhs = sbi_zalloc(ಗಾತ್ರ(*fhs) + array_ಗಾತ್ರ(features) * ಗಾತ್ರ(ರಚನೆ fwft_config));
		ಆದರೆ (!fhs)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;

		fhs->config_ಎಣಿಕೆ = array_ಗಾತ್ರ(features);
		ಗಾಗಿ (i = 0; i < array_ಗಾತ್ರ(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_ಗಣ_hart_state_ptr(scratch, fhs);
	}

	ಗಾಗಿ (i = 0; i < array_ಗಾತ್ರ(features); i++)
		fwft_clear_config_ಬೀಗ(features[i].id);

	ಹಿಂತಿರುಗಿ 0;
}
