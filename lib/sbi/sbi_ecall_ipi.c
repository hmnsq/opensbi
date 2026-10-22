/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_ecall.h>
#অন্তর্ভাৱ <sbi/sbi_ecall_interface.h>
#অন্তর্ভাৱ <sbi/sbi_trap.h>
#অন্তর্ভাৱ <sbi/sbi_ipi.h>

স্থির পূর্ণাঙ্ক sbi_ecall_ipi_handler(unsigned long extid, unsigned long funcid,
				 সংরচনা sbi_trap_regs *regs,
				 সংরচনা sbi_ecall_return *out)
{
	পূর্ণাঙ্ক ret = 0;

	যদি (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	অন্যথা
		ret = SBI_ENOTSUPP;

	ঘূৰাই ret;
}

সংরচনা sbi_ecall_extension ecall_ipi;

স্থির পূর্ণাঙ্ক sbi_ecall_ipi_register_extensions(শূন্য)
{
	ঘূৰাই sbi_ecall_register_extension(&ecall_ipi);
}

সংরচনা sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_start		= SBI_EXT_IPI,
	.extid_end		= SBI_EXT_IPI,
	.register_extensions	= sbi_ecall_ipi_register_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
