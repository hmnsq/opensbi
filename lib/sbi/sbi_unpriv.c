/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_unpriv.h>

اتحاد sbi_unpriv_داده {
	u8 b;
	u16 h;
	u32 w;
#إذا __riscv_xlen == 64
	u64 d;
#پایان_اگر
	u8 بایتها[__riscv_xlen / 8];
};

/**
 * a3 must a pointer to the sbi_trap_info and a4 is used as a temporary
 * register in the trap handler. Make sure that compiler doesn't use a3 & a4.
 */
#تعریف_کن DEFINE_UNPRIVILEGED_بارگیری_FUNCTION(type, insn)                         \
	type sbi_بارگیری##type(ثابت type *addr,                                \
			     بنية sbi_trap_info *trap)                      \
	{                                                                     \
		سجل ulong tinfo asm("a3") = (ulong)trap;                 \
		سجل ulong mstatus = 0;                                   \
		سجل ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		type ret = 0;                                                 \
		trap->cause = 0;                                              \
		asm متطاير(                                                 \
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
		أرجع ret;                                                   \
	}

#تعریف_کن DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                        \
	فراغ sbi_store_##type(type *addr, type val,                           \
			      بنية sbi_trap_info *trap)                     \
	{                                                                     \
		سجل ulong tinfo asm("a3") = (ulong)trap;                 \
		سجل ulong mstatus = 0;                                   \
		سجل ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		trap->cause = 0;                                              \
		asm متطاير(                                                 \
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

DEFINE_UNPRIVILEGED_بارگیری_FUNCTION(u8, lbu)
DEFINE_UNPRIVILEGED_بارگیری_FUNCTION(u16, lhu)
DEFINE_UNPRIVILEGED_بارگیری_FUNCTION(s8, lb)
DEFINE_UNPRIVILEGED_بارگیری_FUNCTION(s16, lh)
DEFINE_UNPRIVILEGED_بارگیری_FUNCTION(s32, lw)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u8, sb)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u16, sh)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u32, sw)
#إذا __riscv_xlen == 64
DEFINE_UNPRIVILEGED_بارگیری_FUNCTION(u32, lwu)
DEFINE_UNPRIVILEGED_بارگیری_FUNCTION(u64, ld)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, sd)
DEFINE_UNPRIVILEGED_بارگیری_FUNCTION(ulong, ld)
#وإلاإذا __riscv_xlen == 32
DEFINE_UNPRIVILEGED_بارگیری_FUNCTION(u32, lw)
DEFINE_UNPRIVILEGED_بارگیری_FUNCTION(ulong, lw)

u64 sbi_بارگیری_u64(ثابت u64 *addr,
		 بنية sbi_trap_info *trap)
{
	u64 ret = sbi_بارگیری_u32((u32 *)addr, trap);

	إذا (trap->cause)
		أرجع 0;
	ret |= ((u64)sbi_بارگیری_u32((u32 *)addr + 1, trap) << 32);
	إذا (trap->cause)
		أرجع 0;

	أرجع ret;
}

فراغ sbi_store_u64(u64 *addr, u64 val,
		   بنية sbi_trap_info *trap)
{
	sbi_store_u32((u32 *)addr, val, trap);
	إذا (trap->cause)
		أرجع;

	sbi_store_u32((u32 *)addr + 1, val >> 32, trap);
	إذا (trap->cause)
		أرجع;
}
#وإلا
# error "Unexpected __riscv_xlen"
#پایان_اگر

فراغ sbi_بارگیری_loop(u8 *میانگیر, ulong addr, ulong len,
		   بنية sbi_trap_info *trap)
{
	اتحاد sbi_unpriv_داده داده;

	trap->cause = 0;
	طالما (len) {
		بیعلامت صحيح عرض = __riscv_xlen / 8;
		فراغ *ptr = (فراغ*)addr;

		طالما (len < عرض || (addr & (عرض - 1)))
			عرض /= 2;

		بدل (عرض) {
		حالة 1:
			داده.b = sbi_بارگیری_u8(ptr, trap);
			اكسر;
		حالة 2:
			داده.h = sbi_بارگیری_u16(ptr, trap);
			اكسر;
		حالة 4:
			داده.w = sbi_بارگیری_u32(ptr, trap);
			اكسر;
#إذا __riscv_xlen == 64
		حالة 8:
			داده.d = sbi_بارگیری_u64(ptr, trap);
			اكسر;
#پایان_اگر
		}
		إذا (trap->cause)
			أرجع;

		sbi_memcpy(میانگیر, داده.بایتها, عرض);
		len -= عرض;
		addr += عرض;
		میانگیر += عرض;
	}
}

فراغ sbi_store_loop(u8 *میانگیر, ulong addr, ulong len,
		    بنية sbi_trap_info *trap)
{
	اتحاد sbi_unpriv_داده داده;

	trap->cause = 0;
	طالما (len) {
		بیعلامت صحيح عرض = __riscv_xlen / 8;
		فراغ *ptr = (فراغ*)addr;

		طالما (len < عرض || (addr & (عرض - 1)))
			عرض /= 2;

		sbi_memcpy(داده.بایتها, میانگیر, عرض);
		بدل (عرض) {
		حالة 1:
			sbi_store_u8(ptr, داده.b, trap);
			اكسر;
		حالة 2:
			sbi_store_u16(ptr, داده.h, trap);
			اكسر;
		حالة 4:
			sbi_store_u32(ptr, داده.w, trap);
			اكسر;
#إذا __riscv_xlen == 64
		حالة 8:
			sbi_store_u64(ptr, داده.d, trap);
			اكسر;
#پایان_اگر
		}
		إذا (trap->cause)
			أرجع;

		len -= عرض;
		addr += عرض;
		میانگیر += عرض;
	}
}

ulong sbi_get_insn(ulong mepc, بنية sbi_trap_info *trap)
{
	سجل ulong tinfo asm("a3");
	سجل ulong ttmp asm("a4");
	سجل ulong mstatus = 0;
	سجل ulong mtvec = (ulong)sbi_hart_expected_trap;
	ulong insn = 0;

	trap->cause = 0;

	asm متطاير(
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

	بدل (trap->cause) {
	حالة CAUSE_بارگیری_ACCESS:
		trap->cause = CAUSE_FETCH_ACCESS;
		trap->tinst = 0UL;
		اكسر;
	حالة CAUSE_بارگیری_صفحه_FAULT:
		trap->cause = CAUSE_FETCH_صفحه_FAULT;
		trap->tinst = 0UL;
		اكسر;
	حالة CAUSE_بارگیری_GUEST_صفحه_FAULT:
		trap->cause = CAUSE_FETCH_GUEST_صفحه_FAULT;
		إذا (trap->tinst != INSN_PSEUDO_VS_بارگیری &&
		    trap->tinst != INSN_PSEUDO_VS_STORE)
			trap->tinst = 0UL;
		اكسر;
	افتراضي:
		اكسر;
	}

	أرجع insn;
}
