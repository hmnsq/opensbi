// SPDX-License-Identifier: BSD-2-Clause
#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_system.h>

अटल पूर्णांक sbi_ecall_susp_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  काष्ठा sbi_trap_regs *regs,
				  काष्ठा sbi_ecall_return *out)
{
	पूर्णांक ret = SBI_ENOTSUPP;

	अगर (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	अगर (ret >= 0) {
		out->मूल्य = ret;
		ret = 0;
	}

	वापस ret;
}

अटल bool susp_available(व्योम)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	क्रम (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		अगर (sbi_system_suspend_supported(type))
			वापस true;
	}

	वापस false;
}

काष्ठा sbi_ecall_extension ecall_susp;

अटल पूर्णांक sbi_ecall_susp_register_extensions(व्योम)
{
	अगर (!susp_available())
		वापस 0;

	वापस sbi_ecall_register_extension(&ecall_susp);
}

काष्ठा sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.handle			= sbi_ecall_susp_handler,
};
