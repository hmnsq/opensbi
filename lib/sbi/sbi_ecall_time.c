/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_timer.h>

స్థిర పూర్ణాంకం sbi_ecall_సమయం_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				  నిర్మాణం sbi_trap_regs *regs,
				  నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	పూర్ణాంకం ret = 0;

	ఒకవేళ (funcid == SBI_EXT_సమయం_సమితి_TIMER) {
#ఒకవేళ __riscv_xlen == 32
		sbi_timer_smode_event_ప్రారంభం((((u64)regs->a1 << 32) | (u64)regs->a0));
#లేకపోతే
		sbi_timer_smode_event_ప్రారంభం((u64)regs->a0);
#అయితే_ముగింపు
	} లేకపోతే
		ret = SBI_ENOTSUPP;

	తిరిగి ret;
}

నిర్మాణం sbi_ecall_extension ecall_సమయం;

స్థిర పూర్ణాంకం sbi_ecall_సమయం_రిజిస్టరు_extensions(శూన్య)
{
	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_సమయం);
}

నిర్మాణం sbi_ecall_extension ecall_సమయం = {
	.name			= "time",
	.extid_ప్రారంభం		= SBI_EXT_సమయం,
	.extid_చివర		= SBI_EXT_సమయం,
	.రిజిస్టరు_extensions	= sbi_ecall_సమయం_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_సమయం_handler,
};
