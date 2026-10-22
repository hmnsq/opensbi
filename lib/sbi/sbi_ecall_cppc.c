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

ساكن صحيح sbi_ecall_cppc_handler(بنانشانی لما extid, بنانشانی لما funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *باہرلا)
{
	صحيح ret = 0;
	uint64_t عارضی;

	بدل (funcid) {
	حالة SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &عارضی);
		باہرلا->قدر = عارضی;
		اكسر;
	حالة SBI_EXT_CPPC_READ_HI:
#إذا __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &عارضی);
		باہرلا->قدر = عارضی >> 32;
#وإلا
		باہرلا->قدر = 0;
#جے_انت
		اكسر;
	حالة SBI_EXT_CPPC_WRITE:
#إذا __riscv_xlen == 32
		عارضی = regs->a2;
		عارضی = (عارضی << 32) | regs->a1;
#وإلا
		عارضی = regs->a1;
#جے_انت
		ret = sbi_cppc_write(regs->a0, عارضی);
		اكسر;
	حالة SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		إذا (ret >= 0) {
			باہرلا->قدر = ret;
			ret = 0;
		}
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_cppc;

ساكن صحيح sbi_ecall_cppc_رجسٹر_extensions(فراغ)
{
	إذا (!sbi_cppc_get_device())
		أرجع 0;

	أرجع sbi_ecall_رجسٹر_extension(&ecall_cppc);
}

بنية sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_شروع		= SBI_EXT_CPPC,
	.extid_سرا		= SBI_EXT_CPPC,
	.رجسٹر_extensions	= sbi_ecall_cppc_رجسٹر_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
