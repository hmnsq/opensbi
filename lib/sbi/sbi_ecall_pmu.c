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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_pmu_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				 ರಚನೆ sbi_trap_regs *regs,
				 ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಪೂರ್ಣಾಂಕ ret = 0;
	uint64_t ತಾತ್ಕಾಲಿಕ;

	ಆಯ್ಕೆ (funcid) {
	ಸ್ಥಿತಿ SBI_EXT_PMU_ಅಂಕೆ_COUNTERS:
		ret = sbi_pmu_ಅಂಕೆ_ctr();
		ಆದರೆ (ret >= 0) {
			ಹೊರಹೋಗು->ಮೌಲ್ಯ = ret;
			ret = 0;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &ಹೊರಹೋಗು->ಮೌಲ್ಯ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_PMU_COUNTER_CFG_ಹೊಂದಿಕೆ:
#ಆದರೆ __riscv_xlen == 32
		ತಾತ್ಕಾಲಿಕ = ((uint64_t)regs->a5 << 32) | regs->a4;
#ಇಲ್ಲದಿದ್ದರೆ
		ತಾತ್ಕಾಲಿಕ = regs->a4;
#ಆದರೆ_ಅಂತ್ಯ
		ret = sbi_pmu_ctr_cfg_ಹೊಂದಿಕೆ(regs->a0, regs->a1, regs->a2,
					    regs->a3, ತಾತ್ಕಾಲಿಕ);
		ಆದರೆ (ret >= 0) {
			ಹೊರಹೋಗು->ಮೌಲ್ಯ = ret;
			ret = 0;
		}

		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &ತಾತ್ಕಾಲಿಕ, false);
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = ತಾತ್ಕಾಲಿಕ;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &ತಾತ್ಕಾಲಿಕ, true);
#ಆದರೆ __riscv_xlen == 32
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = ತಾತ್ಕಾಲಿಕ >> 32;
#ಇಲ್ಲದಿದ್ದರೆ
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = 0;
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_PMU_COUNTER_ಆರಂಭ:

#ಆದರೆ __riscv_xlen == 32
		ತಾತ್ಕಾಲಿಕ = ((uint64_t)regs->a4 << 32) | regs->a3;
#ಇಲ್ಲದಿದ್ದರೆ
		ತಾತ್ಕಾಲಿಕ = regs->a3;
#ಆದರೆ_ಅಂತ್ಯ
		ret = sbi_pmu_ctr_ಆರಂಭ(regs->a0, regs->a1, regs->a2, ತಾತ್ಕಾಲಿಕ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_PMU_SNAPSHOT_ಗಣ_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	ಪೂರ್ವನಿಯೋಜಿತ:
		ret = SBI_ENOTSUPP;
	}

	ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ sbi_ecall_extension ecall_pmu;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_pmu_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_pmu);
}

ರಚನೆ sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_ಆರಂಭ		= SBI_EXT_PMU,
	.extid_ಕೊನೆ		= SBI_EXT_PMU,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_pmu_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
