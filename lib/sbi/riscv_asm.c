/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_pmp.h>

/* determine CPU extension, return non-zero support */
పూర్ణాంకం misa_extension_imp(అక్షరం ext)
{
	గుర్తులేని దీర్ఘ misa = csr_read(CSR_MISA);

	ఒకవేళ (misa) {
		ఒకవేళ ('A' <= ext && ext <= 'Z')
			తిరిగి misa & (1 << (ext - 'A'));
		ఒకవేళ ('a' <= ext && ext <= 'z')
			తిరిగి misa & (1 << (ext - 'a'));
		తిరిగి 0;
	}

	తిరిగి sbi_platform_misa_extension(sbi_platform_thishart_ptr(), ext);
}

పూర్ణాంకం misa_xlen(శూన్య)
{
	దీర్ఘ r;

	ఒకవేళ (csr_read(CSR_MISA) == 0)
		తిరిగి sbi_platform_misa_xlen(sbi_platform_thishart_ptr());

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

	తిరిగి r ? r : -1;
}

శూన్య misa_string(పూర్ణాంకం xlen, అక్షరం *బహిర్గతం, గుర్తులేని పూర్ణాంకం బహిర్గతం_sz)
{
	గుర్తులేని పూర్ణాంకం i, pos = 0;
	స్థిరరాశి అక్షరం valid_isa_క్రమం[] = "iemafdqclbjtpvnhkorwxyzg";

	ఒకవేళ (!బహిర్గతం)
		తిరిగి;

	ఒకవేళ (5 <= (బహిర్గతం_sz - pos)) {
		బహిర్గతం[pos++] = 'r';
		బహిర్గతం[pos++] = 'v';
		ఎంపిక (xlen) {
		స్థితి 1:
			బహిర్గతం[pos++] = '3';
			బహిర్గతం[pos++] = '2';
			ఆపు;
		స్థితి 2:
			బహిర్గతం[pos++] = '6';
			బహిర్గతం[pos++] = '4';
			ఆపు;
		స్థితి 3:
			బహిర్గతం[pos++] = '1';
			బహిర్గతం[pos++] = '2';
			బహిర్గతం[pos++] = '8';
			ఆపు;
		అప్రమేయ:
			sbi_panic("%s: Unknown misa.MXL encoding %d",
				   __func__, xlen);
			తిరిగి;
		}
	}

	కొరకు (i = 0; i < array_కొలత(valid_isa_క్రమం) && (pos < బహిర్గతం_sz); i++) {
		ఒకవేళ (misa_extension_imp(valid_isa_క్రమం[i]))
			బహిర్గతం[pos++] = valid_isa_క్రమం[i];
	}

	ఒకవేళ (pos < బహిర్గతం_sz)
		బహిర్గతం[pos++] = '\0';
}

గుర్తులేని దీర్ఘ csr_read_అంకె(పూర్ణాంకం csr_అంకె)
{
#నిర్వచించు switchcase_csr_read(__csr_num)			\
	స్థితి __csr_num:					\
		తిరిగి csr_read(__csr_num);
#నిర్వచించు switchcase_csr_read_2(__csr_num)		\
	switchcase_csr_read(__csr_num + 0)		\
	switchcase_csr_read(__csr_num + 1)
#నిర్వచించు switchcase_csr_read_4(__csr_num)		\
	switchcase_csr_read_2(__csr_num + 0)		\
	switchcase_csr_read_2(__csr_num + 2)
#నిర్వచించు switchcase_csr_read_8(__csr_num)		\
	switchcase_csr_read_4(__csr_num + 0)		\
	switchcase_csr_read_4(__csr_num + 4)
#నిర్వచించు switchcase_csr_read_16(__csr_num)		\
	switchcase_csr_read_8(__csr_num + 0)		\
	switchcase_csr_read_8(__csr_num + 8)
#నిర్వచించు switchcase_csr_read_32(__csr_num)		\
	switchcase_csr_read_16(__csr_num + 0)		\
	switchcase_csr_read_16(__csr_num + 16)
#నిర్వచించు switchcase_csr_read_64(__csr_num)		\
	switchcase_csr_read_32(__csr_num + 0)		\
	switchcase_csr_read_32(__csr_num + 32)
#నిర్వచించు switchcase_csr_read_128(__csr_num)		\
	switchcase_csr_read_64(__csr_num + 0)		\
	switchcase_csr_read_64(__csr_num + 64)
#నిర్వచించు switchcase_csr_read_256(__csr_num)		\
	switchcase_csr_read_128(__csr_num + 0)		\
	switchcase_csr_read_128(__csr_num + 128)

	ఎంపిక (csr_అంకె) {
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
#ఒకవేళ __riscv_xlen == 32
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
#అయితే_ముగింపు
	switchcase_csr_read_256(CSR_CUSTOM0_U_RW_పునాది)
	switchcase_csr_read_64(CSR_CUSTOM1_U_RO_పునాది)
	switchcase_csr_read_64(CSR_CUSTOM2_S_RW_పునాది)
	switchcase_csr_read_64(CSR_CUSTOM3_S_RW_పునాది)
	switchcase_csr_read_64(CSR_CUSTOM4_S_RO_పునాది)
	switchcase_csr_read_64(CSR_CUSTOM5_HS_RW_పునాది)
	switchcase_csr_read_64(CSR_CUSTOM6_HS_RW_పునాది)
	switchcase_csr_read_64(CSR_CUSTOM7_HS_RO_పునాది)
	switchcase_csr_read_64(CSR_CUSTOM8_M_RW_పునాది)
	switchcase_csr_read_64(CSR_CUSTOM9_M_RW_పునాది)
	switchcase_csr_read_64(CSR_CUSTOM10_M_RO_పునాది)

	అప్రమేయ:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_అంకె);
		తిరిగి 0;
	}

#నిర్వచనం_రద్దు switchcase_csr_read_256
#నిర్వచనం_రద్దు switchcase_csr_read_128
#నిర్వచనం_రద్దు switchcase_csr_read_64
#నిర్వచనం_రద్దు switchcase_csr_read_32
#నిర్వచనం_రద్దు switchcase_csr_read_16
#నిర్వచనం_రద్దు switchcase_csr_read_8
#నిర్వచనం_రద్దు switchcase_csr_read_4
#నిర్వచనం_రద్దు switchcase_csr_read_2
#నిర్వచనం_రద్దు switchcase_csr_read
}

శూన్య csr_write_అంకె(పూర్ణాంకం csr_అంకె, గుర్తులేని దీర్ఘ val)
{
#నిర్వచించు switchcase_csr_write(__csr_num, __val)		\
	స్థితి __csr_num:					\
		csr_write(__csr_num, __val);		\
		ఆపు;
#నిర్వచించు switchcase_csr_write_2(__csr_num, __val)	\
	switchcase_csr_write(__csr_num + 0, __val)	\
	switchcase_csr_write(__csr_num + 1, __val)
#నిర్వచించు switchcase_csr_write_4(__csr_num, __val)	\
	switchcase_csr_write_2(__csr_num + 0, __val)	\
	switchcase_csr_write_2(__csr_num + 2, __val)
#నిర్వచించు switchcase_csr_write_8(__csr_num, __val)	\
	switchcase_csr_write_4(__csr_num + 0, __val)	\
	switchcase_csr_write_4(__csr_num + 4, __val)
#నిర్వచించు switchcase_csr_write_16(__csr_num, __val)	\
	switchcase_csr_write_8(__csr_num + 0, __val)	\
	switchcase_csr_write_8(__csr_num + 8, __val)
#నిర్వచించు switchcase_csr_write_32(__csr_num, __val)	\
	switchcase_csr_write_16(__csr_num + 0, __val)	\
	switchcase_csr_write_16(__csr_num + 16, __val)
#నిర్వచించు switchcase_csr_write_64(__csr_num, __val)	\
	switchcase_csr_write_32(__csr_num + 0, __val)	\
	switchcase_csr_write_32(__csr_num + 32, __val)
#నిర్వచించు switchcase_csr_write_128(__csr_num, __val)	\
	switchcase_csr_write_64(__csr_num + 0, __val)	\
	switchcase_csr_write_64(__csr_num + 64, __val)
#నిర్వచించు switchcase_csr_write_256(__csr_num, __val)	\
	switchcase_csr_write_128(__csr_num + 0, __val)	\
	switchcase_csr_write_128(__csr_num + 128, __val)

	ఎంపిక (csr_అంకె) {
	switchcase_csr_write_16(CSR_PMPCFG0, val)
	switchcase_csr_write_64(CSR_PMPADDR0, val)
	switchcase_csr_write(CSR_MCYCLE, val)
	switchcase_csr_write(CSR_MINSTRET, val)
	switchcase_csr_write(CSR_MHPMCOUNTER3, val)
	switchcase_csr_write_4(CSR_MHPMCOUNTER4, val)
	switchcase_csr_write_8(CSR_MHPMCOUNTER8, val)
	switchcase_csr_write_16(CSR_MHPMCOUNTER16, val)
#ఒకవేళ __riscv_xlen == 32
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
#అయితే_ముగింపు
	switchcase_csr_write(CSR_MCOUNTINHIBIT, val)
	switchcase_csr_write(CSR_MCYCLECFG, val)
	switchcase_csr_write(CSR_MINSTRETCFG, val)
	switchcase_csr_write(CSR_MHPMEVENT3, val)
	switchcase_csr_write_4(CSR_MHPMEVENT4, val)
	switchcase_csr_write_8(CSR_MHPMEVENT8, val)
	switchcase_csr_write_16(CSR_MHPMEVENT16, val)
	switchcase_csr_write_256(CSR_CUSTOM0_U_RW_పునాది, val)
	switchcase_csr_write_64(CSR_CUSTOM2_S_RW_పునాది, val)
	switchcase_csr_write_64(CSR_CUSTOM3_S_RW_పునాది, val)
	switchcase_csr_write_64(CSR_CUSTOM5_HS_RW_పునాది, val)
	switchcase_csr_write_64(CSR_CUSTOM6_HS_RW_పునాది, val)
	switchcase_csr_write_64(CSR_CUSTOM8_M_RW_పునాది, val)
	switchcase_csr_write_64(CSR_CUSTOM9_M_RW_పునాది, val)

	అప్రమేయ:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_అంకె);
		ఆపు;
	}

#నిర్వచనం_రద్దు switchcase_csr_write_256
#నిర్వచనం_రద్దు switchcase_csr_write_128
#నిర్వచనం_రద్దు switchcase_csr_write_64
#నిర్వచనం_రద్దు switchcase_csr_write_32
#నిర్వచనం_రద్దు switchcase_csr_write_16
#నిర్వచనం_రద్దు switchcase_csr_write_8
#నిర్వచనం_రద్దు switchcase_csr_write_4
#నిర్వచనం_రద్దు switchcase_csr_write_2
#నిర్వచనం_రద్దు switchcase_csr_write
}
