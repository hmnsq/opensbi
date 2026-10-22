/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart_protection.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_sse.h>

/** Information about hardware counters */
ਬਣਤਰ sbi_pmu_hw_event {
	uint32_t counters;
	uint32_t ਸ਼ੁਰੂ_idx;
	uint32_t ਸਿਰਾ_idx;
	/* Event selector value used only for raw events. The event select value
	 * can be a even id or a selector value for set of events encoded in few
	 * bits. In case latter, the bits used for encoding of the events should
	 * be zeroed out in the select value.
	 */
	uint64_t select;
	 /**
	  * The select_mask indicates which bits are encoded for the event(s).
	  */
	uint64_t select_ਨਕਾਬ;
};

/* Information about PMU counters as per SBI specification */
ਸੰਘ sbi_pmu_ctr_info {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ;
	ਬਣਤਰ {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ csr:12;
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਚੌੜਾਈ:6;
#ਜੇ __riscv_xlen == 32
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reserved:13;
#ਨਹੀਂਤਾਂ
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reserved:45;
#ਜੇ_ਅੰਤ
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ type:1;
	};
};

#ਜੇ SBI_PMU_FW_CTR_MAX >= ਬਿੱਟ_ਪ੍ਰਤੀ_LONG
#ਗਲਤੀ "Can't handle firmware counters beyond BITS_PER_LONG"
#ਜੇ_ਅੰਤ

/** HW event configuration parameters */
ਬਣਤਰ sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_ਅੰਕੜੇ;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
ਬਣਤਰ sbi_pmu_hart_state {
	/* HART to which this state belongs */
	uint32_t hartid;
	/* Counter to enabled event mapping */
	uint32_t ਸਰਗਰਮ_events[SBI_PMU_HW_CTR_MAX + SBI_PMU_FW_CTR_MAX];
	/* Bitmap of firmware counters started */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fw_counters_started;
	/* if true, SSE is enabled */
	bool sse_enabled;
	/*
	 * Counter values for SBI firmware events and event codes
	 * for platform firmware events. Both are mutually exclusive
	 * and hence can optimally share the same memory.
	 */
	uint64_t fw_counters_ਅੰਕੜੇ[SBI_PMU_FW_CTR_MAX];
	/* HW events configuration parameters from
	 * sbi_pmu_ctr_cfg_match() command which are
	 * used for restoring RAW hardware events after
	 * cpu suspending.
	 */
	ਬਣਤਰ sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ phs_ptr_ਵਿੱਥ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_ਵਿੱਥ ? sbi_scratch_read_type((__scratch), ਖਾਲੀ *, phs_ptr_ਵਿੱਥ) : ਨਿਰਾ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ pmu_ਸੈੱਟ_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), ਖਾਲੀ *, phs_ptr_ਵਿੱਥ, (__phs))

/* Platform specific PMU device */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_pmu_device *pmu_dev = ਨਿਰਾ;

/* Mapping between event range and possible counters  */
ਸਥਿਰ ਬਣਤਰ sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
ਸਥਿਰ uint32_t ਅੰਕ_hw_events;
/* Maximum number of hardware counters available */
ਸਥਿਰ uint32_t ਅੰਕ_hw_ctrs;

/* Maximum number of counters available */
ਸਥਿਰ uint32_t ਕੁੱਲ_ctrs;

/* Helper macros to retrieve event idx and code type */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_cidx_type(x) \
  (((x) & SBI_PMU_EVENT_IDX_TYPE_ਨਕਾਬ) >> SBI_PMU_EVENT_IDX_TYPE_ਵਿੱਥ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_cidx_ਕੋਡ(x) (x & SBI_PMU_EVENT_IDX_ਕੋਡ_ਨਕਾਬ)

/**
 * Perform a sanity check on event & counter mappings with event range overlap check
 * @param evtA Pointer to the existing hw event structure
 * @param evtB Pointer to the new hw event structure
 *
 * Return false if the range doesn't overlap, true otherwise
 */
ਸਥਿਰ bool pmu_event_range_overlap(ਬਣਤਰ sbi_pmu_hw_event *evtA,
				    ਬਣਤਰ sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	ਜੇ (((evtA->ਸਿਰਾ_idx < evtB->ਸ਼ੁਰੂ_idx) && (evtA->ਸਿਰਾ_idx < evtB->ਸਿਰਾ_idx)) ||
	   ((evtA->ਸ਼ੁਰੂ_idx > evtB->ਸ਼ੁਰੂ_idx) && (evtA->ਸ਼ੁਰੂ_idx > evtB->ਸਿਰਾ_idx)))
		ਵਾਪਸ false;
	ਵਾਪਸ true;
}

ਸਥਿਰ bool pmu_event_select_overlap(ਬਣਤਰ sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_ਨਕਾਬ)
{
	ਜੇ ((evt->select == select_val) && (evt->select_ਨਕਾਬ == select_ਨਕਾਬ))
		ਵਾਪਸ true;

	ਵਾਪਸ false;
}

ਸਥਿਰ ਪੂਰਨ pmu_event_validate(ਬਣਤਰ sbi_pmu_hart_state *phs,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_ਕੋਡ = get_cidx_ਕੋਡ(event_idx);
	uint32_t event_idx_ਕੋਡ_max = -1;
	uint32_t cache_ops_ਨਤੀਜਾ, cache_ops_id, cache_id;

	ਚੋਣ(event_idx_type) {
	ਹਾਲਤ SBI_PMU_EVENT_TYPE_HW:
		event_idx_ਕੋਡ_max = SBI_PMU_HW_GENERAL_MAX;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_PMU_EVENT_TYPE_FW:
		ਜੇ ((event_idx_ਕੋਡ >= SBI_PMU_FW_MAX &&
		    event_idx_ਕੋਡ <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_ਕੋਡ > SBI_PMU_FW_PLATFORM)
			ਵਾਪਸ SBI_EINVAL;

		ਜੇ (SBI_PMU_FW_PLATFORM == event_idx_ਕੋਡ &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			ਵਾਪਸ pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		ਨਹੀਂਤਾਂ
			event_idx_ਕੋਡ_max = SBI_PMU_FW_MAX;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_ਨਤੀਜਾ = event_idx_ਕੋਡ &
					SBI_PMU_EVENT_HW_CACHE_OPS_ਨਤੀਜਾ;
		cache_ops_id = (event_idx_ਕੋਡ &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_ਨਕਾਬ) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_ਵਿੱਥ;
		cache_id = (event_idx_ਕੋਡ &
			    SBI_PMU_EVENT_HW_CACHE_ID_ਨਕਾਬ) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_ਵਿੱਥ;
		ਜੇ ((cache_ops_ਨਤੀਜਾ < SBI_PMU_HW_CACHE_ਨਤੀਜਾ_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			ਵਾਪਸ event_idx_type;
		ਨਹੀਂਤਾਂ
			ਵਾਪਸ SBI_EINVAL;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_PMU_EVENT_TYPE_HW_RAW:
	ਹਾਲਤ SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_ਕੋਡ_max = 1; // event_idx.code should be zero
		ਰੋਕੋ;
	ਮੂਲ:
		ਵਾਪਸ SBI_EINVAL;
	}

	ਜੇ (event_idx_ਕੋਡ < event_idx_ਕੋਡ_max)
		ਵਾਪਸ event_idx_type;

	ਵਾਪਸ SBI_EINVAL;
}

ਸਥਿਰ ਪੂਰਨ pmu_ctr_validate(ਬਣਤਰ sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_ਕੋਡ)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	ਜੇ (cidx >= ਕੁੱਲ_ctrs)
		ਵਾਪਸ SBI_EINVAL;

	event_idx_val = phs->ਸਰਗਰਮ_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	ਜੇ (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		ਵਾਪਸ SBI_EINVAL;

	*event_idx_ਕੋਡ = get_cidx_ਕੋਡ(event_idx_val);

	ਵਾਪਸ event_idx_type;
}

ਸਥਿਰ bool pmu_ctr_idx_validate(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cbase, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cmask)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਆਖਰੀ;

	ਜੇ (!cmask)
		ਵਾਪਸ false;

	ਆਖਰੀ = sbi_fls(cmask);
	ਜੇ (cbase > -1UL - ਆਖਰੀ)
		ਵਾਪਸ false;

	ਵਾਪਸ (cbase + ਆਖਰੀ) < ਕੁੱਲ_ctrs;
}

ਪੂਰਨ sbi_pmu_ctr_fw_read(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cidx, uint64_t *cval, bool ਉੱਚਾ_ਬਿੱਟ)
{
	ਪੂਰਨ event_idx_type;
	uint32_t event_ਕੋਡ;
	ਬਣਤਰ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ਜੇ (unlikely(!phs))
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (cidx < ਅੰਕ_hw_ctrs || cidx >= ਕੁੱਲ_ctrs)
		ਵਾਪਸ SBI_EINVAL;

#ਜੇ __riscv_xlen > 32
	ਜੇ (ਉੱਚਾ_ਬਿੱਟ)
		ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_ਕੋਡ);
	ਜੇ (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ ((event_ਕੋਡ >= SBI_PMU_FW_MAX &&
	    event_ਕੋਡ <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_ਕੋਡ > SBI_PMU_FW_PLATFORM)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (SBI_PMU_FW_PLATFORM == event_ਕੋਡ) {
		ਜੇ (pmu_dev && pmu_dev->fw_counter_read_ਮੁੱਲ)
			*cval = pmu_dev->fw_counter_read_ਮੁੱਲ(phs->hartid,
							       cidx -
							       ਅੰਕ_hw_ctrs);
		ਨਹੀਂਤਾਂ
			*cval = 0;
	} ਨਹੀਂਤਾਂ
		*cval = phs->fw_counters_ਅੰਕੜੇ[cidx - ਅੰਕ_hw_ctrs];

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ pmu_ਜੋੜੋ_hw_event_map(u32 eidx_ਸ਼ੁਰੂ, u32 eidx_ਸਿਰਾ, u32 cmap,
				uint64_t select, uint64_t select_ਨਕਾਬ)
{
	ਪੂਰਨ i = 0;
	bool is_overlap;
	ਬਣਤਰ sbi_pmu_hw_event *event = &hw_event_map[ਅੰਕ_hw_events];
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_ਨਕਾਬ = sbi_hart_mhpm_ਨਕਾਬ(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	ਜੇ (eidx_ਸ਼ੁਰੂ > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_ਨਕਾਬ))
		ਵਾਪਸ SBI_EDENIED;

	ਜੇ (ਅੰਕ_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		ਵਾਪਸ SBI_EFAIL;
	}

	event->ਸ਼ੁਰੂ_idx = eidx_ਸ਼ੁਰੂ;
	event->ਸਿਰਾ_idx = eidx_ਸਿਰਾ;

	/* Sanity check */
	ਲਈ (i = 0; i < ਅੰਕ_hw_events; i++) {
		ਜੇ (eidx_ਸ਼ੁਰੂ == SBI_PMU_EVENT_RAW_IDX ||
			eidx_ਸ਼ੁਰੂ == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_ਨਕਾਬ);
		ਨਹੀਂਤਾਂ
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		ਜੇ (is_overlap)
			ਜਾਓ reset_event;
	}

	event->select_ਨਕਾਬ = select_ਨਕਾਬ;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_ਨਕਾਬ;
	event->select = select;
	ਅੰਕ_hw_events++;

	ਵਾਪਸ 0;

reset_event:
	event->ਸ਼ੁਰੂ_idx = 0;
	event->ਸਿਰਾ_idx = 0;
	ਵਾਪਸ SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
ਪੂਰਨ sbi_pmu_ਜੋੜੋ_hw_event_counter_map(u32 eidx_ਸ਼ੁਰੂ, u32 eidx_ਸਿਰਾ, u32 cmap)
{
	ਜੇ ((eidx_ਸ਼ੁਰੂ > eidx_ਸਿਰਾ) || eidx_ਸ਼ੁਰੂ >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_ਸਿਰਾ >= SBI_PMU_EVENT_RAW_IDX)
		ਵਾਪਸ SBI_EINVAL;

	ਵਾਪਸ pmu_ਜੋੜੋ_hw_event_map(eidx_ਸ਼ੁਰੂ, eidx_ਸਿਰਾ, cmap, 0, 0);
}

ਪੂਰਨ sbi_pmu_ਜੋੜੋ_raw_event_counter_map(uint64_t select, uint64_t select_ਨਕਾਬ, u32 cmap)
{
	ਵਾਪਸ pmu_ਜੋੜੋ_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_ਨਕਾਬ);
}

ਖਾਲੀ sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_ਨਕਾਬ());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_ਉਛਾਲ);
}

ਸਥਿਰ ਪੂਰਨ pmu_ctr_enable_irq_hw(ਪੂਰਨ ctr_idx)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mhpmevent_csr;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mhpmevent_curr;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mip_val;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਦਾ_ਨਕਾਬ;

	ਜੇ (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		ਵਾਪਸ SBI_EFAIL;

#ਜੇ __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	ਦਾ_ਨਕਾਬ = (uint32_t)~MHPMEVENTH_ਦਾ;
#ਨਹੀਂਤਾਂ
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	ਦਾ_ਨਕਾਬ = ~MHPMEVENT_ਦਾ;
#ਜੇ_ਅੰਤ

	mhpmevent_curr = csr_read_ਅੰਕ(mhpmevent_csr);
	mip_val = csr_read(CSR_MIP);
	/**
	 * Clear out the OF bit so that next interrupt can be enabled.
	 * This should be done only when the corresponding overflow interrupt
	 * bit is cleared. That indicates that software has already handled the
	 * previous interrupts or the hardware yet to set an overflow interrupt.
	 * Otherwise, there will be race conditions where we may clear the bit
	 * the software is yet to handle the interrupt.
	 */
	ਜੇ (!(mip_val & sbi_pmu_irq_ਨਕਾਬ())) {
		mhpmevent_curr &= ਦਾ_ਨਕਾਬ;
		csr_write_ਅੰਕ(mhpmevent_csr, mhpmevent_curr);
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#ਜੇ __riscv_xlen == 32
	csr_write_ਅੰਕ(CSR_MCYCLE + cidx, 0);
	csr_write_ਅੰਕ(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_ਅੰਕ(CSR_MCYCLEH + cidx, ival >> ਬਿੱਟ_ਪ੍ਰਤੀ_LONG);
#ਨਹੀਂਤਾਂ
	csr_write_ਅੰਕ(CSR_MCYCLE + cidx, ival);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ pmu_ctr_ਸ਼ੁਰੂ_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	ਜੇ (cidx >= ਅੰਕ_hw_ctrs || cidx == 1)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) < SBI_HART_PRIV_VER_1_11) {
		ਜੇ (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		ਵਾਪਸ 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	ਜੇ (!__test_bit(cidx, &mctr_inhbt))
		ਵਾਪਸ SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	ਜੇ (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	ਜੇ (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_pmu_irq_ਦੋਆਧਾਰੀ_ਅੰਕ(ਖਾਲੀ)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		ਵਾਪਸ IRQ_PMU_OVF;
	ਜੇ (pmu_dev && pmu_dev->hw_counter_irq_ਦੋਆਧਾਰੀ_ਅੰਕ)
		ਵਾਪਸ pmu_dev->hw_counter_irq_ਦੋਆਧਾਰੀ_ਅੰਕ();

	ਵਾਪਸ -1;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_pmu_irq_ਨਕਾਬ(ਖਾਲੀ)
{
	ਪੂਰਨ irq_ਦੋਆਧਾਰੀ_ਅੰਕ = sbi_pmu_irq_ਦੋਆਧਾਰੀ_ਅੰਕ();

	ਜੇ (irq_ਦੋਆਧਾਰੀ_ਅੰਕ < 0)
		ਵਾਪਸ 0;

	ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ(irq_ਦੋਆਧਾਰੀ_ਅੰਕ);
}

ਸਥਿਰ ਪੂਰਨ pmu_ctr_ਸ਼ੁਰੂ_fw(ਬਣਤਰ sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_ਕੋਡ,
			    uint64_t event_ਅੰਕੜੇ, uint64_t ival,
			    bool ival_update)
{
	ਪੂਰਨ ret;

	ਜੇ ((event_ਕੋਡ >= SBI_PMU_FW_MAX &&
	    event_ਕੋਡ <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_ਕੋਡ > SBI_PMU_FW_PLATFORM)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (phs->fw_counters_started & ਦੋਆਧਾਰੀ_ਅੰਕ(cidx - ਅੰਕ_hw_ctrs))
		ਵਾਪਸ SBI_EALREADY_STARTED;

	ਜੇ (SBI_PMU_FW_PLATFORM == event_ਕੋਡ) {
		ਜੇ (!pmu_dev ||
		    !pmu_dev->fw_counter_write_ਮੁੱਲ ||
		    !pmu_dev->fw_counter_ਸ਼ੁਰੂ) {
			ਵਾਪਸ SBI_EINVAL;
		}

		ਜੇ (ival_update)
			pmu_dev->fw_counter_write_ਮੁੱਲ(phs->hartid,
							cidx - ਅੰਕ_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_ਸ਼ੁਰੂ(phs->hartid,
						 cidx - ਅੰਕ_hw_ctrs,
						 event_ਅੰਕੜੇ);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	} ਨਹੀਂਤਾਂ {
		ਜੇ (ival_update)
			phs->fw_counters_ਅੰਕੜੇ[cidx - ਅੰਕ_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= ਦੋਆਧਾਰੀ_ਅੰਕ(cidx - ਅੰਕ_hw_ctrs);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ pmu_update_inhibit_flags(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags, uint64_t *mhpmevent_val)
{
	ਜੇ (flags & SBI_PMU_CFG_FLAG_ਸੈੱਟ_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	ਜੇ (flags & SBI_PMU_CFG_FLAG_ਸੈੱਟ_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	ਜੇ (flags & SBI_PMU_CFG_FLAG_ਸੈੱਟ_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	ਜੇ (flags & SBI_PMU_CFG_FLAG_ਸੈੱਟ_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

ਸਥਿਰ ਪੂਰਨ pmu_update_hw_mhpmevent(ਬਣਤਰ sbi_pmu_hw_event *hw_evt, ਪੂਰਨ ctr_idx,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ eindex,
				   uint64_t ਅੰਕੜੇ)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_ਮੰਜ਼ਿਲ_mhpmevent(plat, eindex, ਅੰਕੜੇ);

	ਜੇ (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		ਵਾਪਸ SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_ਨਕਾਬ) |
				 MHPMEVENT_MINH | MHPMEVENT_ਦਾ;

	ਜੇ (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	ਜੇ (pmu_dev && pmu_dev->hw_counter_filter_ਬਹੁਲਕ)
		pmu_dev->hw_counter_filter_ਬਹੁਲਕ(flags, ctr_idx);

#ਜੇ __riscv_xlen == 32
	csr_write_ਅੰਕ(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_ਅੰਕ(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> ਬਿੱਟ_ਪ੍ਰਤੀ_LONG);
#ਨਹੀਂਤਾਂ
	csr_write_ਅੰਕ(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#ਜੇ_ਅੰਤ

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_pmu_ctr_ਸ਼ੁਰੂ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cbase, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cmask,
		      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags, uint64_t ival)
{
	ਬਣਤਰ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ਜੇ (unlikely(!phs))
		ਵਾਪਸ SBI_EINVAL;

	ਪੂਰਨ event_idx_type;
	uint32_t event_ਕੋਡ;
	ਪੂਰਨ ret = SBI_EINVAL;
	bool bUpdate = false;
	ਪੂਰਨ i, cidx;
	uint64_t edata;

	ਜੇ (!pmu_ctr_idx_validate(cbase, cmask))
		ਵਾਪਸ ret;

	ਜੇ (flags & ~SBI_PMU_ਸ਼ੁਰੂ_FLAGS_ਨਕਾਬ)
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	ਜੇ (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		ਵਾਪਸ SBI_ENO_SHMEM;

	ਜੇ (flags & SBI_PMU_ਸ਼ੁਰੂ_FLAG_ਸੈੱਟ_INIT_ਮੁੱਲ)
		bUpdate = true;

	for_each_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(i, &cmask, ਬਿੱਟ_ਪ੍ਰਤੀ_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_ਕੋਡ);
		ਜੇ (event_idx_type < 0)
			/* Continue the start operation for other counters */
			ਜਾਰੀ;
		ਨਹੀਂਤਾਂ ਜੇ (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_ਕੋਡ == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_ਅੰਕੜੇ[cidx - ਅੰਕ_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_ਸ਼ੁਰੂ_fw(phs, cidx, event_ਕੋਡ, edata,
					       ival, bUpdate);
			ਜੇ (ret)
				ਵਾਪਸ ret;
		} ਨਹੀਂਤਾਂ {
			ਜੇ (cidx >= 3) {
				ਬਣਤਰ sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->ਸਰਗਰਮ_events[cidx],
							ev_cfg->event_ਅੰਕੜੇ);
				ਜੇ (ret)
					ਵਾਪਸ ret;
			}
			ret = pmu_ctr_ਸ਼ੁਰੂ_hw(cidx, ival, bUpdate);
			ਜੇ (ret)
				ਵਾਪਸ ret;
		}
	}

	ਵਾਪਸ ret;
}

ਸਥਿਰ ਪੂਰਨ pmu_ctr_stop_hw(uint32_t cidx)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mctr_inhbt;

	ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) < SBI_HART_PRIV_VER_1_11)
		ਵਾਪਸ 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	ਜੇ (cidx >= ਅੰਕ_hw_ctrs || cidx == 1)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		ਜੇ (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		ਵਾਪਸ 0;
	} ਨਹੀਂਤਾਂ
		ਵਾਪਸ SBI_EALREADY_STOPPED;
}

ਸਥਿਰ ਪੂਰਨ pmu_ctr_stop_fw(ਬਣਤਰ sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_ਕੋਡ)
{
	ਪੂਰਨ ret;

	ਜੇ ((event_ਕੋਡ >= SBI_PMU_FW_MAX &&
	    event_ਕੋਡ <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_ਕੋਡ > SBI_PMU_FW_PLATFORM)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (!(phs->fw_counters_started & ਦੋਆਧਾਰੀ_ਅੰਕ(cidx - ਅੰਕ_hw_ctrs)))
		ਵਾਪਸ SBI_EALREADY_STOPPED;

	ਜੇ (SBI_PMU_FW_PLATFORM == event_ਕੋਡ &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - ਅੰਕ_hw_ctrs);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	phs->fw_counters_started &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(cidx - ਅੰਕ_hw_ctrs);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ pmu_reset_hw_mhpmevent(ਪੂਰਨ ctr_idx)
{
	ਜੇ (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		ਵਾਪਸ SBI_EFAIL;
#ਜੇ __riscv_xlen == 32
	csr_write_ਅੰਕ(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	ਜੇ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_ਅੰਕ(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#ਨਹੀਂਤਾਂ
	csr_write_ਅੰਕ(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#ਜੇ_ਅੰਤ

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_pmu_ctr_stop(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cbase, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cmask,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flag)
{
	ਬਣਤਰ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ਜੇ (unlikely(!phs))
		ਵਾਪਸ SBI_EINVAL;

	ਪੂਰਨ ret = SBI_EINVAL;
	ਪੂਰਨ event_idx_type;
	uint32_t event_ਕੋਡ;
	ਪੂਰਨ i, cidx;

	ਜੇ (!pmu_ctr_idx_validate(cbase, cmask))
		ਵਾਪਸ ret;

	ਜੇ (flag & ~SBI_PMU_STOP_FLAGS_ਨਕਾਬ)
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	ਜੇ (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		ਵਾਪਸ SBI_ENO_SHMEM;

	for_each_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(i, &cmask, ਬਿੱਟ_ਪ੍ਰਤੀ_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_ਕੋਡ);
		ਜੇ (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			ਜਾਰੀ;

		ਨਹੀਂਤਾਂ ਜੇ (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_ਕੋਡ);
		ਨਹੀਂਤਾਂ
			ret = pmu_ctr_stop_hw(cidx);

		ਜੇ(ret)
			ਵਾਪਸ ret;

		ਜੇ (cidx > (CSR_INSTRET - CSR_ਚੱਕਰ) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->ਸਰਗਰਮ_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	ਜੇ (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_ਨਕਾਬ());

	ਵਾਪਸ ret;
}

ਸਥਿਰ ਪੂਰਨ pmu_fixed_ctr_update_inhibit_ਬਿੱਟ(ਪੂਰਨ fixed_ctr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#ਜੇ __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#ਜੇ_ਅੰਤ
	ਜੇ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_ਬਹੁਲਕ))
		ਵਾਪਸ fixed_ctr;

	ਚੋਣ (fixed_ctr) {
	ਹਾਲਤ 0:
		cfg_csr_no = CSR_MCYCLECFG;
#ਜੇ __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	ਹਾਲਤ 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#ਜੇ __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	ਮੂਲ:
		ਵਾਪਸ SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#ਜੇ __riscv_xlen == 32
		csr_write_ਅੰਕ(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_ਅੰਕ(cfgh_csr_no, cfg_val >> ਬਿੱਟ_ਪ੍ਰਤੀ_LONG);
#ਨਹੀਂਤਾਂ
		csr_write_ਅੰਕ(cfg_csr_no, cfg_val);
#ਜੇ_ਅੰਤ
	}
	ਜੇ (pmu_dev && pmu_dev->hw_counter_filter_ਬਹੁਲਕ)
		pmu_dev->hw_counter_filter_ਬਹੁਲਕ(flags, fixed_ctr);
	ਵਾਪਸ fixed_ctr;
}

ਸਥਿਰ ਪੂਰਨ pmu_ctr_ਲੱਭੋ_fixed_hw(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ evt_idx_ਕੋਡ)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	ਜੇ (evt_idx_ਕੋਡ == SBI_PMU_HW_CPU_ਚੱਕਰਾਂ)
		ਵਾਪਸ 0;
	ਨਹੀਂਤਾਂ ਜੇ (evt_idx_ਕੋਡ == SBI_PMU_HW_INSTRUCTIONS)
		ਵਾਪਸ 2;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ SBI_EINVAL;
}

ਸਥਿਰ ਪੂਰਨ pmu_ctr_ਲੱਭੋ_hw(ਬਣਤਰ sbi_pmu_hart_state *phs,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cbase, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cmask,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ event_idx, uint64_t ਅੰਕੜੇ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ctr_ਨਕਾਬ;
	ਪੂਰਨ i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	ਬਣਤਰ sbi_pmu_hw_event *ਆਰਜ਼ੀ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mctr_inhbt = 0;
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ਜੇ (cbase >= ਅੰਕ_hw_ctrs)
		ਵਾਪਸ SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_ਲੱਭੋ_fixed_hw(event_idx);
	ਜੇ (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		ਵਾਪਸ pmu_fixed_ctr_update_inhibit_ਬਿੱਟ(fixed_ctr, flags);

	ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	ਲਈ (i = 0; i < ਅੰਕ_hw_events; i++) {
		ਆਰਜ਼ੀ = &hw_event_map[i];
		ਜੇ ((ਆਰਜ਼ੀ->ਸ਼ੁਰੂ_idx > event_idx && event_idx < ਆਰਜ਼ੀ->ਸਿਰਾ_idx) ||
		    (ਆਰਜ਼ੀ->ਸ਼ੁਰੂ_idx < event_idx && event_idx > ਆਰਜ਼ੀ->ਸਿਰਾ_idx))
			ਜਾਰੀ;

		/* For raw events, event data is used as the select value */
		ਜੇ (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_ਨਕਾਬ = ਆਰਜ਼ੀ->select_ਨਕਾਬ;

			/* The non-event map bits of data should match the selector */
			ਜੇ (ਆਰਜ਼ੀ->select != (ਅੰਕੜੇ & select_ਨਕਾਬ))
				ਜਾਰੀ;
		}
		/* Fixed counters should not be part of the search */
		ctr_ਨਕਾਬ = ਆਰਜ਼ੀ->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_ਨਕਾਬ);
		for_each_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_from(cbase, &ctr_ਨਕਾਬ, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			ਜੇ (phs->ਸਰਗਰਮ_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				ਜਾਰੀ;
			/* If mcountinhibit is supported, the bit must be enabled */
			ਜੇ ((sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				ਜਾਰੀ;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			ਰੋਕੋ;
		}
	}

	ਜੇ (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		ਜੇ (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			ਵਾਪਸ SBI_EFAIL;

		ਵਾਪਸ pmu_fixed_ctr_update_inhibit_ਬਿੱਟ(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(ਆਰਜ਼ੀ, ctr_idx, flags, event_idx, ਅੰਕੜੇ);

	ਜੇ (!ret)
		ret = ctr_idx;

	ਵਾਪਸ ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
ਸਥਿਰ ਪੂਰਨ pmu_ctr_ਲੱਭੋ_fw(ਬਣਤਰ sbi_pmu_hart_state *phs,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cbase, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cmask,
			   uint32_t event_ਕੋਡ, uint64_t edata)
{
	ਪੂਰਨ i, cidx;

	for_each_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(i, &cmask, ਬਿੱਟ_ਪ੍ਰਤੀ_LONG) {
		cidx = i + cbase;
		ਜੇ (cidx < ਅੰਕ_hw_ctrs || ਕੁੱਲ_ctrs <= cidx)
			ਜਾਰੀ;
		ਜੇ (phs->ਸਰਗਰਮ_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			ਜਾਰੀ;
		ਜੇ (SBI_PMU_FW_PLATFORM == event_ਕੋਡ &&
		    pmu_dev && pmu_dev->fw_counter_ਮੇਲ_encoding) {
			ਜੇ (!pmu_dev->fw_counter_ਮੇਲ_encoding(phs->hartid,
							    cidx - ਅੰਕ_hw_ctrs,
							    edata))
				ਜਾਰੀ;
		}

		ਵਾਪਸ cidx;
	}

	ਵਾਪਸ SBI_ENOTSUPP;
}

ਪੂਰਨ sbi_pmu_ctr_cfg_ਮੇਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cidx_ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cidx_ਨਕਾਬ,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ event_idx,
			  uint64_t event_ਅੰਕੜੇ)
{
	ਬਣਤਰ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ਜੇ (unlikely(!phs))
		ਵਾਪਸ SBI_EINVAL;

	ਪੂਰਨ ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_ਕੋਡ;

	ਜੇ (!pmu_ctr_idx_validate(cidx_ਅਧਾਰ, cidx_ਨਕਾਬ))
		ਵਾਪਸ SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_ਅੰਕੜੇ);
	ਜੇ (event_type < 0)
		ਵਾਪਸ SBI_EINVAL;
	event_ਕੋਡ = get_cidx_ਕੋਡ(event_idx);

	ਜੇ (flags & SBI_PMU_CFG_FLAG_SKIP_ਮੇਲ) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cidx_ਪਹਿਲਾ = cidx_ਅਧਾਰ + sbi_ffs(cidx_ਨਕਾਬ);

		ਜੇ (cidx_ਪਹਿਲਾ >= ਕੁੱਲ_ctrs)
			ਵਾਪਸ SBI_EINVAL;

		ਜੇ (phs->ਸਰਗਰਮ_events[cidx_ਪਹਿਲਾ] == SBI_PMU_EVENT_IDX_INVALID)
			ਵਾਪਸ SBI_EINVAL;
		ctr_idx = cidx_ਪਹਿਲਾ;
		ਜਾਓ skip_ਮੇਲ;
	}

	ਜੇ (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_ਲੱਭੋ_fw(phs, cidx_ਅਧਾਰ, cidx_ਨਕਾਬ,
					  event_ਕੋਡ, event_ਅੰਕੜੇ);
		ਜੇ ((event_ਕੋਡ == SBI_PMU_FW_PLATFORM) && (ctr_idx >= ਅੰਕ_hw_ctrs))
			phs->fw_counters_ਅੰਕੜੇ[ctr_idx - ਅੰਕ_hw_ctrs] =
								event_ਅੰਕੜੇ;
	} ਨਹੀਂਤਾਂ {
		ctr_idx = pmu_ctr_ਲੱਭੋ_hw(phs, cidx_ਅਧਾਰ, cidx_ਨਕਾਬ, flags,
					  event_idx, event_ਅੰਕੜੇ);
		ਜੇ (ctr_idx >= 0) {
			ਬਣਤਰ sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_ਅੰਕੜੇ = event_ਅੰਕੜੇ;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_ਨਕਾਬ;
		}
	}

	ਜੇ (ctr_idx < 0)
		ਵਾਪਸ SBI_ENOTSUPP;

	phs->ਸਰਗਰਮ_events[ctr_idx] = event_idx;
skip_ਮੇਲ:
	ਜੇ (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		ਜੇ (flags & SBI_PMU_CFG_FLAG_CLEAR_ਮੁੱਲ)
			pmu_ctr_write_hw(ctr_idx, 0);
		ਜੇ (flags & SBI_PMU_CFG_FLAG_AUTO_ਸ਼ੁਰੂ)
			pmu_ctr_ਸ਼ੁਰੂ_hw(ctr_idx, 0, false);
	} ਨਹੀਂਤਾਂ ਜੇ (event_type == SBI_PMU_EVENT_TYPE_FW) {
		ਜੇ (flags & SBI_PMU_CFG_FLAG_CLEAR_ਮੁੱਲ)
			phs->fw_counters_ਅੰਕੜੇ[ctr_idx - ਅੰਕ_hw_ctrs] = 0;
		ਜੇ (flags & SBI_PMU_CFG_FLAG_AUTO_ਸ਼ੁਰੂ) {
			ਜੇ (SBI_PMU_FW_PLATFORM == event_ਕੋਡ &&
			    pmu_dev && pmu_dev->fw_counter_ਸ਼ੁਰੂ) {
				ret = pmu_dev->fw_counter_ਸ਼ੁਰੂ(
					phs->hartid,
					ctr_idx - ਅੰਕ_hw_ctrs, event_ਅੰਕੜੇ);
				ਜੇ (ret)
					ਵਾਪਸ ret;
			}
			phs->fw_counters_started |= ਦੋਆਧਾਰੀ_ਅੰਕ(ctr_idx - ਅੰਕ_hw_ctrs);
		}
	}

	ਵਾਪਸ ctr_idx;
}

ਪੂਰਨ sbi_pmu_ctr_incr_fw(ਗਿਣਤੀ sbi_pmu_fw_event_ਕੋਡ_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = ਨਿਰਾ;
	ਬਣਤਰ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ਜੇ (unlikely(!phs))
		ਵਾਪਸ 0;

	ਜੇ (likely(!phs->fw_counters_started))
		ਵਾਪਸ 0;

	ਜੇ (unlikely(fw_id >= SBI_PMU_FW_MAX))
		ਵਾਪਸ SBI_EINVAL;

	ਲਈ (cidx = ਅੰਕ_hw_ctrs; cidx < ਕੁੱਲ_ctrs; cidx++) {
		ਜੇ (get_cidx_ਕੋਡ(phs->ਸਰਗਰਮ_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & ਦੋਆਧਾਰੀ_ਅੰਕ(cidx - ਅੰਕ_hw_ctrs))) {
			fcounter = &phs->fw_counters_ਅੰਕੜੇ[cidx - ਅੰਕ_hw_ctrs];
			ਰੋਕੋ;
		}
	}

	ਜੇ (fcounter)
		(*fcounter)++;

	ਵਾਪਸ 0;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_pmu_ਅੰਕ_ctr(ਖਾਲੀ)
{
	ਵਾਪਸ (ਅੰਕ_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

ਪੂਰਨ sbi_pmu_ctr_get_info(uint32_t cidx, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ctr_info)
{
	ਪੂਰਨ ਚੌੜਾਈ;
	ਸੰਘ sbi_pmu_ctr_info cinfo = {0};
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ counter_ਨਕਾਬ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)sbi_hart_mhpm_ਨਕਾਬ(scratch) |
				     SBI_PMU_CY_IR_ਨਕਾਬ;

	/* Sanity check */
	ਜੇ (cidx >= ਕੁੱਲ_ctrs)
		ਵਾਪਸ SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	ਜੇ (cidx < ਅੰਕ_hw_ctrs) {
		ਜੇ (!(__test_bit(cidx, &counter_ਨਕਾਬ)))
			ਵਾਪਸ SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_ਚੱਕਰ + cidx;
		/* mcycle & minstret are always 64 bit */
		ਜੇ (cidx == 0 || cidx == 2)
			cinfo.ਚੌੜਾਈ = 63;
		ਨਹੀਂਤਾਂ
			cinfo.ਚੌੜਾਈ = sbi_hart_mhpm_ਬਿੱਟ(scratch) - 1;
	} ਨਹੀਂਤਾਂ {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.ਚੌੜਾਈ = 63;
		ਜੇ (pmu_dev && pmu_dev->fw_counter_ਚੌੜਾਈ) {
			ਚੌੜਾਈ = pmu_dev->fw_counter_ਚੌੜਾਈ();
			ਜੇ (ਚੌੜਾਈ)
				cinfo.ਚੌੜਾਈ = ਚੌੜਾਈ - 1;
		}
	}

	*ctr_info = cinfo.ਮੁੱਲ;

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_pmu_event_get_info(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ shmem_phys_lo, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ shmem_phys_hi,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅੰਕ_events, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ shmem_ਮਾਪ = ਅੰਕ_events * ਆਕਾਰ(ਬਣਤਰ sbi_pmu_event_info);
	ਪੂਰਨ i, j, event_type;
	ਬਣਤਰ sbi_pmu_event_info *einfo;
	ਬਣਤਰ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	ਬਣਤਰ sbi_pmu_hw_event *ਆਰਜ਼ੀ;
	bool ਮਿਲਿਆ = false;

	ਜੇ (flags != 0)
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	ਜੇ (!ਅੰਕ_events || (shmem_phys_lo & 0xF))
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	ਜੇ (shmem_phys_hi)
		ਵਾਪਸ SBI_EINVALID_ADDR;

	ਜੇ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_ਮਾਪ, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ਵਾਪਸ SBI_ERR_INVALID_ਪਤਾ;

	sbi_hart_protection_ਆਰਜ਼ੀ_map_range(shmem_phys_lo, shmem_ਮਾਪ);

	einfo = (ਬਣਤਰ sbi_pmu_event_info *)(shmem_phys_lo);
	ਲਈ (i = 0; i < ਅੰਕ_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		ਜੇ (event_idx & SBI_PMU_EVENT_IDX_MBZ_ਨਕਾਬ)
			ਵਾਪਸ SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_ਅੰਕੜੇ);
		ਜੇ (event_type < 0) {
			einfo[i].output = 0;
		} ਨਹੀਂਤਾਂ ਜੇ (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} ਨਹੀਂਤਾਂ {
			ਲਈ (j = 0; j < ਅੰਕ_hw_events; j++) {
				ਆਰਜ਼ੀ = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				ਜੇ (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					ਜੇ (ਆਰਜ਼ੀ->ਸ਼ੁਰੂ_idx > event_idx ||
					    event_idx > ਆਰਜ਼ੀ->ਸਿਰਾ_idx)
						ਜਾਰੀ;
					/* just match the selector */
					ਜੇ (ਆਰਜ਼ੀ->select == (einfo[i].event_ਅੰਕੜੇ &
									ਆਰਜ਼ੀ->select_ਨਕਾਬ)) {
						ਮਿਲਿਆ = true;
						ਰੋਕੋ;
					}
				} ਨਹੀਂਤਾਂ ਜੇ (ਆਰਜ਼ੀ->ਸ਼ੁਰੂ_idx <= event_idx &&
					   event_idx <= ਆਰਜ਼ੀ->ਸਿਰਾ_idx) {
					ਮਿਲਿਆ = true;
					ਰੋਕੋ;
				}
			}
			ਜੇ (ਮਿਲਿਆ)
				einfo[i].output = 1;
			ਨਹੀਂਤਾਂ
				einfo[i].output = 0;
			ਮਿਲਿਆ = false;
		}
	}

	sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range(shmem_phys_lo, shmem_ਮਾਪ);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ pmu_reset_event_map(ਬਣਤਰ sbi_pmu_hart_state *phs)
{
	ਪੂਰਨ j;

	/* Initialize the counter to event mapping table */
	ਲਈ (j = 3; j < ਕੁੱਲ_ctrs; j++)
		phs->ਸਰਗਰਮ_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	ਲਈ (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_ਅੰਕੜੇ[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

ਅਚਲ ਬਣਤਰ sbi_pmu_device *sbi_pmu_get_device(ਖਾਲੀ)
{
	ਵਾਪਸ pmu_dev;
}

ਖਾਲੀ sbi_pmu_ਸੈੱਟ_device(ਅਚਲ ਬਣਤਰ sbi_pmu_device *dev)
{
	ਜੇ (!dev || pmu_dev)
		ਵਾਪਸ;

	pmu_dev = dev;
}

ਖਾਲੀ sbi_pmu_exit(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	ਜੇ (unlikely(!phs))
		ਵਾਪਸ;

	pmu_reset_event_map(phs);
}

ਸਥਿਰ ਖਾਲੀ pmu_sse_enable(uint32_t event_id)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ irq_ਨਕਾਬ = sbi_pmu_irq_ਨਕਾਬ();

	csr_ਸੈੱਟ(CSR_MIE, irq_ਨਕਾਬ);
}

ਸਥਿਰ ਖਾਲੀ pmu_sse_disable(uint32_t event_id)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ irq_ਨਕਾਬ = sbi_pmu_irq_ਨਕਾਬ();

	csr_clear(CSR_MIE, irq_ਨਕਾਬ);
	csr_clear(CSR_MIP, irq_ਨਕਾਬ);
}

ਸਥਿਰ ਖਾਲੀ pmu_sse_complete(uint32_t event_id)
{
	csr_ਸੈੱਟ(CSR_MIE, sbi_pmu_irq_ਨਕਾਬ());
}

ਸਥਿਰ ਖਾਲੀ pmu_sse_ਰਜਿਸਟਰ(uint32_t event_id)
{
	ਬਣਤਰ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_ਨਕਾਬ());
}

ਸਥਿਰ ਖਾਲੀ pmu_sse_unregister(uint32_t event_id)
{
	ਬਣਤਰ sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_ਸੈੱਟ(CSR_MIDELEG, sbi_pmu_irq_ਨਕਾਬ());
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_sse_cb_ops pmu_sse_cb_ops = {
	.ਰਜਿਸਟਰ_cb = pmu_sse_ਰਜਿਸਟਰ,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

ਪੂਰਨ sbi_pmu_init(ਬਣਤਰ sbi_scratch *scratch, bool cold_boot)
{
	ਪੂਰਨ hpm_ਗਿਣਤੀ = sbi_fls(sbi_hart_mhpm_ਨਕਾਬ(scratch));
	ਬਣਤਰ sbi_pmu_hart_state *phs;
	ਅਚਲ ਬਣਤਰ sbi_platform *plat;
	ਪੂਰਨ rc;

	ਜੇ (cold_boot) {
		hw_event_map = sbi_calloc(ਆਕਾਰ(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		ਜੇ (!hw_event_map)
			ਵਾਪਸ SBI_ENOMEM;

		phs_ptr_ਵਿੱਥ = sbi_scratch_alloc_type_ਵਿੱਥ(ਖਾਲੀ *);
		ਜੇ (!phs_ptr_ਵਿੱਥ) {
			sbi_free(hw_event_map);
			ਵਾਪਸ SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		ਜੇ (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		ਜੇ (!hpm_ਗਿਣਤੀ)
			/* Only CY, TM & IR are implemented in the hw */
			ਅੰਕ_hw_ctrs = 3;
		ਨਹੀਂਤਾਂ
			ਅੰਕ_hw_ctrs = hpm_ਗਿਣਤੀ + 1;

		ਜੇ (ਅੰਕ_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			ਵਾਪਸ SBI_EINVAL;

		ਕੁੱਲ_ctrs = ਅੰਕ_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		ਜੇ (sbi_pmu_irq_ਦੋਆਧਾਰੀ_ਅੰਕ() >= 0)
			sbi_sse_ਜੋੜੋ_event(SBI_SSE_EVENT_LOCAL_PMU_ਉਛਾਲ, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	ਜੇ (!phs) {
		phs = sbi_zalloc(ਆਕਾਰ(*phs));
		ਜੇ (!phs)
			ਵਾਪਸ SBI_ENOMEM;
		phs->hartid = current_hartid();
		pmu_ਸੈੱਟ_hart_state_ptr(scratch, phs);
	}

	pmu_reset_event_map(phs);

	/* First three counters are fixed by the priv spec and we enable it by default */
	phs->ਸਰਗਰਮ_events[0] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_ਵਿੱਥ) |
				SBI_PMU_HW_CPU_ਚੱਕਰਾਂ;
	phs->ਸਰਗਰਮ_events[1] = SBI_PMU_EVENT_IDX_INVALID;
	phs->ਸਰਗਰਮ_events[2] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_ਵਿੱਥ) |
				SBI_PMU_HW_INSTRUCTIONS;

	ਵਾਪਸ 0;
}
