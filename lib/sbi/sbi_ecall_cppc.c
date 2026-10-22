/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_cppc.h>

স্থির পূর্ণাঙ্ক sbi_ecall_cppc_handler(unsigned long extid, unsigned long funcid,
				  সংরচনা sbi_trap_regs *regs,
				  সংরচনা sbi_ecall_return *out)
{
	পূর্ণাঙ্ক ret = 0;
	uint64_t temp;

	বিকল্প (funcid) {
	পক্ষ SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &temp);
		out->মান = temp;
		ভঙ্গ;
	পক্ষ SBI_EXT_CPPC_READ_HI:
#যদি __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &temp);
		out->মান = temp >> 32;
#অন্যথা
		out->মান = 0;
#endif
		ভঙ্গ;
	পক্ষ SBI_EXT_CPPC_WRITE:
#যদি __riscv_xlen == 32
		temp = regs->a2;
		temp = (temp << 32) | regs->a1;
#অন্যথা
		temp = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, temp);
		ভঙ্গ;
	পক্ষ SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		যদি (ret >= 0) {
			out->মান = ret;
			ret = 0;
		}
		ভঙ্গ;
	স্বভাব:
		ret = SBI_ENOTSUPP;
	}

	প্রত্যাগম ret;
}

সংরচনা sbi_ecall_extension ecall_cppc;

স্থির পূর্ণাঙ্ক sbi_ecall_cppc_register_extensions(শূন্য)
{
	যদি (!sbi_cppc_get_device())
		প্রত্যাগম 0;

	প্রত্যাগম sbi_ecall_register_extension(&ecall_cppc);
}

সংরচনা sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
