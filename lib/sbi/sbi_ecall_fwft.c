/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_fwft.h>
#अन्तर्भाव <sbi/sbi_trap.h>

स्थिर पूर्णाङ्क sbi_ecall_fwft_handler(अचिह्नित दीर्घ extid, अचिह्नित दीर्घ funcid,
				 संरचना sbi_trap_regs *regs,
				 संरचना sbi_ecall_return *बहिर्गमनम्)
{
	पूर्णाङ्क ret = 0;

	विकल्प (funcid) {
	पक्ष SBI_EXT_FWFT_समुच्चयः:
		ret = sbi_fwft_समुच्चयः(regs->a0, regs->a1, regs->a2);
		भङ्ग;
	पक्ष SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &बहिर्गमनम्->मानम्);
		भङ्ग;
	स्वभाव:
		ret = SBI_ENOTSUPP;
		भङ्ग;
	}

	प्रत्यागम ret;
}

संरचना sbi_ecall_extension ecall_fwft;

स्थिर पूर्णाङ्क sbi_ecall_fwft_पञ्जिकाकोशः_extensions(शून्य)
{
	प्रत्यागम sbi_ecall_पञ्जिकाकोशः_extension(&ecall_fwft);
}

संरचना sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_start		= SBI_EXT_FWFT,
	.extid_अन्तः		= SBI_EXT_FWFT,
	.पञ्जिकाकोशः_extensions	= sbi_ecall_fwft_पञ्जिकाकोशः_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
