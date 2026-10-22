/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_version.h>
#અન્તર્ભાવ <sbi/sbi_pmu.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>

સ્થિર પૂર્ણાંક sbi_ecall_pmu_handler(unsigned long extid, unsigned long funcid,
				 સંરચના sbi_trap_regs *regs,
				 સંરચના sbi_ecall_return *out)
{
	પૂર્ણાંક ret = 0;
	uint64_t temp;

	પસંદગી (funcid) {
	સ્થિતિ SBI_EXT_PMU_અંક_COUNTERS:
		ret = sbi_pmu_અંક_ctr();
		જો (ret >= 0) {
			out->કિંમત = ret;
			ret = 0;
		}
		તોડો;
	સ્થિતિ SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &out->કિંમત);
		તોડો;
	સ્થિતિ SBI_EXT_PMU_COUNTER_CFG_MATCH:
#જો __riscv_xlen == 32
		temp = ((uint64_t)regs->a5 << 32) | regs->a4;
#નહીંતર
		temp = regs->a4;
#endif
		ret = sbi_pmu_ctr_cfg_match(regs->a0, regs->a1, regs->a2,
					    regs->a3, temp);
		જો (ret >= 0) {
			out->કિંમત = ret;
			ret = 0;
		}

		તોડો;
	સ્થિતિ SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, false);
		out->કિંમત = temp;
		તોડો;
	સ્થિતિ SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, true);
#જો __riscv_xlen == 32
		out->કિંમત = temp >> 32;
#નહીંતર
		out->કિંમત = 0;
#endif
		તોડો;
	સ્થિતિ SBI_EXT_PMU_COUNTER_START:

#જો __riscv_xlen == 32
		temp = ((uint64_t)regs->a4 << 32) | regs->a3;
#નહીંતર
		temp = regs->a3;
#endif
		ret = sbi_pmu_ctr_start(regs->a0, regs->a1, regs->a2, temp);
		તોડો;
	સ્થિતિ SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		તોડો;
	સ્થિતિ SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		તોડો;
	સ્થિતિ SBI_EXT_PMU_SNAPSHOT_SET_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	મૂળભૂત:
		ret = SBI_ENOTSUPP;
	}

	પાછા ret;
}

સંરચના sbi_ecall_extension ecall_pmu;

સ્થિર પૂર્ણાંક sbi_ecall_pmu_register_extensions(શૂન્ય)
{
	પાછા sbi_ecall_register_extension(&ecall_pmu);
}

સંરચના sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_start		= SBI_EXT_PMU,
	.extid_end		= SBI_EXT_PMU,
	.register_extensions	= sbi_ecall_pmu_register_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
