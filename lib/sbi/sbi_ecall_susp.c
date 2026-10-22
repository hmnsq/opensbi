// SPDX-License-Identifier: BSD-2-Clause
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_susp_handler(unsigned long extid, unsigned long funcid,
				  ಸಂರಚನಾ sbi_trap_regs *regs,
				  ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಪೂರ್ಣಾಙ್ಕ ret = SBI_ENOTSUPP;

	ಯದಿ (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	ಯದಿ (ret >= 0) {
		out->ಮೌಲ್ಯ = ret;
		ret = 0;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ bool susp_available(ಶೂನ್ಯ)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	ಪ್ರತಿ (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		ಯದಿ (sbi_system_suspend_supported(type))
			ಪ್ರತ್ಯಾಗಮ true;
	}

	ಪ್ರತ್ಯಾಗಮ false;
}

ಸಂರಚನಾ sbi_ecall_extension ecall_susp;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_susp_register_extensions(ಶೂನ್ಯ)
{
	ಯದಿ (!susp_available())
		ಪ್ರತ್ಯಾಗಮ 0;

	ಪ್ರತ್ಯಾಗಮ sbi_ecall_register_extension(&ecall_susp);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.handle			= sbi_ecall_susp_handler,
};
