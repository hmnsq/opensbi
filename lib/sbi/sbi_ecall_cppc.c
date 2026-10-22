/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_cppc.h>

ساكن صحيح sbi_ecall_cppc_handler(بینښه اوږد extid, بینښه اوږد funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *بهر)
{
	صحيح ret = 0;
	uint64_t لنډمهاله;

	بدل (funcid) {
	حالة SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &لنډمهاله);
		بهر->ارزښت = لنډمهاله;
		اكسر;
	حالة SBI_EXT_CPPC_READ_HI:
#إذا __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &لنډمهاله);
		بهر->ارزښت = لنډمهاله >> 32;
#وإلا
		بهر->ارزښت = 0;
#د_که_پای
		اكسر;
	حالة SBI_EXT_CPPC_WRITE:
#إذا __riscv_xlen == 32
		لنډمهاله = regs->a2;
		لنډمهاله = (لنډمهاله << 32) | regs->a1;
#وإلا
		لنډمهاله = regs->a1;
#د_که_پای
		ret = sbi_cppc_write(regs->a0, لنډمهاله);
		اكسر;
	حالة SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		إذا (ret >= 0) {
			بهر->ارزښت = ret;
			ret = 0;
		}
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_cppc;

ساكن صحيح sbi_ecall_cppc_ثبتوونکی_extensions(فراغ)
{
	إذا (!sbi_cppc_get_device())
		أرجع 0;

	أرجع sbi_ecall_ثبتوونکی_extension(&ecall_cppc);
}

بنية sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_پیل		= SBI_EXT_CPPC,
	.extid_پای_ټکی		= SBI_EXT_CPPC,
	.ثبتوونکی_extensions	= sbi_ecall_cppc_ثبتوونکی_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
