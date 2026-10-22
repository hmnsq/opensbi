/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <clement.leger@rivosinc.com>
 */

#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_sse.h>
#அந்தர்பாவ <sbi/sbi_trap.h>

முழு sbi_double_trap_handler(கட்டமைப்பு sbi_trap_context *tcntx)
{
	கட்டமைப்பு sbi_trap_regs *regs = &tcntx->regs;
	மாறிலி கட்டமைப்பு sbi_trap_info *trap = &tcntx->trap;
	bool prev_virt = sbi_regs_from_virt(regs);

	எனில் (sbi_mstatus_prev_mode(regs->mstatus) != PRV_S)
		திரும்பு SBI_ERR_INVALID_PARAM;

	/* Exception was taken in VS-mode, redirect it to S-mode */
	எனில் (prev_virt)
		திரும்பு sbi_trap_redirect(regs, trap);

	திரும்பு sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP);
}

வெறுமை sbi_double_trap_init(கட்டமைப்பு sbi_scratch *scratch)
{
	எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSDBLTRP))
		sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP, வெற்று);
}