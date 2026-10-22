/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_cppc.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_cppc_handler(unsigned long extid, unsigned long funcid,
				  ସଂରଚନା sbi_trap_regs *regs,
				  ସଂରଚନା sbi_ecall_return *out)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
	uint64_t temp;

	ବାଛ (funcid) {
	ସ୍ଥିତି SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &temp);
		out->ମୂଲ୍ୟ = temp;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_CPPC_READ_HI:
#ଯଦି __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &temp);
		out->ମୂଲ୍ୟ = temp >> 32;
#ଅନ୍ୟଥା
		out->ମୂଲ୍ୟ = 0;
#endif
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_CPPC_WRITE:
#ଯଦି __riscv_xlen == 32
		temp = regs->a2;
		temp = (temp << 32) | regs->a1;
#ଅନ୍ୟଥା
		temp = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, temp);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		ଯଦି (ret >= 0) {
			out->ମୂଲ୍ୟ = ret;
			ret = 0;
		}
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ret = SBI_ENOTSUPP;
	}

	ଫେରନ୍ତୁ ret;
}

ସଂରଚନା sbi_ecall_extension ecall_cppc;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_cppc_register_extensions(ଶୂନ୍ୟ)
{
	ଯଦି (!sbi_cppc_get_device())
		ଫେରନ୍ତୁ 0;

	ଫେରନ୍ତୁ sbi_ecall_register_extension(&ecall_cppc);
}

ସଂରଚନା sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_start		= SBI_EXT_CPPC,
	.extid_end		= SBI_EXT_CPPC,
	.register_extensions	= sbi_ecall_cppc_register_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
