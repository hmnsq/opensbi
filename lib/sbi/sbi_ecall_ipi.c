/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_ipi.h>

ساکن صحیح sbi_ecall_ipi_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				 ساخت sbi_trap_regs *regs,
				 ساخت sbi_ecall_return *برآمد)
{
	صحیح ret = 0;

	اگر (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	ورنہ
		ret = SBI_ENOTSUPP;

	واپس ret;
}

ساخت sbi_ecall_extension ecall_ipi;

ساکن صحیح sbi_ecall_ipi_رجسٹر_extensions(خالی)
{
	واپس sbi_ecall_رجسٹر_extension(&ecall_ipi);
}

ساخت sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_آغاز		= SBI_EXT_IPI,
	.extid_انتہا		= SBI_EXT_IPI,
	.رجسٹر_extensions	= sbi_ecall_ipi_رجسٹر_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
