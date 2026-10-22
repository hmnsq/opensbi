/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <clement.leger@rivosinc.com>
 */

#include <sbi/sbi_console.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_sse.h>
#include <sbi/sbi_trap.h>

ཧྲིལ sbi_double_trap_handler(sa_nracanaa sbi_trap_context *tcntx)
{
	sa_nracanaa sbi_trap_regs *regs = &tcntx->regs;
	const sa_nracanaa sbi_trap_info *trap = &tcntx->trap;
	bool prev_virt = sbi_regs_from_virt(regs);

	གལ་ཏེ (sbi_mstatus_prev_mode(regs->mstatus) != PRV_S)
		ཕྱིར SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	གལ་ཏེ (prev_virt)
		ཕྱིར sbi_trap_redirect(regs, trap);

	ཕྱིར sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

void sbi_double_trap_init(sa_nracanaa sbi_scratch *scratch)
{
	གལ་ཏེ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, NULL);
}