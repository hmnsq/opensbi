/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাৱ <sbi/sbi_ecall.h>
#অন্তর্ভাৱ <sbi/sbi_ecall_interface.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_hart.h>
#অন্তর্ভাৱ <sbi/sbi_trap.h>
#অন্তর্ভাৱ <sbi/sbi_version.h>
#অন্তর্ভাৱ <sbi/sbi_pmu.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi/riscv_asm.h>

স্থির পূর্ণাঙ্ক sbi_ecall_pmu_handler(unsigned long extid, unsigned long funcid,
				 সংরচনা sbi_trap_regs *regs,
				 সংরচনা sbi_ecall_return *out)
{
	পূর্ণাঙ্ক ret = 0;
	uint64_t temp;

	ৱিকল্প (funcid) {
	পক্ষ SBI_EXT_PMU_অঙ্ক_COUNTERS:
		ret = sbi_pmu_অঙ্ক_ctr();
		যদি (ret >= 0) {
			out->মান = ret;
			ret = 0;
		}
		ভঙ্গ;
	পক্ষ SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &out->মান);
		ভঙ্গ;
	পক্ষ SBI_EXT_PMU_COUNTER_CFG_MATCH:
#যদি __riscv_xlen == 32
		temp = ((uint64_t)regs->a5 << 32) | regs->a4;
#অন্যথা
		temp = regs->a4;
#endif
		ret = sbi_pmu_ctr_cfg_match(regs->a0, regs->a1, regs->a2,
					    regs->a3, temp);
		যদি (ret >= 0) {
			out->মান = ret;
			ret = 0;
		}

		ভঙ্গ;
	পক্ষ SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, false);
		out->মান = temp;
		ভঙ্গ;
	পক্ষ SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, true);
#যদি __riscv_xlen == 32
		out->মান = temp >> 32;
#অন্যথা
		out->মান = 0;
#endif
		ভঙ্গ;
	পক্ষ SBI_EXT_PMU_COUNTER_START:

#যদি __riscv_xlen == 32
		temp = ((uint64_t)regs->a4 << 32) | regs->a3;
#অন্যথা
		temp = regs->a3;
#endif
		ret = sbi_pmu_ctr_start(regs->a0, regs->a1, regs->a2, temp);
		ভঙ্গ;
	পক্ষ SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		ভঙ্গ;
	পক্ষ SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		ভঙ্গ;
	পক্ষ SBI_EXT_PMU_SNAPSHOT_SET_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	স্ৱভাৱ:
		ret = SBI_ENOTSUPP;
	}

	ঘূৰাই ret;
}

সংরচনা sbi_ecall_extension ecall_pmu;

স্থির পূর্ণাঙ্ক sbi_ecall_pmu_register_extensions(শূন্য)
{
	ঘূৰাই sbi_ecall_register_extension(&ecall_pmu);
}

সংরচনা sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_start		= SBI_EXT_PMU,
	.extid_end		= SBI_EXT_PMU,
	.register_extensions	= sbi_ecall_pmu_register_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
