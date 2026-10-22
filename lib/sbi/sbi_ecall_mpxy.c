/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_mpxy.h>

ساكن صحيح sbi_ecall_mpxy_handler(بیعلامت دراز extid, بیعلامت دراز funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *بیرون)
{
	صحيح ret = 0;

	بدل (funcid) {
	حالة SBI_EXT_MPXY_GET_SHMEM_اندازه:
		بیرون->مقدار = sbi_mpxy_get_shmem_اندازه();
		اكسر;
	حالة SBI_EXT_MPXY_مجموعه_SHMEM:
		ret = sbi_mpxy_مجموعه_shmem(regs->a0, regs->a1, regs->a2);
		اكسر;
	حالة SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		اكسر;
	حالة SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		اكسر;
	حالة SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		اكسر;
	حالة SBI_EXT_MPXY_SEND_پیام_WITH_RESP:
		ret = sbi_mpxy_send_پیام(regs->a0, regs->a1,
					    regs->a2, &بیرون->مقدار);
		اكسر;
	حالة SBI_EXT_MPXY_SEND_پیام_WITHOUT_RESP:
		ret = sbi_mpxy_send_پیام(regs->a0, regs->a1, regs->a2,
					    عدم);
		اكسر;
	حالة SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &بیرون->مقدار);
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_mpxy;

ساكن صحيح sbi_ecall_mpxy_ثبات_extensions(فراغ)
{
	إذا (!sbi_mpxy_channel_موجود())
		أرجع 0;

	أرجع sbi_ecall_ثبات_extension(&ecall_mpxy);
}

بنية sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_شروع		= SBI_EXT_MPXY,
	.extid_انتها		= SBI_EXT_MPXY,
	.ثبات_extensions	= sbi_ecall_mpxy_ثبات_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
