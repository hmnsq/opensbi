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
	بنانشانی لما misa = csr_read(CSR_MISA);

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
	لما r;

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

فراغ misa_string(صحيح xlen, حرف *باہرلا, بنانشانی صحيح باہرلا_sz)
{
	بنانشانی صحيح i, pos = 0;
	ثابت حرف valid_isa_ترتیب[] = "iemafdqclbjtpvnhkorwxyzg";

	إذا (!باہرلا)
		أرجع;

	إذا (5 <= (باہرلا_sz - pos)) {
		باہرلا[pos++] = 'r';
		باہرلا[pos++] = 'v';
		بدل (xlen) {
		حالة 1:
			باہرلا[pos++] = '3';
			باہرلا[pos++] = '2';
			اكسر;
		حالة 2:
			باہرلا[pos++] = '6';
			باہرلا[pos++] = '4';
			اكسر;
		حالة 3:
			باہرلا[pos++] = '1';
			باہرلا[pos++] = '2';
			باہرلا[pos++] = '8';
			اكسر;
		افتراضي:
			sbi_panic("%s: Unknown misa.MXL encoding %d",
				   __func__, xlen);
			أرجع;
		}
	}

	لكل (i = 0; i < array_ناپ(valid_isa_ترتیب) && (pos < باہرلا_sz); i++) {
		إذا (misa_extension_imp(valid_isa_ترتیب[i]))
			باہرلا[pos++] = valid_isa_ترتیب[i];
	}

	إذا (pos < باہرلا_sz)
		باہرلا[pos++] = '\0';
}

بنانشانی لما csr_read_عدد(صحيح csr_عدد)
{
#تعریف_کرو switchcase_csr_read(__csr_num)			\
	حالة __csr_num:					\
		أرجع csr_read(__csr_num);
#تعریف_کرو switchcase_csr_read_2(__csr_num)		\
	switchcase_csr_read(__csr_num + 0)		\
	switchcase_csr_read(__csr_num + 1)
#تعریف_کرو switchcase_csr_read_4(__csr_num)		\
	switchcase_csr_read_2(__csr_num + 0)		\
	switchcase_csr_read_2(__csr_num + 2)
#تعریف_کرو switchcase_csr_read_8(__csr_num)		\
	switchcase_csr_read_4(__csr_num + 0)		\
	switchcase_csr_read_4(__csr_num + 4)
#تعریف_کرو switchcase_csr_read_16(__csr_num)		\
	switchcase_csr_read_8(__csr_num + 0)		\
	switchcase_csr_read_8(__csr_num + 8)
#تعریف_کرو switchcase_csr_read_32(__csr_num)		\
	switchcase_csr_read_16(__csr_num + 0)		\
	switchcase_csr_read_16(__csr_num + 16)
#تعریف_کرو switchcase_csr_read_64(__csr_num)		\
	switchcase_csr_read_32(__csr_num + 0)		\
	switchcase_csr_read_32(__csr_num + 32)
#تعریف_کرو switchcase_csr_read_128(__csr_num)		\
	switchcase_csr_read_64(__csr_num + 0)		\
	switchcase_csr_read_64(__csr_num + 64)
#تعریف_کرو switchcase_csr_read_256(__csr_num)		\
	switchcase_csr_read_128(__csr_num + 0)		\
	switchcase_csr_read_128(__csr_num + 128)

	بدل (csr_عدد) {
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
#جے_انت
	switchcase_csr_read_256(CSR_CUSTOM0_U_RW_ادھار)
	switchcase_csr_read_64(CSR_CUSTOM1_U_RO_ادھار)
	switchcase_csr_read_64(CSR_CUSTOM2_S_RW_ادھار)
	switchcase_csr_read_64(CSR_CUSTOM3_S_RW_ادھار)
	switchcase_csr_read_64(CSR_CUSTOM4_S_RO_ادھار)
	switchcase_csr_read_64(CSR_CUSTOM5_HS_RW_ادھار)
	switchcase_csr_read_64(CSR_CUSTOM6_HS_RW_ادھار)
	switchcase_csr_read_64(CSR_CUSTOM7_HS_RO_ادھار)
	switchcase_csr_read_64(CSR_CUSTOM8_M_RW_ادھار)
	switchcase_csr_read_64(CSR_CUSTOM9_M_RW_ادھار)
	switchcase_csr_read_64(CSR_CUSTOM10_M_RO_ادھار)

	افتراضي:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_عدد);
		أرجع 0;
	}

#تعریف_رد switchcase_csr_read_256
#تعریف_رد switchcase_csr_read_128
#تعریف_رد switchcase_csr_read_64
#تعریف_رد switchcase_csr_read_32
#تعریف_رد switchcase_csr_read_16
#تعریف_رد switchcase_csr_read_8
#تعریف_رد switchcase_csr_read_4
#تعریف_رد switchcase_csr_read_2
#تعریف_رد switchcase_csr_read
}

فراغ csr_write_عدد(صحيح csr_عدد, بنانشانی لما val)
{
#تعریف_کرو switchcase_csr_write(__csr_num, __val)		\
	حالة __csr_num:					\
		csr_write(__csr_num, __val);		\
		اكسر;
#تعریف_کرو switchcase_csr_write_2(__csr_num, __val)	\
	switchcase_csr_write(__csr_num + 0, __val)	\
	switchcase_csr_write(__csr_num + 1, __val)
#تعریف_کرو switchcase_csr_write_4(__csr_num, __val)	\
	switchcase_csr_write_2(__csr_num + 0, __val)	\
	switchcase_csr_write_2(__csr_num + 2, __val)
#تعریف_کرو switchcase_csr_write_8(__csr_num, __val)	\
	switchcase_csr_write_4(__csr_num + 0, __val)	\
	switchcase_csr_write_4(__csr_num + 4, __val)
#تعریف_کرو switchcase_csr_write_16(__csr_num, __val)	\
	switchcase_csr_write_8(__csr_num + 0, __val)	\
	switchcase_csr_write_8(__csr_num + 8, __val)
#تعریف_کرو switchcase_csr_write_32(__csr_num, __val)	\
	switchcase_csr_write_16(__csr_num + 0, __val)	\
	switchcase_csr_write_16(__csr_num + 16, __val)
#تعریف_کرو switchcase_csr_write_64(__csr_num, __val)	\
	switchcase_csr_write_32(__csr_num + 0, __val)	\
	switchcase_csr_write_32(__csr_num + 32, __val)
#تعریف_کرو switchcase_csr_write_128(__csr_num, __val)	\
	switchcase_csr_write_64(__csr_num + 0, __val)	\
	switchcase_csr_write_64(__csr_num + 64, __val)
#تعریف_کرو switchcase_csr_write_256(__csr_num, __val)	\
	switchcase_csr_write_128(__csr_num + 0, __val)	\
	switchcase_csr_write_128(__csr_num + 128, __val)

	بدل (csr_عدد) {
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
#جے_انت
	switchcase_csr_write(CSR_MCOUNTINHIBIT, val)
	switchcase_csr_write(CSR_MCYCLECFG, val)
	switchcase_csr_write(CSR_MINSTRETCFG, val)
	switchcase_csr_write(CSR_MHPMEVENT3, val)
	switchcase_csr_write_4(CSR_MHPMEVENT4, val)
	switchcase_csr_write_8(CSR_MHPMEVENT8, val)
	switchcase_csr_write_16(CSR_MHPMEVENT16, val)
	switchcase_csr_write_256(CSR_CUSTOM0_U_RW_ادھار, val)
	switchcase_csr_write_64(CSR_CUSTOM2_S_RW_ادھار, val)
	switchcase_csr_write_64(CSR_CUSTOM3_S_RW_ادھار, val)
	switchcase_csr_write_64(CSR_CUSTOM5_HS_RW_ادھار, val)
	switchcase_csr_write_64(CSR_CUSTOM6_HS_RW_ادھار, val)
	switchcase_csr_write_64(CSR_CUSTOM8_M_RW_ادھار, val)
	switchcase_csr_write_64(CSR_CUSTOM9_M_RW_ادھار, val)

	افتراضي:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_عدد);
		اكسر;
	}

#تعریف_رد switchcase_csr_write_256
#تعریف_رد switchcase_csr_write_128
#تعریف_رد switchcase_csr_write_64
#تعریف_رد switchcase_csr_write_32
#تعریف_رد switchcase_csr_write_16
#تعریف_رد switchcase_csr_write_8
#تعریف_رد switchcase_csr_write_4
#تعریف_رد switchcase_csr_write_2
#تعریف_رد switchcase_csr_write
}
