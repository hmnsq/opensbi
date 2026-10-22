/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_version.h>
#അന്തര്ഭാവ <sbi/sbi_pmu.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_pmu_handler(unsigned long extid, unsigned long funcid,
				 ഘടന sbi_trap_regs *regs,
				 ഘടന sbi_ecall_return *out)
{
	പൂർണ്ണസംഖ്യ ret = 0;
	uint64_t temp;

	തിരഞ്ഞെടുപ്പ് (funcid) {
	അവസ്ഥ SBI_EXT_PMU_അക്കം_COUNTERS:
		ret = sbi_pmu_അക്കം_ctr();
		എങ്കിൽ (ret >= 0) {
			out->മൂല്യം = ret;
			ret = 0;
		}
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &out->മൂല്യം);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_PMU_COUNTER_CFG_MATCH:
#എങ്കിൽ __riscv_xlen == 32
		temp = ((uint64_t)regs->a5 << 32) | regs->a4;
#അല്ലെങ്കിൽ
		temp = regs->a4;
#endif
		ret = sbi_pmu_ctr_cfg_match(regs->a0, regs->a1, regs->a2,
					    regs->a3, temp);
		എങ്കിൽ (ret >= 0) {
			out->മൂല്യം = ret;
			ret = 0;
		}

		നിർത്തുക;
	അവസ്ഥ SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, false);
		out->മൂല്യം = temp;
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, true);
#എങ്കിൽ __riscv_xlen == 32
		out->മൂല്യം = temp >> 32;
#അല്ലെങ്കിൽ
		out->മൂല്യം = 0;
#endif
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_PMU_COUNTER_START:

#എങ്കിൽ __riscv_xlen == 32
		temp = ((uint64_t)regs->a4 << 32) | regs->a3;
#അല്ലെങ്കിൽ
		temp = regs->a3;
#endif
		ret = sbi_pmu_ctr_start(regs->a0, regs->a1, regs->a2, temp);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_PMU_SNAPSHOT_SET_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	സ്ഥിരസ്ഥിതി:
		ret = SBI_ENOTSUPP;
	}

	തിരികെ ret;
}

ഘടന sbi_ecall_extension ecall_pmu;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_pmu_register_extensions(ശൂന്യം)
{
	തിരികെ sbi_ecall_register_extension(&ecall_pmu);
}

ഘടന sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_start		= SBI_EXT_PMU,
	.extid_end		= SBI_EXT_PMU,
	.register_extensions	= sbi_ecall_pmu_register_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
