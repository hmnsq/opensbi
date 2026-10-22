/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_barrier.h>
#அந்தர்பாவ <sbi/riscv_encoding.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_emulate_csr.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_illegal_atomic.h>
#அந்தர்பாவ <sbi/sbi_illegal_insn.h>
#அந்தர்பாவ <sbi/sbi_pmu.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/sbi_unpriv.h>
#அந்தர்பாவ <sbi/sbi_console.h>

முழு truly_illegal_insn(ulong insn, கட்டமைப்பு sbi_trap_regs *regs)
{
	கட்டமைப்பு sbi_trap_info trap;

	trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
	trap.tval = insn;
	trap.tval2 = 0;
	trap.tinst = 0;
	trap.gva   = 0;

	திரும்பு sbi_trap_redirect(regs, &trap);
}

நிலையான முழு misc_mem_opcode_insn(ulong insn, கட்டமைப்பு sbi_trap_regs *regs)
{
	/* Errata workaround: emulate `fence.tso` as `fence rw, rw`. */
	எனில் ((insn & INSN_MASK_FENCE_TSO) == INSN_MATCH_FENCE_TSO) {
		smp_mb();
		regs->mepc += 4;
		திரும்பு 0;
	}

	திரும்பு truly_illegal_insn(insn, regs);
}

நிலையான முழு system_opcode_insn(ulong insn, கட்டமைப்பு sbi_trap_regs *regs)
{
	bool do_write	= false;
	முழு rs1_இலக்கம்	= GET_RS1_இலக்கம்(insn);
	ulong rs1_val	= GET_RS1(insn, regs);
	முழு csr_இலக்கம்	= GET_CSR_இலக்கம்((u32)insn);
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	ulong csr_val, new_csr_val;

	எனில் (prev_mode == PRV_M) {
		sbi_printf("%s: Failed to access CSR %#x from M-mode",
			__func__, csr_இலக்கம்);
		திரும்பு SBI_EFAIL;
	}

	/* Ensure that we got CSR read/write instruction */
	முழு funct3 = GET_RM(insn);
	எனில் (funct3 == 0 || funct3 == 4) {
		sbi_printf("%s: Invalid opcode for CSR read/write instruction",
			   __func__);
		திரும்பு truly_illegal_insn(insn, regs);
	}

	எனில் (sbi_emulate_csr_read(csr_இலக்கம், regs, &csr_val))
		திரும்பு truly_illegal_insn(insn, regs);

	தேர்வு (funct3) {
	நிலை CSRRW:
		new_csr_val = rs1_val;
		do_write    = true;
		நிறுத்து;
	நிலை CSRRS:
		new_csr_val = csr_val | rs1_val;
		do_write    = (rs1_இலக்கம் != 0);
		நிறுத்து;
	நிலை CSRRC:
		new_csr_val = csr_val & ~rs1_val;
		do_write    = (rs1_இலக்கம் != 0);
		நிறுத்து;
	நிலை CSRRWI:
		new_csr_val = rs1_இலக்கம்;
		do_write    = true;
		நிறுத்து;
	நிலை CSRRSI:
		new_csr_val = csr_val | rs1_இலக்கம்;
		do_write    = (rs1_இலக்கம் != 0);
		நிறுத்து;
	நிலை CSRRCI:
		new_csr_val = csr_val & ~rs1_இலக்கம்;
		do_write    = (rs1_இலக்கம் != 0);
		நிறுத்து;
	இயல்பு:
		திரும்பு truly_illegal_insn(insn, regs);
	}

	எனில் (do_write && sbi_emulate_csr_write(csr_இலக்கம், regs, new_csr_val))
		திரும்பு truly_illegal_insn(insn, regs);

	SET_RD(insn, regs, csr_val);

	regs->mepc += 4;

	திரும்பு 0;
}

நிலையான மாறிலி illegal_insn_func illegal_insn_table[32] = {
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

முழு sbi_illegal_insn_handler(கட்டமைப்பு sbi_trap_context *tcntx)
{
	கட்டமைப்பு sbi_trap_regs *regs = &tcntx->regs;
	ulong insn = tcntx->trap.tval;
	கட்டமைப்பு sbi_trap_info uptrap;

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
	எனில் (unlikely((insn & 3) != 3)) {
		insn = sbi_get_insn(regs->mepc, &uptrap);
		எனில் (uptrap.cause)
			திரும்பு sbi_trap_redirect(regs, &uptrap);
		எனில் ((insn & 3) != 3)
			திரும்பு truly_illegal_insn(insn, regs);
	}

	திரும்பு illegal_insn_table[(insn & 0x7c) >> 2](insn, regs);
}
