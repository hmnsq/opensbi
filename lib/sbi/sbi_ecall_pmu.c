/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_version.h>
#అన్తర్భావ <sbi/sbi_pmu.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/riscv_asm.h>

స్థిర పూర్ణాంకం sbi_ecall_pmu_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				 నిర్మాణం sbi_trap_regs *regs,
				 నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	పూర్ణాంకం ret = 0;
	uint64_t తాత్కాలిక;

	ఎంపిక (funcid) {
	స్థితి SBI_EXT_PMU_అంకె_COUNTERS:
		ret = sbi_pmu_అంకె_ctr();
		ఒకవేళ (ret >= 0) {
			బహిర్గతం->విలువ = ret;
			ret = 0;
		}
		ఆపు;
	స్థితి SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &బహిర్గతం->విలువ);
		ఆపు;
	స్థితి SBI_EXT_PMU_COUNTER_CFG_సరిపోలిక:
#ఒకవేళ __riscv_xlen == 32
		తాత్కాలిక = ((uint64_t)regs->a5 << 32) | regs->a4;
#లేకపోతే
		తాత్కాలిక = regs->a4;
#అయితే_ముగింపు
		ret = sbi_pmu_ctr_cfg_సరిపోలిక(regs->a0, regs->a1, regs->a2,
					    regs->a3, తాత్కాలిక);
		ఒకవేళ (ret >= 0) {
			బహిర్గతం->విలువ = ret;
			ret = 0;
		}

		ఆపు;
	స్థితి SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &తాత్కాలిక, false);
		బహిర్గతం->విలువ = తాత్కాలిక;
		ఆపు;
	స్థితి SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &తాత్కాలిక, true);
#ఒకవేళ __riscv_xlen == 32
		బహిర్గతం->విలువ = తాత్కాలిక >> 32;
#లేకపోతే
		బహిర్గతం->విలువ = 0;
#అయితే_ముగింపు
		ఆపు;
	స్థితి SBI_EXT_PMU_COUNTER_ప్రారంభం:

#ఒకవేళ __riscv_xlen == 32
		తాత్కాలిక = ((uint64_t)regs->a4 << 32) | regs->a3;
#లేకపోతే
		తాత్కాలిక = regs->a3;
#అయితే_ముగింపు
		ret = sbi_pmu_ctr_ప్రారంభం(regs->a0, regs->a1, regs->a2, తాత్కాలిక);
		ఆపు;
	స్థితి SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		ఆపు;
	స్థితి SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		ఆపు;
	స్థితి SBI_EXT_PMU_SNAPSHOT_సమితి_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	అప్రమేయ:
		ret = SBI_ENOTSUPP;
	}

	తిరిగి ret;
}

నిర్మాణం sbi_ecall_extension ecall_pmu;

స్థిర పూర్ణాంకం sbi_ecall_pmu_రిజిస్టరు_extensions(శూన్య)
{
	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_pmu);
}

నిర్మాణం sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_ప్రారంభం		= SBI_EXT_PMU,
	.extid_చివర		= SBI_EXT_PMU,
	.రిజిస్టరు_extensions	= sbi_ecall_pmu_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
