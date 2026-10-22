/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_encoding.h>
#અન્તર્ભાવ <sbi/sbi_bitops.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_string.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_unpriv.h>

સંઘ sbi_unpriv_data {
	u8 b;
	u16 h;
	u32 w;
#જો __riscv_xlen == 64
	u64 d;
#endif
	u8 bytes[__riscv_xlen / 8];
};

/**
 * a3 must a pointer to the sbi_trap_info and a4 is used as a temporary
 * register in the trap handler. Make sure that compiler doesn't use a3 & a4.
 */
#define DEFINE_UNPRIVILEGED_LOAD_FUNCTION(type, insn)                         \
	type sbi_load_##type(અચલ type *addr,                                \
			     સંરચના sbi_trap_info *trap)                      \
	{                                                                     \
		નોંધણી ulong tinfo asm("a3") = (ulong)trap;                 \
		નોંધણી ulong mstatus = 0;                                   \
		નોંધણી ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		type ret = 0;                                                 \
		trap->cause = 0;                                              \
		asm અસ્થિર(                                                 \
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
		પાછા ret;                                                   \
	}

#define DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                        \
	શૂન્ય sbi_store_##type(type *addr, type val,                           \
			      સંરચના sbi_trap_info *trap)                     \
	{                                                                     \
		નોંધણી ulong tinfo asm("a3") = (ulong)trap;                 \
		નોંધણી ulong mstatus = 0;                                   \
		નોંધણી ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		trap->cause = 0;                                              \
		asm અસ્થિર(                                                 \
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
#જો __riscv_xlen == 64
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lwu)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u64, ld)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, sd)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, ld)
#અથવાજો __riscv_xlen == 32
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lw)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, lw)

u64 sbi_load_u64(અચલ u64 *addr,
		 સંરચના sbi_trap_info *trap)
{
	u64 ret = sbi_load_u32((u32 *)addr, trap);

	જો (trap->cause)
		પાછા 0;
	ret |= ((u64)sbi_load_u32((u32 *)addr + 1, trap) << 32);
	જો (trap->cause)
		પાછા 0;

	પાછા ret;
}

શૂન્ય sbi_store_u64(u64 *addr, u64 val,
		   સંરચના sbi_trap_info *trap)
{
	sbi_store_u32((u32 *)addr, val, trap);
	જો (trap->cause)
		પાછા;

	sbi_store_u32((u32 *)addr + 1, val >> 32, trap);
	જો (trap->cause)
		પાછા;
}
#નહીંતર
# error "Unexpected __riscv_xlen"
#endif

શૂન્ય sbi_load_loop(u8 *buffer, ulong addr, ulong len,
		   સંરચના sbi_trap_info *trap)
{
	સંઘ sbi_unpriv_data data;

	trap->cause = 0;
	જ્યારે (len) {
		unsigned પૂર્ણાંક width = __riscv_xlen / 8;
		શૂન્ય *ptr = (શૂન્ય*)addr;

		જ્યારે (len < width || (addr & (width - 1)))
			width /= 2;

		પસંદગી (width) {
		સ્થિતિ 1:
			data.b = sbi_load_u8(ptr, trap);
			તોડો;
		સ્થિતિ 2:
			data.h = sbi_load_u16(ptr, trap);
			તોડો;
		સ્થિતિ 4:
			data.w = sbi_load_u32(ptr, trap);
			તોડો;
#જો __riscv_xlen == 64
		સ્થિતિ 8:
			data.d = sbi_load_u64(ptr, trap);
			તોડો;
#endif
		}
		જો (trap->cause)
			પાછા;

		sbi_memcpy(buffer, data.bytes, width);
		len -= width;
		addr += width;
		buffer += width;
	}
}

શૂન્ય sbi_store_loop(u8 *buffer, ulong addr, ulong len,
		    સંરચના sbi_trap_info *trap)
{
	સંઘ sbi_unpriv_data data;

	trap->cause = 0;
	જ્યારે (len) {
		unsigned પૂર્ણાંક width = __riscv_xlen / 8;
		શૂન્ય *ptr = (શૂન્ય*)addr;

		જ્યારે (len < width || (addr & (width - 1)))
			width /= 2;

		sbi_memcpy(data.bytes, buffer, width);
		પસંદગી (width) {
		સ્થિતિ 1:
			sbi_store_u8(ptr, data.b, trap);
			તોડો;
		સ્થિતિ 2:
			sbi_store_u16(ptr, data.h, trap);
			તોડો;
		સ્થિતિ 4:
			sbi_store_u32(ptr, data.w, trap);
			તોડો;
#જો __riscv_xlen == 64
		સ્થિતિ 8:
			sbi_store_u64(ptr, data.d, trap);
			તોડો;
#endif
		}
		જો (trap->cause)
			પાછા;

		len -= width;
		addr += width;
		buffer += width;
	}
}

ulong sbi_get_insn(ulong mepc, સંરચના sbi_trap_info *trap)
{
	નોંધણી ulong tinfo asm("a3");
	નોંધણી ulong ttmp asm("a4");
	નોંધણી ulong mstatus = 0;
	નોંધણી ulong mtvec = (ulong)sbi_hart_expected_trap;
	ulong insn = 0;

	trap->cause = 0;

	asm અસ્થિર(
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

	પસંદગી (trap->cause) {
	સ્થિતિ CAUSE_LOAD_ACCESS:
		trap->cause = CAUSE_FETCH_ACCESS;
		trap->tinst = 0UL;
		તોડો;
	સ્થિતિ CAUSE_LOAD_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_PAGE_FAULT;
		trap->tinst = 0UL;
		તોડો;
	સ્થિતિ CAUSE_LOAD_GUEST_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_GUEST_PAGE_FAULT;
		જો (trap->tinst != INSN_PSEUDO_VS_LOAD &&
		    trap->tinst != INSN_PSEUDO_VS_STORE)
			trap->tinst = 0UL;
		તોડો;
	મૂળભૂત:
		તોડો;
	}

	પાછા insn;
}
