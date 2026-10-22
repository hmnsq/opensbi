// SPDX-License-Identifier: BSD-2-Clause
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_susp_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				  ਬਣਤਰ sbi_trap_regs *regs,
				  ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਪੂਰਨ ret = SBI_ENOTSUPP;

	ਜੇ (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	ਜੇ (ret >= 0) {
		ਬਾਹਰਲਾ->ਮੁੱਲ = ret;
		ret = 0;
	}

	ਵਾਪਸ ret;
}

ਸਥਿਰ bool susp_ਉਪਲਬਧ(ਖਾਲੀ)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	ਲਈ (type = 0; type <= SBI_SUSP_SLEEP_TYPE_ਆਖਰੀ; type++) {
		ਜੇ (sbi_system_suspend_supported(type))
			ਵਾਪਸ true;
	}

	ਵਾਪਸ false;
}

ਬਣਤਰ sbi_ecall_extension ecall_susp;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_susp_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਜੇ (!susp_ਉਪਲਬਧ())
		ਵਾਪਸ 0;

	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_susp);
}

ਬਣਤਰ sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_SUSP,
	.extid_ਸਿਰਾ		= SBI_EXT_SUSP,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_susp_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_susp_handler,
};
