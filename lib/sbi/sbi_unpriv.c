/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_encoding.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_unpriv.h>

സംഘം sbi_unpriv_data {
	u8 b;
	u16 h;
	u32 w;
#എങ്കിൽ __riscv_xlen == 64
	u64 d;
#endif
	u8 bytes[__riscv_xlen / 8];
};

/**
 * a3 must a pointer to the sbi_trap_info and a4 is used as a temporary
 * register in the trap handler. Make sure that compiler doesn't use a3 & a4.
 */
#define DEFINE_UNPRIVILEGED_LOAD_FUNCTION(type, insn)                         \
	type sbi_load_##type(സ്ഥിരം type *addr,                                \
			     ഘടന sbi_trap_info *trap)                      \
	{                                                                     \
		രജിസ്റ്റർ ulong tinfo asm("a3") = (ulong)trap;                 \
		രജിസ്റ്റർ ulong mstatus = 0;                                   \
		രജിസ്റ്റർ ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		type ret = 0;                                                 \
		trap->cause = 0;                                              \
		asm അസ്ഥിര(                                                 \
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
		തിരികെ ret;                                                   \
	}

#define DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                        \
	ശൂന്യം sbi_store_##type(type *addr, type val,                           \
			      ഘടന sbi_trap_info *trap)                     \
	{                                                                     \
		രജിസ്റ്റർ ulong tinfo asm("a3") = (ulong)trap;                 \
		രജിസ്റ്റർ ulong mstatus = 0;                                   \
		രജിസ്റ്റർ ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		trap->cause = 0;                                              \
		asm അസ്ഥിര(                                                 \
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
#എങ്കിൽ __riscv_xlen == 64
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lwu)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u64, ld)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, sd)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, ld)
#അല്ലെങ്കിലെങ്കിൽ __riscv_xlen == 32
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lw)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, lw)

u64 sbi_load_u64(സ്ഥിരം u64 *addr,
		 ഘടന sbi_trap_info *trap)
{
	u64 ret = sbi_load_u32((u32 *)addr, trap);

	എങ്കിൽ (trap->cause)
		തിരികെ 0;
	ret |= ((u64)sbi_load_u32((u32 *)addr + 1, trap) << 32);
	എങ്കിൽ (trap->cause)
		തിരികെ 0;

	തിരികെ ret;
}

ശൂന്യം sbi_store_u64(u64 *addr, u64 val,
		   ഘടന sbi_trap_info *trap)
{
	sbi_store_u32((u32 *)addr, val, trap);
	എങ്കിൽ (trap->cause)
		തിരികെ;

	sbi_store_u32((u32 *)addr + 1, val >> 32, trap);
	എങ്കിൽ (trap->cause)
		തിരികെ;
}
#അല്ലെങ്കിൽ
# error "Unexpected __riscv_xlen"
#endif

ശൂന്യം sbi_load_loop(u8 *buffer, ulong addr, ulong len,
		   ഘടന sbi_trap_info *trap)
{
	സംഘം sbi_unpriv_data data;

	trap->cause = 0;
	ആകുമ്പോൾ (len) {
		unsigned പൂർണ്ണസംഖ്യ width = __riscv_xlen / 8;
		ശൂന്യം *ptr = (ശൂന്യം*)addr;

		ആകുമ്പോൾ (len < width || (addr & (width - 1)))
			width /= 2;

		തിരഞ്ഞെടുപ്പ് (width) {
		അവസ്ഥ 1:
			data.b = sbi_load_u8(ptr, trap);
			നിർത്തുക;
		അവസ്ഥ 2:
			data.h = sbi_load_u16(ptr, trap);
			നിർത്തുക;
		അവസ്ഥ 4:
			data.w = sbi_load_u32(ptr, trap);
			നിർത്തുക;
#എങ്കിൽ __riscv_xlen == 64
		അവസ്ഥ 8:
			data.d = sbi_load_u64(ptr, trap);
			നിർത്തുക;
#endif
		}
		എങ്കിൽ (trap->cause)
			തിരികെ;

		sbi_memcpy(buffer, data.bytes, width);
		len -= width;
		addr += width;
		buffer += width;
	}
}

ശൂന്യം sbi_store_loop(u8 *buffer, ulong addr, ulong len,
		    ഘടന sbi_trap_info *trap)
{
	സംഘം sbi_unpriv_data data;

	trap->cause = 0;
	ആകുമ്പോൾ (len) {
		unsigned പൂർണ്ണസംഖ്യ width = __riscv_xlen / 8;
		ശൂന്യം *ptr = (ശൂന്യം*)addr;

		ആകുമ്പോൾ (len < width || (addr & (width - 1)))
			width /= 2;

		sbi_memcpy(data.bytes, buffer, width);
		തിരഞ്ഞെടുപ്പ് (width) {
		അവസ്ഥ 1:
			sbi_store_u8(ptr, data.b, trap);
			നിർത്തുക;
		അവസ്ഥ 2:
			sbi_store_u16(ptr, data.h, trap);
			നിർത്തുക;
		അവസ്ഥ 4:
			sbi_store_u32(ptr, data.w, trap);
			നിർത്തുക;
#എങ്കിൽ __riscv_xlen == 64
		അവസ്ഥ 8:
			sbi_store_u64(ptr, data.d, trap);
			നിർത്തുക;
#endif
		}
		എങ്കിൽ (trap->cause)
			തിരികെ;

		len -= width;
		addr += width;
		buffer += width;
	}
}

ulong sbi_get_insn(ulong mepc, ഘടന sbi_trap_info *trap)
{
	രജിസ്റ്റർ ulong tinfo asm("a3");
	രജിസ്റ്റർ ulong ttmp asm("a4");
	രജിസ്റ്റർ ulong mstatus = 0;
	രജിസ്റ്റർ ulong mtvec = (ulong)sbi_hart_expected_trap;
	ulong insn = 0;

	trap->cause = 0;

	asm അസ്ഥിര(
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

	തിരഞ്ഞെടുപ്പ് (trap->cause) {
	അവസ്ഥ CAUSE_LOAD_ACCESS:
		trap->cause = CAUSE_FETCH_ACCESS;
		trap->tinst = 0UL;
		നിർത്തുക;
	അവസ്ഥ CAUSE_LOAD_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_PAGE_FAULT;
		trap->tinst = 0UL;
		നിർത്തുക;
	അവസ്ഥ CAUSE_LOAD_GUEST_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_GUEST_PAGE_FAULT;
		എങ്കിൽ (trap->tinst != INSN_PSEUDO_VS_LOAD &&
		    trap->tinst != INSN_PSEUDO_VS_STORE)
			trap->tinst = 0UL;
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}

	തിരികെ insn;
}
