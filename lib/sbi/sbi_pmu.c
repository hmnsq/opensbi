/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart_protection.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmu.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_sse.h>

/** Information about hardware counters */
ರಚನೆ sbi_pmu_hw_event {
	uint32_t counters;
	uint32_t ಆರಂಭ_idx;
	uint32_t ಕೊನೆ_idx;
	/* Event selector value used only for raw events. The event select value
	 * can be a even id or a selector value for set of events encoded in few
	 * bits. In case latter, the bits used for encoding of the events should
	 * be zeroed out in the select value.
	 */
	uint64_t select;
	 /**
	  * The select_mask indicates which bits are encoded for the event(s).
	  */
	uint64_t select_ಮುಖವಾಡ;
};

/* Information about PMU counters as per SBI specification */
ಒಕ್ಕೂಟ sbi_pmu_ctr_info {
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ;
	ರಚನೆ {
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ csr:12;
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಅಗಲ:6;
#ಆದರೆ __riscv_xlen == 32
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reserved:13;
#ಇಲ್ಲದಿದ್ದರೆ
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reserved:45;
#ಆದರೆ_ಅಂತ್ಯ
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ type:1;
	};
};

#ಆದರೆ SBI_PMU_FW_CTR_MAX >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG
#ದೋಷ "Can't handle firmware counters beyond BITS_PER_LONG"
#ಆದರೆ_ಅಂತ್ಯ

/** HW event configuration parameters */
ರಚನೆ sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_ದತ್ತಾಂಶ;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
ರಚನೆ sbi_pmu_hart_state {
	/* HART to which this state belongs */
	uint32_t hartid;
	/* Counter to enabled event mapping */
	uint32_t ಸಕ್ರಿಯ_events[SBI_PMU_HW_CTR_MAX + SBI_PMU_FW_CTR_MAX];
	/* Bitmap of firmware counters started */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fw_counters_started;
	/* if true, SSE is enabled */
	bool sse_enabled;
	/*
	 * Counter values for SBI firmware events and event codes
	 * for platform firmware events. Both are mutually exclusive
	 * and hence can optimally share the same memory.
	 */
	uint64_t fw_counters_ದತ್ತಾಂಶ[SBI_PMU_FW_CTR_MAX];
	/* HW events configuration parameters from
	 * sbi_pmu_ctr_cfg_match() command which are
	 * used for restoring RAW hardware events after
	 * cpu suspending.
	 */
	ರಚನೆ sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ phs_ptr_ಸ್ಥಾನಾಂತರ;

#ವ್ಯಾಖ್ಯಾನಿಸು pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_ಸ್ಥಾನಾಂತರ ? sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, phs_ptr_ಸ್ಥಾನಾಂತರ) : ಖಾಲಿ

#ವ್ಯಾಖ್ಯಾನಿಸು pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#ವ್ಯಾಖ್ಯಾನಿಸು pmu_ಗಣ_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, phs_ptr_ಸ್ಥಾನಾಂತರ, (__phs))

/* Platform specific PMU device */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_pmu_device *pmu_dev = ಖಾಲಿ;

/* Mapping between event range and possible counters  */
ಸ್ಥಿರ ರಚನೆ sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
ಸ್ಥಿರ uint32_t ಅಂಕೆ_hw_events;
/* Maximum number of hardware counters available */
ಸ್ಥಿರ uint32_t ಅಂಕೆ_hw_ctrs;

/* Maximum number of counters available */
ಸ್ಥಿರ uint32_t ಒಟ್ಟು_ctrs;

/* Helper macros to retrieve event idx and code type */
#ವ್ಯಾಖ್ಯಾನಿಸು get_cidx_type(x) \
  (((x) & SBI_PMU_EVENT_IDX_TYPE_ಮುಖವಾಡ) >> SBI_PMU_EVENT_IDX_TYPE_ಸ್ಥಾನಾಂತರ)
#ವ್ಯಾಖ್ಯಾನಿಸು get_cidx_ಸಂಕೇತ(x) (x & SBI_PMU_EVENT_IDX_ಸಂಕೇತ_ಮುಖವಾಡ)

/**
 * Perform a sanity check on event & counter mappings with event range overlap check
 * @param evtA Pointer to the existing hw event structure
 * @param evtB Pointer to the new hw event structure
 *
 * Return false if the range doesn't overlap, true otherwise
 */
ಸ್ಥಿರ bool pmu_event_range_overlap(ರಚನೆ sbi_pmu_hw_event *evtA,
				    ರಚನೆ sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	ಆದರೆ (((evtA->ಕೊನೆ_idx < evtB->ಆರಂಭ_idx) && (evtA->ಕೊನೆ_idx < evtB->ಕೊನೆ_idx)) ||
	   ((evtA->ಆರಂಭ_idx > evtB->ಆರಂಭ_idx) && (evtA->ಆರಂಭ_idx > evtB->ಕೊನೆ_idx)))
		ಹಿಂತಿರುಗಿ false;
	ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ bool pmu_event_select_overlap(ರಚನೆ sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_ಮುಖವಾಡ)
{
	ಆದರೆ ((evt->select == select_val) && (evt->select_ಮುಖವಾಡ == select_ಮುಖವಾಡ))
		ಹಿಂತಿರುಗಿ true;

	ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_event_validate(ರಚನೆ sbi_pmu_hart_state *phs,
			      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_ಸಂಕೇತ = get_cidx_ಸಂಕೇತ(event_idx);
	uint32_t event_idx_ಸಂಕೇತ_max = -1;
	uint32_t cache_ops_ಫಲಿತಾಂಶ, cache_ops_id, cache_id;

	ಆಯ್ಕೆ(event_idx_type) {
	ಸ್ಥಿತಿ SBI_PMU_EVENT_TYPE_HW:
		event_idx_ಸಂಕೇತ_max = SBI_PMU_HW_GENERAL_MAX;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_PMU_EVENT_TYPE_FW:
		ಆದರೆ ((event_idx_ಸಂಕೇತ >= SBI_PMU_FW_MAX &&
		    event_idx_ಸಂಕೇತ <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_ಸಂಕೇತ > SBI_PMU_FW_PLATFORM)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;

		ಆದರೆ (SBI_PMU_FW_PLATFORM == event_idx_ಸಂಕೇತ &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			ಹಿಂತಿರುಗಿ pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		ಇಲ್ಲದಿದ್ದರೆ
			event_idx_ಸಂಕೇತ_max = SBI_PMU_FW_MAX;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_ಫಲಿತಾಂಶ = event_idx_ಸಂಕೇತ &
					SBI_PMU_EVENT_HW_CACHE_OPS_ಫಲಿತಾಂಶ;
		cache_ops_id = (event_idx_ಸಂಕೇತ &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_ಮುಖವಾಡ) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_ಸ್ಥಾನಾಂತರ;
		cache_id = (event_idx_ಸಂಕೇತ &
			    SBI_PMU_EVENT_HW_CACHE_ID_ಮುಖವಾಡ) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_ಸ್ಥಾನಾಂತರ;
		ಆದರೆ ((cache_ops_ಫಲಿತಾಂಶ < SBI_PMU_HW_CACHE_ಫಲಿತಾಂಶ_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			ಹಿಂತಿರುಗಿ event_idx_type;
		ಇಲ್ಲದಿದ್ದರೆ
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_PMU_EVENT_TYPE_HW_RAW:
	ಸ್ಥಿತಿ SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_ಸಂಕೇತ_max = 1; // event_idx.code should be zero
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	ಆದರೆ (event_idx_ಸಂಕೇತ < event_idx_ಸಂಕೇತ_max)
		ಹಿಂತಿರುಗಿ event_idx_type;

	ಹಿಂತಿರುಗಿ SBI_EINVAL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_ctr_validate(ರಚನೆ sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_ಸಂಕೇತ)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	ಆದರೆ (cidx >= ಒಟ್ಟು_ctrs)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	event_idx_val = phs->ಸಕ್ರಿಯ_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	ಆದರೆ (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	*event_idx_ಸಂಕೇತ = get_cidx_ಸಂಕೇತ(event_idx_val);

	ಹಿಂತಿರುಗಿ event_idx_type;
}

ಸ್ಥಿರ bool pmu_ctr_idx_validate(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cbase, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cmask)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಕೊನೆಯ;

	ಆದರೆ (!cmask)
		ಹಿಂತಿರುಗಿ false;

	ಕೊನೆಯ = sbi_fls(cmask);
	ಆದರೆ (cbase > -1UL - ಕೊನೆಯ)
		ಹಿಂತಿರುಗಿ false;

	ಹಿಂತಿರುಗಿ (cbase + ಕೊನೆಯ) < ಒಟ್ಟು_ctrs;
}

ಪೂರ್ಣಾಂಕ sbi_pmu_ctr_fw_read(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cidx, uint64_t *cval, bool ಮೇಲಿನ_ಬಿಟ್ಟುಗಳು)
{
	ಪೂರ್ಣಾಂಕ event_idx_type;
	uint32_t event_ಸಂಕೇತ;
	ರಚನೆ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ಆದರೆ (unlikely(!phs))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (cidx < ಅಂಕೆ_hw_ctrs || cidx >= ಒಟ್ಟು_ctrs)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

#ಆದರೆ __riscv_xlen > 32
	ಆದರೆ (ಮೇಲಿನ_ಬಿಟ್ಟುಗಳು)
		ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_ಸಂಕೇತ);
	ಆದರೆ (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ ((event_ಸಂಕೇತ >= SBI_PMU_FW_MAX &&
	    event_ಸಂಕೇತ <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_ಸಂಕೇತ > SBI_PMU_FW_PLATFORM)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (SBI_PMU_FW_PLATFORM == event_ಸಂಕೇತ) {
		ಆದರೆ (pmu_dev && pmu_dev->fw_counter_read_ಮೌಲ್ಯ)
			*cval = pmu_dev->fw_counter_read_ಮೌಲ್ಯ(phs->hartid,
							       cidx -
							       ಅಂಕೆ_hw_ctrs);
		ಇಲ್ಲದಿದ್ದರೆ
			*cval = 0;
	} ಇಲ್ಲದಿದ್ದರೆ
		*cval = phs->fw_counters_ದತ್ತಾಂಶ[cidx - ಅಂಕೆ_hw_ctrs];

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_ಕೂಡಿಸು_hw_event_map(u32 eidx_ಆರಂಭ, u32 eidx_ಕೊನೆ, u32 cmap,
				uint64_t select, uint64_t select_ಮುಖವಾಡ)
{
	ಪೂರ್ಣಾಂಕ i = 0;
	bool is_overlap;
	ರಚನೆ sbi_pmu_hw_event *event = &hw_event_map[ಅಂಕೆ_hw_events];
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_ಮುಖವಾಡ = sbi_hart_mhpm_ಮುಖವಾಡ(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	ಆದರೆ (eidx_ಆರಂಭ > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_ಮುಖವಾಡ))
		ಹಿಂತಿರುಗಿ SBI_EDENIED;

	ಆದರೆ (ಅಂಕೆ_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		ಹಿಂತಿರುಗಿ SBI_EFAIL;
	}

	event->ಆರಂಭ_idx = eidx_ಆರಂಭ;
	event->ಕೊನೆ_idx = eidx_ಕೊನೆ;

	/* Sanity check */
	ಗಾಗಿ (i = 0; i < ಅಂಕೆ_hw_events; i++) {
		ಆದರೆ (eidx_ಆರಂಭ == SBI_PMU_EVENT_RAW_IDX ||
			eidx_ಆರಂಭ == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_ಮುಖವಾಡ);
		ಇಲ್ಲದಿದ್ದರೆ
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		ಆದರೆ (is_overlap)
			ಹೋಗು reset_event;
	}

	event->select_ಮುಖವಾಡ = select_ಮುಖವಾಡ;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_ಮುಖವಾಡ;
	event->select = select;
	ಅಂಕೆ_hw_events++;

	ಹಿಂತಿರುಗಿ 0;

reset_event:
	event->ಆರಂಭ_idx = 0;
	event->ಕೊನೆ_idx = 0;
	ಹಿಂತಿರುಗಿ SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
ಪೂರ್ಣಾಂಕ sbi_pmu_ಕೂಡಿಸು_hw_event_counter_map(u32 eidx_ಆರಂಭ, u32 eidx_ಕೊನೆ, u32 cmap)
{
	ಆದರೆ ((eidx_ಆರಂಭ > eidx_ಕೊನೆ) || eidx_ಆರಂಭ >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_ಕೊನೆ >= SBI_PMU_EVENT_RAW_IDX)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಹಿಂತಿರುಗಿ pmu_ಕೂಡಿಸು_hw_event_map(eidx_ಆರಂಭ, eidx_ಕೊನೆ, cmap, 0, 0);
}

ಪೂರ್ಣಾಂಕ sbi_pmu_ಕೂಡಿಸು_raw_event_counter_map(uint64_t select, uint64_t select_ಮುಖವಾಡ, u32 cmap)
{
	ಹಿಂತಿರುಗಿ pmu_ಕೂಡಿಸು_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_ಮುಖವಾಡ);
}

ಶೂನ್ಯ sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_ಮುಖವಾಡ());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_ಅತಿಪ್ರವಾಹ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_ctr_enable_irq_hw(ಪೂರ್ಣಾಂಕ ctr_idx)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mhpmevent_csr;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mhpmevent_curr;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mip_val;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದ_ಮುಖವಾಡ;

	ಆದರೆ (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

#ಆದರೆ __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	ದ_ಮುಖವಾಡ = (uint32_t)~MHPMEVENTH_ದ;
#ಇಲ್ಲದಿದ್ದರೆ
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	ದ_ಮುಖವಾಡ = ~MHPMEVENT_ದ;
#ಆದರೆ_ಅಂತ್ಯ

	mhpmevent_curr = csr_read_ಅಂಕೆ(mhpmevent_csr);
	mip_val = csr_read(CSR_MIP);
	/**
	 * Clear out the OF bit so that next interrupt can be enabled.
	 * This should be done only when the corresponding overflow interrupt
	 * bit is cleared. That indicates that software has already handled the
	 * previous interrupts or the hardware yet to set an overflow interrupt.
	 * Otherwise, there will be race conditions where we may clear the bit
	 * the software is yet to handle the interrupt.
	 */
	ಆದರೆ (!(mip_val & sbi_pmu_irq_ಮುಖವಾಡ())) {
		mhpmevent_curr &= ದ_ಮುಖವಾಡ;
		csr_write_ಅಂಕೆ(mhpmevent_csr, mhpmevent_curr);
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#ಆದರೆ __riscv_xlen == 32
	csr_write_ಅಂಕೆ(CSR_MCYCLE + cidx, 0);
	csr_write_ಅಂಕೆ(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_ಅಂಕೆ(CSR_MCYCLEH + cidx, ival >> ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG);
#ಇಲ್ಲದಿದ್ದರೆ
	csr_write_ಅಂಕೆ(CSR_MCYCLE + cidx, ival);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_ctr_ಆರಂಭ_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	ಆದರೆ (cidx >= ಅಂಕೆ_hw_ctrs || cidx == 1)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) < SBI_HART_PRIV_VER_1_11) {
		ಆದರೆ (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		ಹಿಂತಿರುಗಿ 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	ಆದರೆ (!__test_bit(cidx, &mctr_inhbt))
		ಹಿಂತಿರುಗಿ SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	ಆದರೆ (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	ಆದರೆ (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_pmu_irq_ಬಿಟ್(ಶೂನ್ಯ)
{
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		ಹಿಂತಿರುಗಿ IRQ_PMU_OVF;
	ಆದರೆ (pmu_dev && pmu_dev->hw_counter_irq_ಬಿಟ್)
		ಹಿಂತಿರುಗಿ pmu_dev->hw_counter_irq_ಬಿಟ್();

	ಹಿಂತಿರುಗಿ -1;
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_pmu_irq_ಮುಖವಾಡ(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ irq_ಬಿಟ್ = sbi_pmu_irq_ಬಿಟ್();

	ಆದರೆ (irq_ಬಿಟ್ < 0)
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ ಬಿಟ್(irq_ಬಿಟ್);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_ctr_ಆರಂಭ_fw(ರಚನೆ sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_ಸಂಕೇತ,
			    uint64_t event_ದತ್ತಾಂಶ, uint64_t ival,
			    bool ival_update)
{
	ಪೂರ್ಣಾಂಕ ret;

	ಆದರೆ ((event_ಸಂಕೇತ >= SBI_PMU_FW_MAX &&
	    event_ಸಂಕೇತ <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_ಸಂಕೇತ > SBI_PMU_FW_PLATFORM)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (phs->fw_counters_started & ಬಿಟ್(cidx - ಅಂಕೆ_hw_ctrs))
		ಹಿಂತಿರುಗಿ SBI_EALREADY_STARTED;

	ಆದರೆ (SBI_PMU_FW_PLATFORM == event_ಸಂಕೇತ) {
		ಆದರೆ (!pmu_dev ||
		    !pmu_dev->fw_counter_write_ಮೌಲ್ಯ ||
		    !pmu_dev->fw_counter_ಆರಂಭ) {
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		}

		ಆದರೆ (ival_update)
			pmu_dev->fw_counter_write_ಮೌಲ್ಯ(phs->hartid,
							cidx - ಅಂಕೆ_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_ಆರಂಭ(phs->hartid,
						 cidx - ಅಂಕೆ_hw_ctrs,
						 event_ದತ್ತಾಂಶ);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (ival_update)
			phs->fw_counters_ದತ್ತಾಂಶ[cidx - ಅಂಕೆ_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= ಬಿಟ್(cidx - ಅಂಕೆ_hw_ctrs);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ pmu_update_inhibit_flags(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags, uint64_t *mhpmevent_val)
{
	ಆದರೆ (flags & SBI_PMU_CFG_FLAG_ಗಣ_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	ಆದರೆ (flags & SBI_PMU_CFG_FLAG_ಗಣ_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	ಆದರೆ (flags & SBI_PMU_CFG_FLAG_ಗಣ_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	ಆದರೆ (flags & SBI_PMU_CFG_FLAG_ಗಣ_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_update_hw_mhpmevent(ರಚನೆ sbi_pmu_hw_event *hw_evt, ಪೂರ್ಣಾಂಕ ctr_idx,
				   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ eindex,
				   uint64_t ದತ್ತಾಂಶ)
{
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_ಗಮ್ಯ_mhpmevent(plat, eindex, ದತ್ತಾಂಶ);

	ಆದರೆ (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_ಮುಖವಾಡ) |
				 MHPMEVENT_MINH | MHPMEVENT_ದ;

	ಆದರೆ (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	ಆದರೆ (pmu_dev && pmu_dev->hw_counter_filter_ಬಹುಲಕ)
		pmu_dev->hw_counter_filter_ಬಹುಲಕ(flags, ctr_idx);

#ಆದರೆ __riscv_xlen == 32
	csr_write_ಅಂಕೆ(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_ಅಂಕೆ(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG);
#ಇಲ್ಲದಿದ್ದರೆ
	csr_write_ಅಂಕೆ(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_pmu_ctr_ಆರಂಭ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cbase, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cmask,
		      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags, uint64_t ival)
{
	ರಚನೆ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ಆದರೆ (unlikely(!phs))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಪೂರ್ಣಾಂಕ event_idx_type;
	uint32_t event_ಸಂಕೇತ;
	ಪೂರ್ಣಾಂಕ ret = SBI_EINVAL;
	bool bUpdate = false;
	ಪೂರ್ಣಾಂಕ i, cidx;
	uint64_t edata;

	ಆದರೆ (!pmu_ctr_idx_validate(cbase, cmask))
		ಹಿಂತಿರುಗಿ ret;

	ಆದರೆ (flags & ~SBI_PMU_ಆರಂಭ_FLAGS_ಮುಖವಾಡ)
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	ಆದರೆ (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		ಹಿಂತಿರುಗಿ SBI_ENO_SHMEM;

	ಆದರೆ (flags & SBI_PMU_ಆರಂಭ_FLAG_ಗಣ_INIT_ಮೌಲ್ಯ)
		bUpdate = true;

	for_each_ಗಣ_ಬಿಟ್(i, &cmask, ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_ಸಂಕೇತ);
		ಆದರೆ (event_idx_type < 0)
			/* Continue the start operation for other counters */
			ಮುಂದುವರಿ;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_ಸಂಕೇತ == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_ದತ್ತಾಂಶ[cidx - ಅಂಕೆ_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_ಆರಂಭ_fw(phs, cidx, event_ಸಂಕೇತ, edata,
					       ival, bUpdate);
			ಆದರೆ (ret)
				ಹಿಂತಿರುಗಿ ret;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (cidx >= 3) {
				ರಚನೆ sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->ಸಕ್ರಿಯ_events[cidx],
							ev_cfg->event_ದತ್ತಾಂಶ);
				ಆದರೆ (ret)
					ಹಿಂತಿರುಗಿ ret;
			}
			ret = pmu_ctr_ಆರಂಭ_hw(cidx, ival, bUpdate);
			ಆದರೆ (ret)
				ಹಿಂತಿರುಗಿ ret;
		}
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_ctr_stop_hw(uint32_t cidx)
{
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mctr_inhbt;

	ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) < SBI_HART_PRIV_VER_1_11)
		ಹಿಂತಿರುಗಿ 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	ಆದರೆ (cidx >= ಅಂಕೆ_hw_ctrs || cidx == 1)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		ಆದರೆ (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		ಹಿಂತಿರುಗಿ 0;
	} ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ SBI_EALREADY_STOPPED;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_ctr_stop_fw(ರಚನೆ sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_ಸಂಕೇತ)
{
	ಪೂರ್ಣಾಂಕ ret;

	ಆದರೆ ((event_ಸಂಕೇತ >= SBI_PMU_FW_MAX &&
	    event_ಸಂಕೇತ <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_ಸಂಕೇತ > SBI_PMU_FW_PLATFORM)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (!(phs->fw_counters_started & ಬಿಟ್(cidx - ಅಂಕೆ_hw_ctrs)))
		ಹಿಂತಿರುಗಿ SBI_EALREADY_STOPPED;

	ಆದರೆ (SBI_PMU_FW_PLATFORM == event_ಸಂಕೇತ &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - ಅಂಕೆ_hw_ctrs);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	phs->fw_counters_started &= ~ಬಿಟ್(cidx - ಅಂಕೆ_hw_ctrs);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_reset_hw_mhpmevent(ಪೂರ್ಣಾಂಕ ctr_idx)
{
	ಆದರೆ (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;
#ಆದರೆ __riscv_xlen == 32
	csr_write_ಅಂಕೆ(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	ಆದರೆ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_ಅಂಕೆ(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#ಇಲ್ಲದಿದ್ದರೆ
	csr_write_ಅಂಕೆ(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_pmu_ctr_stop(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cbase, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cmask,
		     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flag)
{
	ರಚನೆ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ಆದರೆ (unlikely(!phs))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಪೂರ್ಣಾಂಕ ret = SBI_EINVAL;
	ಪೂರ್ಣಾಂಕ event_idx_type;
	uint32_t event_ಸಂಕೇತ;
	ಪೂರ್ಣಾಂಕ i, cidx;

	ಆದರೆ (!pmu_ctr_idx_validate(cbase, cmask))
		ಹಿಂತಿರುಗಿ ret;

	ಆದರೆ (flag & ~SBI_PMU_STOP_FLAGS_ಮುಖವಾಡ)
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	ಆದರೆ (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		ಹಿಂತಿರುಗಿ SBI_ENO_SHMEM;

	for_each_ಗಣ_ಬಿಟ್(i, &cmask, ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_ಸಂಕೇತ);
		ಆದರೆ (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			ಮುಂದುವರಿ;

		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_ಸಂಕೇತ);
		ಇಲ್ಲದಿದ್ದರೆ
			ret = pmu_ctr_stop_hw(cidx);

		ಆದರೆ(ret)
			ಹಿಂತಿರುಗಿ ret;

		ಆದರೆ (cidx > (CSR_INSTRET - CSR_ಚಕ್ರ) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->ಸಕ್ರಿಯ_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	ಆದರೆ (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_ಮುಖವಾಡ());

	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_fixed_ctr_update_inhibit_ಬಿಟ್ಟುಗಳು(ಪೂರ್ಣಾಂಕ fixed_ctr, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags)
{
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#ಆದರೆ __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_ಬಹುಲಕ))
		ಹಿಂತಿರುಗಿ fixed_ctr;

	ಆಯ್ಕೆ (fixed_ctr) {
	ಸ್ಥಿತಿ 0:
		cfg_csr_no = CSR_MCYCLECFG;
#ಆದರೆ __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#ಆದರೆ __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#ಆದರೆ __riscv_xlen == 32
		csr_write_ಅಂಕೆ(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_ಅಂಕೆ(cfgh_csr_no, cfg_val >> ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG);
#ಇಲ್ಲದಿದ್ದರೆ
		csr_write_ಅಂಕೆ(cfg_csr_no, cfg_val);
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಆದರೆ (pmu_dev && pmu_dev->hw_counter_filter_ಬಹುಲಕ)
		pmu_dev->hw_counter_filter_ಬಹುಲಕ(flags, fixed_ctr);
	ಹಿಂತಿರುಗಿ fixed_ctr;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_ctr_ಕಂಡುಹಿಡಿ_fixed_hw(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ evt_idx_ಸಂಕೇತ)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	ಆದರೆ (evt_idx_ಸಂಕೇತ == SBI_PMU_HW_CPU_ಚಕ್ರಗಳು)
		ಹಿಂತಿರುಗಿ 0;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (evt_idx_ಸಂಕೇತ == SBI_PMU_HW_INSTRUCTIONS)
		ಹಿಂತಿರುಗಿ 2;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_ctr_ಕಂಡುಹಿಡಿ_hw(ರಚನೆ sbi_pmu_hart_state *phs,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cbase, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cmask,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ event_idx, uint64_t ದತ್ತಾಂಶ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ctr_ಮುಖವಾಡ;
	ಪೂರ್ಣಾಂಕ i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	ರಚನೆ sbi_pmu_hw_event *ತಾತ್ಕಾಲಿಕ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mctr_inhbt = 0;
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ಆದರೆ (cbase >= ಅಂಕೆ_hw_ctrs)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_ಕಂಡುಹಿಡಿ_fixed_hw(event_idx);
	ಆದರೆ (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		ಹಿಂತಿರುಗಿ pmu_fixed_ctr_update_inhibit_ಬಿಟ್ಟುಗಳು(fixed_ctr, flags);

	ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	ಗಾಗಿ (i = 0; i < ಅಂಕೆ_hw_events; i++) {
		ತಾತ್ಕಾಲಿಕ = &hw_event_map[i];
		ಆದರೆ ((ತಾತ್ಕಾಲಿಕ->ಆರಂಭ_idx > event_idx && event_idx < ತಾತ್ಕಾಲಿಕ->ಕೊನೆ_idx) ||
		    (ತಾತ್ಕಾಲಿಕ->ಆರಂಭ_idx < event_idx && event_idx > ತಾತ್ಕಾಲಿಕ->ಕೊನೆ_idx))
			ಮುಂದುವರಿ;

		/* For raw events, event data is used as the select value */
		ಆದರೆ (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_ಮುಖವಾಡ = ತಾತ್ಕಾಲಿಕ->select_ಮುಖವಾಡ;

			/* The non-event map bits of data should match the selector */
			ಆದರೆ (ತಾತ್ಕಾಲಿಕ->select != (ದತ್ತಾಂಶ & select_ಮುಖವಾಡ))
				ಮುಂದುವರಿ;
		}
		/* Fixed counters should not be part of the search */
		ctr_ಮುಖವಾಡ = ತಾತ್ಕಾಲಿಕ->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_ಮುಖವಾಡ);
		for_each_ಗಣ_ಬಿಟ್_from(cbase, &ctr_ಮುಖವಾಡ, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			ಆದರೆ (phs->ಸಕ್ರಿಯ_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				ಮುಂದುವರಿ;
			/* If mcountinhibit is supported, the bit must be enabled */
			ಆದರೆ ((sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				ಮುಂದುವರಿ;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			ನಿಲ್ಲಿಸು;
		}
	}

	ಆದರೆ (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		ಆದರೆ (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			ಹಿಂತಿರುಗಿ SBI_EFAIL;

		ಹಿಂತಿರುಗಿ pmu_fixed_ctr_update_inhibit_ಬಿಟ್ಟುಗಳು(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(ತಾತ್ಕಾಲಿಕ, ctr_idx, flags, event_idx, ದತ್ತಾಂಶ);

	ಆದರೆ (!ret)
		ret = ctr_idx;

	ಹಿಂತಿರುಗಿ ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pmu_ctr_ಕಂಡುಹಿಡಿ_fw(ರಚನೆ sbi_pmu_hart_state *phs,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cbase, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cmask,
			   uint32_t event_ಸಂಕೇತ, uint64_t edata)
{
	ಪೂರ್ಣಾಂಕ i, cidx;

	for_each_ಗಣ_ಬಿಟ್(i, &cmask, ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG) {
		cidx = i + cbase;
		ಆದರೆ (cidx < ಅಂಕೆ_hw_ctrs || ಒಟ್ಟು_ctrs <= cidx)
			ಮುಂದುವರಿ;
		ಆದರೆ (phs->ಸಕ್ರಿಯ_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			ಮುಂದುವರಿ;
		ಆದರೆ (SBI_PMU_FW_PLATFORM == event_ಸಂಕೇತ &&
		    pmu_dev && pmu_dev->fw_counter_ಹೊಂದಿಕೆ_encoding) {
			ಆದರೆ (!pmu_dev->fw_counter_ಹೊಂದಿಕೆ_encoding(phs->hartid,
							    cidx - ಅಂಕೆ_hw_ctrs,
							    edata))
				ಮುಂದುವರಿ;
		}

		ಹಿಂತಿರುಗಿ cidx;
	}

	ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
}

ಪೂರ್ಣಾಂಕ sbi_pmu_ctr_cfg_ಹೊಂದಿಕೆ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cidx_ತಳ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cidx_ಮುಖವಾಡ,
			  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ event_idx,
			  uint64_t event_ದತ್ತಾಂಶ)
{
	ರಚನೆ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ಆದರೆ (unlikely(!phs))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಪೂರ್ಣಾಂಕ ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_ಸಂಕೇತ;

	ಆದರೆ (!pmu_ctr_idx_validate(cidx_ತಳ, cidx_ಮುಖವಾಡ))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_ದತ್ತಾಂಶ);
	ಆದರೆ (event_type < 0)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	event_ಸಂಕೇತ = get_cidx_ಸಂಕೇತ(event_idx);

	ಆದರೆ (flags & SBI_PMU_CFG_FLAG_SKIP_ಹೊಂದಿಕೆ) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cidx_ಮೊದಲ = cidx_ತಳ + sbi_ffs(cidx_ಮುಖವಾಡ);

		ಆದರೆ (cidx_ಮೊದಲ >= ಒಟ್ಟು_ctrs)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;

		ಆದರೆ (phs->ಸಕ್ರಿಯ_events[cidx_ಮೊದಲ] == SBI_PMU_EVENT_IDX_INVALID)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		ctr_idx = cidx_ಮೊದಲ;
		ಹೋಗು skip_ಹೊಂದಿಕೆ;
	}

	ಆದರೆ (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_ಕಂಡುಹಿಡಿ_fw(phs, cidx_ತಳ, cidx_ಮುಖವಾಡ,
					  event_ಸಂಕೇತ, event_ದತ್ತಾಂಶ);
		ಆದರೆ ((event_ಸಂಕೇತ == SBI_PMU_FW_PLATFORM) && (ctr_idx >= ಅಂಕೆ_hw_ctrs))
			phs->fw_counters_ದತ್ತಾಂಶ[ctr_idx - ಅಂಕೆ_hw_ctrs] =
								event_ದತ್ತಾಂಶ;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ctr_idx = pmu_ctr_ಕಂಡುಹಿಡಿ_hw(phs, cidx_ತಳ, cidx_ಮುಖವಾಡ, flags,
					  event_idx, event_ದತ್ತಾಂಶ);
		ಆದರೆ (ctr_idx >= 0) {
			ರಚನೆ sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_ದತ್ತಾಂಶ = event_ದತ್ತಾಂಶ;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_ಮುಖವಾಡ;
		}
	}

	ಆದರೆ (ctr_idx < 0)
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	phs->ಸಕ್ರಿಯ_events[ctr_idx] = event_idx;
skip_ಹೊಂದಿಕೆ:
	ಆದರೆ (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		ಆದರೆ (flags & SBI_PMU_CFG_FLAG_CLEAR_ಮೌಲ್ಯ)
			pmu_ctr_write_hw(ctr_idx, 0);
		ಆದರೆ (flags & SBI_PMU_CFG_FLAG_AUTO_ಆರಂಭ)
			pmu_ctr_ಆರಂಭ_hw(ctr_idx, 0, false);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (event_type == SBI_PMU_EVENT_TYPE_FW) {
		ಆದರೆ (flags & SBI_PMU_CFG_FLAG_CLEAR_ಮೌಲ್ಯ)
			phs->fw_counters_ದತ್ತಾಂಶ[ctr_idx - ಅಂಕೆ_hw_ctrs] = 0;
		ಆದರೆ (flags & SBI_PMU_CFG_FLAG_AUTO_ಆರಂಭ) {
			ಆದರೆ (SBI_PMU_FW_PLATFORM == event_ಸಂಕೇತ &&
			    pmu_dev && pmu_dev->fw_counter_ಆರಂಭ) {
				ret = pmu_dev->fw_counter_ಆರಂಭ(
					phs->hartid,
					ctr_idx - ಅಂಕೆ_hw_ctrs, event_ದತ್ತಾಂಶ);
				ಆದರೆ (ret)
					ಹಿಂತಿರುಗಿ ret;
			}
			phs->fw_counters_started |= ಬಿಟ್(ctr_idx - ಅಂಕೆ_hw_ctrs);
		}
	}

	ಹಿಂತಿರುಗಿ ctr_idx;
}

ಪೂರ್ಣಾಂಕ sbi_pmu_ctr_incr_fw(ಎಣಿಕೆ sbi_pmu_fw_event_ಸಂಕೇತ_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = ಖಾಲಿ;
	ರಚನೆ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ಆದರೆ (unlikely(!phs))
		ಹಿಂತಿರುಗಿ 0;

	ಆದರೆ (likely(!phs->fw_counters_started))
		ಹಿಂತಿರುಗಿ 0;

	ಆದರೆ (unlikely(fw_id >= SBI_PMU_FW_MAX))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಗಾಗಿ (cidx = ಅಂಕೆ_hw_ctrs; cidx < ಒಟ್ಟು_ctrs; cidx++) {
		ಆದರೆ (get_cidx_ಸಂಕೇತ(phs->ಸಕ್ರಿಯ_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & ಬಿಟ್(cidx - ಅಂಕೆ_hw_ctrs))) {
			fcounter = &phs->fw_counters_ದತ್ತಾಂಶ[cidx - ಅಂಕೆ_hw_ctrs];
			ನಿಲ್ಲಿಸು;
		}
	}

	ಆದರೆ (fcounter)
		(*fcounter)++;

	ಹಿಂತಿರುಗಿ 0;
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_pmu_ಅಂಕೆ_ctr(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ (ಅಂಕೆ_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

ಪೂರ್ಣಾಂಕ sbi_pmu_ctr_get_info(uint32_t cidx, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ctr_info)
{
	ಪೂರ್ಣಾಂಕ ಅಗಲ;
	ಒಕ್ಕೂಟ sbi_pmu_ctr_info cinfo = {0};
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ counter_ಮುಖವಾಡ = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)sbi_hart_mhpm_ಮುಖವಾಡ(scratch) |
				     SBI_PMU_CY_IR_ಮುಖವಾಡ;

	/* Sanity check */
	ಆದರೆ (cidx >= ಒಟ್ಟು_ctrs)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	ಆದರೆ (cidx < ಅಂಕೆ_hw_ctrs) {
		ಆದರೆ (!(__test_bit(cidx, &counter_ಮುಖವಾಡ)))
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_ಚಕ್ರ + cidx;
		/* mcycle & minstret are always 64 bit */
		ಆದರೆ (cidx == 0 || cidx == 2)
			cinfo.ಅಗಲ = 63;
		ಇಲ್ಲದಿದ್ದರೆ
			cinfo.ಅಗಲ = sbi_hart_mhpm_ಬಿಟ್ಟುಗಳು(scratch) - 1;
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.ಅಗಲ = 63;
		ಆದರೆ (pmu_dev && pmu_dev->fw_counter_ಅಗಲ) {
			ಅಗಲ = pmu_dev->fw_counter_ಅಗಲ();
			ಆದರೆ (ಅಗಲ)
				cinfo.ಅಗಲ = ಅಗಲ - 1;
		}
	}

	*ctr_info = cinfo.ಮೌಲ್ಯ;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_pmu_event_get_info(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ shmem_phys_lo, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ shmem_phys_hi,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಅಂಕೆ_events, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ shmem_ಗಾತ್ರ = ಅಂಕೆ_events * ಗಾತ್ರ(ರಚನೆ sbi_pmu_event_info);
	ಪೂರ್ಣಾಂಕ i, j, event_type;
	ರಚನೆ sbi_pmu_event_info *einfo;
	ರಚನೆ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	ರಚನೆ sbi_pmu_hw_event *ತಾತ್ಕಾಲಿಕ;
	bool ಸಿಕ್ಕಿತು = false;

	ಆದರೆ (flags != 0)
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	ಆದರೆ (!ಅಂಕೆ_events || (shmem_phys_lo & 0xF))
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	ಆದರೆ (shmem_phys_hi)
		ಹಿಂತಿರುಗಿ SBI_EINVALID_ADDR;

	ಆದರೆ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_ಗಾತ್ರ, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_ವಿಳಾಸ;

	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range(shmem_phys_lo, shmem_ಗಾತ್ರ);

	einfo = (ರಚನೆ sbi_pmu_event_info *)(shmem_phys_lo);
	ಗಾಗಿ (i = 0; i < ಅಂಕೆ_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		ಆದರೆ (event_idx & SBI_PMU_EVENT_IDX_MBZ_ಮುಖವಾಡ)
			ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_ದತ್ತಾಂಶ);
		ಆದರೆ (event_type < 0) {
			einfo[i].output = 0;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಗಾಗಿ (j = 0; j < ಅಂಕೆ_hw_events; j++) {
				ತಾತ್ಕಾಲಿಕ = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				ಆದರೆ (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					ಆದರೆ (ತಾತ್ಕಾಲಿಕ->ಆರಂಭ_idx > event_idx ||
					    event_idx > ತಾತ್ಕಾಲಿಕ->ಕೊನೆ_idx)
						ಮುಂದುವರಿ;
					/* just match the selector */
					ಆದರೆ (ತಾತ್ಕಾಲಿಕ->select == (einfo[i].event_ದತ್ತಾಂಶ &
									ತಾತ್ಕಾಲಿಕ->select_ಮುಖವಾಡ)) {
						ಸಿಕ್ಕಿತು = true;
						ನಿಲ್ಲಿಸು;
					}
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ತಾತ್ಕಾಲಿಕ->ಆರಂಭ_idx <= event_idx &&
					   event_idx <= ತಾತ್ಕಾಲಿಕ->ಕೊನೆ_idx) {
					ಸಿಕ್ಕಿತು = true;
					ನಿಲ್ಲಿಸು;
				}
			}
			ಆದರೆ (ಸಿಕ್ಕಿತು)
				einfo[i].output = 1;
			ಇಲ್ಲದಿದ್ದರೆ
				einfo[i].output = 0;
			ಸಿಕ್ಕಿತು = false;
		}
	}

	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range(shmem_phys_lo, shmem_ಗಾತ್ರ);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ pmu_reset_event_map(ರಚನೆ sbi_pmu_hart_state *phs)
{
	ಪೂರ್ಣಾಂಕ j;

	/* Initialize the counter to event mapping table */
	ಗಾಗಿ (j = 3; j < ಒಟ್ಟು_ctrs; j++)
		phs->ಸಕ್ರಿಯ_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	ಗಾಗಿ (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_ದತ್ತಾಂಶ[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_pmu_device *sbi_pmu_get_device(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ pmu_dev;
}

ಶೂನ್ಯ sbi_pmu_ಗಣ_device(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_pmu_device *dev)
{
	ಆದರೆ (!dev || pmu_dev)
		ಹಿಂತಿರುಗಿ;

	pmu_dev = dev;
}

ಶೂನ್ಯ sbi_pmu_exit(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	ಆದರೆ (unlikely(!phs))
		ಹಿಂತಿರುಗಿ;

	pmu_reset_event_map(phs);
}

ಸ್ಥಿರ ಶೂನ್ಯ pmu_sse_enable(uint32_t event_id)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ irq_ಮುಖವಾಡ = sbi_pmu_irq_ಮುಖವಾಡ();

	csr_ಗಣ(CSR_MIE, irq_ಮುಖವಾಡ);
}

ಸ್ಥಿರ ಶೂನ್ಯ pmu_sse_disable(uint32_t event_id)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ irq_ಮುಖವಾಡ = sbi_pmu_irq_ಮುಖವಾಡ();

	csr_clear(CSR_MIE, irq_ಮುಖವಾಡ);
	csr_clear(CSR_MIP, irq_ಮುಖವಾಡ);
}

ಸ್ಥಿರ ಶೂನ್ಯ pmu_sse_complete(uint32_t event_id)
{
	csr_ಗಣ(CSR_MIE, sbi_pmu_irq_ಮುಖವಾಡ());
}

ಸ್ಥಿರ ಶೂನ್ಯ pmu_sse_ರಿಜಿಸ್ಟರ್(uint32_t event_id)
{
	ರಚನೆ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_ಮುಖವಾಡ());
}

ಸ್ಥಿರ ಶೂನ್ಯ pmu_sse_unregister(uint32_t event_id)
{
	ರಚನೆ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_ಗಣ(CSR_MIDELEG, sbi_pmu_irq_ಮುಖವಾಡ());
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_sse_cb_ops pmu_sse_cb_ops = {
	.ರಿಜಿಸ್ಟರ್_cb = pmu_sse_ರಿಜಿಸ್ಟರ್,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

ಪೂರ್ಣಾಂಕ sbi_pmu_init(ರಚನೆ sbi_scratch *scratch, bool cold_boot)
{
	ಪೂರ್ಣಾಂಕ hpm_ಎಣಿಕೆ = sbi_fls(sbi_hart_mhpm_ಮುಖವಾಡ(scratch));
	ರಚನೆ sbi_pmu_hart_state *phs;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (cold_boot) {
		hw_event_map = sbi_calloc(ಗಾತ್ರ(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		ಆದರೆ (!hw_event_map)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;

		phs_ptr_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_type_ಸ್ಥಾನಾಂತರ(ಶೂನ್ಯ *);
		ಆದರೆ (!phs_ptr_ಸ್ಥಾನಾಂತರ) {
			sbi_free(hw_event_map);
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		ಆದರೆ (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		ಆದರೆ (!hpm_ಎಣಿಕೆ)
			/* Only CY, TM & IR are implemented in the hw */
			ಅಂಕೆ_hw_ctrs = 3;
		ಇಲ್ಲದಿದ್ದರೆ
			ಅಂಕೆ_hw_ctrs = hpm_ಎಣಿಕೆ + 1;

		ಆದರೆ (ಅಂಕೆ_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;

		ಒಟ್ಟು_ctrs = ಅಂಕೆ_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		ಆದರೆ (sbi_pmu_irq_ಬಿಟ್() >= 0)
			sbi_sse_ಕೂಡಿಸು_event(SBI_SSE_EVENT_LOCAL_PMU_ಅತಿಪ್ರವಾಹ, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	ಆದರೆ (!phs) {
		phs = sbi_zalloc(ಗಾತ್ರ(*phs));
		ಆದರೆ (!phs)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
		phs->hartid = current_hartid();
		pmu_ಗಣ_hart_state_ptr(scratch, phs);
	}

	pmu_reset_event_map(phs);

	/* First three counters are fixed by the priv spec and we enable it by default */
	phs->ಸಕ್ರಿಯ_events[0] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_ಸ್ಥಾನಾಂತರ) |
				SBI_PMU_HW_CPU_ಚಕ್ರಗಳು;
	phs->ಸಕ್ರಿಯ_events[1] = SBI_PMU_EVENT_IDX_INVALID;
	phs->ಸಕ್ರಿಯ_events[2] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_ಸ್ಥಾನಾಂತರ) |
				SBI_PMU_HW_INSTRUCTIONS;

	ಹಿಂತಿರುಗಿ 0;
}
