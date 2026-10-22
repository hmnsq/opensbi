/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_ipi.h>

अटल पूर्णांक sbi_ecall_ipi_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				 काष्ठा sbi_trap_regs *regs,
				 काष्ठा sbi_ecall_return *out)
{
	पूर्णांक ret = 0;

	अगर (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	अन्यथा
		ret = SBI_ENOTSUPP;

	वापस ret;
}

काष्ठा sbi_ecall_extension ecall_ipi;

अटल पूर्णांक sbi_ecall_ipi_register_extensions(व्योम)
{
	वापस sbi_ecall_register_extension(&ecall_ipi);
}

काष्ठा sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_start		= SBI_EXT_IPI,
	.extid_end		= SBI_EXT_IPI,
	.register_extensions	= sbi_ecall_ipi_register_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
