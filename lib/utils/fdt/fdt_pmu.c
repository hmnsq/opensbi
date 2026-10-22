// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_pmu.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/fdt/fdt_pmu.h>

ساكن بنية fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
ساكن uint32_t hw_event_شمار;

uint64_t fdt_pmu_get_select_مقدار(uint32_t event_idx)
{
	صحيح i;
	بنية fdt_pmu_hw_event_select_map *event;

	لكل (i = 0; i < hw_event_شمار; i++) {
		event = &fdt_pmu_evt_select[i];
		إذا (event->eidx == event_idx)
			أرجع event->select;
	}

	أرجع 0;
}

صحيح fdt_pmu_fixup(فراغ *fdt)
{
	صحيح pmu_جابه_جایی;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	إذا (!fdt)
		أرجع SBI_EINVAL;

	pmu_جابه_جایی = fdt_گره_جابه_جایی_by_compatible(fdt, -1, "riscv,pmu");
	إذا (pmu_جابه_جایی < 0)
		أرجع SBI_EFAIL;

	fdt_delprop(fdt, pmu_جابه_جایی, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_جابه_جایی, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_جابه_جایی, "riscv,raw-event-to-mhpmcounters");
	إذا (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_جابه_جایی, "interrupts-extended");

	أرجع 0;
}

صحيح fdt_pmu_راهاندازی(ثابت فراغ *fdt)
{
	صحيح i, pmu_جابه_جایی, len, نتیجه;
	ثابت u32 *event_val;
	ثابت u32 *event_ctr_map;
	بنية fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_ماسک;
	u32 event_idx_شروع, event_idx_انتها, ctr_map;

	إذا (!fdt)
		أرجع SBI_EINVAL;

	pmu_جابه_جایی = fdt_گره_جابه_جایی_by_compatible(fdt, -1, "riscv,pmu");
	إذا (pmu_جابه_جایی < 0)
		أرجع SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_جابه_جایی,
				    "riscv,event-to-mhpmcounters", &len);
	إذا (event_ctr_map) {
		len = len / (حجم(u32) * 3);
		لكل (i = 0; i < len; i++) {
			event_idx_شروع = fdt32_مقصد_cpu(event_ctr_map[3 * i]);
			event_idx_انتها = fdt32_مقصد_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_مقصد_cpu(event_ctr_map[3 * i + 2]);
			نتیجه = sbi_pmu_جمع_کن_hw_event_counter_map(
				event_idx_شروع, event_idx_انتها, ctr_map);
			إذا (نتیجه)
				أرجع نتیجه;
		}
	}

	event_val = fdt_getprop(fdt, pmu_جابه_جایی,
				"riscv,event-to-mhpmevent", &len);
	إذا (event_val) {
		len = len / (حجم(u32) * 3);

		hw_event_شمار = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_شمار,
						حجم(*fdt_pmu_evt_select));
		إذا (!fdt_pmu_evt_select)
			أرجع SBI_ENOMEM;

		لكل (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_مقصد_cpu(event_val[3 * i]);
			event->select = fdt32_مقصد_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_مقصد_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_جابه_جایی,
				"riscv,raw-event-to-mhpmcounters", &len);
	إذا (event_val) {
		len = len / (حجم(u32) * 5);
		لكل (i = 0; i < len; i++) {
			raw_selector = fdt32_مقصد_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_مقصد_cpu(event_val[5 * i + 1]);
			select_ماسک = fdt32_مقصد_cpu(event_val[5 * i + 2]);
			select_ماسک = (select_ماسک  << 32) |
					fdt32_مقصد_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_مقصد_cpu(event_val[5 * i + 4]);
			نتیجه = sbi_pmu_جمع_کن_raw_event_counter_map(
					raw_selector, select_ماسک, ctr_map);
			إذا (نتیجه)
				أرجع نتیجه;
		}
	}

	أرجع 0;
}
