/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <clement.leger@rivosinc.com>
 */

#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_sse.h>
#अन्तर्भाव <sbi/sbi_trap.h>

पूर्णाङ्क sbi_double_trap_handler(संरचना sbi_trap_context *tcntx)
{
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	ध्रुव संरचना sbi_trap_info *trap = &tcntx->trap;
	bool prev_virt = sbi_regs_from_virt(regs);

	सचे (sbi_mstatus_prev_mode(regs->mstatus) != PRV_S)
		पटिनिवत्त SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	सचे (prev_virt)
		पटिनिवत्त sbi_trap_redirect(regs, trap);

	पटिनिवत्त sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

शून्य sbi_double_trap_init(संरचना sbi_scratch *scratch)
{
	सचे (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, रिक्त);
}