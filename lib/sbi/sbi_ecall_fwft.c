/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_fwft.h>
#అన్తర్భావ <sbi/sbi_trap.h>

స్థిర పూర్ణాంకం sbi_ecall_fwft_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				 నిర్మాణం sbi_trap_regs *regs,
				 నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	పూర్ణాంకం ret = 0;

	ఎంపిక (funcid) {
	స్థితి SBI_EXT_FWFT_సమితి:
		ret = sbi_fwft_సమితి(regs->a0, regs->a1, regs->a2);
		ఆపు;
	స్థితి SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &బహిర్గతం->విలువ);
		ఆపు;
	అప్రమేయ:
		ret = SBI_ENOTSUPP;
		ఆపు;
	}

	తిరిగి ret;
}

నిర్మాణం sbi_ecall_extension ecall_fwft;

స్థిర పూర్ణాంకం sbi_ecall_fwft_రిజిస్టరు_extensions(శూన్య)
{
	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_fwft);
}

నిర్మాణం sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_ప్రారంభం		= SBI_EXT_FWFT,
	.extid_చివర		= SBI_EXT_FWFT,
	.రిజిస్టరు_extensions	= sbi_ecall_fwft_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
