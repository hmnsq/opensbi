/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_encoding.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_unpriv.h>

जोड़ sbi_unpriv_तथ्य {
	u8 b;
	u16 h;
	u32 w;
#अगर __riscv_xlen == 64
	u64 d;
#पूर्ण_अगर
	u8 bytes[__riscv_xlen / 8];
};

/**
 * a3 must a pointer to the sbi_trap_info and a4 is used as a temporary
 * register in the trap handler. Make sure that compiler doesn't use a3 & a4.
 */
#घोषणा DEFINE_UNPRIVILEGED_LOAD_FUNCTION(type, insn)                         \
	type sbi_load_##type(स्थिर type *addr,                                \
			     काष्ठा sbi_trap_info *trap)                      \
	{                                                                     \
		रेजिस्टर ulong tinfo यंत्र("a3") = (ulong)trap;                 \
		रेजिस्टर ulong mstatus = 0;                                   \
		रेजिस्टर ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		type ret = 0;                                                 \
		trap->cause = 0;                                              \
		यंत्र अस्थिर(                                                 \
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
		वापस ret;                                                   \
	}

#घोषणा DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                        \
	व्योम sbi_store_##type(type *addr, type val,                           \
			      काष्ठा sbi_trap_info *trap)                     \
	{                                                                     \
		रेजिस्टर ulong tinfo यंत्र("a3") = (ulong)trap;                 \
		रेजिस्टर ulong mstatus = 0;                                   \
		रेजिस्टर ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		trap->cause = 0;                                              \
		यंत्र अस्थिर(                                                 \
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

DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u8, lbu)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u16, lhu)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(s8, lb)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(s16, lh)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(s32, lw)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u8, sb)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u16, sh)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u32, sw)
#अगर __riscv_xlen == 64
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lwu)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u64, ld)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, sd)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, ld)
#या_अगर __riscv_xlen == 32
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lw)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, lw)

u64 sbi_load_u64(स्थिर u64 *addr,
		 काष्ठा sbi_trap_info *trap)
{
	u64 ret = sbi_load_u32((u32 *)addr, trap);

	अगर (trap->cause)
		वापस 0;
	ret |= ((u64)sbi_load_u32((u32 *)addr + 1, trap) << 32);
	अगर (trap->cause)
		वापस 0;

	वापस ret;
}

व्योम sbi_store_u64(u64 *addr, u64 val,
		   काष्ठा sbi_trap_info *trap)
{
	sbi_store_u32((u32 *)addr, val, trap);
	अगर (trap->cause)
		वापस;

	sbi_store_u32((u32 *)addr + 1, val >> 32, trap);
	अगर (trap->cause)
		वापस;
}
#अन्यथा
# error "Unexpected __riscv_xlen"
#पूर्ण_अगर

व्योम sbi_load_loop(u8 *बफर, ulong addr, ulong len,
		   काष्ठा sbi_trap_info *trap)
{
	जोड़ sbi_unpriv_तथ्य तथ्य;

	trap->cause = 0;
	जबतक (len) {
		अचिन्हित पूर्णांक width = __riscv_xlen / 8;
		व्योम *ptr = (व्योम*)addr;

		जबतक (len < width || (addr & (width - 1)))
			width /= 2;

		चयन (width) {
		हाल 1:
			तथ्य.b = sbi_load_u8(ptr, trap);
			अवरोध;
		हाल 2:
			तथ्य.h = sbi_load_u16(ptr, trap);
			अवरोध;
		हाल 4:
			तथ्य.w = sbi_load_u32(ptr, trap);
			अवरोध;
#अगर __riscv_xlen == 64
		हाल 8:
			तथ्य.d = sbi_load_u64(ptr, trap);
			अवरोध;
#पूर्ण_अगर
		}
		अगर (trap->cause)
			वापस;

		sbi_memcpy(बफर, तथ्य.bytes, width);
		len -= width;
		addr += width;
		बफर += width;
	}
}

व्योम sbi_store_loop(u8 *बफर, ulong addr, ulong len,
		    काष्ठा sbi_trap_info *trap)
{
	जोड़ sbi_unpriv_तथ्य तथ्य;

	trap->cause = 0;
	जबतक (len) {
		अचिन्हित पूर्णांक width = __riscv_xlen / 8;
		व्योम *ptr = (व्योम*)addr;

		जबतक (len < width || (addr & (width - 1)))
			width /= 2;

		sbi_memcpy(तथ्य.bytes, बफर, width);
		चयन (width) {
		हाल 1:
			sbi_store_u8(ptr, तथ्य.b, trap);
			अवरोध;
		हाल 2:
			sbi_store_u16(ptr, तथ्य.h, trap);
			अवरोध;
		हाल 4:
			sbi_store_u32(ptr, तथ्य.w, trap);
			अवरोध;
#अगर __riscv_xlen == 64
		हाल 8:
			sbi_store_u64(ptr, तथ्य.d, trap);
			अवरोध;
#पूर्ण_अगर
		}
		अगर (trap->cause)
			वापस;

		len -= width;
		addr += width;
		बफर += width;
	}
}

ulong sbi_get_insn(ulong mepc, काष्ठा sbi_trap_info *trap)
{
	रेजिस्टर ulong tinfo यंत्र("a3");
	रेजिस्टर ulong ttmp यंत्र("a4");
	रेजिस्टर ulong mstatus = 0;
	रेजिस्टर ulong mtvec = (ulong)sbi_hart_expected_trap;
	ulong insn = 0;

	trap->cause = 0;

	यंत्र अस्थिर(
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

	चयन (trap->cause) {
	हाल CAUSE_LOAD_ACCESS:
		trap->cause = CAUSE_FETCH_ACCESS;
		trap->tinst = 0UL;
		अवरोध;
	हाल CAUSE_LOAD_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_PAGE_FAULT;
		trap->tinst = 0UL;
		अवरोध;
	हाल CAUSE_LOAD_GUEST_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_GUEST_PAGE_FAULT;
		अगर (trap->tinst != INSN_PSEUDO_VS_LOAD &&
		    trap->tinst != INSN_PSEUDO_VS_STORE)
			trap->tinst = 0UL;
		अवरोध;
	शेष:
		अवरोध;
	}

	वापस insn;
}
