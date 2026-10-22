/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_unpriv.h>

సంఘం sbi_unpriv_దత్తాంశం {
	u8 b;
	u16 h;
	u32 w;
#ఒకవేళ __riscv_xlen == 64
	u64 d;
#అయితే_ముగింపు
	u8 బైట్లు[__riscv_xlen / 8];
};

/**
 * a3 must a pointer to the sbi_trap_info and a4 is used as a temporary
 * register in the trap handler. Make sure that compiler doesn't use a3 & a4.
 */
#నిర్వచించు DEFINE_UNPRIVILEGED_ఎక్కించు_FUNCTION(type, insn)                         \
	type sbi_ఎక్కించు##type(స్థిరరాశి type *addr,                                \
			     నిర్మాణం sbi_trap_info *trap)                      \
	{                                                                     \
		నమోదు ulong tinfo asm("a3") = (ulong)trap;                 \
		నమోదు ulong mstatus = 0;                                   \
		నమోదు ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		type ret = 0;                                                 \
		trap->cause = 0;                                              \
		asm అస్థిర(                                                 \
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
		తిరిగి ret;                                                   \
	}

#నిర్వచించు DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                        \
	శూన్య sbi_store_##type(type *addr, type val,                           \
			      నిర్మాణం sbi_trap_info *trap)                     \
	{                                                                     \
		నమోదు ulong tinfo asm("a3") = (ulong)trap;                 \
		నమోదు ulong mstatus = 0;                                   \
		నమోదు ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		trap->cause = 0;                                              \
		asm అస్థిర(                                                 \
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

DEFINE_UNPRIVILEGED_ఎక్కించు_FUNCTION(u8, lbu)
DEFINE_UNPRIVILEGED_ఎక్కించు_FUNCTION(u16, lhu)
DEFINE_UNPRIVILEGED_ఎక్కించు_FUNCTION(s8, lb)
DEFINE_UNPRIVILEGED_ఎక్కించు_FUNCTION(s16, lh)
DEFINE_UNPRIVILEGED_ఎక్కించు_FUNCTION(s32, lw)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u8, sb)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u16, sh)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u32, sw)
#ఒకవేళ __riscv_xlen == 64
DEFINE_UNPRIVILEGED_ఎక్కించు_FUNCTION(u32, lwu)
DEFINE_UNPRIVILEGED_ఎక్కించు_FUNCTION(u64, ld)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, sd)
DEFINE_UNPRIVILEGED_ఎక్కించు_FUNCTION(ulong, ld)
#లేదావేళ __riscv_xlen == 32
DEFINE_UNPRIVILEGED_ఎక్కించు_FUNCTION(u32, lw)
DEFINE_UNPRIVILEGED_ఎక్కించు_FUNCTION(ulong, lw)

u64 sbi_ఎక్కించు_u64(స్థిరరాశి u64 *addr,
		 నిర్మాణం sbi_trap_info *trap)
{
	u64 ret = sbi_ఎక్కించు_u32((u32 *)addr, trap);

	ఒకవేళ (trap->cause)
		తిరిగి 0;
	ret |= ((u64)sbi_ఎక్కించు_u32((u32 *)addr + 1, trap) << 32);
	ఒకవేళ (trap->cause)
		తిరిగి 0;

	తిరిగి ret;
}

శూన్య sbi_store_u64(u64 *addr, u64 val,
		   నిర్మాణం sbi_trap_info *trap)
{
	sbi_store_u32((u32 *)addr, val, trap);
	ఒకవేళ (trap->cause)
		తిరిగి;

	sbi_store_u32((u32 *)addr + 1, val >> 32, trap);
	ఒకవేళ (trap->cause)
		తిరిగి;
}
#లేకపోతే
# error "Unexpected __riscv_xlen"
#అయితే_ముగింపు

శూన్య sbi_ఎక్కించు_loop(u8 *బఫరు, ulong addr, ulong len,
		   నిర్మాణం sbi_trap_info *trap)
{
	సంఘం sbi_unpriv_దత్తాంశం దత్తాంశం;

	trap->cause = 0;
	అయితే (len) {
		గుర్తులేని పూర్ణాంకం వెడల్పు = __riscv_xlen / 8;
		శూన్య *ptr = (శూన్య*)addr;

		అయితే (len < వెడల్పు || (addr & (వెడల్పు - 1)))
			వెడల్పు /= 2;

		ఎంపిక (వెడల్పు) {
		స్థితి 1:
			దత్తాంశం.b = sbi_ఎక్కించు_u8(ptr, trap);
			ఆపు;
		స్థితి 2:
			దత్తాంశం.h = sbi_ఎక్కించు_u16(ptr, trap);
			ఆపు;
		స్థితి 4:
			దత్తాంశం.w = sbi_ఎక్కించు_u32(ptr, trap);
			ఆపు;
#ఒకవేళ __riscv_xlen == 64
		స్థితి 8:
			దత్తాంశం.d = sbi_ఎక్కించు_u64(ptr, trap);
			ఆపు;
#అయితే_ముగింపు
		}
		ఒకవేళ (trap->cause)
			తిరిగి;

		sbi_memcpy(బఫరు, దత్తాంశం.బైట్లు, వెడల్పు);
		len -= వెడల్పు;
		addr += వెడల్పు;
		బఫరు += వెడల్పు;
	}
}

శూన్య sbi_store_loop(u8 *బఫరు, ulong addr, ulong len,
		    నిర్మాణం sbi_trap_info *trap)
{
	సంఘం sbi_unpriv_దత్తాంశం దత్తాంశం;

	trap->cause = 0;
	అయితే (len) {
		గుర్తులేని పూర్ణాంకం వెడల్పు = __riscv_xlen / 8;
		శూన్య *ptr = (శూన్య*)addr;

		అయితే (len < వెడల్పు || (addr & (వెడల్పు - 1)))
			వెడల్పు /= 2;

		sbi_memcpy(దత్తాంశం.బైట్లు, బఫరు, వెడల్పు);
		ఎంపిక (వెడల్పు) {
		స్థితి 1:
			sbi_store_u8(ptr, దత్తాంశం.b, trap);
			ఆపు;
		స్థితి 2:
			sbi_store_u16(ptr, దత్తాంశం.h, trap);
			ఆపు;
		స్థితి 4:
			sbi_store_u32(ptr, దత్తాంశం.w, trap);
			ఆపు;
#ఒకవేళ __riscv_xlen == 64
		స్థితి 8:
			sbi_store_u64(ptr, దత్తాంశం.d, trap);
			ఆపు;
#అయితే_ముగింపు
		}
		ఒకవేళ (trap->cause)
			తిరిగి;

		len -= వెడల్పు;
		addr += వెడల్పు;
		బఫరు += వెడల్పు;
	}
}

ulong sbi_get_insn(ulong mepc, నిర్మాణం sbi_trap_info *trap)
{
	నమోదు ulong tinfo asm("a3");
	నమోదు ulong ttmp asm("a4");
	నమోదు ulong mstatus = 0;
	నమోదు ulong mtvec = (ulong)sbi_hart_expected_trap;
	ulong insn = 0;

	trap->cause = 0;

	asm అస్థిర(
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

	ఎంపిక (trap->cause) {
	స్థితి CAUSE_ఎక్కించు_ACCESS:
		trap->cause = CAUSE_FETCH_ACCESS;
		trap->tinst = 0UL;
		ఆపు;
	స్థితి CAUSE_ఎక్కించు_పుట_FAULT:
		trap->cause = CAUSE_FETCH_పుట_FAULT;
		trap->tinst = 0UL;
		ఆపు;
	స్థితి CAUSE_ఎక్కించు_GUEST_పుట_FAULT:
		trap->cause = CAUSE_FETCH_GUEST_పుట_FAULT;
		ఒకవేళ (trap->tinst != INSN_PSEUDO_VS_ఎక్కించు &&
		    trap->tinst != INSN_PSEUDO_VS_STORE)
			trap->tinst = 0UL;
		ఆపు;
	అప్రమేయ:
		ఆపు;
	}

	తిరిగి insn;
}
