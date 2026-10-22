/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <clement.leger@rivosinc.com>
 */

#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_sse.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>

પૂર્ણાંક sbi_double_trap_handler(સંરચના sbi_trap_context *tcntx)
{
	સંરચના sbi_trap_regs *regs = &tcntx->regs;
	અચલ સંરચના sbi_trap_info *trap = &tcntx->trap;
	bool prev_virt = sbi_regs_from_virt(regs);

	જો (sbi_mstatus_prev_mode(regs->mstatus) != PRV_S)
		પાછા SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	જો (prev_virt)
		પાછા sbi_trap_redirect(regs, trap);

	પાછા sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

શૂન્ય sbi_double_trap_init(સંરચના sbi_scratch *scratch)
{
	જો (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, ખાલી);
}