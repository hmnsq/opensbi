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

ساكن صحيح sbi_ecall_cppc_handler(بیعلامت دراز extid, بیعلامت دراز funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *بیرون)
{
	صحيح ret = 0;
	uint64_t موقت;

	بدل (funcid) {
	حالة SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &موقت);
		بیرون->مقدار = موقت;
		اكسر;
	حالة SBI_EXT_CPPC_READ_HI:
#إذا __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &موقت);
		بیرون->مقدار = موقت >> 32;
#وإلا
		بیرون->مقدار = 0;
#پایان_اگر
		اكسر;
	حالة SBI_EXT_CPPC_WRITE:
#إذا __riscv_xlen == 32
		موقت = regs->a2;
		موقت = (موقت << 32) | regs->a1;
#وإلا
		موقت = regs->a1;
#پایان_اگر
		ret = sbi_cppc_write(regs->a0, موقت);
		اكسر;
	حالة SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		إذا (ret >= 0) {
			بیرون->مقدار = ret;
			ret = 0;
		}
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_cppc;

ساكن صحيح sbi_ecall_cppc_ثبات_extensions(فراغ)
{
	إذا (!sbi_cppc_get_device())
		أرجع 0;

	أرجع sbi_ecall_ثبات_extension(&ecall_cppc);
}

بنية sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_شروع		= SBI_EXT_CPPC,
	.extid_انتها		= SBI_EXT_CPPC,
	.ثبات_extensions	= sbi_ecall_cppc_ثبات_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
