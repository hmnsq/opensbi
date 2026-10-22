/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_barrier.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_fp.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_csr_detect.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart_pmp.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>

ਬਾਹਰੀ ਖਾਲੀ __sbi_expected_trap(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ __sbi_expected_trap_hext(ਖਾਲੀ);

ਖਾਲੀ (*sbi_hart_expected_trap)(ਖਾਲੀ) = &__sbi_expected_trap;

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hart_features_ਵਿੱਥ;

ਸਥਿਰ ਖਾਲੀ mstatus_init(ਬਣਤਰ sbi_scratch *scratch)
{
	ਪੂਰਨ cidx;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mstatus_val = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ mhpm_ਨਕਾਬ = sbi_hart_mhpm_ਨਕਾਬ(scratch);
	uint64_t mhpmevent_init_val = 0;
	uint64_t menvcfg_val, mstateen_val;

	/* Enable FPU */
	ਜੇ (misa_extension('D') || misa_extension('F'))
		mstatus_val |=  MSTATUS_FS;

	/* Enable Vector context */
	ਜੇ (misa_extension('V'))
		mstatus_val |=  MSTATUS_VS;

	csr_write(CSR_MSTATUS, mstatus_val);

	/* Disable user mode usage of all perf counters except TM */
	ਜੇ (misa_extension('S') &&
	    sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_SCOUNTEREN, 0x02);

	/**
	 * OpenSBI doesn't use any PMU counters in M-mode.
	 * Supervisor mode usage for all counters are enabled by default
	 * But counters will not run until mcountinhibit is set.
	 */
	ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	/* All programmable counters will start running at runtime after S-mode request */
	ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	/**
	 * The mhpmeventn[h] CSR should be initialized with interrupt disabled
	 * and inhibited running in M-mode during init.
	 */
	mhpmevent_init_val |= (MHPMEVENT_ਦਾ | MHPMEVENT_MINH);
	ਲਈ (cidx = 0; cidx <= 28; cidx++) {
		ਜੇ (!(mhpm_ਨਕਾਬ & 1UL << (cidx + 3)))
			ਜਾਰੀ;
#ਜੇ __riscv_xlen == 32
		csr_write_ਅੰਕ(CSR_MHPMEVENT3 + cidx,
			       mhpmevent_init_val & 0xFFFFFFFF);
		ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
			csr_write_ਅੰਕ(CSR_MHPMEVENT3H + cidx,
				      mhpmevent_init_val >> ਬਿੱਟ_ਪ੍ਰਤੀ_LONG);
#ਨਹੀਂਤਾਂ
		csr_write_ਅੰਕ(CSR_MHPMEVENT3 + cidx, mhpmevent_init_val);
#ਜੇ_ਅੰਤ
	}

	ਜੇ (misa_extension('H'))
		csr_write(CSR_HSTATUS, 0);

	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMSTATEEN)) {
		mstateen_val = 0;
		mstateen_val |= SMSTATEEN_STATEN;
		mstateen_val |= SMSTATEEN0_CONTEXT;
		mstateen_val |= SMSTATEEN0_HSENVCFG;

		ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA))
			mstateen_val |= (SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);
		ਨਹੀਂਤਾਂ
			mstateen_val &= ~(SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);

		ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA) ||
		    sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCSRIND))
			mstateen_val |= (SMSTATEEN0_SVSLCT);
		ਨਹੀਂਤਾਂ
			mstateen_val &= ~(SMSTATEEN0_SVSLCT);

		ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCTR))
			mstateen_val |= SMSTATEEN0_CTR;
		ਨਹੀਂਤਾਂ
			mstateen_val &= ~SMSTATEEN0_CTR;

		ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
			mstateen_val |= SMSTATEEN0_SRMCFG;
		ਨਹੀਂਤਾਂ
			mstateen_val &= ~SMSTATEEN0_SRMCFG;

		csr_write64(CSR_MSTATEEN0, mstateen_val);
		csr_write64(CSR_MSTATEEN1, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN2, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN3, SMSTATEEN_STATEN);
	}

	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSSTATEEN)) {
		ਜੇ (misa_extension('S')) {
			csr_write(CSR_SSTATEEN0, 0);
			csr_write(CSR_SSTATEEN1, 0);
			csr_write(CSR_SSTATEEN2, 0);
			csr_write(CSR_SSTATEEN3, 0);
		}
		ਜੇ (misa_extension('H')) {
			csr_write64(CSR_HSTATEEN0, (uint64_t)0);
			csr_write64(CSR_HSTATEEN1, (uint64_t)0);
			csr_write64(CSR_HSTATEEN2, (uint64_t)0);
			csr_write64(CSR_HSTATEEN3, (uint64_t)0);
		}
	}

	ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_12) {
		menvcfg_val = csr_read64(CSR_MENVCFG);

		/* Disable HW A/D updating by default */
		menvcfg_val &= ~ENVCFG_ADUE;

		/* Disable double trap by default */
		menvcfg_val &= ~ENVCFG_DTE;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __set_menvcfg_ext(__ext, __bits)				\
		ਜੇ (sbi_hart_has_extension(scratch, __ext))		\
			menvcfg_val |= __bits;

		/*
		 * Enable access to extensions if they are present in the
		 * hardware or in the device tree.
		 */

		__set_menvcfg_ext(SBI_HART_EXT_ZICBOZ, ENVCFG_CBZE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM, ENVCFG_CBCFE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM,
				  ENVCFG_CBIE_INV << ENVCFG_CBIE_ਖਿਸਕਾਓ)
#ਜੇ __riscv_xlen > 32
		__set_menvcfg_ext(SBI_HART_EXT_SVPBMT, ENVCFG_PBMTE)
#ਜੇ_ਅੰਤ
		__set_menvcfg_ext(SBI_HART_EXT_SSTC, ENVCFG_STCE)
		__set_menvcfg_ext(SBI_HART_EXT_SMCDELEG, ENVCFG_CDE);

		/*
		 * Assume only Svadu is supported when it is the only extension
		 * present in the ISA string. Svade is assumed when neither are
		 * present. When both are present we must default to Svade (see
		 * the zero reset value of FWFT.PTE_AD_HW_UPDATING).
		 */
		ਜੇ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SVADE))
			__set_menvcfg_ext(SBI_HART_EXT_SVADU, ENVCFG_ADUE);

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __set_menvcfg_ext

		csr_write64(CSR_MENVCFG, menvcfg_val);

		/* Enable S-mode access to seed CSR */
		ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
			csr_ਸੈੱਟ(CSR_MSECCFG, MSECCFG_SSEED);
			csr_clear(CSR_MSECCFG, MSECCFG_USEED);
		}
	}

	/* Disable all interrupts */
	csr_write(CSR_MIE, 0);

	/* Disable S-mode paging */
	ਜੇ (misa_extension('S'))
		csr_write(CSR_SATP, 0);
}

ਸਥਿਰ ਪੂਰਨ fp_init(ਬਣਤਰ sbi_scratch *scratch)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __riscv_flen
	ਪੂਰਨ i;
#ਜੇ_ਅੰਤ

	ਜੇ (!misa_extension('D') && !misa_extension('F'))
		ਵਾਪਸ 0;

	ਜੇ (!(csr_read(CSR_MSTATUS) & MSTATUS_FS))
		ਵਾਪਸ SBI_EINVAL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __riscv_flen
	ਲਈ (i = 0; i < 32; i++)
		init_fp_reg(i);
	csr_write(CSR_FCSR, 0);
#ਜੇ_ਅੰਤ

	ਵਾਪਸ 0;
}

ਖਾਲੀ sbi_hart_delegation_dump(ਬਣਤਰ sbi_scratch *scratch,
			      ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *suffix)
{
	ਜੇ (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist*/
		ਵਾਪਸ;

	sbi_printf("%sMIDELEG%s: 0x%" PRILX "\n",
		   ਅਗੇਤਰ, suffix, csr_read(CSR_MIDELEG));
	sbi_printf("%sMEDELEG%s: 0x%" PRILX "\n",
		   ਅਗੇਤਰ, suffix, csr_read(CSR_MEDELEG));
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ sbi_hart_mhpm_ਨਕਾਬ(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_hart_features *hfeatures =
			sbi_scratch_ਵਿੱਥ_ptr(scratch, hart_features_ਵਿੱਥ);

	ਵਾਪਸ hfeatures->mhpm_ਨਕਾਬ;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ sbi_hart_mhpm_ਬਿੱਟ(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_hart_features *hfeatures =
			sbi_scratch_ਵਿੱਥ_ptr(scratch, hart_features_ਵਿੱਥ);

	ਵਾਪਸ hfeatures->mhpm_ਬਿੱਟ;
}

ਪੂਰਨ sbi_hart_priv_ਸੰਸਕਰਣ(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_hart_features *hfeatures =
			sbi_scratch_ਵਿੱਥ_ptr(scratch, hart_features_ਵਿੱਥ);

	ਵਾਪਸ hfeatures->priv_ਸੰਸਕਰਣ;
}

ਖਾਲੀ sbi_hart_get_priv_ਸੰਸਕਰਣ_str(ਬਣਤਰ sbi_scratch *scratch,
				   ਅੱਖਰ *ਸੰਸਕਰਣ_str, ਪੂਰਨ nvstr)
{
	ਅੱਖਰ *ਆਰਜ਼ੀ;
	ਬਣਤਰ sbi_hart_features *hfeatures =
			sbi_scratch_ਵਿੱਥ_ptr(scratch, hart_features_ਵਿੱਥ);

	ਚੋਣ (hfeatures->priv_ਸੰਸਕਰਣ) {
	ਹਾਲਤ SBI_HART_PRIV_VER_1_10:
		ਆਰਜ਼ੀ = "v1.10";
		ਰੋਕੋ;
	ਹਾਲਤ SBI_HART_PRIV_VER_1_11:
		ਆਰਜ਼ੀ = "v1.11";
		ਰੋਕੋ;
	ਹਾਲਤ SBI_HART_PRIV_VER_1_12:
		ਆਰਜ਼ੀ = "v1.12";
		ਰੋਕੋ;
	ਮੂਲ:
		ਆਰਜ਼ੀ = "unknown";
		ਰੋਕੋ;
	}

	sbi_snprintf(ਸੰਸਕਰਣ_str, nvstr, "%s", ਆਰਜ਼ੀ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ __sbi_hart_update_extension(
					ਬਣਤਰ sbi_hart_features *hfeatures,
					ਗਿਣਤੀ sbi_hart_extensions ext,
					bool enable)
{
	ਜੇ (enable)
		__set_bit(ext, hfeatures->extensions);
	ਨਹੀਂਤਾਂ
		__clear_bit(ext, hfeatures->extensions);
}

/**
 * Enable/Disable a particular hart extension
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @param enable new state of hart extension
 */
ਖਾਲੀ sbi_hart_update_extension(ਬਣਤਰ sbi_scratch *scratch,
			       ਗਿਣਤੀ sbi_hart_extensions ext,
			       bool enable)
{
	ਬਣਤਰ sbi_hart_features *hfeatures =
			sbi_scratch_ਵਿੱਥ_ptr(scratch, hart_features_ਵਿੱਥ);

	__sbi_hart_update_extension(hfeatures, ext, enable);
}

/**
 * Check whether a particular hart extension is available
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @returns true (available) or false (not available)
 */
bool sbi_hart_has_extension(ਬਣਤਰ sbi_scratch *scratch,
			    ਗਿਣਤੀ sbi_hart_extensions ext)
{
	ਬਣਤਰ sbi_hart_features *hfeatures =
			sbi_scratch_ਵਿੱਥ_ptr(scratch, hart_features_ਵਿੱਥ);

	ਜੇ (__test_bit(ext, hfeatures->extensions))
		ਵਾਪਸ true;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ false;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __SBI_HART_EXT_DATA(_name, _id) {	\
	.name = #_name,				\
	.id = _id,				\
}

ਅਚਲ ਬਣਤਰ sbi_hart_ext_ਅੰਕੜੇ sbi_hart_ext[] = {
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

_Static_assert(SBI_HART_EXT_MAX == array_ਮਾਪ(sbi_hart_ext),
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
ਖਾਲੀ sbi_hart_get_extensions_str(ਬਣਤਰ sbi_scratch *scratch,
				 ਅੱਖਰ *extensions_str, ਪੂਰਨ nestr)
{
	ਬਣਤਰ sbi_hart_features *hfeatures =
			sbi_scratch_ਵਿੱਥ_ptr(scratch, hart_features_ਵਿੱਥ);
	ਪੂਰਨ ਵਿੱਥ = 0, ext = 0;

	ਜੇ (!extensions_str || nestr <= 0)
		ਵਾਪਸ;
	sbi_memset(extensions_str, 0, nestr);

	for_each_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ext, hfeatures->extensions, SBI_HART_EXT_MAX) {
		ਜੇ (ਵਿੱਥ + sbi_strlen(sbi_hart_ext[ext].name) + 1 > nestr) {
			sbi_printf("%s:extension name is longer than buffer (error)\n", __func__);
			ਰੋਕੋ;
		}
		sbi_snprintf(extensions_str + ਵਿੱਥ,
				 nestr - ਵਿੱਥ,
				 "%s,", sbi_hart_ext[ext].name);
		ਵਿੱਥ = ਵਿੱਥ + sbi_strlen(sbi_hart_ext[ext].name) + 1;
	}

	ਜੇ (ਵਿੱਥ)
		extensions_str[ਵਿੱਥ - 1] = '\0';
	ਨਹੀਂਤਾਂ
		sbi_strncpy(extensions_str, "none", nestr);
}

/**
 * Check whether a particular CSR is present on the HART
 *
 * @param scratch pointer to the HART scratch space
 * @param csr the CSR number to check
 */
bool sbi_hart_has_csr(ਬਣਤਰ sbi_scratch *scratch, ਗਿਣਤੀ sbi_hart_csrs csr)
{
	ਬਣਤਰ sbi_hart_features *hfeatures =
			sbi_scratch_ਵਿੱਥ_ptr(scratch, hart_features_ਵਿੱਥ);

	ਵਾਪਸ __test_bit(csr, hfeatures->csrs);
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hart_pmp_get_allowed_addr(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val = 0;
	ਬਣਤਰ sbi_trap_info trap = {0};

	csr_write_allowed(CSR_PMPCFG0, &trap, 0);
	ਜੇ (trap.cause)
		ਵਾਪਸ 0;

	csr_write_allowed(CSR_PMPADDR0, &trap, PMP_ADDR_ਨਕਾਬ);
	ਜੇ (!trap.cause) {
		val = csr_read_allowed(CSR_PMPADDR0, &trap);
		ਜੇ (trap.cause)
			val = 0;
	}

	ਵਾਪਸ val;
}

ਸਥਿਰ ਪੂਰਨ hart_mhpm_get_allowed_ਬਿੱਟ(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val = ~(0UL);
	ਬਣਤਰ sbi_trap_info trap = {0};
	ਪੂਰਨ ਅੰਕ_ਬਿੱਟ = 0;

	/**
	 * It is assumed that platforms will implement same number of bits for
	 * all the performance counters including mcycle/minstret.
	 */
	csr_write_allowed(CSR_MHPMCOUNTER3, &trap, val);
	ਜੇ (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3, &trap);
		ਜੇ (trap.cause)
			ਵਾਪਸ 0;
	}
	ਅੰਕ_ਬਿੱਟ = sbi_fls(val) + 1;
#ਜੇ __riscv_xlen == 32
	csr_write_allowed(CSR_MHPMCOUNTER3H, &trap, val);
	ਜੇ (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3H, &trap);
		ਜੇ (trap.cause)
			ਵਾਪਸ ਅੰਕ_ਬਿੱਟ;
	}
	ਅੰਕ_ਬਿੱਟ += sbi_fls(val) + 1;

#ਜੇ_ਅੰਤ

	ਵਾਪਸ ਅੰਕ_ਬਿੱਟ;
}

ਸਥਿਰ ਪੂਰਨ hart_detect_features(ਬਣਤਰ sbi_scratch *scratch, bool cold_boot)
{
	ਬਣਤਰ sbi_trap_info trap = {0};
	ਬਣਤਰ sbi_hart_features *hfeatures =
		sbi_scratch_ਵਿੱਥ_ptr(scratch, hart_features_ਵਿੱਥ);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val, oldval;
	ਪੂਰਨ rc;

	/* If hart features already detected then do nothing */
	ਜੇ (hfeatures->detected)
		ਵਾਪਸ 0;

	/*
	 * Parse device tree extensions early, before any trap-based checks.
	 * Needed to detect Smrnmi and install NMI handlers before CSR probes
	 * that may trigger traps.
	 */
	rc = sbi_platform_extensions_init(sbi_platform_ptr(scratch), cold_boot);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMRNMI)) {
		ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_thishart_ptr();
		ਅਚਲ ਬਣਤਰ sbi_platform_operations *ops = sbi_platform_ops(plat);
		ਬਾਹਰੀ ਖਾਲੀ _trap_rnmi_handler(ਖਾਲੀ);
		ਬਾਹਰੀ ਖਾਲੀ _trap_handler(ਖਾਲੀ);

		ਜੇ (!ops || !ops->smrnmi_handlers_init)
			sbi_panic("Smrnmi detected, but platform lacks smrnmi_handlers_init callback\n");

		/* Reuse _trap_handler for the RNME slot since RNME is taken
		 * as a regular M-mode trap with NMIE=0. */
		ops->smrnmi_handlers_init(_trap_rnmi_handler, _trap_handler);

		/* Initialize MNSCRATCH for the RNMI handler */
		csr_write(CSR_MNSCRATCH, scratch);

		/* Enable NMIs */
		csr_ਸੈੱਟ(CSR_MNSTATUS, MNSTATUS_NMIE);
	}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_hpm_csr(__csr, __mask) 					  \
	oldval = csr_read_allowed(__csr, &trap);			  \
	ਜੇ (!trap.cause) {						  \
		csr_write_allowed(__csr, &trap, 1UL);			  \
		ਜੇ (!trap.cause && csr_ਅਦਲਾ_ਬਦਲੀ(__csr, oldval) == 1UL) {	  \
			(hfeatures->__mask) |= 1 << (__csr - CSR_MCYCLE); \
		}							  \
	}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_hpm_csr_2(__csr, __mask)	 		  \
	__check_hpm_csr(__csr + 0, __mask)	 		  \
	__check_hpm_csr(__csr + 1, __mask)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_hpm_csr_4(__csr, __mask)	 		  \
	__check_hpm_csr_2(__csr + 0, __mask) 			  \
	__check_hpm_csr_2(__csr + 2, __mask)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_hpm_csr_8(__csr, __mask)	 		  \
	__check_hpm_csr_4(__csr + 0, __mask) 			  \
	__check_hpm_csr_4(__csr + 4, __mask)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_hpm_csr_16(__csr, __mask)	 		  \
	__check_hpm_csr_8(__csr + 0, __mask) 			  \
	__check_hpm_csr_8(__csr + 8, __mask)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_csr(__csr, __rdonly, __wrval, __field, __skip)		\
	oldval = csr_read_allowed(__csr, &trap);			\
	ਜੇ (!trap.cause) {						\
		ਜੇ (__rdonly) {						\
			(hfeatures->__field)++;				\
		} ਨਹੀਂਤਾਂ {						\
			csr_write_allowed(__csr, &trap, __wrval);	\
			ਜੇ (!trap.cause) {				\
				ਜੇ ((csr_ਅਦਲਾ_ਬਦਲੀ(__csr, oldval) & __wrval) == __wrval)	\
					(hfeatures->__field)++;		\
				ਨਹੀਂਤਾਂ					\
					ਜਾਓ __skip;			\
			} ਨਹੀਂਤਾਂ {					\
				ਜਾਓ __skip;				\
			}						\
		}							\
	} ਨਹੀਂਤਾਂ {							\
		ਜਾਓ __skip;						\
	}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_csr_2(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 1, __rdonly, __wrval, __field, __skip)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_csr_4(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 2, __rdonly, __wrval, __field, __skip)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_csr_8(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 4, __rdonly, __wrval, __field, __skip)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_csr_16(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 8, __rdonly, __wrval, __field, __skip)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_csr_32(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 16, __rdonly, __wrval, __field, __skip)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_csr_64(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 32, __rdonly, __wrval, __field, __skip)

	/**
	 * Detect the allowed address bits & granularity. At least PMPADDR0
	 * should be implemented.
	 */
	val = hart_pmp_get_allowed_addr();
	ਜੇ (val) {
		hfeatures->pmp_log2gran = sbi_ffs(val) + 2;
		hfeatures->pmp_addr_ਬਿੱਟ = sbi_fls(val) + 1;
		/* Detect number of PMP regions. At least PMPADDR0 should be implemented*/
		__check_csr_64(CSR_PMPADDR0, 0, val, pmp_ਗਿਣਤੀ, __pmp_skip);
	}
__pmp_skip:
	/* Detect number of MHPM counters */
	__check_hpm_csr(CSR_MHPMCOUNTER3, mhpm_ਨਕਾਬ);
	hfeatures->mhpm_ਬਿੱਟ = hart_mhpm_get_allowed_ਬਿੱਟ();
	__check_hpm_csr_4(CSR_MHPMCOUNTER4, mhpm_ਨਕਾਬ);
	__check_hpm_csr_8(CSR_MHPMCOUNTER8, mhpm_ਨਕਾਬ);
	__check_hpm_csr_16(CSR_MHPMCOUNTER16, mhpm_ਨਕਾਬ);

	/**
	 * No need to check for MHPMCOUNTERH for RV32 as they are expected to be
	 * implemented if MHPMCOUNTER is implemented.
	 */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __check_csr_64
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __check_csr_32
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __check_csr_16
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __check_csr_8
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __check_csr_4
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __check_csr_2
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __check_csr


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_priv(__csr, __base_priv, __priv)			\
	val = csr_read_allowed(__csr, &trap);				\
	ਜੇ (!trap.cause && (hfeatures->priv_ਸੰਸਕਰਣ >= __base_priv)) {	\
		hfeatures->priv_ਸੰਸਕਰਣ = __priv;			\
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

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __check_priv_csr

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_ext_csr(__base_priv, __csr, __ext)			\
	ਜੇ (hfeatures->priv_ਸੰਸਕਰਣ >= __base_priv) {			\
		csr_read_allowed(__csr, &trap);				\
		ਜੇ (!trap.cause)					\
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

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __check_ext_csr

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __check_csr_existence(__csr, __csr_id)				\
	csr_read_allowed(__csr, &trap);					\
	ਜੇ (!trap.cause)						\
		__set_bit(__csr_id, hfeatures->csrs);

	__check_csr_existence(CSR_ਚੱਕਰ, SBI_HART_CSR_ਚੱਕਰ);
	__check_csr_existence(CSR_ਸਮਾਂ, SBI_HART_CSR_ਸਮਾਂ);
	__check_csr_existence(CSR_INSTRET, SBI_HART_CSR_INSTRET);

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __check_csr_existence

	/* Zicntr should only be detected using traps */
	__sbi_hart_update_extension(hfeatures, SBI_HART_EXT_ZICNTR,
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_ਚੱਕਰ) &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_ਸਮਾਂ)  &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET));

	/* Extensions implied by other extensions and features */
	ਜੇ (hfeatures->mhpm_ਨਕਾਬ)
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
	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP) &&
	    (csr_read(CSR_MSECCFG) & MSECCFG_MML))
		ਵਾਪਸ SBI_EILL;

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_hart_reinit(ਬਣਤਰ sbi_scratch *scratch)
{
	ਪੂਰਨ rc;

	mstatus_init(scratch);

	rc = fp_init(scratch);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_hart_init(ਬਣਤਰ sbi_scratch *scratch, bool cold_boot)
{
	ਪੂਰਨ rc;

	/*
	 * Clear mip CSR before proceeding with init to avoid any spurious
	 * external interrupts in S-mode.
	 */
	csr_write(CSR_MIP, 0);

	ਜੇ (cold_boot) {
		ਜੇ (misa_extension('H'))
			sbi_hart_expected_trap = &__sbi_expected_trap_hext;

		hart_features_ਵਿੱਥ = sbi_scratch_alloc_ਵਿੱਥ(
					ਆਕਾਰ(ਬਣਤਰ sbi_hart_features));
		ਜੇ (!hart_features_ਵਿੱਥ)
			ਵਾਪਸ SBI_ENOMEM;
	}

	rc = hart_detect_features(scratch, cold_boot);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਜੇ (cold_boot) {
		rc = sbi_hart_pmp_init(scratch);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	ਵਾਪਸ sbi_hart_reinit(scratch);
}

ਖਾਲੀ __attribute__((noreturn)) sbi_hart_hang(ਖਾਲੀ)
{
	ਜਦੋਂ (1)
		wfi();
	__builtin_unreachable();
}

ਖਾਲੀ __attribute__((noreturn))
sbi_hart_switch_ਬਹੁਲਕ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg0, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg1,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਗਲਾ_addr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਗਲਾ_ਬਹੁਲਕ,
		     bool ਅਗਲਾ_virt)
{
#ਜੇ __riscv_xlen == 32
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val, valH;
#ਨਹੀਂਤਾਂ
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val;
#ਜੇ_ਅੰਤ

	ਚੋਣ (ਅਗਲਾ_ਬਹੁਲਕ) {
	ਹਾਲਤ PRV_M:
		ਰੋਕੋ;
	ਹਾਲਤ PRV_S:
		ਜੇ (!misa_extension('S'))
			sbi_hart_hang();
		ਰੋਕੋ;
	ਹਾਲਤ PRV_U:
		ਜੇ (!misa_extension('U'))
			sbi_hart_hang();
		ਰੋਕੋ;
	ਮੂਲ:
		sbi_hart_hang();
	}

	val = csr_read(CSR_MSTATUS);
	val = ਪਾਓ_ਖੇਤਰ(val, MSTATUS_MPP, ਅਗਲਾ_ਬਹੁਲਕ);
	val = ਪਾਓ_ਖੇਤਰ(val, MSTATUS_MPIE, 0);
#ਜੇ __riscv_xlen == 32
	ਜੇ (misa_extension('H')) {
		valH = csr_read(CSR_MSTATUSH);
		valH = ਪਾਓ_ਖੇਤਰ(valH, MSTATUSH_MPV, ਅਗਲਾ_virt);
		csr_write(CSR_MSTATUSH, valH);
	}
#ਨਹੀਂਤਾਂ
	ਜੇ (misa_extension('H'))
		val = ਪਾਓ_ਖੇਤਰ(val, MSTATUS_MPV, ਅਗਲਾ_virt);
#ਜੇ_ਅੰਤ
	csr_write(CSR_MSTATUS, val);
	csr_write(CSR_MEPC, ਅਗਲਾ_addr);

	ਜੇ (ਅਗਲਾ_ਬਹੁਲਕ == PRV_S) {
		ਜੇ (ਅਗਲਾ_virt) {
			csr_write(CSR_VSTVEC, ਅਗਲਾ_addr);
			csr_write(CSR_VSSCRATCH, 0);
			csr_write(CSR_VSIE, 0);
			csr_write(CSR_VSATP, 0);
		} ਨਹੀਂਤਾਂ {
			csr_write(CSR_STVEC, ਅਗਲਾ_addr);
			csr_write(CSR_SSCRATCH, 0);
			csr_write(CSR_SIE, 0);
			csr_write(CSR_SATP, 0);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (ਅਗਲਾ_ਬਹੁਲਕ == PRV_U) {
		ਜੇ (misa_extension('N')) {
			csr_write(CSR_UTVEC, ਅਗਲਾ_addr);
			csr_write(CSR_USCRATCH, 0);
			csr_write(CSR_UIE, 0);
		}
	}

	ਰਜਿਸਟਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a0 asm("a0") = arg0;
	ਰਜਿਸਟਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a1 asm("a1") = arg1;
	__asm__ __volatile__("mret" : : "r"(a0), "r"(a1));
	__builtin_unreachable();
}
