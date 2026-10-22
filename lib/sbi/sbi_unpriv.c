/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_encoding.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_unpriv.h>

اتحاد sbi_unpriv_ڈیٹا {
	u8 b;
	u16 h;
	u32 w;
#اگر __riscv_xlen == 64
	u64 d;
#اگر_ختم
	u8 بائٹیں[__riscv_xlen / 8];
};

/**
 * a3 must a pointer to the sbi_trap_info and a4 is used as a temporary
 * register in the trap handler. Make sure that compiler doesn't use a3 & a4.
 */
#تعریف_کرو DEFINE_UNPRIVILEGED_بھرو_FUNCTION(type, insn)                         \
	type sbi_بھرو##type(ثابت type *addr,                                \
			     ساخت sbi_trap_info *trap)                      \
	{                                                                     \
		اندراج ulong tinfo asm("a3") = (ulong)trap;                 \
		اندراج ulong mstatus = 0;                                   \
		اندراج ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		type ret = 0;                                                 \
		trap->cause = 0;                                              \
		asm متلون(                                                 \
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
		واپس ret;                                                   \
	}

#تعریف_کرو DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                        \
	خالی sbi_store_##type(type *addr, type val,                           \
			      ساخت sbi_trap_info *trap)                     \
	{                                                                     \
		اندراج ulong tinfo asm("a3") = (ulong)trap;                 \
		اندراج ulong mstatus = 0;                                   \
		اندراج ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		trap->cause = 0;                                              \
		asm متلون(                                                 \
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

DEFINE_UNPRIVILEGED_بھرو_FUNCTION(u8, lbu)
DEFINE_UNPRIVILEGED_بھرو_FUNCTION(u16, lhu)
DEFINE_UNPRIVILEGED_بھرو_FUNCTION(s8, lb)
DEFINE_UNPRIVILEGED_بھرو_FUNCTION(s16, lh)
DEFINE_UNPRIVILEGED_بھرو_FUNCTION(s32, lw)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u8, sb)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u16, sh)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u32, sw)
#اگر __riscv_xlen == 64
DEFINE_UNPRIVILEGED_بھرو_FUNCTION(u32, lwu)
DEFINE_UNPRIVILEGED_بھرو_FUNCTION(u64, ld)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, sd)
DEFINE_UNPRIVILEGED_بھرو_FUNCTION(ulong, ld)
#وگرنہ_اگر __riscv_xlen == 32
DEFINE_UNPRIVILEGED_بھرو_FUNCTION(u32, lw)
DEFINE_UNPRIVILEGED_بھرو_FUNCTION(ulong, lw)

u64 sbi_بھرو_u64(ثابت u64 *addr,
		 ساخت sbi_trap_info *trap)
{
	u64 ret = sbi_بھرو_u32((u32 *)addr, trap);

	اگر (trap->cause)
		واپس 0;
	ret |= ((u64)sbi_بھرو_u32((u32 *)addr + 1, trap) << 32);
	اگر (trap->cause)
		واپس 0;

	واپس ret;
}

خالی sbi_store_u64(u64 *addr, u64 val,
		   ساخت sbi_trap_info *trap)
{
	sbi_store_u32((u32 *)addr, val, trap);
	اگر (trap->cause)
		واپس;

	sbi_store_u32((u32 *)addr + 1, val >> 32, trap);
	اگر (trap->cause)
		واپس;
}
#ورنہ
# error "Unexpected __riscv_xlen"
#اگر_ختم

خالی sbi_بھرو_loop(u8 *بفر, ulong addr, ulong len,
		   ساخت sbi_trap_info *trap)
{
	اتحاد sbi_unpriv_ڈیٹا ڈیٹا;

	trap->cause = 0;
	جب (len) {
		غیرعلامتی صحیح چوڑائی = __riscv_xlen / 8;
		خالی *ptr = (خالی*)addr;

		جب (len < چوڑائی || (addr & (چوڑائی - 1)))
			چوڑائی /= 2;

		انتخاب (چوڑائی) {
		حالت 1:
			ڈیٹا.b = sbi_بھرو_u8(ptr, trap);
			توڑو;
		حالت 2:
			ڈیٹا.h = sbi_بھرو_u16(ptr, trap);
			توڑو;
		حالت 4:
			ڈیٹا.w = sbi_بھرو_u32(ptr, trap);
			توڑو;
#اگر __riscv_xlen == 64
		حالت 8:
			ڈیٹا.d = sbi_بھرو_u64(ptr, trap);
			توڑو;
#اگر_ختم
		}
		اگر (trap->cause)
			واپس;

		sbi_memcpy(بفر, ڈیٹا.بائٹیں, چوڑائی);
		len -= چوڑائی;
		addr += چوڑائی;
		بفر += چوڑائی;
	}
}

خالی sbi_store_loop(u8 *بفر, ulong addr, ulong len,
		    ساخت sbi_trap_info *trap)
{
	اتحاد sbi_unpriv_ڈیٹا ڈیٹا;

	trap->cause = 0;
	جب (len) {
		غیرعلامتی صحیح چوڑائی = __riscv_xlen / 8;
		خالی *ptr = (خالی*)addr;

		جب (len < چوڑائی || (addr & (چوڑائی - 1)))
			چوڑائی /= 2;

		sbi_memcpy(ڈیٹا.بائٹیں, بفر, چوڑائی);
		انتخاب (چوڑائی) {
		حالت 1:
			sbi_store_u8(ptr, ڈیٹا.b, trap);
			توڑو;
		حالت 2:
			sbi_store_u16(ptr, ڈیٹا.h, trap);
			توڑو;
		حالت 4:
			sbi_store_u32(ptr, ڈیٹا.w, trap);
			توڑو;
#اگر __riscv_xlen == 64
		حالت 8:
			sbi_store_u64(ptr, ڈیٹا.d, trap);
			توڑو;
#اگر_ختم
		}
		اگر (trap->cause)
			واپس;

		len -= چوڑائی;
		addr += چوڑائی;
		بفر += چوڑائی;
	}
}

ulong sbi_get_insn(ulong mepc, ساخت sbi_trap_info *trap)
{
	اندراج ulong tinfo asm("a3");
	اندراج ulong ttmp asm("a4");
	اندراج ulong mstatus = 0;
	اندراج ulong mtvec = (ulong)sbi_hart_expected_trap;
	ulong insn = 0;

	trap->cause = 0;

	asm متلون(
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

	انتخاب (trap->cause) {
	حالت CAUSE_بھرو_ACCESS:
		trap->cause = CAUSE_FETCH_ACCESS;
		trap->tinst = 0UL;
		توڑو;
	حالت CAUSE_بھرو_صفحہ_FAULT:
		trap->cause = CAUSE_FETCH_صفحہ_FAULT;
		trap->tinst = 0UL;
		توڑو;
	حالت CAUSE_بھرو_GUEST_صفحہ_FAULT:
		trap->cause = CAUSE_FETCH_GUEST_صفحہ_FAULT;
		اگر (trap->tinst != INSN_PSEUDO_VS_بھرو &&
		    trap->tinst != INSN_PSEUDO_VS_STORE)
			trap->tinst = 0UL;
		توڑو;
	طےشدہ:
		توڑو;
	}

	واپس insn;
}
