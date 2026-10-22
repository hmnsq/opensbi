/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmp.h>

/* determine CPU extension, return non-zero support */
ಪೂರ್ಣಾಂಕ misa_extension_imp(ಅಕ್ಷರ ext)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ misa = csr_read(CSR_MISA);

	ಆದರೆ (misa) {
		ಆದರೆ ('A' <= ext && ext <= 'Z')
			ಹಿಂತಿರುಗಿ misa & (1 << (ext - 'A'));
		ಆದರೆ ('a' <= ext && ext <= 'z')
			ಹಿಂತಿರುಗಿ misa & (1 << (ext - 'a'));
		ಹಿಂತಿರುಗಿ 0;
	}

	ಹಿಂತಿರುಗಿ sbi_platform_misa_extension(sbi_platform_thishart_ptr(), ext);
}

ಪೂರ್ಣಾಂಕ misa_xlen(ಶೂನ್ಯ)
{
	ದೀರ್ಘ r;

	ಆದರೆ (csr_read(CSR_MISA) == 0)
		ಹಿಂತಿರುಗಿ sbi_platform_misa_xlen(sbi_platform_thishart_ptr());

	__asm__ __volatile__(
		"csrr   t0, misa\n\t"
		"slti   t1, t0, 0\n\t"
		"slli   t1, t1, 1\n\t"
		"slli   t0, t0, 1\n\t"
		"slti   t0, t0, 0\n\t"
		"add    %0, t0, t1"
		: "=r"(r)
		:
		: "t0", "t1");

	ಹಿಂತಿರುಗಿ r ? r : -1;
}

ಶೂನ್ಯ misa_string(ಪೂರ್ಣಾಂಕ xlen, ಅಕ್ಷರ *ಹೊರಹೋಗು, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_sz)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i, pos = 0;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ valid_isa_ಕ್ರಮ[] = "iemafdqclbjtpvnhkorwxyzg";

	ಆದರೆ (!ಹೊರಹೋಗು)
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (5 <= (ಹೊರಹೋಗು_sz - pos)) {
		ಹೊರಹೋಗು[pos++] = 'r';
		ಹೊರಹೋಗು[pos++] = 'v';
		ಆಯ್ಕೆ (xlen) {
		ಸ್ಥಿತಿ 1:
			ಹೊರಹೋಗು[pos++] = '3';
			ಹೊರಹೋಗು[pos++] = '2';
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 2:
			ಹೊರಹೋಗು[pos++] = '6';
			ಹೊರಹೋಗು[pos++] = '4';
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 3:
			ಹೊರಹೋಗು[pos++] = '1';
			ಹೊರಹೋಗು[pos++] = '2';
			ಹೊರಹೋಗು[pos++] = '8';
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			sbi_panic("%s: Unknown misa.MXL encoding %d",
				   __func__, xlen);
			ಹಿಂತಿರುಗಿ;
		}
	}

	ಗಾಗಿ (i = 0; i < array_ಗಾತ್ರ(valid_isa_ಕ್ರಮ) && (pos < ಹೊರಹೋಗು_sz); i++) {
		ಆದರೆ (misa_extension_imp(valid_isa_ಕ್ರಮ[i]))
			ಹೊರಹೋಗು[pos++] = valid_isa_ಕ್ರಮ[i];
	}

	ಆದರೆ (pos < ಹೊರಹೋಗು_sz)
		ಹೊರಹೋಗು[pos++] = '\0';
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ csr_read_ಅಂಕೆ(ಪೂರ್ಣಾಂಕ csr_ಅಂಕೆ)
{
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_read(__csr_num)			\
	ಸ್ಥಿತಿ __csr_num:					\
		ಹಿಂತಿರುಗಿ csr_read(__csr_num);
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_read_2(__csr_num)		\
	switchcase_csr_read(__csr_num + 0)		\
	switchcase_csr_read(__csr_num + 1)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_read_4(__csr_num)		\
	switchcase_csr_read_2(__csr_num + 0)		\
	switchcase_csr_read_2(__csr_num + 2)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_read_8(__csr_num)		\
	switchcase_csr_read_4(__csr_num + 0)		\
	switchcase_csr_read_4(__csr_num + 4)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_read_16(__csr_num)		\
	switchcase_csr_read_8(__csr_num + 0)		\
	switchcase_csr_read_8(__csr_num + 8)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_read_32(__csr_num)		\
	switchcase_csr_read_16(__csr_num + 0)		\
	switchcase_csr_read_16(__csr_num + 16)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_read_64(__csr_num)		\
	switchcase_csr_read_32(__csr_num + 0)		\
	switchcase_csr_read_32(__csr_num + 32)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_read_128(__csr_num)		\
	switchcase_csr_read_64(__csr_num + 0)		\
	switchcase_csr_read_64(__csr_num + 64)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_read_256(__csr_num)		\
	switchcase_csr_read_128(__csr_num + 0)		\
	switchcase_csr_read_128(__csr_num + 128)

	ಆಯ್ಕೆ (csr_ಅಂಕೆ) {
	switchcase_csr_read_16(CSR_PMPCFG0)
	switchcase_csr_read_64(CSR_PMPADDR0)
	switchcase_csr_read(CSR_MCYCLE)
	switchcase_csr_read(CSR_MINSTRET)
	switchcase_csr_read(CSR_MHPMCOUNTER3)
	switchcase_csr_read_4(CSR_MHPMCOUNTER4)
	switchcase_csr_read_8(CSR_MHPMCOUNTER8)
	switchcase_csr_read_16(CSR_MHPMCOUNTER16)
	switchcase_csr_read(CSR_MCOUNTINHIBIT)
	switchcase_csr_read(CSR_MCYCLECFG)
	switchcase_csr_read(CSR_MINSTRETCFG)
	switchcase_csr_read(CSR_MHPMEVENT3)
	switchcase_csr_read_4(CSR_MHPMEVENT4)
	switchcase_csr_read_8(CSR_MHPMEVENT8)
	switchcase_csr_read_16(CSR_MHPMEVENT16)
#ಆದರೆ __riscv_xlen == 32
	switchcase_csr_read(CSR_MCYCLEH)
	switchcase_csr_read(CSR_MINSTRETH)
	switchcase_csr_read(CSR_MHPMCOUNTER3H)
	switchcase_csr_read_4(CSR_MHPMCOUNTER4H)
	switchcase_csr_read_8(CSR_MHPMCOUNTER8H)
	switchcase_csr_read_16(CSR_MHPMCOUNTER16H)
	/**
	 * The CSR range M[CYCLE, INSTRET]CFGH are available only if smcntrpmf
	 * extension is present. The caller must ensure that.
	 */
	switchcase_csr_read(CSR_MCYCLECFGH)
	switchcase_csr_read(CSR_MINSTRETCFGH)
	/**
	 * The CSR range MHPMEVENT[3-16]H are available only if sscofpmf
	 * extension is present. The caller must ensure that.
	 */
	switchcase_csr_read(CSR_MHPMEVENT3H)
	switchcase_csr_read_4(CSR_MHPMEVENT4H)
	switchcase_csr_read_8(CSR_MHPMEVENT8H)
	switchcase_csr_read_16(CSR_MHPMEVENT16H)
#ಆದರೆ_ಅಂತ್ಯ
	switchcase_csr_read_256(CSR_CUSTOM0_U_RW_ತಳ)
	switchcase_csr_read_64(CSR_CUSTOM1_U_RO_ತಳ)
	switchcase_csr_read_64(CSR_CUSTOM2_S_RW_ತಳ)
	switchcase_csr_read_64(CSR_CUSTOM3_S_RW_ತಳ)
	switchcase_csr_read_64(CSR_CUSTOM4_S_RO_ತಳ)
	switchcase_csr_read_64(CSR_CUSTOM5_HS_RW_ತಳ)
	switchcase_csr_read_64(CSR_CUSTOM6_HS_RW_ತಳ)
	switchcase_csr_read_64(CSR_CUSTOM7_HS_RO_ತಳ)
	switchcase_csr_read_64(CSR_CUSTOM8_M_RW_ತಳ)
	switchcase_csr_read_64(CSR_CUSTOM9_M_RW_ತಳ)
	switchcase_csr_read_64(CSR_CUSTOM10_M_RO_ತಳ)

	ಪೂರ್ವನಿಯೋಜಿತ:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_ಅಂಕೆ);
		ಹಿಂತಿರುಗಿ 0;
	}

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_read_256
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_read_128
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_read_64
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_read_32
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_read_16
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_read_8
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_read_4
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_read_2
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_read
}

ಶೂನ್ಯ csr_write_ಅಂಕೆ(ಪೂರ್ಣಾಂಕ csr_ಅಂಕೆ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val)
{
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_write(__csr_num, __val)		\
	ಸ್ಥಿತಿ __csr_num:					\
		csr_write(__csr_num, __val);		\
		ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_write_2(__csr_num, __val)	\
	switchcase_csr_write(__csr_num + 0, __val)	\
	switchcase_csr_write(__csr_num + 1, __val)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_write_4(__csr_num, __val)	\
	switchcase_csr_write_2(__csr_num + 0, __val)	\
	switchcase_csr_write_2(__csr_num + 2, __val)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_write_8(__csr_num, __val)	\
	switchcase_csr_write_4(__csr_num + 0, __val)	\
	switchcase_csr_write_4(__csr_num + 4, __val)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_write_16(__csr_num, __val)	\
	switchcase_csr_write_8(__csr_num + 0, __val)	\
	switchcase_csr_write_8(__csr_num + 8, __val)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_write_32(__csr_num, __val)	\
	switchcase_csr_write_16(__csr_num + 0, __val)	\
	switchcase_csr_write_16(__csr_num + 16, __val)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_write_64(__csr_num, __val)	\
	switchcase_csr_write_32(__csr_num + 0, __val)	\
	switchcase_csr_write_32(__csr_num + 32, __val)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_write_128(__csr_num, __val)	\
	switchcase_csr_write_64(__csr_num + 0, __val)	\
	switchcase_csr_write_64(__csr_num + 64, __val)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_csr_write_256(__csr_num, __val)	\
	switchcase_csr_write_128(__csr_num + 0, __val)	\
	switchcase_csr_write_128(__csr_num + 128, __val)

	ಆಯ್ಕೆ (csr_ಅಂಕೆ) {
	switchcase_csr_write_16(CSR_PMPCFG0, val)
	switchcase_csr_write_64(CSR_PMPADDR0, val)
	switchcase_csr_write(CSR_MCYCLE, val)
	switchcase_csr_write(CSR_MINSTRET, val)
	switchcase_csr_write(CSR_MHPMCOUNTER3, val)
	switchcase_csr_write_4(CSR_MHPMCOUNTER4, val)
	switchcase_csr_write_8(CSR_MHPMCOUNTER8, val)
	switchcase_csr_write_16(CSR_MHPMCOUNTER16, val)
#ಆದರೆ __riscv_xlen == 32
	switchcase_csr_write(CSR_MCYCLEH, val)
	switchcase_csr_write(CSR_MINSTRETH, val)
	switchcase_csr_write(CSR_MHPMCOUNTER3H, val)
	switchcase_csr_write_4(CSR_MHPMCOUNTER4H, val)
	switchcase_csr_write_8(CSR_MHPMCOUNTER8H, val)
	switchcase_csr_write_16(CSR_MHPMCOUNTER16H, val)
	switchcase_csr_write(CSR_MCYCLECFGH, val)
	switchcase_csr_write(CSR_MINSTRETCFGH, val)
	switchcase_csr_write(CSR_MHPMEVENT3H, val)
	switchcase_csr_write_4(CSR_MHPMEVENT4H, val)
	switchcase_csr_write_8(CSR_MHPMEVENT8H, val)
	switchcase_csr_write_16(CSR_MHPMEVENT16H, val)
#ಆದರೆ_ಅಂತ್ಯ
	switchcase_csr_write(CSR_MCOUNTINHIBIT, val)
	switchcase_csr_write(CSR_MCYCLECFG, val)
	switchcase_csr_write(CSR_MINSTRETCFG, val)
	switchcase_csr_write(CSR_MHPMEVENT3, val)
	switchcase_csr_write_4(CSR_MHPMEVENT4, val)
	switchcase_csr_write_8(CSR_MHPMEVENT8, val)
	switchcase_csr_write_16(CSR_MHPMEVENT16, val)
	switchcase_csr_write_256(CSR_CUSTOM0_U_RW_ತಳ, val)
	switchcase_csr_write_64(CSR_CUSTOM2_S_RW_ತಳ, val)
	switchcase_csr_write_64(CSR_CUSTOM3_S_RW_ತಳ, val)
	switchcase_csr_write_64(CSR_CUSTOM5_HS_RW_ತಳ, val)
	switchcase_csr_write_64(CSR_CUSTOM6_HS_RW_ತಳ, val)
	switchcase_csr_write_64(CSR_CUSTOM8_M_RW_ತಳ, val)
	switchcase_csr_write_64(CSR_CUSTOM9_M_RW_ತಳ, val)

	ಪೂರ್ವನಿಯೋಜಿತ:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_ಅಂಕೆ);
		ನಿಲ್ಲಿಸು;
	}

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_write_256
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_write_128
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_write_64
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_write_32
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_write_16
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_write_8
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_write_4
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_write_2
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_csr_write
}
