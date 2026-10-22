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

ساكن صحيح sbi_ecall_pmu_handler(بنانشان ڊگهو extid, بنانشان ڊگهو funcid,
				 بنية sbi_trap_regs *regs,
				 بنية sbi_ecall_return *ٻاهريون)
{
	صحيح ret = 0;
	uint64_t عارضي;

	بدل (funcid) {
	حالة SBI_EXT_PMU_انگ_COUNTERS:
		ret = sbi_pmu_انگ_ctr();
		إذا (ret >= 0) {
			ٻاهريون->قدر = ret;
			ret = 0;
		}
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &ٻاهريون->قدر);
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_CFG_ميلاپ:
#إذا __riscv_xlen == 32
		عارضي = ((uint64_t)regs->a5 << 32) | regs->a4;
#وإلا
		عارضي = regs->a4;
#جيڪڏهن_پورو
		ret = sbi_pmu_ctr_cfg_ميلاپ(regs->a0, regs->a1, regs->a2,
					    regs->a3, عارضي);
		إذا (ret >= 0) {
			ٻاهريون->قدر = ret;
			ret = 0;
		}

		اكسر;
	حالة SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &عارضي, false);
		ٻاهريون->قدر = عارضي;
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &عارضي, true);
#إذا __riscv_xlen == 32
		ٻاهريون->قدر = عارضي >> 32;
#وإلا
		ٻاهريون->قدر = 0;
#جيڪڏهن_پورو
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_شروعات:

#إذا __riscv_xlen == 32
		عارضي = ((uint64_t)regs->a4 << 32) | regs->a3;
#وإلا
		عارضي = regs->a3;
#جيڪڏهن_پورو
		ret = sbi_pmu_ctr_شروعات(regs->a0, regs->a1, regs->a2, عارضي);
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		اكسر;
	حالة SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		اكسر;
	حالة SBI_EXT_PMU_SNAPSHOT_سيٽ_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_pmu;

ساكن صحيح sbi_ecall_pmu_رجسٽر_extensions(فراغ)
{
	أرجع sbi_ecall_رجسٽر_extension(&ecall_pmu);
}

بنية sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_شروعات		= SBI_EXT_PMU,
	.extid_پڇاڙي		= SBI_EXT_PMU,
	.رجسٽر_extensions	= sbi_ecall_pmu_رجسٽر_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
