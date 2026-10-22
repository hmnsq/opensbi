/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_barrier.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/riscv_fp.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_csr_detect.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_hart_pmp.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_pmu.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_trap.h>

బాహ్య శూన్య __sbi_expected_trap(శూన్య);
బాహ్య శూన్య __sbi_expected_trap_hext(శూన్య);

శూన్య (*sbi_hart_expected_trap)(శూన్య) = &__sbi_expected_trap;

గుర్తులేని దీర్ఘ hart_features_స్థానభ్రంశం;

స్థిర శూన్య mstatus_init(నిర్మాణం sbi_scratch *scratch)
{
	పూర్ణాంకం cidx;
	గుర్తులేని దీర్ఘ mstatus_val = 0;
	గుర్తులేని పూర్ణాంకం mhpm_ముసుగు = sbi_hart_mhpm_ముసుగు(scratch);
	uint64_t mhpmevent_init_val = 0;
	uint64_t menvcfg_val, mstateen_val;

	/* Enable FPU */
	ఒకవేళ (misa_extension('D') || misa_extension('F'))
		mstatus_val |=  MSTATUS_FS;

	/* Enable Vector context */
	ఒకవేళ (misa_extension('V'))
		mstatus_val |=  MSTATUS_VS;

	csr_write(CSR_MSTATUS, mstatus_val);

	/* Disable user mode usage of all perf counters except TM */
	ఒకవేళ (misa_extension('S') &&
	    sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_SCOUNTEREN, 0x02);

	/**
	 * OpenSBI doesn't use any PMU counters in M-mode.
	 * Supervisor mode usage for all counters are enabled by default
	 * But counters will not run until mcountinhibit is set.
	 */
	ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	/* All programmable counters will start running at runtime after S-mode request */
	ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	/**
	 * The mhpmeventn[h] CSR should be initialized with interrupt disabled
	 * and inhibited running in M-mode during init.
	 */
	mhpmevent_init_val |= (MHPMEVENT_యొక్క | MHPMEVENT_MINH);
	కొరకు (cidx = 0; cidx <= 28; cidx++) {
		ఒకవేళ (!(mhpm_ముసుగు & 1UL << (cidx + 3)))
			కొనసాగు;
#ఒకవేళ __riscv_xlen == 32
		csr_write_అంకె(CSR_MHPMEVENT3 + cidx,
			       mhpmevent_init_val & 0xFFFFFFFF);
		ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
			csr_write_అంకె(CSR_MHPMEVENT3H + cidx,
				      mhpmevent_init_val >> బిట్లు_చొప్పున_LONG);
#లేకపోతే
		csr_write_అంకె(CSR_MHPMEVENT3 + cidx, mhpmevent_init_val);
#అయితే_ముగింపు
	}

	ఒకవేళ (misa_extension('H'))
		csr_write(CSR_HSTATUS, 0);

	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMSTATEEN)) {
		mstateen_val = 0;
		mstateen_val |= SMSTATEEN_STATEN;
		mstateen_val |= SMSTATEEN0_CONTEXT;
		mstateen_val |= SMSTATEEN0_HSENVCFG;

		ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA))
			mstateen_val |= (SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);
		లేకపోతే
			mstateen_val &= ~(SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);

		ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA) ||
		    sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCSRIND))
			mstateen_val |= (SMSTATEEN0_SVSLCT);
		లేకపోతే
			mstateen_val &= ~(SMSTATEEN0_SVSLCT);

		ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCTR))
			mstateen_val |= SMSTATEEN0_CTR;
		లేకపోతే
			mstateen_val &= ~SMSTATEEN0_CTR;

		ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
			mstateen_val |= SMSTATEEN0_SRMCFG;
		లేకపోతే
			mstateen_val &= ~SMSTATEEN0_SRMCFG;

		csr_write64(CSR_MSTATEEN0, mstateen_val);
		csr_write64(CSR_MSTATEEN1, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN2, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN3, SMSTATEEN_STATEN);
	}

	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSSTATEEN)) {
		ఒకవేళ (misa_extension('S')) {
			csr_write(CSR_SSTATEEN0, 0);
			csr_write(CSR_SSTATEEN1, 0);
			csr_write(CSR_SSTATEEN2, 0);
			csr_write(CSR_SSTATEEN3, 0);
		}
		ఒకవేళ (misa_extension('H')) {
			csr_write64(CSR_HSTATEEN0, (uint64_t)0);
			csr_write64(CSR_HSTATEEN1, (uint64_t)0);
			csr_write64(CSR_HSTATEEN2, (uint64_t)0);
			csr_write64(CSR_HSTATEEN3, (uint64_t)0);
		}
	}

	ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_12) {
		menvcfg_val = csr_read64(CSR_MENVCFG);

		/* Disable HW A/D updating by default */
		menvcfg_val &= ~ENVCFG_ADUE;

		/* Disable double trap by default */
		menvcfg_val &= ~ENVCFG_DTE;

#నిర్వచించు __set_menvcfg_ext(__ext, __bits)				\
		ఒకవేళ (sbi_hart_has_extension(scratch, __ext))		\
			menvcfg_val |= __bits;

		/*
		 * Enable access to extensions if they are present in the
		 * hardware or in the device tree.
		 */

		__set_menvcfg_ext(SBI_HART_EXT_ZICBOZ, ENVCFG_CBZE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM, ENVCFG_CBCFE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM,
				  ENVCFG_CBIE_INV << ENVCFG_CBIE_జరుపు)
#ఒకవేళ __riscv_xlen > 32
		__set_menvcfg_ext(SBI_HART_EXT_SVPBMT, ENVCFG_PBMTE)
#అయితే_ముగింపు
		__set_menvcfg_ext(SBI_HART_EXT_SSTC, ENVCFG_STCE)
		__set_menvcfg_ext(SBI_HART_EXT_SMCDELEG, ENVCFG_CDE);

		/*
		 * Assume only Svadu is supported when it is the only extension
		 * present in the ISA string. Svade is assumed when neither are
		 * present. When both are present we must default to Svade (see
		 * the zero reset value of FWFT.PTE_AD_HW_UPDATING).
		 */
		ఒకవేళ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SVADE))
			__set_menvcfg_ext(SBI_HART_EXT_SVADU, ENVCFG_ADUE);

#నిర్వచనం_రద్దు __set_menvcfg_ext

		csr_write64(CSR_MENVCFG, menvcfg_val);

		/* Enable S-mode access to seed CSR */
		ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
			csr_సమితి(CSR_MSECCFG, MSECCFG_SSEED);
			csr_clear(CSR_MSECCFG, MSECCFG_USEED);
		}
	}

	/* Disable all interrupts */
	csr_write(CSR_MIE, 0);

	/* Disable S-mode paging */
	ఒకవేళ (misa_extension('S'))
		csr_write(CSR_SATP, 0);
}

స్థిర పూర్ణాంకం fp_init(నిర్మాణం sbi_scratch *scratch)
{
#నిర్వచితమైతే __riscv_flen
	పూర్ణాంకం i;
#అయితే_ముగింపు

	ఒకవేళ (!misa_extension('D') && !misa_extension('F'))
		తిరిగి 0;

	ఒకవేళ (!(csr_read(CSR_MSTATUS) & MSTATUS_FS))
		తిరిగి SBI_EINVAL;

#నిర్వచితమైతే __riscv_flen
	కొరకు (i = 0; i < 32; i++)
		init_fp_reg(i);
	csr_write(CSR_FCSR, 0);
#అయితే_ముగింపు

	తిరిగి 0;
}

శూన్య sbi_hart_delegation_dump(నిర్మాణం sbi_scratch *scratch,
			      స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *suffix)
{
	ఒకవేళ (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist*/
		తిరిగి;

	sbi_printf("%sMIDELEG%s: 0x%" PRILX "\n",
		   ఉపసర్గ, suffix, csr_read(CSR_MIDELEG));
	sbi_printf("%sMEDELEG%s: 0x%" PRILX "\n",
		   ఉపసర్గ, suffix, csr_read(CSR_MEDELEG));
}

గుర్తులేని పూర్ణాంకం sbi_hart_mhpm_ముసుగు(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_hart_features *hfeatures =
			sbi_scratch_స్థానభ్రంశం_ptr(scratch, hart_features_స్థానభ్రంశం);

	తిరిగి hfeatures->mhpm_ముసుగు;
}

గుర్తులేని పూర్ణాంకం sbi_hart_mhpm_బిట్లు(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_hart_features *hfeatures =
			sbi_scratch_స్థానభ్రంశం_ptr(scratch, hart_features_స్థానభ్రంశం);

	తిరిగి hfeatures->mhpm_బిట్లు;
}

పూర్ణాంకం sbi_hart_priv_సంస్కరణ(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_hart_features *hfeatures =
			sbi_scratch_స్థానభ్రంశం_ptr(scratch, hart_features_స్థానభ్రంశం);

	తిరిగి hfeatures->priv_సంస్కరణ;
}

శూన్య sbi_hart_get_priv_సంస్కరణ_str(నిర్మాణం sbi_scratch *scratch,
				   అక్షరం *సంస్కరణ_str, పూర్ణాంకం nvstr)
{
	అక్షరం *తాత్కాలిక;
	నిర్మాణం sbi_hart_features *hfeatures =
			sbi_scratch_స్థానభ్రంశం_ptr(scratch, hart_features_స్థానభ్రంశం);

	ఎంపిక (hfeatures->priv_సంస్కరణ) {
	స్థితి SBI_HART_PRIV_VER_1_10:
		తాత్కాలిక = "v1.10";
		ఆపు;
	స్థితి SBI_HART_PRIV_VER_1_11:
		తాత్కాలిక = "v1.11";
		ఆపు;
	స్థితి SBI_HART_PRIV_VER_1_12:
		తాత్కాలిక = "v1.12";
		ఆపు;
	అప్రమేయ:
		తాత్కాలిక = "unknown";
		ఆపు;
	}

	sbi_snprintf(సంస్కరణ_str, nvstr, "%s", తాత్కాలిక);
}

స్థిర అంతఃవరుస శూన్య __sbi_hart_update_extension(
					నిర్మాణం sbi_hart_features *hfeatures,
					గణన sbi_hart_extensions ext,
					bool enable)
{
	ఒకవేళ (enable)
		__set_bit(ext, hfeatures->extensions);
	లేకపోతే
		__clear_bit(ext, hfeatures->extensions);
}

/**
 * Enable/Disable a particular hart extension
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @param enable new state of hart extension
 */
శూన్య sbi_hart_update_extension(నిర్మాణం sbi_scratch *scratch,
			       గణన sbi_hart_extensions ext,
			       bool enable)
{
	నిర్మాణం sbi_hart_features *hfeatures =
			sbi_scratch_స్థానభ్రంశం_ptr(scratch, hart_features_స్థానభ్రంశం);

	__sbi_hart_update_extension(hfeatures, ext, enable);
}

/**
 * Check whether a particular hart extension is available
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @returns true (available) or false (not available)
 */
bool sbi_hart_has_extension(నిర్మాణం sbi_scratch *scratch,
			    గణన sbi_hart_extensions ext)
{
	నిర్మాణం sbi_hart_features *hfeatures =
			sbi_scratch_స్థానభ్రంశం_ptr(scratch, hart_features_స్థానభ్రంశం);

	ఒకవేళ (__test_bit(ext, hfeatures->extensions))
		తిరిగి true;
	లేకపోతే
		తిరిగి false;
}

#నిర్వచించు __SBI_HART_EXT_DATA(_name, _id) {	\
	.name = #_name,				\
	.id = _id,				\
}

స్థిరరాశి నిర్మాణం sbi_hart_ext_దత్తాంశం sbi_hart_ext[] = {
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

_Static_assert(SBI_HART_EXT_MAX == array_కొలత(sbi_hart_ext),
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
శూన్య sbi_hart_get_extensions_str(నిర్మాణం sbi_scratch *scratch,
				 అక్షరం *extensions_str, పూర్ణాంకం nestr)
{
	నిర్మాణం sbi_hart_features *hfeatures =
			sbi_scratch_స్థానభ్రంశం_ptr(scratch, hart_features_స్థానభ్రంశం);
	పూర్ణాంకం స్థానభ్రంశం = 0, ext = 0;

	ఒకవేళ (!extensions_str || nestr <= 0)
		తిరిగి;
	sbi_memset(extensions_str, 0, nestr);

	for_each_సమితి_బిట్టు(ext, hfeatures->extensions, SBI_HART_EXT_MAX) {
		ఒకవేళ (స్థానభ్రంశం + sbi_strlen(sbi_hart_ext[ext].name) + 1 > nestr) {
			sbi_printf("%s:extension name is longer than buffer (error)\n", __func__);
			ఆపు;
		}
		sbi_snprintf(extensions_str + స్థానభ్రంశం,
				 nestr - స్థానభ్రంశం,
				 "%s,", sbi_hart_ext[ext].name);
		స్థానభ్రంశం = స్థానభ్రంశం + sbi_strlen(sbi_hart_ext[ext].name) + 1;
	}

	ఒకవేళ (స్థానభ్రంశం)
		extensions_str[స్థానభ్రంశం - 1] = '\0';
	లేకపోతే
		sbi_strncpy(extensions_str, "none", nestr);
}

/**
 * Check whether a particular CSR is present on the HART
 *
 * @param scratch pointer to the HART scratch space
 * @param csr the CSR number to check
 */
bool sbi_hart_has_csr(నిర్మాణం sbi_scratch *scratch, గణన sbi_hart_csrs csr)
{
	నిర్మాణం sbi_hart_features *hfeatures =
			sbi_scratch_స్థానభ్రంశం_ptr(scratch, hart_features_స్థానభ్రంశం);

	తిరిగి __test_bit(csr, hfeatures->csrs);
}

స్థిర గుర్తులేని దీర్ఘ hart_pmp_get_allowed_addr(శూన్య)
{
	గుర్తులేని దీర్ఘ val = 0;
	నిర్మాణం sbi_trap_info trap = {0};

	csr_write_allowed(CSR_PMPCFG0, &trap, 0);
	ఒకవేళ (trap.cause)
		తిరిగి 0;

	csr_write_allowed(CSR_PMPADDR0, &trap, PMP_ADDR_ముసుగు);
	ఒకవేళ (!trap.cause) {
		val = csr_read_allowed(CSR_PMPADDR0, &trap);
		ఒకవేళ (trap.cause)
			val = 0;
	}

	తిరిగి val;
}

స్థిర పూర్ణాంకం hart_mhpm_get_allowed_బిట్లు(శూన్య)
{
	గుర్తులేని దీర్ఘ val = ~(0UL);
	నిర్మాణం sbi_trap_info trap = {0};
	పూర్ణాంకం అంకె_బిట్లు = 0;

	/**
	 * It is assumed that platforms will implement same number of bits for
	 * all the performance counters including mcycle/minstret.
	 */
	csr_write_allowed(CSR_MHPMCOUNTER3, &trap, val);
	ఒకవేళ (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3, &trap);
		ఒకవేళ (trap.cause)
			తిరిగి 0;
	}
	అంకె_బిట్లు = sbi_fls(val) + 1;
#ఒకవేళ __riscv_xlen == 32
	csr_write_allowed(CSR_MHPMCOUNTER3H, &trap, val);
	ఒకవేళ (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3H, &trap);
		ఒకవేళ (trap.cause)
			తిరిగి అంకె_బిట్లు;
	}
	అంకె_బిట్లు += sbi_fls(val) + 1;

#అయితే_ముగింపు

	తిరిగి అంకె_బిట్లు;
}

స్థిర పూర్ణాంకం hart_detect_features(నిర్మాణం sbi_scratch *scratch, bool cold_boot)
{
	నిర్మాణం sbi_trap_info trap = {0};
	నిర్మాణం sbi_hart_features *hfeatures =
		sbi_scratch_స్థానభ్రంశం_ptr(scratch, hart_features_స్థానభ్రంశం);
	గుర్తులేని దీర్ఘ val, oldval;
	పూర్ణాంకం rc;

	/* If hart features already detected then do nothing */
	ఒకవేళ (hfeatures->detected)
		తిరిగి 0;

	/*
	 * Parse device tree extensions early, before any trap-based checks.
	 * Needed to detect Smrnmi and install NMI handlers before CSR probes
	 * that may trigger traps.
	 */
	rc = sbi_platform_extensions_init(sbi_platform_ptr(scratch), cold_boot);
	ఒకవేళ (rc)
		తిరిగి rc;

	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMRNMI)) {
		స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_thishart_ptr();
		స్థిరరాశి నిర్మాణం sbi_platform_operations *ops = sbi_platform_ops(plat);
		బాహ్య శూన్య _trap_rnmi_handler(శూన్య);
		బాహ్య శూన్య _trap_handler(శూన్య);

		ఒకవేళ (!ops || !ops->smrnmi_handlers_init)
			sbi_panic("Smrnmi detected, but platform lacks smrnmi_handlers_init callback\n");

		/* Reuse _trap_handler for the RNME slot since RNME is taken
		 * as a regular M-mode trap with NMIE=0. */
		ops->smrnmi_handlers_init(_trap_rnmi_handler, _trap_handler);

		/* Initialize MNSCRATCH for the RNMI handler */
		csr_write(CSR_MNSCRATCH, scratch);

		/* Enable NMIs */
		csr_సమితి(CSR_MNSTATUS, MNSTATUS_NMIE);
	}

#నిర్వచించు __check_hpm_csr(__csr, __mask) 					  \
	oldval = csr_read_allowed(__csr, &trap);			  \
	ఒకవేళ (!trap.cause) {						  \
		csr_write_allowed(__csr, &trap, 1UL);			  \
		ఒకవేళ (!trap.cause && csr_మార్పిడి(__csr, oldval) == 1UL) {	  \
			(hfeatures->__mask) |= 1 << (__csr - CSR_MCYCLE); \
		}							  \
	}

#నిర్వచించు __check_hpm_csr_2(__csr, __mask)	 		  \
	__check_hpm_csr(__csr + 0, __mask)	 		  \
	__check_hpm_csr(__csr + 1, __mask)
#నిర్వచించు __check_hpm_csr_4(__csr, __mask)	 		  \
	__check_hpm_csr_2(__csr + 0, __mask) 			  \
	__check_hpm_csr_2(__csr + 2, __mask)
#నిర్వచించు __check_hpm_csr_8(__csr, __mask)	 		  \
	__check_hpm_csr_4(__csr + 0, __mask) 			  \
	__check_hpm_csr_4(__csr + 4, __mask)
#నిర్వచించు __check_hpm_csr_16(__csr, __mask)	 		  \
	__check_hpm_csr_8(__csr + 0, __mask) 			  \
	__check_hpm_csr_8(__csr + 8, __mask)

#నిర్వచించు __check_csr(__csr, __rdonly, __wrval, __field, __skip)		\
	oldval = csr_read_allowed(__csr, &trap);			\
	ఒకవేళ (!trap.cause) {						\
		ఒకవేళ (__rdonly) {						\
			(hfeatures->__field)++;				\
		} లేకపోతే {						\
			csr_write_allowed(__csr, &trap, __wrval);	\
			ఒకవేళ (!trap.cause) {				\
				ఒకవేళ ((csr_మార్పిడి(__csr, oldval) & __wrval) == __wrval)	\
					(hfeatures->__field)++;		\
				లేకపోతే					\
					వెళ్ళు __skip;			\
			} లేకపోతే {					\
				వెళ్ళు __skip;				\
			}						\
		}							\
	} లేకపోతే {							\
		వెళ్ళు __skip;						\
	}
#నిర్వచించు __check_csr_2(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 1, __rdonly, __wrval, __field, __skip)
#నిర్వచించు __check_csr_4(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 2, __rdonly, __wrval, __field, __skip)
#నిర్వచించు __check_csr_8(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 4, __rdonly, __wrval, __field, __skip)
#నిర్వచించు __check_csr_16(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 8, __rdonly, __wrval, __field, __skip)
#నిర్వచించు __check_csr_32(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 16, __rdonly, __wrval, __field, __skip)
#నిర్వచించు __check_csr_64(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 32, __rdonly, __wrval, __field, __skip)

	/**
	 * Detect the allowed address bits & granularity. At least PMPADDR0
	 * should be implemented.
	 */
	val = hart_pmp_get_allowed_addr();
	ఒకవేళ (val) {
		hfeatures->pmp_log2gran = sbi_ffs(val) + 2;
		hfeatures->pmp_addr_బిట్లు = sbi_fls(val) + 1;
		/* Detect number of PMP regions. At least PMPADDR0 should be implemented*/
		__check_csr_64(CSR_PMPADDR0, 0, val, pmp_లెక్క, __pmp_skip);
	}
__pmp_skip:
	/* Detect number of MHPM counters */
	__check_hpm_csr(CSR_MHPMCOUNTER3, mhpm_ముసుగు);
	hfeatures->mhpm_బిట్లు = hart_mhpm_get_allowed_బిట్లు();
	__check_hpm_csr_4(CSR_MHPMCOUNTER4, mhpm_ముసుగు);
	__check_hpm_csr_8(CSR_MHPMCOUNTER8, mhpm_ముసుగు);
	__check_hpm_csr_16(CSR_MHPMCOUNTER16, mhpm_ముసుగు);

	/**
	 * No need to check for MHPMCOUNTERH for RV32 as they are expected to be
	 * implemented if MHPMCOUNTER is implemented.
	 */
#నిర్వచనం_రద్దు __check_csr_64
#నిర్వచనం_రద్దు __check_csr_32
#నిర్వచనం_రద్దు __check_csr_16
#నిర్వచనం_రద్దు __check_csr_8
#నిర్వచనం_రద్దు __check_csr_4
#నిర్వచనం_రద్దు __check_csr_2
#నిర్వచనం_రద్దు __check_csr


#నిర్వచించు __check_priv(__csr, __base_priv, __priv)			\
	val = csr_read_allowed(__csr, &trap);				\
	ఒకవేళ (!trap.cause && (hfeatures->priv_సంస్కరణ >= __base_priv)) {	\
		hfeatures->priv_సంస్కరణ = __priv;			\
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

#నిర్వచనం_రద్దు __check_priv_csr

#నిర్వచించు __check_ext_csr(__base_priv, __csr, __ext)			\
	ఒకవేళ (hfeatures->priv_సంస్కరణ >= __base_priv) {			\
		csr_read_allowed(__csr, &trap);				\
		ఒకవేళ (!trap.cause)					\
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

#నిర్వచనం_రద్దు __check_ext_csr

#నిర్వచించు __check_csr_existence(__csr, __csr_id)				\
	csr_read_allowed(__csr, &trap);					\
	ఒకవేళ (!trap.cause)						\
		__set_bit(__csr_id, hfeatures->csrs);

	__check_csr_existence(CSR_చక్రం, SBI_HART_CSR_చక్రం);
	__check_csr_existence(CSR_సమయం, SBI_HART_CSR_సమయం);
	__check_csr_existence(CSR_INSTRET, SBI_HART_CSR_INSTRET);

#నిర్వచనం_రద్దు __check_csr_existence

	/* Zicntr should only be detected using traps */
	__sbi_hart_update_extension(hfeatures, SBI_HART_EXT_ZICNTR,
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_చక్రం) &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_సమయం)  &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET));

	/* Extensions implied by other extensions and features */
	ఒకవేళ (hfeatures->mhpm_ముసుగు)
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
	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP) &&
	    (csr_read(CSR_MSECCFG) & MSECCFG_MML))
		తిరిగి SBI_EILL;

	తిరిగి 0;
}

పూర్ణాంకం sbi_hart_reinit(నిర్మాణం sbi_scratch *scratch)
{
	పూర్ణాంకం rc;

	mstatus_init(scratch);

	rc = fp_init(scratch);
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి 0;
}

పూర్ణాంకం sbi_hart_init(నిర్మాణం sbi_scratch *scratch, bool cold_boot)
{
	పూర్ణాంకం rc;

	/*
	 * Clear mip CSR before proceeding with init to avoid any spurious
	 * external interrupts in S-mode.
	 */
	csr_write(CSR_MIP, 0);

	ఒకవేళ (cold_boot) {
		ఒకవేళ (misa_extension('H'))
			sbi_hart_expected_trap = &__sbi_expected_trap_hext;

		hart_features_స్థానభ్రంశం = sbi_scratch_alloc_స్థానభ్రంశం(
					పరిమాణం(నిర్మాణం sbi_hart_features));
		ఒకవేళ (!hart_features_స్థానభ్రంశం)
			తిరిగి SBI_ENOMEM;
	}

	rc = hart_detect_features(scratch, cold_boot);
	ఒకవేళ (rc)
		తిరిగి rc;

	ఒకవేళ (cold_boot) {
		rc = sbi_hart_pmp_init(scratch);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	తిరిగి sbi_hart_reinit(scratch);
}

శూన్య __attribute__((noreturn)) sbi_hart_hang(శూన్య)
{
	అయితే (1)
		wfi();
	__builtin_unreachable();
}

శూన్య __attribute__((noreturn))
sbi_hart_switch_బహుళకం(గుర్తులేని దీర్ఘ arg0, గుర్తులేని దీర్ఘ arg1,
		     గుర్తులేని దీర్ఘ తదుపరి_addr, గుర్తులేని దీర్ఘ తదుపరి_బహుళకం,
		     bool తదుపరి_virt)
{
#ఒకవేళ __riscv_xlen == 32
	గుర్తులేని దీర్ఘ val, valH;
#లేకపోతే
	గుర్తులేని దీర్ఘ val;
#అయితే_ముగింపు

	ఎంపిక (తదుపరి_బహుళకం) {
	స్థితి PRV_M:
		ఆపు;
	స్థితి PRV_S:
		ఒకవేళ (!misa_extension('S'))
			sbi_hart_hang();
		ఆపు;
	స్థితి PRV_U:
		ఒకవేళ (!misa_extension('U'))
			sbi_hart_hang();
		ఆపు;
	అప్రమేయ:
		sbi_hart_hang();
	}

	val = csr_read(CSR_MSTATUS);
	val = చొప్పించు_క్షేత్రం(val, MSTATUS_MPP, తదుపరి_బహుళకం);
	val = చొప్పించు_క్షేత్రం(val, MSTATUS_MPIE, 0);
#ఒకవేళ __riscv_xlen == 32
	ఒకవేళ (misa_extension('H')) {
		valH = csr_read(CSR_MSTATUSH);
		valH = చొప్పించు_క్షేత్రం(valH, MSTATUSH_MPV, తదుపరి_virt);
		csr_write(CSR_MSTATUSH, valH);
	}
#లేకపోతే
	ఒకవేళ (misa_extension('H'))
		val = చొప్పించు_క్షేత్రం(val, MSTATUS_MPV, తదుపరి_virt);
#అయితే_ముగింపు
	csr_write(CSR_MSTATUS, val);
	csr_write(CSR_MEPC, తదుపరి_addr);

	ఒకవేళ (తదుపరి_బహుళకం == PRV_S) {
		ఒకవేళ (తదుపరి_virt) {
			csr_write(CSR_VSTVEC, తదుపరి_addr);
			csr_write(CSR_VSSCRATCH, 0);
			csr_write(CSR_VSIE, 0);
			csr_write(CSR_VSATP, 0);
		} లేకపోతే {
			csr_write(CSR_STVEC, తదుపరి_addr);
			csr_write(CSR_SSCRATCH, 0);
			csr_write(CSR_SIE, 0);
			csr_write(CSR_SATP, 0);
		}
	} లేకపోతే ఒకవేళ (తదుపరి_బహుళకం == PRV_U) {
		ఒకవేళ (misa_extension('N')) {
			csr_write(CSR_UTVEC, తదుపరి_addr);
			csr_write(CSR_USCRATCH, 0);
			csr_write(CSR_UIE, 0);
		}
	}

	నమోదు గుర్తులేని దీర్ఘ a0 asm("a0") = arg0;
	నమోదు గుర్తులేని దీర్ఘ a1 asm("a1") = arg1;
	__asm__ __volatile__("mret" : : "r"(a0), "r"(a1));
	__builtin_unreachable();
}
