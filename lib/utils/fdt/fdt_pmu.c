// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_pmu.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/fdt/fdt_pmu.h>

अटल काष्ठा fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
अटल uint32_t hw_event_गिनती;

uint64_t fdt_pmu_get_select_मूल्य(uint32_t event_idx)
{
	पूर्णांक i;
	काष्ठा fdt_pmu_hw_event_select_map *event;

	क्रम (i = 0; i < hw_event_गिनती; i++) {
		event = &fdt_pmu_evt_select[i];
		अगर (event->eidx == event_idx)
			वापस event->select;
	}

	वापस 0;
}

पूर्णांक fdt_pmu_fixup(व्योम *fdt)
{
	पूर्णांक pmu_offset;
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	अगर (!fdt)
		वापस SBI_EINVAL;

	pmu_offset = fdt_गाँठ_offset_by_compatible(fdt, -1, "riscv,pmu");
	अगर (pmu_offset < 0)
		वापस SBI_EFAIL;

	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_offset, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_offset, "riscv,raw-event-to-mhpmcounters");
	अगर (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_offset, "interrupts-extended");

	वापस 0;
}

पूर्णांक fdt_pmu_setup(स्थिर व्योम *fdt)
{
	पूर्णांक i, pmu_offset, len, परिणाम;
	स्थिर u32 *event_val;
	स्थिर u32 *event_ctr_map;
	काष्ठा fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_mask;
	u32 event_idx_start, event_idx_end, ctr_map;

	अगर (!fdt)
		वापस SBI_EINVAL;

	pmu_offset = fdt_गाँठ_offset_by_compatible(fdt, -1, "riscv,pmu");
	अगर (pmu_offset < 0)
		वापस SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_offset,
				    "riscv,event-to-mhpmcounters", &len);
	अगर (event_ctr_map) {
		len = len / (माप(u32) * 3);
		क्रम (i = 0; i < len; i++) {
			event_idx_start = fdt32_to_cpu(event_ctr_map[3 * i]);
			event_idx_end = fdt32_to_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_to_cpu(event_ctr_map[3 * i + 2]);
			परिणाम = sbi_pmu_add_hw_event_गणक_map(
				event_idx_start, event_idx_end, ctr_map);
			अगर (परिणाम)
				वापस परिणाम;
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,event-to-mhpmevent", &len);
	अगर (event_val) {
		len = len / (माप(u32) * 3);

		hw_event_गिनती = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_गिनती,
						माप(*fdt_pmu_evt_select));
		अगर (!fdt_pmu_evt_select)
			वापस SBI_ENOMEM;

		क्रम (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_to_cpu(event_val[3 * i]);
			event->select = fdt32_to_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_to_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_offset,
				"riscv,raw-event-to-mhpmcounters", &len);
	अगर (event_val) {
		len = len / (माप(u32) * 5);
		क्रम (i = 0; i < len; i++) {
			raw_selector = fdt32_to_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_to_cpu(event_val[5 * i + 1]);
			select_mask = fdt32_to_cpu(event_val[5 * i + 2]);
			select_mask = (select_mask  << 32) |
					fdt32_to_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_to_cpu(event_val[5 * i + 4]);
			परिणाम = sbi_pmu_add_raw_event_गणक_map(
					raw_selector, select_mask, ctr_map);
			अगर (परिणाम)
				वापस परिणाम;
		}
	}

	वापस 0;
}
