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

ایستا صحیح sbi_ecall_pmu_handler(unsigned long extid, unsigned long funcid,
				 ساختار sbi_trap_regs *regs,
				 ساختار sbi_ecall_return *out)
{
	صحیح ret = 0;
	uint64_t temp;

	انتخاب (funcid) {
	حالت SBI_EXT_PMU_رقم_COUNTERS:
		ret = sbi_pmu_رقم_ctr();
		اگر (ret >= 0) {
			out->مقدار = ret;
			ret = 0;
		}
		بشکن;
	حالت SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &out->مقدار);
		بشکن;
	حالت SBI_EXT_PMU_COUNTER_CFG_MATCH:
#اگر __riscv_xlen == 32
		temp = ((uint64_t)regs->a5 << 32) | regs->a4;
#وگرنه
		temp = regs->a4;
#endif
		ret = sbi_pmu_ctr_cfg_match(regs->a0, regs->a1, regs->a2,
					    regs->a3, temp);
		اگر (ret >= 0) {
			out->مقدار = ret;
			ret = 0;
		}

		بشکن;
	حالت SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, false);
		out->مقدار = temp;
		بشکن;
	حالت SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, true);
#اگر __riscv_xlen == 32
		out->مقدار = temp >> 32;
#وگرنه
		out->مقدار = 0;
#endif
		بشکن;
	حالت SBI_EXT_PMU_COUNTER_START:

#اگر __riscv_xlen == 32
		temp = ((uint64_t)regs->a4 << 32) | regs->a3;
#وگرنه
		temp = regs->a3;
#endif
		ret = sbi_pmu_ctr_start(regs->a0, regs->a1, regs->a2, temp);
		بشکن;
	حالت SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		بشکن;
	حالت SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		بشکن;
	حالت SBI_EXT_PMU_SNAPSHOT_SET_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	پیشفرض:
		ret = SBI_ENOTSUPP;
	}

	بازگشت ret;
}

ساختار sbi_ecall_extension ecall_pmu;

ایستا صحیح sbi_ecall_pmu_register_extensions(تهی)
{
	بازگشت sbi_ecall_register_extension(&ecall_pmu);
}

ساختار sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_start		= SBI_EXT_PMU,
	.extid_end		= SBI_EXT_PMU,
	.register_extensions	= sbi_ecall_pmu_register_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
