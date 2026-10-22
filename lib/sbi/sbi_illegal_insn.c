/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_barrier.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_emulate_csr.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_illegal_atomic.h>
#అన్తర్భావ <sbi/sbi_illegal_insn.h>
#అన్తర్భావ <sbi/sbi_pmu.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_unpriv.h>
#అన్తర్భావ <sbi/sbi_console.h>

పూర్ణాంకం truly_illegal_insn(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	నిర్మాణం sbi_trap_info trap;

	trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
	trap.tval = insn;
	trap.tval2 = 0;
	trap.tinst = 0;
	trap.gva   = 0;

	తిరిగి sbi_trap_redirect(regs, &trap);
}

స్థిర పూర్ణాంకం misc_mem_opcode_insn(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	/* Errata workaround: emulate `fence.tso` as `fence rw, rw`. */
	ఒకవేళ ((insn & INSN_ముసుగు_FENCE_TSO) == INSN_సరిపోలిక_FENCE_TSO) {
		smp_mb();
		regs->mepc += 4;
		తిరిగి 0;
	}

	తిరిగి truly_illegal_insn(insn, regs);
}

స్థిర పూర్ణాంకం system_opcode_insn(ulong insn, నిర్మాణం sbi_trap_regs *regs)
{
	bool do_write	= false;
	పూర్ణాంకం rs1_అంకె	= GET_RS1_అంకె(insn);
	ulong rs1_val	= GET_RS1(insn, regs);
	పూర్ణాంకం csr_అంకె	= GET_CSR_అంకె((u32)insn);
	ulong prev_బహుళకం = sbi_mstatus_prev_బహుళకం(regs->mstatus);
	ulong csr_val, new_csr_val;

	ఒకవేళ (prev_బహుళకం == PRV_M) {
		sbi_printf("%s: Failed to access CSR %#x from M-mode",
			__func__, csr_అంకె);
		తిరిగి SBI_EFAIL;
	}

	/* Ensure that we got CSR read/write instruction */
	పూర్ణాంకం funct3 = GET_RM(insn);
	ఒకవేళ (funct3 == 0 || funct3 == 4) {
		sbi_printf("%s: Invalid opcode for CSR read/write instruction",
			   __func__);
		తిరిగి truly_illegal_insn(insn, regs);
	}

	ఒకవేళ (sbi_emulate_csr_read(csr_అంకె, regs, &csr_val))
		తిరిగి truly_illegal_insn(insn, regs);

	ఎంపిక (funct3) {
	స్థితి CSRRW:
		new_csr_val = rs1_val;
		do_write    = true;
		ఆపు;
	స్థితి CSRRS:
		new_csr_val = csr_val | rs1_val;
		do_write    = (rs1_అంకె != 0);
		ఆపు;
	స్థితి CSRRC:
		new_csr_val = csr_val & ~rs1_val;
		do_write    = (rs1_అంకె != 0);
		ఆపు;
	స్థితి CSRRWI:
		new_csr_val = rs1_అంకె;
		do_write    = true;
		ఆపు;
	స్థితి CSRRSI:
		new_csr_val = csr_val | rs1_అంకె;
		do_write    = (rs1_అంకె != 0);
		ఆపు;
	స్థితి CSRRCI:
		new_csr_val = csr_val & ~rs1_అంకె;
		do_write    = (rs1_అంకె != 0);
		ఆపు;
	అప్రమేయ:
		తిరిగి truly_illegal_insn(insn, regs);
	}

	ఒకవేళ (do_write && sbi_emulate_csr_write(csr_అంకె, regs, new_csr_val))
		తిరిగి truly_illegal_insn(insn, regs);

	సమితి_RD(insn, regs, csr_val);

	regs->mepc += 4;

	తిరిగి 0;
}

స్థిర స్థిరరాశి illegal_insn_func illegal_insn_పట్టిక[32] = {
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

పూర్ణాంకం sbi_illegal_insn_handler(నిర్మాణం sbi_trap_context *tcntx)
{
	నిర్మాణం sbi_trap_regs *regs = &tcntx->regs;
	ulong insn = tcntx->trap.tval;
	నిర్మాణం sbi_trap_info uptrap;

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
	ఒకవేళ (unlikely((insn & 3) != 3)) {
		insn = sbi_get_insn(regs->mepc, &uptrap);
		ఒకవేళ (uptrap.cause)
			తిరిగి sbi_trap_redirect(regs, &uptrap);
		ఒకవేళ ((insn & 3) != 3)
			తిరిగి truly_illegal_insn(insn, regs);
	}

	తిరిగి illegal_insn_పట్టిక[(insn & 0x7c) >> 2](insn, regs);
}
