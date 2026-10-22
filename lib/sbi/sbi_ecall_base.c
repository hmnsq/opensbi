/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_version.h>
#अन्तर्भाव <sbi/riscv_asm.h>

स्थिर पूर्णाङ्क sbi_ecall_आधारः_probe(अचिह्नित दीर्घ extid, अचिह्नित दीर्घ *बहिर्गमनम्_val)
{
	संरचना sbi_ecall_extension *ext;

	ext = sbi_ecall_find_extension(extid);
	यदि (!ext) {
		*बहिर्गमनम्_val = 0;
		प्रत्यागम 0;
	}

	यदि (ext->probe)
		प्रत्यागम ext->probe(extid, बहिर्गमनम्_val);

	*बहिर्गमनम्_val = 1;
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क sbi_ecall_आधारः_handler(अचिह्नित दीर्घ extid, अचिह्नित दीर्घ funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *बहिर्गमनम्)
{
	पूर्णाङ्क ret = 0;

	विकल्प (funcid) {
	पक्ष SBI_EXT_आधारः_GET_SPEC_संस्करणम्:
		बहिर्गमनम्->मानम् = (SBI_ECALL_संस्करणम्_MAJOR <<
			      SBI_SPEC_संस्करणम्_MAJOR_विस्थापनम्) &
			     (SBI_SPEC_संस्करणम्_MAJOR_आवरणम् <<
			      SBI_SPEC_संस्करणम्_MAJOR_विस्थापनम्);
		बहिर्गमनम्->मानम् = बहिर्गमनम्->मानम् | SBI_ECALL_संस्करणम्_MINOR;
		भङ्ग;
	पक्ष SBI_EXT_आधारः_GET_IMP_ID:
		बहिर्गमनम्->मानम् = sbi_ecall_get_impid();
		भङ्ग;
	पक्ष SBI_EXT_आधारः_GET_IMP_संस्करणम्:
		बहिर्गमनम्->मानम् = OPENSBI_संस्करणम्;
		भङ्ग;
	पक्ष SBI_EXT_आधारः_GET_MVENDORID:
		बहिर्गमनम्->मानम् = csr_read(CSR_MVENDORID);
		भङ्ग;
	पक्ष SBI_EXT_आधारः_GET_MARCHID:
		बहिर्गमनम्->मानम् = csr_read(CSR_MARCHID);
		भङ्ग;
	पक्ष SBI_EXT_आधारः_GET_MIMPID:
		बहिर्गमनम्->मानम् = csr_read(CSR_MIMPID);
		भङ्ग;
	पक्ष SBI_EXT_आधारः_PROBE_EXT:
		ret = sbi_ecall_आधारः_probe(regs->a0, &बहिर्गमनम्->मानम्);
		भङ्ग;
	स्वभाव:
		ret = SBI_ENOTSUPP;
	}

	प्रत्यागम ret;
}

संरचना sbi_ecall_extension ecall_आधारः;

स्थिर पूर्णाङ्क sbi_ecall_आधारः_पञ्जिकाकोशः_extensions(शून्य)
{
	प्रत्यागम sbi_ecall_पञ्जिकाकोशः_extension(&ecall_आधारः);
}

संरचना sbi_ecall_extension ecall_आधारः = {
	.name			= "base",
	.extid_start		= SBI_EXT_आधारः,
	.extid_अन्तः		= SBI_EXT_आधारः,
	.पञ्जिकाकोशः_extensions	= sbi_ecall_आधारः_पञ्जिकाकोशः_extensions,
	.handle			= sbi_ecall_आधारः_handler,
};
