/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_timer.h>

ساكن صحيح sbi_ecall_ویلا_handler(بنانشانی لما extid, بنانشانی لما funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *باہرلا)
{
	صحيح ret = 0;

	إذا (funcid == SBI_EXT_ویلا_سیٹ_TIMER) {
#إذا __riscv_xlen == 32
		sbi_timer_smode_event_شروع((((u64)regs->a1 << 32) | (u64)regs->a0));
#وإلا
		sbi_timer_smode_event_شروع((u64)regs->a0);
#جے_انت
	} وإلا
		ret = SBI_ENOTSUPP;

	أرجع ret;
}

بنية sbi_ecall_extension ecall_ویلا;

ساكن صحيح sbi_ecall_ویلا_رجسٹر_extensions(فراغ)
{
	أرجع sbi_ecall_رجسٹر_extension(&ecall_ویلا);
}

بنية sbi_ecall_extension ecall_ویلا = {
	.name			= "time",
	.extid_شروع		= SBI_EXT_ویلا,
	.extid_سرا		= SBI_EXT_ویلا,
	.رجسٹر_extensions	= sbi_ecall_ویلا_رجسٹر_extensions,
	.handle			= sbi_ecall_ویلا_handler,
};
