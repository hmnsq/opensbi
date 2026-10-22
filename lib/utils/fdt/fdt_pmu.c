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
स्थिर uint32_t hw_event_गन्ती;

uint64_t fdt_pmu_get_select_मान(uint32_t event_idx)
{
	पूर्णाङ्क i;
	संरचना fdt_pmu_hw_event_select_map *event;

	लागि (i = 0; i < hw_event_गन्ती; i++) {
		event = &fdt_pmu_evt_select[i];
		यदि (event->eidx == event_idx)
			फर्काऊ event->select;
	}

	फर्काऊ 0;
}

पूर्णाङ्क fdt_pmu_fixup(शून्य *fdt)
{
	पूर्णाङ्क pmu_offset;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	यदि (!fdt)
		फर्काऊ SBI_EINVAL;

	pmu_offset = fdt_गाँठो_offset_by_compatible(fdt, -1, "riscv,pmu");
	यदि (pmu_offset < 0)
		फर्काऊ SBI_EFAIL;

	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_offset, "riscv,raw-event-to-mhpmcounters");
	यदि (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_offset, "interrupts-extended");

	फर्काऊ 0;
}

पूर्णाङ्क fdt_pmu_setup(अचर शून्य *fdt)
{
	पूर्णाङ्क i, pmu_offset, len, result;
	अचर u32 *event_val;
	अचर u32 *event_ctr_map;
	संरचना fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_mask;
	u32 event_idx_start, event_idx_end, ctr_map;

	यदि (!fdt)
		फर्काऊ SBI_EINVAL;

	pmu_offset = fdt_गाँठो_offset_by_compatible(fdt, -1, "riscv,pmu");
	यदि (pmu_offset < 0)
		फर्काऊ SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_offset,
				    "riscv,event-to-mhpmcounters", &len);
	यदि (event_ctr_map) {
		len = len / (परिमाण(u32) * 3);
		लागि (i = 0; i < len; i++) {
			event_idx_start = fdt32_to_cpu(event_ctr_map[3 * i]);
			event_idx_end = fdt32_to_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_to_cpu(event_ctr_map[3 * i + 2]);
			result = sbi_pmu_add_hw_event_counter_map(
				event_idx_start, event_idx_end, ctr_map);
			यदि (result)
				फर्काऊ result;
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,event-to-mhpmevent", &len);
	यदि (event_val) {
		len = len / (परिमाण(u32) * 3);

		hw_event_गन्ती = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_गन्ती,
						परिमाण(*fdt_pmu_evt_select));
		यदि (!fdt_pmu_evt_select)
			फर्काऊ SBI_ENOMEM;

		लागि (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_to_cpu(event_val[3 * i]);
			event->select = fdt32_to_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_to_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,raw-event-to-mhpmcounters", &len);
	यदि (event_val) {
		len = len / (परिमाण(u32) * 5);
		लागि (i = 0; i < len; i++) {
			raw_selector = fdt32_to_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_to_cpu(event_val[5 * i + 1]);
			select_mask = fdt32_to_cpu(event_val[5 * i + 2]);
			select_mask = (select_mask  << 32) |
					fdt32_to_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_to_cpu(event_val[5 * i + 4]);
			result = sbi_pmu_add_raw_event_counter_map(
					raw_selector, select_mask, ctr_map);
			यदि (result)
				फर्काऊ result;
		}
	}

	फर्काऊ 0;
}
