/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_cppc.h>

अटल पूर्णांक sbi_ecall_cppc_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  काष्ठा sbi_trap_regs *regs,
				  काष्ठा sbi_ecall_return *out)
{
	पूर्णांक ret = 0;
	uint64_t अस्थायी;

	चयन (funcid) {
	हाल SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &अस्थायी);
		out->मूल्य = अस्थायी;
		अवरोध;
	हाल SBI_EXT_CPPC_READ_HI:
#अगर __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &अस्थायी);
		out->मूल्य = अस्थायी >> 32;
#अन्यथा
		out->मूल्य = 0;
#पूर्ण_अगर
		अवरोध;
	हाल SBI_EXT_CPPC_WRITE:
#अगर __riscv_xlen == 32
		अस्थायी = regs->a2;
		अस्थायी = (अस्थायी << 32) | regs->a1;
#अन्यथा
		अस्थायी = regs->a1;
#पूर्ण_अगर
		ret = sbi_cppc_write(regs->a0, अस्थायी);
		अवरोध;
	हाल SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		अगर (ret >= 0) {
			out->मूल्य = ret;
			ret = 0;
		}
		अवरोध;
	शेष:
		ret = SBI_ENOTSUPP;
	}

	वापस ret;
}

काष्ठा sbi_ecall_extension ecall_cppc;

अटल पूर्णांक sbi_ecall_cppc_register_extensions(व्योम)
{
	अगर (!sbi_cppc_get_device())
		वापस 0;

	वापस sbi_ecall_register_extension(&ecall_cppc);
}

काष्ठा sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
