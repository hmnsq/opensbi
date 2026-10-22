/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_cppc.h>

ایستا صحیح sbi_ecall_cppc_handler(unsigned long extid, unsigned long funcid,
				  ساختار sbi_trap_regs *regs,
				  ساختار sbi_ecall_return *out)
{
	صحیح ret = 0;
	uint64_t temp;

	انتخاب (funcid) {
	حالت SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &temp);
		out->مقدار = temp;
		بشکن;
	حالت SBI_EXT_CPPC_READ_HI:
#اگر __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &temp);
		out->مقدار = temp >> 32;
#وگرنه
		out->مقدار = 0;
#endif
		بشکن;
	حالت SBI_EXT_CPPC_WRITE:
#اگر __riscv_xlen == 32
		temp = regs->a2;
		temp = (temp << 32) | regs->a1;
#وگرنه
		temp = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, temp);
		بشکن;
	حالت SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		اگر (ret >= 0) {
			out->مقدار = ret;
			ret = 0;
		}
		بشکن;
	پیشفرض:
		ret = SBI_ENOTSUPP;
	}

	بازگشت ret;
}

ساختار sbi_ecall_extension ecall_cppc;

ایستا صحیح sbi_ecall_cppc_register_extensions(تهی)
{
	اگر (!sbi_cppc_get_device())
		بازگشت 0;

	بازگشت sbi_ecall_register_extension(&ecall_cppc);
}

ساختار sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
