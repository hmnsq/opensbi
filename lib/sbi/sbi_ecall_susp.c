// SPDX-License-Identifier: BSD-2-Clause
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_susp_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				  ರಚನೆ sbi_trap_regs *regs,
				  ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಪೂರ್ಣಾಂಕ ret = SBI_ENOTSUPP;

	ಆದರೆ (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	ಆದರೆ (ret >= 0) {
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = ret;
		ret = 0;
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ bool susp_ಲಭ್ಯ(ಶೂನ್ಯ)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	ಗಾಗಿ (type = 0; type <= SBI_SUSP_SLEEP_TYPE_ಕೊನೆಯ; type++) {
		ಆದರೆ (sbi_system_suspend_supported(type))
			ಹಿಂತಿರುಗಿ true;
	}

	ಹಿಂತಿರುಗಿ false;
}

ರಚನೆ sbi_ecall_extension ecall_susp;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_susp_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಆದರೆ (!susp_ಲಭ್ಯ())
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_susp);
}

ರಚನೆ sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_ಆರಂಭ		= SBI_EXT_SUSP,
	.extid_ಕೊನೆ		= SBI_EXT_SUSP,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_susp_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_susp_handler,
};
