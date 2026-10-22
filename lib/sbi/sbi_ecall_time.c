/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>

સ્થિર પૂર્ણાંક sbi_ecall_time_handler(unsigned long extid, unsigned long funcid,
				  સંરચના sbi_trap_regs *regs,
				  સંરચના sbi_ecall_return *out)
{
	પૂર્ણાંક ret = 0;

	જો (funcid == SBI_EXT_TIME_SET_TIMER) {
#જો __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#નહીંતર
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
	} નહીંતર
		ret = SBI_ENOTSUPP;

	પાછા ret;
}

સંરચના sbi_ecall_extension ecall_time;

સ્થિર પૂર્ણાંક sbi_ecall_time_register_extensions(શૂન્ય)
{
	પાછા sbi_ecall_register_extension(&ecall_time);
}

સંરચના sbi_ecall_extension ecall_time = {
	.name			= "time",
	.extid_start		= SBI_EXT_TIME,
	.extid_end		= SBI_EXT_TIME,
	.register_extensions	= sbi_ecall_time_register_extensions,
	.handle			= sbi_ecall_time_handler,
};
