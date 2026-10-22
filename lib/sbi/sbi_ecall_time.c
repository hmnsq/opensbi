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

ساکن صحیح sbi_ecall_وقت_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				  ساخت sbi_trap_regs *regs,
				  ساخت sbi_ecall_return *برآمد)
{
	صحیح ret = 0;

	اگر (funcid == SBI_EXT_وقت_سیٹ_TIMER) {
#اگر __riscv_xlen == 32
		sbi_timer_smode_event_آغاز((((u64)regs->a1 << 32) | (u64)regs->a0));
#ورنہ
		sbi_timer_smode_event_آغاز((u64)regs->a0);
#اگر_ختم
	} ورنہ
		ret = SBI_ENOTSUPP;

	واپس ret;
}

ساخت sbi_ecall_extension ecall_وقت;

ساکن صحیح sbi_ecall_وقت_رجسٹر_extensions(خالی)
{
	واپس sbi_ecall_رجسٹر_extension(&ecall_وقت);
}

ساخت sbi_ecall_extension ecall_وقت = {
	.name			= "time",
	.extid_آغاز		= SBI_EXT_وقت,
	.extid_انتہا		= SBI_EXT_وقت,
	.رجسٹر_extensions	= sbi_ecall_وقت_رجسٹر_extensions,
	.handle			= sbi_ecall_وقت_handler,
};
