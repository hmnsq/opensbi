/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_cppc.h>

static ཧྲིལ sbi_ecall_cppc_handler(unsigned long extid, unsigned long funcid,
				  sa_nracanaa sbi_trap_regs *regs,
				  sa_nracanaa sbi_ecall_return *out)
{
	ཧྲིལ ret = 0;
	uint64_t temp;

	switch (funcid) {
	pak_sa SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &temp);
		out->value = temp;
		bha_nga;
	pak_sa SBI_EXT_CPPC_READ_HI:
#གལ་ཏེ __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &temp);
		out->value = temp >> 32;
#དེ་མིན
		out->value = 0;
#endif
		bha_nga;
	pak_sa SBI_EXT_CPPC_WRITE:
#གལ་ཏེ __riscv_xlen == 32
		temp = regs->a2;
		temp = (temp << 32) | regs->a1;
#དེ་མིན
		temp = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, temp);
		bha_nga;
	pak_sa SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		གལ་ཏེ (ret >= 0) {
			out->value = ret;
			ret = 0;
		}
		bha_nga;
	default:
		ret = SBI_ENOTSUPP;
	}

	ཕྱིར ret;
}

sa_nracanaa sbi_ecall_extension ecall_cppc;

static ཧྲིལ sbi_ecall_cppc_register_extensions(void)
{
	གལ་ཏེ (!sbi_cppc_get_device())
		ཕྱིར 0;

	ཕྱིར sbi_ecall_register_extension(&ecall_cppc);
}

sa_nracanaa sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
