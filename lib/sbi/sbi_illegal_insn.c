/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_barrier.h>
#അന്തര്ഭാവ <sbi/riscv_encoding.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/sbi_emulate_csr.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_illegal_atomic.h>
#അന്തര്ഭാവ <sbi/sbi_illegal_insn.h>
#അന്തര്ഭാവ <sbi/sbi_pmu.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_unpriv.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>

പൂർണ്ണസംഖ്യ truly_illegal_insn(ulong insn, ഘടന sbi_trap_regs *regs)
{
	ഘടന sbi_trap_info trap;

	trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
	trap.tval = insn;
	trap.tval2 = 0;
	trap.tinst = 0;
	trap.gva   = 0;

	തിരികെ sbi_trap_redirect(regs, &trap);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ misc_mem_opcode_insn(ulong insn, ഘടന sbi_trap_regs *regs)
{
	/* Errata workaround: emulate `fence.tso` as `fence rw, rw`. */
	എങ്കിൽ ((insn & INSN_MASK_FENCE_TSO) == INSN_MATCH_FENCE_TSO) {
		smp_mb();
		regs->mepc += 4;
		തിരികെ 0;
	}

	തിരികെ truly_illegal_insn(insn, regs);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ system_opcode_insn(ulong insn, ഘടന sbi_trap_regs *regs)
{
	bool do_write	= false;
	പൂർണ്ണസംഖ്യ rs1_അക്കം	= GET_RS1_അക്കം(insn);
	ulong rs1_val	= GET_RS1(insn, regs);
	പൂർണ്ണസംഖ്യ csr_അക്കം	= GET_CSR_അക്കം((u32)insn);
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	ulong csr_val, new_csr_val;

	എങ്കിൽ (prev_mode == PRV_M) {
		sbi_printf("%s: Failed to access CSR %#x from M-mode",
			__func__, csr_അക്കം);
		തിരികെ SBI_EFAIL;
	}

	/* Ensure that we got CSR read/write instruction */
	പൂർണ്ണസംഖ്യ funct3 = GET_RM(insn);
	എങ്കിൽ (funct3 == 0 || funct3 == 4) {
		sbi_printf("%s: Invalid opcode for CSR read/write instruction",
			   __func__);
		തിരികെ truly_illegal_insn(insn, regs);
	}

	എങ്കിൽ (sbi_emulate_csr_read(csr_അക്കം, regs, &csr_val))
		തിരികെ truly_illegal_insn(insn, regs);

	തിരഞ്ഞെടുപ്പ് (funct3) {
	അവസ്ഥ CSRRW:
		new_csr_val = rs1_val;
		do_write    = true;
		നിർത്തുക;
	അവസ്ഥ CSRRS:
		new_csr_val = csr_val | rs1_val;
		do_write    = (rs1_അക്കം != 0);
		നിർത്തുക;
	അവസ്ഥ CSRRC:
		new_csr_val = csr_val & ~rs1_val;
		do_write    = (rs1_അക്കം != 0);
		നിർത്തുക;
	അവസ്ഥ CSRRWI:
		new_csr_val = rs1_അക്കം;
		do_write    = true;
		നിർത്തുക;
	അവസ്ഥ CSRRSI:
		new_csr_val = csr_val | rs1_അക്കം;
		do_write    = (rs1_അക്കം != 0);
		നിർത്തുക;
	അവസ്ഥ CSRRCI:
		new_csr_val = csr_val & ~rs1_അക്കം;
		do_write    = (rs1_അക്കം != 0);
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		തിരികെ truly_illegal_insn(insn, regs);
	}

	എങ്കിൽ (do_write && sbi_emulate_csr_write(csr_അക്കം, regs, new_csr_val))
		തിരികെ truly_illegal_insn(insn, regs);

	SET_RD(insn, regs, csr_val);

	regs->mepc += 4;

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം illegal_insn_func illegal_insn_table[32] = {
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

പൂർണ്ണസംഖ്യ sbi_illegal_insn_handler(ഘടന sbi_trap_context *tcntx)
{
	ഘടന sbi_trap_regs *regs = &tcntx->regs;
	ulong insn = tcntx->trap.tval;
	ഘടന sbi_trap_info uptrap;

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
	എങ്കിൽ (unlikely((insn & 3) != 3)) {
		insn = sbi_get_insn(regs->mepc, &uptrap);
		എങ്കിൽ (uptrap.cause)
			തിരികെ sbi_trap_redirect(regs, &uptrap);
		എങ്കിൽ ((insn & 3) != 3)
			തിരികെ truly_illegal_insn(insn, regs);
	}

	തിരികെ illegal_insn_table[(insn & 0x7c) >> 2](insn, regs);
}
