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

स्थिर पूर्णाङ्क sbi_ecall_cppc_handler(अचिह्नित दीर्घ extid, अचिह्नित दीर्घ funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *बहिर्गमनम्)
{
	पूर्णाङ्क ret = 0;
	uint64_t अस्थायी;

	विकल्प (funcid) {
	पक्ष SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &अस्थायी);
		बहिर्गमनम्->मानम् = अस्थायी;
		भङ्ग;
	पक्ष SBI_EXT_CPPC_READ_HI:
#यदि __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &अस्थायी);
		बहिर्गमनम्->मानम् = अस्थायी >> 32;
#अन्यथा
		बहिर्गमनम्->मानम् = 0;
#यदि_अन्तः
		भङ्ग;
	पक्ष SBI_EXT_CPPC_WRITE:
#यदि __riscv_xlen == 32
		अस्थायी = regs->a2;
		अस्थायी = (अस्थायी << 32) | regs->a1;
#अन्यथा
		अस्थायी = regs->a1;
#यदि_अन्तः
		ret = sbi_cppc_write(regs->a0, अस्थायी);
		भङ्ग;
	पक्ष SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		यदि (ret >= 0) {
			बहिर्गमनम्->मानम् = ret;
			ret = 0;
		}
		भङ्ग;
	स्वभाव:
		ret = SBI_ENOTSUPP;
	}

	प्रत्यागम ret;
}

संरचना sbi_ecall_extension ecall_cppc;

स्थिर पूर्णाङ्क sbi_ecall_cppc_पञ्जिकाकोशः_extensions(शून्य)
{
	यदि (!sbi_cppc_get_device())
		प्रत्यागम 0;

	प्रत्यागम sbi_ecall_पञ्जिकाकोशः_extension(&ecall_cppc);
}

संरचना sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_अन्तः		= SBI_EXT_CPPC,
	.पञ्जिकाकोशः_extensions	= sbi_ecall_cppc_पञ्जिकाकोशः_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
