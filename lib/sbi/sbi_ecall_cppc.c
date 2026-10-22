/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_cppc.h>

સ્થિર પૂર્ણાંક sbi_ecall_cppc_handler(unsigned long extid, unsigned long funcid,
				  સંરચના sbi_trap_regs *regs,
				  સંરચના sbi_ecall_return *out)
{
	પૂર્ણાંક ret = 0;
	uint64_t temp;

	પસંદગી (funcid) {
	સ્થિતિ SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &temp);
		out->કિંમત = temp;
		તોડો;
	સ્થિતિ SBI_EXT_CPPC_READ_HI:
#જો __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &temp);
		out->કિંમત = temp >> 32;
#નહીંતર
		out->કિંમત = 0;
#endif
		તોડો;
	સ્થિતિ SBI_EXT_CPPC_WRITE:
#જો __riscv_xlen == 32
		temp = regs->a2;
		temp = (temp << 32) | regs->a1;
#નહીંતર
		temp = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, temp);
		તોડો;
	સ્થિતિ SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		જો (ret >= 0) {
			out->કિંમત = ret;
			ret = 0;
		}
		તોડો;
	મૂળભૂત:
		ret = SBI_ENOTSUPP;
	}

	પાછા ret;
}

સંરચના sbi_ecall_extension ecall_cppc;

સ્થિર પૂર્ણાંક sbi_ecall_cppc_register_extensions(શૂન્ય)
{
	જો (!sbi_cppc_get_device())
		પાછા 0;

	પાછા sbi_ecall_register_extension(&ecall_cppc);
}

સંરચના sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
