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

ساكن صحيح sbi_ecall_pmu_handler(unsigned long extid, unsigned long funcid,
				 بنية sbi_trap_regs *regs,
				 بنية sbi_ecall_return *out)
{
	صحيح ret = 0;
	uint64_t temp;

	بدل (funcid) {
	حالة SBI_EXT_PMU_ژمارە_COUNTERS:
		ret = sbi_pmu_ژمارە_ctr();
		إذا (ret >= 0) {
			out->بەها = ret;
			ret = 0;
		}
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &out->بەها);
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_CFG_MATCH:
#إذا __riscv_xlen == 32
		temp = ((uint64_t)regs->a5 << 32) | regs->a4;
#وإلا
		temp = regs->a4;
#endif
		ret = sbi_pmu_ctr_cfg_match(regs->a0, regs->a1, regs->a2,
					    regs->a3, temp);
		إذا (ret >= 0) {
			out->بەها = ret;
			ret = 0;
		}

		اكسر;
	حالة SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, false);
		out->بەها = temp;
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &temp, true);
#إذا __riscv_xlen == 32
		out->بەها = temp >> 32;
#وإلا
		out->بەها = 0;
#endif
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_START:

#إذا __riscv_xlen == 32
		temp = ((uint64_t)regs->a4 << 32) | regs->a3;
#وإلا
		temp = regs->a3;
#endif
		ret = sbi_pmu_ctr_start(regs->a0, regs->a1, regs->a2, temp);
		اكسر;
	حالة SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		اكسر;
	حالة SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		اكسر;
	حالة SBI_EXT_PMU_SNAPSHOT_SET_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_pmu;

ساكن صحيح sbi_ecall_pmu_register_extensions(فراغ)
{
	أرجع sbi_ecall_register_extension(&ecall_pmu);
}

بنية sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_start		= SBI_EXT_PMU,
	.extid_end		= SBI_EXT_PMU,
	.register_extensions	= sbi_ecall_pmu_register_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
