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
#అన్తర్భావ <sbi/sbi_system.h>

స్థిర పూర్ణాంకం sbi_ecall_srst_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				  నిర్మాణం sbi_trap_regs *regs,
				  నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	ఒకవేళ (funcid == SBI_EXT_SRST_RESET) {
		ఒకవేళ ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			తిరిగి SBI_EINVAL;

		ఎంపిక (regs->a0) {
		స్థితి SBI_SRST_RESET_TYPE_SHUTDOWN:
		స్థితి SBI_SRST_RESET_TYPE_COLD_REBOOT:
		స్థితి SBI_SRST_RESET_TYPE_WARM_REBOOT:
			ఆపు;
		అప్రమేయ:
			తిరిగి SBI_EINVAL;
		}

		ఎంపిక (regs->a1) {
		స్థితి SBI_SRST_RESET_REASON_NONE:
		స్థితి SBI_SRST_RESET_REASON_SYSFAIL:
			ఆపు;
		అప్రమేయ:
			తిరిగి SBI_EINVAL;
		}

		ఒకవేళ (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	తిరిగి SBI_ENOTSUPP;
}

స్థిర bool srst_అందుబాటు(శూన్య)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	కొరకు (type = 0; type <= SBI_SRST_RESET_TYPE_చివరి; type++) {
		ఒకవేళ (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			తిరిగి true;
	}

	తిరిగి false;
}

నిర్మాణం sbi_ecall_extension ecall_srst;

స్థిర పూర్ణాంకం sbi_ecall_srst_రిజిస్టరు_extensions(శూన్య)
{
	ఒకవేళ (!srst_అందుబాటు())
		తిరిగి 0;

	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_srst);
}

నిర్మాణం sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_ప్రారంభం		= SBI_EXT_SRST,
	.extid_చివర		= SBI_EXT_SRST,
	.రిజిస్టరు_extensions	= sbi_ecall_srst_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_srst_handler,
};
