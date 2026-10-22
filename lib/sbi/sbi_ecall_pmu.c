/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/sbi_version.h>
#அந்தர்பாவ <sbi/sbi_pmu.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/riscv_asm.h>

நிலையான முழு sbi_ecall_pmu_handler(unsigned long extid, unsigned long funcid,
				 கட்டமைப்பு sbi_trap_regs *regs,
				 கட்டமைப்பு sbi_ecall_return *out)
{
	முழு ret = 0;
	uint64_t temp;

	தேர்வு (funcid) {
	நிலை SBI_EXT_PMU_இலக்கம்_COUNTERS:
		ret = sbi_pmu_இலக்கம்_ctr();
		எனில் (ret >= 0) {
			out->மதிப்பு = ret;
			ret = 0;
		}
		நிறுத்து;
	நிலை SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &out->மதிப்பு);
		நிறுத்து;
	நிலை SBI_EXT_PMU_COUNTER_CFG_MATCH:
#எனில் __riscv_xlen == 32
		temp = ((uint64_t)regs->a5 << 32) | regs->a4;
#இல்லையெனில்
		temp = regs->a4;
#endif
		ret = sbi_pmu_ctr_cfg_match(regs->a0, regs->a1, regs->a2,
					    regs->a3, temp);
		எனில் (ret >= 0) {
			out->மதிப்பு = ret;
			ret = 0;
		}

		நிறுத்து;
	நிலை SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, false);
		out->மதிப்பு = temp;
		நிறுத்து;
	நிலை SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, true);
#எனில் __riscv_xlen == 32
		out->மதிப்பு = temp >> 32;
#இல்லையெனில்
		out->மதிப்பு = 0;
#endif
		நிறுத்து;
	நிலை SBI_EXT_PMU_COUNTER_START:

#எனில் __riscv_xlen == 32
		temp = ((uint64_t)regs->a4 << 32) | regs->a3;
#இல்லையெனில்
		temp = regs->a3;
#endif
		ret = sbi_pmu_ctr_start(regs->a0, regs->a1, regs->a2, temp);
		நிறுத்து;
	நிலை SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		நிறுத்து;
	நிலை SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		நிறுத்து;
	நிலை SBI_EXT_PMU_SNAPSHOT_SET_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	இயல்பு:
		ret = SBI_ENOTSUPP;
	}

	திரும்பு ret;
}

கட்டமைப்பு sbi_ecall_extension ecall_pmu;

நிலையான முழு sbi_ecall_pmu_register_extensions(வெறுமை)
{
	திரும்பு sbi_ecall_register_extension(&ecall_pmu);
}

கட்டமைப்பு sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_start		= SBI_EXT_PMU,
	.extid_end		= SBI_EXT_PMU,
	.register_extensions	= sbi_ecall_pmu_register_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
