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

ساكن صحيح sbi_ecall_زمان_handler(بیعلامت دراز extid, بیعلامت دراز funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *بیرون)
{
	صحيح ret = 0;

	إذا (funcid == SBI_EXT_زمان_مجموعه_TIMER) {
#إذا __riscv_xlen == 32
		sbi_timer_smode_event_شروع((((u64)regs->a1 << 32) | (u64)regs->a0));
#وإلا
		sbi_timer_smode_event_شروع((u64)regs->a0);
#پایان_اگر
	} وإلا
		ret = SBI_ENOTSUPP;

	أرجع ret;
}

بنية sbi_ecall_extension ecall_زمان;

ساكن صحيح sbi_ecall_زمان_ثبات_extensions(فراغ)
{
	أرجع sbi_ecall_ثبات_extension(&ecall_زمان);
}

بنية sbi_ecall_extension ecall_زمان = {
	.name			= "time",
	.extid_شروع		= SBI_EXT_زمان,
	.extid_انتها		= SBI_EXT_زمان,
	.ثبات_extensions	= sbi_ecall_زمان_ثبات_extensions,
	.handle			= sbi_ecall_زمان_handler,
};
