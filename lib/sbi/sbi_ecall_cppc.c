/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/sbi_cppc.h>

நிலையான முழு sbi_ecall_cppc_handler(unsigned long extid, unsigned long funcid,
				  கட்டமைப்பு sbi_trap_regs *regs,
				  கட்டமைப்பு sbi_ecall_return *out)
{
	முழு ret = 0;
	uint64_t temp;

	தேர்வு (funcid) {
	நிலை SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &temp);
		out->மதிப்பு = temp;
		நிறுத்து;
	நிலை SBI_EXT_CPPC_READ_HI:
#எனில் __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &temp);
		out->மதிப்பு = temp >> 32;
#இல்லையெனில்
		out->மதிப்பு = 0;
#endif
		நிறுத்து;
	நிலை SBI_EXT_CPPC_WRITE:
#எனில் __riscv_xlen == 32
		temp = regs->a2;
		temp = (temp << 32) | regs->a1;
#இல்லையெனில்
		temp = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, temp);
		நிறுத்து;
	நிலை SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		எனில் (ret >= 0) {
			out->மதிப்பு = ret;
			ret = 0;
		}
		நிறுத்து;
	இயல்பு:
		ret = SBI_ENOTSUPP;
	}

	திரும்பு ret;
}

கட்டமைப்பு sbi_ecall_extension ecall_cppc;

நிலையான முழு sbi_ecall_cppc_register_extensions(வெறுமை)
{
	எனில் (!sbi_cppc_get_device())
		திரும்பு 0;

	திரும்பு sbi_ecall_register_extension(&ecall_cppc);
}

கட்டமைப்பு sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
