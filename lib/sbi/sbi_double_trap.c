/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <clement.leger@rivosinc.com>
 */

#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_sse.h>
#অন্তর্ভাব <sbi/sbi_trap.h>

পূর্ণসংখ্যা sbi_double_trap_handler(কাঠামো sbi_trap_context *tcntx)
{
	কাঠামো sbi_trap_regs *regs = &tcntx->regs;
	ধ্রুবক কাঠামো sbi_trap_info *trap = &tcntx->trap;
	bool পূর্ব_virt = sbi_regs_from_virt(regs);

	যদি (sbi_mstatus_পূর্ব_mode(regs->mstatus) != PRV_S)
		ফেরত SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	যদি (পূর্ব_virt)
		ফেরত sbi_trap_redirect(regs, trap);

	ফেরত sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

শূন্য sbi_double_trap_init(কাঠামো sbi_scratch *scratch)
{
	যদি (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, ফাঁকা);
}