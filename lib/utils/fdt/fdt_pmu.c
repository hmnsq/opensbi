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

static struct fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
static uint32_t hw_event_count;

uint64_t fdt_pmu_get_select_value(uint32_t event_idx)
{
	số_nguyên i;
	struct fdt_pmu_hw_event_select_map *event;

	cho (i = 0; i < hw_event_count; i++) {
		event = &fdt_pmu_evt_select[i];
		nếu (event->eidx == event_idx)
			trả_về event->select;
	}

	trả_về 0;
}

số_nguyên fdt_pmu_fixup(void *fdt)
{
	số_nguyên pmu_offset;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	nếu (!fdt)
		trả_về SBI_EINVAL;

	pmu_offset = fdt_node_offset_by_compatible(fdt, -1, "riscv,pmu");
	nếu (pmu_offset < 0)
		trả_về SBI_EFAIL;

	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_offset, "riscv,raw-event-to-mhpmcounters");
	nếu (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_offset, "interrupts-extended");

	trả_về 0;
}

số_nguyên fdt_pmu_setup(const void *fdt)
{
	số_nguyên i, pmu_offset, len, result;
	const u32 *event_val;
	const u32 *event_ctr_map;
	struct fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_mask;
	u32 event_idx_start, event_idx_end, ctr_map;

	nếu (!fdt)
		trả_về SBI_EINVAL;

	pmu_offset = fdt_node_offset_by_compatible(fdt, -1, "riscv,pmu");
	nếu (pmu_offset < 0)
		trả_về SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_offset,
				    "riscv,event-to-mhpmcounters", &len);
	nếu (event_ctr_map) {
		len = len / (sizeof(u32) * 3);
		cho (i = 0; i < len; i++) {
			event_idx_start = fdt32_to_cpu(event_ctr_map[3 * i]);
			event_idx_end = fdt32_to_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_to_cpu(event_ctr_map[3 * i + 2]);
			result = sbi_pmu_add_hw_event_counter_map(
				event_idx_start, event_idx_end, ctr_map);
			nếu (result)
				trả_về result;
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,event-to-mhpmevent", &len);
	nếu (event_val) {
		len = len / (sizeof(u32) * 3);

		hw_event_count = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_count,
						sizeof(*fdt_pmu_evt_select));
		nếu (!fdt_pmu_evt_select)
			trả_về SBI_ENOMEM;

		cho (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_to_cpu(event_val[3 * i]);
			event->select = fdt32_to_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_to_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,raw-event-to-mhpmcounters", &len);
	nếu (event_val) {
		len = len / (sizeof(u32) * 5);
		cho (i = 0; i < len; i++) {
			raw_selector = fdt32_to_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_to_cpu(event_val[5 * i + 1]);
			select_mask = fdt32_to_cpu(event_val[5 * i + 2]);
			select_mask = (select_mask  << 32) |
					fdt32_to_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_to_cpu(event_val[5 * i + 4]);
			result = sbi_pmu_add_raw_event_counter_map(
					raw_selector, select_mask, ctr_map);
			nếu (result)
				trả_về result;
		}
	}

	trả_về 0;
}
