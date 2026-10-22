// SPDX-License-Identifier: BSD-2-Clause
#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/sbi_system.h>

நிலையான முழு sbi_ecall_susp_handler(unsigned long extid, unsigned long funcid,
				  கட்டமைப்பு sbi_trap_regs *regs,
				  கட்டமைப்பு sbi_ecall_return *out)
{
	முழு ret = SBI_ENOTSUPP;

	எனில் (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	எனில் (ret >= 0) {
		out->மதிப்பு = ret;
		ret = 0;
	}

	திரும்பு ret;
}

நிலையான bool susp_available(வெறுமை)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	க்கு (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		எனில் (sbi_system_suspend_supported(type))
			திரும்பு true;
	}

	திரும்பு false;
}

கட்டமைப்பு sbi_ecall_extension ecall_susp;

நிலையான முழு sbi_ecall_susp_register_extensions(வெறுமை)
{
	எனில் (!susp_available())
		திரும்பு 0;

	திரும்பு sbi_ecall_register_extension(&ecall_susp);
}

கட்டமைப்பு sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.handle			= sbi_ecall_susp_handler,
};
