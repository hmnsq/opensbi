// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_pmu.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_pmu.h>

స్థిర నిర్మాణం fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
స్థిర uint32_t hw_event_లెక్క;

uint64_t fdt_pmu_get_select_విలువ(uint32_t event_idx)
{
	పూర్ణాంకం i;
	నిర్మాణం fdt_pmu_hw_event_select_map *event;

	కొరకు (i = 0; i < hw_event_లెక్క; i++) {
		event = &fdt_pmu_evt_select[i];
		ఒకవేళ (event->eidx == event_idx)
			తిరిగి event->select;
	}

	తిరిగి 0;
}

పూర్ణాంకం fdt_pmu_fixup(శూన్య *fdt)
{
	పూర్ణాంకం pmu_స్థానభ్రంశం;
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ఒకవేళ (!fdt)
		తిరిగి SBI_EINVAL;

	pmu_స్థానభ్రంశం = fdt_కణుపు_స్థానభ్రంశం_by_compatible(fdt, -1, "riscv,pmu");
	ఒకవేళ (pmu_స్థానభ్రంశం < 0)
		తిరిగి SBI_EFAIL;

	fdt_delprop(fdt, pmu_స్థానభ్రంశం, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_స్థానభ్రంశం, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_స్థానభ్రంశం, "riscv,raw-event-to-mhpmcounters");
	ఒకవేళ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_స్థానభ్రంశం, "interrupts-extended");

	తిరిగి 0;
}

పూర్ణాంకం fdt_pmu_అమరిక(స్థిరరాశి శూన్య *fdt)
{
	పూర్ణాంకం i, pmu_స్థానభ్రంశం, len, ఫలితం;
	స్థిరరాశి u32 *event_val;
	స్థిరరాశి u32 *event_ctr_map;
	నిర్మాణం fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_ముసుగు;
	u32 event_idx_ప్రారంభం, event_idx_చివర, ctr_map;

	ఒకవేళ (!fdt)
		తిరిగి SBI_EINVAL;

	pmu_స్థానభ్రంశం = fdt_కణుపు_స్థానభ్రంశం_by_compatible(fdt, -1, "riscv,pmu");
	ఒకవేళ (pmu_స్థానభ్రంశం < 0)
		తిరిగి SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_స్థానభ్రంశం,
				    "riscv,event-to-mhpmcounters", &len);
	ఒకవేళ (event_ctr_map) {
		len = len / (పరిమాణం(u32) * 3);
		కొరకు (i = 0; i < len; i++) {
			event_idx_ప్రారంభం = fdt32_గమ్యం_cpu(event_ctr_map[3 * i]);
			event_idx_చివర = fdt32_గమ్యం_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_గమ్యం_cpu(event_ctr_map[3 * i + 2]);
			ఫలితం = sbi_pmu_కూడు_hw_event_counter_map(
				event_idx_ప్రారంభం, event_idx_చివర, ctr_map);
			ఒకవేళ (ఫలితం)
				తిరిగి ఫలితం;
		}
	}

	event_val = fdt_getprop(fdt, pmu_స్థానభ్రంశం,
				"riscv,event-to-mhpmevent", &len);
	ఒకవేళ (event_val) {
		len = len / (పరిమాణం(u32) * 3);

		hw_event_లెక్క = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_లెక్క,
						పరిమాణం(*fdt_pmu_evt_select));
		ఒకవేళ (!fdt_pmu_evt_select)
			తిరిగి SBI_ENOMEM;

		కొరకు (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_గమ్యం_cpu(event_val[3 * i]);
			event->select = fdt32_గమ్యం_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_గమ్యం_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_స్థానభ్రంశం,
				"riscv,raw-event-to-mhpmcounters", &len);
	ఒకవేళ (event_val) {
		len = len / (పరిమాణం(u32) * 5);
		కొరకు (i = 0; i < len; i++) {
			raw_selector = fdt32_గమ్యం_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_గమ్యం_cpu(event_val[5 * i + 1]);
			select_ముసుగు = fdt32_గమ్యం_cpu(event_val[5 * i + 2]);
			select_ముసుగు = (select_ముసుగు  << 32) |
					fdt32_గమ్యం_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_గమ్యం_cpu(event_val[5 * i + 4]);
			ఫలితం = sbi_pmu_కూడు_raw_event_counter_map(
					raw_selector, select_ముసుగు, ctr_map);
			ఒకవేళ (ఫలితం)
				తిరిగి ఫలితం;
		}
	}

	తిరిగి 0;
}
