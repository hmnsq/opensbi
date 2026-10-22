/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_barrier.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/riscv_fp.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_csr_detect.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_hart_pmp.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_pmu.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_trap.h>

خارجي فراغ __sbi_expected_trap(فراغ);
خارجي فراغ __sbi_expected_trap_hext(فراغ);

فراغ (*sbi_hart_expected_trap)(فراغ) = &__sbi_expected_trap;

بنانشانی لما hart_features_وتھ;

ساكن فراغ mstatus_init(بنية sbi_scratch *scratch)
{
	صحيح cidx;
	بنانشانی لما mstatus_val = 0;
	بنانشانی صحيح mhpm_نقاب = sbi_hart_mhpm_نقاب(scratch);
	uint64_t mhpmevent_init_val = 0;
	uint64_t menvcfg_val, mstateen_val;

	/* Enable FPU */
	إذا (misa_extension('D') || misa_extension('F'))
		mstatus_val |=  MSTATUS_FS;

	/* Enable Vector context */
	إذا (misa_extension('V'))
		mstatus_val |=  MSTATUS_VS;

	csr_write(CSR_MSTATUS, mstatus_val);

	/* Disable user mode usage of all perf counters except TM */
	إذا (misa_extension('S') &&
	    sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_SCOUNTEREN, 0x02);

	/**
	 * OpenSBI doesn't use any PMU counters in M-mode.
	 * Supervisor mode usage for all counters are enabled by default
	 * But counters will not run until mcountinhibit is set.
	 */
	إذا (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	/* All programmable counters will start running at runtime after S-mode request */
	إذا (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	/**
	 * The mhpmeventn[h] CSR should be initialized with interrupt disabled
	 * and inhibited running in M-mode during init.
	 */
	mhpmevent_init_val |= (MHPMEVENT_دا | MHPMEVENT_MINH);
	لكل (cidx = 0; cidx <= 28; cidx++) {
		إذا (!(mhpm_نقاب & 1UL << (cidx + 3)))
			استمر;
#إذا __riscv_xlen == 32
		csr_write_عدد(CSR_MHPMEVENT3 + cidx,
			       mhpmevent_init_val & 0xFFFFFFFF);
		إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
			csr_write_عدد(CSR_MHPMEVENT3H + cidx,
				      mhpmevent_init_val >> بٹ_فی_LONG);
#وإلا
		csr_write_عدد(CSR_MHPMEVENT3 + cidx, mhpmevent_init_val);
#جے_انت
	}

	إذا (misa_extension('H'))
		csr_write(CSR_HSTATUS, 0);

	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMSTATEEN)) {
		mstateen_val = 0;
		mstateen_val |= SMSTATEEN_STATEN;
		mstateen_val |= SMSTATEEN0_CONTEXT;
		mstateen_val |= SMSTATEEN0_HSENVCFG;

		إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA))
			mstateen_val |= (SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);
		وإلا
			mstateen_val &= ~(SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);

		إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA) ||
		    sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCSRIND))
			mstateen_val |= (SMSTATEEN0_SVSLCT);
		وإلا
			mstateen_val &= ~(SMSTATEEN0_SVSLCT);

		إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCTR))
			mstateen_val |= SMSTATEEN0_CTR;
		وإلا
			mstateen_val &= ~SMSTATEEN0_CTR;

		إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
			mstateen_val |= SMSTATEEN0_SRMCFG;
		وإلا
			mstateen_val &= ~SMSTATEEN0_SRMCFG;

		csr_write64(CSR_MSTATEEN0, mstateen_val);
		csr_write64(CSR_MSTATEEN1, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN2, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN3, SMSTATEEN_STATEN);
	}

	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSSTATEEN)) {
		إذا (misa_extension('S')) {
			csr_write(CSR_SSTATEEN0, 0);
			csr_write(CSR_SSTATEEN1, 0);
			csr_write(CSR_SSTATEEN2, 0);
			csr_write(CSR_SSTATEEN3, 0);
		}
		إذا (misa_extension('H')) {
			csr_write64(CSR_HSTATEEN0, (uint64_t)0);
			csr_write64(CSR_HSTATEEN1, (uint64_t)0);
			csr_write64(CSR_HSTATEEN2, (uint64_t)0);
			csr_write64(CSR_HSTATEEN3, (uint64_t)0);
		}
	}

	إذا (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_12) {
		menvcfg_val = csr_read64(CSR_MENVCFG);

		/* Disable HW A/D updating by default */
		menvcfg_val &= ~ENVCFG_ADUE;

		/* Disable double trap by default */
		menvcfg_val &= ~ENVCFG_DTE;

#تعریف_کرو __set_menvcfg_ext(__ext, __bits)				\
		إذا (sbi_hart_has_extension(scratch, __ext))		\
			menvcfg_val |= __bits;

		/*
		 * Enable access to extensions if they are present in the
		 * hardware or in the device tree.
		 */

		__set_menvcfg_ext(SBI_HART_EXT_ZICBOZ, ENVCFG_CBZE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM, ENVCFG_CBCFE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM,
				  ENVCFG_CBIE_INV << ENVCFG_CBIE_کھسکاؤ)
#إذا __riscv_xlen > 32
		__set_menvcfg_ext(SBI_HART_EXT_SVPBMT, ENVCFG_PBMTE)
#جے_انت
		__set_menvcfg_ext(SBI_HART_EXT_SSTC, ENVCFG_STCE)
		__set_menvcfg_ext(SBI_HART_EXT_SMCDELEG, ENVCFG_CDE);

		/*
		 * Assume only Svadu is supported when it is the only extension
		 * present in the ISA string. Svade is assumed when neither are
		 * present. When both are present we must default to Svade (see
		 * the zero reset value of FWFT.PTE_AD_HW_UPDATING).
		 */
		إذا (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SVADE))
			__set_menvcfg_ext(SBI_HART_EXT_SVADU, ENVCFG_ADUE);

#تعریف_رد __set_menvcfg_ext

		csr_write64(CSR_MENVCFG, menvcfg_val);

		/* Enable S-mode access to seed CSR */
		إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
			csr_سیٹ(CSR_MSECCFG, MSECCFG_SSEED);
			csr_clear(CSR_MSECCFG, MSECCFG_USEED);
		}
	}

	/* Disable all interrupts */
	csr_write(CSR_MIE, 0);

	/* Disable S-mode paging */
	إذا (misa_extension('S'))
		csr_write(CSR_SATP, 0);
}

ساكن صحيح fp_init(بنية sbi_scratch *scratch)
{
#جے_تعریف_شدہ __riscv_flen
	صحيح i;
#جے_انت

	إذا (!misa_extension('D') && !misa_extension('F'))
		أرجع 0;

	إذا (!(csr_read(CSR_MSTATUS) & MSTATUS_FS))
		أرجع SBI_EINVAL;

#جے_تعریف_شدہ __riscv_flen
	لكل (i = 0; i < 32; i++)
		init_fp_reg(i);
	csr_write(CSR_FCSR, 0);
#جے_انت

	أرجع 0;
}

فراغ sbi_hart_delegation_dump(بنية sbi_scratch *scratch,
			      ثابت حرف *اگیتر, ثابت حرف *suffix)
{
	إذا (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist*/
		أرجع;

	sbi_printf("%sMIDELEG%s: 0x%" PRILX "\n",
		   اگیتر, suffix, csr_read(CSR_MIDELEG));
	sbi_printf("%sMEDELEG%s: 0x%" PRILX "\n",
		   اگیتر, suffix, csr_read(CSR_MEDELEG));
}

بنانشانی صحيح sbi_hart_mhpm_نقاب(بنية sbi_scratch *scratch)
{
	بنية sbi_hart_features *hfeatures =
			sbi_scratch_وتھ_ptr(scratch, hart_features_وتھ);

	أرجع hfeatures->mhpm_نقاب;
}

بنانشانی صحيح sbi_hart_mhpm_بٹ(بنية sbi_scratch *scratch)
{
	بنية sbi_hart_features *hfeatures =
			sbi_scratch_وتھ_ptr(scratch, hart_features_وتھ);

	أرجع hfeatures->mhpm_بٹ;
}

صحيح sbi_hart_priv_نسخہ(بنية sbi_scratch *scratch)
{
	بنية sbi_hart_features *hfeatures =
			sbi_scratch_وتھ_ptr(scratch, hart_features_وتھ);

	أرجع hfeatures->priv_نسخہ;
}

فراغ sbi_hart_get_priv_نسخہ_str(بنية sbi_scratch *scratch,
				   حرف *نسخہ_str, صحيح nvstr)
{
	حرف *عارضی;
	بنية sbi_hart_features *hfeatures =
			sbi_scratch_وتھ_ptr(scratch, hart_features_وتھ);

	بدل (hfeatures->priv_نسخہ) {
	حالة SBI_HART_PRIV_VER_1_10:
		عارضی = "v1.10";
		اكسر;
	حالة SBI_HART_PRIV_VER_1_11:
		عارضی = "v1.11";
		اكسر;
	حالة SBI_HART_PRIV_VER_1_12:
		عارضی = "v1.12";
		اكسر;
	افتراضي:
		عارضی = "unknown";
		اكسر;
	}

	sbi_snprintf(نسخہ_str, nvstr, "%s", عارضی);
}

ساكن مضمن فراغ __sbi_hart_update_extension(
					بنية sbi_hart_features *hfeatures,
					تعداد sbi_hart_extensions ext,
					bool enable)
{
	إذا (enable)
		__set_bit(ext, hfeatures->extensions);
	وإلا
		__clear_bit(ext, hfeatures->extensions);
}

/**
 * Enable/Disable a particular hart extension
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @param enable new state of hart extension
 */
فراغ sbi_hart_update_extension(بنية sbi_scratch *scratch,
			       تعداد sbi_hart_extensions ext,
			       bool enable)
{
	بنية sbi_hart_features *hfeatures =
			sbi_scratch_وتھ_ptr(scratch, hart_features_وتھ);

	__sbi_hart_update_extension(hfeatures, ext, enable);
}

/**
 * Check whether a particular hart extension is available
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @returns true (available) or false (not available)
 */
bool sbi_hart_has_extension(بنية sbi_scratch *scratch,
			    تعداد sbi_hart_extensions ext)
{
	بنية sbi_hart_features *hfeatures =
			sbi_scratch_وتھ_ptr(scratch, hart_features_وتھ);

	إذا (__test_bit(ext, hfeatures->extensions))
		أرجع true;
	وإلا
		أرجع false;
}

#تعریف_کرو __SBI_HART_EXT_DATA(_name, _id) {	\
	.name = #_name,				\
	.id = _id,				\
}

ثابت بنية sbi_hart_ext_انکڑے sbi_hart_ext[] = {
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

_Static_assert(SBI_HART_EXT_MAX == array_ناپ(sbi_hart_ext),
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
فراغ sbi_hart_get_extensions_str(بنية sbi_scratch *scratch,
				 حرف *extensions_str, صحيح nestr)
{
	بنية sbi_hart_features *hfeatures =
			sbi_scratch_وتھ_ptr(scratch, hart_features_وتھ);
	صحيح وتھ = 0, ext = 0;

	إذا (!extensions_str || nestr <= 0)
		أرجع;
	sbi_memset(extensions_str, 0, nestr);

	for_each_سیٹ_ثنائی_ہندسہ(ext, hfeatures->extensions, SBI_HART_EXT_MAX) {
		إذا (وتھ + sbi_strlen(sbi_hart_ext[ext].name) + 1 > nestr) {
			sbi_printf("%s:extension name is longer than buffer (error)\n", __func__);
			اكسر;
		}
		sbi_snprintf(extensions_str + وتھ,
				 nestr - وتھ,
				 "%s,", sbi_hart_ext[ext].name);
		وتھ = وتھ + sbi_strlen(sbi_hart_ext[ext].name) + 1;
	}

	إذا (وتھ)
		extensions_str[وتھ - 1] = '\0';
	وإلا
		sbi_strncpy(extensions_str, "none", nestr);
}

/**
 * Check whether a particular CSR is present on the HART
 *
 * @param scratch pointer to the HART scratch space
 * @param csr the CSR number to check
 */
bool sbi_hart_has_csr(بنية sbi_scratch *scratch, تعداد sbi_hart_csrs csr)
{
	بنية sbi_hart_features *hfeatures =
			sbi_scratch_وتھ_ptr(scratch, hart_features_وتھ);

	أرجع __test_bit(csr, hfeatures->csrs);
}

ساكن بنانشانی لما hart_pmp_get_allowed_addr(فراغ)
{
	بنانشانی لما val = 0;
	بنية sbi_trap_info trap = {0};

	csr_write_allowed(CSR_PMPCFG0, &trap, 0);
	إذا (trap.cause)
		أرجع 0;

	csr_write_allowed(CSR_PMPADDR0, &trap, PMP_ADDR_نقاب);
	إذا (!trap.cause) {
		val = csr_read_allowed(CSR_PMPADDR0, &trap);
		إذا (trap.cause)
			val = 0;
	}

	أرجع val;
}

ساكن صحيح hart_mhpm_get_allowed_بٹ(فراغ)
{
	بنانشانی لما val = ~(0UL);
	بنية sbi_trap_info trap = {0};
	صحيح عدد_بٹ = 0;

	/**
	 * It is assumed that platforms will implement same number of bits for
	 * all the performance counters including mcycle/minstret.
	 */
	csr_write_allowed(CSR_MHPMCOUNTER3, &trap, val);
	إذا (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3, &trap);
		إذا (trap.cause)
			أرجع 0;
	}
	عدد_بٹ = sbi_fls(val) + 1;
#إذا __riscv_xlen == 32
	csr_write_allowed(CSR_MHPMCOUNTER3H, &trap, val);
	إذا (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3H, &trap);
		إذا (trap.cause)
			أرجع عدد_بٹ;
	}
	عدد_بٹ += sbi_fls(val) + 1;

#جے_انت

	أرجع عدد_بٹ;
}

ساكن صحيح hart_detect_features(بنية sbi_scratch *scratch, bool cold_boot)
{
	بنية sbi_trap_info trap = {0};
	بنية sbi_hart_features *hfeatures =
		sbi_scratch_وتھ_ptr(scratch, hart_features_وتھ);
	بنانشانی لما val, oldval;
	صحيح rc;

	/* If hart features already detected then do nothing */
	إذا (hfeatures->detected)
		أرجع 0;

	/*
	 * Parse device tree extensions early, before any trap-based checks.
	 * Needed to detect Smrnmi and install NMI handlers before CSR probes
	 * that may trigger traps.
	 */
	rc = sbi_platform_extensions_init(sbi_platform_ptr(scratch), cold_boot);
	إذا (rc)
		أرجع rc;

	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMRNMI)) {
		ثابت بنية sbi_platform *plat = sbi_platform_thishart_ptr();
		ثابت بنية sbi_platform_operations *ops = sbi_platform_ops(plat);
		خارجي فراغ _trap_rnmi_handler(فراغ);
		خارجي فراغ _trap_handler(فراغ);

		إذا (!ops || !ops->smrnmi_handlers_init)
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
	إذا (!trap.cause) {						  \
		csr_write_allowed(__csr, &trap, 1UL);			  \
		إذا (!trap.cause && csr_وٹاندرا(__csr, oldval) == 1UL) {	  \
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
	إذا (!trap.cause) {						\
		إذا (__rdonly) {						\
			(hfeatures->__field)++;				\
		} وإلا {						\
			csr_write_allowed(__csr, &trap, __wrval);	\
			إذا (!trap.cause) {				\
				إذا ((csr_وٹاندرا(__csr, oldval) & __wrval) == __wrval)	\
					(hfeatures->__field)++;		\
				وإلا					\
					اذهب __skip;			\
			} وإلا {					\
				اذهب __skip;				\
			}						\
		}							\
	} وإلا {							\
		اذهب __skip;						\
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
	إذا (val) {
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
#تعریف_رد __check_csr_64
#تعریف_رد __check_csr_32
#تعریف_رد __check_csr_16
#تعریف_رد __check_csr_8
#تعریف_رد __check_csr_4
#تعریف_رد __check_csr_2
#تعریف_رد __check_csr


#تعریف_کرو __check_priv(__csr, __base_priv, __priv)			\
	val = csr_read_allowed(__csr, &trap);				\
	إذا (!trap.cause && (hfeatures->priv_نسخہ >= __base_priv)) {	\
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

#تعریف_رد __check_priv_csr

#تعریف_کرو __check_ext_csr(__base_priv, __csr, __ext)			\
	إذا (hfeatures->priv_نسخہ >= __base_priv) {			\
		csr_read_allowed(__csr, &trap);				\
		إذا (!trap.cause)					\
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

#تعریف_رد __check_ext_csr

#تعریف_کرو __check_csr_existence(__csr, __csr_id)				\
	csr_read_allowed(__csr, &trap);					\
	إذا (!trap.cause)						\
		__set_bit(__csr_id, hfeatures->csrs);

	__check_csr_existence(CSR_چکر, SBI_HART_CSR_چکر);
	__check_csr_existence(CSR_ویلا, SBI_HART_CSR_ویلا);
	__check_csr_existence(CSR_INSTRET, SBI_HART_CSR_INSTRET);

#تعریف_رد __check_csr_existence

	/* Zicntr should only be detected using traps */
	__sbi_hart_update_extension(hfeatures, SBI_HART_EXT_ZICNTR,
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_چکر) &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_ویلا)  &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET));

	/* Extensions implied by other extensions and features */
	إذا (hfeatures->mhpm_نقاب)
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
	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP) &&
	    (csr_read(CSR_MSECCFG) & MSECCFG_MML))
		أرجع SBI_EILL;

	أرجع 0;
}

صحيح sbi_hart_reinit(بنية sbi_scratch *scratch)
{
	صحيح rc;

	mstatus_init(scratch);

	rc = fp_init(scratch);
	إذا (rc)
		أرجع rc;

	أرجع 0;
}

صحيح sbi_hart_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	صحيح rc;

	/*
	 * Clear mip CSR before proceeding with init to avoid any spurious
	 * external interrupts in S-mode.
	 */
	csr_write(CSR_MIP, 0);

	إذا (cold_boot) {
		إذا (misa_extension('H'))
			sbi_hart_expected_trap = &__sbi_expected_trap_hext;

		hart_features_وتھ = sbi_scratch_alloc_وتھ(
					حجم(بنية sbi_hart_features));
		إذا (!hart_features_وتھ)
			أرجع SBI_ENOMEM;
	}

	rc = hart_detect_features(scratch, cold_boot);
	إذا (rc)
		أرجع rc;

	إذا (cold_boot) {
		rc = sbi_hart_pmp_init(scratch);
		إذا (rc)
			أرجع rc;
	}

	أرجع sbi_hart_reinit(scratch);
}

فراغ __attribute__((noreturn)) sbi_hart_hang(فراغ)
{
	طالما (1)
		wfi();
	__builtin_unreachable();
}

فراغ __attribute__((noreturn))
sbi_hart_switch_منوال(بنانشانی لما arg0, بنانشانی لما arg1,
		     بنانشانی لما اگلا_addr, بنانشانی لما اگلا_منوال,
		     bool اگلا_virt)
{
#إذا __riscv_xlen == 32
	بنانشانی لما val, valH;
#وإلا
	بنانشانی لما val;
#جے_انت

	بدل (اگلا_منوال) {
	حالة PRV_M:
		اكسر;
	حالة PRV_S:
		إذا (!misa_extension('S'))
			sbi_hart_hang();
		اكسر;
	حالة PRV_U:
		إذا (!misa_extension('U'))
			sbi_hart_hang();
		اكسر;
	افتراضي:
		sbi_hart_hang();
	}

	val = csr_read(CSR_MSTATUS);
	val = داخل_کرو_میدان(val, MSTATUS_MPP, اگلا_منوال);
	val = داخل_کرو_میدان(val, MSTATUS_MPIE, 0);
#إذا __riscv_xlen == 32
	إذا (misa_extension('H')) {
		valH = csr_read(CSR_MSTATUSH);
		valH = داخل_کرو_میدان(valH, MSTATUSH_MPV, اگلا_virt);
		csr_write(CSR_MSTATUSH, valH);
	}
#وإلا
	إذا (misa_extension('H'))
		val = داخل_کرو_میدان(val, MSTATUS_MPV, اگلا_virt);
#جے_انت
	csr_write(CSR_MSTATUS, val);
	csr_write(CSR_MEPC, اگلا_addr);

	إذا (اگلا_منوال == PRV_S) {
		إذا (اگلا_virt) {
			csr_write(CSR_VSTVEC, اگلا_addr);
			csr_write(CSR_VSSCRATCH, 0);
			csr_write(CSR_VSIE, 0);
			csr_write(CSR_VSATP, 0);
		} وإلا {
			csr_write(CSR_STVEC, اگلا_addr);
			csr_write(CSR_SSCRATCH, 0);
			csr_write(CSR_SIE, 0);
			csr_write(CSR_SATP, 0);
		}
	} وإلا إذا (اگلا_منوال == PRV_U) {
		إذا (misa_extension('N')) {
			csr_write(CSR_UTVEC, اگلا_addr);
			csr_write(CSR_USCRATCH, 0);
			csr_write(CSR_UIE, 0);
		}
	}

	سجل بنانشانی لما a0 asm("a0") = arg0;
	سجل بنانشانی لما a1 asm("a1") = arg1;
	__asm__ __volatile__("mret" : : "r"(a0), "r"(a1));
	__builtin_unreachable();
}
