/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <clement.leger@rivosinc.com>
 */

#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_sse.h>
#ضمن <sbi/sbi_trap.h>

صحيح sbi_double_trap_handler(بنية sbi_trap_context *tcntx)
{
	بنية sbi_trap_regs *regs = &tcntx->regs;
	ثابت بنية sbi_trap_info *trap = &tcntx->trap;
	bool prev_virt = sbi_regs_from_virt(regs);

	إذا (sbi_mstatus_prev_گهڻائي(regs->mstatus) != PRV_S)
		أرجع SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	إذا (prev_virt)
		أرجع sbi_trap_redirect(regs, trap);

	أرجع sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

فراغ sbi_double_trap_init(بنية sbi_scratch *scratch)
{
	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_جوڙيو_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, عدم);
}