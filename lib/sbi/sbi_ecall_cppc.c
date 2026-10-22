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

static целое sbi_ecall_cppc_handler(unsigned long extid, unsigned long funcid,
				  struct sbi_trap_regs *regs,
				  struct sbi_ecall_return *out)
{
	целое ret = 0;
	uint64_t temp;

	switch (funcid) {
	case SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &temp);
		out->value = temp;
		break;
	case SBI_EXT_CPPC_READ_HI:
#если __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &temp);
		out->value = temp >> 32;
#иначе
		out->value = 0;
#endif
		break;
	case SBI_EXT_CPPC_WRITE:
#если __riscv_xlen == 32
		temp = regs->a2;
		temp = (temp << 32) | regs->a1;
#иначе
		temp = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, temp);
		break;
	case SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		если (ret >= 0) {
			out->value = ret;
			ret = 0;
		}
		break;
	default:
		ret = SBI_ENOTSUPP;
	}

	вернуть ret;
}

struct sbi_ecall_extension ecall_cppc;

static целое sbi_ecall_cppc_register_extensions(void)
{
	если (!sbi_cppc_get_device())
		вернуть 0;

	вернуть sbi_ecall_register_extension(&ecall_cppc);
}

struct sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
