/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unpriv.h>

ਸੰਘ sbi_unpriv_ਅੰਕੜੇ {
	u8 b;
	u16 h;
	u32 w;
#ਜੇ __riscv_xlen == 64
	u64 d;
#ਜੇ_ਅੰਤ
	u8 ਬਾਈਟਾਂ[__riscv_xlen / 8];
};

/**
 * a3 must a pointer to the sbi_trap_info and a4 is used as a temporary
 * register in the trap handler. Make sure that compiler doesn't use a3 & a4.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_UNPRIVILEGED_ਲੱਦੋ_FUNCTION(type, insn)                         \
	type sbi_ਲੱਦੋ##type(ਅਚਲ type *addr,                                \
			     ਬਣਤਰ sbi_trap_info *trap)                      \
	{                                                                     \
		ਰਜਿਸਟਰ ulong tinfo asm("a3") = (ulong)trap;                 \
		ਰਜਿਸਟਰ ulong mstatus = 0;                                   \
		ਰਜਿਸਟਰ ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		type ret = 0;                                                 \
		trap->cause = 0;                                              \
		asm ਅਸਥਿਰ(                                                 \
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
		ਵਾਪਸ ret;                                                   \
	}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                        \
	ਖਾਲੀ sbi_store_##type(type *addr, type val,                           \
			      ਬਣਤਰ sbi_trap_info *trap)                     \
	{                                                                     \
		ਰਜਿਸਟਰ ulong tinfo asm("a3") = (ulong)trap;                 \
		ਰਜਿਸਟਰ ulong mstatus = 0;                                   \
		ਰਜਿਸਟਰ ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		trap->cause = 0;                                              \
		asm ਅਸਥਿਰ(                                                 \
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

DEFINE_UNPRIVILEGED_ਲੱਦੋ_FUNCTION(u8, lbu)
DEFINE_UNPRIVILEGED_ਲੱਦੋ_FUNCTION(u16, lhu)
DEFINE_UNPRIVILEGED_ਲੱਦੋ_FUNCTION(s8, lb)
DEFINE_UNPRIVILEGED_ਲੱਦੋ_FUNCTION(s16, lh)
DEFINE_UNPRIVILEGED_ਲੱਦੋ_FUNCTION(s32, lw)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u8, sb)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u16, sh)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u32, sw)
#ਜੇ __riscv_xlen == 64
DEFINE_UNPRIVILEGED_ਲੱਦੋ_FUNCTION(u32, lwu)
DEFINE_UNPRIVILEGED_ਲੱਦੋ_FUNCTION(u64, ld)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, sd)
DEFINE_UNPRIVILEGED_ਲੱਦੋ_FUNCTION(ulong, ld)
#ਜਾਂਜੇ __riscv_xlen == 32
DEFINE_UNPRIVILEGED_ਲੱਦੋ_FUNCTION(u32, lw)
DEFINE_UNPRIVILEGED_ਲੱਦੋ_FUNCTION(ulong, lw)

u64 sbi_ਲੱਦੋ_u64(ਅਚਲ u64 *addr,
		 ਬਣਤਰ sbi_trap_info *trap)
{
	u64 ret = sbi_ਲੱਦੋ_u32((u32 *)addr, trap);

	ਜੇ (trap->cause)
		ਵਾਪਸ 0;
	ret |= ((u64)sbi_ਲੱਦੋ_u32((u32 *)addr + 1, trap) << 32);
	ਜੇ (trap->cause)
		ਵਾਪਸ 0;

	ਵਾਪਸ ret;
}

ਖਾਲੀ sbi_store_u64(u64 *addr, u64 val,
		   ਬਣਤਰ sbi_trap_info *trap)
{
	sbi_store_u32((u32 *)addr, val, trap);
	ਜੇ (trap->cause)
		ਵਾਪਸ;

	sbi_store_u32((u32 *)addr + 1, val >> 32, trap);
	ਜੇ (trap->cause)
		ਵਾਪਸ;
}
#ਨਹੀਂਤਾਂ
# error "Unexpected __riscv_xlen"
#ਜੇ_ਅੰਤ

ਖਾਲੀ sbi_ਲੱਦੋ_loop(u8 *ਬਫਰ, ulong addr, ulong len,
		   ਬਣਤਰ sbi_trap_info *trap)
{
	ਸੰਘ sbi_unpriv_ਅੰਕੜੇ ਅੰਕੜੇ;

	trap->cause = 0;
	ਜਦੋਂ (len) {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਚੌੜਾਈ = __riscv_xlen / 8;
		ਖਾਲੀ *ptr = (ਖਾਲੀ*)addr;

		ਜਦੋਂ (len < ਚੌੜਾਈ || (addr & (ਚੌੜਾਈ - 1)))
			ਚੌੜਾਈ /= 2;

		ਚੋਣ (ਚੌੜਾਈ) {
		ਹਾਲਤ 1:
			ਅੰਕੜੇ.b = sbi_ਲੱਦੋ_u8(ptr, trap);
			ਰੋਕੋ;
		ਹਾਲਤ 2:
			ਅੰਕੜੇ.h = sbi_ਲੱਦੋ_u16(ptr, trap);
			ਰੋਕੋ;
		ਹਾਲਤ 4:
			ਅੰਕੜੇ.w = sbi_ਲੱਦੋ_u32(ptr, trap);
			ਰੋਕੋ;
#ਜੇ __riscv_xlen == 64
		ਹਾਲਤ 8:
			ਅੰਕੜੇ.d = sbi_ਲੱਦੋ_u64(ptr, trap);
			ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		}
		ਜੇ (trap->cause)
			ਵਾਪਸ;

		sbi_memcpy(ਬਫਰ, ਅੰਕੜੇ.ਬਾਈਟਾਂ, ਚੌੜਾਈ);
		len -= ਚੌੜਾਈ;
		addr += ਚੌੜਾਈ;
		ਬਫਰ += ਚੌੜਾਈ;
	}
}

ਖਾਲੀ sbi_store_loop(u8 *ਬਫਰ, ulong addr, ulong len,
		    ਬਣਤਰ sbi_trap_info *trap)
{
	ਸੰਘ sbi_unpriv_ਅੰਕੜੇ ਅੰਕੜੇ;

	trap->cause = 0;
	ਜਦੋਂ (len) {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਚੌੜਾਈ = __riscv_xlen / 8;
		ਖਾਲੀ *ptr = (ਖਾਲੀ*)addr;

		ਜਦੋਂ (len < ਚੌੜਾਈ || (addr & (ਚੌੜਾਈ - 1)))
			ਚੌੜਾਈ /= 2;

		sbi_memcpy(ਅੰਕੜੇ.ਬਾਈਟਾਂ, ਬਫਰ, ਚੌੜਾਈ);
		ਚੋਣ (ਚੌੜਾਈ) {
		ਹਾਲਤ 1:
			sbi_store_u8(ptr, ਅੰਕੜੇ.b, trap);
			ਰੋਕੋ;
		ਹਾਲਤ 2:
			sbi_store_u16(ptr, ਅੰਕੜੇ.h, trap);
			ਰੋਕੋ;
		ਹਾਲਤ 4:
			sbi_store_u32(ptr, ਅੰਕੜੇ.w, trap);
			ਰੋਕੋ;
#ਜੇ __riscv_xlen == 64
		ਹਾਲਤ 8:
			sbi_store_u64(ptr, ਅੰਕੜੇ.d, trap);
			ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		}
		ਜੇ (trap->cause)
			ਵਾਪਸ;

		len -= ਚੌੜਾਈ;
		addr += ਚੌੜਾਈ;
		ਬਫਰ += ਚੌੜਾਈ;
	}
}

ulong sbi_get_insn(ulong mepc, ਬਣਤਰ sbi_trap_info *trap)
{
	ਰਜਿਸਟਰ ulong tinfo asm("a3");
	ਰਜਿਸਟਰ ulong ttmp asm("a4");
	ਰਜਿਸਟਰ ulong mstatus = 0;
	ਰਜਿਸਟਰ ulong mtvec = (ulong)sbi_hart_expected_trap;
	ulong insn = 0;

	trap->cause = 0;

	asm ਅਸਥਿਰ(
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

	ਚੋਣ (trap->cause) {
	ਹਾਲਤ CAUSE_ਲੱਦੋ_ACCESS:
		trap->cause = CAUSE_FETCH_ACCESS;
		trap->tinst = 0UL;
		ਰੋਕੋ;
	ਹਾਲਤ CAUSE_ਲੱਦੋ_ਪੰਨਾ_FAULT:
		trap->cause = CAUSE_FETCH_ਪੰਨਾ_FAULT;
		trap->tinst = 0UL;
		ਰੋਕੋ;
	ਹਾਲਤ CAUSE_ਲੱਦੋ_GUEST_ਪੰਨਾ_FAULT:
		trap->cause = CAUSE_FETCH_GUEST_ਪੰਨਾ_FAULT;
		ਜੇ (trap->tinst != INSN_PSEUDO_VS_ਲੱਦੋ &&
		    trap->tinst != INSN_PSEUDO_VS_STORE)
			trap->tinst = 0UL;
		ਰੋਕੋ;
	ਮੂਲ:
		ਰੋਕੋ;
	}

	ਵਾਪਸ insn;
}
