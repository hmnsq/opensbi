/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_barrier.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_bitops.h>
#include <sbi/sbi_emulate_csr.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_illegal_atomic.h>
#include <sbi/sbi_illegal_insn.h>
#include <sbi/sbi_pmu.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_unpriv.h>
#include <sbi/sbi_console.h>

puur_naa_nka truly_illegal_insn(ulong insn, sa_nracanaa sbi_trap_regs *regs)
{
	sa_nracanaa sbi_trap_info trap;

	trap.cause = CAUSE_ILLEGAL_INSTRUCTION;
	trap.tval = insn;
	trap.tval2 = 0;
	trap.tinst = 0;
	trap.gva   = 0;

	ပွၵ်ႈ sbi_trap_redirect(regs, &trap);
}

static puur_naa_nka misc_mem_opcode_insn(ulong insn, sa_nracanaa sbi_trap_regs *regs)
{
	/* Errata workaround: emulate `fence.tso` as `fence rw, rw`. */
	ပေႃး ((insn & INSN_MASK_FENCE_TSO) == INSN_MATCH_FENCE_TSO) {
		smp_mb();
		regs->mepc += 4;
		ပွၵ်ႈ 0;
	}

	ပွၵ်ႈ truly_illegal_insn(insn, regs);
}

static puur_naa_nka system_opcode_insn(ulong insn, sa_nracanaa sbi_trap_regs *regs)
{
	bool do_write	= false;
	puur_naa_nka rs1_num	= GET_RS1_NUM(insn);
	ulong rs1_val	= GET_RS1(insn, regs);
	puur_naa_nka csr_num	= GET_CSR_NUM((u32)insn);
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	ulong csr_val, new_csr_val;

	ပေႃး (prev_mode == PRV_M) {
		sbi_printf("%s: Failed to access CSR %#x from M-mode",
			__func__, csr_num);
		ပွၵ်ႈ SBI_EFAIL;
	}

	/* Ensure that we got CSR read/write instruction */
	puur_naa_nka funct3 = GET_RM(insn);
	ပေႃး (funct3 == 0 || funct3 == 4) {
		sbi_printf("%s: Invalid opcode for CSR read/write instruction",
			   __func__);
		ပွၵ်ႈ truly_illegal_insn(insn, regs);
	}

	ပေႃး (sbi_emulate_csr_read(csr_num, regs, &csr_val))
		ပွၵ်ႈ truly_illegal_insn(insn, regs);

	switch (funct3) {
	pak_sa CSRRW:
		new_csr_val = rs1_val;
		do_write    = true;
		bha_nga;
	pak_sa CSRRS:
		new_csr_val = csr_val | rs1_val;
		do_write    = (rs1_num != 0);
		bha_nga;
	pak_sa CSRRC:
		new_csr_val = csr_val & ~rs1_val;
		do_write    = (rs1_num != 0);
		bha_nga;
	pak_sa CSRRWI:
		new_csr_val = rs1_num;
		do_write    = true;
		bha_nga;
	pak_sa CSRRSI:
		new_csr_val = csr_val | rs1_num;
		do_write    = (rs1_num != 0);
		bha_nga;
	pak_sa CSRRCI:
		new_csr_val = csr_val & ~rs1_num;
		do_write    = (rs1_num != 0);
		bha_nga;
	default:
		ပွၵ်ႈ truly_illegal_insn(insn, regs);
	}

	ပေႃး (do_write && sbi_emulate_csr_write(csr_num, regs, new_csr_val))
		ပွၵ်ႈ truly_illegal_insn(insn, regs);

	SET_RD(insn, regs, csr_val);

	regs->mepc += 4;

	ပွၵ်ႈ 0;
}

static const illegal_insn_func illegal_insn_table[32] = {
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

puur_naa_nka sbi_illegal_insn_handler(sa_nracanaa sbi_trap_context *tcntx)
{
	sa_nracanaa sbi_trap_regs *regs = &tcntx->regs;
	ulong insn = tcntx->trap.tval;
	sa_nracanaa sbi_trap_info uptrap;

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
	ပေႃး (unlikely((insn & 3) != 3)) {
		insn = sbi_get_insn(regs->mepc, &uptrap);
		ပေႃး (uptrap.cause)
			ပွၵ်ႈ sbi_trap_redirect(regs, &uptrap);
		ပေႃး ((insn & 3) != 3)
			ပွၵ်ႈ truly_illegal_insn(insn, regs);
	}

	ပွၵ်ႈ illegal_insn_table[(insn & 0x7c) >> 2](insn, regs);
}
