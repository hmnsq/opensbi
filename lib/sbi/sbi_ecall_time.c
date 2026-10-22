/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#include <sbi/sbi_error.h>
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_timer.h>

static ကိန်းပြည့် sbi_ecall_time_handler(unsigned long extid, unsigned long funcid,
				  sa_nracanaa sbi_trap_regs *regs,
				  sa_nracanaa sbi_ecall_return *out)
{
	ကိန်းပြည့် ret = 0;

	အကယ်၍ (funcid == SBI_EXT_TIME_SET_TIMER) {
#အကယ်၍ __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#မဟုတ်ပါက
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
	} မဟုတ်ပါက
		ret = SBI_ENOTSUPP;

	ပြန်ပေး ret;
}

sa_nracanaa sbi_ecall_extension ecall_time;

static ကိန်းပြည့် sbi_ecall_time_register_extensions(void)
{
	ပြန်ပေး sbi_ecall_register_extension(&ecall_time);
}

sa_nracanaa sbi_ecall_extension ecall_time = {
	.name			= "time",
	.extid_start		= SBI_EXT_TIME,
	.extid_end		= SBI_EXT_TIME,
	.register_extensions	= sbi_ecall_time_register_extensions,
	.handle			= sbi_ecall_time_handler,
};
