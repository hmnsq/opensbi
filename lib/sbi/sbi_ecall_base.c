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

स्थिर पूर्णांक sbi_ecall_base_probe(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ *out_val)
{
	संरचना sbi_ecall_extension *ext;

	ext = sbi_ecall_find_extension(extid);
	जर (!ext) {
		*out_val = 0;
		परत 0;
	}

	जर (ext->probe)
		परत ext->probe(extid, out_val);

	*out_val = 1;
	परत 0;
}

स्थिर पूर्णांक sbi_ecall_base_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *out)
{
	पूर्णांक ret = 0;

	निवड (funcid) {
	स्थिती SBI_EXT_BASE_GET_SPEC_VERSION:
		out->मूल्य = (SBI_ECALL_VERSION_MAJOR <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET) &
			     (SBI_SPEC_VERSION_MAJOR_MASK <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET);
		out->मूल्य = out->मूल्य | SBI_ECALL_VERSION_MINOR;
		थांबा;
	स्थिती SBI_EXT_BASE_GET_IMP_ID:
		out->मूल्य = sbi_ecall_get_impid();
		थांबा;
	स्थिती SBI_EXT_BASE_GET_IMP_VERSION:
		out->मूल्य = OPENSBI_VERSION;
		थांबा;
	स्थिती SBI_EXT_BASE_GET_MVENDORID:
		out->मूल्य = csr_read(CSR_MVENDORID);
		थांबा;
	स्थिती SBI_EXT_BASE_GET_MARCHID:
		out->मूल्य = csr_read(CSR_MARCHID);
		थांबा;
	स्थिती SBI_EXT_BASE_GET_MIMPID:
		out->मूल्य = csr_read(CSR_MIMPID);
		थांबा;
	स्थिती SBI_EXT_BASE_PROBE_EXT:
		ret = sbi_ecall_base_probe(regs->a0, &out->मूल्य);
		थांबा;
	पूर्वनिर्धारित:
		ret = SBI_ENOTSUPP;
	}

	परत ret;
}

संरचना sbi_ecall_extension ecall_base;

स्थिर पूर्णांक sbi_ecall_base_register_extensions(शून्य)
{
	परत sbi_ecall_register_extension(&ecall_base);
}

संरचना sbi_ecall_extension ecall_base = {
	.name			= "base",
	.extid_start		= SBI_EXT_BASE,
	.extid_end		= SBI_EXT_BASE,
	.register_extensions	= sbi_ecall_base_register_extensions,
	.handle			= sbi_ecall_base_handler,
};
