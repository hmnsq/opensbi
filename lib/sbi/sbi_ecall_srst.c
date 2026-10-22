/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_srst_handler(unsigned long extid, unsigned long funcid,
				  ഘടന sbi_trap_regs *regs,
				  ഘടന sbi_ecall_return *out)
{
	എങ്കിൽ (funcid == SBI_EXT_SRST_RESET) {
		എങ്കിൽ ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			തിരികെ SBI_EINVAL;

		തിരഞ്ഞെടുപ്പ് (regs->a0) {
		അവസ്ഥ SBI_SRST_RESET_TYPE_SHUTDOWN:
		അവസ്ഥ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		അവസ്ഥ SBI_SRST_RESET_TYPE_WARM_REBOOT:
			നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
			തിരികെ SBI_EINVAL;
		}

		തിരഞ്ഞെടുപ്പ് (regs->a1) {
		അവസ്ഥ SBI_SRST_RESET_REASON_NONE:
		അവസ്ഥ SBI_SRST_RESET_REASON_SYSFAIL:
			നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
			തിരികെ SBI_EINVAL;
		}

		എങ്കിൽ (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	തിരികെ SBI_ENOTSUPP;
}

സ്ഥിതം bool srst_available(ശൂന്യം)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	വേണ്ടി (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		എങ്കിൽ (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			തിരികെ true;
	}

	തിരികെ false;
}

ഘടന sbi_ecall_extension ecall_srst;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_srst_register_extensions(ശൂന്യം)
{
	എങ്കിൽ (!srst_available())
		തിരികെ 0;

	തിരികെ sbi_ecall_register_extension(&ecall_srst);
}

ഘടന sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
