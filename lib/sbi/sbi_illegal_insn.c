/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_barrier.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_emulate_csr.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_illegal_atomic.h>
#شامل <sbi/sbi_illegal_insn.h>
#شامل <sbi/sbi_pmu.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_unpriv.h>
#شامل <sbi/sbi_console.h>

صحیح truly_illegal_insn(ulong insn, ساختار sbi_trap_regs *regs)
{
	ساختار sbi_trap_info trap;

	trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
	trap.tval = insn;
	trap.tval2 = 0;
	trap.tinst = 0;
	trap.gva   = 0;

	بازگشت sbi_trap_redirect(regs, &trap);
}

ایستا صحیح misc_mem_opcode_insn(ulong insn, ساختار sbi_trap_regs *regs)
{
	/* Errata workaround: emulate `fence.tso` as `fence rw, rw`. */
	اگر ((insn & INSN_MASK_FENCE_TSO) == INSN_MATCH_FENCE_TSO) {
		smp_mb();
		regs->mepc += 4;
		بازگشت 0;
	}

	بازگشت truly_illegal_insn(insn, regs);
}

ایستا صحیح system_opcode_insn(ulong insn, ساختار sbi_trap_regs *regs)
{
	bool do_write	= false;
	صحیح rs1_رقم	= GET_RS1_رقم(insn);
	ulong rs1_val	= GET_RS1(insn, regs);
	صحیح csr_رقم	= GET_CSR_رقم((u32)insn);
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	ulong csr_val, new_csr_val;

	اگر (prev_mode == PRV_M) {
		sbi_printf("%s: Failed to access CSR %#x from M-mode",
			__func__, csr_رقم);
		بازگشت SBI_EFAIL;
	}

	/* Ensure that we got CSR read/write instruction */
	صحیح funct3 = GET_RM(insn);
	اگر (funct3 == 0 || funct3 == 4) {
		sbi_printf("%s: Invalid opcode for CSR read/write instruction",
			   __func__);
		بازگشت truly_illegal_insn(insn, regs);
	}

	اگر (sbi_emulate_csr_read(csr_رقم, regs, &csr_val))
		بازگشت truly_illegal_insn(insn, regs);

	انتخاب (funct3) {
	حالت CSRRW:
		new_csr_val = rs1_val;
		do_write    = true;
		بشکن;
	حالت CSRRS:
		new_csr_val = csr_val | rs1_val;
		do_write    = (rs1_رقم != 0);
		بشکن;
	حالت CSRRC:
		new_csr_val = csr_val & ~rs1_val;
		do_write    = (rs1_رقم != 0);
		بشکن;
	حالت CSRRWI:
		new_csr_val = rs1_رقم;
		do_write    = true;
		بشکن;
	حالت CSRRSI:
		new_csr_val = csr_val | rs1_رقم;
		do_write    = (rs1_رقم != 0);
		بشکن;
	حالت CSRRCI:
		new_csr_val = csr_val & ~rs1_رقم;
		do_write    = (rs1_رقم != 0);
		بشکن;
	پیشفرض:
		بازگشت truly_illegal_insn(insn, regs);
	}

	اگر (do_write && sbi_emulate_csr_write(csr_رقم, regs, new_csr_val))
		بازگشت truly_illegal_insn(insn, regs);

	SET_RD(insn, regs, csr_val);

	regs->mepc += 4;

	بازگشت 0;
}

ایستا ثابت illegal_insn_func illegal_insn_table[32] = {
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

صحیح sbi_illegal_insn_handler(ساختار sbi_trap_context *tcntx)
{
	ساختار sbi_trap_regs *regs = &tcntx->regs;
	ulong insn = tcntx->trap.tval;
	ساختار sbi_trap_info uptrap;

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
	اگر (unlikely((insn & 3) != 3)) {
		insn = sbi_get_insn(regs->mepc, &uptrap);
		اگر (uptrap.cause)
			بازگشت sbi_trap_redirect(regs, &uptrap);
		اگر ((insn & 3) != 3)
			بازگشت truly_illegal_insn(insn, regs);
	}

	بازگشت illegal_insn_table[(insn & 0x7c) >> 2](insn, regs);
}
