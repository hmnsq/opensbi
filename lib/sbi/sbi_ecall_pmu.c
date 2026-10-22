/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_version.h>
#অন্তর্ভাব <sbi/sbi_pmu.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/riscv_asm.h>

স্থির পূর্ণসংখ্যা sbi_ecall_pmu_handler(unsigned long extid, unsigned long funcid,
				 কাঠামো sbi_trap_regs *regs,
				 কাঠামো sbi_ecall_return *out)
{
	পূর্ণসংখ্যা ret = 0;
	uint64_t অস্থায়ী;

	নির্বাচন (funcid) {
	ক্ষেত্র SBI_EXT_PMU_অঙ্ক_COUNTERS:
		ret = sbi_pmu_অঙ্ক_ctr();
		যদি (ret >= 0) {
			out->মান = ret;
			ret = 0;
		}
		বিরতি;
	ক্ষেত্র SBI_EXT_PMU_গণক_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &out->মান);
		বিরতি;
	ক্ষেত্র SBI_EXT_PMU_গণক_CFG_MATCH:
#যদি __riscv_xlen == 32
		অস্থায়ী = ((uint64_t)regs->a5 << 32) | regs->a4;
#অন্যথা
		অস্থায়ী = regs->a4;
#endif
		ret = sbi_pmu_ctr_cfg_match(regs->a0, regs->a1, regs->a2,
					    regs->a3, অস্থায়ী);
		যদি (ret >= 0) {
			out->মান = ret;
			ret = 0;
		}

		বিরতি;
	ক্ষেত্র SBI_EXT_PMU_গণক_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &অস্থায়ী, false);
		out->মান = অস্থায়ী;
		বিরতি;
	ক্ষেত্র SBI_EXT_PMU_গণক_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &অস্থায়ী, true);
#যদি __riscv_xlen == 32
		out->মান = অস্থায়ী >> 32;
#অন্যথা
		out->মান = 0;
#endif
		বিরতি;
	ক্ষেত্র SBI_EXT_PMU_গণক_START:

#যদি __riscv_xlen == 32
		অস্থায়ী = ((uint64_t)regs->a4 << 32) | regs->a3;
#অন্যথা
		অস্থায়ী = regs->a3;
#endif
		ret = sbi_pmu_ctr_start(regs->a0, regs->a1, regs->a2, অস্থায়ী);
		বিরতি;
	ক্ষেত্র SBI_EXT_PMU_গণক_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		বিরতি;
	ক্ষেত্র SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		বিরতি;
	ক্ষেত্র SBI_EXT_PMU_SNAPSHOT_SET_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	পূর্বনির্ধারিত:
		ret = SBI_ENOTSUPP;
	}

	ফেরত ret;
}

কাঠামো sbi_ecall_extension ecall_pmu;

স্থির পূর্ণসংখ্যা sbi_ecall_pmu_register_extensions(শূন্য)
{
	ফেরত sbi_ecall_register_extension(&ecall_pmu);
}

কাঠামো sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_start		= SBI_EXT_PMU,
	.extid_end		= SBI_EXT_PMU,
	.register_extensions	= sbi_ecall_pmu_register_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
