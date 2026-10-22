// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmu.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_pmu.h>

ಸ್ಥಿರ ರಚನೆ fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
ಸ್ಥಿರ uint32_t hw_event_ಎಣಿಕೆ;

uint64_t fdt_pmu_get_select_ಮೌಲ್ಯ(uint32_t event_idx)
{
	ಪೂರ್ಣಾಂಕ i;
	ರಚನೆ fdt_pmu_hw_event_select_map *event;

	ಗಾಗಿ (i = 0; i < hw_event_ಎಣಿಕೆ; i++) {
		event = &fdt_pmu_evt_select[i];
		ಆದರೆ (event->eidx == event_idx)
			ಹಿಂತಿರುಗಿ event->select;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_pmu_fixup(ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಂಕ pmu_ಸ್ಥಾನಾಂತರ;
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ಆದರೆ (!fdt)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	pmu_ಸ್ಥಾನಾಂತರ = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_compatible(fdt, -1, "riscv,pmu");
	ಆದರೆ (pmu_ಸ್ಥಾನಾಂತರ < 0)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	fdt_delprop(fdt, pmu_ಸ್ಥಾನಾಂತರ, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_ಸ್ಥಾನಾಂತರ, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_ಸ್ಥಾನಾಂತರ, "riscv,raw-event-to-mhpmcounters");
	ಆದರೆ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_ಸ್ಥಾನಾಂತರ, "interrupts-extended");

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_pmu_ಸಿದ್ಧತೆ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಂಕ i, pmu_ಸ್ಥಾನಾಂತರ, len, ಫಲಿತಾಂಶ;
	ಸ್ಥಿರಾಂಕ u32 *event_val;
	ಸ್ಥಿರಾಂಕ u32 *event_ctr_map;
	ರಚನೆ fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_ಮುಖವಾಡ;
	u32 event_idx_ಆರಂಭ, event_idx_ಕೊನೆ, ctr_map;

	ಆದರೆ (!fdt)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	pmu_ಸ್ಥಾನಾಂತರ = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_compatible(fdt, -1, "riscv,pmu");
	ಆದರೆ (pmu_ಸ್ಥಾನಾಂತರ < 0)
		ಹಿಂತಿರುಗಿ SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_ಸ್ಥಾನಾಂತರ,
				    "riscv,event-to-mhpmcounters", &len);
	ಆದರೆ (event_ctr_map) {
		len = len / (ಗಾತ್ರ(u32) * 3);
		ಗಾಗಿ (i = 0; i < len; i++) {
			event_idx_ಆರಂಭ = fdt32_ಗಮ್ಯ_cpu(event_ctr_map[3 * i]);
			event_idx_ಕೊನೆ = fdt32_ಗಮ್ಯ_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_ಗಮ್ಯ_cpu(event_ctr_map[3 * i + 2]);
			ಫಲಿತಾಂಶ = sbi_pmu_ಕೂಡಿಸು_hw_event_counter_map(
				event_idx_ಆರಂಭ, event_idx_ಕೊನೆ, ctr_map);
			ಆದರೆ (ಫಲಿತಾಂಶ)
				ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
		}
	}

	event_val = fdt_getprop(fdt, pmu_ಸ್ಥಾನಾಂತರ,
				"riscv,event-to-mhpmevent", &len);
	ಆದರೆ (event_val) {
		len = len / (ಗಾತ್ರ(u32) * 3);

		hw_event_ಎಣಿಕೆ = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_ಎಣಿಕೆ,
						ಗಾತ್ರ(*fdt_pmu_evt_select));
		ಆದರೆ (!fdt_pmu_evt_select)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;

		ಗಾಗಿ (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_ಗಮ್ಯ_cpu(event_val[3 * i]);
			event->select = fdt32_ಗಮ್ಯ_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_ಗಮ್ಯ_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_ಸ್ಥಾನಾಂತರ,
				"riscv,raw-event-to-mhpmcounters", &len);
	ಆದರೆ (event_val) {
		len = len / (ಗಾತ್ರ(u32) * 5);
		ಗಾಗಿ (i = 0; i < len; i++) {
			raw_selector = fdt32_ಗಮ್ಯ_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_ಗಮ್ಯ_cpu(event_val[5 * i + 1]);
			select_ಮುಖವಾಡ = fdt32_ಗಮ್ಯ_cpu(event_val[5 * i + 2]);
			select_ಮುಖವಾಡ = (select_ಮುಖವಾಡ  << 32) |
					fdt32_ಗಮ್ಯ_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_ಗಮ್ಯ_cpu(event_val[5 * i + 4]);
			ಫಲಿತಾಂಶ = sbi_pmu_ಕೂಡಿಸು_raw_event_counter_map(
					raw_selector, select_ಮುಖವಾಡ, ctr_map);
			ಆದರೆ (ಫಲಿತಾಂಶ)
				ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
		}
	}

	ಹಿಂತಿರುಗಿ 0;
}
