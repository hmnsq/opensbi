/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_fwft.h>
#समावेश <sbi/sbi_trap.h>

अटल पूर्णांक sbi_ecall_fwft_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				 काष्ठा sbi_trap_regs *regs,
				 काष्ठा sbi_ecall_return *out)
{
	पूर्णांक ret = 0;

	चयन (funcid) {
	हाल SBI_EXT_FWFT_SET:
		ret = sbi_fwft_set(regs->a0, regs->a1, regs->a2);
		अवरोध;
	हाल SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &out->मूल्य);
		अवरोध;
	शेष:
		ret = SBI_ENOTSUPP;
		अवरोध;
	}

	वापस ret;
}

काष्ठा sbi_ecall_extension ecall_fwft;

अटल पूर्णांक sbi_ecall_fwft_register_extensions(व्योम)
{
	वापस sbi_ecall_register_extension(&ecall_fwft);
}

काष्ठा sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_start		= SBI_EXT_FWFT,
	.extid_end		= SBI_EXT_FWFT,
	.register_extensions	= sbi_ecall_fwft_register_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
