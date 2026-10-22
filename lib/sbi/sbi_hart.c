/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_barrier.h>
#அந்தர்பாவ <sbi/riscv_encoding.h>
#அந்தர்பாவ <sbi/riscv_fp.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_csr_detect.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_hart_pmp.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_pmu.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi/sbi_trap.h>

வெளி வெறுமை __sbi_expected_trap(வெறுமை);
வெளி வெறுமை __sbi_expected_trap_hext(வெறுமை);

வெறுமை (*sbi_hart_expected_trap)(வெறுமை) = &__sbi_expected_trap;

unsigned long hart_features_offset;

நிலையான வெறுமை mstatus_init(கட்டமைப்பு sbi_scratch *scratch)
{
	முழு cidx;
	unsigned long mstatus_val = 0;
	unsigned முழு mhpm_mask = sbi_hart_mhpm_mask(scratch);
	uint64_t mhpmevent_init_val = 0;
	uint64_t menvcfg_val, mstateen_val;

	/* Enable FPU */
	எனில் (misa_extension('D') || misa_extension('F'))
		mstatus_val |=  MSTATUS_FS;

	/* Enable Vector context */
	எனில் (misa_extension('V'))
		mstatus_val |=  MSTATUS_VS;

	csr_write(CSR_MSTATUS, mstatus_val);

	/* Disable user mode usage of all perf counters except TM */
	எனில் (misa_extension('S') &&
	    sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_SCOUNTEREN, 0x02);

	/**
	 * OpenSBI doesn't use any PMU counters in M-mode.
	 * Supervisor mode usage for all counters are enabled by default
	 * But counters will not run until mcountinhibit is set.
	 */
	எனில் (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	/* All programmable counters will start running at runtime after S-mode request */
	எனில் (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	/**
	 * The mhpmeventn[h] CSR should be initialized with interrupt disabled
	 * and inhibited running in M-mode during init.
	 */
	mhpmevent_init_val |= (MHPMEVENT_OF | MHPMEVENT_MINH);
	க்கு (cidx = 0; cidx <= 28; cidx++) {
		எனில் (!(mhpm_mask & 1UL << (cidx + 3)))
			தொடர்;
#எனில் __riscv_xlen == 32
		csr_write_இலக்கம்(CSR_MHPMEVENT3 + cidx,
			       mhpmevent_init_val & 0xFFFFFFFF);
		எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
			csr_write_இலக்கம்(CSR_MHPMEVENT3H + cidx,
				      mhpmevent_init_val >> BITS_PER_LONG);
#இல்லையெனில்
		csr_write_இலக்கம்(CSR_MHPMEVENT3 + cidx, mhpmevent_init_val);
#endif
	}

	எனில் (misa_extension('H'))
		csr_write(CSR_HSTATUS, 0);

	எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMSTATEEN)) {
		mstateen_val = 0;
		mstateen_val |= SMSTATEEN_STATEN;
		mstateen_val |= SMSTATEEN0_CONTEXT;
		mstateen_val |= SMSTATEEN0_HSENVCFG;

		எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA))
			mstateen_val |= (SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);
		இல்லையெனில்
			mstateen_val &= ~(SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);

		எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA) ||
		    sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCSRIND))
			mstateen_val |= (SMSTATEEN0_SVSLCT);
		இல்லையெனில்
			mstateen_val &= ~(SMSTATEEN0_SVSLCT);

		எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCTR))
			mstateen_val |= SMSTATEEN0_CTR;
		இல்லையெனில்
			mstateen_val &= ~SMSTATEEN0_CTR;

		எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
			mstateen_val |= SMSTATEEN0_SRMCFG;
		இல்லையெனில்
			mstateen_val &= ~SMSTATEEN0_SRMCFG;

		csr_write64(CSR_MSTATEEN0, mstateen_val);
		csr_write64(CSR_MSTATEEN1, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN2, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN3, SMSTATEEN_STATEN);
	}

	எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSSTATEEN)) {
		எனில் (misa_extension('S')) {
			csr_write(CSR_SSTATEEN0, 0);
			csr_write(CSR_SSTATEEN1, 0);
			csr_write(CSR_SSTATEEN2, 0);
			csr_write(CSR_SSTATEEN3, 0);
		}
		எனில் (misa_extension('H')) {
			csr_write64(CSR_HSTATEEN0, (uint64_t)0);
			csr_write64(CSR_HSTATEEN1, (uint64_t)0);
			csr_write64(CSR_HSTATEEN2, (uint64_t)0);
			csr_write64(CSR_HSTATEEN3, (uint64_t)0);
		}
	}

	எனில் (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12) {
		menvcfg_val = csr_read64(CSR_MENVCFG);

		/* Disable HW A/D updating by default */
		menvcfg_val &= ~ENVCFG_ADUE;

		/* Disable double trap by default */
		menvcfg_val &= ~ENVCFG_DTE;

#define __set_menvcfg_ext(__ext, __bits)				\
		எனில் (sbi_hart_has_extension(scratch, __ext))		\
			menvcfg_val |= __bits;

		/*
		 * Enable access to extensions if they are present in the
		 * hardware or in the device tree.
		 */

		__set_menvcfg_ext(SBI_HART_EXT_ZICBOZ, ENVCFG_CBZE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM, ENVCFG_CBCFE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM,
				  ENVCFG_CBIE_INV << ENVCFG_CBIE_SHIFT)
#எனில் __riscv_xlen > 32
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
		எனில் (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SVADE))
			__set_menvcfg_ext(SBI_HART_EXT_SVADU, ENVCFG_ADUE);

#undef __set_menvcfg_ext

		csr_write64(CSR_MENVCFG, menvcfg_val);

		/* Enable S-mode access to seed CSR */
		எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
			csr_set(CSR_MSECCFG, MSECCFG_SSEED);
			csr_clear(CSR_MSECCFG, MSECCFG_USEED);
		}
	}

	/* Disable all interrupts */
	csr_write(CSR_MIE, 0);

	/* Disable S-mode paging */
	எனில் (misa_extension('S'))
		csr_write(CSR_SATP, 0);
}

நிலையான முழு fp_init(கட்டமைப்பு sbi_scratch *scratch)
{
#ifdef __riscv_flen
	முழு i;
#endif

	எனில் (!misa_extension('D') && !misa_extension('F'))
		திரும்பு 0;

	எனில் (!(csr_read(CSR_MSTATUS) & MSTATUS_FS))
		திரும்பு SBI_EINVAL;

#ifdef __riscv_flen
	க்கு (i = 0; i < 32; i++)
		init_fp_reg(i);
	csr_write(CSR_FCSR, 0);
#endif

	திரும்பு 0;
}

வெறுமை sbi_hart_delegation_dump(கட்டமைப்பு sbi_scratch *scratch,
			      மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *suffix)
{
	எனில் (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist*/
		திரும்பு;

	sbi_printf("%sMIDELEG%s: 0x%" PRILX "\n",
		   prefix, suffix, csr_read(CSR_MIDELEG));
	sbi_printf("%sMEDELEG%s: 0x%" PRILX "\n",
		   prefix, suffix, csr_read(CSR_MEDELEG));
}

unsigned முழு sbi_hart_mhpm_mask(கட்டமைப்பு sbi_scratch *scratch)
{
	கட்டமைப்பு sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	திரும்பு hfeatures->mhpm_mask;
}

unsigned முழு sbi_hart_mhpm_bits(கட்டமைப்பு sbi_scratch *scratch)
{
	கட்டமைப்பு sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	திரும்பு hfeatures->mhpm_bits;
}

முழு sbi_hart_priv_version(கட்டமைப்பு sbi_scratch *scratch)
{
	கட்டமைப்பு sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	திரும்பு hfeatures->priv_version;
}

வெறுமை sbi_hart_get_priv_version_str(கட்டமைப்பு sbi_scratch *scratch,
				   எழுத்து *version_str, முழு nvstr)
{
	எழுத்து *temp;
	கட்டமைப்பு sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	தேர்வு (hfeatures->priv_version) {
	நிலை SBI_HART_PRIV_VER_1_10:
		temp = "v1.10";
		நிறுத்து;
	நிலை SBI_HART_PRIV_VER_1_11:
		temp = "v1.11";
		நிறுத்து;
	நிலை SBI_HART_PRIV_VER_1_12:
		temp = "v1.12";
		நிறுத்து;
	இயல்பு:
		temp = "unknown";
		நிறுத்து;
	}

	sbi_snprintf(version_str, nvstr, "%s", temp);
}

நிலையான உள்வரி வெறுமை __sbi_hart_update_extension(
					கட்டமைப்பு sbi_hart_features *hfeatures,
					எண்ணிக்கை sbi_hart_extensions ext,
					bool enable)
{
	எனில் (enable)
		__set_bit(ext, hfeatures->extensions);
	இல்லையெனில்
		__clear_bit(ext, hfeatures->extensions);
}

/**
 * Enable/Disable a particular hart extension
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @param enable new state of hart extension
 */
வெறுமை sbi_hart_update_extension(கட்டமைப்பு sbi_scratch *scratch,
			       எண்ணிக்கை sbi_hart_extensions ext,
			       bool enable)
{
	கட்டமைப்பு sbi_hart_features *hfeatures =
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
bool sbi_hart_has_extension(கட்டமைப்பு sbi_scratch *scratch,
			    எண்ணிக்கை sbi_hart_extensions ext)
{
	கட்டமைப்பு sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	எனில் (__test_bit(ext, hfeatures->extensions))
		திரும்பு true;
	இல்லையெனில்
		திரும்பு false;
}

#define __SBI_HART_EXT_DATA(_name, _id) {	\
	.name = #_name,				\
	.id = _id,				\
}

மாறிலி கட்டமைப்பு sbi_hart_ext_data sbi_hart_ext[] = {
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
வெறுமை sbi_hart_get_extensions_str(கட்டமைப்பு sbi_scratch *scratch,
				 எழுத்து *extensions_str, முழு nestr)
{
	கட்டமைப்பு sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);
	முழு offset = 0, ext = 0;

	எனில் (!extensions_str || nestr <= 0)
		திரும்பு;
	sbi_memset(extensions_str, 0, nestr);

	for_each_set_bit(ext, hfeatures->extensions, SBI_HART_EXT_MAX) {
		எனில் (offset + sbi_strlen(sbi_hart_ext[ext].name) + 1 > nestr) {
			sbi_printf("%s:extension name is longer than buffer (error)\n", __func__);
			நிறுத்து;
		}
		sbi_snprintf(extensions_str + offset,
				 nestr - offset,
				 "%s,", sbi_hart_ext[ext].name);
		offset = offset + sbi_strlen(sbi_hart_ext[ext].name) + 1;
	}

	எனில் (offset)
		extensions_str[offset - 1] = '\0';
	இல்லையெனில்
		sbi_strncpy(extensions_str, "none", nestr);
}

/**
 * Check whether a particular CSR is present on the HART
 *
 * @param scratch pointer to the HART scratch space
 * @param csr the CSR number to check
 */
bool sbi_hart_has_csr(கட்டமைப்பு sbi_scratch *scratch, எண்ணிக்கை sbi_hart_csrs csr)
{
	கட்டமைப்பு sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	திரும்பு __test_bit(csr, hfeatures->csrs);
}

நிலையான unsigned long hart_pmp_get_allowed_addr(வெறுமை)
{
	unsigned long val = 0;
	கட்டமைப்பு sbi_trap_info trap = {0};

	csr_write_allowed(CSR_PMPCFG0, &trap, 0);
	எனில் (trap.cause)
		திரும்பு 0;

	csr_write_allowed(CSR_PMPADDR0, &trap, PMP_ADDR_MASK);
	எனில் (!trap.cause) {
		val = csr_read_allowed(CSR_PMPADDR0, &trap);
		எனில் (trap.cause)
			val = 0;
	}

	திரும்பு val;
}

நிலையான முழு hart_mhpm_get_allowed_bits(வெறுமை)
{
	unsigned long val = ~(0UL);
	கட்டமைப்பு sbi_trap_info trap = {0};
	முழு இலக்கம்_bits = 0;

	/**
	 * It is assumed that platforms will implement same number of bits for
	 * all the performance counters including mcycle/minstret.
	 */
	csr_write_allowed(CSR_MHPMCOUNTER3, &trap, val);
	எனில் (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3, &trap);
		எனில் (trap.cause)
			திரும்பு 0;
	}
	இலக்கம்_bits = sbi_fls(val) + 1;
#எனில் __riscv_xlen == 32
	csr_write_allowed(CSR_MHPMCOUNTER3H, &trap, val);
	எனில் (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3H, &trap);
		எனில் (trap.cause)
			திரும்பு இலக்கம்_bits;
	}
	இலக்கம்_bits += sbi_fls(val) + 1;

#endif

	திரும்பு இலக்கம்_bits;
}

நிலையான முழு hart_detect_features(கட்டமைப்பு sbi_scratch *scratch, bool cold_boot)
{
	கட்டமைப்பு sbi_trap_info trap = {0};
	கட்டமைப்பு sbi_hart_features *hfeatures =
		sbi_scratch_offset_ptr(scratch, hart_features_offset);
	unsigned long val, oldval;
	முழு rc;

	/* If hart features already detected then do nothing */
	எனில் (hfeatures->detected)
		திரும்பு 0;

	/*
	 * Parse device tree extensions early, before any trap-based checks.
	 * Needed to detect Smrnmi and install NMI handlers before CSR probes
	 * that may trigger traps.
	 */
	rc = sbi_platform_extensions_init(sbi_platform_ptr(scratch), cold_boot);
	எனில் (rc)
		திரும்பு rc;

	எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMRNMI)) {
		மாறிலி கட்டமைப்பு sbi_platform *plat = sbi_platform_thishart_ptr();
		மாறிலி கட்டமைப்பு sbi_platform_operations *ops = sbi_platform_ops(plat);
		வெளி வெறுமை _trap_rnmi_handler(வெறுமை);
		வெளி வெறுமை _trap_handler(வெறுமை);

		எனில் (!ops || !ops->smrnmi_handlers_init)
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
	எனில் (!trap.cause) {						  \
		csr_write_allowed(__csr, &trap, 1UL);			  \
		எனில் (!trap.cause && csr_swap(__csr, oldval) == 1UL) {	  \
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
	எனில் (!trap.cause) {						\
		எனில் (__rdonly) {						\
			(hfeatures->__field)++;				\
		} இல்லையெனில் {						\
			csr_write_allowed(__csr, &trap, __wrval);	\
			எனில் (!trap.cause) {				\
				எனில் ((csr_swap(__csr, oldval) & __wrval) == __wrval)	\
					(hfeatures->__field)++;		\
				இல்லையெனில்					\
					செல் __skip;			\
			} இல்லையெனில் {					\
				செல் __skip;				\
			}						\
		}							\
	} இல்லையெனில் {							\
		செல் __skip;						\
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
	எனில் (val) {
		hfeatures->pmp_log2gran = sbi_ffs(val) + 2;
		hfeatures->pmp_addr_bits = sbi_fls(val) + 1;
		/* Detect number of PMP regions. At least PMPADDR0 should be implemented*/
		__check_csr_64(CSR_PMPADDR0, 0, val, pmp_மொத்தம், __pmp_skip);
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
	எனில் (!trap.cause && (hfeatures->priv_version >= __base_priv)) {	\
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
	எனில் (hfeatures->priv_version >= __base_priv) {			\
		csr_read_allowed(__csr, &trap);				\
		எனில் (!trap.cause)					\
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
	எனில் (!trap.cause)						\
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
	எனில் (hfeatures->mhpm_mask)
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
	எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP) &&
	    (csr_read(CSR_MSECCFG) & MSECCFG_MML))
		திரும்பு SBI_EILL;

	திரும்பு 0;
}

முழு sbi_hart_reinit(கட்டமைப்பு sbi_scratch *scratch)
{
	முழு rc;

	mstatus_init(scratch);

	rc = fp_init(scratch);
	எனில் (rc)
		திரும்பு rc;

	திரும்பு 0;
}

முழு sbi_hart_init(கட்டமைப்பு sbi_scratch *scratch, bool cold_boot)
{
	முழு rc;

	/*
	 * Clear mip CSR before proceeding with init to avoid any spurious
	 * external interrupts in S-mode.
	 */
	csr_write(CSR_MIP, 0);

	எனில் (cold_boot) {
		எனில் (misa_extension('H'))
			sbi_hart_expected_trap = &__sbi_expected_trap_hext;

		hart_features_offset = sbi_scratch_alloc_offset(
					அளவு(கட்டமைப்பு sbi_hart_features));
		எனில் (!hart_features_offset)
			திரும்பு SBI_ENOMEM;
	}

	rc = hart_detect_features(scratch, cold_boot);
	எனில் (rc)
		திரும்பு rc;

	எனில் (cold_boot) {
		rc = sbi_hart_pmp_init(scratch);
		எனில் (rc)
			திரும்பு rc;
	}

	திரும்பு sbi_hart_reinit(scratch);
}

வெறுமை __attribute__((noreturn)) sbi_hart_hang(வெறுமை)
{
	போது (1)
		wfi();
	__builtin_unreachable();
}

வெறுமை __attribute__((noreturn))
sbi_hart_switch_mode(unsigned long arg0, unsigned long arg1,
		     unsigned long next_addr, unsigned long next_mode,
		     bool next_virt)
{
#எனில் __riscv_xlen == 32
	unsigned long val, valH;
#இல்லையெனில்
	unsigned long val;
#endif

	தேர்வு (next_mode) {
	நிலை PRV_M:
		நிறுத்து;
	நிலை PRV_S:
		எனில் (!misa_extension('S'))
			sbi_hart_hang();
		நிறுத்து;
	நிலை PRV_U:
		எனில் (!misa_extension('U'))
			sbi_hart_hang();
		நிறுத்து;
	இயல்பு:
		sbi_hart_hang();
	}

	val = csr_read(CSR_MSTATUS);
	val = செருகு_FIELD(val, MSTATUS_MPP, next_mode);
	val = செருகு_FIELD(val, MSTATUS_MPIE, 0);
#எனில் __riscv_xlen == 32
	எனில் (misa_extension('H')) {
		valH = csr_read(CSR_MSTATUSH);
		valH = செருகு_FIELD(valH, MSTATUSH_MPV, next_virt);
		csr_write(CSR_MSTATUSH, valH);
	}
#இல்லையெனில்
	எனில் (misa_extension('H'))
		val = செருகு_FIELD(val, MSTATUS_MPV, next_virt);
#endif
	csr_write(CSR_MSTATUS, val);
	csr_write(CSR_MEPC, next_addr);

	எனில் (next_mode == PRV_S) {
		எனில் (next_virt) {
			csr_write(CSR_VSTVEC, next_addr);
			csr_write(CSR_VSSCRATCH, 0);
			csr_write(CSR_VSIE, 0);
			csr_write(CSR_VSATP, 0);
		} இல்லையெனில் {
			csr_write(CSR_STVEC, next_addr);
			csr_write(CSR_SSCRATCH, 0);
			csr_write(CSR_SIE, 0);
			csr_write(CSR_SATP, 0);
		}
	} இல்லையெனில் எனில் (next_mode == PRV_U) {
		எனில் (misa_extension('N')) {
			csr_write(CSR_UTVEC, next_addr);
			csr_write(CSR_USCRATCH, 0);
			csr_write(CSR_UIE, 0);
		}
	}

	பதிவு unsigned long a0 asm("a0") = arg0;
	பதிவு unsigned long a1 asm("a1") = arg1;
	__asm__ __volatile__("mret" : : "r"(a0), "r"(a1));
	__builtin_unreachable();
}
