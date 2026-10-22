/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_barrier.h>
#অন্তর্ভাব <sbi/riscv_encoding.h>
#অন্তর্ভাব <sbi/sbi_bitops.h>
#অন্তর্ভাব <sbi/sbi_emulate_csr.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_illegal_atomic.h>
#অন্তর্ভাব <sbi/sbi_illegal_insn.h>
#অন্তর্ভাব <sbi/sbi_pmu.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_unpriv.h>
#অন্তর্ভাব <sbi/sbi_console.h>

পূর্ণসংখ্যা truly_illegal_insn(ulong insn, কাঠামো sbi_trap_regs *regs)
{
	কাঠামো sbi_trap_info trap;

	trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
	trap.tval = insn;
	trap.tval2 = 0;
	trap.tinst = 0;
	trap.gva   = 0;

	ফেরত sbi_trap_redirect(regs, &trap);
}

স্থির পূর্ণসংখ্যা misc_mem_opcode_insn(ulong insn, কাঠামো sbi_trap_regs *regs)
{
	/* Errata workaround: emulate `fence.tso` as `fence rw, rw`. */
	যদি ((insn & INSN_MASK_FENCE_TSO) == INSN_MATCH_FENCE_TSO) {
		smp_mb();
		regs->mepc += 4;
		ফেরত 0;
	}

	ফেরত truly_illegal_insn(insn, regs);
}

স্থির পূর্ণসংখ্যা system_opcode_insn(ulong insn, কাঠামো sbi_trap_regs *regs)
{
	bool do_write	= false;
	পূর্ণসংখ্যা rs1_অঙ্ক	= GET_RS1_অঙ্ক(insn);
	ulong rs1_val	= GET_RS1(insn, regs);
	পূর্ণসংখ্যা csr_অঙ্ক	= GET_CSR_অঙ্ক((u32)insn);
	ulong পূর্ব_mode = sbi_mstatus_পূর্ব_mode(regs->mstatus);
	ulong csr_val, new_csr_val;

	যদি (পূর্ব_mode == PRV_M) {
		sbi_printf("%s: Failed to access CSR %#x from M-mode",
			__func__, csr_অঙ্ক);
		ফেরত SBI_EFAIL;
	}

	/* Ensure that we got CSR read/write instruction */
	পূর্ণসংখ্যা funct3 = GET_RM(insn);
	যদি (funct3 == 0 || funct3 == 4) {
		sbi_printf("%s: Invalid opcode for CSR read/write instruction",
			   __func__);
		ফেরত truly_illegal_insn(insn, regs);
	}

	যদি (sbi_emulate_csr_read(csr_অঙ্ক, regs, &csr_val))
		ফেরত truly_illegal_insn(insn, regs);

	নির্বাচন (funct3) {
	ক্ষেত্র CSRRW:
		new_csr_val = rs1_val;
		do_write    = true;
		বিরতি;
	ক্ষেত্র CSRRS:
		new_csr_val = csr_val | rs1_val;
		do_write    = (rs1_অঙ্ক != 0);
		বিরতি;
	ক্ষেত্র CSRRC:
		new_csr_val = csr_val & ~rs1_val;
		do_write    = (rs1_অঙ্ক != 0);
		বিরতি;
	ক্ষেত্র CSRRWI:
		new_csr_val = rs1_অঙ্ক;
		do_write    = true;
		বিরতি;
	ক্ষেত্র CSRRSI:
		new_csr_val = csr_val | rs1_অঙ্ক;
		do_write    = (rs1_অঙ্ক != 0);
		বিরতি;
	ক্ষেত্র CSRRCI:
		new_csr_val = csr_val & ~rs1_অঙ্ক;
		do_write    = (rs1_অঙ্ক != 0);
		বিরতি;
	পূর্বনির্ধারিত:
		ফেরত truly_illegal_insn(insn, regs);
	}

	যদি (do_write && sbi_emulate_csr_write(csr_অঙ্ক, regs, new_csr_val))
		ফেরত truly_illegal_insn(insn, regs);

	SET_RD(insn, regs, csr_val);

	regs->mepc += 4;

	ফেরত 0;
}

স্থির ধ্রুবক illegal_insn_func illegal_insn_table[32] = {
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

পূর্ণসংখ্যা sbi_illegal_insn_handler(কাঠামো sbi_trap_context *tcntx)
{
	কাঠামো sbi_trap_regs *regs = &tcntx->regs;
	ulong insn = tcntx->trap.tval;
	কাঠামো sbi_trap_info uptrap;

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
	যদি (unlikely((insn & 3) != 3)) {
		insn = sbi_get_insn(regs->mepc, &uptrap);
		যদি (uptrap.cause)
			ফেরত sbi_trap_redirect(regs, &uptrap);
		যদি ((insn & 3) != 3)
			ফেরত truly_illegal_insn(insn, regs);
	}

	ফেরত illegal_insn_table[(insn & 0x7c) >> 2](insn, regs);
}
