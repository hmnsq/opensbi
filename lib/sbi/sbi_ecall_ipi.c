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
#అన్తర్భావ <sbi/sbi_ipi.h>

స్థిర పూర్ణాంకం sbi_ecall_ipi_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				 నిర్మాణం sbi_trap_regs *regs,
				 నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	పూర్ణాంకం ret = 0;

	ఒకవేళ (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	లేకపోతే
		ret = SBI_ENOTSUPP;

	తిరిగి ret;
}

నిర్మాణం sbi_ecall_extension ecall_ipi;

స్థిర పూర్ణాంకం sbi_ecall_ipi_రిజిస్టరు_extensions(శూన్య)
{
	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_ipi);
}

నిర్మాణం sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_ప్రారంభం		= SBI_EXT_IPI,
	.extid_చివర		= SBI_EXT_IPI,
	.రిజిస్టరు_extensions	= sbi_ecall_ipi_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
