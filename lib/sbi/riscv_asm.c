/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_pmp.h>

/* determine CPU extension, return non-zero support */
صحيح misa_extension_imp(حرف ext)
{
	بیعلامت دراز misa = csr_read(CSR_MISA);

	إذا (misa) {
		إذا ('A' <= ext && ext <= 'Z')
			أرجع misa & (1 << (ext - 'A'));
		إذا ('a' <= ext && ext <= 'z')
			أرجع misa & (1 << (ext - 'a'));
		أرجع 0;
	}

	أرجع sbi_platform_misa_extension(sbi_platform_thishart_ptr(), ext);
}

صحيح misa_xlen(فراغ)
{
	دراز r;

	إذا (csr_read(CSR_MISA) == 0)
		أرجع sbi_platform_misa_xlen(sbi_platform_thishart_ptr());

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

	أرجع r ? r : -1;
}

فراغ misa_string(صحيح xlen, حرف *بیرون, بیعلامت صحيح بیرون_sz)
{
	بیعلامت صحيح i, pos = 0;
	ثابت حرف valid_isa_ترتیب[] = "iemafdqclbjtpvnhkorwxyzg";

	إذا (!بیرون)
		أرجع;

	إذا (5 <= (بیرون_sz - pos)) {
		بیرون[pos++] = 'r';
		بیرون[pos++] = 'v';
		بدل (xlen) {
		حالة 1:
			بیرون[pos++] = '3';
			بیرون[pos++] = '2';
			اكسر;
		حالة 2:
			بیرون[pos++] = '6';
			بیرون[pos++] = '4';
			اكسر;
		حالة 3:
			بیرون[pos++] = '1';
			بیرون[pos++] = '2';
			بیرون[pos++] = '8';
			اكسر;
		افتراضي:
			sbi_panic("%s: Unknown misa.MXL encoding %d",
				   __func__, xlen);
			أرجع;
		}
	}

	لكل (i = 0; i < array_اندازه(valid_isa_ترتیب) && (pos < بیرون_sz); i++) {
		إذا (misa_extension_imp(valid_isa_ترتیب[i]))
			بیرون[pos++] = valid_isa_ترتیب[i];
	}

	إذا (pos < بیرون_sz)
		بیرون[pos++] = '\0';
}

بیعلامت دراز csr_read_رقم(صحيح csr_رقم)
{
#تعریف_کن switchcase_csr_read(__csr_num)			\
	حالة __csr_num:					\
		أرجع csr_read(__csr_num);
#تعریف_کن switchcase_csr_read_2(__csr_num)		\
	switchcase_csr_read(__csr_num + 0)		\
	switchcase_csr_read(__csr_num + 1)
#تعریف_کن switchcase_csr_read_4(__csr_num)		\
	switchcase_csr_read_2(__csr_num + 0)		\
	switchcase_csr_read_2(__csr_num + 2)
#تعریف_کن switchcase_csr_read_8(__csr_num)		\
	switchcase_csr_read_4(__csr_num + 0)		\
	switchcase_csr_read_4(__csr_num + 4)
#تعریف_کن switchcase_csr_read_16(__csr_num)		\
	switchcase_csr_read_8(__csr_num + 0)		\
	switchcase_csr_read_8(__csr_num + 8)
#تعریف_کن switchcase_csr_read_32(__csr_num)		\
	switchcase_csr_read_16(__csr_num + 0)		\
	switchcase_csr_read_16(__csr_num + 16)
#تعریف_کن switchcase_csr_read_64(__csr_num)		\
	switchcase_csr_read_32(__csr_num + 0)		\
	switchcase_csr_read_32(__csr_num + 32)
#تعریف_کن switchcase_csr_read_128(__csr_num)		\
	switchcase_csr_read_64(__csr_num + 0)		\
	switchcase_csr_read_64(__csr_num + 64)
#تعریف_کن switchcase_csr_read_256(__csr_num)		\
	switchcase_csr_read_128(__csr_num + 0)		\
	switchcase_csr_read_128(__csr_num + 128)

	بدل (csr_رقم) {
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
#إذا __riscv_xlen == 32
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
#پایان_اگر
	switchcase_csr_read_256(CSR_CUSTOM0_U_RW_پایه)
	switchcase_csr_read_64(CSR_CUSTOM1_U_RO_پایه)
	switchcase_csr_read_64(CSR_CUSTOM2_S_RW_پایه)
	switchcase_csr_read_64(CSR_CUSTOM3_S_RW_پایه)
	switchcase_csr_read_64(CSR_CUSTOM4_S_RO_پایه)
	switchcase_csr_read_64(CSR_CUSTOM5_HS_RW_پایه)
	switchcase_csr_read_64(CSR_CUSTOM6_HS_RW_پایه)
	switchcase_csr_read_64(CSR_CUSTOM7_HS_RO_پایه)
	switchcase_csr_read_64(CSR_CUSTOM8_M_RW_پایه)
	switchcase_csr_read_64(CSR_CUSTOM9_M_RW_پایه)
	switchcase_csr_read_64(CSR_CUSTOM10_M_RO_پایه)

	افتراضي:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_رقم);
		أرجع 0;
	}

#لغو_تعریف switchcase_csr_read_256
#لغو_تعریف switchcase_csr_read_128
#لغو_تعریف switchcase_csr_read_64
#لغو_تعریف switchcase_csr_read_32
#لغو_تعریف switchcase_csr_read_16
#لغو_تعریف switchcase_csr_read_8
#لغو_تعریف switchcase_csr_read_4
#لغو_تعریف switchcase_csr_read_2
#لغو_تعریف switchcase_csr_read
}

فراغ csr_write_رقم(صحيح csr_رقم, بیعلامت دراز val)
{
#تعریف_کن switchcase_csr_write(__csr_num, __val)		\
	حالة __csr_num:					\
		csr_write(__csr_num, __val);		\
		اكسر;
#تعریف_کن switchcase_csr_write_2(__csr_num, __val)	\
	switchcase_csr_write(__csr_num + 0, __val)	\
	switchcase_csr_write(__csr_num + 1, __val)
#تعریف_کن switchcase_csr_write_4(__csr_num, __val)	\
	switchcase_csr_write_2(__csr_num + 0, __val)	\
	switchcase_csr_write_2(__csr_num + 2, __val)
#تعریف_کن switchcase_csr_write_8(__csr_num, __val)	\
	switchcase_csr_write_4(__csr_num + 0, __val)	\
	switchcase_csr_write_4(__csr_num + 4, __val)
#تعریف_کن switchcase_csr_write_16(__csr_num, __val)	\
	switchcase_csr_write_8(__csr_num + 0, __val)	\
	switchcase_csr_write_8(__csr_num + 8, __val)
#تعریف_کن switchcase_csr_write_32(__csr_num, __val)	\
	switchcase_csr_write_16(__csr_num + 0, __val)	\
	switchcase_csr_write_16(__csr_num + 16, __val)
#تعریف_کن switchcase_csr_write_64(__csr_num, __val)	\
	switchcase_csr_write_32(__csr_num + 0, __val)	\
	switchcase_csr_write_32(__csr_num + 32, __val)
#تعریف_کن switchcase_csr_write_128(__csr_num, __val)	\
	switchcase_csr_write_64(__csr_num + 0, __val)	\
	switchcase_csr_write_64(__csr_num + 64, __val)
#تعریف_کن switchcase_csr_write_256(__csr_num, __val)	\
	switchcase_csr_write_128(__csr_num + 0, __val)	\
	switchcase_csr_write_128(__csr_num + 128, __val)

	بدل (csr_رقم) {
	switchcase_csr_write_16(CSR_PMPCFG0, val)
	switchcase_csr_write_64(CSR_PMPADDR0, val)
	switchcase_csr_write(CSR_MCYCLE, val)
	switchcase_csr_write(CSR_MINSTRET, val)
	switchcase_csr_write(CSR_MHPMCOUNTER3, val)
	switchcase_csr_write_4(CSR_MHPMCOUNTER4, val)
	switchcase_csr_write_8(CSR_MHPMCOUNTER8, val)
	switchcase_csr_write_16(CSR_MHPMCOUNTER16, val)
#إذا __riscv_xlen == 32
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
#پایان_اگر
	switchcase_csr_write(CSR_MCOUNTINHIBIT, val)
	switchcase_csr_write(CSR_MCYCLECFG, val)
	switchcase_csr_write(CSR_MINSTRETCFG, val)
	switchcase_csr_write(CSR_MHPMEVENT3, val)
	switchcase_csr_write_4(CSR_MHPMEVENT4, val)
	switchcase_csr_write_8(CSR_MHPMEVENT8, val)
	switchcase_csr_write_16(CSR_MHPMEVENT16, val)
	switchcase_csr_write_256(CSR_CUSTOM0_U_RW_پایه, val)
	switchcase_csr_write_64(CSR_CUSTOM2_S_RW_پایه, val)
	switchcase_csr_write_64(CSR_CUSTOM3_S_RW_پایه, val)
	switchcase_csr_write_64(CSR_CUSTOM5_HS_RW_پایه, val)
	switchcase_csr_write_64(CSR_CUSTOM6_HS_RW_پایه, val)
	switchcase_csr_write_64(CSR_CUSTOM8_M_RW_پایه, val)
	switchcase_csr_write_64(CSR_CUSTOM9_M_RW_پایه, val)

	افتراضي:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_رقم);
		اكسر;
	}

#لغو_تعریف switchcase_csr_write_256
#لغو_تعریف switchcase_csr_write_128
#لغو_تعریف switchcase_csr_write_64
#لغو_تعریف switchcase_csr_write_32
#لغو_تعریف switchcase_csr_write_16
#لغو_تعریف switchcase_csr_write_8
#لغو_تعریف switchcase_csr_write_4
#لغو_تعریف switchcase_csr_write_2
#لغو_تعریف switchcase_csr_write
}
