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

ಪೂರ್ಣಾಂಕ sbi_double_trap_handler(ರಚನೆ sbi_trap_context *tcntx)
{
	ರಚನೆ sbi_trap_regs *regs = &tcntx->regs;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_trap_info *trap = &tcntx->trap;
	bool prev_virt = sbi_regs_from_virt(regs);

	ಆದರೆ (sbi_mstatus_prev_ಬಹುಲಕ(regs->mstatus) != PRV_S)
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	ಆದರೆ (prev_virt)
		ಹಿಂತಿರುಗಿ sbi_trap_redirect(regs, trap);

	ಹಿಂತಿರುಗಿ sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

ಶೂನ್ಯ sbi_double_trap_init(ರಚನೆ sbi_scratch *scratch)
{
	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_ಕೂಡಿಸು_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, ಖಾಲಿ);
}