/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_barrier.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/riscv_fp.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_csr_detect.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hart_pmp.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_pmu.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_trap.h>

خارجی خالی __sbi_expected_trap(خالی);
خارجی خالی __sbi_expected_trap_hext(خالی);

خالی (*sbi_hart_expected_trap)(خالی) = &__sbi_expected_trap;

غیرعلامتی طویل hart_features_انحراف;

ساکن خالی mstatus_init(ساخت sbi_scratch *scratch)
{
	صحیح cidx;
	غیرعلامتی طویل mstatus_val = 0;
	غیرعلامتی صحیح mhpm_نقاب = sbi_hart_mhpm_نقاب(scratch);
	uint64_t mhpmevent_init_val = 0;
	uint64_t menvcfg_val, mstateen_val;

	/* Enable FPU */
	اگر (misa_extension('D') || misa_extension('F'))
		mstatus_val |=  MSTATUS_FS;

	/* Enable Vector context */
	اگر (misa_extension('V'))
		mstatus_val |=  MSTATUS_VS;

	csr_write(CSR_MSTATUS, mstatus_val);

	/* Disable user mode usage of all perf counters except TM */
	اگر (misa_extension('S') &&
	    sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_SCOUNTEREN, 0x02);

	/**
	 * OpenSBI doesn't use any PMU counters in M-mode.
	 * Supervisor mode usage for all counters are enabled by default
	 * But counters will not run until mcountinhibit is set.
	 */
	اگر (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	/* All programmable counters will start running at runtime after S-mode request */
	اگر (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	/**
	 * The mhpmeventn[h] CSR should be initialized with interrupt disabled
	 * and inhibited running in M-mode during init.
	 */
	mhpmevent_init_val |= (MHPMEVENT_کا | MHPMEVENT_MINH);
	برائے (cidx = 0; cidx <= 28; cidx++) {
		اگر (!(mhpm_نقاب & 1UL << (cidx + 3)))
			جاری;
#اگر __riscv_xlen == 32
		csr_write_عدد(CSR_MHPMEVENT3 + cidx,
			       mhpmevent_init_val & 0xFFFFFFFF);
		اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
			csr_write_عدد(CSR_MHPMEVENT3H + cidx,
				      mhpmevent_init_val >> بٹ_فی_LONG);
#ورنہ
		csr_write_عدد(CSR_MHPMEVENT3 + cidx, mhpmevent_init_val);
#اگر_ختم
	}

	اگر (misa_extension('H'))
		csr_write(CSR_HSTATUS, 0);

	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMSTATEEN)) {
		mstateen_val = 0;
		mstateen_val |= SMSTATEEN_STATEN;
		mstateen_val |= SMSTATEEN0_CONTEXT;
		mstateen_val |= SMSTATEEN0_HSENVCFG;

		اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA))
			mstateen_val |= (SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);
		ورنہ
			mstateen_val &= ~(SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);

		اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA) ||
		    sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCSRIND))
			mstateen_val |= (SMSTATEEN0_SVSLCT);
		ورنہ
			mstateen_val &= ~(SMSTATEEN0_SVSLCT);

		اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCTR))
			mstateen_val |= SMSTATEEN0_CTR;
		ورنہ
			mstateen_val &= ~SMSTATEEN0_CTR;

		اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
			mstateen_val |= SMSTATEEN0_SRMCFG;
		ورنہ
			mstateen_val &= ~SMSTATEEN0_SRMCFG;

		csr_write64(CSR_MSTATEEN0, mstateen_val);
		csr_write64(CSR_MSTATEEN1, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN2, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN3, SMSTATEEN_STATEN);
	}

	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSSTATEEN)) {
		اگر (misa_extension('S')) {
			csr_write(CSR_SSTATEEN0, 0);
			csr_write(CSR_SSTATEEN1, 0);
			csr_write(CSR_SSTATEEN2, 0);
			csr_write(CSR_SSTATEEN3, 0);
		}
		اگر (misa_extension('H')) {
			csr_write64(CSR_HSTATEEN0, (uint64_t)0);
			csr_write64(CSR_HSTATEEN1, (uint64_t)0);
			csr_write64(CSR_HSTATEEN2, (uint64_t)0);
			csr_write64(CSR_HSTATEEN3, (uint64_t)0);
		}
	}

	اگر (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_12) {
		menvcfg_val = csr_read64(CSR_MENVCFG);

		/* Disable HW A/D updating by default */
		menvcfg_val &= ~ENVCFG_ADUE;

		/* Disable double trap by default */
		menvcfg_val &= ~ENVCFG_DTE;

#تعریف_کرو __set_menvcfg_ext(__ext, __bits)				\
		اگر (sbi_hart_has_extension(scratch, __ext))		\
			menvcfg_val |= __bits;

		/*
		 * Enable access to extensions if they are present in the
		 * hardware or in the device tree.
		 */

		__set_menvcfg_ext(SBI_HART_EXT_ZICBOZ, ENVCFG_CBZE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM, ENVCFG_CBCFE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM,
				  ENVCFG_CBIE_INV << ENVCFG_CBIE_سرکاؤ)
#اگر __riscv_xlen > 32
		__set_menvcfg_ext(SBI_HART_EXT_SVPBMT, ENVCFG_PBMTE)
#اگر_ختم
		__set_menvcfg_ext(SBI_HART_EXT_SSTC, ENVCFG_STCE)
		__set_menvcfg_ext(SBI_HART_EXT_SMCDELEG, ENVCFG_CDE);

		/*
		 * Assume only Svadu is supported when it is the only extension
		 * present in the ISA string. Svade is assumed when neither are
		 * present. When both are present we must default to Svade (see
		 * the zero reset value of FWFT.PTE_AD_HW_UPDATING).
		 */
		اگر (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SVADE))
			__set_menvcfg_ext(SBI_HART_EXT_SVADU, ENVCFG_ADUE);

#تعریف_منسوخ __set_menvcfg_ext

		csr_write64(CSR_MENVCFG, menvcfg_val);

		/* Enable S-mode access to seed CSR */
		اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
			csr_سیٹ(CSR_MSECCFG, MSECCFG_SSEED);
			csr_clear(CSR_MSECCFG, MSECCFG_USEED);
		}
	}

	/* Disable all interrupts */
	csr_write(CSR_MIE, 0);

	/* Disable S-mode paging */
	اگر (misa_extension('S'))
		csr_write(CSR_SATP, 0);
}

ساکن صحیح fp_init(ساخت sbi_scratch *scratch)
{
#اگر_تعریف_شدہ __riscv_flen
	صحیح i;
#اگر_ختم

	اگر (!misa_extension('D') && !misa_extension('F'))
		واپس 0;

	اگر (!(csr_read(CSR_MSTATUS) & MSTATUS_FS))
		واپس SBI_EINVAL;

#اگر_تعریف_شدہ __riscv_flen
	برائے (i = 0; i < 32; i++)
		init_fp_reg(i);
	csr_write(CSR_FCSR, 0);
#اگر_ختم

	واپس 0;
}

خالی sbi_hart_delegation_dump(ساخت sbi_scratch *scratch,
			      ثابت حرف *سابقہ, ثابت حرف *suffix)
{
	اگر (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist*/
		واپس;

	sbi_printf("%sMIDELEG%s: 0x%" PRILX "\n",
		   سابقہ, suffix, csr_read(CSR_MIDELEG));
	sbi_printf("%sMEDELEG%s: 0x%" PRILX "\n",
		   سابقہ, suffix, csr_read(CSR_MEDELEG));
}

غیرعلامتی صحیح sbi_hart_mhpm_نقاب(ساخت sbi_scratch *scratch)
{
	ساخت sbi_hart_features *hfeatures =
			sbi_scratch_انحراف_ptr(scratch, hart_features_انحراف);

	واپس hfeatures->mhpm_نقاب;
}

غیرعلامتی صحیح sbi_hart_mhpm_بٹ(ساخت sbi_scratch *scratch)
{
	ساخت sbi_hart_features *hfeatures =
			sbi_scratch_انحراف_ptr(scratch, hart_features_انحراف);

	واپس hfeatures->mhpm_بٹ;
}

صحیح sbi_hart_priv_نسخہ(ساخت sbi_scratch *scratch)
{
	ساخت sbi_hart_features *hfeatures =
			sbi_scratch_انحراف_ptr(scratch, hart_features_انحراف);

	واپس hfeatures->priv_نسخہ;
}

خالی sbi_hart_get_priv_نسخہ_str(ساخت sbi_scratch *scratch,
				   حرف *نسخہ_str, صحیح nvstr)
{
	حرف *عارضی;
	ساخت sbi_hart_features *hfeatures =
			sbi_scratch_انحراف_ptr(scratch, hart_features_انحراف);

	انتخاب (hfeatures->priv_نسخہ) {
	حالت SBI_HART_PRIV_VER_1_10:
		عارضی = "v1.10";
		توڑو;
	حالت SBI_HART_PRIV_VER_1_11:
		عارضی = "v1.11";
		توڑو;
	حالت SBI_HART_PRIV_VER_1_12:
		عارضی = "v1.12";
		توڑو;
	طےشدہ:
		عارضی = "unknown";
		توڑو;
	}

	sbi_snprintf(نسخہ_str, nvstr, "%s", عارضی);
}

ساکن اندرونی خالی __sbi_hart_update_extension(
					ساخت sbi_hart_features *hfeatures,
					شمار sbi_hart_extensions ext,
					bool enable)
{
	اگر (enable)
		__set_bit(ext, hfeatures->extensions);
	ورنہ
		__clear_bit(ext, hfeatures->extensions);
}

/**
 * Enable/Disable a particular hart extension
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @param enable new state of hart extension
 */
خالی sbi_hart_update_extension(ساخت sbi_scratch *scratch,
			       شمار sbi_hart_extensions ext,
			       bool enable)
{
	ساخت sbi_hart_features *hfeatures =
			sbi_scratch_انحراف_ptr(scratch, hart_features_انحراف);

	__sbi_hart_update_extension(hfeatures, ext, enable);
}

/**
 * Check whether a particular hart extension is available
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @returns true (available) or false (not available)
 */
bool sbi_hart_has_extension(ساخت sbi_scratch *scratch,
			    شمار sbi_hart_extensions ext)
{
	ساخت sbi_hart_features *hfeatures =
			sbi_scratch_انحراف_ptr(scratch, hart_features_انحراف);

	اگر (__test_bit(ext, hfeatures->extensions))
		واپس true;
	ورنہ
		واپس false;
}

#تعریف_کرو __SBI_HART_EXT_DATA(_name, _id) {	\
	.name = #_name,				\
	.id = _id,				\
}

ثابت ساخت sbi_hart_ext_ڈیٹا sbi_hart_ext[] = {
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

_Static_assert(SBI_HART_EXT_MAX == array_سائز(sbi_hart_ext),
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
خالی sbi_hart_get_extensions_str(ساخت sbi_scratch *scratch,
				 حرف *extensions_str, صحیح nestr)
{
	ساخت sbi_hart_features *hfeatures =
			sbi_scratch_انحراف_ptr(scratch, hart_features_انحراف);
	صحیح انحراف = 0, ext = 0;

	اگر (!extensions_str || nestr <= 0)
		واپس;
	sbi_memset(extensions_str, 0, nestr);

	for_each_سیٹ_ثنائی_ہندسہ(ext, hfeatures->extensions, SBI_HART_EXT_MAX) {
		اگر (انحراف + sbi_strlen(sbi_hart_ext[ext].name) + 1 > nestr) {
			sbi_printf("%s:extension name is longer than buffer (error)\n", __func__);
			توڑو;
		}
		sbi_snprintf(extensions_str + انحراف,
				 nestr - انحراف,
				 "%s,", sbi_hart_ext[ext].name);
		انحراف = انحراف + sbi_strlen(sbi_hart_ext[ext].name) + 1;
	}

	اگر (انحراف)
		extensions_str[انحراف - 1] = '\0';
	ورنہ
		sbi_strncpy(extensions_str, "none", nestr);
}

/**
 * Check whether a particular CSR is present on the HART
 *
 * @param scratch pointer to the HART scratch space
 * @param csr the CSR number to check
 */
bool sbi_hart_has_csr(ساخت sbi_scratch *scratch, شمار sbi_hart_csrs csr)
{
	ساخت sbi_hart_features *hfeatures =
			sbi_scratch_انحراف_ptr(scratch, hart_features_انحراف);

	واپس __test_bit(csr, hfeatures->csrs);
}

ساکن غیرعلامتی طویل hart_pmp_get_allowed_addr(خالی)
{
	غیرعلامتی طویل val = 0;
	ساخت sbi_trap_info trap = {0};

	csr_write_allowed(CSR_PMPCFG0, &trap, 0);
	اگر (trap.cause)
		واپس 0;

	csr_write_allowed(CSR_PMPADDR0, &trap, PMP_ADDR_نقاب);
	اگر (!trap.cause) {
		val = csr_read_allowed(CSR_PMPADDR0, &trap);
		اگر (trap.cause)
			val = 0;
	}

	واپس val;
}

ساکن صحیح hart_mhpm_get_allowed_بٹ(خالی)
{
	غیرعلامتی طویل val = ~(0UL);
	ساخت sbi_trap_info trap = {0};
	صحیح عدد_بٹ = 0;

	/**
	 * It is assumed that platforms will implement same number of bits for
	 * all the performance counters including mcycle/minstret.
	 */
	csr_write_allowed(CSR_MHPMCOUNTER3, &trap, val);
	اگر (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3, &trap);
		اگر (trap.cause)
			واپس 0;
	}
	عدد_بٹ = sbi_fls(val) + 1;
#اگر __riscv_xlen == 32
	csr_write_allowed(CSR_MHPMCOUNTER3H, &trap, val);
	اگر (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3H, &trap);
		اگر (trap.cause)
			واپس عدد_بٹ;
	}
	عدد_بٹ += sbi_fls(val) + 1;

#اگر_ختم

	واپس عدد_بٹ;
}

ساکن صحیح hart_detect_features(ساخت sbi_scratch *scratch, bool cold_boot)
{
	ساخت sbi_trap_info trap = {0};
	ساخت sbi_hart_features *hfeatures =
		sbi_scratch_انحراف_ptr(scratch, hart_features_انحراف);
	غیرعلامتی طویل val, oldval;
	صحیح rc;

	/* If hart features already detected then do nothing */
	اگر (hfeatures->detected)
		واپس 0;

	/*
	 * Parse device tree extensions early, before any trap-based checks.
	 * Needed to detect Smrnmi and install NMI handlers before CSR probes
	 * that may trigger traps.
	 */
	rc = sbi_platform_extensions_init(sbi_platform_ptr(scratch), cold_boot);
	اگر (rc)
		واپس rc;

	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMRNMI)) {
		ثابت ساخت sbi_platform *plat = sbi_platform_thishart_ptr();
		ثابت ساخت sbi_platform_operations *ops = sbi_platform_ops(plat);
		خارجی خالی _trap_rnmi_handler(خالی);
		خارجی خالی _trap_handler(خالی);

		اگر (!ops || !ops->smrnmi_handlers_init)
			sbi_panic("Smrnmi detected, but platform lacks smrnmi_handlers_init callback\n");

		/* Reuse _trap_handler for the RNME slot since RNME is taken
		 * as a regular M-mode trap with NMIE=0. */
		ops->smrnmi_handlers_init(_trap_rnmi_handler, _trap_handler);

		/* Initialize MNSCRATCH for the RNMI handler */
		csr_write(CSR_MNSCRATCH, scratch);

		/* Enable NMIs */
		csr_سیٹ(CSR_MNSTATUS, MNSTATUS_NMIE);
	}

#تعریف_کرو __check_hpm_csr(__csr, __mask) 					  \
	oldval = csr_read_allowed(__csr, &trap);			  \
	اگر (!trap.cause) {						  \
		csr_write_allowed(__csr, &trap, 1UL);			  \
		اگر (!trap.cause && csr_تبادلہ(__csr, oldval) == 1UL) {	  \
			(hfeatures->__mask) |= 1 << (__csr - CSR_MCYCLE); \
		}							  \
	}

#تعریف_کرو __check_hpm_csr_2(__csr, __mask)	 		  \
	__check_hpm_csr(__csr + 0, __mask)	 		  \
	__check_hpm_csr(__csr + 1, __mask)
#تعریف_کرو __check_hpm_csr_4(__csr, __mask)	 		  \
	__check_hpm_csr_2(__csr + 0, __mask) 			  \
	__check_hpm_csr_2(__csr + 2, __mask)
#تعریف_کرو __check_hpm_csr_8(__csr, __mask)	 		  \
	__check_hpm_csr_4(__csr + 0, __mask) 			  \
	__check_hpm_csr_4(__csr + 4, __mask)
#تعریف_کرو __check_hpm_csr_16(__csr, __mask)	 		  \
	__check_hpm_csr_8(__csr + 0, __mask) 			  \
	__check_hpm_csr_8(__csr + 8, __mask)

#تعریف_کرو __check_csr(__csr, __rdonly, __wrval, __field, __skip)		\
	oldval = csr_read_allowed(__csr, &trap);			\
	اگر (!trap.cause) {						\
		اگر (__rdonly) {						\
			(hfeatures->__field)++;				\
		} ورنہ {						\
			csr_write_allowed(__csr, &trap, __wrval);	\
			اگر (!trap.cause) {				\
				اگر ((csr_تبادلہ(__csr, oldval) & __wrval) == __wrval)	\
					(hfeatures->__field)++;		\
				ورنہ					\
					جاؤ __skip;			\
			} ورنہ {					\
				جاؤ __skip;				\
			}						\
		}							\
	} ورنہ {							\
		جاؤ __skip;						\
	}
#تعریف_کرو __check_csr_2(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 1, __rdonly, __wrval, __field, __skip)
#تعریف_کرو __check_csr_4(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 2, __rdonly, __wrval, __field, __skip)
#تعریف_کرو __check_csr_8(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 4, __rdonly, __wrval, __field, __skip)
#تعریف_کرو __check_csr_16(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 8, __rdonly, __wrval, __field, __skip)
#تعریف_کرو __check_csr_32(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 16, __rdonly, __wrval, __field, __skip)
#تعریف_کرو __check_csr_64(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 32, __rdonly, __wrval, __field, __skip)

	/**
	 * Detect the allowed address bits & granularity. At least PMPADDR0
	 * should be implemented.
	 */
	val = hart_pmp_get_allowed_addr();
	اگر (val) {
		hfeatures->pmp_log2gran = sbi_ffs(val) + 2;
		hfeatures->pmp_addr_بٹ = sbi_fls(val) + 1;
		/* Detect number of PMP regions. At least PMPADDR0 should be implemented*/
		__check_csr_64(CSR_PMPADDR0, 0, val, pmp_گنتی, __pmp_skip);
	}
__pmp_skip:
	/* Detect number of MHPM counters */
	__check_hpm_csr(CSR_MHPMCOUNTER3, mhpm_نقاب);
	hfeatures->mhpm_بٹ = hart_mhpm_get_allowed_بٹ();
	__check_hpm_csr_4(CSR_MHPMCOUNTER4, mhpm_نقاب);
	__check_hpm_csr_8(CSR_MHPMCOUNTER8, mhpm_نقاب);
	__check_hpm_csr_16(CSR_MHPMCOUNTER16, mhpm_نقاب);

	/**
	 * No need to check for MHPMCOUNTERH for RV32 as they are expected to be
	 * implemented if MHPMCOUNTER is implemented.
	 */
#تعریف_منسوخ __check_csr_64
#تعریف_منسوخ __check_csr_32
#تعریف_منسوخ __check_csr_16
#تعریف_منسوخ __check_csr_8
#تعریف_منسوخ __check_csr_4
#تعریف_منسوخ __check_csr_2
#تعریف_منسوخ __check_csr


#تعریف_کرو __check_priv(__csr, __base_priv, __priv)			\
	val = csr_read_allowed(__csr, &trap);				\
	اگر (!trap.cause && (hfeatures->priv_نسخہ >= __base_priv)) {	\
		hfeatures->priv_نسخہ = __priv;			\
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

#تعریف_منسوخ __check_priv_csr

#تعریف_کرو __check_ext_csr(__base_priv, __csr, __ext)			\
	اگر (hfeatures->priv_نسخہ >= __base_priv) {			\
		csr_read_allowed(__csr, &trap);				\
		اگر (!trap.cause)					\
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

#تعریف_منسوخ __check_ext_csr

#تعریف_کرو __check_csr_existence(__csr, __csr_id)				\
	csr_read_allowed(__csr, &trap);					\
	اگر (!trap.cause)						\
		__set_bit(__csr_id, hfeatures->csrs);

	__check_csr_existence(CSR_چکر, SBI_HART_CSR_چکر);
	__check_csr_existence(CSR_وقت, SBI_HART_CSR_وقت);
	__check_csr_existence(CSR_INSTRET, SBI_HART_CSR_INSTRET);

#تعریف_منسوخ __check_csr_existence

	/* Zicntr should only be detected using traps */
	__sbi_hart_update_extension(hfeatures, SBI_HART_EXT_ZICNTR,
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_چکر) &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_وقت)  &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET));

	/* Extensions implied by other extensions and features */
	اگر (hfeatures->mhpm_نقاب)
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
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP) &&
	    (csr_read(CSR_MSECCFG) & MSECCFG_MML))
		واپس SBI_EILL;

	واپس 0;
}

صحیح sbi_hart_reinit(ساخت sbi_scratch *scratch)
{
	صحیح rc;

	mstatus_init(scratch);

	rc = fp_init(scratch);
	اگر (rc)
		واپس rc;

	واپس 0;
}

صحیح sbi_hart_init(ساخت sbi_scratch *scratch, bool cold_boot)
{
	صحیح rc;

	/*
	 * Clear mip CSR before proceeding with init to avoid any spurious
	 * external interrupts in S-mode.
	 */
	csr_write(CSR_MIP, 0);

	اگر (cold_boot) {
		اگر (misa_extension('H'))
			sbi_hart_expected_trap = &__sbi_expected_trap_hext;

		hart_features_انحراف = sbi_scratch_alloc_انحراف(
					حجم(ساخت sbi_hart_features));
		اگر (!hart_features_انحراف)
			واپس SBI_ENOMEM;
	}

	rc = hart_detect_features(scratch, cold_boot);
	اگر (rc)
		واپس rc;

	اگر (cold_boot) {
		rc = sbi_hart_pmp_init(scratch);
		اگر (rc)
			واپس rc;
	}

	واپس sbi_hart_reinit(scratch);
}

خالی __attribute__((noreturn)) sbi_hart_hang(خالی)
{
	جب (1)
		wfi();
	__builtin_unreachable();
}

خالی __attribute__((noreturn))
sbi_hart_switch_منوال(غیرعلامتی طویل arg0, غیرعلامتی طویل arg1,
		     غیرعلامتی طویل اگلا_addr, غیرعلامتی طویل اگلا_منوال,
		     bool اگلا_virt)
{
#اگر __riscv_xlen == 32
	غیرعلامتی طویل val, valH;
#ورنہ
	غیرعلامتی طویل val;
#اگر_ختم

	انتخاب (اگلا_منوال) {
	حالت PRV_M:
		توڑو;
	حالت PRV_S:
		اگر (!misa_extension('S'))
			sbi_hart_hang();
		توڑو;
	حالت PRV_U:
		اگر (!misa_extension('U'))
			sbi_hart_hang();
		توڑو;
	طےشدہ:
		sbi_hart_hang();
	}

	val = csr_read(CSR_MSTATUS);
	val = داخل_کرو_میدان(val, MSTATUS_MPP, اگلا_منوال);
	val = داخل_کرو_میدان(val, MSTATUS_MPIE, 0);
#اگر __riscv_xlen == 32
	اگر (misa_extension('H')) {
		valH = csr_read(CSR_MSTATUSH);
		valH = داخل_کرو_میدان(valH, MSTATUSH_MPV, اگلا_virt);
		csr_write(CSR_MSTATUSH, valH);
	}
#ورنہ
	اگر (misa_extension('H'))
		val = داخل_کرو_میدان(val, MSTATUS_MPV, اگلا_virt);
#اگر_ختم
	csr_write(CSR_MSTATUS, val);
	csr_write(CSR_MEPC, اگلا_addr);

	اگر (اگلا_منوال == PRV_S) {
		اگر (اگلا_virt) {
			csr_write(CSR_VSTVEC, اگلا_addr);
			csr_write(CSR_VSSCRATCH, 0);
			csr_write(CSR_VSIE, 0);
			csr_write(CSR_VSATP, 0);
		} ورنہ {
			csr_write(CSR_STVEC, اگلا_addr);
			csr_write(CSR_SSCRATCH, 0);
			csr_write(CSR_SIE, 0);
			csr_write(CSR_SATP, 0);
		}
	} ورنہ اگر (اگلا_منوال == PRV_U) {
		اگر (misa_extension('N')) {
			csr_write(CSR_UTVEC, اگلا_addr);
			csr_write(CSR_USCRATCH, 0);
			csr_write(CSR_UIE, 0);
		}
	}

	اندراج غیرعلامتی طویل a0 asm("a0") = arg0;
	اندراج غیرعلامتی طویل a1 asm("a1") = arg1;
	__asm__ __volatile__("mret" : : "r"(a0), "r"(a1));
	__builtin_unreachable();
}
