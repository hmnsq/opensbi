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

ساكن صحيح sbi_ecall_cppc_handler(بنانشان ڊگهو extid, بنانشان ڊگهو funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *ٻاهريون)
{
	صحيح ret = 0;
	uint64_t عارضي;

	بدل (funcid) {
	حالة SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &عارضي);
		ٻاهريون->قدر = عارضي;
		اكسر;
	حالة SBI_EXT_CPPC_READ_HI:
#إذا __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &عارضي);
		ٻاهريون->قدر = عارضي >> 32;
#وإلا
		ٻاهريون->قدر = 0;
#جيڪڏهن_پورو
		اكسر;
	حالة SBI_EXT_CPPC_WRITE:
#إذا __riscv_xlen == 32
		عارضي = regs->a2;
		عارضي = (عارضي << 32) | regs->a1;
#وإلا
		عارضي = regs->a1;
#جيڪڏهن_پورو
		ret = sbi_cppc_write(regs->a0, عارضي);
		اكسر;
	حالة SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		إذا (ret >= 0) {
			ٻاهريون->قدر = ret;
			ret = 0;
		}
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_cppc;

ساكن صحيح sbi_ecall_cppc_رجسٽر_extensions(فراغ)
{
	إذا (!sbi_cppc_get_device())
		أرجع 0;

	أرجع sbi_ecall_رجسٽر_extension(&ecall_cppc);
}

بنية sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_شروعات		= SBI_EXT_CPPC,
	.extid_پڇاڙي		= SBI_EXT_CPPC,
	.رجسٽر_extensions	= sbi_ecall_cppc_رجسٽر_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
