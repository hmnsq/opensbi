/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_timer.h>

ایستا صحیح sbi_ecall_time_handler(unsigned long extid, unsigned long funcid,
				  ساختار sbi_trap_regs *regs,
				  ساختار sbi_ecall_return *out)
{
	صحیح ret = 0;

	اگر (funcid == SBI_EXT_TIME_SET_TIMER) {
#اگر __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#وگرنه
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
	} وگرنه
		ret = SBI_ENOTSUPP;

	بازگشت ret;
}

ساختار sbi_ecall_extension ecall_time;

ایستا صحیح sbi_ecall_time_register_extensions(تهی)
{
	بازگشت sbi_ecall_register_extension(&ecall_time);
}

ساختار sbi_ecall_extension ecall_time = {
	.name			= "time",
	.extid_start		= SBI_EXT_TIME,
	.extid_end		= SBI_EXT_TIME,
	.register_extensions	= sbi_ecall_time_register_extensions,
	.handle			= sbi_ecall_time_handler,
};
