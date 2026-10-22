/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_version.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmu.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_pmu_handler(unsigned long extid, unsigned long funcid,
				 ಸಂರಚನಾ sbi_trap_regs *regs,
				 ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;
	uint64_t temp;

	ವಿಕಲ್ಪ (funcid) {
	ಪಕ್ಷ SBI_EXT_PMU_ಅಂಕೆ_COUNTERS:
		ret = sbi_pmu_ಅಂಕೆ_ctr();
		ಯದಿ (ret >= 0) {
			out->ಮೌಲ್ಯ = ret;
			ret = 0;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &out->ಮೌಲ್ಯ);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_PMU_COUNTER_CFG_MATCH:
#ಯದಿ __riscv_xlen == 32
		temp = ((uint64_t)regs->a5 << 32) | regs->a4;
#ಅನ್ಯಥಾ
		temp = regs->a4;
#endif
		ret = sbi_pmu_ctr_cfg_match(regs->a0, regs->a1, regs->a2,
					    regs->a3, temp);
		ಯದಿ (ret >= 0) {
			out->ಮೌಲ್ಯ = ret;
			ret = 0;
		}

		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, false);
		out->ಮೌಲ್ಯ = temp;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, true);
#ಯದಿ __riscv_xlen == 32
		out->ಮೌಲ್ಯ = temp >> 32;
#ಅನ್ಯಥಾ
		out->ಮೌಲ್ಯ = 0;
#endif
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_PMU_COUNTER_START:

#ಯದಿ __riscv_xlen == 32
		temp = ((uint64_t)regs->a4 << 32) | regs->a3;
#ಅನ್ಯಥಾ
		temp = regs->a3;
#endif
		ret = sbi_pmu_ctr_start(regs->a0, regs->a1, regs->a2, temp);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_PMU_SNAPSHOT_SET_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	ಸ್ವಭಾವ:
		ret = SBI_ENOTSUPP;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸಂರಚನಾ sbi_ecall_extension ecall_pmu;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_pmu_register_extensions(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ sbi_ecall_register_extension(&ecall_pmu);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_start		= SBI_EXT_PMU,
	.extid_end		= SBI_EXT_PMU,
	.register_extensions	= sbi_ecall_pmu_register_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
