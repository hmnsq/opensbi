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
	bool पूर्व_virt = sbi_regs_from_virt(regs);

	यदि (sbi_mstatus_पूर्व_बहुलकम्(regs->mstatus) != PRV_S)
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	यदि (पूर्व_virt)
		प्रत्यागम sbi_trap_redirect(regs, trap);

	प्रत्यागम sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

शून्य sbi_double_trap_init(संरचना sbi_scratch *scratch)
{
	यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_सङ्कलय_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, रिक्त);
}