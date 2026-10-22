/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_cppc.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_cppc_handler(unsigned long extid, unsigned long funcid,
				  ಸಂರಚನಾ sbi_trap_regs *regs,
				  ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;
	uint64_t temp;

	ವಿಕಲ್ಪ (funcid) {
	ಪಕ್ಷ SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &temp);
		out->ಮೌಲ್ಯ = temp;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_CPPC_READ_HI:
#ಯದಿ __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &temp);
		out->ಮೌಲ್ಯ = temp >> 32;
#ಅನ್ಯಥಾ
		out->ಮೌಲ್ಯ = 0;
#endif
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_CPPC_WRITE:
#ಯದಿ __riscv_xlen == 32
		temp = regs->a2;
		temp = (temp << 32) | regs->a1;
#ಅನ್ಯಥಾ
		temp = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, temp);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		ಯದಿ (ret >= 0) {
			out->ಮೌಲ್ಯ = ret;
			ret = 0;
		}
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ret = SBI_ENOTSUPP;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸಂರಚನಾ sbi_ecall_extension ecall_cppc;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_cppc_register_extensions(ಶೂನ್ಯ)
{
	ಯದಿ (!sbi_cppc_get_device())
		ಪ್ರತ್ಯಾಗಮ 0;

	ಪ್ರತ್ಯಾಗಮ sbi_ecall_register_extension(&ecall_cppc);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
