/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 MIPS
 *
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_illegal_atomic.h>
#अन्तर्भाव <sbi/sbi_illegal_insn.h>

#यदि !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#दोषः "opensbi strongly relies on the Zaamo or Zalrsc extension of RISC-V"
#यदि_अन्तः

#यदि defined(__riscv_atomic) || defined(__riscv_zaamo)

पूर्णाङ्क sbi_illegal_atomic(ulong insn, संरचना sbi_trap_regs *regs)
{
	प्रत्यागम truly_illegal_insn(insn, regs);
}

#नोचेद्यदि defined(__riscv_zalrsc)

#परिभाषय DEFINE_UNPRIVILEGED_LR_FUNCTION(type, aqrl, insn)			\
	स्थिर type lr_##type##aqrl(ध्रुव type *addr,				\
				संरचना sbi_trap_info *trap)			\
	{									\
		पञ्जिका ulong tinfo asm("a3");					\
		पञ्जिका ulong mstatus = 0;					\
		पञ्जिका ulong mtvec = (ulong)sbi_hart_expected_trap;		\
		type ret = 0;							\
		trap->cause = 0;						\
		asm चञ्चल(							\
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
		प्रत्यागम ret;							\
	}

#परिभाषय DEFINE_UNPRIVILEGED_SC_FUNCTION(type, aqrl, insn)			\
	स्थिर type sc_##type##aqrl(type *addr, type val,			\
				संरचना sbi_trap_info *trap)			\
	{									\
		पञ्जिका ulong tinfo asm("a3");					\
		पञ्जिका ulong mstatus = 0;					\
		पञ्जिका ulong mtvec = (ulong)sbi_hart_expected_trap;		\
		type ret = 0;							\
		trap->cause = 0;						\
		asm चञ्चल(							\
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
		प्रत्यागम ret;							\
	}

DEFINE_UNPRIVILEGED_LR_FUNCTION(s32, , lr.w);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s32, _aq, lr.w.aq);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s32, _rl, lr.w.rl);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s32, _aqrl, lr.w.aqrl);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s32, , sc.w);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s32, _aq, sc.w.aq);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s32, _rl, sc.w.rl);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s32, _aqrl, sc.w.aqrl);
#यदि __riscv_xlen == 64
DEFINE_UNPRIVILEGED_LR_FUNCTION(s64, , lr.d);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s64, _aq, lr.d.aq);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s64, _rl, lr.d.rl);
DEFINE_UNPRIVILEGED_LR_FUNCTION(s64, _aqrl, lr.d.aqrl);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s64, , sc.d);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s64, _aq, sc.d.aq);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s64, _rl, sc.d.rl);
DEFINE_UNPRIVILEGED_SC_FUNCTION(s64, _aqrl, sc.d.aqrl);
#यदि_अन्तः

#परिभाषय DEFINE_ATOMIC_FUNCTION(name, type, func)				\
	स्थिर पूर्णाङ्क atomic_##name(ulong insn, संरचना sbi_trap_regs *regs)	\
	{									\
		संरचना sbi_trap_info uptrap;					\
		ulong addr = GET_RS1(insn, regs);				\
		ulong val = GET_RS2(insn, regs);				\
		ulong rd_val = 0;						\
		ulong fail = 1;							\
		यावत् (fail) {							\
			rd_val = lr_##type((शून्य *)addr, &uptrap);		\
			यदि (uptrap.cause) {					\
				प्रत्यागम sbi_trap_redirect(regs, &uptrap);	\
			}							\
			fail = sc_##type((शून्य *)addr, func, &uptrap);	\
			यदि (uptrap.cause) {					\
				प्रत्यागम sbi_trap_redirect(regs, &uptrap);	\
			}							\
		}								\
		समुच्चयः_RD(insn, regs, rd_val);					\
		regs->mepc += 4;						\
		प्रत्यागम 0;							\
	}

DEFINE_ATOMIC_FUNCTION(सङ्कलय_w, s32, rd_val + val);
DEFINE_ATOMIC_FUNCTION(सङ्कलय_w_aq, s32_aq, rd_val + val);
DEFINE_ATOMIC_FUNCTION(सङ्कलय_w_rl, s32_rl, rd_val + val);
DEFINE_ATOMIC_FUNCTION(सङ्कलय_w_aqrl, s32_aqrl, rd_val + val);
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
DEFINE_ATOMIC_FUNCTION(swap_w, s32, val);
DEFINE_ATOMIC_FUNCTION(swap_w_aq, s32_aq, val);
DEFINE_ATOMIC_FUNCTION(swap_w_rl, s32_rl, val);
DEFINE_ATOMIC_FUNCTION(swap_w_aqrl, s32_aqrl, val);
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

#यदि __riscv_xlen == 64
DEFINE_ATOMIC_FUNCTION(सङ्कलय_d, s64, rd_val + val);
DEFINE_ATOMIC_FUNCTION(सङ्कलय_d_aq, s64_aq, rd_val + val);
DEFINE_ATOMIC_FUNCTION(सङ्कलय_d_rl, s64_rl, rd_val + val);
DEFINE_ATOMIC_FUNCTION(सङ्कलय_d_aqrl, s64_aqrl, rd_val + val);
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
DEFINE_ATOMIC_FUNCTION(swap_d, s64, val);
DEFINE_ATOMIC_FUNCTION(swap_d_aq, s64_aq, val);
DEFINE_ATOMIC_FUNCTION(swap_d_rl, s64_rl, val);
DEFINE_ATOMIC_FUNCTION(swap_d_aqrl, s64_aqrl, val);
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
#यदि_अन्तः

स्थिर ध्रुव illegal_insn_func amoadd_सारणी[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	atomic_सङ्कलय_w, /* 8 */
	atomic_सङ्कलय_w_rl, /* 9 */
	atomic_सङ्कलय_w_aq, /* 10 */
	atomic_सङ्कलय_w_aqrl, /* 11 */
#यदि __riscv_xlen == 64
	atomic_सङ्कलय_d, /* 12 */
	atomic_सङ्कलय_d_rl, /* 13 */
	atomic_सङ्कलय_d_aq, /* 14 */
	atomic_सङ्कलय_d_aqrl, /* 15 */
#अन्यथा
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#यदि_अन्तः
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

स्थिर ध्रुव illegal_insn_func amoswap_सारणी[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	truly_illegal_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	atomic_swap_w, /* 8 */
	atomic_swap_w_rl, /* 9 */
	atomic_swap_w_aq, /* 10 */
	atomic_swap_w_aqrl, /* 11 */
#यदि __riscv_xlen == 64
	atomic_swap_d, /* 12 */
	atomic_swap_d_rl, /* 13 */
	atomic_swap_d_aq, /* 14 */
	atomic_swap_d_aqrl, /* 15 */
#अन्यथा
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#यदि_अन्तः
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

स्थिर ध्रुव illegal_insn_func amoxor_सारणी[32] = {
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
#यदि __riscv_xlen == 64
	atomic_xor_d, /* 12 */
	atomic_xor_d_rl, /* 13 */
	atomic_xor_d_aq, /* 14 */
	atomic_xor_d_aqrl, /* 15 */
#अन्यथा
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#यदि_अन्तः
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

स्थिर ध्रुव illegal_insn_func amoor_सारणी[32] = {
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
#यदि __riscv_xlen == 64
	atomic_or_d, /* 12 */
	atomic_or_d_rl, /* 13 */
	atomic_or_d_aq, /* 14 */
	atomic_or_d_aqrl, /* 15 */
#अन्यथा
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#यदि_अन्तः
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

स्थिर ध्रुव illegal_insn_func amoand_सारणी[32] = {
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
#यदि __riscv_xlen == 64
	atomic_and_d, /* 12 */
	atomic_and_d_rl, /* 13 */
	atomic_and_d_aq, /* 14 */
	atomic_and_d_aqrl, /* 15 */
#अन्यथा
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#यदि_अन्तः
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

स्थिर ध्रुव illegal_insn_func amomin_सारणी[32] = {
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
#यदि __riscv_xlen == 64
	atomic_min_d, /* 12 */
	atomic_min_d_rl, /* 13 */
	atomic_min_d_aq, /* 14 */
	atomic_min_d_aqrl, /* 15 */
#अन्यथा
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#यदि_अन्तः
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

स्थिर ध्रुव illegal_insn_func amomax_सारणी[32] = {
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
#यदि __riscv_xlen == 64
	atomic_max_d, /* 12 */
	atomic_max_d_rl, /* 13 */
	atomic_max_d_aq, /* 14 */
	atomic_max_d_aqrl, /* 15 */
#अन्यथा
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#यदि_अन्तः
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

स्थिर ध्रुव illegal_insn_func amominu_सारणी[32] = {
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
#यदि __riscv_xlen == 64
	atomic_minu_d, /* 12 */
	atomic_minu_d_rl, /* 13 */
	atomic_minu_d_aq, /* 14 */
	atomic_minu_d_aqrl, /* 15 */
#अन्यथा
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#यदि_अन्तः
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

स्थिर ध्रुव illegal_insn_func amomaxu_सारणी[32] = {
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
#यदि __riscv_xlen == 64
	atomic_maxu_d, /* 12 */
	atomic_maxu_d_rl, /* 13 */
	atomic_maxu_d_aq, /* 14 */
	atomic_maxu_d_aqrl, /* 15 */
#अन्यथा
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
#यदि_अन्तः
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

स्थिर पूर्णाङ्क amoadd_insn(ulong insn, संरचना sbi_trap_regs *regs)
{
	प्रत्यागम amoadd_सारणी[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

स्थिर पूर्णाङ्क amoswap_insn(ulong insn, संरचना sbi_trap_regs *regs)
{
	प्रत्यागम amoswap_सारणी[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

स्थिर पूर्णाङ्क amoxor_insn(ulong insn, संरचना sbi_trap_regs *regs)
{
	प्रत्यागम amoxor_सारणी[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

स्थिर पूर्णाङ्क amoor_insn(ulong insn, संरचना sbi_trap_regs *regs)
{
	प्रत्यागम amoor_सारणी[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

स्थिर पूर्णाङ्क amoand_insn(ulong insn, संरचना sbi_trap_regs *regs)
{
	प्रत्यागम amoand_सारणी[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

स्थिर पूर्णाङ्क amomin_insn(ulong insn, संरचना sbi_trap_regs *regs)
{
	प्रत्यागम amomin_सारणी[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

स्थिर पूर्णाङ्क amomax_insn(ulong insn, संरचना sbi_trap_regs *regs)
{
	प्रत्यागम amomax_सारणी[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

स्थिर पूर्णाङ्क amominu_insn(ulong insn, संरचना sbi_trap_regs *regs)
{
	प्रत्यागम amominu_सारणी[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

स्थिर पूर्णाङ्क amomaxu_insn(ulong insn, संरचना sbi_trap_regs *regs)
{
	प्रत्यागम amomaxu_सारणी[(GET_FUNC3(insn) << 2) + GET_AQRL(insn)](insn, regs);
}

स्थिर ध्रुव illegal_insn_func amo_insn_सारणी[32] = {
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

पूर्णाङ्क sbi_illegal_atomic(ulong insn, संरचना sbi_trap_regs *regs)
{
	प्रत्यागम amo_insn_सारणी[(insn >> 27) & 0x1f](insn, regs);
}

#अन्यथा
#दोषः "need a or zalrsc"
#यदि_अन्तः
