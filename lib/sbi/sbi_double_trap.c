/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <clement.leger@rivosinc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_sse.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>

ಪೂರ್ಣಾಙ್ಕ sbi_double_trap_handler(ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	ಸಂರಚನಾ sbi_trap_regs *regs = &tcntx->regs;
	ಧ್ರುವ ಸಂರಚನಾ sbi_trap_info *trap = &tcntx->trap;
	bool prev_virt = sbi_regs_from_virt(regs);

	ಯದಿ (sbi_mstatus_prev_mode(regs->mstatus) != PRV_S)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	ಯದಿ (prev_virt)
		ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, trap);

	ಪ್ರತ್ಯಾಗಮ sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

ಶೂನ್ಯ sbi_double_trap_init(ಸಂರಚನಾ sbi_scratch *scratch)
{
	ಯದಿ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, ರಿಕ್ತ);
}