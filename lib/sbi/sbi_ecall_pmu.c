/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_version.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_pmu.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_pmu_handler(unsigned long extid, unsigned long funcid,
				 ସଂରଚନା sbi_trap_regs *regs,
				 ସଂରଚନା sbi_ecall_return *out)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
	uint64_t temp;

	ବାଛ (funcid) {
	ସ୍ଥିତି SBI_EXT_PMU_ଅଙ୍କ_COUNTERS:
		ret = sbi_pmu_ଅଙ୍କ_ctr();
		ଯଦି (ret >= 0) {
			out->ମୂଲ୍ୟ = ret;
			ret = 0;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &out->ମୂଲ୍ୟ);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_PMU_COUNTER_CFG_MATCH:
#ଯଦି __riscv_xlen == 32
		temp = ((uint64_t)regs->a5 << 32) | regs->a4;
#ଅନ୍ୟଥା
		temp = regs->a4;
#endif
		ret = sbi_pmu_ctr_cfg_match(regs->a0, regs->a1, regs->a2,
					    regs->a3, temp);
		ଯଦି (ret >= 0) {
			out->ମୂଲ୍ୟ = ret;
			ret = 0;
		}

		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, false);
		out->ମୂଲ୍ୟ = temp;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, true);
#ଯଦି __riscv_xlen == 32
		out->ମୂଲ୍ୟ = temp >> 32;
#ଅନ୍ୟଥା
		out->ମୂଲ୍ୟ = 0;
#endif
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_PMU_COUNTER_START:

#ଯଦି __riscv_xlen == 32
		temp = ((uint64_t)regs->a4 << 32) | regs->a3;
#ଅନ୍ୟଥା
		temp = regs->a3;
#endif
		ret = sbi_pmu_ctr_start(regs->a0, regs->a1, regs->a2, temp);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_PMU_SNAPSHOT_SET_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ret = SBI_ENOTSUPP;
	}

	ଫେରନ୍ତୁ ret;
}

ସଂରଚନା sbi_ecall_extension ecall_pmu;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_pmu_register_extensions(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ sbi_ecall_register_extension(&ecall_pmu);
}

ସଂରଚନା sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_start		= SBI_EXT_PMU,
	.extid_end		= SBI_EXT_PMU,
	.register_extensions	= sbi_ecall_pmu_register_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
