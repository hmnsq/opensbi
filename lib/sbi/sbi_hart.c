/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_barrier.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/riscv_fp.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_csr_detect.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_hart_pmp.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_pmu.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_trap.h>

बाह्य शून्य __sbi_expected_trap(शून्य);
बाह्य शून्य __sbi_expected_trap_hext(शून्य);

शून्य (*sbi_hart_expected_trap)(शून्य) = &__sbi_expected_trap;

अचिह्नित दीर्घ hart_features_विस्थापनम्;

स्थिर शून्य mstatus_init(संरचना sbi_scratch *scratch)
{
	पूर्णाङ्क cidx;
	अचिह्नित दीर्घ mstatus_val = 0;
	अचिह्नित पूर्णाङ्क mhpm_आवरणम् = sbi_hart_mhpm_आवरणम्(scratch);
	uint64_t mhpmevent_init_val = 0;
	uint64_t menvcfg_val, mstateen_val;

	/* Enable FPU */
	यदि (misa_extension('D') || misa_extension('F'))
		mstatus_val |=  MSTATUS_FS;

	/* Enable Vector context */
	यदि (misa_extension('V'))
		mstatus_val |=  MSTATUS_VS;

	csr_write(CSR_MSTATUS, mstatus_val);

	/* Disable user mode usage of all perf counters except TM */
	यदि (misa_extension('S') &&
	    sbi_hart_priv_संस्करणम्(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_SCOUNTEREN, 0x02);

	/**
	 * OpenSBI doesn't use any PMU counters in M-mode.
	 * Supervisor mode usage for all counters are enabled by default
	 * But counters will not run until mcountinhibit is set.
	 */
	यदि (sbi_hart_priv_संस्करणम्(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	/* All programmable counters will start running at runtime after S-mode request */
	यदि (sbi_hart_priv_संस्करणम्(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	/**
	 * The mhpmeventn[h] CSR should be initialized with interrupt disabled
	 * and inhibited running in M-mode during init.
	 */
	mhpmevent_init_val |= (MHPMEVENT_सम्बन्धि | MHPMEVENT_MINH);
	प्रति (cidx = 0; cidx <= 28; cidx++) {
		यदि (!(mhpm_आवरणम् & 1UL << (cidx + 3)))
			अनुवर्त;
#यदि __riscv_xlen == 32
		csr_write_अङ्कः(CSR_MHPMEVENT3 + cidx,
			       mhpmevent_init_val & 0xFFFFFFFF);
		यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
			csr_write_अङ्कः(CSR_MHPMEVENT3H + cidx,
				      mhpmevent_init_val >> द्वयङ्काः_प्रत्येकम्_LONG);
#अन्यथा
		csr_write_अङ्कः(CSR_MHPMEVENT3 + cidx, mhpmevent_init_val);
#यदि_अन्तः
	}

	यदि (misa_extension('H'))
		csr_write(CSR_HSTATUS, 0);

	यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMSTATEEN)) {
		mstateen_val = 0;
		mstateen_val |= SMSTATEEN_STATEN;
		mstateen_val |= SMSTATEEN0_CONTEXT;
		mstateen_val |= SMSTATEEN0_HSENVCFG;

		यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA))
			mstateen_val |= (SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);
		अन्यथा
			mstateen_val &= ~(SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);

		यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA) ||
		    sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCSRIND))
			mstateen_val |= (SMSTATEEN0_SVSLCT);
		अन्यथा
			mstateen_val &= ~(SMSTATEEN0_SVSLCT);

		यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCTR))
			mstateen_val |= SMSTATEEN0_CTR;
		अन्यथा
			mstateen_val &= ~SMSTATEEN0_CTR;

		यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
			mstateen_val |= SMSTATEEN0_SRMCFG;
		अन्यथा
			mstateen_val &= ~SMSTATEEN0_SRMCFG;

		csr_write64(CSR_MSTATEEN0, mstateen_val);
		csr_write64(CSR_MSTATEEN1, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN2, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN3, SMSTATEEN_STATEN);
	}

	यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSSTATEEN)) {
		यदि (misa_extension('S')) {
			csr_write(CSR_SSTATEEN0, 0);
			csr_write(CSR_SSTATEEN1, 0);
			csr_write(CSR_SSTATEEN2, 0);
			csr_write(CSR_SSTATEEN3, 0);
		}
		यदि (misa_extension('H')) {
			csr_write64(CSR_HSTATEEN0, (uint64_t)0);
			csr_write64(CSR_HSTATEEN1, (uint64_t)0);
			csr_write64(CSR_HSTATEEN2, (uint64_t)0);
			csr_write64(CSR_HSTATEEN3, (uint64_t)0);
		}
	}

	यदि (sbi_hart_priv_संस्करणम्(scratch) >= SBI_HART_PRIV_VER_1_12) {
		menvcfg_val = csr_read64(CSR_MENVCFG);

		/* Disable HW A/D updating by default */
		menvcfg_val &= ~ENVCFG_ADUE;

		/* Disable double trap by default */
		menvcfg_val &= ~ENVCFG_DTE;

#परिभाषय __set_menvcfg_ext(__ext, __bits)				\
		यदि (sbi_hart_has_extension(scratch, __ext))		\
			menvcfg_val |= __bits;

		/*
		 * Enable access to extensions if they are present in the
		 * hardware or in the device tree.
		 */

		__set_menvcfg_ext(SBI_HART_EXT_ZICBOZ, ENVCFG_CBZE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM, ENVCFG_CBCFE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM,
				  ENVCFG_CBIE_INV << ENVCFG_CBIE_सरणम्)
#यदि __riscv_xlen > 32
		__set_menvcfg_ext(SBI_HART_EXT_SVPBMT, ENVCFG_PBMTE)
#यदि_अन्तः
		__set_menvcfg_ext(SBI_HART_EXT_SSTC, ENVCFG_STCE)
		__set_menvcfg_ext(SBI_HART_EXT_SMCDELEG, ENVCFG_CDE);

		/*
		 * Assume only Svadu is supported when it is the only extension
		 * present in the ISA string. Svade is assumed when neither are
		 * present. When both are present we must default to Svade (see
		 * the zero reset value of FWFT.PTE_AD_HW_UPDATING).
		 */
		यदि (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SVADE))
			__set_menvcfg_ext(SBI_HART_EXT_SVADU, ENVCFG_ADUE);

#अपरिभाषय __set_menvcfg_ext

		csr_write64(CSR_MENVCFG, menvcfg_val);

		/* Enable S-mode access to seed CSR */
		यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
			csr_समुच्चयः(CSR_MSECCFG, MSECCFG_SSEED);
			csr_clear(CSR_MSECCFG, MSECCFG_USEED);
		}
	}

	/* Disable all interrupts */
	csr_write(CSR_MIE, 0);

	/* Disable S-mode paging */
	यदि (misa_extension('S'))
		csr_write(CSR_SATP, 0);
}

स्थिर पूर्णाङ्क fp_init(संरचना sbi_scratch *scratch)
{
#यदि_परिभाषितम् __riscv_flen
	पूर्णाङ्क i;
#यदि_अन्तः

	यदि (!misa_extension('D') && !misa_extension('F'))
		प्रत्यागम 0;

	यदि (!(csr_read(CSR_MSTATUS) & MSTATUS_FS))
		प्रत्यागम SBI_EINVAL;

#यदि_परिभाषितम् __riscv_flen
	प्रति (i = 0; i < 32; i++)
		init_fp_reg(i);
	csr_write(CSR_FCSR, 0);
#यदि_अन्तः

	प्रत्यागम 0;
}

शून्य sbi_hart_delegation_dump(संरचना sbi_scratch *scratch,
			      ध्रुव वर्ण *उपसर्गः, ध्रुव वर्ण *suffix)
{
	यदि (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist*/
		प्रत्यागम;

	sbi_printf("%sMIDELEG%s: 0x%" PRILX "\n",
		   उपसर्गः, suffix, csr_read(CSR_MIDELEG));
	sbi_printf("%sMEDELEG%s: 0x%" PRILX "\n",
		   उपसर्गः, suffix, csr_read(CSR_MEDELEG));
}

अचिह्नित पूर्णाङ्क sbi_hart_mhpm_आवरणम्(संरचना sbi_scratch *scratch)
{
	संरचना sbi_hart_features *hfeatures =
			sbi_scratch_विस्थापनम्_ptr(scratch, hart_features_विस्थापनम्);

	प्रत्यागम hfeatures->mhpm_आवरणम्;
}

अचिह्नित पूर्णाङ्क sbi_hart_mhpm_द्वयङ्काः(संरचना sbi_scratch *scratch)
{
	संरचना sbi_hart_features *hfeatures =
			sbi_scratch_विस्थापनम्_ptr(scratch, hart_features_विस्थापनम्);

	प्रत्यागम hfeatures->mhpm_द्वयङ्काः;
}

पूर्णाङ्क sbi_hart_priv_संस्करणम्(संरचना sbi_scratch *scratch)
{
	संरचना sbi_hart_features *hfeatures =
			sbi_scratch_विस्थापनम्_ptr(scratch, hart_features_विस्थापनम्);

	प्रत्यागम hfeatures->priv_संस्करणम्;
}

शून्य sbi_hart_get_priv_संस्करणम्_str(संरचना sbi_scratch *scratch,
				   वर्ण *संस्करणम्_str, पूर्णाङ्क nvstr)
{
	वर्ण *अस्थायी;
	संरचना sbi_hart_features *hfeatures =
			sbi_scratch_विस्थापनम्_ptr(scratch, hart_features_विस्थापनम्);

	विकल्प (hfeatures->priv_संस्करणम्) {
	पक्ष SBI_HART_PRIV_VER_1_10:
		अस्थायी = "v1.10";
		भङ्ग;
	पक्ष SBI_HART_PRIV_VER_1_11:
		अस्थायी = "v1.11";
		भङ्ग;
	पक्ष SBI_HART_PRIV_VER_1_12:
		अस्थायी = "v1.12";
		भङ्ग;
	स्वभाव:
		अस्थायी = "unknown";
		भङ्ग;
	}

	sbi_snprintf(संस्करणम्_str, nvstr, "%s", अस्थायी);
}

स्थिर अन्तःस्थ शून्य __sbi_hart_update_extension(
					संरचना sbi_hart_features *hfeatures,
					गणना sbi_hart_extensions ext,
					bool enable)
{
	यदि (enable)
		__set_bit(ext, hfeatures->extensions);
	अन्यथा
		__clear_bit(ext, hfeatures->extensions);
}

/**
 * Enable/Disable a particular hart extension
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @param enable new state of hart extension
 */
शून्य sbi_hart_update_extension(संरचना sbi_scratch *scratch,
			       गणना sbi_hart_extensions ext,
			       bool enable)
{
	संरचना sbi_hart_features *hfeatures =
			sbi_scratch_विस्थापनम्_ptr(scratch, hart_features_विस्थापनम्);

	__sbi_hart_update_extension(hfeatures, ext, enable);
}

/**
 * Check whether a particular hart extension is available
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @returns true (available) or false (not available)
 */
bool sbi_hart_has_extension(संरचना sbi_scratch *scratch,
			    गणना sbi_hart_extensions ext)
{
	संरचना sbi_hart_features *hfeatures =
			sbi_scratch_विस्थापनम्_ptr(scratch, hart_features_विस्थापनम्);

	यदि (__test_bit(ext, hfeatures->extensions))
		प्रत्यागम true;
	अन्यथा
		प्रत्यागम false;
}

#परिभाषय __SBI_HART_EXT_DATA(_name, _id) {	\
	.name = #_name,				\
	.id = _id,				\
}

ध्रुव संरचना sbi_hart_ext_दत्त sbi_hart_ext[] = {
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

_Static_assert(SBI_HART_EXT_MAX == array_आकार(sbi_hart_ext),
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
शून्य sbi_hart_get_extensions_str(संरचना sbi_scratch *scratch,
				 वर्ण *extensions_str, पूर्णाङ्क nestr)
{
	संरचना sbi_hart_features *hfeatures =
			sbi_scratch_विस्थापनम्_ptr(scratch, hart_features_विस्थापनम्);
	पूर्णाङ्क विस्थापनम् = 0, ext = 0;

	यदि (!extensions_str || nestr <= 0)
		प्रत्यागम;
	sbi_memset(extensions_str, 0, nestr);

	for_each_समुच्चयः_bit(ext, hfeatures->extensions, SBI_HART_EXT_MAX) {
		यदि (विस्थापनम् + sbi_strlen(sbi_hart_ext[ext].name) + 1 > nestr) {
			sbi_printf("%s:extension name is longer than buffer (error)\n", __func__);
			भङ्ग;
		}
		sbi_snprintf(extensions_str + विस्थापनम्,
				 nestr - विस्थापनम्,
				 "%s,", sbi_hart_ext[ext].name);
		विस्थापनम् = विस्थापनम् + sbi_strlen(sbi_hart_ext[ext].name) + 1;
	}

	यदि (विस्थापनम्)
		extensions_str[विस्थापनम् - 1] = '\0';
	अन्यथा
		sbi_strncpy(extensions_str, "none", nestr);
}

/**
 * Check whether a particular CSR is present on the HART
 *
 * @param scratch pointer to the HART scratch space
 * @param csr the CSR number to check
 */
bool sbi_hart_has_csr(संरचना sbi_scratch *scratch, गणना sbi_hart_csrs csr)
{
	संरचना sbi_hart_features *hfeatures =
			sbi_scratch_विस्थापनम्_ptr(scratch, hart_features_विस्थापनम्);

	प्रत्यागम __test_bit(csr, hfeatures->csrs);
}

स्थिर अचिह्नित दीर्घ hart_pmp_get_allowed_addr(शून्य)
{
	अचिह्नित दीर्घ val = 0;
	संरचना sbi_trap_info trap = {0};

	csr_write_allowed(CSR_PMPCFG0, &trap, 0);
	यदि (trap.cause)
		प्रत्यागम 0;

	csr_write_allowed(CSR_PMPADDR0, &trap, PMP_ADDR_आवरणम्);
	यदि (!trap.cause) {
		val = csr_read_allowed(CSR_PMPADDR0, &trap);
		यदि (trap.cause)
			val = 0;
	}

	प्रत्यागम val;
}

स्थिर पूर्णाङ्क hart_mhpm_get_allowed_द्वयङ्काः(शून्य)
{
	अचिह्नित दीर्घ val = ~(0UL);
	संरचना sbi_trap_info trap = {0};
	पूर्णाङ्क अङ्कः_द्वयङ्काः = 0;

	/**
	 * It is assumed that platforms will implement same number of bits for
	 * all the performance counters including mcycle/minstret.
	 */
	csr_write_allowed(CSR_MHPMCOUNTER3, &trap, val);
	यदि (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3, &trap);
		यदि (trap.cause)
			प्रत्यागम 0;
	}
	अङ्कः_द्वयङ्काः = sbi_fls(val) + 1;
#यदि __riscv_xlen == 32
	csr_write_allowed(CSR_MHPMCOUNTER3H, &trap, val);
	यदि (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3H, &trap);
		यदि (trap.cause)
			प्रत्यागम अङ्कः_द्वयङ्काः;
	}
	अङ्कः_द्वयङ्काः += sbi_fls(val) + 1;

#यदि_अन्तः

	प्रत्यागम अङ्कः_द्वयङ्काः;
}

स्थिर पूर्णाङ्क hart_detect_features(संरचना sbi_scratch *scratch, bool cold_boot)
{
	संरचना sbi_trap_info trap = {0};
	संरचना sbi_hart_features *hfeatures =
		sbi_scratch_विस्थापनम्_ptr(scratch, hart_features_विस्थापनम्);
	अचिह्नित दीर्घ val, oldval;
	पूर्णाङ्क rc;

	/* If hart features already detected then do nothing */
	यदि (hfeatures->detected)
		प्रत्यागम 0;

	/*
	 * Parse device tree extensions early, before any trap-based checks.
	 * Needed to detect Smrnmi and install NMI handlers before CSR probes
	 * that may trigger traps.
	 */
	rc = sbi_platform_extensions_init(sbi_platform_ptr(scratch), cold_boot);
	यदि (rc)
		प्रत्यागम rc;

	यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMRNMI)) {
		ध्रुव संरचना sbi_platform *plat = sbi_platform_thishart_ptr();
		ध्रुव संरचना sbi_platform_operations *ops = sbi_platform_ops(plat);
		बाह्य शून्य _trap_rnmi_handler(शून्य);
		बाह्य शून्य _trap_handler(शून्य);

		यदि (!ops || !ops->smrnmi_handlers_init)
			sbi_panic("Smrnmi detected, but platform lacks smrnmi_handlers_init callback\n");

		/* Reuse _trap_handler for the RNME slot since RNME is taken
		 * as a regular M-mode trap with NMIE=0. */
		ops->smrnmi_handlers_init(_trap_rnmi_handler, _trap_handler);

		/* Initialize MNSCRATCH for the RNMI handler */
		csr_write(CSR_MNSCRATCH, scratch);

		/* Enable NMIs */
		csr_समुच्चयः(CSR_MNSTATUS, MNSTATUS_NMIE);
	}

#परिभाषय __check_hpm_csr(__csr, __mask) 					  \
	oldval = csr_read_allowed(__csr, &trap);			  \
	यदि (!trap.cause) {						  \
		csr_write_allowed(__csr, &trap, 1UL);			  \
		यदि (!trap.cause && csr_swap(__csr, oldval) == 1UL) {	  \
			(hfeatures->__mask) |= 1 << (__csr - CSR_MCYCLE); \
		}							  \
	}

#परिभाषय __check_hpm_csr_2(__csr, __mask)	 		  \
	__check_hpm_csr(__csr + 0, __mask)	 		  \
	__check_hpm_csr(__csr + 1, __mask)
#परिभाषय __check_hpm_csr_4(__csr, __mask)	 		  \
	__check_hpm_csr_2(__csr + 0, __mask) 			  \
	__check_hpm_csr_2(__csr + 2, __mask)
#परिभाषय __check_hpm_csr_8(__csr, __mask)	 		  \
	__check_hpm_csr_4(__csr + 0, __mask) 			  \
	__check_hpm_csr_4(__csr + 4, __mask)
#परिभाषय __check_hpm_csr_16(__csr, __mask)	 		  \
	__check_hpm_csr_8(__csr + 0, __mask) 			  \
	__check_hpm_csr_8(__csr + 8, __mask)

#परिभाषय __check_csr(__csr, __rdonly, __wrval, __field, __skip)		\
	oldval = csr_read_allowed(__csr, &trap);			\
	यदि (!trap.cause) {						\
		यदि (__rdonly) {						\
			(hfeatures->__field)++;				\
		} अन्यथा {						\
			csr_write_allowed(__csr, &trap, __wrval);	\
			यदि (!trap.cause) {				\
				यदि ((csr_swap(__csr, oldval) & __wrval) == __wrval)	\
					(hfeatures->__field)++;		\
				अन्यथा					\
					गच्छ __skip;			\
			} अन्यथा {					\
				गच्छ __skip;				\
			}						\
		}							\
	} अन्यथा {							\
		गच्छ __skip;						\
	}
#परिभाषय __check_csr_2(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 1, __rdonly, __wrval, __field, __skip)
#परिभाषय __check_csr_4(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 2, __rdonly, __wrval, __field, __skip)
#परिभाषय __check_csr_8(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 4, __rdonly, __wrval, __field, __skip)
#परिभाषय __check_csr_16(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 8, __rdonly, __wrval, __field, __skip)
#परिभाषय __check_csr_32(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 16, __rdonly, __wrval, __field, __skip)
#परिभाषय __check_csr_64(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 32, __rdonly, __wrval, __field, __skip)

	/**
	 * Detect the allowed address bits & granularity. At least PMPADDR0
	 * should be implemented.
	 */
	val = hart_pmp_get_allowed_addr();
	यदि (val) {
		hfeatures->pmp_log2gran = sbi_ffs(val) + 2;
		hfeatures->pmp_addr_द्वयङ्काः = sbi_fls(val) + 1;
		/* Detect number of PMP regions. At least PMPADDR0 should be implemented*/
		__check_csr_64(CSR_PMPADDR0, 0, val, pmp_गणनम्, __pmp_skip);
	}
__pmp_skip:
	/* Detect number of MHPM counters */
	__check_hpm_csr(CSR_MHPMCOUNTER3, mhpm_आवरणम्);
	hfeatures->mhpm_द्वयङ्काः = hart_mhpm_get_allowed_द्वयङ्काः();
	__check_hpm_csr_4(CSR_MHPMCOUNTER4, mhpm_आवरणम्);
	__check_hpm_csr_8(CSR_MHPMCOUNTER8, mhpm_आवरणम्);
	__check_hpm_csr_16(CSR_MHPMCOUNTER16, mhpm_आवरणम्);

	/**
	 * No need to check for MHPMCOUNTERH for RV32 as they are expected to be
	 * implemented if MHPMCOUNTER is implemented.
	 */
#अपरिभाषय __check_csr_64
#अपरिभाषय __check_csr_32
#अपरिभाषय __check_csr_16
#अपरिभाषय __check_csr_8
#अपरिभाषय __check_csr_4
#अपरिभाषय __check_csr_2
#अपरिभाषय __check_csr


#परिभाषय __check_priv(__csr, __base_priv, __priv)			\
	val = csr_read_allowed(__csr, &trap);				\
	यदि (!trap.cause && (hfeatures->priv_संस्करणम् >= __base_priv)) {	\
		hfeatures->priv_संस्करणम् = __priv;			\
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

#अपरिभाषय __check_priv_csr

#परिभाषय __check_ext_csr(__base_priv, __csr, __ext)			\
	यदि (hfeatures->priv_संस्करणम् >= __base_priv) {			\
		csr_read_allowed(__csr, &trap);				\
		यदि (!trap.cause)					\
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

#अपरिभाषय __check_ext_csr

#परिभाषय __check_csr_existence(__csr, __csr_id)				\
	csr_read_allowed(__csr, &trap);					\
	यदि (!trap.cause)						\
		__set_bit(__csr_id, hfeatures->csrs);

	__check_csr_existence(CSR_आवृत्तिः, SBI_HART_CSR_आवृत्तिः);
	__check_csr_existence(CSR_TIME, SBI_HART_CSR_TIME);
	__check_csr_existence(CSR_INSTRET, SBI_HART_CSR_INSTRET);

#अपरिभाषय __check_csr_existence

	/* Zicntr should only be detected using traps */
	__sbi_hart_update_extension(hfeatures, SBI_HART_EXT_ZICNTR,
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_आवृत्तिः) &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME)  &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET));

	/* Extensions implied by other extensions and features */
	यदि (hfeatures->mhpm_आवरणम्)
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
	यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP) &&
	    (csr_read(CSR_MSECCFG) & MSECCFG_MML))
		प्रत्यागम SBI_EILL;

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_hart_reinit(संरचना sbi_scratch *scratch)
{
	पूर्णाङ्क rc;

	mstatus_init(scratch);

	rc = fp_init(scratch);
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_hart_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	पूर्णाङ्क rc;

	/*
	 * Clear mip CSR before proceeding with init to avoid any spurious
	 * external interrupts in S-mode.
	 */
	csr_write(CSR_MIP, 0);

	यदि (cold_boot) {
		यदि (misa_extension('H'))
			sbi_hart_expected_trap = &__sbi_expected_trap_hext;

		hart_features_विस्थापनम् = sbi_scratch_alloc_विस्थापनम्(
					परिमाण(संरचना sbi_hart_features));
		यदि (!hart_features_विस्थापनम्)
			प्रत्यागम SBI_ENOMEM;
	}

	rc = hart_detect_features(scratch, cold_boot);
	यदि (rc)
		प्रत्यागम rc;

	यदि (cold_boot) {
		rc = sbi_hart_pmp_init(scratch);
		यदि (rc)
			प्रत्यागम rc;
	}

	प्रत्यागम sbi_hart_reinit(scratch);
}

शून्य __attribute__((noreturn)) sbi_hart_hang(शून्य)
{
	यावत् (1)
		wfi();
	__builtin_unreachable();
}

शून्य __attribute__((noreturn))
sbi_hart_switch_बहुलकम्(अचिह्नित दीर्घ arg0, अचिह्नित दीर्घ arg1,
		     अचिह्नित दीर्घ अग्र_addr, अचिह्नित दीर्घ अग्र_बहुलकम्,
		     bool अग्र_virt)
{
#यदि __riscv_xlen == 32
	अचिह्नित दीर्घ val, valH;
#अन्यथा
	अचिह्नित दीर्घ val;
#यदि_अन्तः

	विकल्प (अग्र_बहुलकम्) {
	पक्ष PRV_M:
		भङ्ग;
	पक्ष PRV_S:
		यदि (!misa_extension('S'))
			sbi_hart_hang();
		भङ्ग;
	पक्ष PRV_U:
		यदि (!misa_extension('U'))
			sbi_hart_hang();
		भङ्ग;
	स्वभाव:
		sbi_hart_hang();
	}

	val = csr_read(CSR_MSTATUS);
	val = निवेशय_क्षेत्रम्(val, MSTATUS_MPP, अग्र_बहुलकम्);
	val = निवेशय_क्षेत्रम्(val, MSTATUS_MPIE, 0);
#यदि __riscv_xlen == 32
	यदि (misa_extension('H')) {
		valH = csr_read(CSR_MSTATUSH);
		valH = निवेशय_क्षेत्रम्(valH, MSTATUSH_MPV, अग्र_virt);
		csr_write(CSR_MSTATUSH, valH);
	}
#अन्यथा
	यदि (misa_extension('H'))
		val = निवेशय_क्षेत्रम्(val, MSTATUS_MPV, अग्र_virt);
#यदि_अन्तः
	csr_write(CSR_MSTATUS, val);
	csr_write(CSR_MEPC, अग्र_addr);

	यदि (अग्र_बहुलकम् == PRV_S) {
		यदि (अग्र_virt) {
			csr_write(CSR_VSTVEC, अग्र_addr);
			csr_write(CSR_VSSCRATCH, 0);
			csr_write(CSR_VSIE, 0);
			csr_write(CSR_VSATP, 0);
		} अन्यथा {
			csr_write(CSR_STVEC, अग्र_addr);
			csr_write(CSR_SSCRATCH, 0);
			csr_write(CSR_SIE, 0);
			csr_write(CSR_SATP, 0);
		}
	} अन्यथा यदि (अग्र_बहुलकम् == PRV_U) {
		यदि (misa_extension('N')) {
			csr_write(CSR_UTVEC, अग्र_addr);
			csr_write(CSR_USCRATCH, 0);
			csr_write(CSR_UIE, 0);
		}
	}

	पञ्जिका अचिह्नित दीर्घ a0 asm("a0") = arg0;
	पञ्जिका अचिह्नित दीर्घ a1 asm("a1") = arg1;
	__asm__ __volatile__("mret" : : "r"(a0), "r"(a1));
	__builtin_unreachable();
}
