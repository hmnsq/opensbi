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

स्थिर पूर्णाङ्क sbi_ecall_cppc_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *out)
{
	पूर्णाङ्क ret = 0;
	uint64_t temp;

	विकल्प (funcid) {
	पक्ष SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &temp);
		out->मान = temp;
		रोक;
	पक्ष SBI_EXT_CPPC_READ_HI:
#यदि __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &temp);
		out->मान = temp >> 32;
#अन्यथा
		out->मान = 0;
#endif
		रोक;
	पक्ष SBI_EXT_CPPC_WRITE:
#यदि __riscv_xlen == 32
		temp = regs->a2;
		temp = (temp << 32) | regs->a1;
#अन्यथा
		temp = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, temp);
		रोक;
	पक्ष SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		यदि (ret >= 0) {
			out->मान = ret;
			ret = 0;
		}
		रोक;
	स्वभाव:
		ret = SBI_ENOTSUPP;
	}

	फर्काऊ ret;
}

संरचना sbi_ecall_extension ecall_cppc;

स्थिर पूर्णाङ्क sbi_ecall_cppc_register_extensions(शून्य)
{
	यदि (!sbi_cppc_get_device())
		फर्काऊ 0;

	फर्काऊ sbi_ecall_register_extension(&ecall_cppc);
}

संरचना sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
