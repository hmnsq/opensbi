/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_cppc.h>

స్థిర పూర్ణాంకం sbi_ecall_cppc_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				  నిర్మాణం sbi_trap_regs *regs,
				  నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	పూర్ణాంకం ret = 0;
	uint64_t తాత్కాలిక;

	ఎంపిక (funcid) {
	స్థితి SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &తాత్కాలిక);
		బహిర్గతం->విలువ = తాత్కాలిక;
		ఆపు;
	స్థితి SBI_EXT_CPPC_READ_HI:
#ఒకవేళ __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &తాత్కాలిక);
		బహిర్గతం->విలువ = తాత్కాలిక >> 32;
#లేకపోతే
		బహిర్గతం->విలువ = 0;
#అయితే_ముగింపు
		ఆపు;
	స్థితి SBI_EXT_CPPC_WRITE:
#ఒకవేళ __riscv_xlen == 32
		తాత్కాలిక = regs->a2;
		తాత్కాలిక = (తాత్కాలిక << 32) | regs->a1;
#లేకపోతే
		తాత్కాలిక = regs->a1;
#అయితే_ముగింపు
		ret = sbi_cppc_write(regs->a0, తాత్కాలిక);
		ఆపు;
	స్థితి SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		ఒకవేళ (ret >= 0) {
			బహిర్గతం->విలువ = ret;
			ret = 0;
		}
		ఆపు;
	అప్రమేయ:
		ret = SBI_ENOTSUPP;
	}

	తిరిగి ret;
}

నిర్మాణం sbi_ecall_extension ecall_cppc;

స్థిర పూర్ణాంకం sbi_ecall_cppc_రిజిస్టరు_extensions(శూన్య)
{
	ఒకవేళ (!sbi_cppc_get_device())
		తిరిగి 0;

	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_cppc);
}

నిర్మాణం sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_ప్రారంభం		= SBI_EXT_CPPC,
	.extid_చివర		= SBI_EXT_CPPC,
	.రిజిస్టరు_extensions	= sbi_ecall_cppc_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
