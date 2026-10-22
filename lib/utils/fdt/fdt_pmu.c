// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_pmu.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_pmu.h>

स्थिर संरचना fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
स्थिर uint32_t hw_event_गणनम्;

uint64_t fdt_pmu_get_select_मानम्(uint32_t event_idx)
{
	पूर्णाङ्क i;
	संरचना fdt_pmu_hw_event_select_map *event;

	प्रति (i = 0; i < hw_event_गणनम्; i++) {
		event = &fdt_pmu_evt_select[i];
		यदि (event->eidx == event_idx)
			प्रत्यागम event->select;
	}

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_pmu_fixup(शून्य *fdt)
{
	पूर्णाङ्क pmu_विस्थापनम्;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	यदि (!fdt)
		प्रत्यागम SBI_EINVAL;

	pmu_विस्थापनम् = fdt_ग्रन्थिः_विस्थापनम्_by_compatible(fdt, -1, "riscv,pmu");
	यदि (pmu_विस्थापनम् < 0)
		प्रत्यागम SBI_EFAIL;

	fdt_delprop(fdt, pmu_विस्थापनम्, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_विस्थापनम्, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_विस्थापनम्, "riscv,raw-event-to-mhpmcounters");
	यदि (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_विस्थापनम्, "interrupts-extended");

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_pmu_सज्जता(ध्रुव शून्य *fdt)
{
	पूर्णाङ्क i, pmu_विस्थापनम्, len, परिणाम;
	ध्रुव u32 *event_val;
	ध्रुव u32 *event_ctr_map;
	संरचना fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_आवरणम्;
	u32 event_idx_start, event_idx_अन्तः, ctr_map;

	यदि (!fdt)
		प्रत्यागम SBI_EINVAL;

	pmu_विस्थापनम् = fdt_ग्रन्थिः_विस्थापनम्_by_compatible(fdt, -1, "riscv,pmu");
	यदि (pmu_विस्थापनम् < 0)
		प्रत्यागम SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_विस्थापनम्,
				    "riscv,event-to-mhpmcounters", &len);
	यदि (event_ctr_map) {
		len = len / (परिमाण(u32) * 3);
		प्रति (i = 0; i < len; i++) {
			event_idx_start = fdt32_गन्तव्यम्_cpu(event_ctr_map[3 * i]);
			event_idx_अन्तः = fdt32_गन्तव्यम्_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_गन्तव्यम्_cpu(event_ctr_map[3 * i + 2]);
			परिणाम = sbi_pmu_सङ्कलय_hw_event_गणक_map(
				event_idx_start, event_idx_अन्तः, ctr_map);
			यदि (परिणाम)
				प्रत्यागम परिणाम;
		}
	}

	event_val = fdt_getprop(fdt, pmu_विस्थापनम्,
				"riscv,event-to-mhpmevent", &len);
	यदि (event_val) {
		len = len / (परिमाण(u32) * 3);

		hw_event_गणनम् = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_गणनम्,
						परिमाण(*fdt_pmu_evt_select));
		यदि (!fdt_pmu_evt_select)
			प्रत्यागम SBI_ENOMEM;

		प्रति (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_गन्तव्यम्_cpu(event_val[3 * i]);
			event->select = fdt32_गन्तव्यम्_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_गन्तव्यम्_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_विस्थापनम्,
				"riscv,raw-event-to-mhpmcounters", &len);
	यदि (event_val) {
		len = len / (परिमाण(u32) * 5);
		प्रति (i = 0; i < len; i++) {
			raw_selector = fdt32_गन्तव्यम्_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_गन्तव्यम्_cpu(event_val[5 * i + 1]);
			select_आवरणम् = fdt32_गन्तव्यम्_cpu(event_val[5 * i + 2]);
			select_आवरणम् = (select_आवरणम्  << 32) |
					fdt32_गन्तव्यम्_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_गन्तव्यम्_cpu(event_val[5 * i + 4]);
			परिणाम = sbi_pmu_सङ्कलय_raw_event_गणक_map(
					raw_selector, select_आवरणम्, ctr_map);
			यदि (परिणाम)
				प्रत्यागम परिणाम;
		}
	}

	प्रत्यागम 0;
}
