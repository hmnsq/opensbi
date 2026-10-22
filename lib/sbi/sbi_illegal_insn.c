/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_barrier.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_emulate_csr.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_illegal_atomic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_illegal_insn.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unpriv.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>

ਪੂਰਨ truly_illegal_insn(ulong insn, ਬਣਤਰ sbi_trap_regs *regs)
{
	ਬਣਤਰ sbi_trap_info trap;

	trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
	trap.tval = insn;
	trap.tval2 = 0;
	trap.tinst = 0;
	trap.gva   = 0;

	ਵਾਪਸ sbi_trap_redirect(regs, &trap);
}

ਸਥਿਰ ਪੂਰਨ misc_mem_opcode_insn(ulong insn, ਬਣਤਰ sbi_trap_regs *regs)
{
	/* Errata workaround: emulate `fence.tso` as `fence rw, rw`. */
	ਜੇ ((insn & INSN_ਨਕਾਬ_FENCE_TSO) == INSN_ਮੇਲ_FENCE_TSO) {
		smp_mb();
		regs->mepc += 4;
		ਵਾਪਸ 0;
	}

	ਵਾਪਸ truly_illegal_insn(insn, regs);
}

ਸਥਿਰ ਪੂਰਨ system_opcode_insn(ulong insn, ਬਣਤਰ sbi_trap_regs *regs)
{
	bool do_write	= false;
	ਪੂਰਨ rs1_ਅੰਕ	= GET_RS1_ਅੰਕ(insn);
	ulong rs1_val	= GET_RS1(insn, regs);
	ਪੂਰਨ csr_ਅੰਕ	= GET_CSR_ਅੰਕ((u32)insn);
	ulong prev_ਬਹੁਲਕ = sbi_mstatus_prev_ਬਹੁਲਕ(regs->mstatus);
	ulong csr_val, new_csr_val;

	ਜੇ (prev_ਬਹੁਲਕ == PRV_M) {
		sbi_printf("%s: Failed to access CSR %#x from M-mode",
			__func__, csr_ਅੰਕ);
		ਵਾਪਸ SBI_EFAIL;
	}

	/* Ensure that we got CSR read/write instruction */
	ਪੂਰਨ funct3 = GET_RM(insn);
	ਜੇ (funct3 == 0 || funct3 == 4) {
		sbi_printf("%s: Invalid opcode for CSR read/write instruction",
			   __func__);
		ਵਾਪਸ truly_illegal_insn(insn, regs);
	}

	ਜੇ (sbi_emulate_csr_read(csr_ਅੰਕ, regs, &csr_val))
		ਵਾਪਸ truly_illegal_insn(insn, regs);

	ਚੋਣ (funct3) {
	ਹਾਲਤ CSRRW:
		new_csr_val = rs1_val;
		do_write    = true;
		ਰੋਕੋ;
	ਹਾਲਤ CSRRS:
		new_csr_val = csr_val | rs1_val;
		do_write    = (rs1_ਅੰਕ != 0);
		ਰੋਕੋ;
	ਹਾਲਤ CSRRC:
		new_csr_val = csr_val & ~rs1_val;
		do_write    = (rs1_ਅੰਕ != 0);
		ਰੋਕੋ;
	ਹਾਲਤ CSRRWI:
		new_csr_val = rs1_ਅੰਕ;
		do_write    = true;
		ਰੋਕੋ;
	ਹਾਲਤ CSRRSI:
		new_csr_val = csr_val | rs1_ਅੰਕ;
		do_write    = (rs1_ਅੰਕ != 0);
		ਰੋਕੋ;
	ਹਾਲਤ CSRRCI:
		new_csr_val = csr_val & ~rs1_ਅੰਕ;
		do_write    = (rs1_ਅੰਕ != 0);
		ਰੋਕੋ;
	ਮੂਲ:
		ਵਾਪਸ truly_illegal_insn(insn, regs);
	}

	ਜੇ (do_write && sbi_emulate_csr_write(csr_ਅੰਕ, regs, new_csr_val))
		ਵਾਪਸ truly_illegal_insn(insn, regs);

	ਸੈੱਟ_RD(insn, regs, csr_val);

	regs->mepc += 4;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ illegal_insn_func illegal_insn_ਸਾਰਣੀ[32] = {
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

ਪੂਰਨ sbi_illegal_insn_handler(ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਬਣਤਰ sbi_trap_regs *regs = &tcntx->regs;
	ulong insn = tcntx->trap.tval;
	ਬਣਤਰ sbi_trap_info uptrap;

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
	ਜੇ (unlikely((insn & 3) != 3)) {
		insn = sbi_get_insn(regs->mepc, &uptrap);
		ਜੇ (uptrap.cause)
			ਵਾਪਸ sbi_trap_redirect(regs, &uptrap);
		ਜੇ ((insn & 3) != 3)
			ਵਾਪਸ truly_illegal_insn(insn, regs);
	}

	ਵਾਪਸ illegal_insn_ਸਾਰਣੀ[(insn & 0x7c) >> 2](insn, regs);
}
