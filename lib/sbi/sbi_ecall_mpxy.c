/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাৱ <sbi/sbi_ecall.h>
#অন্তর্ভাৱ <sbi/sbi_ecall_interface.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_trap.h>
#অন্তর্ভাৱ <sbi/sbi_mpxy.h>

স্থির পূর্ণাঙ্ক sbi_ecall_mpxy_handler(unsigned long extid, unsigned long funcid,
				  সংরচনা sbi_trap_regs *regs,
				  সংরচনা sbi_ecall_return *out)
{
	পূর্ণাঙ্ক ret = 0;

	ৱিকল্প (funcid) {
	পক্ষ SBI_EXT_MPXY_GET_SHMEM_SIZE:
		out->মান = sbi_mpxy_get_shmem_size();
		ভঙ্গ;
	পক্ষ SBI_EXT_MPXY_SET_SHMEM:
		ret = sbi_mpxy_set_shmem(regs->a0, regs->a1, regs->a2);
		ভঙ্গ;
	পক্ষ SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		ভঙ্গ;
	পক্ষ SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		ভঙ্গ;
	পক্ষ SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		ভঙ্গ;
	পক্ষ SBI_EXT_MPXY_SEND_বাৰ্তা_WITH_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1,
					    regs->a2, &out->মান);
		ভঙ্গ;
	পক্ষ SBI_EXT_MPXY_SEND_বাৰ্তা_WITHOUT_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1, regs->a2,
					    রিক্ত);
		ভঙ্গ;
	পক্ষ SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &out->মান);
		ভঙ্গ;
	স্ৱভাৱ:
		ret = SBI_ENOTSUPP;
	}

	ঘূৰাই ret;
}

সংরচনা sbi_ecall_extension ecall_mpxy;

স্থির পূর্ণাঙ্ক sbi_ecall_mpxy_register_extensions(শূন্য)
{
	যদি (!sbi_mpxy_channel_available())
		ঘূৰাই 0;

	ঘূৰাই sbi_ecall_register_extension(&ecall_mpxy);
}

সংরচনা sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_start		= SBI_EXT_MPXY,
	.extid_end		= SBI_EXT_MPXY,
	.register_extensions	= sbi_ecall_mpxy_register_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
