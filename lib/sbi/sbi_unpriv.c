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

اجتماع sbi_unpriv_data {
	u8 b;
	u16 h;
	u32 w;
#اگر __riscv_xlen == 64
	u64 d;
#endif
	u8 bytes[__riscv_xlen / 8];
};

/**
 * a3 must a pointer to the sbi_trap_info and a4 is used as a temporary
 * register in the trap handler. Make sure that compiler doesn't use a3 & a4.
 */
#define DEFINE_UNPRIVILEGED_LOAD_FUNCTION(type, insn)                         \
	type sbi_load_##type(ثابت type *addr,                                \
			     ساختار sbi_trap_info *trap)                      \
	{                                                                     \
		ثبات ulong tinfo asm("a3") = (ulong)trap;                 \
		ثبات ulong mstatus = 0;                                   \
		ثبات ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		type ret = 0;                                                 \
		trap->cause = 0;                                              \
		asm فرار(                                                 \
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
		بازگشت ret;                                                   \
	}

#define DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                        \
	تهی sbi_store_##type(type *addr, type val,                           \
			      ساختار sbi_trap_info *trap)                     \
	{                                                                     \
		ثبات ulong tinfo asm("a3") = (ulong)trap;                 \
		ثبات ulong mstatus = 0;                                   \
		ثبات ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		trap->cause = 0;                                              \
		asm فرار(                                                 \
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
#اگر __riscv_xlen == 64
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lwu)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u64, ld)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, sd)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, ld)
#وگرنهاگر __riscv_xlen == 32
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lw)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, lw)

u64 sbi_load_u64(ثابت u64 *addr,
		 ساختار sbi_trap_info *trap)
{
	u64 ret = sbi_load_u32((u32 *)addr, trap);

	اگر (trap->cause)
		بازگشت 0;
	ret |= ((u64)sbi_load_u32((u32 *)addr + 1, trap) << 32);
	اگر (trap->cause)
		بازگشت 0;

	بازگشت ret;
}

تهی sbi_store_u64(u64 *addr, u64 val,
		   ساختار sbi_trap_info *trap)
{
	sbi_store_u32((u32 *)addr, val, trap);
	اگر (trap->cause)
		بازگشت;

	sbi_store_u32((u32 *)addr + 1, val >> 32, trap);
	اگر (trap->cause)
		بازگشت;
}
#وگرنه
# error "Unexpected __riscv_xlen"
#endif

تهی sbi_load_loop(u8 *buffer, ulong addr, ulong len,
		   ساختار sbi_trap_info *trap)
{
	اجتماع sbi_unpriv_data data;

	trap->cause = 0;
	وقتی (len) {
		unsigned صحیح width = __riscv_xlen / 8;
		تهی *ptr = (تهی*)addr;

		وقتی (len < width || (addr & (width - 1)))
			width /= 2;

		انتخاب (width) {
		حالت 1:
			data.b = sbi_load_u8(ptr, trap);
			بشکن;
		حالت 2:
			data.h = sbi_load_u16(ptr, trap);
			بشکن;
		حالت 4:
			data.w = sbi_load_u32(ptr, trap);
			بشکن;
#اگر __riscv_xlen == 64
		حالت 8:
			data.d = sbi_load_u64(ptr, trap);
			بشکن;
#endif
		}
		اگر (trap->cause)
			بازگشت;

		sbi_memcpy(buffer, data.bytes, width);
		len -= width;
		addr += width;
		buffer += width;
	}
}

تهی sbi_store_loop(u8 *buffer, ulong addr, ulong len,
		    ساختار sbi_trap_info *trap)
{
	اجتماع sbi_unpriv_data data;

	trap->cause = 0;
	وقتی (len) {
		unsigned صحیح width = __riscv_xlen / 8;
		تهی *ptr = (تهی*)addr;

		وقتی (len < width || (addr & (width - 1)))
			width /= 2;

		sbi_memcpy(data.bytes, buffer, width);
		انتخاب (width) {
		حالت 1:
			sbi_store_u8(ptr, data.b, trap);
			بشکن;
		حالت 2:
			sbi_store_u16(ptr, data.h, trap);
			بشکن;
		حالت 4:
			sbi_store_u32(ptr, data.w, trap);
			بشکن;
#اگر __riscv_xlen == 64
		حالت 8:
			sbi_store_u64(ptr, data.d, trap);
			بشکن;
#endif
		}
		اگر (trap->cause)
			بازگشت;

		len -= width;
		addr += width;
		buffer += width;
	}
}

ulong sbi_get_insn(ulong mepc, ساختار sbi_trap_info *trap)
{
	ثبات ulong tinfo asm("a3");
	ثبات ulong ttmp asm("a4");
	ثبات ulong mstatus = 0;
	ثبات ulong mtvec = (ulong)sbi_hart_expected_trap;
	ulong insn = 0;

	trap->cause = 0;

	asm فرار(
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
	حالت CAUSE_LOAD_ACCESS:
		trap->cause = CAUSE_FETCH_ACCESS;
		trap->tinst = 0UL;
		بشکن;
	حالت CAUSE_LOAD_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_PAGE_FAULT;
		trap->tinst = 0UL;
		بشکن;
	حالت CAUSE_LOAD_GUEST_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_GUEST_PAGE_FAULT;
		اگر (trap->tinst != INSN_PSEUDO_VS_LOAD &&
		    trap->tinst != INSN_PSEUDO_VS_STORE)
			trap->tinst = 0UL;
		بشکن;
	پیشفرض:
		بشکن;
	}

	بازگشت insn;
}
