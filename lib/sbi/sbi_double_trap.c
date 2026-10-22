/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <clement.leger@rivosinc.com>
 */

#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_sse.h>
#అన్తర్భావ <sbi/sbi_trap.h>

పూర్ణాంకం sbi_double_trap_handler(నిర్మాణం sbi_trap_context *tcntx)
{
	నిర్మాణం sbi_trap_regs *regs = &tcntx->regs;
	స్థిరరాశి నిర్మాణం sbi_trap_info *trap = &tcntx->trap;
	bool prev_virt = sbi_regs_from_virt(regs);

	ఒకవేళ (sbi_mstatus_prev_బహుళకం(regs->mstatus) != PRV_S)
		తిరిగి SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	ఒకవేళ (prev_virt)
		తిరిగి sbi_trap_redirect(regs, trap);

	తిరిగి sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

శూన్య sbi_double_trap_init(నిర్మాణం sbi_scratch *scratch)
{
	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_కూడు_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, ఖాళీ);
}