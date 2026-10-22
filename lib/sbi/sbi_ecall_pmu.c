/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_version.h>
#समावेश <sbi/sbi_pmu.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/riscv_asm.h>

अटल पूर्णांक sbi_ecall_pmu_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				 काष्ठा sbi_trap_regs *regs,
				 काष्ठा sbi_ecall_return *out)
{
	पूर्णांक ret = 0;
	uint64_t अस्थायी;

	चयन (funcid) {
	हाल SBI_EXT_PMU_अंक_COUNTERS:
		ret = sbi_pmu_अंक_ctr();
		अगर (ret >= 0) {
			out->मूल्य = ret;
			ret = 0;
		}
		अवरोध;
	हाल SBI_EXT_PMU_गणक_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &out->मूल्य);
		अवरोध;
	हाल SBI_EXT_PMU_गणक_CFG_MATCH:
#अगर __riscv_xlen == 32
		अस्थायी = ((uint64_t)regs->a5 << 32) | regs->a4;
#अन्यथा
		अस्थायी = regs->a4;
#पूर्ण_अगर
		ret = sbi_pmu_ctr_cfg_match(regs->a0, regs->a1, regs->a2,
					    regs->a3, अस्थायी);
		अगर (ret >= 0) {
			out->मूल्य = ret;
			ret = 0;
		}

		अवरोध;
	हाल SBI_EXT_PMU_गणक_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &अस्थायी, false);
		out->मूल्य = अस्थायी;
		अवरोध;
	हाल SBI_EXT_PMU_गणक_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &अस्थायी, true);
#अगर __riscv_xlen == 32
		out->मूल्य = अस्थायी >> 32;
#अन्यथा
		out->मूल्य = 0;
#पूर्ण_अगर
		अवरोध;
	हाल SBI_EXT_PMU_गणक_START:

#अगर __riscv_xlen == 32
		अस्थायी = ((uint64_t)regs->a4 << 32) | regs->a3;
#अन्यथा
		अस्थायी = regs->a3;
#पूर्ण_अगर
		ret = sbi_pmu_ctr_start(regs->a0, regs->a1, regs->a2, अस्थायी);
		अवरोध;
	हाल SBI_EXT_PMU_गणक_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		अवरोध;
	हाल SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		अवरोध;
	हाल SBI_EXT_PMU_SNAPSHOT_SET_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	शेष:
		ret = SBI_ENOTSUPP;
	}

	वापस ret;
}

काष्ठा sbi_ecall_extension ecall_pmu;

अटल पूर्णांक sbi_ecall_pmu_register_extensions(व्योम)
{
	वापस sbi_ecall_register_extension(&ecall_pmu);
}

काष्ठा sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_start		= SBI_EXT_PMU,
	.extid_end		= SBI_EXT_PMU,
	.register_extensions	= sbi_ecall_pmu_register_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
