/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_barrier.h>
#અન્તર્ભાવ <sbi/riscv_encoding.h>
#અન્તર્ભાવ <sbi/sbi_bitops.h>
#અન્તર્ભાવ <sbi/sbi_emulate_csr.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_illegal_atomic.h>
#અન્તર્ભાવ <sbi/sbi_illegal_insn.h>
#અન્તર્ભાવ <sbi/sbi_pmu.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_unpriv.h>
#અન્તર્ભાવ <sbi/sbi_console.h>

પૂર્ણાંક truly_illegal_insn(ulong insn, સંરચના sbi_trap_regs *regs)
{
	સંરચના sbi_trap_info trap;

	trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
	trap.tval = insn;
	trap.tval2 = 0;
	trap.tinst = 0;
	trap.gva   = 0;

	પાછા sbi_trap_redirect(regs, &trap);
}

સ્થિર પૂર્ણાંક misc_mem_opcode_insn(ulong insn, સંરચના sbi_trap_regs *regs)
{
	/* Errata workaround: emulate `fence.tso` as `fence rw, rw`. */
	જો ((insn & INSN_MASK_FENCE_TSO) == INSN_MATCH_FENCE_TSO) {
		smp_mb();
		regs->mepc += 4;
		પાછા 0;
	}

	પાછા truly_illegal_insn(insn, regs);
}

સ્થિર પૂર્ણાંક system_opcode_insn(ulong insn, સંરચના sbi_trap_regs *regs)
{
	bool do_write	= false;
	પૂર્ણાંક rs1_અંક	= GET_RS1_અંક(insn);
	ulong rs1_val	= GET_RS1(insn, regs);
	પૂર્ણાંક csr_અંક	= GET_CSR_અંક((u32)insn);
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	ulong csr_val, new_csr_val;

	જો (prev_mode == PRV_M) {
		sbi_printf("%s: Failed to access CSR %#x from M-mode",
			__func__, csr_અંક);
		પાછા SBI_EFAIL;
	}

	/* Ensure that we got CSR read/write instruction */
	પૂર્ણાંક funct3 = GET_RM(insn);
	જો (funct3 == 0 || funct3 == 4) {
		sbi_printf("%s: Invalid opcode for CSR read/write instruction",
			   __func__);
		પાછા truly_illegal_insn(insn, regs);
	}

	જો (sbi_emulate_csr_read(csr_અંક, regs, &csr_val))
		પાછા truly_illegal_insn(insn, regs);

	પસંદગી (funct3) {
	સ્થિતિ CSRRW:
		new_csr_val = rs1_val;
		do_write    = true;
		તોડો;
	સ્થિતિ CSRRS:
		new_csr_val = csr_val | rs1_val;
		do_write    = (rs1_અંક != 0);
		તોડો;
	સ્થિતિ CSRRC:
		new_csr_val = csr_val & ~rs1_val;
		do_write    = (rs1_અંક != 0);
		તોડો;
	સ્થિતિ CSRRWI:
		new_csr_val = rs1_અંક;
		do_write    = true;
		તોડો;
	સ્થિતિ CSRRSI:
		new_csr_val = csr_val | rs1_અંક;
		do_write    = (rs1_અંક != 0);
		તોડો;
	સ્થિતિ CSRRCI:
		new_csr_val = csr_val & ~rs1_અંક;
		do_write    = (rs1_અંક != 0);
		તોડો;
	મૂળભૂત:
		પાછા truly_illegal_insn(insn, regs);
	}

	જો (do_write && sbi_emulate_csr_write(csr_અંક, regs, new_csr_val))
		પાછા truly_illegal_insn(insn, regs);

	SET_RD(insn, regs, csr_val);

	regs->mepc += 4;

	પાછા 0;
}

સ્થિર અચલ illegal_insn_func illegal_insn_table[32] = {
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

પૂર્ણાંક sbi_illegal_insn_handler(સંરચના sbi_trap_context *tcntx)
{
	સંરચના sbi_trap_regs *regs = &tcntx->regs;
	ulong insn = tcntx->trap.tval;
	સંરચના sbi_trap_info uptrap;

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
	જો (unlikely((insn & 3) != 3)) {
		insn = sbi_get_insn(regs->mepc, &uptrap);
		જો (uptrap.cause)
			પાછા sbi_trap_redirect(regs, &uptrap);
		જો ((insn & 3) != 3)
			પાછા truly_illegal_insn(insn, regs);
	}

	પાછા illegal_insn_table[(insn & 0x7c) >> 2](insn, regs);
}
