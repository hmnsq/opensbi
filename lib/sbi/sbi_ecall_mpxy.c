/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_mpxy.h>

ایستا صحیح sbi_ecall_mpxy_handler(unsigned long extid, unsigned long funcid,
				  ساختار sbi_trap_regs *regs,
				  ساختار sbi_ecall_return *out)
{
	صحیح ret = 0;

	انتخاب (funcid) {
	حالت SBI_EXT_MPXY_GET_SHMEM_SIZE:
		out->مقدار = sbi_mpxy_get_shmem_size();
		بشکن;
	حالت SBI_EXT_MPXY_SET_SHMEM:
		ret = sbi_mpxy_set_shmem(regs->a0, regs->a1, regs->a2);
		بشکن;
	حالت SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		بشکن;
	حالت SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		بشکن;
	حالت SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		بشکن;
	حالت SBI_EXT_MPXY_SEND_پیام_WITH_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1,
					    regs->a2, &out->مقدار);
		بشکن;
	حالت SBI_EXT_MPXY_SEND_پیام_WITHOUT_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1, regs->a2,
					    پوچ);
		بشکن;
	حالت SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &out->مقدار);
		بشکن;
	پیشفرض:
		ret = SBI_ENOTSUPP;
	}

	بازگشت ret;
}

ساختار sbi_ecall_extension ecall_mpxy;

ایستا صحیح sbi_ecall_mpxy_register_extensions(تهی)
{
	اگر (!sbi_mpxy_channel_available())
		بازگشت 0;

	بازگشت sbi_ecall_register_extension(&ecall_mpxy);
}

ساختار sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_start		= SBI_EXT_MPXY,
	.extid_end		= SBI_EXT_MPXY,
	.register_extensions	= sbi_ecall_mpxy_register_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
