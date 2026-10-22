// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_pmu.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_pmu.h>

স্থির সংরচনা fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
স্থির uint32_t hw_event_গুনতি;

uint64_t fdt_pmu_get_select_মান(uint32_t event_idx)
{
	পূর্ণাঙ্ক i;
	সংরচনা fdt_pmu_hw_event_select_map *event;

	প্রতি (i = 0; i < hw_event_গুনতি; i++) {
		event = &fdt_pmu_evt_select[i];
		যদি (event->eidx == event_idx)
			প্রত্যাগম event->select;
	}

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_pmu_fixup(শূন্য *fdt)
{
	পূর্ণাঙ্ক pmu_offset;
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	যদি (!fdt)
		প্রত্যাগম SBI_EINVAL;

	pmu_offset = fdt_গ্রন্থি_offset_by_compatible(fdt, -1, "riscv,pmu");
	যদি (pmu_offset < 0)
		প্রত্যাগম SBI_EFAIL;

	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_offset, "riscv,raw-event-to-mhpmcounters");
	যদি (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_offset, "interrupts-extended");

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_pmu_setup(ধ্রুব শূন্য *fdt)
{
	পূর্ণাঙ্ক i, pmu_offset, len, result;
	ধ্রুব u32 *event_val;
	ধ্রুব u32 *event_ctr_map;
	সংরচনা fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_mask;
	u32 event_idx_start, event_idx_end, ctr_map;

	যদি (!fdt)
		প্রত্যাগম SBI_EINVAL;

	pmu_offset = fdt_গ্রন্থি_offset_by_compatible(fdt, -1, "riscv,pmu");
	যদি (pmu_offset < 0)
		প্রত্যাগম SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_offset,
				    "riscv,event-to-mhpmcounters", &len);
	যদি (event_ctr_map) {
		len = len / (পরিমাণ(u32) * 3);
		প্রতি (i = 0; i < len; i++) {
			event_idx_start = fdt32_to_cpu(event_ctr_map[3 * i]);
			event_idx_end = fdt32_to_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_to_cpu(event_ctr_map[3 * i + 2]);
			result = sbi_pmu_add_hw_event_counter_map(
				event_idx_start, event_idx_end, ctr_map);
			যদি (result)
				প্রত্যাগম result;
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,event-to-mhpmevent", &len);
	যদি (event_val) {
		len = len / (পরিমাণ(u32) * 3);

		hw_event_গুনতি = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_গুনতি,
						পরিমাণ(*fdt_pmu_evt_select));
		যদি (!fdt_pmu_evt_select)
			প্রত্যাগম SBI_ENOMEM;

		প্রতি (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_to_cpu(event_val[3 * i]);
			event->select = fdt32_to_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_to_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,raw-event-to-mhpmcounters", &len);
	যদি (event_val) {
		len = len / (পরিমাণ(u32) * 5);
		প্রতি (i = 0; i < len; i++) {
			raw_selector = fdt32_to_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_to_cpu(event_val[5 * i + 1]);
			select_mask = fdt32_to_cpu(event_val[5 * i + 2]);
			select_mask = (select_mask  << 32) |
					fdt32_to_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_to_cpu(event_val[5 * i + 4]);
			result = sbi_pmu_add_raw_event_counter_map(
					raw_selector, select_mask, ctr_map);
			যদি (result)
				প্রত্যাগম result;
		}
	}

	প্রত্যাগম 0;
}
