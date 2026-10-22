/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_version.h>
#ضمن <sbi/sbi_pmu.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/riscv_asm.h>

ساكن صحيح sbi_ecall_pmu_handler(بنانشانی لما extid, بنانشانی لما funcid,
				 بنية sbi_trap_regs *regs,
				 بنية sbi_ecall_return *باہرلا)
{
	صحيح ret = 0;
	uint64_t عارضی;

	بدل (funcid) {
	حالة SBI_EXT_PMU_عدد_COUNTERS:
		ret = sbi_pmu_عدد_ctr();
		إذا (ret >= 0) {
			باہرلا->قدر = ret;
			ret = 0;
		}
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &باہرلا->قدر);
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_CFG_میل:
#إذا __riscv_xlen == 32
		عارضی = ((uint64_t)regs->a5 << 32) | regs->a4;
#وإلا
		عارضی = regs->a4;
#جے_انت
		ret = sbi_pmu_ctr_cfg_میل(regs->a0, regs->a1, regs->a2,
					    regs->a3, عارضی);
		إذا (ret >= 0) {
			باہرلا->قدر = ret;
			ret = 0;
		}

		اكسر;
	حالة SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &عارضی, false);
		باہرلا->قدر = عارضی;
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &عارضی, true);
#إذا __riscv_xlen == 32
		باہرلا->قدر = عارضی >> 32;
#وإلا
		باہرلا->قدر = 0;
#جے_انت
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_شروع:

#إذا __riscv_xlen == 32
		عارضی = ((uint64_t)regs->a4 << 32) | regs->a3;
#وإلا
		عارضی = regs->a3;
#جے_انت
		ret = sbi_pmu_ctr_شروع(regs->a0, regs->a1, regs->a2, عارضی);
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		اكسر;
	حالة SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		اكسر;
	حالة SBI_EXT_PMU_SNAPSHOT_سیٹ_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_pmu;

ساكن صحيح sbi_ecall_pmu_رجسٹر_extensions(فراغ)
{
	أرجع sbi_ecall_رجسٹر_extension(&ecall_pmu);
}

بنية sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_شروع		= SBI_EXT_PMU,
	.extid_سرا		= SBI_EXT_PMU,
	.رجسٹر_extensions	= sbi_ecall_pmu_رجسٹر_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
