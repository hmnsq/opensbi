// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_pmu.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_pmu.h>

સ્થિર સંરચના fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
સ્થિર uint32_t hw_event_ગણતરી;

uint64_t fdt_pmu_get_select_કિંમત(uint32_t event_idx)
{
	પૂર્ણાંક i;
	સંરચના fdt_pmu_hw_event_select_map *event;

	માટે (i = 0; i < hw_event_ગણતરી; i++) {
		event = &fdt_pmu_evt_select[i];
		જો (event->eidx == event_idx)
			પાછા event->select;
	}

	પાછા 0;
}

પૂર્ણાંક fdt_pmu_fixup(શૂન્ય *fdt)
{
	પૂર્ણાંક pmu_offset;
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	જો (!fdt)
		પાછા SBI_EINVAL;

	pmu_offset = fdt_ગાંઠ_offset_by_compatible(fdt, -1, "riscv,pmu");
	જો (pmu_offset < 0)
		પાછા SBI_EFAIL;

	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_offset, "riscv,raw-event-to-mhpmcounters");
	જો (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_offset, "interrupts-extended");

	પાછા 0;
}

પૂર્ણાંક fdt_pmu_setup(અચલ શૂન્ય *fdt)
{
	પૂર્ણાંક i, pmu_offset, len, result;
	અચલ u32 *event_val;
	અચલ u32 *event_ctr_map;
	સંરચના fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_mask;
	u32 event_idx_start, event_idx_end, ctr_map;

	જો (!fdt)
		પાછા SBI_EINVAL;

	pmu_offset = fdt_ગાંઠ_offset_by_compatible(fdt, -1, "riscv,pmu");
	જો (pmu_offset < 0)
		પાછા SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_offset,
				    "riscv,event-to-mhpmcounters", &len);
	જો (event_ctr_map) {
		len = len / (કદ(u32) * 3);
		માટે (i = 0; i < len; i++) {
			event_idx_start = fdt32_to_cpu(event_ctr_map[3 * i]);
			event_idx_end = fdt32_to_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_to_cpu(event_ctr_map[3 * i + 2]);
			result = sbi_pmu_add_hw_event_counter_map(
				event_idx_start, event_idx_end, ctr_map);
			જો (result)
				પાછા result;
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,event-to-mhpmevent", &len);
	જો (event_val) {
		len = len / (કદ(u32) * 3);

		hw_event_ગણતરી = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_ગણતરી,
						કદ(*fdt_pmu_evt_select));
		જો (!fdt_pmu_evt_select)
			પાછા SBI_ENOMEM;

		માટે (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_to_cpu(event_val[3 * i]);
			event->select = fdt32_to_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_to_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,raw-event-to-mhpmcounters", &len);
	જો (event_val) {
		len = len / (કદ(u32) * 5);
		માટે (i = 0; i < len; i++) {
			raw_selector = fdt32_to_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_to_cpu(event_val[5 * i + 1]);
			select_mask = fdt32_to_cpu(event_val[5 * i + 2]);
			select_mask = (select_mask  << 32) |
					fdt32_to_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_to_cpu(event_val[5 * i + 4]);
			result = sbi_pmu_add_raw_event_counter_map(
					raw_selector, select_mask, ctr_map);
			જો (result)
				પાછા result;
		}
	}

	પાછા 0;
}
