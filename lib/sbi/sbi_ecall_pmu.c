/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_version.h>
#شامل <sbi/sbi_pmu.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/riscv_asm.h>

ساکن صحیح sbi_ecall_pmu_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				 ساخت sbi_trap_regs *regs,
				 ساخت sbi_ecall_return *برآمد)
{
	صحیح ret = 0;
	uint64_t عارضی;

	انتخاب (funcid) {
	حالت SBI_EXT_PMU_عدد_COUNTERS:
		ret = sbi_pmu_عدد_ctr();
		اگر (ret >= 0) {
			برآمد->قدر = ret;
			ret = 0;
		}
		توڑو;
	حالت SBI_EXT_PMU_شمار_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &برآمد->قدر);
		توڑو;
	حالت SBI_EXT_PMU_شمار_CFG_مطابقت:
#اگر __riscv_xlen == 32
		عارضی = ((uint64_t)regs->a5 << 32) | regs->a4;
#ورنہ
		عارضی = regs->a4;
#اگر_ختم
		ret = sbi_pmu_ctr_cfg_مطابقت(regs->a0, regs->a1, regs->a2,
					    regs->a3, عارضی);
		اگر (ret >= 0) {
			برآمد->قدر = ret;
			ret = 0;
		}

		توڑو;
	حالت SBI_EXT_PMU_شمار_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &عارضی, false);
		برآمد->قدر = عارضی;
		توڑو;
	حالت SBI_EXT_PMU_شمار_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &عارضی, true);
#اگر __riscv_xlen == 32
		برآمد->قدر = عارضی >> 32;
#ورنہ
		برآمد->قدر = 0;
#اگر_ختم
		توڑو;
	حالت SBI_EXT_PMU_شمار_آغاز:

#اگر __riscv_xlen == 32
		عارضی = ((uint64_t)regs->a4 << 32) | regs->a3;
#ورنہ
		عارضی = regs->a3;
#اگر_ختم
		ret = sbi_pmu_ctr_آغاز(regs->a0, regs->a1, regs->a2, عارضی);
		توڑو;
	حالت SBI_EXT_PMU_شمار_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		توڑو;
	حالت SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		توڑو;
	حالت SBI_EXT_PMU_SNAPSHOT_سیٹ_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	طےشدہ:
		ret = SBI_ENOTSUPP;
	}

	واپس ret;
}

ساخت sbi_ecall_extension ecall_pmu;

ساکن صحیح sbi_ecall_pmu_رجسٹر_extensions(خالی)
{
	واپس sbi_ecall_رجسٹر_extension(&ecall_pmu);
}

ساخت sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_آغاز		= SBI_EXT_PMU,
	.extid_انتہا		= SBI_EXT_PMU,
	.رجسٹر_extensions	= sbi_ecall_pmu_رجسٹر_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
