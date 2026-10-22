/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_cppc.h>

स्थिर पूर्णाङ्क sbi_ecall_cppc_handler(unsigned long extid, unsigned long funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *out)
{
	पूर्णाङ्क ret = 0;
	uint64_t temp;

	विकल्प (funcid) {
	पक्ष SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &temp);
		out->मानम् = temp;
		भङ्ग;
	पक्ष SBI_EXT_CPPC_READ_HI:
#जइ __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &temp);
		out->मानम् = temp >> 32;
#अण्णहा
		out->मानम् = 0;
#endif
		भङ्ग;
	पक्ष SBI_EXT_CPPC_WRITE:
#जइ __riscv_xlen == 32
		temp = regs->a2;
		temp = (temp << 32) | regs->a1;
#अण्णहा
		temp = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, temp);
		भङ्ग;
	पक्ष SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		जइ (ret >= 0) {
			out->मानम् = ret;
			ret = 0;
		}
		भङ्ग;
	स्वभाव:
		ret = SBI_ENOTSUPP;
	}

	प्रत्यागम ret;
}

संरचना sbi_ecall_extension ecall_cppc;

स्थिर पूर्णाङ्क sbi_ecall_cppc_register_extensions(शून्य)
{
	जइ (!sbi_cppc_get_device())
		प्रत्यागम 0;

	प्रत्यागम sbi_ecall_register_extension(&ecall_cppc);
}

संरचना sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
