/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_encoding.h>
#অন্তর্ভাব <sbi/sbi_bitops.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_unpriv.h>

সঙ্ঘ sbi_unpriv_data {
	u8 b;
	u16 h;
	u32 w;
#যদি __riscv_xlen == 64
	u64 d;
#endif
	u8 bytes[__riscv_xlen / 8];
};

/**
 * a3 must a pointer to the sbi_trap_info and a4 is used as a temporary
 * register in the trap handler. Make sure that compiler doesn't use a3 & a4.
 */
#define DEFINE_UNPRIVILEGED_LOAD_FUNCTION(type, insn)                         \
	type sbi_load_##type(ধ্রুব type *addr,                                \
			     সংরচনা sbi_trap_info *trap)                      \
	{                                                                     \
		পঞ্জিকা ulong tinfo asm("a3") = (ulong)trap;                 \
		পঞ্জিকা ulong mstatus = 0;                                   \
		পঞ্জিকা ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		type ret = 0;                                                 \
		trap->cause = 0;                                              \
		asm চঞ্চল(                                                 \
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
		প্রত্যাগম ret;                                                   \
	}

#define DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                        \
	শূন্য sbi_store_##type(type *addr, type val,                           \
			      সংরচনা sbi_trap_info *trap)                     \
	{                                                                     \
		পঞ্জিকা ulong tinfo asm("a3") = (ulong)trap;                 \
		পঞ্জিকা ulong mstatus = 0;                                   \
		পঞ্জিকা ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		trap->cause = 0;                                              \
		asm চঞ্চল(                                                 \
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
#যদি __riscv_xlen == 64
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lwu)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u64, ld)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, sd)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, ld)
#নোচেদ্যদি __riscv_xlen == 32
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lw)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, lw)

u64 sbi_load_u64(ধ্রুব u64 *addr,
		 সংরচনা sbi_trap_info *trap)
{
	u64 ret = sbi_load_u32((u32 *)addr, trap);

	যদি (trap->cause)
		প্রত্যাগম 0;
	ret |= ((u64)sbi_load_u32((u32 *)addr + 1, trap) << 32);
	যদি (trap->cause)
		প্রত্যাগম 0;

	প্রত্যাগম ret;
}

শূন্য sbi_store_u64(u64 *addr, u64 val,
		   সংরচনা sbi_trap_info *trap)
{
	sbi_store_u32((u32 *)addr, val, trap);
	যদি (trap->cause)
		প্রত্যাগম;

	sbi_store_u32((u32 *)addr + 1, val >> 32, trap);
	যদি (trap->cause)
		প্রত্যাগম;
}
#অন্যথা
# error "Unexpected __riscv_xlen"
#endif

শূন্য sbi_load_loop(u8 *buffer, ulong addr, ulong len,
		   সংরচনা sbi_trap_info *trap)
{
	সঙ্ঘ sbi_unpriv_data data;

	trap->cause = 0;
	যাবত্ (len) {
		unsigned পূর্ণাঙ্ক width = __riscv_xlen / 8;
		শূন্য *ptr = (শূন্য*)addr;

		যাবত্ (len < width || (addr & (width - 1)))
			width /= 2;

		বিকল্প (width) {
		পক্ষ 1:
			data.b = sbi_load_u8(ptr, trap);
			ভঙ্গ;
		পক্ষ 2:
			data.h = sbi_load_u16(ptr, trap);
			ভঙ্গ;
		পক্ষ 4:
			data.w = sbi_load_u32(ptr, trap);
			ভঙ্গ;
#যদি __riscv_xlen == 64
		পক্ষ 8:
			data.d = sbi_load_u64(ptr, trap);
			ভঙ্গ;
#endif
		}
		যদি (trap->cause)
			প্রত্যাগম;

		sbi_memcpy(buffer, data.bytes, width);
		len -= width;
		addr += width;
		buffer += width;
	}
}

শূন্য sbi_store_loop(u8 *buffer, ulong addr, ulong len,
		    সংরচনা sbi_trap_info *trap)
{
	সঙ্ঘ sbi_unpriv_data data;

	trap->cause = 0;
	যাবত্ (len) {
		unsigned পূর্ণাঙ্ক width = __riscv_xlen / 8;
		শূন্য *ptr = (শূন্য*)addr;

		যাবত্ (len < width || (addr & (width - 1)))
			width /= 2;

		sbi_memcpy(data.bytes, buffer, width);
		বিকল্প (width) {
		পক্ষ 1:
			sbi_store_u8(ptr, data.b, trap);
			ভঙ্গ;
		পক্ষ 2:
			sbi_store_u16(ptr, data.h, trap);
			ভঙ্গ;
		পক্ষ 4:
			sbi_store_u32(ptr, data.w, trap);
			ভঙ্গ;
#যদি __riscv_xlen == 64
		পক্ষ 8:
			sbi_store_u64(ptr, data.d, trap);
			ভঙ্গ;
#endif
		}
		যদি (trap->cause)
			প্রত্যাগম;

		len -= width;
		addr += width;
		buffer += width;
	}
}

ulong sbi_get_insn(ulong mepc, সংরচনা sbi_trap_info *trap)
{
	পঞ্জিকা ulong tinfo asm("a3");
	পঞ্জিকা ulong ttmp asm("a4");
	পঞ্জিকা ulong mstatus = 0;
	পঞ্জিকা ulong mtvec = (ulong)sbi_hart_expected_trap;
	ulong insn = 0;

	trap->cause = 0;

	asm চঞ্চল(
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

	বিকল্প (trap->cause) {
	পক্ষ CAUSE_LOAD_ACCESS:
		trap->cause = CAUSE_FETCH_ACCESS;
		trap->tinst = 0UL;
		ভঙ্গ;
	পক্ষ CAUSE_LOAD_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_PAGE_FAULT;
		trap->tinst = 0UL;
		ভঙ্গ;
	পক্ষ CAUSE_LOAD_GUEST_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_GUEST_PAGE_FAULT;
		যদি (trap->tinst != INSN_PSEUDO_VS_LOAD &&
		    trap->tinst != INSN_PSEUDO_VS_STORE)
			trap->tinst = 0UL;
		ভঙ্গ;
	স্বভাব:
		ভঙ্গ;
	}

	প্রত্যাগম insn;
}
