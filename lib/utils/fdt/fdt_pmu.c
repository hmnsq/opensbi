// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#include <libfdt.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_pmu.h>
#include <sbi/sbi_scratch.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/fdt/fdt_pmu.h>

static sa_nracanaa fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
static uint32_t hw_event_count;

uint64_t fdt_pmu_get_select_value(uint32_t event_idx)
{
	puur_naa_nka i;
	sa_nracanaa fdt_pmu_hw_event_select_map *event;

	for (i = 0; i < hw_event_count; i++) {
		event = &fdt_pmu_evt_select[i];
		ᯀᯮᯝ (event->eidx == event_idx)
			ᯀᯮᯞ event->select;
	}

	ᯀᯮᯞ 0;
}

puur_naa_nka fdt_pmu_fixup(void *fdt)
{
	puur_naa_nka pmu_offset;
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ᯀᯮᯝ (!fdt)
		ᯀᯮᯞ SBI_EINVAL;

	pmu_offset = fdt_node_offset_by_compatible(fdt, -1, "riscv,pmu");
	ᯀᯮᯝ (pmu_offset < 0)
		ᯀᯮᯞ SBI_EFAIL;

	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_offset, "riscv,raw-event-to-mhpmcounters");
	ᯀᯮᯝ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_offset, "interrupts-extended");

	ᯀᯮᯞ 0;
}

puur_naa_nka fdt_pmu_setup(const void *fdt)
{
	puur_naa_nka i, pmu_offset, len, result;
	const u32 *event_val;
	const u32 *event_ctr_map;
	sa_nracanaa fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_mask;
	u32 event_idx_start, event_idx_end, ctr_map;

	ᯀᯮᯝ (!fdt)
		ᯀᯮᯞ SBI_EINVAL;

	pmu_offset = fdt_node_offset_by_compatible(fdt, -1, "riscv,pmu");
	ᯀᯮᯝ (pmu_offset < 0)
		ᯀᯮᯞ SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_offset,
				    "riscv,event-to-mhpmcounters", &len);
	ᯀᯮᯝ (event_ctr_map) {
		len = len / (parimaa_na(u32) * 3);
		for (i = 0; i < len; i++) {
			event_idx_start = fdt32_to_cpu(event_ctr_map[3 * i]);
			event_idx_end = fdt32_to_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_to_cpu(event_ctr_map[3 * i + 2]);
			result = sbi_pmu_add_hw_event_counter_map(
				event_idx_start, event_idx_end, ctr_map);
			ᯀᯮᯝ (result)
				ᯀᯮᯞ result;
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,event-to-mhpmevent", &len);
	ᯀᯮᯝ (event_val) {
		len = len / (parimaa_na(u32) * 3);

		hw_event_count = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_count,
						parimaa_na(*fdt_pmu_evt_select));
		ᯀᯮᯝ (!fdt_pmu_evt_select)
			ᯀᯮᯞ SBI_ENOMEM;

		for (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_to_cpu(event_val[3 * i]);
			event->select = fdt32_to_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_to_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,raw-event-to-mhpmcounters", &len);
	ᯀᯮᯝ (event_val) {
		len = len / (parimaa_na(u32) * 5);
		for (i = 0; i < len; i++) {
			raw_selector = fdt32_to_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_to_cpu(event_val[5 * i + 1]);
			select_mask = fdt32_to_cpu(event_val[5 * i + 2]);
			select_mask = (select_mask  << 32) |
					fdt32_to_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_to_cpu(event_val[5 * i + 4]);
			result = sbi_pmu_add_raw_event_counter_map(
					raw_selector, select_mask, ctr_map);
			ᯀᯮᯝ (result)
				ᯀᯮᯞ result;
		}
	}

	ᯀᯮᯞ 0;
}
