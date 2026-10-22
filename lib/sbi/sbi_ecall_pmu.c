/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_version.h>
#अन्तर्भाव <sbi/sbi_pmu.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/riscv_asm.h>

स्थिर पूर्णाङ्क sbi_ecall_pmu_handler(unsigned long extid, unsigned long funcid,
				 संरचना sbi_trap_regs *regs,
				 संरचना sbi_ecall_return *out)
{
	पूर्णाङ्क ret = 0;
	uint64_t temp;

	विकल्प (funcid) {
	पक्ष SBI_EXT_PMU_अंक_COUNTERS:
		ret = sbi_pmu_अंक_ctr();
		यदि (ret >= 0) {
			out->मूल्य = ret;
			ret = 0;
		}
		भङ्ग;
	पक्ष SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &out->मूल्य);
		भङ्ग;
	पक्ष SBI_EXT_PMU_COUNTER_CFG_MATCH:
#यदि __riscv_xlen == 32
		temp = ((uint64_t)regs->a5 << 32) | regs->a4;
#अन्यथा
		temp = regs->a4;
#endif
		ret = sbi_pmu_ctr_cfg_match(regs->a0, regs->a1, regs->a2,
					    regs->a3, temp);
		यदि (ret >= 0) {
			out->मूल्य = ret;
			ret = 0;
		}

		भङ्ग;
	पक्ष SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, false);
		out->मूल्य = temp;
		भङ्ग;
	पक्ष SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, true);
#यदि __riscv_xlen == 32
		out->मूल्य = temp >> 32;
#अन्यथा
		out->मूल्य = 0;
#endif
		भङ्ग;
	पक्ष SBI_EXT_PMU_COUNTER_START:

#यदि __riscv_xlen == 32
		temp = ((uint64_t)regs->a4 << 32) | regs->a3;
#अन्यथा
		temp = regs->a3;
#endif
		ret = sbi_pmu_ctr_start(regs->a0, regs->a1, regs->a2, temp);
		भङ्ग;
	पक्ष SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		भङ्ग;
	पक्ष SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		भङ्ग;
	पक्ष SBI_EXT_PMU_SNAPSHOT_SET_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	स्वभाव:
		ret = SBI_ENOTSUPP;
	}

	प्रत्यागम ret;
}

संरचना sbi_ecall_extension ecall_pmu;

स्थिर पूर्णाङ्क sbi_ecall_pmu_register_extensions(शून्य)
{
	प्रत्यागम sbi_ecall_register_extension(&ecall_pmu);
}

संरचना sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_start		= SBI_EXT_PMU,
	.extid_end		= SBI_EXT_PMU,
	.register_extensions	= sbi_ecall_pmu_register_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
