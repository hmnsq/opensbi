/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <clement.leger@rivosinc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_sse.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_double_trap_handler(ସଂରଚନା sbi_trap_context *tcntx)
{
	ସଂରଚନା sbi_trap_regs *regs = &tcntx->regs;
	ଅଚଳ ସଂରଚନା sbi_trap_info *trap = &tcntx->trap;
	bool prev_virt = sbi_regs_from_virt(regs);

	ଯଦି (sbi_mstatus_prev_mode(regs->mstatus) != PRV_S)
		ଫେରନ୍ତୁ SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	ଯଦି (prev_virt)
		ଫେରନ୍ତୁ sbi_trap_redirect(regs, trap);

	ଫେରନ୍ତୁ sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

ଶୂନ୍ୟ sbi_double_trap_init(ସଂରଚନା sbi_scratch *scratch)
{
	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, ଖାଲି);
}