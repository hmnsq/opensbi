/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_ipi.h>

स्थिर पूर्णाङ्क sbi_ecall_ipi_handler(unsigned long extid, unsigned long funcid,
				 संरचना sbi_trap_regs *regs,
				 संरचना sbi_ecall_return *out)
{
	पूर्णाङ्क ret = 0;

	जइ (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	अण्णहा
		ret = SBI_ENOTSUPP;

	प्रत्यागम ret;
}

संरचना sbi_ecall_extension ecall_ipi;

स्थिर पूर्णाङ्क sbi_ecall_ipi_register_extensions(शून्य)
{
	प्रत्यागम sbi_ecall_register_extension(&ecall_ipi);
}

संरचना sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_start		= SBI_EXT_IPI,
	.extid_end		= SBI_EXT_IPI,
	.register_extensions	= sbi_ecall_ipi_register_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
