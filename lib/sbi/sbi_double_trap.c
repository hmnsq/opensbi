/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <clement.leger@rivosinc.com>
 */

#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_sse.h>
#समावेश <sbi/sbi_trap.h>

पूर्णांक sbi_double_trap_handler(काष्ठा sbi_trap_context *tcntx)
{
	काष्ठा sbi_trap_regs *regs = &tcntx->regs;
	स्थिर काष्ठा sbi_trap_info *trap = &tcntx->trap;
	bool पूर्व_virt = sbi_regs_from_virt(regs);

	अगर (sbi_mstatus_पूर्व_mode(regs->mstatus) != PRV_S)
		वापस SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	अगर (पूर्व_virt)
		वापस sbi_trap_redirect(regs, trap);

	वापस sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

व्योम sbi_double_trap_init(काष्ठा sbi_scratch *scratch)
{
	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, शून्य);
}