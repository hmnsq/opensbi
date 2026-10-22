/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_barrier.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_emulate_csr.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_illegal_atomic.h>
#अन्तर्भाव <sbi/sbi_illegal_insn.h>
#अन्तर्भाव <sbi/sbi_pmu.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_unpriv.h>
#अन्तर्भाव <sbi/sbi_console.h>

पूर्णाङ्क truly_illegal_insn(ulong insn, संरचना sbi_trap_regs *regs)
{
	संरचना sbi_trap_info trap;

	trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
	trap.tval = insn;
	trap.tval2 = 0;
	trap.tinst = 0;
	trap.gva   = 0;

	पटिनिवत्त sbi_trap_redirect(regs, &trap);
}

स्थिर पूर्णाङ्क misc_mem_opcode_insn(ulong insn, संरचना sbi_trap_regs *regs)
{
	/* Errata workaround: emulate `fence.tso` as `fence rw, rw`. */
	सचे ((insn & INSN_MASK_FENCE_TSO) == INSN_MATCH_FENCE_TSO) {
		smp_mb();
		regs->mepc += 4;
		पटिनिवत्त 0;
	}

	पटिनिवत्त truly_illegal_insn(insn, regs);
}

स्थिर पूर्णाङ्क system_opcode_insn(ulong insn, संरचना sbi_trap_regs *regs)
{
	bool do_write	= false;
	पूर्णाङ्क rs1_अङ्कः	= GET_RS1_अङ्कः(insn);
	ulong rs1_val	= GET_RS1(insn, regs);
	पूर्णाङ्क csr_अङ्कः	= GET_CSR_अङ्कः((u32)insn);
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	ulong csr_val, new_csr_val;

	सचे (prev_mode == PRV_M) {
		sbi_printf("%s: Failed to access CSR %#x from M-mode",
			__func__, csr_अङ्कः);
		पटिनिवत्त SBI_EFAIL;
	}

	/* Ensure that we got CSR read/write instruction */
	पूर्णाङ्क funct3 = GET_RM(insn);
	सचे (funct3 == 0 || funct3 == 4) {
		sbi_printf("%s: Invalid opcode for CSR read/write instruction",
			   __func__);
		पटिनिवत्त truly_illegal_insn(insn, regs);
	}

	सचे (sbi_emulate_csr_read(csr_अङ्कः, regs, &csr_val))
		पटिनिवत्त truly_illegal_insn(insn, regs);

	विकल्प (funct3) {
	पक्ष CSRRW:
		new_csr_val = rs1_val;
		do_write    = true;
		भङ्ग;
	पक्ष CSRRS:
		new_csr_val = csr_val | rs1_val;
		do_write    = (rs1_अङ्कः != 0);
		भङ्ग;
	पक्ष CSRRC:
		new_csr_val = csr_val & ~rs1_val;
		do_write    = (rs1_अङ्कः != 0);
		भङ्ग;
	पक्ष CSRRWI:
		new_csr_val = rs1_अङ्कः;
		do_write    = true;
		भङ्ग;
	पक्ष CSRRSI:
		new_csr_val = csr_val | rs1_अङ्कः;
		do_write    = (rs1_अङ्कः != 0);
		भङ्ग;
	पक्ष CSRRCI:
		new_csr_val = csr_val & ~rs1_अङ्कः;
		do_write    = (rs1_अङ्कः != 0);
		भङ्ग;
	स्वभाव:
		पटिनिवत्त truly_illegal_insn(insn, regs);
	}

	सचे (do_write && sbi_emulate_csr_write(csr_अङ्कः, regs, new_csr_val))
		पटिनिवत्त truly_illegal_insn(insn, regs);

	SET_RD(insn, regs, csr_val);

	regs->mepc += 4;

	पटिनिवत्त 0;
}

स्थिर ध्रुव illegal_insn_func illegal_insn_table[32] = {
	truly_illegal_insn, /* 0 */
	truly_illegal_insn, /* 1 */
	truly_illegal_insn, /* 2 */
	misc_mem_opcode_insn, /* 3 */
	truly_illegal_insn, /* 4 */
	truly_illegal_insn, /* 5 */
	truly_illegal_insn, /* 6 */
	truly_illegal_insn, /* 7 */
	truly_illegal_insn, /* 8 */
	truly_illegal_insn, /* 9 */
	truly_illegal_insn, /* 10 */
	sbi_illegal_atomic, /* 11 */
	truly_illegal_insn, /* 12 */
	truly_illegal_insn, /* 13 */
	truly_illegal_insn, /* 14 */
	truly_illegal_insn, /* 15 */
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
	system_opcode_insn, /* 28 */
	truly_illegal_insn, /* 29 */
	truly_illegal_insn, /* 30 */
	truly_illegal_insn  /* 31 */
};

पूर्णाङ्क sbi_illegal_insn_handler(संरचना sbi_trap_context *tcntx)
{
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	ulong insn = tcntx->trap.tval;
	संरचना sbi_trap_info uptrap;

	/*
	 * We only deal with 32-bit (or longer) illegal instructions. If we
	 * see instruction is zero OR instruction is 16-bit then we fetch and
	 * check the instruction encoding using unprivilege access.
	 *
	 * The program counter (PC) in RISC-V world is always 2-byte aligned
	 * so handling only 32-bit (or longer) illegal instructions also help
	 * the case where MTVAL CSR contains instruction address for illegal
	 * instruction trap.
	 */

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_ILLEGAL_INSN);
	सचे (unlikely((insn & 3) != 3)) {
		insn = sbi_get_insn(regs->mepc, &uptrap);
		सचे (uptrap.cause)
			पटिनिवत्त sbi_trap_redirect(regs, &uptrap);
		सचे ((insn & 3) != 3)
			पटिनिवत्त truly_illegal_insn(insn, regs);
	}

	पटिनिवत्त illegal_insn_table[(insn & 0x7c) >> 2](insn, regs);
}
