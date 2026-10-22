/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_barrier.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_fp.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_csr_detect.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart_pmp.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmu.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>

ಬಾಹ್ಯ ಶೂನ್ಯ __sbi_expected_trap(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ __sbi_expected_trap_hext(ಶೂನ್ಯ);

ಶೂನ್ಯ (*sbi_hart_expected_trap)(ಶೂನ್ಯ) = &__sbi_expected_trap;

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hart_features_ಸ್ಥಾನಾಂತರ;

ಸ್ಥಿರ ಶೂನ್ಯ mstatus_init(ರಚನೆ sbi_scratch *scratch)
{
	ಪೂರ್ಣಾಂಕ cidx;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mstatus_val = 0;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ mhpm_ಮುಖವಾಡ = sbi_hart_mhpm_ಮುಖವಾಡ(scratch);
	uint64_t mhpmevent_init_val = 0;
	uint64_t menvcfg_val, mstateen_val;

	/* Enable FPU */
	ಆದರೆ (misa_extension('D') || misa_extension('F'))
		mstatus_val |=  MSTATUS_FS;

	/* Enable Vector context */
	ಆದರೆ (misa_extension('V'))
		mstatus_val |=  MSTATUS_VS;

	csr_write(CSR_MSTATUS, mstatus_val);

	/* Disable user mode usage of all perf counters except TM */
	ಆದರೆ (misa_extension('S') &&
	    sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_SCOUNTEREN, 0x02);

	/**
	 * OpenSBI doesn't use any PMU counters in M-mode.
	 * Supervisor mode usage for all counters are enabled by default
	 * But counters will not run until mcountinhibit is set.
	 */
	ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	/* All programmable counters will start running at runtime after S-mode request */
	ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	/**
	 * The mhpmeventn[h] CSR should be initialized with interrupt disabled
	 * and inhibited running in M-mode during init.
	 */
	mhpmevent_init_val |= (MHPMEVENT_ದ | MHPMEVENT_MINH);
	ಗಾಗಿ (cidx = 0; cidx <= 28; cidx++) {
		ಆದರೆ (!(mhpm_ಮುಖವಾಡ & 1UL << (cidx + 3)))
			ಮುಂದುವರಿ;
#ಆದರೆ __riscv_xlen == 32
		csr_write_ಅಂಕೆ(CSR_MHPMEVENT3 + cidx,
			       mhpmevent_init_val & 0xFFFFFFFF);
		ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
			csr_write_ಅಂಕೆ(CSR_MHPMEVENT3H + cidx,
				      mhpmevent_init_val >> ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG);
#ಇಲ್ಲದಿದ್ದರೆ
		csr_write_ಅಂಕೆ(CSR_MHPMEVENT3 + cidx, mhpmevent_init_val);
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಆದರೆ (misa_extension('H'))
		csr_write(CSR_HSTATUS, 0);

	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMSTATEEN)) {
		mstateen_val = 0;
		mstateen_val |= SMSTATEEN_STATEN;
		mstateen_val |= SMSTATEEN0_CONTEXT;
		mstateen_val |= SMSTATEEN0_HSENVCFG;

		ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA))
			mstateen_val |= (SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);
		ಇಲ್ಲದಿದ್ದರೆ
			mstateen_val &= ~(SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);

		ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA) ||
		    sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCSRIND))
			mstateen_val |= (SMSTATEEN0_SVSLCT);
		ಇಲ್ಲದಿದ್ದರೆ
			mstateen_val &= ~(SMSTATEEN0_SVSLCT);

		ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCTR))
			mstateen_val |= SMSTATEEN0_CTR;
		ಇಲ್ಲದಿದ್ದರೆ
			mstateen_val &= ~SMSTATEEN0_CTR;

		ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
			mstateen_val |= SMSTATEEN0_SRMCFG;
		ಇಲ್ಲದಿದ್ದರೆ
			mstateen_val &= ~SMSTATEEN0_SRMCFG;

		csr_write64(CSR_MSTATEEN0, mstateen_val);
		csr_write64(CSR_MSTATEEN1, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN2, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN3, SMSTATEEN_STATEN);
	}

	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSSTATEEN)) {
		ಆದರೆ (misa_extension('S')) {
			csr_write(CSR_SSTATEEN0, 0);
			csr_write(CSR_SSTATEEN1, 0);
			csr_write(CSR_SSTATEEN2, 0);
			csr_write(CSR_SSTATEEN3, 0);
		}
		ಆದರೆ (misa_extension('H')) {
			csr_write64(CSR_HSTATEEN0, (uint64_t)0);
			csr_write64(CSR_HSTATEEN1, (uint64_t)0);
			csr_write64(CSR_HSTATEEN2, (uint64_t)0);
			csr_write64(CSR_HSTATEEN3, (uint64_t)0);
		}
	}

	ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_12) {
		menvcfg_val = csr_read64(CSR_MENVCFG);

		/* Disable HW A/D updating by default */
		menvcfg_val &= ~ENVCFG_ADUE;

		/* Disable double trap by default */
		menvcfg_val &= ~ENVCFG_DTE;

#ವ್ಯಾಖ್ಯಾನಿಸು __set_menvcfg_ext(__ext, __bits)				\
		ಆದರೆ (sbi_hart_has_extension(scratch, __ext))		\
			menvcfg_val |= __bits;

		/*
		 * Enable access to extensions if they are present in the
		 * hardware or in the device tree.
		 */

		__set_menvcfg_ext(SBI_HART_EXT_ZICBOZ, ENVCFG_CBZE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM, ENVCFG_CBCFE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM,
				  ENVCFG_CBIE_INV << ENVCFG_CBIE_ಸರಿಸು)
#ಆದರೆ __riscv_xlen > 32
		__set_menvcfg_ext(SBI_HART_EXT_SVPBMT, ENVCFG_PBMTE)
#ಆದರೆ_ಅಂತ್ಯ
		__set_menvcfg_ext(SBI_HART_EXT_SSTC, ENVCFG_STCE)
		__set_menvcfg_ext(SBI_HART_EXT_SMCDELEG, ENVCFG_CDE);

		/*
		 * Assume only Svadu is supported when it is the only extension
		 * present in the ISA string. Svade is assumed when neither are
		 * present. When both are present we must default to Svade (see
		 * the zero reset value of FWFT.PTE_AD_HW_UPDATING).
		 */
		ಆದರೆ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SVADE))
			__set_menvcfg_ext(SBI_HART_EXT_SVADU, ENVCFG_ADUE);

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __set_menvcfg_ext

		csr_write64(CSR_MENVCFG, menvcfg_val);

		/* Enable S-mode access to seed CSR */
		ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
			csr_ಗಣ(CSR_MSECCFG, MSECCFG_SSEED);
			csr_clear(CSR_MSECCFG, MSECCFG_USEED);
		}
	}

	/* Disable all interrupts */
	csr_write(CSR_MIE, 0);

	/* Disable S-mode paging */
	ಆದರೆ (misa_extension('S'))
		csr_write(CSR_SATP, 0);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fp_init(ರಚನೆ sbi_scratch *scratch)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __riscv_flen
	ಪೂರ್ಣಾಂಕ i;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (!misa_extension('D') && !misa_extension('F'))
		ಹಿಂತಿರುಗಿ 0;

	ಆದರೆ (!(csr_read(CSR_MSTATUS) & MSTATUS_FS))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __riscv_flen
	ಗಾಗಿ (i = 0; i < 32; i++)
		init_fp_reg(i);
	csr_write(CSR_FCSR, 0);
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ sbi_hart_delegation_dump(ರಚನೆ sbi_scratch *scratch,
			      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix)
{
	ಆದರೆ (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist*/
		ಹಿಂತಿರುಗಿ;

	sbi_printf("%sMIDELEG%s: 0x%" PRILX "\n",
		   ಪೂರ್ವಪ್ರತ್ಯಯ, suffix, csr_read(CSR_MIDELEG));
	sbi_printf("%sMEDELEG%s: 0x%" PRILX "\n",
		   ಪೂರ್ವಪ್ರತ್ಯಯ, suffix, csr_read(CSR_MEDELEG));
}

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ sbi_hart_mhpm_ಮುಖವಾಡ(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_hart_features *hfeatures =
			sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, hart_features_ಸ್ಥಾನಾಂತರ);

	ಹಿಂತಿರುಗಿ hfeatures->mhpm_ಮುಖವಾಡ;
}

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ sbi_hart_mhpm_ಬಿಟ್ಟುಗಳು(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_hart_features *hfeatures =
			sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, hart_features_ಸ್ಥಾನಾಂತರ);

	ಹಿಂತಿರುಗಿ hfeatures->mhpm_ಬಿಟ್ಟುಗಳು;
}

ಪೂರ್ಣಾಂಕ sbi_hart_priv_ಆವೃತ್ತಿ(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_hart_features *hfeatures =
			sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, hart_features_ಸ್ಥಾನಾಂತರ);

	ಹಿಂತಿರುಗಿ hfeatures->priv_ಆವೃತ್ತಿ;
}

ಶೂನ್ಯ sbi_hart_get_priv_ಆವೃತ್ತಿ_str(ರಚನೆ sbi_scratch *scratch,
				   ಅಕ್ಷರ *ಆವೃತ್ತಿ_str, ಪೂರ್ಣಾಂಕ nvstr)
{
	ಅಕ್ಷರ *ತಾತ್ಕಾಲಿಕ;
	ರಚನೆ sbi_hart_features *hfeatures =
			sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, hart_features_ಸ್ಥಾನಾಂತರ);

	ಆಯ್ಕೆ (hfeatures->priv_ಆವೃತ್ತಿ) {
	ಸ್ಥಿತಿ SBI_HART_PRIV_VER_1_10:
		ತಾತ್ಕಾಲಿಕ = "v1.10";
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_HART_PRIV_VER_1_11:
		ತಾತ್ಕಾಲಿಕ = "v1.11";
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_HART_PRIV_VER_1_12:
		ತಾತ್ಕಾಲಿಕ = "v1.12";
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ತಾತ್ಕಾಲಿಕ = "unknown";
		ನಿಲ್ಲಿಸು;
	}

	sbi_snprintf(ಆವೃತ್ತಿ_str, nvstr, "%s", ತಾತ್ಕಾಲಿಕ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ __sbi_hart_update_extension(
					ರಚನೆ sbi_hart_features *hfeatures,
					ಎಣಿಕೆ sbi_hart_extensions ext,
					bool enable)
{
	ಆದರೆ (enable)
		__set_bit(ext, hfeatures->extensions);
	ಇಲ್ಲದಿದ್ದರೆ
		__clear_bit(ext, hfeatures->extensions);
}

/**
 * Enable/Disable a particular hart extension
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @param enable new state of hart extension
 */
ಶೂನ್ಯ sbi_hart_update_extension(ರಚನೆ sbi_scratch *scratch,
			       ಎಣಿಕೆ sbi_hart_extensions ext,
			       bool enable)
{
	ರಚನೆ sbi_hart_features *hfeatures =
			sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, hart_features_ಸ್ಥಾನಾಂತರ);

	__sbi_hart_update_extension(hfeatures, ext, enable);
}

/**
 * Check whether a particular hart extension is available
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @returns true (available) or false (not available)
 */
bool sbi_hart_has_extension(ರಚನೆ sbi_scratch *scratch,
			    ಎಣಿಕೆ sbi_hart_extensions ext)
{
	ರಚನೆ sbi_hart_features *hfeatures =
			sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, hart_features_ಸ್ಥಾನಾಂತರ);

	ಆದರೆ (__test_bit(ext, hfeatures->extensions))
		ಹಿಂತಿರುಗಿ true;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ false;
}

#ವ್ಯಾಖ್ಯಾನಿಸು __SBI_HART_EXT_DATA(_name, _id) {	\
	.name = #_name,				\
	.id = _id,				\
}

ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_hart_ext_ದತ್ತಾಂಶ sbi_hart_ext[] = {
	__SBI_HART_EXT_DATA(smaia, SBI_HART_EXT_SMAIA),
	__SBI_HART_EXT_DATA(smepmp, SBI_HART_EXT_SMEPMP),
	__SBI_HART_EXT_DATA(smstateen, SBI_HART_EXT_SMSTATEEN),
	__SBI_HART_EXT_DATA(sscofpmf, SBI_HART_EXT_SSCOFPMF),
	__SBI_HART_EXT_DATA(sstc, SBI_HART_EXT_SSTC),
	__SBI_HART_EXT_DATA(zicntr, SBI_HART_EXT_ZICNTR),
	__SBI_HART_EXT_DATA(zihpm, SBI_HART_EXT_ZIHPM),
	__SBI_HART_EXT_DATA(zkr, SBI_HART_EXT_ZKR),
	__SBI_HART_EXT_DATA(smcntrpmf, SBI_HART_EXT_SMCNTRPMF),
	__SBI_HART_EXT_DATA(xandespmu, SBI_HART_EXT_XANDESPMU),
	__SBI_HART_EXT_DATA(zicboz, SBI_HART_EXT_ZICBOZ),
	__SBI_HART_EXT_DATA(zicbom, SBI_HART_EXT_ZICBOM),
	__SBI_HART_EXT_DATA(svpbmt, SBI_HART_EXT_SVPBMT),
	__SBI_HART_EXT_DATA(sdtrig, SBI_HART_EXT_SDTRIG),
	__SBI_HART_EXT_DATA(smcsrind, SBI_HART_EXT_SMCSRIND),
	__SBI_HART_EXT_DATA(smcdeleg, SBI_HART_EXT_SMCDELEG),
	__SBI_HART_EXT_DATA(sscsrind, SBI_HART_EXT_SSCSRIND),
	__SBI_HART_EXT_DATA(ssccfg, SBI_HART_EXT_SSCCFG),
	__SBI_HART_EXT_DATA(svade, SBI_HART_EXT_SVADE),
	__SBI_HART_EXT_DATA(svadu, SBI_HART_EXT_SVADU),
	__SBI_HART_EXT_DATA(smnpm, SBI_HART_EXT_SMNPM),
	__SBI_HART_EXT_DATA(zicfilp, SBI_HART_EXT_ZICFILP),
	__SBI_HART_EXT_DATA(zicfiss, SBI_HART_EXT_ZICFISS),
	__SBI_HART_EXT_DATA(ssdbltrp, SBI_HART_EXT_SSDBLTRP),
	__SBI_HART_EXT_DATA(smctr, SBI_HART_EXT_SMCTR),
	__SBI_HART_EXT_DATA(ssctr, SBI_HART_EXT_SSCTR),
	__SBI_HART_EXT_DATA(ssqosid, SBI_HART_EXT_SSQOSID),
	__SBI_HART_EXT_DATA(ssstateen, SBI_HART_EXT_SSSTATEEN),
	__SBI_HART_EXT_DATA(xsfcflushdlone, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1),
	__SBI_HART_EXT_DATA(xsfcease, SBI_HART_EXT_XSIFIVE_CEASE),
	__SBI_HART_EXT_DATA(smrnmi, SBI_HART_EXT_SMRNMI),
	__SBI_HART_EXT_DATA(v, SBI_HART_EXT_V),
	__SBI_HART_EXT_DATA(f, SBI_HART_EXT_F),
	__SBI_HART_EXT_DATA(d, SBI_HART_EXT_D),
};

_Static_assert(SBI_HART_EXT_MAX == array_ಗಾತ್ರ(sbi_hart_ext),
	       "sbi_hart_ext[]: wrong number of entries");

/**
 * Get the hart extensions in string format
 *
 * @param scratch pointer to the HART scratch space
 * @param extensions_str pointer to a char array where the extensions string
 *			 will be updated
 * @param nestr length of the features_str. The feature string will be
 *		truncated if nestr is not long enough.
 */
ಶೂನ್ಯ sbi_hart_get_extensions_str(ರಚನೆ sbi_scratch *scratch,
				 ಅಕ್ಷರ *extensions_str, ಪೂರ್ಣಾಂಕ nestr)
{
	ರಚನೆ sbi_hart_features *hfeatures =
			sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, hart_features_ಸ್ಥಾನಾಂತರ);
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0, ext = 0;

	ಆದರೆ (!extensions_str || nestr <= 0)
		ಹಿಂತಿರುಗಿ;
	sbi_memset(extensions_str, 0, nestr);

	for_each_ಗಣ_ಬಿಟ್(ext, hfeatures->extensions, SBI_HART_EXT_MAX) {
		ಆದರೆ (ಸ್ಥಾನಾಂತರ + sbi_strlen(sbi_hart_ext[ext].name) + 1 > nestr) {
			sbi_printf("%s:extension name is longer than buffer (error)\n", __func__);
			ನಿಲ್ಲಿಸು;
		}
		sbi_snprintf(extensions_str + ಸ್ಥಾನಾಂತರ,
				 nestr - ಸ್ಥಾನಾಂತರ,
				 "%s,", sbi_hart_ext[ext].name);
		ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ + sbi_strlen(sbi_hart_ext[ext].name) + 1;
	}

	ಆದರೆ (ಸ್ಥಾನಾಂತರ)
		extensions_str[ಸ್ಥಾನಾಂತರ - 1] = '\0';
	ಇಲ್ಲದಿದ್ದರೆ
		sbi_strncpy(extensions_str, "none", nestr);
}

/**
 * Check whether a particular CSR is present on the HART
 *
 * @param scratch pointer to the HART scratch space
 * @param csr the CSR number to check
 */
bool sbi_hart_has_csr(ರಚನೆ sbi_scratch *scratch, ಎಣಿಕೆ sbi_hart_csrs csr)
{
	ರಚನೆ sbi_hart_features *hfeatures =
			sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, hart_features_ಸ್ಥಾನಾಂತರ);

	ಹಿಂತಿರುಗಿ __test_bit(csr, hfeatures->csrs);
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hart_pmp_get_allowed_addr(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val = 0;
	ರಚನೆ sbi_trap_info trap = {0};

	csr_write_allowed(CSR_PMPCFG0, &trap, 0);
	ಆದರೆ (trap.cause)
		ಹಿಂತಿರುಗಿ 0;

	csr_write_allowed(CSR_PMPADDR0, &trap, PMP_ADDR_ಮುಖವಾಡ);
	ಆದರೆ (!trap.cause) {
		val = csr_read_allowed(CSR_PMPADDR0, &trap);
		ಆದರೆ (trap.cause)
			val = 0;
	}

	ಹಿಂತಿರುಗಿ val;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hart_mhpm_get_allowed_ಬಿಟ್ಟುಗಳು(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val = ~(0UL);
	ರಚನೆ sbi_trap_info trap = {0};
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಬಿಟ್ಟುಗಳು = 0;

	/**
	 * It is assumed that platforms will implement same number of bits for
	 * all the performance counters including mcycle/minstret.
	 */
	csr_write_allowed(CSR_MHPMCOUNTER3, &trap, val);
	ಆದರೆ (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3, &trap);
		ಆದರೆ (trap.cause)
			ಹಿಂತಿರುಗಿ 0;
	}
	ಅಂಕೆ_ಬಿಟ್ಟುಗಳು = sbi_fls(val) + 1;
#ಆದರೆ __riscv_xlen == 32
	csr_write_allowed(CSR_MHPMCOUNTER3H, &trap, val);
	ಆದರೆ (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3H, &trap);
		ಆದರೆ (trap.cause)
			ಹಿಂತಿರುಗಿ ಅಂಕೆ_ಬಿಟ್ಟುಗಳು;
	}
	ಅಂಕೆ_ಬಿಟ್ಟುಗಳು += sbi_fls(val) + 1;

#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ ಅಂಕೆ_ಬಿಟ್ಟುಗಳು;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hart_detect_features(ರಚನೆ sbi_scratch *scratch, bool cold_boot)
{
	ರಚನೆ sbi_trap_info trap = {0};
	ರಚನೆ sbi_hart_features *hfeatures =
		sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, hart_features_ಸ್ಥಾನಾಂತರ);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val, oldval;
	ಪೂರ್ಣಾಂಕ rc;

	/* If hart features already detected then do nothing */
	ಆದರೆ (hfeatures->detected)
		ಹಿಂತಿರುಗಿ 0;

	/*
	 * Parse device tree extensions early, before any trap-based checks.
	 * Needed to detect Smrnmi and install NMI handlers before CSR probes
	 * that may trigger traps.
	 */
	rc = sbi_platform_extensions_init(sbi_platform_ptr(scratch), cold_boot);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMRNMI)) {
		ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_thishart_ptr();
		ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform_operations *ops = sbi_platform_ops(plat);
		ಬಾಹ್ಯ ಶೂನ್ಯ _trap_rnmi_handler(ಶೂನ್ಯ);
		ಬಾಹ್ಯ ಶೂನ್ಯ _trap_handler(ಶೂನ್ಯ);

		ಆದರೆ (!ops || !ops->smrnmi_handlers_init)
			sbi_panic("Smrnmi detected, but platform lacks smrnmi_handlers_init callback\n");

		/* Reuse _trap_handler for the RNME slot since RNME is taken
		 * as a regular M-mode trap with NMIE=0. */
		ops->smrnmi_handlers_init(_trap_rnmi_handler, _trap_handler);

		/* Initialize MNSCRATCH for the RNMI handler */
		csr_write(CSR_MNSCRATCH, scratch);

		/* Enable NMIs */
		csr_ಗಣ(CSR_MNSTATUS, MNSTATUS_NMIE);
	}

#ವ್ಯಾಖ್ಯಾನಿಸು __check_hpm_csr(__csr, __mask) 					  \
	oldval = csr_read_allowed(__csr, &trap);			  \
	ಆದರೆ (!trap.cause) {						  \
		csr_write_allowed(__csr, &trap, 1UL);			  \
		ಆದರೆ (!trap.cause && csr_ಅದಲುಬದಲು(__csr, oldval) == 1UL) {	  \
			(hfeatures->__mask) |= 1 << (__csr - CSR_MCYCLE); \
		}							  \
	}

#ವ್ಯಾಖ್ಯಾನಿಸು __check_hpm_csr_2(__csr, __mask)	 		  \
	__check_hpm_csr(__csr + 0, __mask)	 		  \
	__check_hpm_csr(__csr + 1, __mask)
#ವ್ಯಾಖ್ಯಾನಿಸು __check_hpm_csr_4(__csr, __mask)	 		  \
	__check_hpm_csr_2(__csr + 0, __mask) 			  \
	__check_hpm_csr_2(__csr + 2, __mask)
#ವ್ಯಾಖ್ಯಾನಿಸು __check_hpm_csr_8(__csr, __mask)	 		  \
	__check_hpm_csr_4(__csr + 0, __mask) 			  \
	__check_hpm_csr_4(__csr + 4, __mask)
#ವ್ಯಾಖ್ಯಾನಿಸು __check_hpm_csr_16(__csr, __mask)	 		  \
	__check_hpm_csr_8(__csr + 0, __mask) 			  \
	__check_hpm_csr_8(__csr + 8, __mask)

#ವ್ಯಾಖ್ಯಾನಿಸು __check_csr(__csr, __rdonly, __wrval, __field, __skip)		\
	oldval = csr_read_allowed(__csr, &trap);			\
	ಆದರೆ (!trap.cause) {						\
		ಆದರೆ (__rdonly) {						\
			(hfeatures->__field)++;				\
		} ಇಲ್ಲದಿದ್ದರೆ {						\
			csr_write_allowed(__csr, &trap, __wrval);	\
			ಆದರೆ (!trap.cause) {				\
				ಆದರೆ ((csr_ಅದಲುಬದಲು(__csr, oldval) & __wrval) == __wrval)	\
					(hfeatures->__field)++;		\
				ಇಲ್ಲದಿದ್ದರೆ					\
					ಹೋಗು __skip;			\
			} ಇಲ್ಲದಿದ್ದರೆ {					\
				ಹೋಗು __skip;				\
			}						\
		}							\
	} ಇಲ್ಲದಿದ್ದರೆ {							\
		ಹೋಗು __skip;						\
	}
#ವ್ಯಾಖ್ಯಾನಿಸು __check_csr_2(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 1, __rdonly, __wrval, __field, __skip)
#ವ್ಯಾಖ್ಯಾನಿಸು __check_csr_4(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 2, __rdonly, __wrval, __field, __skip)
#ವ್ಯಾಖ್ಯಾನಿಸು __check_csr_8(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 4, __rdonly, __wrval, __field, __skip)
#ವ್ಯಾಖ್ಯಾನಿಸು __check_csr_16(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 8, __rdonly, __wrval, __field, __skip)
#ವ್ಯಾಖ್ಯಾನಿಸು __check_csr_32(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 16, __rdonly, __wrval, __field, __skip)
#ವ್ಯಾಖ್ಯಾನಿಸು __check_csr_64(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 32, __rdonly, __wrval, __field, __skip)

	/**
	 * Detect the allowed address bits & granularity. At least PMPADDR0
	 * should be implemented.
	 */
	val = hart_pmp_get_allowed_addr();
	ಆದರೆ (val) {
		hfeatures->pmp_log2gran = sbi_ffs(val) + 2;
		hfeatures->pmp_addr_ಬಿಟ್ಟುಗಳು = sbi_fls(val) + 1;
		/* Detect number of PMP regions. At least PMPADDR0 should be implemented*/
		__check_csr_64(CSR_PMPADDR0, 0, val, pmp_ಎಣಿಕೆ, __pmp_skip);
	}
__pmp_skip:
	/* Detect number of MHPM counters */
	__check_hpm_csr(CSR_MHPMCOUNTER3, mhpm_ಮುಖವಾಡ);
	hfeatures->mhpm_ಬಿಟ್ಟುಗಳು = hart_mhpm_get_allowed_ಬಿಟ್ಟುಗಳು();
	__check_hpm_csr_4(CSR_MHPMCOUNTER4, mhpm_ಮುಖವಾಡ);
	__check_hpm_csr_8(CSR_MHPMCOUNTER8, mhpm_ಮುಖವಾಡ);
	__check_hpm_csr_16(CSR_MHPMCOUNTER16, mhpm_ಮುಖವಾಡ);

	/**
	 * No need to check for MHPMCOUNTERH for RV32 as they are expected to be
	 * implemented if MHPMCOUNTER is implemented.
	 */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __check_csr_64
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __check_csr_32
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __check_csr_16
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __check_csr_8
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __check_csr_4
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __check_csr_2
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __check_csr


#ವ್ಯಾಖ್ಯಾನಿಸು __check_priv(__csr, __base_priv, __priv)			\
	val = csr_read_allowed(__csr, &trap);				\
	ಆದರೆ (!trap.cause && (hfeatures->priv_ಆವೃತ್ತಿ >= __base_priv)) {	\
		hfeatures->priv_ಆವೃತ್ತಿ = __priv;			\
	}

	/* Detect if hart supports Priv v1.10 */
	__check_priv(CSR_MCOUNTEREN,
		     SBI_HART_PRIV_VER_UNKNOWN, SBI_HART_PRIV_VER_1_10);
	/* Detect if hart supports Priv v1.11 */
	__check_priv(CSR_MCOUNTINHIBIT,
		     SBI_HART_PRIV_VER_1_10, SBI_HART_PRIV_VER_1_11);
	/* Detect if hart supports Priv v1.12 */
	__check_priv(CSR_MENVCFG,
		     SBI_HART_PRIV_VER_1_11, SBI_HART_PRIV_VER_1_12);

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __check_priv_csr

#ವ್ಯಾಖ್ಯಾನಿಸು __check_ext_csr(__base_priv, __csr, __ext)			\
	ಆದರೆ (hfeatures->priv_ಆವೃತ್ತಿ >= __base_priv) {			\
		csr_read_allowed(__csr, &trap);				\
		ಆದರೆ (!trap.cause)					\
			__sbi_hart_update_extension(hfeatures,		\
						    __ext, true);	\
	}

	/* Counter overflow/filtering is not useful without mcounter/inhibit */
	/* Detect if hart supports sscofpmf */
	__check_ext_csr(SBI_HART_PRIV_VER_1_11,
			CSR_SCOUNTOVF, SBI_HART_EXT_SSCOFPMF);
	/* Detect if hart has AIA local interrupt CSRs */
	__check_ext_csr(SBI_HART_PRIV_VER_UNKNOWN,
			CSR_MTOPI, SBI_HART_EXT_SMAIA);
	/* Detect if hart supports stimecmp CSR(Sstc extension) */
	__check_ext_csr(SBI_HART_PRIV_VER_1_12,
			CSR_STIMECMP, SBI_HART_EXT_SSTC);
	/* Detect if hart supports mstateen CSRs */
	__check_ext_csr(SBI_HART_PRIV_VER_1_12,
			CSR_MSTATEEN0, SBI_HART_EXT_SMSTATEEN);
	/* Detect if hart supports sstateen CSRs */
	__check_ext_csr(SBI_HART_PRIV_VER_1_12,
			CSR_SSTATEEN0, SBI_HART_EXT_SSSTATEEN);
	/* Detect if hart supports smcntrpmf */
	__check_ext_csr(SBI_HART_PRIV_VER_1_12,
			CSR_MCYCLECFG, SBI_HART_EXT_SMCNTRPMF);
	/* Detect if hart support sdtrig (debug triggers) */
	__check_ext_csr(SBI_HART_PRIV_VER_UNKNOWN,
			CSR_TSELECT, SBI_HART_EXT_SDTRIG);

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __check_ext_csr

#ವ್ಯಾಖ್ಯಾನಿಸು __check_csr_existence(__csr, __csr_id)				\
	csr_read_allowed(__csr, &trap);					\
	ಆದರೆ (!trap.cause)						\
		__set_bit(__csr_id, hfeatures->csrs);

	__check_csr_existence(CSR_ಚಕ್ರ, SBI_HART_CSR_ಚಕ್ರ);
	__check_csr_existence(CSR_ಸಮಯ, SBI_HART_CSR_ಸಮಯ);
	__check_csr_existence(CSR_INSTRET, SBI_HART_CSR_INSTRET);

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __check_csr_existence

	/* Zicntr should only be detected using traps */
	__sbi_hart_update_extension(hfeatures, SBI_HART_EXT_ZICNTR,
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_ಚಕ್ರ) &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_ಸಮಯ)  &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET));

	/* Extensions implied by other extensions and features */
	ಆದರೆ (hfeatures->mhpm_ಮುಖವಾಡ)
		__sbi_hart_update_extension(hfeatures,
					SBI_HART_EXT_ZIHPM, true);

	/* Mark hart feature detection done */
	hfeatures->detected = true;

	/*
	 * On platforms with Smepmp, the previous booting stage must
	 * enter OpenSBI with mseccfg.MML == 0. This allows OpenSBI
	 * to configure it's own M-mode only regions without depending
	 * on the previous booting stage.
	 */
	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP) &&
	    (csr_read(CSR_MSECCFG) & MSECCFG_MML))
		ಹಿಂತಿರುಗಿ SBI_EILL;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_hart_reinit(ರಚನೆ sbi_scratch *scratch)
{
	ಪೂರ್ಣಾಂಕ rc;

	mstatus_init(scratch);

	rc = fp_init(scratch);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_hart_init(ರಚನೆ sbi_scratch *scratch, bool cold_boot)
{
	ಪೂರ್ಣಾಂಕ rc;

	/*
	 * Clear mip CSR before proceeding with init to avoid any spurious
	 * external interrupts in S-mode.
	 */
	csr_write(CSR_MIP, 0);

	ಆದರೆ (cold_boot) {
		ಆದರೆ (misa_extension('H'))
			sbi_hart_expected_trap = &__sbi_expected_trap_hext;

		hart_features_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(
					ಗಾತ್ರ(ರಚನೆ sbi_hart_features));
		ಆದರೆ (!hart_features_ಸ್ಥಾನಾಂತರ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	rc = hart_detect_features(scratch, cold_boot);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಆದರೆ (cold_boot) {
		rc = sbi_hart_pmp_init(scratch);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ sbi_hart_reinit(scratch);
}

ಶೂನ್ಯ __attribute__((noreturn)) sbi_hart_hang(ಶೂನ್ಯ)
{
	ಆಗ (1)
		wfi();
	__builtin_unreachable();
}

ಶೂನ್ಯ __attribute__((noreturn))
sbi_hart_switch_ಬಹುಲಕ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg0, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg1,
		     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಂದಿನ_addr, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಂದಿನ_ಬಹುಲಕ,
		     bool ಮುಂದಿನ_virt)
{
#ಆದರೆ __riscv_xlen == 32
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val, valH;
#ಇಲ್ಲದಿದ್ದರೆ
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val;
#ಆದರೆ_ಅಂತ್ಯ

	ಆಯ್ಕೆ (ಮುಂದಿನ_ಬಹುಲಕ) {
	ಸ್ಥಿತಿ PRV_M:
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRV_S:
		ಆದರೆ (!misa_extension('S'))
			sbi_hart_hang();
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRV_U:
		ಆದರೆ (!misa_extension('U'))
			sbi_hart_hang();
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		sbi_hart_hang();
	}

	val = csr_read(CSR_MSTATUS);
	val = ಸೇರಿಸು_ಕ್ಷೇತ್ರ(val, MSTATUS_MPP, ಮುಂದಿನ_ಬಹುಲಕ);
	val = ಸೇರಿಸು_ಕ್ಷೇತ್ರ(val, MSTATUS_MPIE, 0);
#ಆದರೆ __riscv_xlen == 32
	ಆದರೆ (misa_extension('H')) {
		valH = csr_read(CSR_MSTATUSH);
		valH = ಸೇರಿಸು_ಕ್ಷೇತ್ರ(valH, MSTATUSH_MPV, ಮುಂದಿನ_virt);
		csr_write(CSR_MSTATUSH, valH);
	}
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (misa_extension('H'))
		val = ಸೇರಿಸು_ಕ್ಷೇತ್ರ(val, MSTATUS_MPV, ಮುಂದಿನ_virt);
#ಆದರೆ_ಅಂತ್ಯ
	csr_write(CSR_MSTATUS, val);
	csr_write(CSR_MEPC, ಮುಂದಿನ_addr);

	ಆದರೆ (ಮುಂದಿನ_ಬಹುಲಕ == PRV_S) {
		ಆದರೆ (ಮುಂದಿನ_virt) {
			csr_write(CSR_VSTVEC, ಮುಂದಿನ_addr);
			csr_write(CSR_VSSCRATCH, 0);
			csr_write(CSR_VSIE, 0);
			csr_write(CSR_VSATP, 0);
		} ಇಲ್ಲದಿದ್ದರೆ {
			csr_write(CSR_STVEC, ಮುಂದಿನ_addr);
			csr_write(CSR_SSCRATCH, 0);
			csr_write(CSR_SIE, 0);
			csr_write(CSR_SATP, 0);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮುಂದಿನ_ಬಹುಲಕ == PRV_U) {
		ಆದರೆ (misa_extension('N')) {
			csr_write(CSR_UTVEC, ಮುಂದಿನ_addr);
			csr_write(CSR_USCRATCH, 0);
			csr_write(CSR_UIE, 0);
		}
	}

	ನೋಂದಣಿ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a0 asm("a0") = arg0;
	ನೋಂದಣಿ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a1 asm("a1") = arg1;
	__asm__ __volatile__("mret" : : "r"(a0), "r"(a1));
	__builtin_unreachable();
}
