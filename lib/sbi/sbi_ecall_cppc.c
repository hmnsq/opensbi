/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_cppc.h>

ساکن صحیح sbi_ecall_cppc_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				  ساخت sbi_trap_regs *regs,
				  ساخت sbi_ecall_return *برآمد)
{
	صحیح ret = 0;
	uint64_t عارضی;

	انتخاب (funcid) {
	حالت SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &عارضی);
		برآمد->قدر = عارضی;
		توڑو;
	حالت SBI_EXT_CPPC_READ_HI:
#اگر __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &عارضی);
		برآمد->قدر = عارضی >> 32;
#ورنہ
		برآمد->قدر = 0;
#اگر_ختم
		توڑو;
	حالت SBI_EXT_CPPC_WRITE:
#اگر __riscv_xlen == 32
		عارضی = regs->a2;
		عارضی = (عارضی << 32) | regs->a1;
#ورنہ
		عارضی = regs->a1;
#اگر_ختم
		ret = sbi_cppc_write(regs->a0, عارضی);
		توڑو;
	حالت SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		اگر (ret >= 0) {
			برآمد->قدر = ret;
			ret = 0;
		}
		توڑو;
	طےشدہ:
		ret = SBI_ENOTSUPP;
	}

	واپس ret;
}

ساخت sbi_ecall_extension ecall_cppc;

ساکن صحیح sbi_ecall_cppc_رجسٹر_extensions(خالی)
{
	اگر (!sbi_cppc_get_device())
		واپس 0;

	واپس sbi_ecall_رجسٹر_extension(&ecall_cppc);
}

ساخت sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_آغاز		= SBI_EXT_CPPC,
	.extid_انتہا		= SBI_EXT_CPPC,
	.رجسٹر_extensions	= sbi_ecall_cppc_رجسٹر_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
