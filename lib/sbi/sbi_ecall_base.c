/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_version.h>
#समावेश <sbi/riscv_asm.h>

अटल पूर्णांक sbi_ecall_base_probe(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ *out_val)
{
	काष्ठा sbi_ecall_extension *ext;

	ext = sbi_ecall_find_extension(extid);
	अगर (!ext) {
		*out_val = 0;
		वापस 0;
	}

	अगर (ext->probe)
		वापस ext->probe(extid, out_val);

	*out_val = 1;
	वापस 0;
}

अटल पूर्णांक sbi_ecall_base_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  काष्ठा sbi_trap_regs *regs,
				  काष्ठा sbi_ecall_return *out)
{
	पूर्णांक ret = 0;

	चयन (funcid) {
	हाल SBI_EXT_BASE_GET_SPEC_VERSION:
		out->मूल्य = (SBI_ECALL_VERSION_MAJOR <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET) &
			     (SBI_SPEC_VERSION_MAJOR_MASK <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET);
		out->मूल्य = out->मूल्य | SBI_ECALL_VERSION_MINOR;
		अवरोध;
	हाल SBI_EXT_BASE_GET_IMP_ID:
		out->मूल्य = sbi_ecall_get_impid();
		अवरोध;
	हाल SBI_EXT_BASE_GET_IMP_VERSION:
		out->मूल्य = OPENSBI_VERSION;
		अवरोध;
	हाल SBI_EXT_BASE_GET_MVENDORID:
		out->मूल्य = csr_read(CSR_MVENDORID);
		अवरोध;
	हाल SBI_EXT_BASE_GET_MARCHID:
		out->मूल्य = csr_read(CSR_MARCHID);
		अवरोध;
	हाल SBI_EXT_BASE_GET_MIMPID:
		out->मूल्य = csr_read(CSR_MIMPID);
		अवरोध;
	हाल SBI_EXT_BASE_PROBE_EXT:
		ret = sbi_ecall_base_probe(regs->a0, &out->मूल्य);
		अवरोध;
	शेष:
		ret = SBI_ENOTSUPP;
	}

	वापस ret;
}

काष्ठा sbi_ecall_extension ecall_base;

अटल पूर्णांक sbi_ecall_base_register_extensions(व्योम)
{
	वापस sbi_ecall_register_extension(&ecall_base);
}

काष्ठा sbi_ecall_extension ecall_base = {
	.name			= "base",
	.extid_start		= SBI_EXT_BASE,
	.extid_end		= SBI_EXT_BASE,
	.register_extensions	= sbi_ecall_base_register_extensions,
	.handle			= sbi_ecall_base_handler,
};
