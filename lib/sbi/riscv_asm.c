/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmp.h>

/* determine CPU extension, return non-zero support */
ਪੂਰਨ misa_extension_imp(ਅੱਖਰ ext)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ misa = csr_read(CSR_MISA);

	ਜੇ (misa) {
		ਜੇ ('A' <= ext && ext <= 'Z')
			ਵਾਪਸ misa & (1 << (ext - 'A'));
		ਜੇ ('a' <= ext && ext <= 'z')
			ਵਾਪਸ misa & (1 << (ext - 'a'));
		ਵਾਪਸ 0;
	}

	ਵਾਪਸ sbi_platform_misa_extension(sbi_platform_thishart_ptr(), ext);
}

ਪੂਰਨ misa_xlen(ਖਾਲੀ)
{
	ਲੰਮਾ r;

	ਜੇ (csr_read(CSR_MISA) == 0)
		ਵਾਪਸ sbi_platform_misa_xlen(sbi_platform_thishart_ptr());

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

	ਵਾਪਸ r ? r : -1;
}

ਖਾਲੀ misa_string(ਪੂਰਨ xlen, ਅੱਖਰ *ਬਾਹਰਲਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਬਾਹਰਲਾ_sz)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i, pos = 0;
	ਅਚਲ ਅੱਖਰ valid_isa_ਕ੍ਰਮ[] = "iemafdqclbjtpvnhkorwxyzg";

	ਜੇ (!ਬਾਹਰਲਾ)
		ਵਾਪਸ;

	ਜੇ (5 <= (ਬਾਹਰਲਾ_sz - pos)) {
		ਬਾਹਰਲਾ[pos++] = 'r';
		ਬਾਹਰਲਾ[pos++] = 'v';
		ਚੋਣ (xlen) {
		ਹਾਲਤ 1:
			ਬਾਹਰਲਾ[pos++] = '3';
			ਬਾਹਰਲਾ[pos++] = '2';
			ਰੋਕੋ;
		ਹਾਲਤ 2:
			ਬਾਹਰਲਾ[pos++] = '6';
			ਬਾਹਰਲਾ[pos++] = '4';
			ਰੋਕੋ;
		ਹਾਲਤ 3:
			ਬਾਹਰਲਾ[pos++] = '1';
			ਬਾਹਰਲਾ[pos++] = '2';
			ਬਾਹਰਲਾ[pos++] = '8';
			ਰੋਕੋ;
		ਮੂਲ:
			sbi_panic("%s: Unknown misa.MXL encoding %d",
				   __func__, xlen);
			ਵਾਪਸ;
		}
	}

	ਲਈ (i = 0; i < array_ਮਾਪ(valid_isa_ਕ੍ਰਮ) && (pos < ਬਾਹਰਲਾ_sz); i++) {
		ਜੇ (misa_extension_imp(valid_isa_ਕ੍ਰਮ[i]))
			ਬਾਹਰਲਾ[pos++] = valid_isa_ਕ੍ਰਮ[i];
	}

	ਜੇ (pos < ਬਾਹਰਲਾ_sz)
		ਬਾਹਰਲਾ[pos++] = '\0';
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ csr_read_ਅੰਕ(ਪੂਰਨ csr_ਅੰਕ)
{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_read(__csr_num)			\
	ਹਾਲਤ __csr_num:					\
		ਵਾਪਸ csr_read(__csr_num);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_read_2(__csr_num)		\
	switchcase_csr_read(__csr_num + 0)		\
	switchcase_csr_read(__csr_num + 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_read_4(__csr_num)		\
	switchcase_csr_read_2(__csr_num + 0)		\
	switchcase_csr_read_2(__csr_num + 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_read_8(__csr_num)		\
	switchcase_csr_read_4(__csr_num + 0)		\
	switchcase_csr_read_4(__csr_num + 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_read_16(__csr_num)		\
	switchcase_csr_read_8(__csr_num + 0)		\
	switchcase_csr_read_8(__csr_num + 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_read_32(__csr_num)		\
	switchcase_csr_read_16(__csr_num + 0)		\
	switchcase_csr_read_16(__csr_num + 16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_read_64(__csr_num)		\
	switchcase_csr_read_32(__csr_num + 0)		\
	switchcase_csr_read_32(__csr_num + 32)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_read_128(__csr_num)		\
	switchcase_csr_read_64(__csr_num + 0)		\
	switchcase_csr_read_64(__csr_num + 64)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_read_256(__csr_num)		\
	switchcase_csr_read_128(__csr_num + 0)		\
	switchcase_csr_read_128(__csr_num + 128)

	ਚੋਣ (csr_ਅੰਕ) {
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
#ਜੇ __riscv_xlen == 32
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
#ਜੇ_ਅੰਤ
	switchcase_csr_read_256(CSR_CUSTOM0_U_RW_ਅਧਾਰ)
	switchcase_csr_read_64(CSR_CUSTOM1_U_RO_ਅਧਾਰ)
	switchcase_csr_read_64(CSR_CUSTOM2_S_RW_ਅਧਾਰ)
	switchcase_csr_read_64(CSR_CUSTOM3_S_RW_ਅਧਾਰ)
	switchcase_csr_read_64(CSR_CUSTOM4_S_RO_ਅਧਾਰ)
	switchcase_csr_read_64(CSR_CUSTOM5_HS_RW_ਅਧਾਰ)
	switchcase_csr_read_64(CSR_CUSTOM6_HS_RW_ਅਧਾਰ)
	switchcase_csr_read_64(CSR_CUSTOM7_HS_RO_ਅਧਾਰ)
	switchcase_csr_read_64(CSR_CUSTOM8_M_RW_ਅਧਾਰ)
	switchcase_csr_read_64(CSR_CUSTOM9_M_RW_ਅਧਾਰ)
	switchcase_csr_read_64(CSR_CUSTOM10_M_RO_ਅਧਾਰ)

	ਮੂਲ:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_ਅੰਕ);
		ਵਾਪਸ 0;
	}

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_read_256
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_read_128
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_read_64
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_read_32
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_read_16
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_read_8
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_read_4
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_read_2
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_read
}

ਖਾਲੀ csr_write_ਅੰਕ(ਪੂਰਨ csr_ਅੰਕ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val)
{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_write(__csr_num, __val)		\
	ਹਾਲਤ __csr_num:					\
		csr_write(__csr_num, __val);		\
		ਰੋਕੋ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_write_2(__csr_num, __val)	\
	switchcase_csr_write(__csr_num + 0, __val)	\
	switchcase_csr_write(__csr_num + 1, __val)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_write_4(__csr_num, __val)	\
	switchcase_csr_write_2(__csr_num + 0, __val)	\
	switchcase_csr_write_2(__csr_num + 2, __val)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_write_8(__csr_num, __val)	\
	switchcase_csr_write_4(__csr_num + 0, __val)	\
	switchcase_csr_write_4(__csr_num + 4, __val)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_write_16(__csr_num, __val)	\
	switchcase_csr_write_8(__csr_num + 0, __val)	\
	switchcase_csr_write_8(__csr_num + 8, __val)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_write_32(__csr_num, __val)	\
	switchcase_csr_write_16(__csr_num + 0, __val)	\
	switchcase_csr_write_16(__csr_num + 16, __val)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_write_64(__csr_num, __val)	\
	switchcase_csr_write_32(__csr_num + 0, __val)	\
	switchcase_csr_write_32(__csr_num + 32, __val)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_write_128(__csr_num, __val)	\
	switchcase_csr_write_64(__csr_num + 0, __val)	\
	switchcase_csr_write_64(__csr_num + 64, __val)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_csr_write_256(__csr_num, __val)	\
	switchcase_csr_write_128(__csr_num + 0, __val)	\
	switchcase_csr_write_128(__csr_num + 128, __val)

	ਚੋਣ (csr_ਅੰਕ) {
	switchcase_csr_write_16(CSR_PMPCFG0, val)
	switchcase_csr_write_64(CSR_PMPADDR0, val)
	switchcase_csr_write(CSR_MCYCLE, val)
	switchcase_csr_write(CSR_MINSTRET, val)
	switchcase_csr_write(CSR_MHPMCOUNTER3, val)
	switchcase_csr_write_4(CSR_MHPMCOUNTER4, val)
	switchcase_csr_write_8(CSR_MHPMCOUNTER8, val)
	switchcase_csr_write_16(CSR_MHPMCOUNTER16, val)
#ਜੇ __riscv_xlen == 32
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
#ਜੇ_ਅੰਤ
	switchcase_csr_write(CSR_MCOUNTINHIBIT, val)
	switchcase_csr_write(CSR_MCYCLECFG, val)
	switchcase_csr_write(CSR_MINSTRETCFG, val)
	switchcase_csr_write(CSR_MHPMEVENT3, val)
	switchcase_csr_write_4(CSR_MHPMEVENT4, val)
	switchcase_csr_write_8(CSR_MHPMEVENT8, val)
	switchcase_csr_write_16(CSR_MHPMEVENT16, val)
	switchcase_csr_write_256(CSR_CUSTOM0_U_RW_ਅਧਾਰ, val)
	switchcase_csr_write_64(CSR_CUSTOM2_S_RW_ਅਧਾਰ, val)
	switchcase_csr_write_64(CSR_CUSTOM3_S_RW_ਅਧਾਰ, val)
	switchcase_csr_write_64(CSR_CUSTOM5_HS_RW_ਅਧਾਰ, val)
	switchcase_csr_write_64(CSR_CUSTOM6_HS_RW_ਅਧਾਰ, val)
	switchcase_csr_write_64(CSR_CUSTOM8_M_RW_ਅਧਾਰ, val)
	switchcase_csr_write_64(CSR_CUSTOM9_M_RW_ਅਧਾਰ, val)

	ਮੂਲ:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_ਅੰਕ);
		ਰੋਕੋ;
	}

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_write_256
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_write_128
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_write_64
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_write_32
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_write_16
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_write_8
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_write_4
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_write_2
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_csr_write
}
