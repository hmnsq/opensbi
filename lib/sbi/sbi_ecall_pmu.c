/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_version.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_pmu_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				 ਬਣਤਰ sbi_trap_regs *regs,
				 ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਪੂਰਨ ret = 0;
	uint64_t ਆਰਜ਼ੀ;

	ਚੋਣ (funcid) {
	ਹਾਲਤ SBI_EXT_PMU_ਅੰਕ_COUNTERS:
		ret = sbi_pmu_ਅੰਕ_ctr();
		ਜੇ (ret >= 0) {
			ਬਾਹਰਲਾ->ਮੁੱਲ = ret;
			ret = 0;
		}
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_PMU_COUNTER_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &ਬਾਹਰਲਾ->ਮੁੱਲ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_PMU_COUNTER_CFG_ਮੇਲ:
#ਜੇ __riscv_xlen == 32
		ਆਰਜ਼ੀ = ((uint64_t)regs->a5 << 32) | regs->a4;
#ਨਹੀਂਤਾਂ
		ਆਰਜ਼ੀ = regs->a4;
#ਜੇ_ਅੰਤ
		ret = sbi_pmu_ctr_cfg_ਮੇਲ(regs->a0, regs->a1, regs->a2,
					    regs->a3, ਆਰਜ਼ੀ);
		ਜੇ (ret >= 0) {
			ਬਾਹਰਲਾ->ਮੁੱਲ = ret;
			ret = 0;
		}

		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_PMU_COUNTER_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &ਆਰਜ਼ੀ, false);
		ਬਾਹਰਲਾ->ਮੁੱਲ = ਆਰਜ਼ੀ;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_PMU_COUNTER_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &ਆਰਜ਼ੀ, true);
#ਜੇ __riscv_xlen == 32
		ਬਾਹਰਲਾ->ਮੁੱਲ = ਆਰਜ਼ੀ >> 32;
#ਨਹੀਂਤਾਂ
		ਬਾਹਰਲਾ->ਮੁੱਲ = 0;
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_PMU_COUNTER_ਸ਼ੁਰੂ:

#ਜੇ __riscv_xlen == 32
		ਆਰਜ਼ੀ = ((uint64_t)regs->a4 << 32) | regs->a3;
#ਨਹੀਂਤਾਂ
		ਆਰਜ਼ੀ = regs->a3;
#ਜੇ_ਅੰਤ
		ret = sbi_pmu_ctr_ਸ਼ੁਰੂ(regs->a0, regs->a1, regs->a2, ਆਰਜ਼ੀ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_PMU_COUNTER_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_PMU_SNAPSHOT_ਸੈੱਟ_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	ਮੂਲ:
		ret = SBI_ENOTSUPP;
	}

	ਵਾਪਸ ret;
}

ਬਣਤਰ sbi_ecall_extension ecall_pmu;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_pmu_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_pmu);
}

ਬਣਤਰ sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_PMU,
	.extid_ਸਿਰਾ		= SBI_EXT_PMU,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_pmu_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
