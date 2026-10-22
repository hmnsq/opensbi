/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_ipi_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				 ਬਣਤਰ sbi_trap_regs *regs,
				 ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਪੂਰਨ ret = 0;

	ਜੇ (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	ਨਹੀਂਤਾਂ
		ret = SBI_ENOTSUPP;

	ਵਾਪਸ ret;
}

ਬਣਤਰ sbi_ecall_extension ecall_ipi;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_ipi_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_ipi);
}

ਬਣਤਰ sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_IPI,
	.extid_ਸਿਰਾ		= SBI_EXT_IPI,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_ipi_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
