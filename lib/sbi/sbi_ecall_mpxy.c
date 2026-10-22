/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_mpxy.h>

સ્થિર પૂર્ણાંક sbi_ecall_mpxy_handler(unsigned long extid, unsigned long funcid,
				  સંરચના sbi_trap_regs *regs,
				  સંરચના sbi_ecall_return *out)
{
	પૂર્ણાંક ret = 0;

	પસંદગી (funcid) {
	સ્થિતિ SBI_EXT_MPXY_GET_SHMEM_SIZE:
		out->કિંમત = sbi_mpxy_get_shmem_size();
		તોડો;
	સ્થિતિ SBI_EXT_MPXY_SET_SHMEM:
		ret = sbi_mpxy_set_shmem(regs->a0, regs->a1, regs->a2);
		તોડો;
	સ્થિતિ SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		તોડો;
	સ્થિતિ SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		તોડો;
	સ્થિતિ SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		તોડો;
	સ્થિતિ SBI_EXT_MPXY_SEND_સંદેશ_WITH_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1,
					    regs->a2, &out->કિંમત);
		તોડો;
	સ્થિતિ SBI_EXT_MPXY_SEND_સંદેશ_WITHOUT_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1, regs->a2,
					    ખાલી);
		તોડો;
	સ્થિતિ SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &out->કિંમત);
		તોડો;
	મૂળભૂત:
		ret = SBI_ENOTSUPP;
	}

	પાછા ret;
}

સંરચના sbi_ecall_extension ecall_mpxy;

સ્થિર પૂર્ણાંક sbi_ecall_mpxy_register_extensions(શૂન્ય)
{
	જો (!sbi_mpxy_channel_available())
		પાછા 0;

	પાછા sbi_ecall_register_extension(&ecall_mpxy);
}

સંરચના sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_start		= SBI_EXT_MPXY,
	.extid_end		= SBI_EXT_MPXY,
	.register_extensions	= sbi_ecall_mpxy_register_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
