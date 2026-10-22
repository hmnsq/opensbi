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
	अचिन्हित दीर्घ misa = csr_read(CSR_MISA);

	यदि (misa) {
		यदि ('A' <= ext && ext <= 'Z')
			फर्काऊ misa & (1 << (ext - 'A'));
		यदि ('a' <= ext && ext <= 'z')
			फर्काऊ misa & (1 << (ext - 'a'));
		फर्काऊ 0;
	}

	फर्काऊ sbi_platform_misa_extension(sbi_platform_thishart_ptr(), ext);
}

पूर्णाङ्क misa_xlen(शून्य)
{
	दीर्घ r;

	यदि (csr_read(CSR_MISA) == 0)
		फर्काऊ sbi_platform_misa_xlen(sbi_platform_thishart_ptr());

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

	फर्काऊ r ? r : -1;
}

शून्य misa_string(पूर्णाङ्क xlen, वर्ण *out, अचिन्हित पूर्णाङ्क out_sz)
{
	अचिन्हित पूर्णाङ्क i, pos = 0;
	अचर वर्ण valid_isa_order[] = "iemafdqclbjtpvnhkorwxyzg";

	यदि (!out)
		फर्काऊ;

	यदि (5 <= (out_sz - pos)) {
		out[pos++] = 'r';
		out[pos++] = 'v';
		विकल्प (xlen) {
		पक्ष 1:
			out[pos++] = '3';
			out[pos++] = '2';
			रोक;
		पक्ष 2:
			out[pos++] = '6';
			out[pos++] = '4';
			रोक;
		पक्ष 3:
			out[pos++] = '1';
			out[pos++] = '2';
			out[pos++] = '8';
			रोक;
		स्वभाव:
			sbi_panic("%s: Unknown misa.MXL encoding %d",
				   __func__, xlen);
			फर्काऊ;
		}
	}

	लागि (i = 0; i < array_size(valid_isa_order) && (pos < out_sz); i++) {
		यदि (misa_extension_imp(valid_isa_order[i]))
			out[pos++] = valid_isa_order[i];
	}

	यदि (pos < out_sz)
		out[pos++] = '\0';
}

अचिन्हित दीर्घ csr_read_अङ्क(पूर्णाङ्क csr_अङ्क)
{
#define switchcase_csr_read(__csr_num)			\
	पक्ष __csr_num:					\
		फर्काऊ csr_read(__csr_num);
#define switchcase_csr_read_2(__csr_num)		\
	switchcase_csr_read(__csr_num + 0)		\
	switchcase_csr_read(__csr_num + 1)
#define switchcase_csr_read_4(__csr_num)		\
	switchcase_csr_read_2(__csr_num + 0)		\
	switchcase_csr_read_2(__csr_num + 2)
#define switchcase_csr_read_8(__csr_num)		\
	switchcase_csr_read_4(__csr_num + 0)		\
	switchcase_csr_read_4(__csr_num + 4)
#define switchcase_csr_read_16(__csr_num)		\
	switchcase_csr_read_8(__csr_num + 0)		\
	switchcase_csr_read_8(__csr_num + 8)
#define switchcase_csr_read_32(__csr_num)		\
	switchcase_csr_read_16(__csr_num + 0)		\
	switchcase_csr_read_16(__csr_num + 16)
#define switchcase_csr_read_64(__csr_num)		\
	switchcase_csr_read_32(__csr_num + 0)		\
	switchcase_csr_read_32(__csr_num + 32)
#define switchcase_csr_read_128(__csr_num)		\
	switchcase_csr_read_64(__csr_num + 0)		\
	switchcase_csr_read_64(__csr_num + 64)
#define switchcase_csr_read_256(__csr_num)		\
	switchcase_csr_read_128(__csr_num + 0)		\
	switchcase_csr_read_128(__csr_num + 128)

	विकल्प (csr_अङ्क) {
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
#endif
	switchcase_csr_read_256(CSR_CUSTOM0_U_RW_BASE)
	switchcase_csr_read_64(CSR_CUSTOM1_U_RO_BASE)
	switchcase_csr_read_64(CSR_CUSTOM2_S_RW_BASE)
	switchcase_csr_read_64(CSR_CUSTOM3_S_RW_BASE)
	switchcase_csr_read_64(CSR_CUSTOM4_S_RO_BASE)
	switchcase_csr_read_64(CSR_CUSTOM5_HS_RW_BASE)
	switchcase_csr_read_64(CSR_CUSTOM6_HS_RW_BASE)
	switchcase_csr_read_64(CSR_CUSTOM7_HS_RO_BASE)
	switchcase_csr_read_64(CSR_CUSTOM8_M_RW_BASE)
	switchcase_csr_read_64(CSR_CUSTOM9_M_RW_BASE)
	switchcase_csr_read_64(CSR_CUSTOM10_M_RO_BASE)

	स्वभाव:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_अङ्क);
		फर्काऊ 0;
	}

#undef switchcase_csr_read_256
#undef switchcase_csr_read_128
#undef switchcase_csr_read_64
#undef switchcase_csr_read_32
#undef switchcase_csr_read_16
#undef switchcase_csr_read_8
#undef switchcase_csr_read_4
#undef switchcase_csr_read_2
#undef switchcase_csr_read
}

शून्य csr_write_अङ्क(पूर्णाङ्क csr_अङ्क, अचिन्हित दीर्घ val)
{
#define switchcase_csr_write(__csr_num, __val)		\
	पक्ष __csr_num:					\
		csr_write(__csr_num, __val);		\
		रोक;
#define switchcase_csr_write_2(__csr_num, __val)	\
	switchcase_csr_write(__csr_num + 0, __val)	\
	switchcase_csr_write(__csr_num + 1, __val)
#define switchcase_csr_write_4(__csr_num, __val)	\
	switchcase_csr_write_2(__csr_num + 0, __val)	\
	switchcase_csr_write_2(__csr_num + 2, __val)
#define switchcase_csr_write_8(__csr_num, __val)	\
	switchcase_csr_write_4(__csr_num + 0, __val)	\
	switchcase_csr_write_4(__csr_num + 4, __val)
#define switchcase_csr_write_16(__csr_num, __val)	\
	switchcase_csr_write_8(__csr_num + 0, __val)	\
	switchcase_csr_write_8(__csr_num + 8, __val)
#define switchcase_csr_write_32(__csr_num, __val)	\
	switchcase_csr_write_16(__csr_num + 0, __val)	\
	switchcase_csr_write_16(__csr_num + 16, __val)
#define switchcase_csr_write_64(__csr_num, __val)	\
	switchcase_csr_write_32(__csr_num + 0, __val)	\
	switchcase_csr_write_32(__csr_num + 32, __val)
#define switchcase_csr_write_128(__csr_num, __val)	\
	switchcase_csr_write_64(__csr_num + 0, __val)	\
	switchcase_csr_write_64(__csr_num + 64, __val)
#define switchcase_csr_write_256(__csr_num, __val)	\
	switchcase_csr_write_128(__csr_num + 0, __val)	\
	switchcase_csr_write_128(__csr_num + 128, __val)

	विकल्प (csr_अङ्क) {
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
#endif
	switchcase_csr_write(CSR_MCOUNTINHIBIT, val)
	switchcase_csr_write(CSR_MCYCLECFG, val)
	switchcase_csr_write(CSR_MINSTRETCFG, val)
	switchcase_csr_write(CSR_MHPMEVENT3, val)
	switchcase_csr_write_4(CSR_MHPMEVENT4, val)
	switchcase_csr_write_8(CSR_MHPMEVENT8, val)
	switchcase_csr_write_16(CSR_MHPMEVENT16, val)
	switchcase_csr_write_256(CSR_CUSTOM0_U_RW_BASE, val)
	switchcase_csr_write_64(CSR_CUSTOM2_S_RW_BASE, val)
	switchcase_csr_write_64(CSR_CUSTOM3_S_RW_BASE, val)
	switchcase_csr_write_64(CSR_CUSTOM5_HS_RW_BASE, val)
	switchcase_csr_write_64(CSR_CUSTOM6_HS_RW_BASE, val)
	switchcase_csr_write_64(CSR_CUSTOM8_M_RW_BASE, val)
	switchcase_csr_write_64(CSR_CUSTOM9_M_RW_BASE, val)

	स्वभाव:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_अङ्क);
		रोक;
	}

#undef switchcase_csr_write_256
#undef switchcase_csr_write_128
#undef switchcase_csr_write_64
#undef switchcase_csr_write_32
#undef switchcase_csr_write_16
#undef switchcase_csr_write_8
#undef switchcase_csr_write_4
#undef switchcase_csr_write_2
#undef switchcase_csr_write
}
