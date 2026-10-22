// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_pmu.c - Flat Device Tree PMU helper routines
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *	Atish Patra <atish.patra@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_pmu.h>

ਸਥਿਰ ਬਣਤਰ fdt_pmu_hw_event_select_map *fdt_pmu_evt_select;
ਸਥਿਰ uint32_t hw_event_ਗਿਣਤੀ;

uint64_t fdt_pmu_get_select_ਮੁੱਲ(uint32_t event_idx)
{
	ਪੂਰਨ i;
	ਬਣਤਰ fdt_pmu_hw_event_select_map *event;

	ਲਈ (i = 0; i < hw_event_ਗਿਣਤੀ; i++) {
		event = &fdt_pmu_evt_select[i];
		ਜੇ (event->eidx == event_idx)
			ਵਾਪਸ event->select;
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_pmu_fixup(ਖਾਲੀ *fdt)
{
	ਪੂਰਨ pmu_ਵਿੱਥ;
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ਜੇ (!fdt)
		ਵਾਪਸ SBI_EINVAL;

	pmu_ਵਿੱਥ = fdt_ਗੰਢ_ਵਿੱਥ_by_compatible(fdt, -1, "riscv,pmu");
	ਜੇ (pmu_ਵਿੱਥ < 0)
		ਵਾਪਸ SBI_EFAIL;

	fdt_delprop(fdt, pmu_ਵਿੱਥ, "riscv,event-to-mhpmcounters");
	fdt_delprop(fdt, pmu_ਵਿੱਥ, "riscv,event-to-mhpmevent");
	fdt_delprop(fdt, pmu_ਵਿੱਥ, "riscv,raw-event-to-mhpmcounters");
	ਜੇ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		fdt_delprop(fdt, pmu_ਵਿੱਥ, "interrupts-extended");

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_pmu_ਤਿਆਰੀ(ਅਚਲ ਖਾਲੀ *fdt)
{
	ਪੂਰਨ i, pmu_ਵਿੱਥ, len, ਨਤੀਜਾ;
	ਅਚਲ u32 *event_val;
	ਅਚਲ u32 *event_ctr_map;
	ਬਣਤਰ fdt_pmu_hw_event_select_map *event;
	uint64_t raw_selector, select_ਨਕਾਬ;
	u32 event_idx_ਸ਼ੁਰੂ, event_idx_ਸਿਰਾ, ctr_map;

	ਜੇ (!fdt)
		ਵਾਪਸ SBI_EINVAL;

	pmu_ਵਿੱਥ = fdt_ਗੰਢ_ਵਿੱਥ_by_compatible(fdt, -1, "riscv,pmu");
	ਜੇ (pmu_ਵਿੱਥ < 0)
		ਵਾਪਸ SBI_ENOENT;

	event_ctr_map = fdt_getprop(fdt, pmu_ਵਿੱਥ,
				    "riscv,event-to-mhpmcounters", &len);
	ਜੇ (event_ctr_map) {
		len = len / (ਆਕਾਰ(u32) * 3);
		ਲਈ (i = 0; i < len; i++) {
			event_idx_ਸ਼ੁਰੂ = fdt32_ਮੰਜ਼ਿਲ_cpu(event_ctr_map[3 * i]);
			event_idx_ਸਿਰਾ = fdt32_ਮੰਜ਼ਿਲ_cpu(event_ctr_map[3 * i + 1]);
			ctr_map = fdt32_ਮੰਜ਼ਿਲ_cpu(event_ctr_map[3 * i + 2]);
			ਨਤੀਜਾ = sbi_pmu_ਜੋੜੋ_hw_event_counter_map(
				event_idx_ਸ਼ੁਰੂ, event_idx_ਸਿਰਾ, ctr_map);
			ਜੇ (ਨਤੀਜਾ)
				ਵਾਪਸ ਨਤੀਜਾ;
		}
	}

	event_val = fdt_getprop(fdt, pmu_ਵਿੱਥ,
				"riscv,event-to-mhpmevent", &len);
	ਜੇ (event_val) {
		len = len / (ਆਕਾਰ(u32) * 3);

		hw_event_ਗਿਣਤੀ = len;
		fdt_pmu_evt_select = sbi_calloc(hw_event_ਗਿਣਤੀ,
						ਆਕਾਰ(*fdt_pmu_evt_select));
		ਜੇ (!fdt_pmu_evt_select)
			ਵਾਪਸ SBI_ENOMEM;

		ਲਈ (i = 0; i < len; i++) {
			event = &fdt_pmu_evt_select[i];
			event->eidx = fdt32_ਮੰਜ਼ਿਲ_cpu(event_val[3 * i]);
			event->select = fdt32_ਮੰਜ਼ਿਲ_cpu(event_val[3 * i + 1]);
			event->select = (event->select << 32) |
					fdt32_ਮੰਜ਼ਿਲ_cpu(event_val[3 * i + 2]);
		}
	}

	event_val = fdt_getprop(fdt, pmu_ਵਿੱਥ,
				"riscv,raw-event-to-mhpmcounters", &len);
	ਜੇ (event_val) {
		len = len / (ਆਕਾਰ(u32) * 5);
		ਲਈ (i = 0; i < len; i++) {
			raw_selector = fdt32_ਮੰਜ਼ਿਲ_cpu(event_val[5 * i]);
			raw_selector = (raw_selector << 32) |
					fdt32_ਮੰਜ਼ਿਲ_cpu(event_val[5 * i + 1]);
			select_ਨਕਾਬ = fdt32_ਮੰਜ਼ਿਲ_cpu(event_val[5 * i + 2]);
			select_ਨਕਾਬ = (select_ਨਕਾਬ  << 32) |
					fdt32_ਮੰਜ਼ਿਲ_cpu(event_val[5 * i + 3]);
			ctr_map = fdt32_ਮੰਜ਼ਿਲ_cpu(event_val[5 * i + 4]);
			ਨਤੀਜਾ = sbi_pmu_ਜੋੜੋ_raw_event_counter_map(
					raw_selector, select_ਨਕਾਬ, ctr_map);
			ਜੇ (ਨਤੀਜਾ)
				ਵਾਪਸ ਨਤੀਜਾ;
		}
	}

	ਵਾਪਸ 0;
}
