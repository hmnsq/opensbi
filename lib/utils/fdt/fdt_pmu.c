// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmu.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_pmu.h>

ಸ್ಥಿರ ಸಂರಚನಾ fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
ಸ್ಥಿರ uint32_t hw_event_ಎಣಿಕೆ;

uint64_t fdt_pmu_get_select_ಮೌಲ್ಯ(uint32_t event_idx)
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ಸಂರಚನಾ fdt_pmu_hw_event_select_map *event;

	ಪ್ರತಿ (i = 0; i < hw_event_ಎಣಿಕೆ; i++) {
		event = &fdt_pmu_evt_select[i];
		ಯದಿ (event->eidx == event_idx)
			ಪ್ರತ್ಯಾಗಮ event->select;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_pmu_fixup(ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಙ್ಕ pmu_offset;
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ಯದಿ (!fdt)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	pmu_offset = fdt_ಗಂಟು_offset_by_compatible(fdt, -1, "riscv,pmu");
	ಯದಿ (pmu_offset < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_offset, "riscv,raw-event-to-mhpmcounters");
	ಯದಿ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_offset, "interrupts-extended");

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_pmu_setup(ಧ್ರುವ ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಙ್ಕ i, pmu_offset, len, result;
	ಧ್ರುವ u32 *event_val;
	ಧ್ರುವ u32 *event_ctr_map;
	ಸಂರಚನಾ fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_mask;
	u32 event_idx_start, event_idx_end, ctr_map;

	ಯದಿ (!fdt)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	pmu_offset = fdt_ಗಂಟು_offset_by_compatible(fdt, -1, "riscv,pmu");
	ಯದಿ (pmu_offset < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_offset,
				    "riscv,event-to-mhpmcounters", &len);
	ಯದಿ (event_ctr_map) {
		len = len / (ಪರಿಮಾಣ(u32) * 3);
		ಪ್ರತಿ (i = 0; i < len; i++) {
			event_idx_start = fdt32_to_cpu(event_ctr_map[3 * i]);
			event_idx_end = fdt32_to_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_to_cpu(event_ctr_map[3 * i + 2]);
			result = sbi_pmu_add_hw_event_counter_map(
				event_idx_start, event_idx_end, ctr_map);
			ಯದಿ (result)
				ಪ್ರತ್ಯಾಗಮ result;
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,event-to-mhpmevent", &len);
	ಯದಿ (event_val) {
		len = len / (ಪರಿಮಾಣ(u32) * 3);

		hw_event_ಎಣಿಕೆ = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_ಎಣಿಕೆ,
						ಪರಿಮಾಣ(*fdt_pmu_evt_select));
		ಯದಿ (!fdt_pmu_evt_select)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

		ಪ್ರತಿ (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_to_cpu(event_val[3 * i]);
			event->select = fdt32_to_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_to_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,raw-event-to-mhpmcounters", &len);
	ಯದಿ (event_val) {
		len = len / (ಪರಿಮಾಣ(u32) * 5);
		ಪ್ರತಿ (i = 0; i < len; i++) {
			raw_selector = fdt32_to_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_to_cpu(event_val[5 * i + 1]);
			select_mask = fdt32_to_cpu(event_val[5 * i + 2]);
			select_mask = (select_mask  << 32) |
					fdt32_to_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_to_cpu(event_val[5 * i + 4]);
			result = sbi_pmu_add_raw_event_counter_map(
					raw_selector, select_mask, ctr_map);
			ಯದಿ (result)
				ಪ್ರತ್ಯಾಗಮ result;
		}
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}
