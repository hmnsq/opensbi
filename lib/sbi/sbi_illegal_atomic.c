/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 MIPS
 *
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_illegal_atomic.h>
#ضمن <sbi/sbi_illegal_insn.h>

#إذا !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#تېروتنه "opensbi strongly relies on the Zaamo or Zalrsc extension of RISC-V"
#د_که_پای

#إذا defined(__riscv_atomic) || defined(__riscv_zaamo)

صحيح sbi_illegal_atomic(ulong insn, بنية sbi_trap_regs *regs)
{
	أرجع truly_illegal_insn(insn, regs);
}

#وإلاإذا defined(__riscv_zalrsc)

#تعریف_کړه DEFINE_UNPRIVILEGED_LR_FUNCTION(type, aqrl, insn)			\
	ساكن type lr_##type##aqrl(ثابت type *addr,				\
				بنية sbi_trap_info *trap)			\
	{									\
		سجل ulong tinfo asm("a3");					\
		سجل ulong mstatus = 0;					\
		سجل ulong mtvec = (ulong)sbi_hart_expected_trap;		\
		type ret = 0;							\
		trap->cause = 0;						\
		asm متطاير(							\
			"add %[tinfo], %[taddr], zero\n"			\
			"csrrw %[mtvec], " STR(CSR_MTVEC) ", %[mtvec]\n"	\
			"csrrs %[mstatus], " STR(CSR_MSTATUS) ", %[mprv]\n"	\
			".option push\n"					\
			".option norvc\n"					\
			#insn " %[ret], %[addr]\n"				\
			".option pop\n"						\
			"csrw " STR(CSR_MSTATUS) ", %[mstatus]\n"		\
			"csrw " STR(CSR_MTVEC) ", %[mtvec]"			\
		    : [mstatus] "+&r"(mstatus), [mtvec] "+&r"(mtvec),		\
		      [tinfo] "+&r"(tinfo), [ret] "=&r"(ret)			\
		    : [addr] "m"(*addr), [mprv] "r"(MSTATUS_MPRV),		\
		      [taddr] "r"((ulong)trap)					\
		    : "a4", "memory");						\
		أرجع ret;							\
	}

#تعریف_کړه DEFINE_UNPRIVILEGED_SC_FUNCTION(type, aqrl, insn)			\
	ساكن type sc_##type##aqrl(type *addr, type val,			\
				بنية sbi_trap_info *trap)			\
	{									\
		سجل ulong tinfo asm("a3");					\
		سجل ulong mstatus = 0;					\
		سجل ulong mtvec = (ulong)sbi_hart_expected_trap;		\
		type ret = 0;							\
		trap->cause = 0;						\
		asm متطاير(							\
			"add %[tinfo], %[taddr], zero\n"			\
			"csrrw %[mtvec], " STR(CSR_MTVEC) ", %[mtvec]\n"	\
			"csrrs %[mstatus], " STR(CSR_MSTATUS) ", %[mprv]\n"	\
			".option push\n"					\
			".option norvc\n"					\
			#insn " %[ret], %[val], %[addr]\n"			\
			".option pop\n"						\
			"csrw " STR(CSR_MSTATUS) ", %[mstatus]\n"		\
			"csrw " STR(CSR_MTVEC) ", %[mtvec]"			\
		    : [mstatus] "+&r"(mstatus), [mtvec] "+&r"(mtvec),		\
		      [tinfo] "+&r"(tinfo), [ret] "=&r"(ret)			\
		    : [addr] "m"(*addr), [mprv] "r"(MSTATUS_MPRV),		\
		      [val] "r"(val), [taddr] "r"((ulong)trap)			\
		    : "a4", "memory");						\
		أرجع ret;							\
	}

DEFINE_UNPRIVILEGED_LR_FUNCTION(s32, , lr.w);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s32, _aq, lr.w.aq);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s32, _rl, lr.w.rl);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s32, _aqrl, lr.w.aqrl);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s32, , sc.w);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s32, _aq, sc.w.aq);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s32, _rl, sc.w.rl);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s32, _aqrl, sc.w.aqrl);
#إذا __riscv_xlen == 64
DEFINE_UNPRIVILEGED_LR_FUNCTION(s64, , lr.d);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s64, _aq, lr.d.aq);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s64, _rl, lr.d.rl);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s64, _aqrl, lr.d.aqrl);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s64, , sc.d);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s64, _aq, sc.d.aq);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s64, _rl, sc.d.rl);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s64, _aqrl, sc.d.aqrl);
#د_که_پای

#تعریف_کړه DEFINE_ATOMIC_FUNCTION(name, type, func)				\
	ساكن صحيح atomic_##name(ulong insn, بنية sbi_trap_regs *regs)	\
	{									\
		بنية sbi_trap_info uptrap;					\
		ulong addr = GET_RS1(insn, regs);				\
		ulong val = GET_RS2(insn, regs);				\
		ulong rd_val = 0;						\
		ulong fail = 1;							\
		طالما (fail) {							\
			rd_val = lr_##type((فراغ *)addr, &uptrap);		\
			إذا (uptrap.cause) {					\
				أرجع sbi_trap_redirect(regs, &uptrap);	\
			}							\
			fail = sc_##type((فراغ *)addr, func, &uptrap);	\
			إذا (uptrap.cause) {					\
				أرجع sbi_trap_redirect(regs, &uptrap);	\
			}							\
		}								\
		سېټ_RD(insn, regs, rd_val);					\
		regs->mepc += 4;						\
		أرجع 0;							\
	}

DEFINE_ATOMIC_FUNCTION(جمع_کړه_w, s32, rd_val + val);
DEFINE_ATOMIC_FUNCTION(جمع_کړه_w_aq, s32_aq, rd_val + val);
DEFINE_ATOMIC_FUNCTION(جمع_کړه_w_rl, s32_rl, rd_val + val);
DEFINE_ATOMIC_FUNCTION(جمع_کړه_w_aqrl, s32_aqrl, rd_val + val);
DEFINE_ATOMIC_FUNCTION(and_w, s32, rd_val & val);
DEFINE_ATOMIC_FUNCTION(and_w_aq, s32_aq, rd_val & val);
DEFINE_ATOMIC_FUNCTION(and_w_rl, s32_rl, rd_val & val);
DEFINE_ATOMIC_FUNCTION(and_w_aqrl, s32_aqrl, rd_val & val);
DEFINE_ATOMIC_FUNCTION(or_w, s32, rd_val | val);
DEFINE_ATOMIC_FUNCTION(or_w_aq, s32_aq, rd_val | val);
DEFINE_ATOMIC_FUNCTION(or_w_rl, s32_rl, rd_val | val);
DEFINE_ATOMIC_FUNCTION(or_w_aqrl, s32_aqrl, rd_val | val);
DEFINE_ATOMIC_FUNCTION(xor_w, s32, rd_val ^ val);
DEFINE_ATOMIC_FUNCTION(xor_w_aq, s32_aq, rd_val ^ val);
DEFINE_ATOMIC_FUNCTION(xor_w_rl, s32_rl, rd_val ^ val);
DEFINE_ATOMIC_FUNCTION(xor_w_aqrl, s32_aqrl, rd_val ^ val);
DEFINE_ATOMIC_FUNCTION(بدلون_w, s32, val);
DEFINE_ATOMIC_FUNCTION(بدلون_w_aq, s32_aq, val);
DEFINE_ATOMIC_FUNCTION(بدلون_w_rl, s32_rl, val);
DEFINE_ATOMIC_FUNCTION(بدلون_w_aqrl, s32_aqrl, val);
DEFINE_ATOMIC_FUNCTION(max_w, s32, (s32)rd_val > (s32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(max_w_aq, s32_aq, (s32)rd_val > (s32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(max_w_rl, s32_rl, (s32)rd_val > (s32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(max_w_aqrl, s32_aqrl, (s32)rd_val > (s32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(maxu_w, s32, (u32)rd_val > (u32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(maxu_w_aq, s32_aq, (u32)rd_val > (u32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(maxu_w_rl, s32_rl, (u32)rd_val > (u32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(maxu_w_aqrl, s32_aqrl, (u32)rd_val > (u32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(min_w, s32, (s32)rd_val < (s32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(min_w_aq, s32_aq, (s32)rd_val < (s32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(min_w_rl, s32_rl, (s32)rd_val < (s32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(min_w_aqrl, s32_aqrl, (s32)rd_val < (s32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(minu_w, s32, (u32)rd_val < (u32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(minu_w_aq, s32_aq, (u32)rd_val < (u32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(minu_w_rl, s32_rl, (u32)rd_val < (u32)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(minu_w_aqrl, s32_aqrl, (u32)rd_val < (u32)val ? rd_val : val);

#إذا __riscv_xlen == 64
DEFINE_ATOMIC_FUNCTION(جمع_کړه_d, s64, rd_val + val);
DEFINE_ATOMIC_FUNCTION(جمع_کړه_d_aq, s64_aq, rd_val + val);
DEFINE_ATOMIC_FUNCTION(جمع_کړه_d_rl, s64_rl, rd_val + val);
DEFINE_ATOMIC_FUNCTION(جمع_کړه_d_aqrl, s64_aqrl, rd_val + val);
DEFINE_ATOMIC_FUNCTION(and_d, s64, rd_val & val);
DEFINE_ATOMIC_FUNCTION(and_d_aq, s64_aq, rd_val & val);
DEFINE_ATOMIC_FUNCTION(and_d_rl, s64_rl, rd_val & val);
DEFINE_ATOMIC_FUNCTION(and_d_aqrl, s64_aqrl, rd_val & val);
DEFINE_ATOMIC_FUNCTION(or_d, s64, rd_val | val);
DEFINE_ATOMIC_FUNCTION(or_d_aq, s64_aq, rd_val | val);
DEFINE_ATOMIC_FUNCTION(or_d_rl, s64_rl, rd_val | val);
DEFINE_ATOMIC_FUNCTION(or_d_aqrl, s64_aqrl, rd_val | val);
DEFINE_ATOMIC_FUNCTION(xor_d, s64, rd_val ^ val);
DEFINE_ATOMIC_FUNCTION(xor_d_aq, s64_aq, rd_val ^ val);
DEFINE_ATOMIC_FUNCTION(xor_d_rl, s64_rl, rd_val ^ val);
DEFINE_ATOMIC_FUNCTION(xor_d_aqrl, s64_aqrl, rd_val ^ val);
DEFINE_ATOMIC_FUNCTION(بدلون_d, s64, val);
DEFINE_ATOMIC_FUNCTION(بدلون_d_aq, s64_aq, val);
DEFINE_ATOMIC_FUNCTION(بدلون_d_rl, s64_rl, val);
DEFINE_ATOMIC_FUNCTION(بدلون_d_aqrl, s64_aqrl, val);
DEFINE_ATOMIC_FUNCTION(max_d, s64, (s64)rd_val > (s64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(max_d_aq, s64_aq, (s64)rd_val > (s64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(max_d_rl, s64_rl, (s64)rd_val > (s64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(max_d_aqrl, s64_aqrl, (s64)rd_val > (s64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(maxu_d, s64, (u64)rd_val > (u64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(maxu_d_aq, s64_aq, (u64)rd_val > (u64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(maxu_d_rl, s64_rl, (u64)rd_val > (u64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(maxu_d_aqrl, s64_aqrl, (u64)rd_val > (u64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(min_d, s64, (s64)rd_val < (s64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(min_d_aq, s64_aq, (s64)rd_val < (s64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(min_d_rl, s64_rl, (s64)rd_val < (s64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(min_d_aqrl, s64_aqrl, (s64)rd_val < (s64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(minu_d, s64, (u64)rd_val < (u64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(minu_d_aq, s64_aq, (u64)rd_val < (u64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(minu_d_rl, s64_rl, (u64)rd_val < (u64)val ? rd_val : val);
DEFINE_ATOMIC_FUNCTION(minu_d_aqrl, s64_aqrl, (u64)rd_val < (u64)val ? rd_val : val);
#د_که_پای

ساكن ثابت illegal_insn_func amoadd_جدول[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	atomic_جمع_کړه_w, /* 8 */
	atomic_جمع_کړه_w_rl, /* 9 */
	atomic_جمع_کړه_w_aq, /* 10 */
	atomic_جمع_کړه_w_aqrl, /* 11 */
#إذا __riscv_xlen == 64
	atomic_جمع_کړه_d, /* 12 */
	atomic_جمع_کړه_d_rl, /* 13 */
	atomic_جمع_کړه_d_aq, /* 14 */
	atomic_جمع_کړه_d_aqrl, /* 15 */
#وإلا
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#د_که_پای
	truly_illegal_insn, /* 16 */
	truly_illegal_insn, /* 17 */
	truly_illegal_insn, /* 18 */
	truly_illegal_insn, /* 19 */
	truly_illegal_insn, /* 20 */
	truly_illegal_insn, /* 21 */
	truly_illegal_insn, /* 22 */
	truly_illegal_insn, /* 23 */
	truly_illegal_insn, /* 24 */
	truly_illegal_insn, /* 25 */
	truly_illegal_insn, /* 26 */
	truly_illegal_insn, /* 27 */
	truly_illegal_insn, /* 28 */
	truly_illegal_insn, /* 29 */
	truly_illegal_insn, /* 30 */
	truly_illegal_insn, /* 31 */
};

ساكن ثابت illegal_insn_func amoswap_جدول[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	atomic_بدلون_w, /* 8 */
	atomic_بدلون_w_rl, /* 9 */
	atomic_بدلون_w_aq, /* 10 */
	atomic_بدلون_w_aqrl, /* 11 */
#إذا __riscv_xlen == 64
	atomic_بدلون_d, /* 12 */
	atomic_بدلون_d_rl, /* 13 */
	atomic_بدلون_d_aq, /* 14 */
	atomic_بدلون_d_aqrl, /* 15 */
#وإلا
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#د_که_پای
	truly_illegal_insn, /* 16 */
	truly_illegal_insn, /* 17 */
	truly_illegal_insn, /* 18 */
	truly_illegal_insn, /* 19 */
	truly_illegal_insn, /* 20 */
	truly_illegal_insn, /* 21 */
	truly_illegal_insn, /* 22 */
	truly_illegal_insn, /* 23 */
	truly_illegal_insn, /* 24 */
	truly_illegal_insn, /* 25 */
	truly_illegal_insn, /* 26 */
	truly_illegal_insn, /* 27 */
	truly_illegal_insn, /* 28 */
	truly_illegal_insn, /* 29 */
	truly_illegal_insn, /* 30 */
	truly_illegal_insn, /* 31 */
};

ساكن ثابت illegal_insn_func amoxor_جدول[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	atomic_xor_w, /* 8 */
	atomic_xor_w_rl, /* 9 */
	atomic_xor_w_aq, /* 10 */
	atomic_xor_w_aqrl, /* 11 */
#إذا __riscv_xlen == 64
	atomic_xor_d, /* 12 */
	atomic_xor_d_rl, /* 13 */
	atomic_xor_d_aq, /* 14 */
	atomic_xor_d_aqrl, /* 15 */
#وإلا
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#د_که_پای
	truly_illegal_insn, /* 16 */
	truly_illegal_insn, /* 17 */
	truly_illegal_insn, /* 18 */
	truly_illegal_insn, /* 19 */
	truly_illegal_insn, /* 20 */
	truly_illegal_insn, /* 21 */
	truly_illegal_insn, /* 22 */
	truly_illegal_insn, /* 23 */
	truly_illegal_insn, /* 24 */
	truly_illegal_insn, /* 25 */
	truly_illegal_insn, /* 26 */
	truly_illegal_insn, /* 27 */
	truly_illegal_insn, /* 28 */
	truly_illegal_insn, /* 29 */
	truly_illegal_insn, /* 30 */
	truly_illegal_insn, /* 31 */
};

ساكن ثابت illegal_insn_func amoor_جدول[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	atomic_or_w, /* 8 */
	atomic_or_w_rl, /* 9 */
	atomic_or_w_aq, /* 10 */
	atomic_or_w_aqrl, /* 11 */
#إذا __riscv_xlen == 64
	atomic_or_d, /* 12 */
	atomic_or_d_rl, /* 13 */
	atomic_or_d_aq, /* 14 */
	atomic_or_d_aqrl, /* 15 */
#وإلا
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#د_که_پای
	truly_illegal_insn, /* 16 */
	truly_illegal_insn, /* 17 */
	truly_illegal_insn, /* 18 */
	truly_illegal_insn, /* 19 */
	truly_illegal_insn, /* 20 */
	truly_illegal_insn, /* 21 */
	truly_illegal_insn, /* 22 */
	truly_illegal_insn, /* 23 */
	truly_illegal_insn, /* 24 */
	truly_illegal_insn, /* 25 */
	truly_illegal_insn, /* 26 */
	truly_illegal_insn, /* 27 */
	truly_illegal_insn, /* 28 */
	truly_illegal_insn, /* 29 */
	truly_illegal_insn, /* 30 */
	truly_illegal_insn, /* 31 */
};

ساكن ثابت illegal_insn_func amoand_جدول[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	atomic_and_w, /* 8 */
	atomic_and_w_rl, /* 9 */
	atomic_and_w_aq, /* 10 */
	atomic_and_w_aqrl, /* 11 */
#إذا __riscv_xlen == 64
	atomic_and_d, /* 12 */
	atomic_and_d_rl, /* 13 */
	atomic_and_d_aq, /* 14 */
	atomic_and_d_aqrl, /* 15 */
#وإلا
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#د_که_پای
	truly_illegal_insn, /* 16 */
	truly_illegal_insn, /* 17 */
	truly_illegal_insn, /* 18 */
	truly_illegal_insn, /* 19 */
	truly_illegal_insn, /* 20 */
	truly_illegal_insn, /* 21 */
	truly_illegal_insn, /* 22 */
	truly_illegal_insn, /* 23 */
	truly_illegal_insn, /* 24 */
	truly_illegal_insn, /* 25 */
	truly_illegal_insn, /* 26 */
	truly_illegal_insn, /* 27 */
	truly_illegal_insn, /* 28 */
	truly_illegal_insn, /* 29 */
	truly_illegal_insn, /* 30 */
	truly_illegal_insn, /* 31 */
};

ساكن ثابت illegal_insn_func amomin_جدول[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	atomic_min_w, /* 8 */
	atomic_min_w_rl, /* 9 */
	atomic_min_w_aq, /* 10 */
	atomic_min_w_aqrl, /* 11 */
#إذا __riscv_xlen == 64
	atomic_min_d, /* 12 */
	atomic_min_d_rl, /* 13 */
	atomic_min_d_aq, /* 14 */
	atomic_min_d_aqrl, /* 15 */
#وإلا
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#د_که_پای
	truly_illegal_insn, /* 16 */
	truly_illegal_insn, /* 17 */
	truly_illegal_insn, /* 18 */
	truly_illegal_insn, /* 19 */
	truly_illegal_insn, /* 20 */
	truly_illegal_insn, /* 21 */
	truly_illegal_insn, /* 22 */
	truly_illegal_insn, /* 23 */
	truly_illegal_insn, /* 24 */
	truly_illegal_insn, /* 25 */
	truly_illegal_insn, /* 26 */
	truly_illegal_insn, /* 27 */
	truly_illegal_insn, /* 28 */
	truly_illegal_insn, /* 29 */
	truly_illegal_insn, /* 30 */
	truly_illegal_insn, /* 31 */
};

ساكن ثابت illegal_insn_func amomax_جدول[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	atomic_max_w, /* 8 */
	atomic_max_w_rl, /* 9 */
	atomic_max_w_aq, /* 10 */
	atomic_max_w_aqrl, /* 11 */
#إذا __riscv_xlen == 64
	atomic_max_d, /* 12 */
	atomic_max_d_rl, /* 13 */
	atomic_max_d_aq, /* 14 */
	atomic_max_d_aqrl, /* 15 */
#وإلا
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#د_که_پای
	truly_illegal_insn, /* 16 */
	truly_illegal_insn, /* 17 */
	truly_illegal_insn, /* 18 */
	truly_illegal_insn, /* 19 */
	truly_illegal_insn, /* 20 */
	truly_illegal_insn, /* 21 */
	truly_illegal_insn, /* 22 */
	truly_illegal_insn, /* 23 */
	truly_illegal_insn, /* 24 */
	truly_illegal_insn, /* 25 */
	truly_illegal_insn, /* 26 */
	truly_illegal_insn, /* 27 */
	truly_illegal_insn, /* 28 */
	truly_illegal_insn, /* 29 */
	truly_illegal_insn, /* 30 */
	truly_illegal_insn, /* 31 */
};

ساكن ثابت illegal_insn_func amominu_جدول[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	atomic_minu_w, /* 8 */
	atomic_minu_w_rl, /* 9 */
	atomic_minu_w_aq, /* 10 */
	atomic_minu_w_aqrl, /* 11 */
#إذا __riscv_xlen == 64
	atomic_minu_d, /* 12 */
	atomic_minu_d_rl, /* 13 */
	atomic_minu_d_aq, /* 14 */
	atomic_minu_d_aqrl, /* 15 */
#وإلا
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#د_که_پای
	truly_illegal_insn, /* 16 */
	truly_illegal_insn, /* 17 */
	truly_illegal_insn, /* 18 */
	truly_illegal_insn, /* 19 */
	truly_illegal_insn, /* 20 */
	truly_illegal_insn, /* 21 */
	truly_illegal_insn, /* 22 */
	truly_illegal_insn, /* 23 */
	truly_illegal_insn, /* 24 */
	truly_illegal_insn, /* 25 */
	truly_illegal_insn, /* 26 */
	truly_illegal_insn, /* 27 */
	truly_illegal_insn, /* 28 */
	truly_illegal_insn, /* 29 */
	truly_illegal_insn, /* 30 */
	truly_illegal_insn, /* 31 */
};

ساكن ثابت illegal_insn_func amomaxu_جدول[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	atomic_maxu_w, /* 8 */
	atomic_maxu_w_rl, /* 9 */
	atomic_maxu_w_aq, /* 10 */
	atomic_maxu_w_aqrl, /* 11 */
#إذا __riscv_xlen == 64
	atomic_maxu_d, /* 12 */
	atomic_maxu_d_rl, /* 13 */
	atomic_maxu_d_aq, /* 14 */
	atomic_maxu_d_aqrl, /* 15 */
#وإلا
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#د_که_پای
	truly_illegal_insn, /* 16 */
	truly_illegal_insn, /* 17 */
	truly_illegal_insn, /* 18 */
	truly_illegal_insn, /* 19 */
	truly_illegal_insn, /* 20 */
	truly_illegal_insn, /* 21 */
	truly_illegal_insn, /* 22 */
	truly_illegal_insn, /* 23 */
	truly_illegal_insn, /* 24 */
	truly_illegal_insn, /* 25 */
	truly_illegal_insn, /* 26 */
	truly_illegal_insn, /* 27 */
	truly_illegal_insn, /* 28 */
	truly_illegal_insn, /* 29 */
	truly_illegal_insn, /* 30 */
	truly_illegal_insn, /* 31 */
};

ساكن صحيح amoadd_insn(ulong insn, بنية sbi_trap_regs *regs)
{
	أرجع amoadd_جدول[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

ساكن صحيح amoswap_insn(ulong insn, بنية sbi_trap_regs *regs)
{
	أرجع amoswap_جدول[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

ساكن صحيح amoxor_insn(ulong insn, بنية sbi_trap_regs *regs)
{
	أرجع amoxor_جدول[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

ساكن صحيح amoor_insn(ulong insn, بنية sbi_trap_regs *regs)
{
	أرجع amoor_جدول[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

ساكن صحيح amoand_insn(ulong insn, بنية sbi_trap_regs *regs)
{
	أرجع amoand_جدول[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

ساكن صحيح amomin_insn(ulong insn, بنية sbi_trap_regs *regs)
{
	أرجع amomin_جدول[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

ساكن صحيح amomax_insn(ulong insn, بنية sbi_trap_regs *regs)
{
	أرجع amomax_جدول[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

ساكن صحيح amominu_insn(ulong insn, بنية sbi_trap_regs *regs)
{
	أرجع amominu_جدول[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

ساكن صحيح amomaxu_insn(ulong insn, بنية sbi_trap_regs *regs)
{
	أرجع amomaxu_جدول[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

ساكن ثابت illegal_insn_func amo_insn_جدول[32] = {
	amoadd_insn, /* 0 */
	amoswap_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	amoxor_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	amoor_insn, /* 8 */
	truly_illegal_insn, /* 9 */
	truly_illegal_insn, /* 10 */
	truly_illegal_insn, /* 11 */
	amoand_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
	amomin_insn, /* 16 */
	truly_illegal_insn, /* 17 */
	truly_illegal_insn, /* 18 */
	truly_illegal_insn, /* 19 */
	amomax_insn, /* 20 */
	truly_illegal_insn, /* 21 */
	truly_illegal_insn, /* 22 */
	truly_illegal_insn, /* 23 */
	amominu_insn, /* 24 */
	truly_illegal_insn, /* 25 */
	truly_illegal_insn, /* 26 */
	truly_illegal_insn, /* 27 */
	amomaxu_insn, /* 28 */
	truly_illegal_insn, /* 29 */
	truly_illegal_insn, /* 30 */
	truly_illegal_insn  /* 31 */
};

صحيح sbi_illegal_atomic(ulong insn, بنية sbi_trap_regs *regs)
{
	أرجع amo_insn_جدول[(insn >> 27) & 0x1f](insn, regs);
}

#وإلا
#تېروتنه "need a or zalrsc"
#د_که_پای
