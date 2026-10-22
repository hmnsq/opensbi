/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <clement.leger@rivosinc.com>
 */

#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_sse.h>
#شامل <sbi/sbi_trap.h>

صحیح sbi_double_trap_handler(ساخت sbi_trap_context *tcntx)
{
	ساخت sbi_trap_regs *regs = &tcntx->regs;
	ثابت ساخت sbi_trap_info *trap = &tcntx->trap;
	bool پچھلا_virt = sbi_regs_from_virt(regs);

	اگر (sbi_mstatus_پچھلا_منوال(regs->mstatus) != PRV_S)
		واپس SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	اگر (پچھلا_virt)
		واپس sbi_trap_redirect(regs, trap);

	واپس sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

خالی sbi_double_trap_init(ساخت sbi_scratch *scratch)
{
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_جوڑو_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, تہی);
}