/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_barrier.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_emulate_csr.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_illegal_atomic.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_illegal_insn.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmu.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_unpriv.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>

ಪೂರ್ಣಾಙ್ಕ truly_illegal_insn(ulong insn, ಸಂರಚನಾ sbi_trap_regs *regs)
{
	ಸಂರಚನಾ sbi_trap_info trap;

	trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
	trap.tval = insn;
	trap.tval2 = 0;
	trap.tinst = 0;
	trap.gva   = 0;

	ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, &trap);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ misc_mem_opcode_insn(ulong insn, ಸಂರಚನಾ sbi_trap_regs *regs)
{
	/* Errata workaround: emulate `fence.tso` as `fence rw, rw`. */
	ಯದಿ ((insn & INSN_MASK_FENCE_TSO) == INSN_MATCH_FENCE_TSO) {
		smp_mb();
		regs->mepc += 4;
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಪ್ರತ್ಯಾಗಮ truly_illegal_insn(insn, regs);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ system_opcode_insn(ulong insn, ಸಂರಚನಾ sbi_trap_regs *regs)
{
	bool do_write	= false;
	ಪೂರ್ಣಾಙ್ಕ rs1_ಅಂಕೆ	= GET_RS1_ಅಂಕೆ(insn);
	ulong rs1_val	= GET_RS1(insn, regs);
	ಪೂರ್ಣಾಙ್ಕ csr_ಅಂಕೆ	= GET_CSR_ಅಂಕೆ((u32)insn);
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	ulong csr_val, new_csr_val;

	ಯದಿ (prev_mode == PRV_M) {
		sbi_printf("%s: Failed to access CSR %#x from M-mode",
			__func__, csr_ಅಂಕೆ);
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;
	}

	/* Ensure that we got CSR read/write instruction */
	ಪೂರ್ಣಾಙ್ಕ funct3 = GET_RM(insn);
	ಯದಿ (funct3 == 0 || funct3 == 4) {
		sbi_printf("%s: Invalid opcode for CSR read/write instruction",
			   __func__);
		ಪ್ರತ್ಯಾಗಮ truly_illegal_insn(insn, regs);
	}

	ಯದಿ (sbi_emulate_csr_read(csr_ಅಂಕೆ, regs, &csr_val))
		ಪ್ರತ್ಯಾಗಮ truly_illegal_insn(insn, regs);

	ವಿಕಲ್ಪ (funct3) {
	ಪಕ್ಷ CSRRW:
		new_csr_val = rs1_val;
		do_write    = true;
		ಭಙ್ಗ;
	ಪಕ್ಷ CSRRS:
		new_csr_val = csr_val | rs1_val;
		do_write    = (rs1_ಅಂಕೆ != 0);
		ಭಙ್ಗ;
	ಪಕ್ಷ CSRRC:
		new_csr_val = csr_val & ~rs1_val;
		do_write    = (rs1_ಅಂಕೆ != 0);
		ಭಙ್ಗ;
	ಪಕ್ಷ CSRRWI:
		new_csr_val = rs1_ಅಂಕೆ;
		do_write    = true;
		ಭಙ್ಗ;
	ಪಕ್ಷ CSRRSI:
		new_csr_val = csr_val | rs1_ಅಂಕೆ;
		do_write    = (rs1_ಅಂಕೆ != 0);
		ಭಙ್ಗ;
	ಪಕ್ಷ CSRRCI:
		new_csr_val = csr_val & ~rs1_ಅಂಕೆ;
		do_write    = (rs1_ಅಂಕೆ != 0);
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ truly_illegal_insn(insn, regs);
	}

	ಯದಿ (do_write && sbi_emulate_csr_write(csr_ಅಂಕೆ, regs, new_csr_val))
		ಪ್ರತ್ಯಾಗಮ truly_illegal_insn(insn, regs);

	SET_RD(insn, regs, csr_val);

	regs->mepc += 4;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ illegal_insn_func illegal_insn_table[32] = {
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

ಪೂರ್ಣಾಙ್ಕ sbi_illegal_insn_handler(ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	ಸಂರಚನಾ sbi_trap_regs *regs = &tcntx->regs;
	ulong insn = tcntx->trap.tval;
	ಸಂರಚನಾ sbi_trap_info uptrap;

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
	ಯದಿ (unlikely((insn & 3) != 3)) {
		insn = sbi_get_insn(regs->mepc, &uptrap);
		ಯದಿ (uptrap.cause)
			ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, &uptrap);
		ಯದಿ ((insn & 3) != 3)
			ಪ್ರತ್ಯಾಗಮ truly_illegal_insn(insn, regs);
	}

	ಪ್ರತ್ಯಾಗಮ illegal_insn_table[(insn & 0x7c) >> 2](insn, regs);
}
