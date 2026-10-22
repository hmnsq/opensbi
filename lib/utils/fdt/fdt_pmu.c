// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_pmu.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/fdt/fdt_pmu.h>

ایستا ساختار fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
ایستا uint32_t hw_event_شمار;

uint64_t fdt_pmu_get_select_مقدار(uint32_t event_idx)
{
	صحیح i;
	ساختار fdt_pmu_hw_event_select_map *event;

	برای (i = 0; i < hw_event_شمار; i++) {
		event = &fdt_pmu_evt_select[i];
		اگر (event->eidx == event_idx)
			بازگشت event->select;
	}

	بازگشت 0;
}

صحیح fdt_pmu_fixup(تهی *fdt)
{
	صحیح pmu_offset;
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	اگر (!fdt)
		بازگشت SBI_EINVAL;

	pmu_offset = fdt_گره_offset_by_compatible(fdt, -1, "riscv,pmu");
	اگر (pmu_offset < 0)
		بازگشت SBI_EFAIL;

	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_offset, "riscv,raw-event-to-mhpmcounters");
	اگر (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_offset, "interrupts-extended");

	بازگشت 0;
}

صحیح fdt_pmu_setup(ثابت تهی *fdt)
{
	صحیح i, pmu_offset, len, result;
	ثابت u32 *event_val;
	ثابت u32 *event_ctr_map;
	ساختار fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_mask;
	u32 event_idx_start, event_idx_end, ctr_map;

	اگر (!fdt)
		بازگشت SBI_EINVAL;

	pmu_offset = fdt_گره_offset_by_compatible(fdt, -1, "riscv,pmu");
	اگر (pmu_offset < 0)
		بازگشت SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_offset,
				    "riscv,event-to-mhpmcounters", &len);
	اگر (event_ctr_map) {
		len = len / (اندازه(u32) * 3);
		برای (i = 0; i < len; i++) {
			event_idx_start = fdt32_to_cpu(event_ctr_map[3 * i]);
			event_idx_end = fdt32_to_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_to_cpu(event_ctr_map[3 * i + 2]);
			result = sbi_pmu_add_hw_event_counter_map(
				event_idx_start, event_idx_end, ctr_map);
			اگر (result)
				بازگشت result;
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,event-to-mhpmevent", &len);
	اگر (event_val) {
		len = len / (اندازه(u32) * 3);

		hw_event_شمار = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_شمار,
						اندازه(*fdt_pmu_evt_select));
		اگر (!fdt_pmu_evt_select)
			بازگشت SBI_ENOMEM;

		برای (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_to_cpu(event_val[3 * i]);
			event->select = fdt32_to_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_to_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,raw-event-to-mhpmcounters", &len);
	اگر (event_val) {
		len = len / (اندازه(u32) * 5);
		برای (i = 0; i < len; i++) {
			raw_selector = fdt32_to_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_to_cpu(event_val[5 * i + 1]);
			select_mask = fdt32_to_cpu(event_val[5 * i + 2]);
			select_mask = (select_mask  << 32) |
					fdt32_to_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_to_cpu(event_val[5 * i + 4]);
			result = sbi_pmu_add_raw_event_counter_map(
					raw_selector, select_mask, ctr_map);
			اگر (result)
				بازگشت result;
		}
	}

	بازگشت 0;
}
