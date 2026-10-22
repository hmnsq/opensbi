/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_time_handler(unsigned long extid, unsigned long funcid,
				  ഘടന sbi_trap_regs *regs,
				  ഘടന sbi_ecall_return *out)
{
	പൂർണ്ണസംഖ്യ ret = 0;

	എങ്കിൽ (funcid == SBI_EXT_TIME_SET_TIMER) {
#എങ്കിൽ __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#അല്ലെങ്കിൽ
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
	} അല്ലെങ്കിൽ
		ret = SBI_ENOTSUPP;

	തിരികെ ret;
}

ഘടന sbi_ecall_extension ecall_time;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_time_register_extensions(ശൂന്യം)
{
	തിരികെ sbi_ecall_register_extension(&ecall_time);
}

ഘടന sbi_ecall_extension ecall_time = {
	.name			= "time",
	.extid_start		= SBI_EXT_TIME,
	.extid_end		= SBI_EXT_TIME,
	.register_extensions	= sbi_ecall_time_register_extensions,
	.handle			= sbi_ecall_time_handler,
};
