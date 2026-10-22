/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_cppc.h>

ساكن صحيح sbi_ecall_cppc_handler(unsigned long extid, unsigned long funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *out)
{
	صحيح ret = 0;
	uint64_t temp;

	بدل (funcid) {
	حالة SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &temp);
		out->value = temp;
		اكسر;
	حالة SBI_EXT_CPPC_READ_HI:
#אם __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &temp);
		out->value = temp >> 32;
#אחרת
		out->value = 0;
#endif
		اكسر;
	حالة SBI_EXT_CPPC_WRITE:
#אם __riscv_xlen == 32
		temp = regs->a2;
		temp = (temp << 32) | regs->a1;
#אחרת
		temp = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, temp);
		اكسر;
	حالة SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		אם (ret >= 0) {
			out->value = ret;
			ret = 0;
		}
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	החזר ret;
}

بنية sbi_ecall_extension ecall_cppc;

ساكن صحيح sbi_ecall_cppc_register_extensions(فراغ)
{
	אם (!sbi_cppc_get_device())
		החזר 0;

	החזר sbi_ecall_register_extension(&ecall_cppc);
}

بنية sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
