// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_pmu.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_pmu.h>

ସ୍ଥିର ସଂରଚନା fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
ସ୍ଥିର uint32_t hw_event_ଗଣନା;

uint64_t fdt_pmu_get_select_ମୂଲ୍ୟ(uint32_t event_idx)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ସଂରଚନା fdt_pmu_hw_event_select_map *event;

	ପାଇଁ (i = 0; i < hw_event_ଗଣନା; i++) {
		event = &fdt_pmu_evt_select[i];
		ଯଦି (event->eidx == event_idx)
			ଫେରନ୍ତୁ event->select;
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_pmu_fixup(ଶୂନ୍ୟ *fdt)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_offset;
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ଯଦି (!fdt)
		ଫେରନ୍ତୁ SBI_EINVAL;

	pmu_offset = fdt_ଗଣ୍ଠି_offset_by_compatible(fdt, -1, "riscv,pmu");
	ଯଦି (pmu_offset < 0)
		ଫେରନ୍ତୁ SBI_EFAIL;

	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_offset, "riscv,raw-event-to-mhpmcounters");
	ଯଦି (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_offset, "interrupts-extended");

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_pmu_setup(ଅଚଳ ଶୂନ୍ୟ *fdt)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, pmu_offset, len, result;
	ଅଚଳ u32 *event_val;
	ଅଚଳ u32 *event_ctr_map;
	ସଂରଚନା fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_mask;
	u32 event_idx_start, event_idx_end, ctr_map;

	ଯଦି (!fdt)
		ଫେରନ୍ତୁ SBI_EINVAL;

	pmu_offset = fdt_ଗଣ୍ଠି_offset_by_compatible(fdt, -1, "riscv,pmu");
	ଯଦି (pmu_offset < 0)
		ଫେରନ୍ତୁ SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_offset,
				    "riscv,event-to-mhpmcounters", &len);
	ଯଦି (event_ctr_map) {
		len = len / (ଆକାର(u32) * 3);
		ପାଇଁ (i = 0; i < len; i++) {
			event_idx_start = fdt32_to_cpu(event_ctr_map[3 * i]);
			event_idx_end = fdt32_to_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_to_cpu(event_ctr_map[3 * i + 2]);
			result = sbi_pmu_add_hw_event_counter_map(
				event_idx_start, event_idx_end, ctr_map);
			ଯଦି (result)
				ଫେରନ୍ତୁ result;
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,event-to-mhpmevent", &len);
	ଯଦି (event_val) {
		len = len / (ଆକାର(u32) * 3);

		hw_event_ଗଣନା = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_ଗଣନା,
						ଆକାର(*fdt_pmu_evt_select));
		ଯଦି (!fdt_pmu_evt_select)
			ଫେରନ୍ତୁ SBI_ENOMEM;

		ପାଇଁ (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_to_cpu(event_val[3 * i]);
			event->select = fdt32_to_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_to_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,raw-event-to-mhpmcounters", &len);
	ଯଦି (event_val) {
		len = len / (ଆକାର(u32) * 5);
		ପାଇଁ (i = 0; i < len; i++) {
			raw_selector = fdt32_to_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_to_cpu(event_val[5 * i + 1]);
			select_mask = fdt32_to_cpu(event_val[5 * i + 2]);
			select_mask = (select_mask  << 32) |
					fdt32_to_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_to_cpu(event_val[5 * i + 4]);
			result = sbi_pmu_add_raw_event_counter_map(
					raw_selector, select_mask, ctr_map);
			ଯଦି (result)
				ଫେରନ୍ତୁ result;
		}
	}

	ଫେରନ୍ତୁ 0;
}
