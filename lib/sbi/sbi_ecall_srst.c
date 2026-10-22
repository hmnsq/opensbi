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
#अन्तर्भाव <sbi/sbi_system.h>

स्थिर पूर्णाङ्क sbi_ecall_srst_handler(unsigned long extid, unsigned long funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *out)
{
	सचे (funcid == SBI_EXT_SRST_RESET) {
		सचे ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			पटिनिवत्त SBI_EINVAL;

		विकल्प (regs->a0) {
		पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
		पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
		पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
			भङ्ग;
		स्वभाव:
			पटिनिवत्त SBI_EINVAL;
		}

		विकल्प (regs->a1) {
		पक्ष SBI_SRST_RESET_REASON_NONE:
		पक्ष SBI_SRST_RESET_REASON_SYSFAIL:
			भङ्ग;
		स्वभाव:
			पटिनिवत्त SBI_EINVAL;
		}

		सचे (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	पटिनिवत्त SBI_ENOTSUPP;
}

स्थिर bool srst_available(शून्य)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	अत्थाय (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		सचे (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			पटिनिवत्त true;
	}

	पटिनिवत्त false;
}

संरचना sbi_ecall_extension ecall_srst;

स्थिर पूर्णाङ्क sbi_ecall_srst_register_extensions(शून्य)
{
	सचे (!srst_available())
		पटिनिवत्त 0;

	पटिनिवत्त sbi_ecall_register_extension(&ecall_srst);
}

संरचना sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
