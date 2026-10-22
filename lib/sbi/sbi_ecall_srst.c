/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_system.h>

अटल पूर्णांक sbi_ecall_srst_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  काष्ठा sbi_trap_regs *regs,
				  काष्ठा sbi_ecall_return *out)
{
	अगर (funcid == SBI_EXT_SRST_RESET) {
		अगर ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			वापस SBI_EINVAL;

		चयन (regs->a0) {
		हाल SBI_SRST_RESET_TYPE_SHUTDOWN:
		हाल SBI_SRST_RESET_TYPE_COLD_REBOOT:
		हाल SBI_SRST_RESET_TYPE_WARM_REBOOT:
			अवरोध;
		शेष:
			वापस SBI_EINVAL;
		}

		चयन (regs->a1) {
		हाल SBI_SRST_RESET_REASON_NONE:
		हाल SBI_SRST_RESET_REASON_SYSFAIL:
			अवरोध;
		शेष:
			वापस SBI_EINVAL;
		}

		अगर (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	वापस SBI_ENOTSUPP;
}

अटल bool srst_available(व्योम)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	क्रम (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		अगर (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			वापस true;
	}

	वापस false;
}

काष्ठा sbi_ecall_extension ecall_srst;

अटल पूर्णांक sbi_ecall_srst_register_extensions(व्योम)
{
	अगर (!srst_available())
		वापस 0;

	वापस sbi_ecall_register_extension(&ecall_srst);
}

काष्ठा sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
