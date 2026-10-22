/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 MIPS
 *
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_illegal_atomic.h>
#అన్తర్భావ <sbi/sbi_illegal_insn.h>

#ఒకవేళ !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#దోషం "opensbi strongly relies on the Zaamo or Zalrsc extension of RISC-V"
#అయితే_ముగింపు

#ఒకవేళ defined(__riscv_atomic) || defined(__riscv_zaamo)

పూర్ణాంకం sbi_illegal_atomic(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	తిరిగి truly_illegal_insn(insn, regs);
}

#లేదావేళ defined(__riscv_zalrsc)

#నిర్వచించు DEFINE_UNPRIVILEGED_LR_FUNCTION(type, aqrl, insn)			\
	స్థిర type lr_##type##aqrl(స్థిరరాశి type *addr,				\
				నిర్మాణం sbi_trap_info *trap)			\
	{									\
		నమోదు ulong tinfo asm("a3");					\
		నమోదు ulong mstatus = 0;					\
		నమోదు ulong mtvec = (ulong)sbi_hart_expected_trap;		\
		type ret = 0;							\
		trap->cause = 0;						\
		asm అస్థిర(							\
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
		తిరిగి ret;							\
	}

#నిర్వచించు DEFINE_UNPRIVILEGED_SC_FUNCTION(type, aqrl, insn)			\
	స్థిర type sc_##type##aqrl(type *addr, type val,			\
				నిర్మాణం sbi_trap_info *trap)			\
	{									\
		నమోదు ulong tinfo asm("a3");					\
		నమోదు ulong mstatus = 0;					\
		నమోదు ulong mtvec = (ulong)sbi_hart_expected_trap;		\
		type ret = 0;							\
		trap->cause = 0;						\
		asm అస్థిర(							\
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
		తిరిగి ret;							\
	}

DEFINE_UNPRIVILEGED_LR_FUNCTION(s32, , lr.w);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s32, _aq, lr.w.aq);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s32, _rl, lr.w.rl);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s32, _aqrl, lr.w.aqrl);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s32, , sc.w);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s32, _aq, sc.w.aq);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s32, _rl, sc.w.rl);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s32, _aqrl, sc.w.aqrl);
#ఒకవేళ __riscv_xlen == 64
DEFINE_UNPRIVILEGED_LR_FUNCTION(s64, , lr.d);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s64, _aq, lr.d.aq);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s64, _rl, lr.d.rl);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s64, _aqrl, lr.d.aqrl);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s64, , sc.d);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s64, _aq, sc.d.aq);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s64, _rl, sc.d.rl);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s64, _aqrl, sc.d.aqrl);
#అయితే_ముగింపు

#నిర్వచించు DEFINE_ATOMIC_FUNCTION(name, type, func)				\
	స్థిర పూర్ణాంకం atomic_##name(ulong insn, నిర్మాణం sbi_trap_regs *regs)	\
	{									\
		నిర్మాణం sbi_trap_info uptrap;					\
		ulong addr = GET_RS1(insn, regs);				\
		ulong val = GET_RS2(insn, regs);				\
		ulong rd_val = 0;						\
		ulong fail = 1;							\
		అయితే (fail) {							\
			rd_val = lr_##type((శూన్య *)addr, &uptrap);		\
			ఒకవేళ (uptrap.cause) {					\
				తిరిగి sbi_trap_redirect(regs, &uptrap);	\
			}							\
			fail = sc_##type((శూన్య *)addr, func, &uptrap);	\
			ఒకవేళ (uptrap.cause) {					\
				తిరిగి sbi_trap_redirect(regs, &uptrap);	\
			}							\
		}								\
		సమితి_RD(insn, regs, rd_val);					\
		regs->mepc += 4;						\
		తిరిగి 0;							\
	}

DEFINE_ATOMIC_FUNCTION(కూడు_w, s32, rd_val + val);
DEFINE_ATOMIC_FUNCTION(కూడు_w_aq, s32_aq, rd_val + val);
DEFINE_ATOMIC_FUNCTION(కూడు_w_rl, s32_rl, rd_val + val);
DEFINE_ATOMIC_FUNCTION(కూడు_w_aqrl, s32_aqrl, rd_val + val);
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
DEFINE_ATOMIC_FUNCTION(మార్పిడి_w, s32, val);
DEFINE_ATOMIC_FUNCTION(మార్పిడి_w_aq, s32_aq, val);
DEFINE_ATOMIC_FUNCTION(మార్పిడి_w_rl, s32_rl, val);
DEFINE_ATOMIC_FUNCTION(మార్పిడి_w_aqrl, s32_aqrl, val);
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

#ఒకవేళ __riscv_xlen == 64
DEFINE_ATOMIC_FUNCTION(కూడు_d, s64, rd_val + val);
DEFINE_ATOMIC_FUNCTION(కూడు_d_aq, s64_aq, rd_val + val);
DEFINE_ATOMIC_FUNCTION(కూడు_d_rl, s64_rl, rd_val + val);
DEFINE_ATOMIC_FUNCTION(కూడు_d_aqrl, s64_aqrl, rd_val + val);
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
DEFINE_ATOMIC_FUNCTION(మార్పిడి_d, s64, val);
DEFINE_ATOMIC_FUNCTION(మార్పిడి_d_aq, s64_aq, val);
DEFINE_ATOMIC_FUNCTION(మార్పిడి_d_rl, s64_rl, val);
DEFINE_ATOMIC_FUNCTION(మార్పిడి_d_aqrl, s64_aqrl, val);
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
#అయితే_ముగింపు

స్థిర స్థిరరాశి illegal_insn_func amoadd_పట్టిక[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	atomic_కూడు_w, /* 8 */
	atomic_కూడు_w_rl, /* 9 */
	atomic_కూడు_w_aq, /* 10 */
	atomic_కూడు_w_aqrl, /* 11 */
#ఒకవేళ __riscv_xlen == 64
	atomic_కూడు_d, /* 12 */
	atomic_కూడు_d_rl, /* 13 */
	atomic_కూడు_d_aq, /* 14 */
	atomic_కూడు_d_aqrl, /* 15 */
#లేకపోతే
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#అయితే_ముగింపు
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

స్థిర స్థిరరాశి illegal_insn_func amoswap_పట్టిక[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	atomic_మార్పిడి_w, /* 8 */
	atomic_మార్పిడి_w_rl, /* 9 */
	atomic_మార్పిడి_w_aq, /* 10 */
	atomic_మార్పిడి_w_aqrl, /* 11 */
#ఒకవేళ __riscv_xlen == 64
	atomic_మార్పిడి_d, /* 12 */
	atomic_మార్పిడి_d_rl, /* 13 */
	atomic_మార్పిడి_d_aq, /* 14 */
	atomic_మార్పిడి_d_aqrl, /* 15 */
#లేకపోతే
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#అయితే_ముగింపు
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

స్థిర స్థిరరాశి illegal_insn_func amoxor_పట్టిక[32] = {
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
#ఒకవేళ __riscv_xlen == 64
	atomic_xor_d, /* 12 */
	atomic_xor_d_rl, /* 13 */
	atomic_xor_d_aq, /* 14 */
	atomic_xor_d_aqrl, /* 15 */
#లేకపోతే
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#అయితే_ముగింపు
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

స్థిర స్థిరరాశి illegal_insn_func amoor_పట్టిక[32] = {
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
#ఒకవేళ __riscv_xlen == 64
	atomic_or_d, /* 12 */
	atomic_or_d_rl, /* 13 */
	atomic_or_d_aq, /* 14 */
	atomic_or_d_aqrl, /* 15 */
#లేకపోతే
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#అయితే_ముగింపు
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

స్థిర స్థిరరాశి illegal_insn_func amoand_పట్టిక[32] = {
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
#ఒకవేళ __riscv_xlen == 64
	atomic_and_d, /* 12 */
	atomic_and_d_rl, /* 13 */
	atomic_and_d_aq, /* 14 */
	atomic_and_d_aqrl, /* 15 */
#లేకపోతే
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#అయితే_ముగింపు
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

స్థిర స్థిరరాశి illegal_insn_func amomin_పట్టిక[32] = {
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
#ఒకవేళ __riscv_xlen == 64
	atomic_min_d, /* 12 */
	atomic_min_d_rl, /* 13 */
	atomic_min_d_aq, /* 14 */
	atomic_min_d_aqrl, /* 15 */
#లేకపోతే
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#అయితే_ముగింపు
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

స్థిర స్థిరరాశి illegal_insn_func amomax_పట్టిక[32] = {
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
#ఒకవేళ __riscv_xlen == 64
	atomic_max_d, /* 12 */
	atomic_max_d_rl, /* 13 */
	atomic_max_d_aq, /* 14 */
	atomic_max_d_aqrl, /* 15 */
#లేకపోతే
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#అయితే_ముగింపు
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

స్థిర స్థిరరాశి illegal_insn_func amominu_పట్టిక[32] = {
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
#ఒకవేళ __riscv_xlen == 64
	atomic_minu_d, /* 12 */
	atomic_minu_d_rl, /* 13 */
	atomic_minu_d_aq, /* 14 */
	atomic_minu_d_aqrl, /* 15 */
#లేకపోతే
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#అయితే_ముగింపు
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

స్థిర స్థిరరాశి illegal_insn_func amomaxu_పట్టిక[32] = {
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
#ఒకవేళ __riscv_xlen == 64
	atomic_maxu_d, /* 12 */
	atomic_maxu_d_rl, /* 13 */
	atomic_maxu_d_aq, /* 14 */
	atomic_maxu_d_aqrl, /* 15 */
#లేకపోతే
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#అయితే_ముగింపు
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

స్థిర పూర్ణాంకం amoadd_insn(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	తిరిగి amoadd_పట్టిక[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

స్థిర పూర్ణాంకం amoswap_insn(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	తిరిగి amoswap_పట్టిక[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

స్థిర పూర్ణాంకం amoxor_insn(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	తిరిగి amoxor_పట్టిక[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

స్థిర పూర్ణాంకం amoor_insn(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	తిరిగి amoor_పట్టిక[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

స్థిర పూర్ణాంకం amoand_insn(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	తిరిగి amoand_పట్టిక[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

స్థిర పూర్ణాంకం amomin_insn(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	తిరిగి amomin_పట్టిక[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

స్థిర పూర్ణాంకం amomax_insn(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	తిరిగి amomax_పట్టిక[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

స్థిర పూర్ణాంకం amominu_insn(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	తిరిగి amominu_పట్టిక[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

స్థిర పూర్ణాంకం amomaxu_insn(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	తిరిగి amomaxu_పట్టిక[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

స్థిర స్థిరరాశి illegal_insn_func amo_insn_పట్టిక[32] = {
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

పూర్ణాంకం sbi_illegal_atomic(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	తిరిగి amo_insn_పట్టిక[(insn >> 27) & 0x1f](insn, regs);
}

#లేకపోతే
#దోషం "need a or zalrsc"
#అయితే_ముగింపు
