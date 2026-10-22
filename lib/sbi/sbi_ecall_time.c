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

ساكن صحيح sbi_ecall_وخت_handler(بینښه اوږد extid, بینښه اوږد funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *بهر)
{
	صحيح ret = 0;

	إذا (funcid == SBI_EXT_وخت_سېټ_TIMER) {
#إذا __riscv_xlen == 32
		sbi_timer_smode_event_پیل((((u64)regs->a1 << 32) | (u64)regs->a0));
#وإلا
		sbi_timer_smode_event_پیل((u64)regs->a0);
#د_که_پای
	} وإلا
		ret = SBI_ENOTSUPP;

	أرجع ret;
}

بنية sbi_ecall_extension ecall_وخت;

ساكن صحيح sbi_ecall_وخت_ثبتوونکی_extensions(فراغ)
{
	أرجع sbi_ecall_ثبتوونکی_extension(&ecall_وخت);
}

بنية sbi_ecall_extension ecall_وخت = {
	.name			= "time",
	.extid_پیل		= SBI_EXT_وخت,
	.extid_پای_ټکی		= SBI_EXT_وخت,
	.ثبتوونکی_extensions	= sbi_ecall_وخت_ثبتوونکی_extensions,
	.handle			= sbi_ecall_وخت_handler,
};
