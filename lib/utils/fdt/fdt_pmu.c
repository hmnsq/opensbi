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

ساکن ساخت fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
ساکن uint32_t hw_event_گنتی;

uint64_t fdt_pmu_get_select_قدر(uint32_t event_idx)
{
	صحیح i;
	ساخت fdt_pmu_hw_event_select_map *event;

	برائے (i = 0; i < hw_event_گنتی; i++) {
		event = &fdt_pmu_evt_select[i];
		اگر (event->eidx == event_idx)
			واپس event->select;
	}

	واپس 0;
}

صحیح fdt_pmu_fixup(خالی *fdt)
{
	صحیح pmu_انحراف;
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	اگر (!fdt)
		واپس SBI_EINVAL;

	pmu_انحراف = fdt_گرہ_انحراف_by_compatible(fdt, -1, "riscv,pmu");
	اگر (pmu_انحراف < 0)
		واپس SBI_EFAIL;

	fdt_delprop(fdt, pmu_انحراف, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_انحراف, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_انحراف, "riscv,raw-event-to-mhpmcounters");
	اگر (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_انحراف, "interrupts-extended");

	واپس 0;
}

صحیح fdt_pmu_تیاری(ثابت خالی *fdt)
{
	صحیح i, pmu_انحراف, len, نتیجہ;
	ثابت u32 *event_val;
	ثابت u32 *event_ctr_map;
	ساخت fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_نقاب;
	u32 event_idx_آغاز, event_idx_انتہا, ctr_map;

	اگر (!fdt)
		واپس SBI_EINVAL;

	pmu_انحراف = fdt_گرہ_انحراف_by_compatible(fdt, -1, "riscv,pmu");
	اگر (pmu_انحراف < 0)
		واپس SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_انحراف,
				    "riscv,event-to-mhpmcounters", &len);
	اگر (event_ctr_map) {
		len = len / (حجم(u32) * 3);
		برائے (i = 0; i < len; i++) {
			event_idx_آغاز = fdt32_مقصد_cpu(event_ctr_map[3 * i]);
			event_idx_انتہا = fdt32_مقصد_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_مقصد_cpu(event_ctr_map[3 * i + 2]);
			نتیجہ = sbi_pmu_جوڑو_hw_event_شمار_map(
				event_idx_آغاز, event_idx_انتہا, ctr_map);
			اگر (نتیجہ)
				واپس نتیجہ;
		}
	}

	event_val = fdt_getprop(fdt, pmu_انحراف,
				"riscv,event-to-mhpmevent", &len);
	اگر (event_val) {
		len = len / (حجم(u32) * 3);

		hw_event_گنتی = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_گنتی,
						حجم(*fdt_pmu_evt_select));
		اگر (!fdt_pmu_evt_select)
			واپس SBI_ENOMEM;

		برائے (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_مقصد_cpu(event_val[3 * i]);
			event->select = fdt32_مقصد_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_مقصد_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_انحراف,
				"riscv,raw-event-to-mhpmcounters", &len);
	اگر (event_val) {
		len = len / (حجم(u32) * 5);
		برائے (i = 0; i < len; i++) {
			raw_selector = fdt32_مقصد_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_مقصد_cpu(event_val[5 * i + 1]);
			select_نقاب = fdt32_مقصد_cpu(event_val[5 * i + 2]);
			select_نقاب = (select_نقاب  << 32) |
					fdt32_مقصد_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_مقصد_cpu(event_val[5 * i + 4]);
			نتیجہ = sbi_pmu_جوڑو_raw_event_شمار_map(
					raw_selector, select_نقاب, ctr_map);
			اگر (نتیجہ)
				واپس نتیجہ;
		}
	}

	واپس 0;
}
