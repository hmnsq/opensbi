// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_pmu.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_pmu.h>

நிலையான கட்டமைப்பு fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
நிலையான uint32_t hw_event_மொத்தம்;

uint64_t fdt_pmu_get_select_மதிப்பு(uint32_t event_idx)
{
	முழு i;
	கட்டமைப்பு fdt_pmu_hw_event_select_map *event;

	க்கு (i = 0; i < hw_event_மொத்தம்; i++) {
		event = &fdt_pmu_evt_select[i];
		எனில் (event->eidx == event_idx)
			திரும்பு event->select;
	}

	திரும்பு 0;
}

முழு fdt_pmu_fixup(வெறுமை *fdt)
{
	முழு pmu_offset;
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	எனில் (!fdt)
		திரும்பு SBI_EINVAL;

	pmu_offset = fdt_முனை_offset_by_compatible(fdt, -1, "riscv,pmu");
	எனில் (pmu_offset < 0)
		திரும்பு SBI_EFAIL;

	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_offset, "riscv,raw-event-to-mhpmcounters");
	எனில் (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_offset, "interrupts-extended");

	திரும்பு 0;
}

முழு fdt_pmu_setup(மாறிலி வெறுமை *fdt)
{
	முழு i, pmu_offset, len, result;
	மாறிலி u32 *event_val;
	மாறிலி u32 *event_ctr_map;
	கட்டமைப்பு fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_mask;
	u32 event_idx_start, event_idx_end, ctr_map;

	எனில் (!fdt)
		திரும்பு SBI_EINVAL;

	pmu_offset = fdt_முனை_offset_by_compatible(fdt, -1, "riscv,pmu");
	எனில் (pmu_offset < 0)
		திரும்பு SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_offset,
				    "riscv,event-to-mhpmcounters", &len);
	எனில் (event_ctr_map) {
		len = len / (அளவு(u32) * 3);
		க்கு (i = 0; i < len; i++) {
			event_idx_start = fdt32_to_cpu(event_ctr_map[3 * i]);
			event_idx_end = fdt32_to_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_to_cpu(event_ctr_map[3 * i + 2]);
			result = sbi_pmu_add_hw_event_counter_map(
				event_idx_start, event_idx_end, ctr_map);
			எனில் (result)
				திரும்பு result;
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,event-to-mhpmevent", &len);
	எனில் (event_val) {
		len = len / (அளவு(u32) * 3);

		hw_event_மொத்தம் = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_மொத்தம்,
						அளவு(*fdt_pmu_evt_select));
		எனில் (!fdt_pmu_evt_select)
			திரும்பு SBI_ENOMEM;

		க்கு (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_to_cpu(event_val[3 * i]);
			event->select = fdt32_to_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_to_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,raw-event-to-mhpmcounters", &len);
	எனில் (event_val) {
		len = len / (அளவு(u32) * 5);
		க்கு (i = 0; i < len; i++) {
			raw_selector = fdt32_to_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_to_cpu(event_val[5 * i + 1]);
			select_mask = fdt32_to_cpu(event_val[5 * i + 2]);
			select_mask = (select_mask  << 32) |
					fdt32_to_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_to_cpu(event_val[5 * i + 4]);
			result = sbi_pmu_add_raw_event_counter_map(
					raw_selector, select_mask, ctr_map);
			எனில் (result)
				திரும்பு result;
		}
	}

	திரும்பு 0;
}
