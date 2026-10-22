/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/sbi_timer.h>

நிலையான முழு sbi_ecall_time_handler(unsigned long extid, unsigned long funcid,
				  கட்டமைப்பு sbi_trap_regs *regs,
				  கட்டமைப்பு sbi_ecall_return *out)
{
	முழு ret = 0;

	எனில் (funcid == SBI_EXT_TIME_SET_TIMER) {
#எனில் __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#இல்லையெனில்
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
	} இல்லையெனில்
		ret = SBI_ENOTSUPP;

	திரும்பு ret;
}

கட்டமைப்பு sbi_ecall_extension ecall_time;

நிலையான முழு sbi_ecall_time_register_extensions(வெறுமை)
{
	திரும்பு sbi_ecall_register_extension(&ecall_time);
}

கட்டமைப்பு sbi_ecall_extension ecall_time = {
	.name			= "time",
	.extid_start		= SBI_EXT_TIME,
	.extid_end		= SBI_EXT_TIME,
	.register_extensions	= sbi_ecall_time_register_extensions,
	.handle			= sbi_ecall_time_handler,
};
