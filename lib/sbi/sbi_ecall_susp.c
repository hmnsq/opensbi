// SPDX-License-Identifier: BSD-2-Clause
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_system.h>

स्थिर पूर्णाङ्क sbi_ecall_susp_handler(अचिह्नित दीर्घ extid, अचिह्नित दीर्घ funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *बहिर्गमनम्)
{
	पूर्णाङ्क ret = SBI_ENOTSUPP;

	यदि (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	यदि (ret >= 0) {
		बहिर्गमनम्->मानम् = ret;
		ret = 0;
	}

	प्रत्यागम ret;
}

स्थिर bool susp_उपलब्धम्(शून्य)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	प्रति (type = 0; type <= SBI_SUSP_SLEEP_TYPE_चरमः; type++) {
		यदि (sbi_system_suspend_supported(type))
			प्रत्यागम true;
	}

	प्रत्यागम false;
}

संरचना sbi_ecall_extension ecall_susp;

स्थिर पूर्णाङ्क sbi_ecall_susp_पञ्जिकाकोशः_extensions(शून्य)
{
	यदि (!susp_उपलब्धम्())
		प्रत्यागम 0;

	प्रत्यागम sbi_ecall_पञ्जिकाकोशः_extension(&ecall_susp);
}

संरचना sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_अन्तः		= SBI_EXT_SUSP,
	.पञ्जिकाकोशः_extensions	= sbi_ecall_susp_पञ्जिकाकोशः_extensions,
	.handle			= sbi_ecall_susp_handler,
};
