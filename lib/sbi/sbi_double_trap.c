/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <clement.leger@rivosinc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_sse.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>

പൂർണ്ണസംഖ്യ sbi_double_trap_handler(ഘടന sbi_trap_context *tcntx)
{
	ഘടന sbi_trap_regs *regs = &tcntx->regs;
	സ്ഥിരം ഘടന sbi_trap_info *trap = &tcntx->trap;
	bool prev_virt = sbi_regs_from_virt(regs);

	എങ്കിൽ (sbi_mstatus_prev_mode(regs->mstatus) != PRV_S)
		തിരികെ SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	എങ്കിൽ (prev_virt)
		തിരികെ sbi_trap_redirect(regs, trap);

	തിരികെ sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

ശൂന്യം sbi_double_trap_init(ഘടന sbi_scratch *scratch)
{
	എങ്കിൽ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, ശൂന്യ);
}