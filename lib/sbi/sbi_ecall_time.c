/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_time_handler(unsigned long extid, unsigned long funcid,
				  ಸಂರಚನಾ sbi_trap_regs *regs,
				  ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;

	ಯದಿ (funcid == SBI_EXT_TIME_SET_TIMER) {
#ಯದಿ __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#ಅನ್ಯಥಾ
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
	} ಅನ್ಯಥಾ
		ret = SBI_ENOTSUPP;

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸಂರಚನಾ sbi_ecall_extension ecall_time;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_time_register_extensions(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ sbi_ecall_register_extension(&ecall_time);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_time = {
	.name			= "time",
	.extid_start		= SBI_EXT_TIME,
	.extid_end		= SBI_EXT_TIME,
	.register_extensions	= sbi_ecall_time_register_extensions,
	.handle			= sbi_ecall_time_handler,
};
