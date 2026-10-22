/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_unpriv.h>

ಒಕ್ಕೂಟ sbi_unpriv_ದತ್ತಾಂಶ {
	u8 b;
	u16 h;
	u32 w;
#ಆದರೆ __riscv_xlen == 64
	u64 d;
#ಆದರೆ_ಅಂತ್ಯ
	u8 ಬೈಟುಗಳು[__riscv_xlen / 8];
};

/**
 * a3 must a pointer to the sbi_trap_info and a4 is used as a temporary
 * register in the trap handler. Make sure that compiler doesn't use a3 & a4.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_UNPRIVILEGED_ತುಂಬು_FUNCTION(type, insn)                         \
	type sbi_ತುಂಬು##type(ಸ್ಥಿರಾಂಕ type *addr,                                \
			     ರಚನೆ sbi_trap_info *trap)                      \
	{                                                                     \
		ನೋಂದಣಿ ulong tinfo asm("a3") = (ulong)trap;                 \
		ನೋಂದಣಿ ulong mstatus = 0;                                   \
		ನೋಂದಣಿ ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		type ret = 0;                                                 \
		trap->cause = 0;                                              \
		asm ಅಸ್ಥಿರ(                                                 \
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
		ಹಿಂತಿರುಗಿ ret;                                                   \
	}

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                        \
	ಶೂನ್ಯ sbi_store_##type(type *addr, type val,                           \
			      ರಚನೆ sbi_trap_info *trap)                     \
	{                                                                     \
		ನೋಂದಣಿ ulong tinfo asm("a3") = (ulong)trap;                 \
		ನೋಂದಣಿ ulong mstatus = 0;                                   \
		ನೋಂದಣಿ ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		trap->cause = 0;                                              \
		asm ಅಸ್ಥಿರ(                                                 \
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

DEFINE_UNPRIVILEGED_ತುಂಬು_FUNCTION(u8, lbu)
DEFINE_UNPRIVILEGED_ತುಂಬು_FUNCTION(u16, lhu)
DEFINE_UNPRIVILEGED_ತುಂಬು_FUNCTION(s8, lb)
DEFINE_UNPRIVILEGED_ತುಂಬು_FUNCTION(s16, lh)
DEFINE_UNPRIVILEGED_ತುಂಬು_FUNCTION(s32, lw)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u8, sb)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u16, sh)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u32, sw)
#ಆದರೆ __riscv_xlen == 64
DEFINE_UNPRIVILEGED_ತುಂಬು_FUNCTION(u32, lwu)
DEFINE_UNPRIVILEGED_ತುಂಬು_FUNCTION(u64, ld)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, sd)
DEFINE_UNPRIVILEGED_ತುಂಬು_FUNCTION(ulong, ld)
#ಅಥವಾಆದರೆ __riscv_xlen == 32
DEFINE_UNPRIVILEGED_ತುಂಬು_FUNCTION(u32, lw)
DEFINE_UNPRIVILEGED_ತುಂಬು_FUNCTION(ulong, lw)

u64 sbi_ತುಂಬು_u64(ಸ್ಥಿರಾಂಕ u64 *addr,
		 ರಚನೆ sbi_trap_info *trap)
{
	u64 ret = sbi_ತುಂಬು_u32((u32 *)addr, trap);

	ಆದರೆ (trap->cause)
		ಹಿಂತಿರುಗಿ 0;
	ret |= ((u64)sbi_ತುಂಬು_u32((u32 *)addr + 1, trap) << 32);
	ಆದರೆ (trap->cause)
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ sbi_store_u64(u64 *addr, u64 val,
		   ರಚನೆ sbi_trap_info *trap)
{
	sbi_store_u32((u32 *)addr, val, trap);
	ಆದರೆ (trap->cause)
		ಹಿಂತಿರುಗಿ;

	sbi_store_u32((u32 *)addr + 1, val >> 32, trap);
	ಆದರೆ (trap->cause)
		ಹಿಂತಿರುಗಿ;
}
#ಇಲ್ಲದಿದ್ದರೆ
# error "Unexpected __riscv_xlen"
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ sbi_ತುಂಬು_loop(u8 *ಬಫರ್, ulong addr, ulong len,
		   ರಚನೆ sbi_trap_info *trap)
{
	ಒಕ್ಕೂಟ sbi_unpriv_ದತ್ತಾಂಶ ದತ್ತಾಂಶ;

	trap->cause = 0;
	ಆಗ (len) {
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಅಗಲ = __riscv_xlen / 8;
		ಶೂನ್ಯ *ptr = (ಶೂನ್ಯ*)addr;

		ಆಗ (len < ಅಗಲ || (addr & (ಅಗಲ - 1)))
			ಅಗಲ /= 2;

		ಆಯ್ಕೆ (ಅಗಲ) {
		ಸ್ಥಿತಿ 1:
			ದತ್ತಾಂಶ.b = sbi_ತುಂಬು_u8(ptr, trap);
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 2:
			ದತ್ತಾಂಶ.h = sbi_ತುಂಬು_u16(ptr, trap);
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 4:
			ದತ್ತಾಂಶ.w = sbi_ತುಂಬು_u32(ptr, trap);
			ನಿಲ್ಲಿಸು;
#ಆದರೆ __riscv_xlen == 64
		ಸ್ಥಿತಿ 8:
			ದತ್ತಾಂಶ.d = sbi_ತುಂಬು_u64(ptr, trap);
			ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಆದರೆ (trap->cause)
			ಹಿಂತಿರುಗಿ;

		sbi_memcpy(ಬಫರ್, ದತ್ತಾಂಶ.ಬೈಟುಗಳು, ಅಗಲ);
		len -= ಅಗಲ;
		addr += ಅಗಲ;
		ಬಫರ್ += ಅಗಲ;
	}
}

ಶೂನ್ಯ sbi_store_loop(u8 *ಬಫರ್, ulong addr, ulong len,
		    ರಚನೆ sbi_trap_info *trap)
{
	ಒಕ್ಕೂಟ sbi_unpriv_ದತ್ತಾಂಶ ದತ್ತಾಂಶ;

	trap->cause = 0;
	ಆಗ (len) {
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಅಗಲ = __riscv_xlen / 8;
		ಶೂನ್ಯ *ptr = (ಶೂನ್ಯ*)addr;

		ಆಗ (len < ಅಗಲ || (addr & (ಅಗಲ - 1)))
			ಅಗಲ /= 2;

		sbi_memcpy(ದತ್ತಾಂಶ.ಬೈಟುಗಳು, ಬಫರ್, ಅಗಲ);
		ಆಯ್ಕೆ (ಅಗಲ) {
		ಸ್ಥಿತಿ 1:
			sbi_store_u8(ptr, ದತ್ತಾಂಶ.b, trap);
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 2:
			sbi_store_u16(ptr, ದತ್ತಾಂಶ.h, trap);
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 4:
			sbi_store_u32(ptr, ದತ್ತಾಂಶ.w, trap);
			ನಿಲ್ಲಿಸು;
#ಆದರೆ __riscv_xlen == 64
		ಸ್ಥಿತಿ 8:
			sbi_store_u64(ptr, ದತ್ತಾಂಶ.d, trap);
			ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಆದರೆ (trap->cause)
			ಹಿಂತಿರುಗಿ;

		len -= ಅಗಲ;
		addr += ಅಗಲ;
		ಬಫರ್ += ಅಗಲ;
	}
}

ulong sbi_get_insn(ulong mepc, ರಚನೆ sbi_trap_info *trap)
{
	ನೋಂದಣಿ ulong tinfo asm("a3");
	ನೋಂದಣಿ ulong ttmp asm("a4");
	ನೋಂದಣಿ ulong mstatus = 0;
	ನೋಂದಣಿ ulong mtvec = (ulong)sbi_hart_expected_trap;
	ulong insn = 0;

	trap->cause = 0;

	asm ಅಸ್ಥಿರ(
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

	ಆಯ್ಕೆ (trap->cause) {
	ಸ್ಥಿತಿ CAUSE_ತುಂಬು_ACCESS:
		trap->cause = CAUSE_FETCH_ACCESS;
		trap->tinst = 0UL;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CAUSE_ತುಂಬು_ಪುಟ_FAULT:
		trap->cause = CAUSE_FETCH_ಪುಟ_FAULT;
		trap->tinst = 0UL;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CAUSE_ತುಂಬು_GUEST_ಪುಟ_FAULT:
		trap->cause = CAUSE_FETCH_GUEST_ಪುಟ_FAULT;
		ಆದರೆ (trap->tinst != INSN_PSEUDO_VS_ತುಂಬು &&
		    trap->tinst != INSN_PSEUDO_VS_STORE)
			trap->tinst = 0UL;
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ insn;
}
