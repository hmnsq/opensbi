// SPDX-License-Identifier: BSD-2-Clause
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_system.h>

स्थिर पूर्णाङ्क sbi_ecall_susp_handler(unsigned long extid, unsigned long funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *out)
{
	पूर्णाङ्क ret = SBI_ENOTSUPP;

	जइ (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	जइ (ret >= 0) {
		out->मानम् = ret;
		ret = 0;
	}

	प्रत्यागम ret;
}

स्थिर bool susp_available(शून्य)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	प्रति (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		जइ (sbi_system_suspend_supported(type))
			प्रत्यागम true;
	}

	प्रत्यागम false;
}

संरचना sbi_ecall_extension ecall_susp;

स्थिर पूर्णाङ्क sbi_ecall_susp_register_extensions(शून्य)
{
	जइ (!susp_available())
		प्रत्यागम 0;

	प्रत्यागम sbi_ecall_register_extension(&ecall_susp);
}

संरचना sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.handle			= sbi_ecall_susp_handler,
};
