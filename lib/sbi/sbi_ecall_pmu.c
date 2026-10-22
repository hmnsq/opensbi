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

ساكن صحيح sbi_ecall_pmu_handler(بیعلامت دراز extid, بیعلامت دراز funcid,
				 بنية sbi_trap_regs *regs,
				 بنية sbi_ecall_return *بیرون)
{
	صحيح ret = 0;
	uint64_t موقت;

	بدل (funcid) {
	حالة SBI_EXT_PMU_رقم_COUNTERS:
		ret = sbi_pmu_رقم_ctr();
		إذا (ret >= 0) {
			بیرون->مقدار = ret;
			ret = 0;
		}
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &بیرون->مقدار);
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_CFG_تطابق:
#إذا __riscv_xlen == 32
		موقت = ((uint64_t)regs->a5 << 32) | regs->a4;
#وإلا
		موقت = regs->a4;
#پایان_اگر
		ret = sbi_pmu_ctr_cfg_تطابق(regs->a0, regs->a1, regs->a2,
					    regs->a3, موقت);
		إذا (ret >= 0) {
			بیرون->مقدار = ret;
			ret = 0;
		}

		اكسر;
	حالة SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &موقت, false);
		بیرون->مقدار = موقت;
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &موقت, true);
#إذا __riscv_xlen == 32
		بیرون->مقدار = موقت >> 32;
#وإلا
		بیرون->مقدار = 0;
#پایان_اگر
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_شروع:

#إذا __riscv_xlen == 32
		موقت = ((uint64_t)regs->a4 << 32) | regs->a3;
#وإلا
		موقت = regs->a3;
#پایان_اگر
		ret = sbi_pmu_ctr_شروع(regs->a0, regs->a1, regs->a2, موقت);
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		اكسر;
	حالة SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		اكسر;
	حالة SBI_EXT_PMU_SNAPSHOT_مجموعه_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_pmu;

ساكن صحيح sbi_ecall_pmu_ثبات_extensions(فراغ)
{
	أرجع sbi_ecall_ثبات_extension(&ecall_pmu);
}

بنية sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_شروع		= SBI_EXT_PMU,
	.extid_انتها		= SBI_EXT_PMU,
	.ثبات_extensions	= sbi_ecall_pmu_ثبات_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
