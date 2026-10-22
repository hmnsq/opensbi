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

स्थिर पूर्णांक sbi_ecall_srst_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *out)
{
	जर (funcid == SBI_EXT_SRST_RESET) {
		जर ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			परत SBI_EINVAL;

		निवड (regs->a0) {
		स्थिती SBI_SRST_RESET_TYPE_SHUTDOWN:
		स्थिती SBI_SRST_RESET_TYPE_COLD_REBOOT:
		स्थिती SBI_SRST_RESET_TYPE_WARM_REBOOT:
			थांबा;
		पूर्वनिर्धारित:
			परत SBI_EINVAL;
		}

		निवड (regs->a1) {
		स्थिती SBI_SRST_RESET_REASON_NONE:
		स्थिती SBI_SRST_RESET_REASON_SYSFAIL:
			थांबा;
		पूर्वनिर्धारित:
			परत SBI_EINVAL;
		}

		जर (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	परत SBI_ENOTSUPP;
}

स्थिर bool srst_available(शून्य)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	साठी (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		जर (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			परत true;
	}

	परत false;
}

संरचना sbi_ecall_extension ecall_srst;

स्थिर पूर्णांक sbi_ecall_srst_register_extensions(शून्य)
{
	जर (!srst_available())
		परत 0;

	परत sbi_ecall_register_extension(&ecall_srst);
}

संरचना sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
