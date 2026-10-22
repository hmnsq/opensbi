/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_timer.h>

ساكن صحيح sbi_ecall_وقت_handler(بنانشان ڊگهو extid, بنانشان ڊگهو funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *ٻاهريون)
{
	صحيح ret = 0;

	إذا (funcid == SBI_EXT_وقت_سيٽ_TIMER) {
#إذا __riscv_xlen == 32
		sbi_timer_smode_event_شروعات((((u64)regs->a1 << 32) | (u64)regs->a0));
#وإلا
		sbi_timer_smode_event_شروعات((u64)regs->a0);
#جيڪڏهن_پورو
	} وإلا
		ret = SBI_ENOTSUPP;

	أرجع ret;
}

بنية sbi_ecall_extension ecall_وقت;

ساكن صحيح sbi_ecall_وقت_رجسٽر_extensions(فراغ)
{
	أرجع sbi_ecall_رجسٽر_extension(&ecall_وقت);
}

بنية sbi_ecall_extension ecall_وقت = {
	.name			= "time",
	.extid_شروعات		= SBI_EXT_وقت,
	.extid_پڇاڙي		= SBI_EXT_وقت,
	.رجسٽر_extensions	= sbi_ecall_وقت_رجسٽر_extensions,
	.handle			= sbi_ecall_وقت_handler,
};
