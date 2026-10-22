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

স্থির পূর্ণসংখ্যা sbi_ecall_cppc_handler(unsigned long extid, unsigned long funcid,
				  কাঠামো sbi_trap_regs *regs,
				  কাঠামো sbi_ecall_return *out)
{
	পূর্ণসংখ্যা ret = 0;
	uint64_t অস্থায়ী;

	নির্বাচন (funcid) {
	ক্ষেত্র SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &অস্থায়ী);
		out->মান = অস্থায়ী;
		বিরতি;
	ক্ষেত্র SBI_EXT_CPPC_READ_HI:
#যদি __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &অস্থায়ী);
		out->মান = অস্থায়ী >> 32;
#অন্যথা
		out->মান = 0;
#endif
		বিরতি;
	ক্ষেত্র SBI_EXT_CPPC_WRITE:
#যদি __riscv_xlen == 32
		অস্থায়ী = regs->a2;
		অস্থায়ী = (অস্থায়ী << 32) | regs->a1;
#অন্যথা
		অস্থায়ী = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, অস্থায়ী);
		বিরতি;
	ক্ষেত্র SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		যদি (ret >= 0) {
			out->মান = ret;
			ret = 0;
		}
		বিরতি;
	পূর্বনির্ধারিত:
		ret = SBI_ENOTSUPP;
	}

	ফেরত ret;
}

কাঠামো sbi_ecall_extension ecall_cppc;

স্থির পূর্ণসংখ্যা sbi_ecall_cppc_register_extensions(শূন্য)
{
	যদি (!sbi_cppc_get_device())
		ফেরত 0;

	ফেরত sbi_ecall_register_extension(&ecall_cppc);
}

কাঠামো sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
