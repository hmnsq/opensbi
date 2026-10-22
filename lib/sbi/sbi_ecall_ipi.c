/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/sbi_ipi.h>

நிலையான முழு sbi_ecall_ipi_handler(unsigned long extid, unsigned long funcid,
				 கட்டமைப்பு sbi_trap_regs *regs,
				 கட்டமைப்பு sbi_ecall_return *out)
{
	முழு ret = 0;

	எனில் (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	இல்லையெனில்
		ret = SBI_ENOTSUPP;

	திரும்பு ret;
}

கட்டமைப்பு sbi_ecall_extension ecall_ipi;

நிலையான முழு sbi_ecall_ipi_register_extensions(வெறுமை)
{
	திரும்பு sbi_ecall_register_extension(&ecall_ipi);
}

கட்டமைப்பு sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_start		= SBI_EXT_IPI,
	.extid_end		= SBI_EXT_IPI,
	.register_extensions	= sbi_ecall_ipi_register_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
