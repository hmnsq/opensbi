/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_cppc.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_cppc_handler(unsigned long extid, unsigned long funcid,
				  ഘടന sbi_trap_regs *regs,
				  ഘടന sbi_ecall_return *out)
{
	പൂർണ്ണസംഖ്യ ret = 0;
	uint64_t temp;

	തിരഞ്ഞെടുപ്പ് (funcid) {
	അവസ്ഥ SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &temp);
		out->മൂല്യം = temp;
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_CPPC_READ_HI:
#എങ്കിൽ __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &temp);
		out->മൂല്യം = temp >> 32;
#അല്ലെങ്കിൽ
		out->മൂല്യം = 0;
#endif
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_CPPC_WRITE:
#എങ്കിൽ __riscv_xlen == 32
		temp = regs->a2;
		temp = (temp << 32) | regs->a1;
#അല്ലെങ്കിൽ
		temp = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, temp);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		എങ്കിൽ (ret >= 0) {
			out->മൂല്യം = ret;
			ret = 0;
		}
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		ret = SBI_ENOTSUPP;
	}

	തിരികെ ret;
}

ഘടന sbi_ecall_extension ecall_cppc;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_cppc_register_extensions(ശൂന്യം)
{
	എങ്കിൽ (!sbi_cppc_get_device())
		തിരികെ 0;

	തിരികെ sbi_ecall_register_extension(&ecall_cppc);
}

ഘടന sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
