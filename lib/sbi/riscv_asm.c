/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_encoding.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_pmp.h>

/* determine CPU extension, return non-zero support */
पूर्णांक misa_extension_imp(अक्षर ext)
{
	अचिन्हित दीर्घ misa = csr_read(CSR_MISA);

	अगर (misa) {
		अगर ('A' <= ext && ext <= 'Z')
			वापस misa & (1 << (ext - 'A'));
		अगर ('a' <= ext && ext <= 'z')
			वापस misa & (1 << (ext - 'a'));
		वापस 0;
	}

	वापस sbi_platform_misa_extension(sbi_platform_thishart_ptr(), ext);
}

पूर्णांक misa_xlen(व्योम)
{
	दीर्घ r;

	अगर (csr_read(CSR_MISA) == 0)
		वापस sbi_platform_misa_xlen(sbi_platform_thishart_ptr());

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

	वापस r ? r : -1;
}

व्योम misa_string(पूर्णांक xlen, अक्षर *out, अचिन्हित पूर्णांक out_sz)
{
	अचिन्हित पूर्णांक i, pos = 0;
	स्थिर अक्षर valid_isa_order[] = "iemafdqclbjtpvnhkorwxyzg";

	अगर (!out)
		वापस;

	अगर (5 <= (out_sz - pos)) {
		out[pos++] = 'r';
		out[pos++] = 'v';
		चयन (xlen) {
		हाल 1:
			out[pos++] = '3';
			out[pos++] = '2';
			अवरोध;
		हाल 2:
			out[pos++] = '6';
			out[pos++] = '4';
			अवरोध;
		हाल 3:
			out[pos++] = '1';
			out[pos++] = '2';
			out[pos++] = '8';
			अवरोध;
		शेष:
			sbi_panic("%s: Unknown misa.MXL encoding %d",
				   __func__, xlen);
			वापस;
		}
	}

	क्रम (i = 0; i < array_आकार(valid_isa_order) && (pos < out_sz); i++) {
		अगर (misa_extension_imp(valid_isa_order[i]))
			out[pos++] = valid_isa_order[i];
	}

	अगर (pos < out_sz)
		out[pos++] = '\0';
}

अचिन्हित दीर्घ csr_read_अंक(पूर्णांक csr_अंक)
{
#घोषणा switchcase_csr_read(__csr_num)			\
	हाल __csr_num:					\
		वापस csr_read(__csr_num);
#घोषणा switchcase_csr_read_2(__csr_num)		\
	switchcase_csr_read(__csr_num + 0)		\
	switchcase_csr_read(__csr_num + 1)
#घोषणा switchcase_csr_read_4(__csr_num)		\
	switchcase_csr_read_2(__csr_num + 0)		\
	switchcase_csr_read_2(__csr_num + 2)
#घोषणा switchcase_csr_read_8(__csr_num)		\
	switchcase_csr_read_4(__csr_num + 0)		\
	switchcase_csr_read_4(__csr_num + 4)
#घोषणा switchcase_csr_read_16(__csr_num)		\
	switchcase_csr_read_8(__csr_num + 0)		\
	switchcase_csr_read_8(__csr_num + 8)
#घोषणा switchcase_csr_read_32(__csr_num)		\
	switchcase_csr_read_16(__csr_num + 0)		\
	switchcase_csr_read_16(__csr_num + 16)
#घोषणा switchcase_csr_read_64(__csr_num)		\
	switchcase_csr_read_32(__csr_num + 0)		\
	switchcase_csr_read_32(__csr_num + 32)
#घोषणा switchcase_csr_read_128(__csr_num)		\
	switchcase_csr_read_64(__csr_num + 0)		\
	switchcase_csr_read_64(__csr_num + 64)
#घोषणा switchcase_csr_read_256(__csr_num)		\
	switchcase_csr_read_128(__csr_num + 0)		\
	switchcase_csr_read_128(__csr_num + 128)

	चयन (csr_अंक) {
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
#अगर __riscv_xlen == 32
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
#पूर्ण_अगर
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

	शेष:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_अंक);
		वापस 0;
	}

#अघोषित switchcase_csr_read_256
#अघोषित switchcase_csr_read_128
#अघोषित switchcase_csr_read_64
#अघोषित switchcase_csr_read_32
#अघोषित switchcase_csr_read_16
#अघोषित switchcase_csr_read_8
#अघोषित switchcase_csr_read_4
#अघोषित switchcase_csr_read_2
#अघोषित switchcase_csr_read
}

व्योम csr_write_अंक(पूर्णांक csr_अंक, अचिन्हित दीर्घ val)
{
#घोषणा switchcase_csr_write(__csr_num, __val)		\
	हाल __csr_num:					\
		csr_write(__csr_num, __val);		\
		अवरोध;
#घोषणा switchcase_csr_write_2(__csr_num, __val)	\
	switchcase_csr_write(__csr_num + 0, __val)	\
	switchcase_csr_write(__csr_num + 1, __val)
#घोषणा switchcase_csr_write_4(__csr_num, __val)	\
	switchcase_csr_write_2(__csr_num + 0, __val)	\
	switchcase_csr_write_2(__csr_num + 2, __val)
#घोषणा switchcase_csr_write_8(__csr_num, __val)	\
	switchcase_csr_write_4(__csr_num + 0, __val)	\
	switchcase_csr_write_4(__csr_num + 4, __val)
#घोषणा switchcase_csr_write_16(__csr_num, __val)	\
	switchcase_csr_write_8(__csr_num + 0, __val)	\
	switchcase_csr_write_8(__csr_num + 8, __val)
#घोषणा switchcase_csr_write_32(__csr_num, __val)	\
	switchcase_csr_write_16(__csr_num + 0, __val)	\
	switchcase_csr_write_16(__csr_num + 16, __val)
#घोषणा switchcase_csr_write_64(__csr_num, __val)	\
	switchcase_csr_write_32(__csr_num + 0, __val)	\
	switchcase_csr_write_32(__csr_num + 32, __val)
#घोषणा switchcase_csr_write_128(__csr_num, __val)	\
	switchcase_csr_write_64(__csr_num + 0, __val)	\
	switchcase_csr_write_64(__csr_num + 64, __val)
#घोषणा switchcase_csr_write_256(__csr_num, __val)	\
	switchcase_csr_write_128(__csr_num + 0, __val)	\
	switchcase_csr_write_128(__csr_num + 128, __val)

	चयन (csr_अंक) {
	switchcase_csr_write_16(CSR_PMPCFG0, val)
	switchcase_csr_write_64(CSR_PMPADDR0, val)
	switchcase_csr_write(CSR_MCYCLE, val)
	switchcase_csr_write(CSR_MINSTRET, val)
	switchcase_csr_write(CSR_MHPMCOUNTER3, val)
	switchcase_csr_write_4(CSR_MHPMCOUNTER4, val)
	switchcase_csr_write_8(CSR_MHPMCOUNTER8, val)
	switchcase_csr_write_16(CSR_MHPMCOUNTER16, val)
#अगर __riscv_xlen == 32
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
#पूर्ण_अगर
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

	शेष:
		sbi_panic("%s: Unknown CSR %#x", __func__, csr_अंक);
		अवरोध;
	}

#अघोषित switchcase_csr_write_256
#अघोषित switchcase_csr_write_128
#अघोषित switchcase_csr_write_64
#अघोषित switchcase_csr_write_32
#अघोषित switchcase_csr_write_16
#अघोषित switchcase_csr_write_8
#अघोषित switchcase_csr_write_4
#अघोषित switchcase_csr_write_2
#अघोषित switchcase_csr_write
}
