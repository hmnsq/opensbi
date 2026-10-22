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

ಸಙ್ಘ sbi_unpriv_data {
	u8 b;
	u16 h;
	u32 w;
#ಯದಿ __riscv_xlen == 64
	u64 d;
#endif
	u8 bytes[__riscv_xlen / 8];
};

/**
 * a3 must a pointer to the sbi_trap_info and a4 is used as a temporary
 * register in the trap handler. Make sure that compiler doesn't use a3 & a4.
 */
#define DEFINE_UNPRIVILEGED_LOAD_FUNCTION(type, insn)                         \
	type sbi_load_##type(ಧ್ರುವ type *addr,                                \
			     ಸಂರಚನಾ sbi_trap_info *trap)                      \
	{                                                                     \
		ಪಞ್ಜಿಕಾ ulong tinfo asm("a3") = (ulong)trap;                 \
		ಪಞ್ಜಿಕಾ ulong mstatus = 0;                                   \
		ಪಞ್ಜಿಕಾ ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		type ret = 0;                                                 \
		trap->cause = 0;                                              \
		asm ಚಞ್ಚಲ(                                                 \
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
		ಪ್ರತ್ಯಾಗಮ ret;                                                   \
	}

#define DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                        \
	ಶೂನ್ಯ sbi_store_##type(type *addr, type val,                           \
			      ಸಂರಚನಾ sbi_trap_info *trap)                     \
	{                                                                     \
		ಪಞ್ಜಿಕಾ ulong tinfo asm("a3") = (ulong)trap;                 \
		ಪಞ್ಜಿಕಾ ulong mstatus = 0;                                   \
		ಪಞ್ಜಿಕಾ ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		trap->cause = 0;                                              \
		asm ಚಞ್ಚಲ(                                                 \
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
#ಯದಿ __riscv_xlen == 64
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lwu)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u64, ld)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, sd)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, ld)
#ನೋಚೇದ್ಯದಿ __riscv_xlen == 32
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lw)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, lw)

u64 sbi_load_u64(ಧ್ರುವ u64 *addr,
		 ಸಂರಚನಾ sbi_trap_info *trap)
{
	u64 ret = sbi_load_u32((u32 *)addr, trap);

	ಯದಿ (trap->cause)
		ಪ್ರತ್ಯಾಗಮ 0;
	ret |= ((u64)sbi_load_u32((u32 *)addr + 1, trap) << 32);
	ಯದಿ (trap->cause)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ sbi_store_u64(u64 *addr, u64 val,
		   ಸಂರಚನಾ sbi_trap_info *trap)
{
	sbi_store_u32((u32 *)addr, val, trap);
	ಯದಿ (trap->cause)
		ಪ್ರತ್ಯಾಗಮ;

	sbi_store_u32((u32 *)addr + 1, val >> 32, trap);
	ಯದಿ (trap->cause)
		ಪ್ರತ್ಯಾಗಮ;
}
#ಅನ್ಯಥಾ
# error "Unexpected __riscv_xlen"
#endif

ಶೂನ್ಯ sbi_load_loop(u8 *buffer, ulong addr, ulong len,
		   ಸಂರಚನಾ sbi_trap_info *trap)
{
	ಸಙ್ಘ sbi_unpriv_data data;

	trap->cause = 0;
	ಯಾವತ್ (len) {
		unsigned ಪೂರ್ಣಾಙ್ಕ width = __riscv_xlen / 8;
		ಶೂನ್ಯ *ptr = (ಶೂನ್ಯ*)addr;

		ಯಾವತ್ (len < width || (addr & (width - 1)))
			width /= 2;

		ವಿಕಲ್ಪ (width) {
		ಪಕ್ಷ 1:
			data.b = sbi_load_u8(ptr, trap);
			ಭಙ್ಗ;
		ಪಕ್ಷ 2:
			data.h = sbi_load_u16(ptr, trap);
			ಭಙ್ಗ;
		ಪಕ್ಷ 4:
			data.w = sbi_load_u32(ptr, trap);
			ಭಙ್ಗ;
#ಯದಿ __riscv_xlen == 64
		ಪಕ್ಷ 8:
			data.d = sbi_load_u64(ptr, trap);
			ಭಙ್ಗ;
#endif
		}
		ಯದಿ (trap->cause)
			ಪ್ರತ್ಯಾಗಮ;

		sbi_memcpy(buffer, data.bytes, width);
		len -= width;
		addr += width;
		buffer += width;
	}
}

ಶೂನ್ಯ sbi_store_loop(u8 *buffer, ulong addr, ulong len,
		    ಸಂರಚನಾ sbi_trap_info *trap)
{
	ಸಙ್ಘ sbi_unpriv_data data;

	trap->cause = 0;
	ಯಾವತ್ (len) {
		unsigned ಪೂರ್ಣಾಙ್ಕ width = __riscv_xlen / 8;
		ಶೂನ್ಯ *ptr = (ಶೂನ್ಯ*)addr;

		ಯಾವತ್ (len < width || (addr & (width - 1)))
			width /= 2;

		sbi_memcpy(data.bytes, buffer, width);
		ವಿಕಲ್ಪ (width) {
		ಪಕ್ಷ 1:
			sbi_store_u8(ptr, data.b, trap);
			ಭಙ್ಗ;
		ಪಕ್ಷ 2:
			sbi_store_u16(ptr, data.h, trap);
			ಭಙ್ಗ;
		ಪಕ್ಷ 4:
			sbi_store_u32(ptr, data.w, trap);
			ಭಙ್ಗ;
#ಯದಿ __riscv_xlen == 64
		ಪಕ್ಷ 8:
			sbi_store_u64(ptr, data.d, trap);
			ಭಙ್ಗ;
#endif
		}
		ಯದಿ (trap->cause)
			ಪ್ರತ್ಯಾಗಮ;

		len -= width;
		addr += width;
		buffer += width;
	}
}

ulong sbi_get_insn(ulong mepc, ಸಂರಚನಾ sbi_trap_info *trap)
{
	ಪಞ್ಜಿಕಾ ulong tinfo asm("a3");
	ಪಞ್ಜಿಕಾ ulong ttmp asm("a4");
	ಪಞ್ಜಿಕಾ ulong mstatus = 0;
	ಪಞ್ಜಿಕಾ ulong mtvec = (ulong)sbi_hart_expected_trap;
	ulong insn = 0;

	trap->cause = 0;

	asm ಚಞ್ಚಲ(
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

	ವಿಕಲ್ಪ (trap->cause) {
	ಪಕ್ಷ CAUSE_LOAD_ACCESS:
		trap->cause = CAUSE_FETCH_ACCESS;
		trap->tinst = 0UL;
		ಭಙ್ಗ;
	ಪಕ್ಷ CAUSE_LOAD_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_PAGE_FAULT;
		trap->tinst = 0UL;
		ಭಙ್ಗ;
	ಪಕ್ಷ CAUSE_LOAD_GUEST_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_GUEST_PAGE_FAULT;
		ಯದಿ (trap->tinst != INSN_PSEUDO_VS_LOAD &&
		    trap->tinst != INSN_PSEUDO_VS_STORE)
			trap->tinst = 0UL;
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ insn;
}
