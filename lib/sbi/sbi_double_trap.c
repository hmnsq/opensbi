/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <clement.leger@rivosinc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_sse.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>

ਪੂਰਨ sbi_double_trap_handler(ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਬਣਤਰ sbi_trap_regs *regs = &tcntx->regs;
	ਅਚਲ ਬਣਤਰ sbi_trap_info *trap = &tcntx->trap;
	bool prev_virt = sbi_regs_from_virt(regs);

	ਜੇ (sbi_mstatus_prev_ਬਹੁਲਕ(regs->mstatus) != PRV_S)
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	ਜੇ (prev_virt)
		ਵਾਪਸ sbi_trap_redirect(regs, trap);

	ਵਾਪਸ sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

ਖਾਲੀ sbi_double_trap_init(ਬਣਤਰ sbi_scratch *scratch)
{
	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_ਜੋੜੋ_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, ਨਿਰਾ);
}