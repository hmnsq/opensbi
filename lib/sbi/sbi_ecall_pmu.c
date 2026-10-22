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

ساكن صحيح sbi_ecall_pmu_handler(بینښه اوږد extid, بینښه اوږد funcid,
				 بنية sbi_trap_regs *regs,
				 بنية sbi_ecall_return *بهر)
{
	صحيح ret = 0;
	uint64_t لنډمهاله;

	بدل (funcid) {
	حالة SBI_EXT_PMU_عدد_COUNTERS:
		ret = sbi_pmu_عدد_ctr();
		إذا (ret >= 0) {
			بهر->ارزښت = ret;
			ret = 0;
		}
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &بهر->ارزښت);
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_CFG_سمون:
#إذا __riscv_xlen == 32
		لنډمهاله = ((uint64_t)regs->a5 << 32) | regs->a4;
#وإلا
		لنډمهاله = regs->a4;
#د_که_پای
		ret = sbi_pmu_ctr_cfg_سمون(regs->a0, regs->a1, regs->a2,
					    regs->a3, لنډمهاله);
		إذا (ret >= 0) {
			بهر->ارزښت = ret;
			ret = 0;
		}

		اكسر;
	حالة SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &لنډمهاله, false);
		بهر->ارزښت = لنډمهاله;
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &لنډمهاله, true);
#إذا __riscv_xlen == 32
		بهر->ارزښت = لنډمهاله >> 32;
#وإلا
		بهر->ارزښت = 0;
#د_که_پای
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_پیل:

#إذا __riscv_xlen == 32
		لنډمهاله = ((uint64_t)regs->a4 << 32) | regs->a3;
#وإلا
		لنډمهاله = regs->a3;
#د_که_پای
		ret = sbi_pmu_ctr_پیل(regs->a0, regs->a1, regs->a2, لنډمهاله);
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		اكسر;
	حالة SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		اكسر;
	حالة SBI_EXT_PMU_SNAPSHOT_سېټ_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_pmu;

ساكن صحيح sbi_ecall_pmu_ثبتوونکی_extensions(فراغ)
{
	أرجع sbi_ecall_ثبتوونکی_extension(&ecall_pmu);
}

بنية sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_پیل		= SBI_EXT_PMU,
	.extid_پای_ټکی		= SBI_EXT_PMU,
	.ثبتوونکی_extensions	= sbi_ecall_pmu_ثبتوونکی_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
