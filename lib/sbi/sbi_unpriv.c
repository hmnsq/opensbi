/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_unpriv.h>

सङ्घ sbi_unpriv_दत्त {
	u8 b;
	u16 h;
	u32 w;
#यदि __riscv_xlen == 64
	u64 d;
#यदि_अन्तः
	u8 अष्टकानि[__riscv_xlen / 8];
};

/**
 * a3 must a pointer to the sbi_trap_info and a4 is used as a temporary
 * register in the trap handler. Make sure that compiler doesn't use a3 & a4.
 */
#परिभाषय DEFINE_UNPRIVILEGED_आरोपय_FUNCTION(type, insn)                         \
	type sbi_आरोपय##type(ध्रुव type *addr,                                \
			     संरचना sbi_trap_info *trap)                      \
	{                                                                     \
		पञ्जिका ulong tinfo asm("a3") = (ulong)trap;                 \
		पञ्जिका ulong mstatus = 0;                                   \
		पञ्जिका ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		type ret = 0;                                                 \
		trap->cause = 0;                                              \
		asm चञ्चल(                                                 \
			"csrrw %[mtvec], " STR(CSR_MTVEC) ", %[mtvec]\n"      \
			"csrrs %[mstatus], " STR(CSR_MSTATUS) ", %[mprv]\n"   \
			".option push\n"                                      \
			".option norvc\n"                                     \
			#insn " %[ret], %[addr]\n"                            \
			".option pop\n"                                       \
			"csrw " STR(CSR_MSTATUS) ", %[mstatus]\n"             \
			"csrw " STR(CSR_MTVEC) ", %[mtvec]"                   \
		    : [mstatus] "+&r"(mstatus), [mtvec] "+&r"(mtvec),         \
		      [tinfo] "+&r"(tinfo), [ret] "=&r"(ret)                  \
		    : [addr] "m"(*addr), [mprv] "r"(MSTATUS_MPRV)             \
		    : "a4", "memory");                                        \
		प्रत्यागम ret;                                                   \
	}

#परिभाषय DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                        \
	शून्य sbi_store_##type(type *addr, type val,                           \
			      संरचना sbi_trap_info *trap)                     \
	{                                                                     \
		पञ्जिका ulong tinfo asm("a3") = (ulong)trap;                 \
		पञ्जिका ulong mstatus = 0;                                   \
		पञ्जिका ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		trap->cause = 0;                                              \
		asm चञ्चल(                                                 \
			"csrrw %[mtvec], " STR(CSR_MTVEC) ", %[mtvec]\n"      \
			"csrrs %[mstatus], " STR(CSR_MSTATUS) ", %[mprv]\n"   \
			".option push\n"                                      \
			".option norvc\n"                                     \
			#insn " %[val], %[addr]\n"                            \
			".option pop\n"                                       \
			"csrw " STR(CSR_MSTATUS) ", %[mstatus]\n"             \
			"csrw " STR(CSR_MTVEC) ", %[mtvec]"                   \
		    : [mstatus] "+&r"(mstatus), [mtvec] "+&r"(mtvec),         \
		      [tinfo] "+&r"(tinfo)                                    \
		    : [addr] "m"(*addr), [mprv] "r"(MSTATUS_MPRV),            \
		      [val] "r"(val)                                          \
		    : "a4", "memory");                                        \
	}

DEFINE_UNPRIVILEGED_आरोपय_FUNCTION(u8, lbu)
DEFINE_UNPRIVILEGED_आरोपय_FUNCTION(u16, lhu)
DEFINE_UNPRIVILEGED_आरोपय_FUNCTION(s8, lb)
DEFINE_UNPRIVILEGED_आरोपय_FUNCTION(s16, lh)
DEFINE_UNPRIVILEGED_आरोपय_FUNCTION(s32, lw)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u8, sb)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u16, sh)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u32, sw)
#यदि __riscv_xlen == 64
DEFINE_UNPRIVILEGED_आरोपय_FUNCTION(u32, lwu)
DEFINE_UNPRIVILEGED_आरोपय_FUNCTION(u64, ld)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, sd)
DEFINE_UNPRIVILEGED_आरोपय_FUNCTION(ulong, ld)
#नोचेद्यदि __riscv_xlen == 32
DEFINE_UNPRIVILEGED_आरोपय_FUNCTION(u32, lw)
DEFINE_UNPRIVILEGED_आरोपय_FUNCTION(ulong, lw)

u64 sbi_आरोपय_u64(ध्रुव u64 *addr,
		 संरचना sbi_trap_info *trap)
{
	u64 ret = sbi_आरोपय_u32((u32 *)addr, trap);

	यदि (trap->cause)
		प्रत्यागम 0;
	ret |= ((u64)sbi_आरोपय_u32((u32 *)addr + 1, trap) << 32);
	यदि (trap->cause)
		प्रत्यागम 0;

	प्रत्यागम ret;
}

शून्य sbi_store_u64(u64 *addr, u64 val,
		   संरचना sbi_trap_info *trap)
{
	sbi_store_u32((u32 *)addr, val, trap);
	यदि (trap->cause)
		प्रत्यागम;

	sbi_store_u32((u32 *)addr + 1, val >> 32, trap);
	यदि (trap->cause)
		प्रत्यागम;
}
#अन्यथा
# error "Unexpected __riscv_xlen"
#यदि_अन्तः

शून्य sbi_आरोपय_loop(u8 *बफर, ulong addr, ulong len,
		   संरचना sbi_trap_info *trap)
{
	सङ्घ sbi_unpriv_दत्त दत्त;

	trap->cause = 0;
	यावत् (len) {
		अचिह्नित पूर्णाङ्क विस्तारः = __riscv_xlen / 8;
		शून्य *ptr = (शून्य*)addr;

		यावत् (len < विस्तारः || (addr & (विस्तारः - 1)))
			विस्तारः /= 2;

		विकल्प (विस्तारः) {
		पक्ष 1:
			दत्त.b = sbi_आरोपय_u8(ptr, trap);
			भङ्ग;
		पक्ष 2:
			दत्त.h = sbi_आरोपय_u16(ptr, trap);
			भङ्ग;
		पक्ष 4:
			दत्त.w = sbi_आरोपय_u32(ptr, trap);
			भङ्ग;
#यदि __riscv_xlen == 64
		पक्ष 8:
			दत्त.d = sbi_आरोपय_u64(ptr, trap);
			भङ्ग;
#यदि_अन्तः
		}
		यदि (trap->cause)
			प्रत्यागम;

		sbi_memcpy(बफर, दत्त.अष्टकानि, विस्तारः);
		len -= विस्तारः;
		addr += विस्तारः;
		बफर += विस्तारः;
	}
}

शून्य sbi_store_loop(u8 *बफर, ulong addr, ulong len,
		    संरचना sbi_trap_info *trap)
{
	सङ्घ sbi_unpriv_दत्त दत्त;

	trap->cause = 0;
	यावत् (len) {
		अचिह्नित पूर्णाङ्क विस्तारः = __riscv_xlen / 8;
		शून्य *ptr = (शून्य*)addr;

		यावत् (len < विस्तारः || (addr & (विस्तारः - 1)))
			विस्तारः /= 2;

		sbi_memcpy(दत्त.अष्टकानि, बफर, विस्तारः);
		विकल्प (विस्तारः) {
		पक्ष 1:
			sbi_store_u8(ptr, दत्त.b, trap);
			भङ्ग;
		पक्ष 2:
			sbi_store_u16(ptr, दत्त.h, trap);
			भङ्ग;
		पक्ष 4:
			sbi_store_u32(ptr, दत्त.w, trap);
			भङ्ग;
#यदि __riscv_xlen == 64
		पक्ष 8:
			sbi_store_u64(ptr, दत्त.d, trap);
			भङ्ग;
#यदि_अन्तः
		}
		यदि (trap->cause)
			प्रत्यागम;

		len -= विस्तारः;
		addr += विस्तारः;
		बफर += विस्तारः;
	}
}

ulong sbi_get_insn(ulong mepc, संरचना sbi_trap_info *trap)
{
	पञ्जिका ulong tinfo asm("a3");
	पञ्जिका ulong ttmp asm("a4");
	पञ्जिका ulong mstatus = 0;
	पञ्जिका ulong mtvec = (ulong)sbi_hart_expected_trap;
	ulong insn = 0;

	trap->cause = 0;

	asm चञ्चल(
	    "add %[tinfo], %[taddr], zero\n"
	    "csrrw %[mtvec], " STR(CSR_MTVEC) ", %[mtvec]\n"
	    "csrrs %[mstatus], " STR(CSR_MSTATUS) ", %[mprv]\n"
	    "lhu %[insn], (%[addr])\n"
	    "andi %[ttmp], %[insn], 3\n"
	    "addi %[ttmp], %[ttmp], -3\n"
	    "bne %[ttmp], zero, 2f\n"
	    "lhu %[ttmp], 2(%[addr])\n"
	    "sll %[ttmp], %[ttmp], 16\n"
	    "add %[insn], %[insn], %[ttmp]\n"
	    "2: csrw " STR(CSR_MSTATUS) ", %[mstatus]\n"
	    "csrw " STR(CSR_MTVEC) ", %[mtvec]"
	    : [mstatus] "+&r"(mstatus), [mtvec] "+&r"(mtvec),
	      [tinfo] "+&r"(tinfo), [ttmp] "+&r"(ttmp),
	      [insn] "=&r"(insn)
	    : [mprv] "r"(MSTATUS_MPRV | MSTATUS_MXR),
	      [taddr] "r"((ulong)trap), [addr] "r"(mepc)
	    : "memory");

	विकल्प (trap->cause) {
	पक्ष CAUSE_आरोपय_ACCESS:
		trap->cause = CAUSE_FETCH_ACCESS;
		trap->tinst = 0UL;
		भङ्ग;
	पक्ष CAUSE_आरोपय_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_PAGE_FAULT;
		trap->tinst = 0UL;
		भङ्ग;
	पक्ष CAUSE_आरोपय_GUEST_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_GUEST_PAGE_FAULT;
		यदि (trap->tinst != INSN_PSEUDO_VS_आरोपय &&
		    trap->tinst != INSN_PSEUDO_VS_STORE)
			trap->tinst = 0UL;
		भङ्ग;
	स्वभाव:
		भङ्ग;
	}

	प्रत्यागम insn;
}
