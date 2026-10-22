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

整数 sbi_double_trap_handler(struct sbi_trap_context *tcntx)
{
	struct sbi_trap_regs *regs = &tcntx->regs;
	const struct sbi_trap_info *trap = &tcntx->trap;
	bool prev_virt = sbi_regs_from_virt(regs);

	如果 (sbi_mstatus_prev_mode(regs->mstatus) != PRV_S)
		返回 SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	如果 (prev_virt)
		返回 sbi_trap_redirect(regs, trap);

	返回 sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

void sbi_double_trap_init(struct sbi_scratch *scratch)
{
	如果 (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, NULL);
}