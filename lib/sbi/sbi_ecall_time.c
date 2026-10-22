/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_timer.h>

স্থির পূর্ণসংখ্যা sbi_ecall_time_handler(unsigned long extid, unsigned long funcid,
				  কাঠামো sbi_trap_regs *regs,
				  কাঠামো sbi_ecall_return *out)
{
	পূর্ণসংখ্যা ret = 0;

	যদি (funcid == SBI_EXT_TIME_SET_TIMER) {
#যদি __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#অন্যথা
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
	} অন্যথা
		ret = SBI_ENOTSUPP;

	ফেরত ret;
}

কাঠামো sbi_ecall_extension ecall_time;

স্থির পূর্ণসংখ্যা sbi_ecall_time_register_extensions(শূন্য)
{
	ফেরত sbi_ecall_register_extension(&ecall_time);
}

কাঠামো sbi_ecall_extension ecall_time = {
	.name			= "time",
	.extid_start		= SBI_EXT_TIME,
	.extid_end		= SBI_EXT_TIME,
	.register_extensions	= sbi_ecall_time_register_extensions,
	.handle			= sbi_ecall_time_handler,
};
