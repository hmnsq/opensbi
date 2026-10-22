/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_barrier.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_emulate_csr.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_illegal_atomic.h>
#ضمن <sbi/sbi_illegal_insn.h>
#ضمن <sbi/sbi_pmu.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_unpriv.h>
#ضمن <sbi/sbi_console.h>

صحيح truly_illegal_insn(ulong insn, بنية sbi_trap_regs *regs)
{
	بنية sbi_trap_info trap;

	trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
	trap.tval = insn;
	trap.tval2 = 0;
	trap.tinst = 0;
	trap.gva   = 0;

	ܦܢܐ sbi_trap_redirect(regs, &trap);
}

ساكن صحيح misc_mem_opcode_insn(ulong insn, بنية sbi_trap_regs *regs)
{
	/* Errata workaround: emulate `fence.tso` as `fence rw, rw`. */
	ܐܢ ((insn & INSN_MASK_FENCE_TSO) == INSN_MATCH_FENCE_TSO) {
		smp_mb();
		regs->mepc += 4;
		ܦܢܐ 0;
	}

	ܦܢܐ truly_illegal_insn(insn, regs);
}

ساكن صحيح system_opcode_insn(ulong insn, بنية sbi_trap_regs *regs)
{
	bool do_write	= false;
	صحيح rs1_num	= GET_RS1_NUM(insn);
	ulong rs1_val	= GET_RS1(insn, regs);
	صحيح csr_num	= GET_CSR_NUM((u32)insn);
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	ulong csr_val, new_csr_val;

	ܐܢ (prev_mode == PRV_M) {
		sbi_printf("%s: Failed to access CSR %#x from M-mode",
			__func__, csr_num);
		ܦܢܐ SBI_EFAIL;
	}

	/* Ensure that we got CSR read/write instruction */
	صحيح funct3 = GET_RM(insn);
	ܐܢ (funct3 == 0 || funct3 == 4) {
		sbi_printf("%s: Invalid opcode for CSR read/write instruction",
			   __func__);
		ܦܢܐ truly_illegal_insn(insn, regs);
	}

	ܐܢ (sbi_emulate_csr_read(csr_num, regs, &csr_val))
		ܦܢܐ truly_illegal_insn(insn, regs);

	بدل (funct3) {
	حالة CSRRW:
		new_csr_val = rs1_val;
		do_write    = true;
		اكسر;
	حالة CSRRS:
		new_csr_val = csr_val | rs1_val;
		do_write    = (rs1_num != 0);
		اكسر;
	حالة CSRRC:
		new_csr_val = csr_val & ~rs1_val;
		do_write    = (rs1_num != 0);
		اكسر;
	حالة CSRRWI:
		new_csr_val = rs1_num;
		do_write    = true;
		اكسر;
	حالة CSRRSI:
		new_csr_val = csr_val | rs1_num;
		do_write    = (rs1_num != 0);
		اكسر;
	حالة CSRRCI:
		new_csr_val = csr_val & ~rs1_num;
		do_write    = (rs1_num != 0);
		اكسر;
	افتراضي:
		ܦܢܐ truly_illegal_insn(insn, regs);
	}

	ܐܢ (do_write && sbi_emulate_csr_write(csr_num, regs, new_csr_val))
		ܦܢܐ truly_illegal_insn(insn, regs);

	SET_RD(insn, regs, csr_val);

	regs->mepc += 4;

	ܦܢܐ 0;
}

ساكن ثابت illegal_insn_func illegal_insn_table[32] = {
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

صحيح sbi_illegal_insn_handler(بنية sbi_trap_context *tcntx)
{
	بنية sbi_trap_regs *regs = &tcntx->regs;
	ulong insn = tcntx->trap.tval;
	بنية sbi_trap_info uptrap;

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
	ܐܢ (unlikely((insn & 3) != 3)) {
		insn = sbi_get_insn(regs->mepc, &uptrap);
		ܐܢ (uptrap.cause)
			ܦܢܐ sbi_trap_redirect(regs, &uptrap);
		ܐܢ ((insn & 3) != 3)
			ܦܢܐ truly_illegal_insn(insn, regs);
	}

	ܦܢܐ illegal_insn_table[(insn & 0x7c) >> 2](insn, regs);
}
