/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_timer.h>

स्थिर पूर्णाङ्क sbi_ecall_time_handler(unsigned long extid, unsigned long funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *out)
{
	पूर्णाङ्क ret = 0;

	यदि (funcid == SBI_EXT_TIME_SET_TIMER) {
#यदि __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#अन्यथा
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
	} अन्यथा
		ret = SBI_ENOTSUPP;

	प्रत्यागम ret;
}

संरचना sbi_ecall_extension ecall_time;

स्थिर पूर्णाङ्क sbi_ecall_time_register_extensions(शून्य)
{
	प्रत्यागम sbi_ecall_register_extension(&ecall_time);
}

संरचना sbi_ecall_extension ecall_time = {
	.name			= "time",
	.extid_start		= SBI_EXT_TIME,
	.extid_end		= SBI_EXT_TIME,
	.register_extensions	= sbi_ecall_time_register_extensions,
	.handle			= sbi_ecall_time_handler,
};
