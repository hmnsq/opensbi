/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_pmp.h>

/* determine CPU extension, return non-zero support */
पूर्णाङ्क misa_extension_imp(वर्ण ext)
{
	अचिह्नित दीर्घ misa = csr_read(CSR_MISA);

	यदि (misa) {
		यदि ('A' <= ext && ext <= 'Z')
			प्रत्यागम misa & (1 << (ext - 'A'));
		यदि ('a' <= ext && ext <= 'z')
			प्रत्यागम misa & (1 << (ext - 'a'));
		प्रत्यागम 0;
	}

	प्रत्यागम sbi_platform_misa_extension(sbi_platform_thishart_ptr(), ext);
}

पूर्णाङ्क misa_xlen(शून्य)
{
	दीर्घ r;

	यदि (csr_read(CSR_MISA) == 0)
		प्रत्यागम sbi_platform_misa_xlen(sbi_platform_thishart_ptr());

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

	प्रत्यागम r ? r : -1;
}

शून्य misa_string(पूर्णाङ्क xlen, वर्ण *बहिर्गमनम्, अचिह्नित पूर्णाङ्क बहिर्गमनम्_sz)
{
	अचिह्नित पूर्णाङ्क i, pos = 0;
	ध्रुव वर्ण valid_isa_क्रमः[] = "iemafdqclbjtpvnhkorwxyzg";

	यदि (!बहिर्गमनम्)
		प्रत्यागम;

	यदि (5 <= (बहिर्गमनम्_sz - pos)) {
		बहिर्गमनम्[pos++] = 'r';
		बहिर्गमनम्[pos++] = 'v';
		विकल्प (xlen) {
		पक्ष 1:
			बहिर्गमनम्[pos++] = '3';
			बहिर्गमनम्[pos++] = '2';
			भङ्ग;
		पक्ष 2:
			बहिर्गमनम्[pos++] = '6';
			बहिर्गमनम्[pos++] = '4';
			भङ्ग;
		पक्ष 3:
			बहिर्गमनम्[pos++] = '1';
			बहिर्गमनम्[pos++] = '2';
			बहिर्गमनम्[pos++] = '8';
			भङ्ग;
		स्वभाव:
			sbi_panic("%s: Unknown misa.MXL encoding %d",
				   __func__, xlen);
			प्रत्यागम;
		}
	}

	प्रति (i = 0; i < array_आकार(valid_isa_क्रमः) && (pos < बहिर्गमनम्_sz); i++) {
		यदि (misa_extension_imp(valid_isa_क्रमः[i]))
			बहिर्गमनम्[pos++] = valid_isa_क्रमः[i];
	}

	यदि (pos < बहिर्गमनम्_sz)
		बहिर्गमनम्[pos++] = '\0';
}

अचिह्नित दीर्घ csr_read_अङ्कः(पूर्णाङ्क csr_अङ्कः)
{
#परिभाषय switchcase_csr_read(__csr_num)			\
	पक्ष __csr_num:					\
		प्रत्यागम csr_read(__csr_num);
#परिभाषय switchcase_csr_read_2(__csr_num)		\
	switchcase_csr_read(__csr_num + 0)		\
	switchcase_csr_read(__csr_num + 1)
#परिभाषय switchcase_csr_read_4(__csr_num)		\
	switchcase_csr_read_2(__csr_num + 0)		\
	switchcase_csr_read_2(__csr_num + 2)
#परिभाषय switchcase_csr_read_8(__csr_num)		\
	switchcase_csr_read_4(__csr_num + 0)		\
	switchcase_csr_read_4(__csr_num + 4)
#परिभाषय switchcase_csr_read_16(__csr_num)		\
	switchcase_csr_read_8(__csr_num + 0)		\
	switchcase_csr_read_8(__csr_num + 8)
#परिभाषय switchcase_csr_read_32(__csr_num)		\
	switchcase_csr_read_16(__csr_num + 0)		\
	switchcase_csr_read_16(__csr_num + 16)
#परिभाषय switchcase_csr_read_64(__csr_num)		\
	switchcase_csr_read_32(__csr_num + 0)		\
	switchcase_csr_read_32(__csr_num + 32)
#परिभाषय switchcase_csr_read_128(__csr_num)		\
	switchcase_csr_read_64(__csr_num + 0)		\
	switchcase_csr_read_64(__csr_num + 64)
#परिभाषय switchcase_csr_read_256(__csr_num)		\
	switchcase_csr_read_128(__csr_num + 0)		\
	switchcase_csr_read_128(__csr_num + 128)

	विकल्प (csr_अङ्कः) {
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
#यदि __riscv_xlen == 32
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
#यदि_अन्तः
	switchcase_csr_read_256(CSR_CUSTOM0_U_RW_आधारः)
	switchcase_csr_read_64(CSR_CUSTOM1_U_RO_आधारः)
	switchcase_csr_read_64(CSR_CUSTOM2_S_RW_आधारः)
	switchcase_csr_read_64(CSR_CUSTOM3_S_RW_आधारः)
	switchcase_csr_read_64(CSR_CUSTOM4_S_RO_आधारः)
	switchcase_csr_read_64(CSR_CUSTOM5_HS_RW_आधारः)
	switchcase_csr_read_64(CSR_CUSTOM6_HS_RW_आधारः)
	switchcase_csr_read_64(CSR_CUSTOM7_HS_RO_आधारः)
	switchcase_csr_read_64(CSR_CUSTOM8_M_RW_आधारः)
	switchcase_csr_read_64(CSR_CUSTOM9_M_RW_आधारः)
	switchcase_csr_read_64(CSR_CUSTOM10_M_RO_आधारः)

	स्वभाव:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_अङ्कः);
		प्रत्यागम 0;
	}

#अपरिभाषय switchcase_csr_read_256
#अपरिभाषय switchcase_csr_read_128
#अपरिभाषय switchcase_csr_read_64
#अपरिभाषय switchcase_csr_read_32
#अपरिभाषय switchcase_csr_read_16
#अपरिभाषय switchcase_csr_read_8
#अपरिभाषय switchcase_csr_read_4
#अपरिभाषय switchcase_csr_read_2
#अपरिभाषय switchcase_csr_read
}

शून्य csr_write_अङ्कः(पूर्णाङ्क csr_अङ्कः, अचिह्नित दीर्घ val)
{
#परिभाषय switchcase_csr_write(__csr_num, __val)		\
	पक्ष __csr_num:					\
		csr_write(__csr_num, __val);		\
		भङ्ग;
#परिभाषय switchcase_csr_write_2(__csr_num, __val)	\
	switchcase_csr_write(__csr_num + 0, __val)	\
	switchcase_csr_write(__csr_num + 1, __val)
#परिभाषय switchcase_csr_write_4(__csr_num, __val)	\
	switchcase_csr_write_2(__csr_num + 0, __val)	\
	switchcase_csr_write_2(__csr_num + 2, __val)
#परिभाषय switchcase_csr_write_8(__csr_num, __val)	\
	switchcase_csr_write_4(__csr_num + 0, __val)	\
	switchcase_csr_write_4(__csr_num + 4, __val)
#परिभाषय switchcase_csr_write_16(__csr_num, __val)	\
	switchcase_csr_write_8(__csr_num + 0, __val)	\
	switchcase_csr_write_8(__csr_num + 8, __val)
#परिभाषय switchcase_csr_write_32(__csr_num, __val)	\
	switchcase_csr_write_16(__csr_num + 0, __val)	\
	switchcase_csr_write_16(__csr_num + 16, __val)
#परिभाषय switchcase_csr_write_64(__csr_num, __val)	\
	switchcase_csr_write_32(__csr_num + 0, __val)	\
	switchcase_csr_write_32(__csr_num + 32, __val)
#परिभाषय switchcase_csr_write_128(__csr_num, __val)	\
	switchcase_csr_write_64(__csr_num + 0, __val)	\
	switchcase_csr_write_64(__csr_num + 64, __val)
#परिभाषय switchcase_csr_write_256(__csr_num, __val)	\
	switchcase_csr_write_128(__csr_num + 0, __val)	\
	switchcase_csr_write_128(__csr_num + 128, __val)

	विकल्प (csr_अङ्कः) {
	switchcase_csr_write_16(CSR_PMPCFG0, val)
	switchcase_csr_write_64(CSR_PMPADDR0, val)
	switchcase_csr_write(CSR_MCYCLE, val)
	switchcase_csr_write(CSR_MINSTRET, val)
	switchcase_csr_write(CSR_MHPMCOUNTER3, val)
	switchcase_csr_write_4(CSR_MHPMCOUNTER4, val)
	switchcase_csr_write_8(CSR_MHPMCOUNTER8, val)
	switchcase_csr_write_16(CSR_MHPMCOUNTER16, val)
#यदि __riscv_xlen == 32
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
#यदि_अन्तः
	switchcase_csr_write(CSR_MCOUNTINHIBIT, val)
	switchcase_csr_write(CSR_MCYCLECFG, val)
	switchcase_csr_write(CSR_MINSTRETCFG, val)
	switchcase_csr_write(CSR_MHPMEVENT3, val)
	switchcase_csr_write_4(CSR_MHPMEVENT4, val)
	switchcase_csr_write_8(CSR_MHPMEVENT8, val)
	switchcase_csr_write_16(CSR_MHPMEVENT16, val)
	switchcase_csr_write_256(CSR_CUSTOM0_U_RW_आधारः, val)
	switchcase_csr_write_64(CSR_CUSTOM2_S_RW_आधारः, val)
	switchcase_csr_write_64(CSR_CUSTOM3_S_RW_आधारः, val)
	switchcase_csr_write_64(CSR_CUSTOM5_HS_RW_आधारः, val)
	switchcase_csr_write_64(CSR_CUSTOM6_HS_RW_आधारः, val)
	switchcase_csr_write_64(CSR_CUSTOM8_M_RW_आधारः, val)
	switchcase_csr_write_64(CSR_CUSTOM9_M_RW_आधारः, val)

	स्वभाव:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_अङ्कः);
		भङ्ग;
	}

#अपरिभाषय switchcase_csr_write_256
#अपरिभाषय switchcase_csr_write_128
#अपरिभाषय switchcase_csr_write_64
#अपरिभाषय switchcase_csr_write_32
#अपरिभाषय switchcase_csr_write_16
#अपरिभाषय switchcase_csr_write_8
#अपरिभाषय switchcase_csr_write_4
#अपरिभाषय switchcase_csr_write_2
#अपरिभाषय switchcase_csr_write
}
