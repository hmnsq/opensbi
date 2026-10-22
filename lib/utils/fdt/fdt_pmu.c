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
ساكن uint32_t hw_event_ڳڻپ;

uint64_t fdt_pmu_get_select_قدر(uint32_t event_idx)
{
	صحيح i;
	بنية fdt_pmu_hw_event_select_map *event;

	لكل (i = 0; i < hw_event_ڳڻپ; i++) {
		event = &fdt_pmu_evt_select[i];
		إذا (event->eidx == event_idx)
			أرجع event->select;
	}

	أرجع 0;
}

صحيح fdt_pmu_fixup(فراغ *fdt)
{
	صحيح pmu_وٿي;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	إذا (!fdt)
		أرجع SBI_EINVAL;

	pmu_وٿي = fdt_ڳنڍ_وٿي_by_compatible(fdt, -1, "riscv,pmu");
	إذا (pmu_وٿي < 0)
		أرجع SBI_EFAIL;

	fdt_delprop(fdt, pmu_وٿي, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_وٿي, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_وٿي, "riscv,raw-event-to-mhpmcounters");
	إذا (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_وٿي, "interrupts-extended");

	أرجع 0;
}

صحيح fdt_pmu_تياري(ثابت فراغ *fdt)
{
	صحيح i, pmu_وٿي, len, نتيجو;
	ثابت u32 *event_val;
	ثابت u32 *event_ctr_map;
	بنية fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_نقاب;
	u32 event_idx_شروعات, event_idx_پڇاڙي, ctr_map;

	إذا (!fdt)
		أرجع SBI_EINVAL;

	pmu_وٿي = fdt_ڳنڍ_وٿي_by_compatible(fdt, -1, "riscv,pmu");
	إذا (pmu_وٿي < 0)
		أرجع SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_وٿي,
				    "riscv,event-to-mhpmcounters", &len);
	إذا (event_ctr_map) {
		len = len / (حجم(u32) * 3);
		لكل (i = 0; i < len; i++) {
			event_idx_شروعات = fdt32_منزل_cpu(event_ctr_map[3 * i]);
			event_idx_پڇاڙي = fdt32_منزل_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_منزل_cpu(event_ctr_map[3 * i + 2]);
			نتيجو = sbi_pmu_جوڙيو_hw_event_counter_map(
				event_idx_شروعات, event_idx_پڇاڙي, ctr_map);
			إذا (نتيجو)
				أرجع نتيجو;
		}
	}

	event_val = fdt_getprop(fdt, pmu_وٿي,
				"riscv,event-to-mhpmevent", &len);
	إذا (event_val) {
		len = len / (حجم(u32) * 3);

		hw_event_ڳڻپ = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_ڳڻپ,
						حجم(*fdt_pmu_evt_select));
		إذا (!fdt_pmu_evt_select)
			أرجع SBI_ENOMEM;

		لكل (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_منزل_cpu(event_val[3 * i]);
			event->select = fdt32_منزل_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_منزل_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_وٿي,
				"riscv,raw-event-to-mhpmcounters", &len);
	إذا (event_val) {
		len = len / (حجم(u32) * 5);
		لكل (i = 0; i < len; i++) {
			raw_selector = fdt32_منزل_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_منزل_cpu(event_val[5 * i + 1]);
			select_نقاب = fdt32_منزل_cpu(event_val[5 * i + 2]);
			select_نقاب = (select_نقاب  << 32) |
					fdt32_منزل_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_منزل_cpu(event_val[5 * i + 4]);
			نتيجو = sbi_pmu_جوڙيو_raw_event_counter_map(
					raw_selector, select_نقاب, ctr_map);
			إذا (نتيجو)
				أرجع نتيجو;
		}
	}

	أرجع 0;
}
