/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_mpxy.h>

స్థిర పూర్ణాంకం sbi_ecall_mpxy_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				  నిర్మాణం sbi_trap_regs *regs,
				  నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	పూర్ణాంకం ret = 0;

	ఎంపిక (funcid) {
	స్థితి SBI_EXT_MPXY_GET_SHMEM_కొలత:
		బహిర్గతం->విలువ = sbi_mpxy_get_shmem_కొలత();
		ఆపు;
	స్థితి SBI_EXT_MPXY_సమితి_SHMEM:
		ret = sbi_mpxy_సమితి_shmem(regs->a0, regs->a1, regs->a2);
		ఆపు;
	స్థితి SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		ఆపు;
	స్థితి SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		ఆపు;
	స్థితి SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		ఆపు;
	స్థితి SBI_EXT_MPXY_SEND_సందేశం_WITH_RESP:
		ret = sbi_mpxy_send_సందేశం(regs->a0, regs->a1,
					    regs->a2, &బహిర్గతం->విలువ);
		ఆపు;
	స్థితి SBI_EXT_MPXY_SEND_సందేశం_WITHOUT_RESP:
		ret = sbi_mpxy_send_సందేశం(regs->a0, regs->a1, regs->a2,
					    ఖాళీ);
		ఆపు;
	స్థితి SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &బహిర్గతం->విలువ);
		ఆపు;
	అప్రమేయ:
		ret = SBI_ENOTSUPP;
	}

	తిరిగి ret;
}

నిర్మాణం sbi_ecall_extension ecall_mpxy;

స్థిర పూర్ణాంకం sbi_ecall_mpxy_రిజిస్టరు_extensions(శూన్య)
{
	ఒకవేళ (!sbi_mpxy_channel_అందుబాటు())
		తిరిగి 0;

	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_mpxy);
}

నిర్మాణం sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_ప్రారంభం		= SBI_EXT_MPXY,
	.extid_చివర		= SBI_EXT_MPXY,
	.రిజిస్టరు_extensions	= sbi_ecall_mpxy_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
