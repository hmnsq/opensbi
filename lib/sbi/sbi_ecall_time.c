/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_timer.h>

अटल पूर्णांक sbi_ecall_time_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  काष्ठा sbi_trap_regs *regs,
				  काष्ठा sbi_ecall_return *out)
{
	पूर्णांक ret = 0;

	अगर (funcid == SBI_EXT_TIME_SET_TIMER) {
#अगर __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#अन्यथा
		sbi_timer_smode_event_start((u64)regs->a0);
#पूर्ण_अगर
	} अन्यथा
		ret = SBI_ENOTSUPP;

	वापस ret;
}

काष्ठा sbi_ecall_extension ecall_time;

अटल पूर्णांक sbi_ecall_time_register_extensions(व्योम)
{
	वापस sbi_ecall_register_extension(&ecall_time);
}

काष्ठा sbi_ecall_extension ecall_time = {
	.name			= "time",
	.extid_start		= SBI_EXT_TIME,
	.extid_end		= SBI_EXT_TIME,
	.register_extensions	= sbi_ecall_time_register_extensions,
	.handle			= sbi_ecall_time_handler,
};
