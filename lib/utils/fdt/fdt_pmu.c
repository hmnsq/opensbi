// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_pmu.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_pmu.h>

സ്ഥിതം ഘടന fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
സ്ഥിതം uint32_t hw_event_എണ്ണം;

uint64_t fdt_pmu_get_select_മൂല്യം(uint32_t event_idx)
{
	പൂർണ്ണസംഖ്യ i;
	ഘടന fdt_pmu_hw_event_select_map *event;

	വേണ്ടി (i = 0; i < hw_event_എണ്ണം; i++) {
		event = &fdt_pmu_evt_select[i];
		എങ്കിൽ (event->eidx == event_idx)
			തിരികെ event->select;
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_pmu_fixup(ശൂന്യം *fdt)
{
	പൂർണ്ണസംഖ്യ pmu_offset;
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	എങ്കിൽ (!fdt)
		തിരികെ SBI_EINVAL;

	pmu_offset = fdt_കണ്ണി_offset_by_compatible(fdt, -1, "riscv,pmu");
	എങ്കിൽ (pmu_offset < 0)
		തിരികെ SBI_EFAIL;

	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_offset, "riscv,raw-event-to-mhpmcounters");
	എങ്കിൽ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_offset, "interrupts-extended");

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_pmu_setup(സ്ഥിരം ശൂന്യം *fdt)
{
	പൂർണ്ണസംഖ്യ i, pmu_offset, len, result;
	സ്ഥിരം u32 *event_val;
	സ്ഥിരം u32 *event_ctr_map;
	ഘടന fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_mask;
	u32 event_idx_start, event_idx_end, ctr_map;

	എങ്കിൽ (!fdt)
		തിരികെ SBI_EINVAL;

	pmu_offset = fdt_കണ്ണി_offset_by_compatible(fdt, -1, "riscv,pmu");
	എങ്കിൽ (pmu_offset < 0)
		തിരികെ SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_offset,
				    "riscv,event-to-mhpmcounters", &len);
	എങ്കിൽ (event_ctr_map) {
		len = len / (വലുപ്പം(u32) * 3);
		വേണ്ടി (i = 0; i < len; i++) {
			event_idx_start = fdt32_to_cpu(event_ctr_map[3 * i]);
			event_idx_end = fdt32_to_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_to_cpu(event_ctr_map[3 * i + 2]);
			result = sbi_pmu_add_hw_event_counter_map(
				event_idx_start, event_idx_end, ctr_map);
			എങ്കിൽ (result)
				തിരികെ result;
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,event-to-mhpmevent", &len);
	എങ്കിൽ (event_val) {
		len = len / (വലുപ്പം(u32) * 3);

		hw_event_എണ്ണം = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_എണ്ണം,
						വലുപ്പം(*fdt_pmu_evt_select));
		എങ്കിൽ (!fdt_pmu_evt_select)
			തിരികെ SBI_ENOMEM;

		വേണ്ടി (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_to_cpu(event_val[3 * i]);
			event->select = fdt32_to_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_to_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,raw-event-to-mhpmcounters", &len);
	എങ്കിൽ (event_val) {
		len = len / (വലുപ്പം(u32) * 5);
		വേണ്ടി (i = 0; i < len; i++) {
			raw_selector = fdt32_to_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_to_cpu(event_val[5 * i + 1]);
			select_mask = fdt32_to_cpu(event_val[5 * i + 2]);
			select_mask = (select_mask  << 32) |
					fdt32_to_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_to_cpu(event_val[5 * i + 4]);
			result = sbi_pmu_add_raw_event_counter_map(
					raw_selector, select_mask, ctr_map);
			എങ്കിൽ (result)
				തിരികെ result;
		}
	}

	തിരികെ 0;
}
