/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_mpxy.h>

স্থির পূর্ণসংখ্যা sbi_ecall_mpxy_handler(unsigned long extid, unsigned long funcid,
				  কাঠামো sbi_trap_regs *regs,
				  কাঠামো sbi_ecall_return *out)
{
	পূর্ণসংখ্যা ret = 0;

	নির্বাচন (funcid) {
	ক্ষেত্র SBI_EXT_MPXY_GET_SHMEM_আকার:
		out->মান = sbi_mpxy_get_shmem_আকার();
		বিরতি;
	ক্ষেত্র SBI_EXT_MPXY_SET_SHMEM:
		ret = sbi_mpxy_set_shmem(regs->a0, regs->a1, regs->a2);
		বিরতি;
	ক্ষেত্র SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		বিরতি;
	ক্ষেত্র SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		বিরতি;
	ক্ষেত্র SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		বিরতি;
	ক্ষেত্র SBI_EXT_MPXY_SEND_বার্তা_WITH_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1,
					    regs->a2, &out->মান);
		বিরতি;
	ক্ষেত্র SBI_EXT_MPXY_SEND_বার্তা_WITHOUT_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1, regs->a2,
					    ফাঁকা);
		বিরতি;
	ক্ষেত্র SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &out->মান);
		বিরতি;
	পূর্বনির্ধারিত:
		ret = SBI_ENOTSUPP;
	}

	ফেরত ret;
}

কাঠামো sbi_ecall_extension ecall_mpxy;

স্থির পূর্ণসংখ্যা sbi_ecall_mpxy_register_extensions(শূন্য)
{
	যদি (!sbi_mpxy_channel_available())
		ফেরত 0;

	ফেরত sbi_ecall_register_extension(&ecall_mpxy);
}

কাঠামো sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_start		= SBI_EXT_MPXY,
	.extid_end		= SBI_EXT_MPXY,
	.register_extensions	= sbi_ecall_mpxy_register_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
