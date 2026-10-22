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

ساکن صحیح sbi_ecall_mpxy_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				  ساخت sbi_trap_regs *regs,
				  ساخت sbi_ecall_return *برآمد)
{
	صحیح ret = 0;

	انتخاب (funcid) {
	حالت SBI_EXT_MPXY_GET_SHMEM_سائز:
		برآمد->قدر = sbi_mpxy_get_shmem_سائز();
		توڑو;
	حالت SBI_EXT_MPXY_سیٹ_SHMEM:
		ret = sbi_mpxy_سیٹ_shmem(regs->a0, regs->a1, regs->a2);
		توڑو;
	حالت SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		توڑو;
	حالت SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		توڑو;
	حالت SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		توڑو;
	حالت SBI_EXT_MPXY_SEND_پیغام_WITH_RESP:
		ret = sbi_mpxy_send_پیغام(regs->a0, regs->a1,
					    regs->a2, &برآمد->قدر);
		توڑو;
	حالت SBI_EXT_MPXY_SEND_پیغام_WITHOUT_RESP:
		ret = sbi_mpxy_send_پیغام(regs->a0, regs->a1, regs->a2,
					    تہی);
		توڑو;
	حالت SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &برآمد->قدر);
		توڑو;
	طےشدہ:
		ret = SBI_ENOTSUPP;
	}

	واپس ret;
}

ساخت sbi_ecall_extension ecall_mpxy;

ساکن صحیح sbi_ecall_mpxy_رجسٹر_extensions(خالی)
{
	اگر (!sbi_mpxy_channel_دستیاب())
		واپس 0;

	واپس sbi_ecall_رجسٹر_extension(&ecall_mpxy);
}

ساخت sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_آغاز		= SBI_EXT_MPXY,
	.extid_انتہا		= SBI_EXT_MPXY,
	.رجسٹر_extensions	= sbi_ecall_mpxy_رجسٹر_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
