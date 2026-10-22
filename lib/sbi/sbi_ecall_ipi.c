/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_ipi.h>

স্থির পূর্ণসংখ্যা sbi_ecall_ipi_handler(unsigned long extid, unsigned long funcid,
				 কাঠামো sbi_trap_regs *regs,
				 কাঠামো sbi_ecall_return *out)
{
	পূর্ণসংখ্যা ret = 0;

	যদি (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	অন্যথা
		ret = SBI_ENOTSUPP;

	ফেরত ret;
}

কাঠামো sbi_ecall_extension ecall_ipi;

স্থির পূর্ণসংখ্যা sbi_ecall_ipi_register_extensions(শূন্য)
{
	ফেরত sbi_ecall_register_extension(&ecall_ipi);
}

কাঠামো sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_start		= SBI_EXT_IPI,
	.extid_end		= SBI_EXT_IPI,
	.register_extensions	= sbi_ecall_ipi_register_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
