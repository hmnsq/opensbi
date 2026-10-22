/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_barrier.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_encoding.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_fp.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitops.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_csr_detect.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart_pmp.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_pmu.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>

ବାହ୍ୟ ଶୂନ୍ୟ __sbi_expected_trap(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ __sbi_expected_trap_hext(ଶୂନ୍ୟ);

ଶୂନ୍ୟ (*sbi_hart_expected_trap)(ଶୂନ୍ୟ) = &__sbi_expected_trap;

unsigned long hart_features_offset;

ସ୍ଥିର ଶୂନ୍ୟ mstatus_init(ସଂରଚନା sbi_scratch *scratch)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cidx;
	unsigned long mstatus_val = 0;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା mhpm_mask = sbi_hart_mhpm_mask(scratch);
	uint64_t mhpmevent_init_val = 0;
	uint64_t menvcfg_val, mstateen_val;

	/* Enable FPU */
	ଯଦି (misa_extension('D') || misa_extension('F'))
		mstatus_val |=  MSTATUS_FS;

	/* Enable Vector context */
	ଯଦି (misa_extension('V'))
		mstatus_val |=  MSTATUS_VS;

	csr_write(CSR_MSTATUS, mstatus_val);

	/* Disable user mode usage of all perf counters except TM */
	ଯଦି (misa_extension('S') &&
	    sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_SCOUNTEREN, 0x02);

	/**
	 * OpenSBI doesn't use any PMU counters in M-mode.
	 * Supervisor mode usage for all counters are enabled by default
	 * But counters will not run until mcountinhibit is set.
	 */
	ଯଦି (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	/* All programmable counters will start running at runtime after S-mode request */
	ଯଦି (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	/**
	 * The mhpmeventn[h] CSR should be initialized with interrupt disabled
	 * and inhibited running in M-mode during init.
	 */
	mhpmevent_init_val |= (MHPMEVENT_OF | MHPMEVENT_MINH);
	ପାଇଁ (cidx = 0; cidx <= 28; cidx++) {
		ଯଦି (!(mhpm_mask & 1UL << (cidx + 3)))
			ଜାରି;
#ଯଦି __riscv_xlen == 32
		csr_write_ଅଙ୍କ(CSR_MHPMEVENT3 + cidx,
			       mhpmevent_init_val & 0xFFFFFFFF);
		ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
			csr_write_ଅଙ୍କ(CSR_MHPMEVENT3H + cidx,
				      mhpmevent_init_val >> BITS_PER_LONG);
#ଅନ୍ୟଥା
		csr_write_ଅଙ୍କ(CSR_MHPMEVENT3 + cidx, mhpmevent_init_val);
#endif
	}

	ଯଦି (misa_extension('H'))
		csr_write(CSR_HSTATUS, 0);

	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMSTATEEN)) {
		mstateen_val = 0;
		mstateen_val |= SMSTATEEN_STATEN;
		mstateen_val |= SMSTATEEN0_CONTEXT;
		mstateen_val |= SMSTATEEN0_HSENVCFG;

		ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA))
			mstateen_val |= (SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);
		ଅନ୍ୟଥା
			mstateen_val &= ~(SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);

		ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA) ||
		    sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCSRIND))
			mstateen_val |= (SMSTATEEN0_SVSLCT);
		ଅନ୍ୟଥା
			mstateen_val &= ~(SMSTATEEN0_SVSLCT);

		ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCTR))
			mstateen_val |= SMSTATEEN0_CTR;
		ଅନ୍ୟଥା
			mstateen_val &= ~SMSTATEEN0_CTR;

		ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
			mstateen_val |= SMSTATEEN0_SRMCFG;
		ଅନ୍ୟଥା
			mstateen_val &= ~SMSTATEEN0_SRMCFG;

		csr_write64(CSR_MSTATEEN0, mstateen_val);
		csr_write64(CSR_MSTATEEN1, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN2, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN3, SMSTATEEN_STATEN);
	}

	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSSTATEEN)) {
		ଯଦି (misa_extension('S')) {
			csr_write(CSR_SSTATEEN0, 0);
			csr_write(CSR_SSTATEEN1, 0);
			csr_write(CSR_SSTATEEN2, 0);
			csr_write(CSR_SSTATEEN3, 0);
		}
		ଯଦି (misa_extension('H')) {
			csr_write64(CSR_HSTATEEN0, (uint64_t)0);
			csr_write64(CSR_HSTATEEN1, (uint64_t)0);
			csr_write64(CSR_HSTATEEN2, (uint64_t)0);
			csr_write64(CSR_HSTATEEN3, (uint64_t)0);
		}
	}

	ଯଦି (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12) {
		menvcfg_val = csr_read64(CSR_MENVCFG);

		/* Disable HW A/D updating by default */
		menvcfg_val &= ~ENVCFG_ADUE;

		/* Disable double trap by default */
		menvcfg_val &= ~ENVCFG_DTE;

#define __set_menvcfg_ext(__ext, __bits)				\
		ଯଦି (sbi_hart_has_extension(scratch, __ext))		\
			menvcfg_val |= __bits;

		/*
		 * Enable access to extensions if they are present in the
		 * hardware or in the device tree.
		 */

		__set_menvcfg_ext(SBI_HART_EXT_ZICBOZ, ENVCFG_CBZE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM, ENVCFG_CBCFE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM,
				  ENVCFG_CBIE_INV << ENVCFG_CBIE_SHIFT)
#ଯଦି __riscv_xlen > 32
		__set_menvcfg_ext(SBI_HART_EXT_SVPBMT, ENVCFG_PBMTE)
#endif
		__set_menvcfg_ext(SBI_HART_EXT_SSTC, ENVCFG_STCE)
		__set_menvcfg_ext(SBI_HART_EXT_SMCDELEG, ENVCFG_CDE);

		/*
		 * Assume only Svadu is supported when it is the only extension
		 * present in the ISA string. Svade is assumed when neither are
		 * present. When both are present we must default to Svade (see
		 * the zero reset value of FWFT.PTE_AD_HW_UPDATING).
		 */
		ଯଦି (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SVADE))
			__set_menvcfg_ext(SBI_HART_EXT_SVADU, ENVCFG_ADUE);

#undef __set_menvcfg_ext

		csr_write64(CSR_MENVCFG, menvcfg_val);

		/* Enable S-mode access to seed CSR */
		ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
			csr_set(CSR_MSECCFG, MSECCFG_SSEED);
			csr_clear(CSR_MSECCFG, MSECCFG_USEED);
		}
	}

	/* Disable all interrupts */
	csr_write(CSR_MIE, 0);

	/* Disable S-mode paging */
	ଯଦି (misa_extension('S'))
		csr_write(CSR_SATP, 0);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fp_init(ସଂରଚନା sbi_scratch *scratch)
{
#ifdef __riscv_flen
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
#endif

	ଯଦି (!misa_extension('D') && !misa_extension('F'))
		ଫେରନ୍ତୁ 0;

	ଯଦି (!(csr_read(CSR_MSTATUS) & MSTATUS_FS))
		ଫେରନ୍ତୁ SBI_EINVAL;

#ifdef __riscv_flen
	ପାଇଁ (i = 0; i < 32; i++)
		init_fp_reg(i);
	csr_write(CSR_FCSR, 0);
#endif

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ sbi_hart_delegation_dump(ସଂରଚନା sbi_scratch *scratch,
			      ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *suffix)
{
	ଯଦି (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist*/
		ଫେରନ୍ତୁ;

	sbi_printf("%sMIDELEG%s: 0x%" PRILX "\n",
		   prefix, suffix, csr_read(CSR_MIDELEG));
	sbi_printf("%sMEDELEG%s: 0x%" PRILX "\n",
		   prefix, suffix, csr_read(CSR_MEDELEG));
}

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_mhpm_mask(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	ଫେରନ୍ତୁ hfeatures->mhpm_mask;
}

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_mhpm_bits(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	ଫେରନ୍ତୁ hfeatures->mhpm_bits;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_priv_version(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	ଫେରନ୍ତୁ hfeatures->priv_version;
}

ଶୂନ୍ୟ sbi_hart_get_priv_version_str(ସଂରଚନା sbi_scratch *scratch,
				   ଅକ୍ଷର *version_str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nvstr)
{
	ଅକ୍ଷର *temp;
	ସଂରଚନା sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	ବାଛ (hfeatures->priv_version) {
	ସ୍ଥିତି SBI_HART_PRIV_VER_1_10:
		temp = "v1.10";
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_HART_PRIV_VER_1_11:
		temp = "v1.11";
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_HART_PRIV_VER_1_12:
		temp = "v1.12";
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		temp = "unknown";
		ଭାଙ୍ଗ;
	}

	sbi_snprintf(version_str, nvstr, "%s", temp);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ __sbi_hart_update_extension(
					ସଂରଚନା sbi_hart_features *hfeatures,
					ଗଣନା sbi_hart_extensions ext,
					bool enable)
{
	ଯଦି (enable)
		__set_bit(ext, hfeatures->extensions);
	ଅନ୍ୟଥା
		__clear_bit(ext, hfeatures->extensions);
}

/**
 * Enable/Disable a particular hart extension
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @param enable new state of hart extension
 */
ଶୂନ୍ୟ sbi_hart_update_extension(ସଂରଚନା sbi_scratch *scratch,
			       ଗଣନା sbi_hart_extensions ext,
			       bool enable)
{
	ସଂରଚନା sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	__sbi_hart_update_extension(hfeatures, ext, enable);
}

/**
 * Check whether a particular hart extension is available
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @returns true (available) or false (not available)
 */
bool sbi_hart_has_extension(ସଂରଚନା sbi_scratch *scratch,
			    ଗଣନା sbi_hart_extensions ext)
{
	ସଂରଚନା sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	ଯଦି (__test_bit(ext, hfeatures->extensions))
		ଫେରନ୍ତୁ true;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ false;
}

#define __SBI_HART_EXT_DATA(_name, _id) {	\
	.name = #_name,				\
	.id = _id,				\
}

ଅଚଳ ସଂରଚନା sbi_hart_ext_data sbi_hart_ext[] = {
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

_Static_assert(SBI_HART_EXT_MAX == array_size(sbi_hart_ext),
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
ଶୂନ୍ୟ sbi_hart_get_extensions_str(ସଂରଚନା sbi_scratch *scratch,
				 ଅକ୍ଷର *extensions_str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nestr)
{
	ସଂରଚନା sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = 0, ext = 0;

	ଯଦି (!extensions_str || nestr <= 0)
		ଫେରନ୍ତୁ;
	sbi_memset(extensions_str, 0, nestr);

	for_each_set_bit(ext, hfeatures->extensions, SBI_HART_EXT_MAX) {
		ଯଦି (offset + sbi_strlen(sbi_hart_ext[ext].name) + 1 > nestr) {
			sbi_printf("%s:extension name is longer than buffer (error)\n", __func__);
			ଭାଙ୍ଗ;
		}
		sbi_snprintf(extensions_str + offset,
				 nestr - offset,
				 "%s,", sbi_hart_ext[ext].name);
		offset = offset + sbi_strlen(sbi_hart_ext[ext].name) + 1;
	}

	ଯଦି (offset)
		extensions_str[offset - 1] = '\0';
	ଅନ୍ୟଥା
		sbi_strncpy(extensions_str, "none", nestr);
}

/**
 * Check whether a particular CSR is present on the HART
 *
 * @param scratch pointer to the HART scratch space
 * @param csr the CSR number to check
 */
bool sbi_hart_has_csr(ସଂରଚନା sbi_scratch *scratch, ଗଣନା sbi_hart_csrs csr)
{
	ସଂରଚନା sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	ଫେରନ୍ତୁ __test_bit(csr, hfeatures->csrs);
}

ସ୍ଥିର unsigned long hart_pmp_get_allowed_addr(ଶୂନ୍ୟ)
{
	unsigned long val = 0;
	ସଂରଚନା sbi_trap_info trap = {0};

	csr_write_allowed(CSR_PMPCFG0, &trap, 0);
	ଯଦି (trap.cause)
		ଫେରନ୍ତୁ 0;

	csr_write_allowed(CSR_PMPADDR0, &trap, PMP_ADDR_MASK);
	ଯଦି (!trap.cause) {
		val = csr_read_allowed(CSR_PMPADDR0, &trap);
		ଯଦି (trap.cause)
			val = 0;
	}

	ଫେରନ୍ତୁ val;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hart_mhpm_get_allowed_bits(ଶୂନ୍ୟ)
{
	unsigned long val = ~(0UL);
	ସଂରଚନା sbi_trap_info trap = {0};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_bits = 0;

	/**
	 * It is assumed that platforms will implement same number of bits for
	 * all the performance counters including mcycle/minstret.
	 */
	csr_write_allowed(CSR_MHPMCOUNTER3, &trap, val);
	ଯଦି (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3, &trap);
		ଯଦି (trap.cause)
			ଫେରନ୍ତୁ 0;
	}
	ଅଙ୍କ_bits = sbi_fls(val) + 1;
#ଯଦି __riscv_xlen == 32
	csr_write_allowed(CSR_MHPMCOUNTER3H, &trap, val);
	ଯଦି (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3H, &trap);
		ଯଦି (trap.cause)
			ଫେରନ୍ତୁ ଅଙ୍କ_bits;
	}
	ଅଙ୍କ_bits += sbi_fls(val) + 1;

#endif

	ଫେରନ୍ତୁ ଅଙ୍କ_bits;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hart_detect_features(ସଂରଚନା sbi_scratch *scratch, bool cold_boot)
{
	ସଂରଚନା sbi_trap_info trap = {0};
	ସଂରଚନା sbi_hart_features *hfeatures =
		sbi_scratch_offset_ptr(scratch, hart_features_offset);
	unsigned long val, oldval;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	/* If hart features already detected then do nothing */
	ଯଦି (hfeatures->detected)
		ଫେରନ୍ତୁ 0;

	/*
	 * Parse device tree extensions early, before any trap-based checks.
	 * Needed to detect Smrnmi and install NMI handlers before CSR probes
	 * that may trigger traps.
	 */
	rc = sbi_platform_extensions_init(sbi_platform_ptr(scratch), cold_boot);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMRNMI)) {
		ଅଚଳ ସଂରଚନା sbi_platform *plat = sbi_platform_thishart_ptr();
		ଅଚଳ ସଂରଚନା sbi_platform_operations *ops = sbi_platform_ops(plat);
		ବାହ୍ୟ ଶୂନ୍ୟ _trap_rnmi_handler(ଶୂନ୍ୟ);
		ବାହ୍ୟ ଶୂନ୍ୟ _trap_handler(ଶୂନ୍ୟ);

		ଯଦି (!ops || !ops->smrnmi_handlers_init)
			sbi_panic("Smrnmi detected, but platform lacks smrnmi_handlers_init callback\n");

		/* Reuse _trap_handler for the RNME slot since RNME is taken
		 * as a regular M-mode trap with NMIE=0. */
		ops->smrnmi_handlers_init(_trap_rnmi_handler, _trap_handler);

		/* Initialize MNSCRATCH for the RNMI handler */
		csr_write(CSR_MNSCRATCH, scratch);

		/* Enable NMIs */
		csr_set(CSR_MNSTATUS, MNSTATUS_NMIE);
	}

#define __check_hpm_csr(__csr, __mask) 					  \
	oldval = csr_read_allowed(__csr, &trap);			  \
	ଯଦି (!trap.cause) {						  \
		csr_write_allowed(__csr, &trap, 1UL);			  \
		ଯଦି (!trap.cause && csr_swap(__csr, oldval) == 1UL) {	  \
			(hfeatures->__mask) |= 1 << (__csr - CSR_MCYCLE); \
		}							  \
	}

#define __check_hpm_csr_2(__csr, __mask)	 		  \
	__check_hpm_csr(__csr + 0, __mask)	 		  \
	__check_hpm_csr(__csr + 1, __mask)
#define __check_hpm_csr_4(__csr, __mask)	 		  \
	__check_hpm_csr_2(__csr + 0, __mask) 			  \
	__check_hpm_csr_2(__csr + 2, __mask)
#define __check_hpm_csr_8(__csr, __mask)	 		  \
	__check_hpm_csr_4(__csr + 0, __mask) 			  \
	__check_hpm_csr_4(__csr + 4, __mask)
#define __check_hpm_csr_16(__csr, __mask)	 		  \
	__check_hpm_csr_8(__csr + 0, __mask) 			  \
	__check_hpm_csr_8(__csr + 8, __mask)

#define __check_csr(__csr, __rdonly, __wrval, __field, __skip)		\
	oldval = csr_read_allowed(__csr, &trap);			\
	ଯଦି (!trap.cause) {						\
		ଯଦି (__rdonly) {						\
			(hfeatures->__field)++;				\
		} ଅନ୍ୟଥା {						\
			csr_write_allowed(__csr, &trap, __wrval);	\
			ଯଦି (!trap.cause) {				\
				ଯଦି ((csr_swap(__csr, oldval) & __wrval) == __wrval)	\
					(hfeatures->__field)++;		\
				ଅନ୍ୟଥା					\
					ଯାଅ __skip;			\
			} ଅନ୍ୟଥା {					\
				ଯାଅ __skip;				\
			}						\
		}							\
	} ଅନ୍ୟଥା {							\
		ଯାଅ __skip;						\
	}
#define __check_csr_2(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 1, __rdonly, __wrval, __field, __skip)
#define __check_csr_4(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 2, __rdonly, __wrval, __field, __skip)
#define __check_csr_8(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 4, __rdonly, __wrval, __field, __skip)
#define __check_csr_16(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 8, __rdonly, __wrval, __field, __skip)
#define __check_csr_32(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 16, __rdonly, __wrval, __field, __skip)
#define __check_csr_64(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 32, __rdonly, __wrval, __field, __skip)

	/**
	 * Detect the allowed address bits & granularity. At least PMPADDR0
	 * should be implemented.
	 */
	val = hart_pmp_get_allowed_addr();
	ଯଦି (val) {
		hfeatures->pmp_log2gran = sbi_ffs(val) + 2;
		hfeatures->pmp_addr_bits = sbi_fls(val) + 1;
		/* Detect number of PMP regions. At least PMPADDR0 should be implemented*/
		__check_csr_64(CSR_PMPADDR0, 0, val, pmp_ଗଣନା, __pmp_skip);
	}
__pmp_skip:
	/* Detect number of MHPM counters */
	__check_hpm_csr(CSR_MHPMCOUNTER3, mhpm_mask);
	hfeatures->mhpm_bits = hart_mhpm_get_allowed_bits();
	__check_hpm_csr_4(CSR_MHPMCOUNTER4, mhpm_mask);
	__check_hpm_csr_8(CSR_MHPMCOUNTER8, mhpm_mask);
	__check_hpm_csr_16(CSR_MHPMCOUNTER16, mhpm_mask);

	/**
	 * No need to check for MHPMCOUNTERH for RV32 as they are expected to be
	 * implemented if MHPMCOUNTER is implemented.
	 */
#undef __check_csr_64
#undef __check_csr_32
#undef __check_csr_16
#undef __check_csr_8
#undef __check_csr_4
#undef __check_csr_2
#undef __check_csr


#define __check_priv(__csr, __base_priv, __priv)			\
	val = csr_read_allowed(__csr, &trap);				\
	ଯଦି (!trap.cause && (hfeatures->priv_version >= __base_priv)) {	\
		hfeatures->priv_version = __priv;			\
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

#undef __check_priv_csr

#define __check_ext_csr(__base_priv, __csr, __ext)			\
	ଯଦି (hfeatures->priv_version >= __base_priv) {			\
		csr_read_allowed(__csr, &trap);				\
		ଯଦି (!trap.cause)					\
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

#undef __check_ext_csr

#define __check_csr_existence(__csr, __csr_id)				\
	csr_read_allowed(__csr, &trap);					\
	ଯଦି (!trap.cause)						\
		__set_bit(__csr_id, hfeatures->csrs);

	__check_csr_existence(CSR_CYCLE, SBI_HART_CSR_CYCLE);
	__check_csr_existence(CSR_TIME, SBI_HART_CSR_TIME);
	__check_csr_existence(CSR_INSTRET, SBI_HART_CSR_INSTRET);

#undef __check_csr_existence

	/* Zicntr should only be detected using traps */
	__sbi_hart_update_extension(hfeatures, SBI_HART_EXT_ZICNTR,
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME)  &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET));

	/* Extensions implied by other extensions and features */
	ଯଦି (hfeatures->mhpm_mask)
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
	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP) &&
	    (csr_read(CSR_MSECCFG) & MSECCFG_MML))
		ଫେରନ୍ତୁ SBI_EILL;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_reinit(ସଂରଚନା sbi_scratch *scratch)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	mstatus_init(scratch);

	rc = fp_init(scratch);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_init(ସଂରଚନା sbi_scratch *scratch, bool cold_boot)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	/*
	 * Clear mip CSR before proceeding with init to avoid any spurious
	 * external interrupts in S-mode.
	 */
	csr_write(CSR_MIP, 0);

	ଯଦି (cold_boot) {
		ଯଦି (misa_extension('H'))
			sbi_hart_expected_trap = &__sbi_expected_trap_hext;

		hart_features_offset = sbi_scratch_alloc_offset(
					ଆକାର(ସଂରଚନା sbi_hart_features));
		ଯଦି (!hart_features_offset)
			ଫେରନ୍ତୁ SBI_ENOMEM;
	}

	rc = hart_detect_features(scratch, cold_boot);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଯଦି (cold_boot) {
		rc = sbi_hart_pmp_init(scratch);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ sbi_hart_reinit(scratch);
}

ଶୂନ୍ୟ __attribute__((noreturn)) sbi_hart_hang(ଶୂନ୍ୟ)
{
	ଯେତେବେଳେ (1)
		wfi();
	__builtin_unreachable();
}

ଶୂନ୍ୟ __attribute__((noreturn))
sbi_hart_switch_mode(unsigned long arg0, unsigned long arg1,
		     unsigned long next_addr, unsigned long next_mode,
		     bool next_virt)
{
#ଯଦି __riscv_xlen == 32
	unsigned long val, valH;
#ଅନ୍ୟଥା
	unsigned long val;
#endif

	ବାଛ (next_mode) {
	ସ୍ଥିତି PRV_M:
		ଭାଙ୍ଗ;
	ସ୍ଥିତି PRV_S:
		ଯଦି (!misa_extension('S'))
			sbi_hart_hang();
		ଭାଙ୍ଗ;
	ସ୍ଥିତି PRV_U:
		ଯଦି (!misa_extension('U'))
			sbi_hart_hang();
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		sbi_hart_hang();
	}

	val = csr_read(CSR_MSTATUS);
	val = ଭର୍ତ୍ତି_FIELD(val, MSTATUS_MPP, next_mode);
	val = ଭର୍ତ୍ତି_FIELD(val, MSTATUS_MPIE, 0);
#ଯଦି __riscv_xlen == 32
	ଯଦି (misa_extension('H')) {
		valH = csr_read(CSR_MSTATUSH);
		valH = ଭର୍ତ୍ତି_FIELD(valH, MSTATUSH_MPV, next_virt);
		csr_write(CSR_MSTATUSH, valH);
	}
#ଅନ୍ୟଥା
	ଯଦି (misa_extension('H'))
		val = ଭର୍ତ୍ତି_FIELD(val, MSTATUS_MPV, next_virt);
#endif
	csr_write(CSR_MSTATUS, val);
	csr_write(CSR_MEPC, next_addr);

	ଯଦି (next_mode == PRV_S) {
		ଯଦି (next_virt) {
			csr_write(CSR_VSTVEC, next_addr);
			csr_write(CSR_VSSCRATCH, 0);
			csr_write(CSR_VSIE, 0);
			csr_write(CSR_VSATP, 0);
		} ଅନ୍ୟଥା {
			csr_write(CSR_STVEC, next_addr);
			csr_write(CSR_SSCRATCH, 0);
			csr_write(CSR_SIE, 0);
			csr_write(CSR_SATP, 0);
		}
	} ଅନ୍ୟଥା ଯଦି (next_mode == PRV_U) {
		ଯଦି (misa_extension('N')) {
			csr_write(CSR_UTVEC, next_addr);
			csr_write(CSR_USCRATCH, 0);
			csr_write(CSR_UIE, 0);
		}
	}

	ପଞ୍ଜିକରଣ unsigned long a0 asm("a0") = arg0;
	ପଞ୍ଜିକରଣ unsigned long a1 asm("a1") = arg1;
	__asm__ __volatile__("mret" : : "r"(a0), "r"(a1));
	__builtin_unreachable();
}
