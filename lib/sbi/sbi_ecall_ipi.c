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

ایستا صحیح sbi_ecall_ipi_handler(unsigned long extid, unsigned long funcid,
				 ساختار sbi_trap_regs *regs,
				 ساختار sbi_ecall_return *out)
{
	صحیح ret = 0;

	اگر (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	وگرنه
		ret = SBI_ENOTSUPP;

	بازگشت ret;
}

ساختار sbi_ecall_extension ecall_ipi;

ایستا صحیح sbi_ecall_ipi_register_extensions(تهی)
{
	بازگشت sbi_ecall_register_extension(&ecall_ipi);
}

ساختار sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_start		= SBI_EXT_IPI,
	.extid_end		= SBI_EXT_IPI,
	.register_extensions	= sbi_ecall_ipi_register_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
