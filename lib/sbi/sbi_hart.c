/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_barrier.h>
#समावेश <sbi/riscv_encoding.h>
#समावेश <sbi/riscv_fp.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_csr_detect.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_hart_pmp.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_pmu.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_trap.h>

बाह्य व्योम __sbi_expected_trap(व्योम);
बाह्य व्योम __sbi_expected_trap_hext(व्योम);

व्योम (*sbi_hart_expected_trap)(व्योम) = &__sbi_expected_trap;

अचिन्हित दीर्घ hart_features_offset;

अटल व्योम mstatus_init(काष्ठा sbi_scratch *scratch)
{
	पूर्णांक cidx;
	अचिन्हित दीर्घ mstatus_val = 0;
	अचिन्हित पूर्णांक mhpm_mask = sbi_hart_mhpm_mask(scratch);
	uint64_t mhpmevent_init_val = 0;
	uint64_t menvcfg_val, mstateen_val;

	/* Enable FPU */
	अगर (misa_extension('D') || misa_extension('F'))
		mstatus_val |=  MSTATUS_FS;

	/* Enable Vector context */
	अगर (misa_extension('V'))
		mstatus_val |=  MSTATUS_VS;

	csr_write(CSR_MSTATUS, mstatus_val);

	/* Disable user mode usage of all perf counters except TM */
	अगर (misa_extension('S') &&
	    sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_SCOUNTEREN, 0x02);

	/**
	 * OpenSBI doesn't use any PMU counters in M-mode.
	 * Supervisor mode usage for all counters are enabled by default
	 * But counters will not run until mcountinhibit is set.
	 */
	अगर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	/* All programmable counters will start running at runtime after S-mode request */
	अगर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	/**
	 * The mhpmeventn[h] CSR should be initialized with interrupt disabled
	 * and inhibited running in M-mode during init.
	 */
	mhpmevent_init_val |= (MHPMEVENT_OF | MHPMEVENT_MINH);
	क्रम (cidx = 0; cidx <= 28; cidx++) {
		अगर (!(mhpm_mask & 1UL << (cidx + 3)))
			जारी;
#अगर __riscv_xlen == 32
		csr_write_अंक(CSR_MHPMEVENT3 + cidx,
			       mhpmevent_init_val & 0xFFFFFFFF);
		अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
			csr_write_अंक(CSR_MHPMEVENT3H + cidx,
				      mhpmevent_init_val >> BITS_PER_LONG);
#अन्यथा
		csr_write_अंक(CSR_MHPMEVENT3 + cidx, mhpmevent_init_val);
#पूर्ण_अगर
	}

	अगर (misa_extension('H'))
		csr_write(CSR_HSTATUS, 0);

	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMSTATEEN)) {
		mstateen_val = 0;
		mstateen_val |= SMSTATEEN_STATEN;
		mstateen_val |= SMSTATEEN0_CONTEXT;
		mstateen_val |= SMSTATEEN0_HSENVCFG;

		अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA))
			mstateen_val |= (SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);
		अन्यथा
			mstateen_val &= ~(SMSTATEEN0_AIA | SMSTATEEN0_IMSIC);

		अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA) ||
		    sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCSRIND))
			mstateen_val |= (SMSTATEEN0_SVSLCT);
		अन्यथा
			mstateen_val &= ~(SMSTATEEN0_SVSLCT);

		अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCTR))
			mstateen_val |= SMSTATEEN0_CTR;
		अन्यथा
			mstateen_val &= ~SMSTATEEN0_CTR;

		अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
			mstateen_val |= SMSTATEEN0_SRMCFG;
		अन्यथा
			mstateen_val &= ~SMSTATEEN0_SRMCFG;

		csr_write64(CSR_MSTATEEN0, mstateen_val);
		csr_write64(CSR_MSTATEEN1, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN2, SMSTATEEN_STATEN);
		csr_write64(CSR_MSTATEEN3, SMSTATEEN_STATEN);
	}

	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSSTATEEN)) {
		अगर (misa_extension('S')) {
			csr_write(CSR_SSTATEEN0, 0);
			csr_write(CSR_SSTATEEN1, 0);
			csr_write(CSR_SSTATEEN2, 0);
			csr_write(CSR_SSTATEEN3, 0);
		}
		अगर (misa_extension('H')) {
			csr_write64(CSR_HSTATEEN0, (uint64_t)0);
			csr_write64(CSR_HSTATEEN1, (uint64_t)0);
			csr_write64(CSR_HSTATEEN2, (uint64_t)0);
			csr_write64(CSR_HSTATEEN3, (uint64_t)0);
		}
	}

	अगर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12) {
		menvcfg_val = csr_read64(CSR_MENVCFG);

		/* Disable HW A/D updating by default */
		menvcfg_val &= ~ENVCFG_ADUE;

		/* Disable double trap by default */
		menvcfg_val &= ~ENVCFG_DTE;

#घोषणा __set_menvcfg_ext(__ext, __bits)				\
		अगर (sbi_hart_has_extension(scratch, __ext))		\
			menvcfg_val |= __bits;

		/*
		 * Enable access to extensions if they are present in the
		 * hardware or in the device tree.
		 */

		__set_menvcfg_ext(SBI_HART_EXT_ZICBOZ, ENVCFG_CBZE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM, ENVCFG_CBCFE)
		__set_menvcfg_ext(SBI_HART_EXT_ZICBOM,
				  ENVCFG_CBIE_INV << ENVCFG_CBIE_SHIFT)
#अगर __riscv_xlen > 32
		__set_menvcfg_ext(SBI_HART_EXT_SVPBMT, ENVCFG_PBMTE)
#पूर्ण_अगर
		__set_menvcfg_ext(SBI_HART_EXT_SSTC, ENVCFG_STCE)
		__set_menvcfg_ext(SBI_HART_EXT_SMCDELEG, ENVCFG_CDE);

		/*
		 * Assume only Svadu is supported when it is the only extension
		 * present in the ISA string. Svade is assumed when neither are
		 * present. When both are present we must default to Svade (see
		 * the zero reset value of FWFT.PTE_AD_HW_UPDATING).
		 */
		अगर (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SVADE))
			__set_menvcfg_ext(SBI_HART_EXT_SVADU, ENVCFG_ADUE);

#अघोषित __set_menvcfg_ext

		csr_write64(CSR_MENVCFG, menvcfg_val);

		/* Enable S-mode access to seed CSR */
		अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
			csr_set(CSR_MSECCFG, MSECCFG_SSEED);
			csr_clear(CSR_MSECCFG, MSECCFG_USEED);
		}
	}

	/* Disable all interrupts */
	csr_write(CSR_MIE, 0);

	/* Disable S-mode paging */
	अगर (misa_extension('S'))
		csr_write(CSR_SATP, 0);
}

अटल पूर्णांक fp_init(काष्ठा sbi_scratch *scratch)
{
#अगर_घोषित __riscv_flen
	पूर्णांक i;
#पूर्ण_अगर

	अगर (!misa_extension('D') && !misa_extension('F'))
		वापस 0;

	अगर (!(csr_read(CSR_MSTATUS) & MSTATUS_FS))
		वापस SBI_EINVAL;

#अगर_घोषित __riscv_flen
	क्रम (i = 0; i < 32; i++)
		init_fp_reg(i);
	csr_write(CSR_FCSR, 0);
#पूर्ण_अगर

	वापस 0;
}

व्योम sbi_hart_delegation_dump(काष्ठा sbi_scratch *scratch,
			      स्थिर अक्षर *prefix, स्थिर अक्षर *suffix)
{
	अगर (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist*/
		वापस;

	sbi_printf("%sMIDELEG%s: 0x%" PRILX "\n",
		   prefix, suffix, csr_read(CSR_MIDELEG));
	sbi_printf("%sMEDELEG%s: 0x%" PRILX "\n",
		   prefix, suffix, csr_read(CSR_MEDELEG));
}

अचिन्हित पूर्णांक sbi_hart_mhpm_mask(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	वापस hfeatures->mhpm_mask;
}

अचिन्हित पूर्णांक sbi_hart_mhpm_bits(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	वापस hfeatures->mhpm_bits;
}

पूर्णांक sbi_hart_priv_version(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	वापस hfeatures->priv_version;
}

व्योम sbi_hart_get_priv_version_str(काष्ठा sbi_scratch *scratch,
				   अक्षर *version_str, पूर्णांक nvstr)
{
	अक्षर *अस्थायी;
	काष्ठा sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	चयन (hfeatures->priv_version) {
	हाल SBI_HART_PRIV_VER_1_10:
		अस्थायी = "v1.10";
		अवरोध;
	हाल SBI_HART_PRIV_VER_1_11:
		अस्थायी = "v1.11";
		अवरोध;
	हाल SBI_HART_PRIV_VER_1_12:
		अस्थायी = "v1.12";
		अवरोध;
	शेष:
		अस्थायी = "unknown";
		अवरोध;
	}

	sbi_snprintf(version_str, nvstr, "%s", अस्थायी);
}

अटल अंतरभूत व्योम __sbi_hart_update_extension(
					काष्ठा sbi_hart_features *hfeatures,
					क्रमागत sbi_hart_extensions ext,
					bool enable)
{
	अगर (enable)
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
व्योम sbi_hart_update_extension(काष्ठा sbi_scratch *scratch,
			       क्रमागत sbi_hart_extensions ext,
			       bool enable)
{
	काष्ठा sbi_hart_features *hfeatures =
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
bool sbi_hart_has_extension(काष्ठा sbi_scratch *scratch,
			    क्रमागत sbi_hart_extensions ext)
{
	काष्ठा sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	अगर (__test_bit(ext, hfeatures->extensions))
		वापस true;
	अन्यथा
		वापस false;
}

#घोषणा __SBI_HART_EXT_DATA(_name, _id) {	\
	.name = #_name,				\
	.id = _id,				\
}

स्थिर काष्ठा sbi_hart_ext_तथ्य sbi_hart_ext[] = {
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
व्योम sbi_hart_get_extensions_str(काष्ठा sbi_scratch *scratch,
				 अक्षर *extensions_str, पूर्णांक nestr)
{
	काष्ठा sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);
	पूर्णांक offset = 0, ext = 0;

	अगर (!extensions_str || nestr <= 0)
		वापस;
	sbi_memset(extensions_str, 0, nestr);

	for_each_set_bit(ext, hfeatures->extensions, SBI_HART_EXT_MAX) {
		अगर (offset + sbi_strlen(sbi_hart_ext[ext].name) + 1 > nestr) {
			sbi_printf("%s:extension name is longer than buffer (error)\n", __func__);
			अवरोध;
		}
		sbi_snprintf(extensions_str + offset,
				 nestr - offset,
				 "%s,", sbi_hart_ext[ext].name);
		offset = offset + sbi_strlen(sbi_hart_ext[ext].name) + 1;
	}

	अगर (offset)
		extensions_str[offset - 1] = '\0';
	अन्यथा
		sbi_strncpy(extensions_str, "none", nestr);
}

/**
 * Check whether a particular CSR is present on the HART
 *
 * @param scratch pointer to the HART scratch space
 * @param csr the CSR number to check
 */
bool sbi_hart_has_csr(काष्ठा sbi_scratch *scratch, क्रमागत sbi_hart_csrs csr)
{
	काष्ठा sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	वापस __test_bit(csr, hfeatures->csrs);
}

अटल अचिन्हित दीर्घ hart_pmp_get_allowed_addr(व्योम)
{
	अचिन्हित दीर्घ val = 0;
	काष्ठा sbi_trap_info trap = {0};

	csr_write_allowed(CSR_PMPCFG0, &trap, 0);
	अगर (trap.cause)
		वापस 0;

	csr_write_allowed(CSR_PMPADDR0, &trap, PMP_ADDR_MASK);
	अगर (!trap.cause) {
		val = csr_read_allowed(CSR_PMPADDR0, &trap);
		अगर (trap.cause)
			val = 0;
	}

	वापस val;
}

अटल पूर्णांक hart_mhpm_get_allowed_bits(व्योम)
{
	अचिन्हित दीर्घ val = ~(0UL);
	काष्ठा sbi_trap_info trap = {0};
	पूर्णांक अंक_bits = 0;

	/**
	 * It is assumed that platforms will implement same number of bits for
	 * all the performance counters including mcycle/minstret.
	 */
	csr_write_allowed(CSR_MHPMCOUNTER3, &trap, val);
	अगर (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3, &trap);
		अगर (trap.cause)
			वापस 0;
	}
	अंक_bits = sbi_fls(val) + 1;
#अगर __riscv_xlen == 32
	csr_write_allowed(CSR_MHPMCOUNTER3H, &trap, val);
	अगर (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER3H, &trap);
		अगर (trap.cause)
			वापस अंक_bits;
	}
	अंक_bits += sbi_fls(val) + 1;

#पूर्ण_अगर

	वापस अंक_bits;
}

अटल पूर्णांक hart_detect_features(काष्ठा sbi_scratch *scratch, bool cold_boot)
{
	काष्ठा sbi_trap_info trap = {0};
	काष्ठा sbi_hart_features *hfeatures =
		sbi_scratch_offset_ptr(scratch, hart_features_offset);
	अचिन्हित दीर्घ val, oldval;
	पूर्णांक rc;

	/* If hart features already detected then do nothing */
	अगर (hfeatures->detected)
		वापस 0;

	/*
	 * Parse device tree extensions early, before any trap-based checks.
	 * Needed to detect Smrnmi and install NMI handlers before CSR probes
	 * that may trigger traps.
	 */
	rc = sbi_platform_extensions_init(sbi_platform_ptr(scratch), cold_boot);
	अगर (rc)
		वापस rc;

	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMRNMI)) {
		स्थिर काष्ठा sbi_platform *plat = sbi_platform_thishart_ptr();
		स्थिर काष्ठा sbi_platform_operations *ops = sbi_platform_ops(plat);
		बाह्य व्योम _trap_rnmi_handler(व्योम);
		बाह्य व्योम _trap_handler(व्योम);

		अगर (!ops || !ops->smrnmi_handlers_init)
			sbi_panic("Smrnmi detected, but platform lacks smrnmi_handlers_init callback\n");

		/* Reuse _trap_handler for the RNME slot since RNME is taken
		 * as a regular M-mode trap with NMIE=0. */
		ops->smrnmi_handlers_init(_trap_rnmi_handler, _trap_handler);

		/* Initialize MNSCRATCH for the RNMI handler */
		csr_write(CSR_MNSCRATCH, scratch);

		/* Enable NMIs */
		csr_set(CSR_MNSTATUS, MNSTATUS_NMIE);
	}

#घोषणा __check_hpm_csr(__csr, __mask) 					  \
	oldval = csr_read_allowed(__csr, &trap);			  \
	अगर (!trap.cause) {						  \
		csr_write_allowed(__csr, &trap, 1UL);			  \
		अगर (!trap.cause && csr_swap(__csr, oldval) == 1UL) {	  \
			(hfeatures->__mask) |= 1 << (__csr - CSR_MCYCLE); \
		}							  \
	}

#घोषणा __check_hpm_csr_2(__csr, __mask)	 		  \
	__check_hpm_csr(__csr + 0, __mask)	 		  \
	__check_hpm_csr(__csr + 1, __mask)
#घोषणा __check_hpm_csr_4(__csr, __mask)	 		  \
	__check_hpm_csr_2(__csr + 0, __mask) 			  \
	__check_hpm_csr_2(__csr + 2, __mask)
#घोषणा __check_hpm_csr_8(__csr, __mask)	 		  \
	__check_hpm_csr_4(__csr + 0, __mask) 			  \
	__check_hpm_csr_4(__csr + 4, __mask)
#घोषणा __check_hpm_csr_16(__csr, __mask)	 		  \
	__check_hpm_csr_8(__csr + 0, __mask) 			  \
	__check_hpm_csr_8(__csr + 8, __mask)

#घोषणा __check_csr(__csr, __rdonly, __wrval, __field, __skip)		\
	oldval = csr_read_allowed(__csr, &trap);			\
	अगर (!trap.cause) {						\
		अगर (__rdonly) {						\
			(hfeatures->__field)++;				\
		} अन्यथा {						\
			csr_write_allowed(__csr, &trap, __wrval);	\
			अगर (!trap.cause) {				\
				अगर ((csr_swap(__csr, oldval) & __wrval) == __wrval)	\
					(hfeatures->__field)++;		\
				अन्यथा					\
					जाओ __skip;			\
			} अन्यथा {					\
				जाओ __skip;				\
			}						\
		}							\
	} अन्यथा {							\
		जाओ __skip;						\
	}
#घोषणा __check_csr_2(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 1, __rdonly, __wrval, __field, __skip)
#घोषणा __check_csr_4(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 2, __rdonly, __wrval, __field, __skip)
#घोषणा __check_csr_8(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 4, __rdonly, __wrval, __field, __skip)
#घोषणा __check_csr_16(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 8, __rdonly, __wrval, __field, __skip)
#घोषणा __check_csr_32(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 16, __rdonly, __wrval, __field, __skip)
#घोषणा __check_csr_64(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 32, __rdonly, __wrval, __field, __skip)

	/**
	 * Detect the allowed address bits & granularity. At least PMPADDR0
	 * should be implemented.
	 */
	val = hart_pmp_get_allowed_addr();
	अगर (val) {
		hfeatures->pmp_log2gran = sbi_ffs(val) + 2;
		hfeatures->pmp_addr_bits = sbi_fls(val) + 1;
		/* Detect number of PMP regions. At least PMPADDR0 should be implemented*/
		__check_csr_64(CSR_PMPADDR0, 0, val, pmp_गिनती, __pmp_skip);
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
#अघोषित __check_csr_64
#अघोषित __check_csr_32
#अघोषित __check_csr_16
#अघोषित __check_csr_8
#अघोषित __check_csr_4
#अघोषित __check_csr_2
#अघोषित __check_csr


#घोषणा __check_priv(__csr, __base_priv, __priv)			\
	val = csr_read_allowed(__csr, &trap);				\
	अगर (!trap.cause && (hfeatures->priv_version >= __base_priv)) {	\
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

#अघोषित __check_priv_csr

#घोषणा __check_ext_csr(__base_priv, __csr, __ext)			\
	अगर (hfeatures->priv_version >= __base_priv) {			\
		csr_read_allowed(__csr, &trap);				\
		अगर (!trap.cause)					\
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

#अघोषित __check_ext_csr

#घोषणा __check_csr_existence(__csr, __csr_id)				\
	csr_read_allowed(__csr, &trap);					\
	अगर (!trap.cause)						\
		__set_bit(__csr_id, hfeatures->csrs);

	__check_csr_existence(CSR_CYCLE, SBI_HART_CSR_CYCLE);
	__check_csr_existence(CSR_TIME, SBI_HART_CSR_TIME);
	__check_csr_existence(CSR_INSTRET, SBI_HART_CSR_INSTRET);

#अघोषित __check_csr_existence

	/* Zicntr should only be detected using traps */
	__sbi_hart_update_extension(hfeatures, SBI_HART_EXT_ZICNTR,
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME)  &&
			    sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET));

	/* Extensions implied by other extensions and features */
	अगर (hfeatures->mhpm_mask)
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
	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP) &&
	    (csr_read(CSR_MSECCFG) & MSECCFG_MML))
		वापस SBI_EILL;

	वापस 0;
}

पूर्णांक sbi_hart_reinit(काष्ठा sbi_scratch *scratch)
{
	पूर्णांक rc;

	mstatus_init(scratch);

	rc = fp_init(scratch);
	अगर (rc)
		वापस rc;

	वापस 0;
}

पूर्णांक sbi_hart_init(काष्ठा sbi_scratch *scratch, bool cold_boot)
{
	पूर्णांक rc;

	/*
	 * Clear mip CSR before proceeding with init to avoid any spurious
	 * external interrupts in S-mode.
	 */
	csr_write(CSR_MIP, 0);

	अगर (cold_boot) {
		अगर (misa_extension('H'))
			sbi_hart_expected_trap = &__sbi_expected_trap_hext;

		hart_features_offset = sbi_scratch_alloc_offset(
					माप(काष्ठा sbi_hart_features));
		अगर (!hart_features_offset)
			वापस SBI_ENOMEM;
	}

	rc = hart_detect_features(scratch, cold_boot);
	अगर (rc)
		वापस rc;

	अगर (cold_boot) {
		rc = sbi_hart_pmp_init(scratch);
		अगर (rc)
			वापस rc;
	}

	वापस sbi_hart_reinit(scratch);
}

व्योम __attribute__((noreturn)) sbi_hart_hang(व्योम)
{
	जबतक (1)
		wfi();
	__builtin_unreachable();
}

व्योम __attribute__((noreturn))
sbi_hart_switch_mode(अचिन्हित दीर्घ arg0, अचिन्हित दीर्घ arg1,
		     अचिन्हित दीर्घ अग्र_addr, अचिन्हित दीर्घ अग्र_mode,
		     bool अग्र_virt)
{
#अगर __riscv_xlen == 32
	अचिन्हित दीर्घ val, valH;
#अन्यथा
	अचिन्हित दीर्घ val;
#पूर्ण_अगर

	चयन (अग्र_mode) {
	हाल PRV_M:
		अवरोध;
	हाल PRV_S:
		अगर (!misa_extension('S'))
			sbi_hart_hang();
		अवरोध;
	हाल PRV_U:
		अगर (!misa_extension('U'))
			sbi_hart_hang();
		अवरोध;
	शेष:
		sbi_hart_hang();
	}

	val = csr_read(CSR_MSTATUS);
	val = जोड़ो_FIELD(val, MSTATUS_MPP, अग्र_mode);
	val = जोड़ो_FIELD(val, MSTATUS_MPIE, 0);
#अगर __riscv_xlen == 32
	अगर (misa_extension('H')) {
		valH = csr_read(CSR_MSTATUSH);
		valH = जोड़ो_FIELD(valH, MSTATUSH_MPV, अग्र_virt);
		csr_write(CSR_MSTATUSH, valH);
	}
#अन्यथा
	अगर (misa_extension('H'))
		val = जोड़ो_FIELD(val, MSTATUS_MPV, अग्र_virt);
#पूर्ण_अगर
	csr_write(CSR_MSTATUS, val);
	csr_write(CSR_MEPC, अग्र_addr);

	अगर (अग्र_mode == PRV_S) {
		अगर (अग्र_virt) {
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
	} अन्यथा अगर (अग्र_mode == PRV_U) {
		अगर (misa_extension('N')) {
			csr_write(CSR_UTVEC, अग्र_addr);
			csr_write(CSR_USCRATCH, 0);
			csr_write(CSR_UIE, 0);
		}
	}

	रेजिस्टर अचिन्हित दीर्घ a0 यंत्र("a0") = arg0;
	रेजिस्टर अचिन्हित दीर्घ a1 यंत्र("a1") = arg1;
	__asm__ __volatile__("mret" : : "r"(a0), "r"(a1));
	__builtin_unreachable();
}
