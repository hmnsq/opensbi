/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_version.h>
#अन्तर्भाव <sbi/sbi_pmu.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/riscv_asm.h>

स्थिर पूर्णाङ्क sbi_ecall_pmu_handler(अचिह्नित दीर्घ extid, अचिह्नित दीर्घ funcid,
				 संरचना sbi_trap_regs *regs,
				 संरचना sbi_ecall_return *बहिर्गमनम्)
{
	पूर्णाङ्क ret = 0;
	uint64_t अस्थायी;

	विकल्प (funcid) {
	पक्ष SBI_EXT_PMU_अङ्कः_COUNTERS:
		ret = sbi_pmu_अङ्कः_ctr();
		यदि (ret >= 0) {
			बहिर्गमनम्->मानम् = ret;
			ret = 0;
		}
		भङ्ग;
	पक्ष SBI_EXT_PMU_गणक_GET_INFO:
		ret = sbi_pmu_ctr_get_info(regs->a0, &बहिर्गमनम्->मानम्);
		भङ्ग;
	पक्ष SBI_EXT_PMU_गणक_CFG_मेलनम्:
#यदि __riscv_xlen == 32
		अस्थायी = ((uint64_t)regs->a5 << 32) | regs->a4;
#अन्यथा
		अस्थायी = regs->a4;
#यदि_अन्तः
		ret = sbi_pmu_ctr_cfg_मेलनम्(regs->a0, regs->a1, regs->a2,
					    regs->a3, अस्थायी);
		यदि (ret >= 0) {
			बहिर्गमनम्->मानम् = ret;
			ret = 0;
		}

		भङ्ग;
	पक्ष SBI_EXT_PMU_गणक_FW_READ:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &अस्थायी, false);
		बहिर्गमनम्->मानम् = अस्थायी;
		भङ्ग;
	पक्ष SBI_EXT_PMU_गणक_FW_READ_HI:
		ret = sbi_pmu_ctr_fw_read(regs->a0, &अस्थायी, true);
#यदि __riscv_xlen == 32
		बहिर्गमनम्->मानम् = अस्थायी >> 32;
#अन्यथा
		बहिर्गमनम्->मानम् = 0;
#यदि_अन्तः
		भङ्ग;
	पक्ष SBI_EXT_PMU_गणक_START:

#यदि __riscv_xlen == 32
		अस्थायी = ((uint64_t)regs->a4 << 32) | regs->a3;
#अन्यथा
		अस्थायी = regs->a3;
#यदि_अन्तः
		ret = sbi_pmu_ctr_start(regs->a0, regs->a1, regs->a2, अस्थायी);
		भङ्ग;
	पक्ष SBI_EXT_PMU_गणक_STOP:
		ret = sbi_pmu_ctr_stop(regs->a0, regs->a1, regs->a2);
		भङ्ग;
	पक्ष SBI_EXT_PMU_EVENT_GET_INFO:
		ret = sbi_pmu_event_get_info(regs->a0, regs->a1, regs->a2, regs->a3);
		भङ्ग;
	पक्ष SBI_EXT_PMU_SNAPSHOT_समुच्चयः_SHMEM:
		/* fallthrough as OpenSBI doesn't support snapshot yet */
	स्वभाव:
		ret = SBI_ENOTSUPP;
	}

	प्रत्यागम ret;
}

संरचना sbi_ecall_extension ecall_pmu;

स्थिर पूर्णाङ्क sbi_ecall_pmu_पञ्जिकाकोशः_extensions(शून्य)
{
	प्रत्यागम sbi_ecall_पञ्जिकाकोशः_extension(&ecall_pmu);
}

संरचना sbi_ecall_extension ecall_pmu = {
	.name			= "pmu",
	.extid_start		= SBI_EXT_PMU,
	.extid_अन्तः		= SBI_EXT_PMU,
	.पञ्जिकाकोशः_extensions	= sbi_ecall_pmu_पञ्जिकाकोशः_extensions,
	.handle			= sbi_ecall_pmu_handler,
};
