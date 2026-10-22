/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_hart_protection.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_pmu.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi/sbi_sse.h>

/** Information about hardware counters */
ഘടന sbi_pmu_hw_event {
	uint32_t counters;
	uint32_t start_idx;
	uint32_t end_idx;
	/* Event selector value used only for raw events. The event select value
	 * can be a even id or a selector value for set of events encoded in few
	 * bits. In case latter, the bits used for encoding of the events should
	 * be zeroed out in the select value.
	 */
	uint64_t select;
	 /**
	  * The select_mask indicates which bits are encoded for the event(s).
	  */
	uint64_t select_mask;
};

/* Information about PMU counters as per SBI specification */
സംഘം sbi_pmu_ctr_info {
	unsigned long മൂല്യം;
	ഘടന {
		unsigned long csr:12;
		unsigned long width:6;
#എങ്കിൽ __riscv_xlen == 32
		unsigned long reserved:13;
#അല്ലെങ്കിൽ
		unsigned long reserved:45;
#endif
		unsigned long type:1;
	};
};

#എങ്കിൽ SBI_PMU_FW_CTR_MAX >= BITS_PER_LONG
#error "Can't handle firmware counters beyond BITS_PER_LONG"
#endif

/** HW event configuration parameters */
ഘടന sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_data;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
ഘടന sbi_pmu_hart_state {
	/* HART to which this state belongs */
	uint32_t hartid;
	/* Counter to enabled event mapping */
	uint32_t active_events[SBI_PMU_HW_CTR_MAX + SBI_PMU_FW_CTR_MAX];
	/* Bitmap of firmware counters started */
	unsigned long fw_counters_started;
	/* if true, SSE is enabled */
	bool sse_enabled;
	/*
	 * Counter values for SBI firmware events and event codes
	 * for platform firmware events. Both are mutually exclusive
	 * and hence can optimally share the same memory.
	 */
	uint64_t fw_counters_data[SBI_PMU_FW_CTR_MAX];
	/* HW events configuration parameters from
	 * sbi_pmu_ctr_cfg_match() command which are
	 * used for restoring RAW hardware events after
	 * cpu suspending.
	 */
	ഘടന sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
സ്ഥിതം unsigned long phs_ptr_offset;

#define pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_offset ? sbi_scratch_read_type((__scratch), ശൂന്യം *, phs_ptr_offset) : ശൂന്യ

#define pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define pmu_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), ശൂന്യം *, phs_ptr_offset, (__phs))

/* Platform specific PMU device */
സ്ഥിതം സ്ഥിരം ഘടന sbi_pmu_device *pmu_dev = ശൂന്യ;

/* Mapping between event range and possible counters  */
സ്ഥിതം ഘടന sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
സ്ഥിതം uint32_t അക്കം_hw_events;
/* Maximum number of hardware counters available */
സ്ഥിതം uint32_t അക്കം_hw_ctrs;

/* Maximum number of counters available */
സ്ഥിതം uint32_t total_ctrs;

/* Helper macros to retrieve event idx and code type */
#define get_cidx_type(x) \
  (((x) & SBI_PMU_EVENT_IDX_TYPE_MASK) >> SBI_PMU_EVENT_IDX_TYPE_OFFSET)
#define get_cidx_code(x) (x & SBI_PMU_EVENT_IDX_CODE_MASK)

/**
 * Perform a sanity check on event & counter mappings with event range overlap check
 * @param evtA Pointer to the existing hw event structure
 * @param evtB Pointer to the new hw event structure
 *
 * Return false if the range doesn't overlap, true otherwise
 */
സ്ഥിതം bool pmu_event_range_overlap(ഘടന sbi_pmu_hw_event *evtA,
				    ഘടന sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	എങ്കിൽ (((evtA->end_idx < evtB->start_idx) && (evtA->end_idx < evtB->end_idx)) ||
	   ((evtA->start_idx > evtB->start_idx) && (evtA->start_idx > evtB->end_idx)))
		തിരികെ false;
	തിരികെ true;
}

സ്ഥിതം bool pmu_event_select_overlap(ഘടന sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_mask)
{
	എങ്കിൽ ((evt->select == select_val) && (evt->select_mask == select_mask))
		തിരികെ true;

	തിരികെ false;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_event_validate(ഘടന sbi_pmu_hart_state *phs,
			      unsigned long event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_code = get_cidx_code(event_idx);
	uint32_t event_idx_code_max = -1;
	uint32_t cache_ops_result, cache_ops_id, cache_id;

	തിരഞ്ഞെടുപ്പ്(event_idx_type) {
	അവസ്ഥ SBI_PMU_EVENT_TYPE_HW:
		event_idx_code_max = SBI_PMU_HW_GENERAL_MAX;
		നിർത്തുക;
	അവസ്ഥ SBI_PMU_EVENT_TYPE_FW:
		എങ്കിൽ ((event_idx_code >= SBI_PMU_FW_MAX &&
		    event_idx_code <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_code > SBI_PMU_FW_PLATFORM)
			തിരികെ SBI_EINVAL;

		എങ്കിൽ (SBI_PMU_FW_PLATFORM == event_idx_code &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			തിരികെ pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		അല്ലെങ്കിൽ
			event_idx_code_max = SBI_PMU_FW_MAX;
		നിർത്തുക;
	അവസ്ഥ SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_result = event_idx_code &
					SBI_PMU_EVENT_HW_CACHE_OPS_RESULT;
		cache_ops_id = (event_idx_code &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_MASK) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_OFFSET;
		cache_id = (event_idx_code &
			    SBI_PMU_EVENT_HW_CACHE_ID_MASK) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_OFFSET;
		എങ്കിൽ ((cache_ops_result < SBI_PMU_HW_CACHE_RESULT_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			തിരികെ event_idx_type;
		അല്ലെങ്കിൽ
			തിരികെ SBI_EINVAL;
		നിർത്തുക;
	അവസ്ഥ SBI_PMU_EVENT_TYPE_HW_RAW:
	അവസ്ഥ SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_code_max = 1; // event_idx.code should be zero
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		തിരികെ SBI_EINVAL;
	}

	എങ്കിൽ (event_idx_code < event_idx_code_max)
		തിരികെ event_idx_type;

	തിരികെ SBI_EINVAL;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_ctr_validate(ഘടന sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_code)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	എങ്കിൽ (cidx >= total_ctrs)
		തിരികെ SBI_EINVAL;

	event_idx_val = phs->active_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	എങ്കിൽ (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		തിരികെ SBI_EINVAL;

	*event_idx_code = get_cidx_code(event_idx_val);

	തിരികെ event_idx_type;
}

സ്ഥിതം bool pmu_ctr_idx_validate(unsigned long cbase, unsigned long cmask)
{
	unsigned long last;

	എങ്കിൽ (!cmask)
		തിരികെ false;

	last = sbi_fls(cmask);
	എങ്കിൽ (cbase > -1UL - last)
		തിരികെ false;

	തിരികെ (cbase + last) < total_ctrs;
}

പൂർണ്ണസംഖ്യ sbi_pmu_ctr_fw_read(unsigned long cidx, uint64_t *cval, bool മുകളിലെ_bits)
{
	പൂർണ്ണസംഖ്യ event_idx_type;
	uint32_t event_code;
	ഘടന sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	എങ്കിൽ (unlikely(!phs))
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (cidx < അക്കം_hw_ctrs || cidx >= total_ctrs)
		തിരികെ SBI_EINVAL;

#എങ്കിൽ __riscv_xlen > 32
	എങ്കിൽ (മുകളിലെ_bits)
		തിരികെ 0;
#endif

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
	എങ്കിൽ (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (SBI_PMU_FW_PLATFORM == event_code) {
		എങ്കിൽ (pmu_dev && pmu_dev->fw_counter_read_മൂല്യം)
			*cval = pmu_dev->fw_counter_read_മൂല്യം(phs->hartid,
							       cidx -
							       അക്കം_hw_ctrs);
		അല്ലെങ്കിൽ
			*cval = 0;
	} അല്ലെങ്കിൽ
		*cval = phs->fw_counters_data[cidx - അക്കം_hw_ctrs];

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_add_hw_event_map(u32 eidx_start, u32 eidx_end, u32 cmap,
				uint64_t select, uint64_t select_mask)
{
	പൂർണ്ണസംഖ്യ i = 0;
	bool is_overlap;
	ഘടന sbi_pmu_hw_event *event = &hw_event_map[അക്കം_hw_events];
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_mask = sbi_hart_mhpm_mask(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	എങ്കിൽ (eidx_start > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_MASK))
		തിരികെ SBI_EDENIED;

	എങ്കിൽ (അക്കം_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		തിരികെ SBI_EFAIL;
	}

	event->start_idx = eidx_start;
	event->end_idx = eidx_end;

	/* Sanity check */
	വേണ്ടി (i = 0; i < അക്കം_hw_events; i++) {
		എങ്കിൽ (eidx_start == SBI_PMU_EVENT_RAW_IDX ||
			eidx_start == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_mask);
		അല്ലെങ്കിൽ
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		എങ്കിൽ (is_overlap)
			പോകുക reset_event;
	}

	event->select_mask = select_mask;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_mask;
	event->select = select;
	അക്കം_hw_events++;

	തിരികെ 0;

reset_event:
	event->start_idx = 0;
	event->end_idx = 0;
	തിരികെ SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
പൂർണ്ണസംഖ്യ sbi_pmu_add_hw_event_counter_map(u32 eidx_start, u32 eidx_end, u32 cmap)
{
	എങ്കിൽ ((eidx_start > eidx_end) || eidx_start >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_end >= SBI_PMU_EVENT_RAW_IDX)
		തിരികെ SBI_EINVAL;

	തിരികെ pmu_add_hw_event_map(eidx_start, eidx_end, cmap, 0, 0);
}

പൂർണ്ണസംഖ്യ sbi_pmu_add_raw_event_counter_map(uint64_t select, uint64_t select_mask, u32 cmap)
{
	തിരികെ pmu_add_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_mask);
}

ശൂന്യം sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_mask());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_ctr_enable_irq_hw(പൂർണ്ണസംഖ്യ ctr_idx)
{
	unsigned long mhpmevent_csr;
	unsigned long mhpmevent_curr;
	unsigned long mip_val;
	unsigned long of_mask;

	എങ്കിൽ (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		തിരികെ SBI_EFAIL;

#എങ്കിൽ __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	of_mask = (uint32_t)~MHPMEVENTH_OF;
#അല്ലെങ്കിൽ
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	of_mask = ~MHPMEVENT_OF;
#endif

	mhpmevent_curr = csr_read_അക്കം(mhpmevent_csr);
	mip_val = csr_read(CSR_MIP);
	/**
	 * Clear out the OF bit so that next interrupt can be enabled.
	 * This should be done only when the corresponding overflow interrupt
	 * bit is cleared. That indicates that software has already handled the
	 * previous interrupts or the hardware yet to set an overflow interrupt.
	 * Otherwise, there will be race conditions where we may clear the bit
	 * the software is yet to handle the interrupt.
	 */
	എങ്കിൽ (!(mip_val & sbi_pmu_irq_mask())) {
		mhpmevent_curr &= of_mask;
		csr_write_അക്കം(mhpmevent_csr, mhpmevent_curr);
	}

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#എങ്കിൽ __riscv_xlen == 32
	csr_write_അക്കം(CSR_MCYCLE + cidx, 0);
	csr_write_അക്കം(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_അക്കം(CSR_MCYCLEH + cidx, ival >> BITS_PER_LONG);
#അല്ലെങ്കിൽ
	csr_write_അക്കം(CSR_MCYCLE + cidx, ival);
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_ctr_start_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	എങ്കിൽ (cidx >= അക്കം_hw_ctrs || cidx == 1)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11) {
		എങ്കിൽ (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		തിരികെ 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	എങ്കിൽ (!__test_bit(cidx, &mctr_inhbt))
		തിരികെ SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	എങ്കിൽ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	എങ്കിൽ (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	എങ്കിൽ (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_pmu_irq_bit(ശൂന്യം)
{
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	എങ്കിൽ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		തിരികെ IRQ_PMU_OVF;
	എങ്കിൽ (pmu_dev && pmu_dev->hw_counter_irq_bit)
		തിരികെ pmu_dev->hw_counter_irq_bit();

	തിരികെ -1;
}

unsigned long sbi_pmu_irq_mask(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ irq_bit = sbi_pmu_irq_bit();

	എങ്കിൽ (irq_bit < 0)
		തിരികെ 0;

	തിരികെ BIT(irq_bit);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_ctr_start_fw(ഘടന sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_code,
			    uint64_t event_data, uint64_t ival,
			    bool ival_update)
{
	പൂർണ്ണസംഖ്യ ret;

	എങ്കിൽ ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (phs->fw_counters_started & BIT(cidx - അക്കം_hw_ctrs))
		തിരികെ SBI_EALREADY_STARTED;

	എങ്കിൽ (SBI_PMU_FW_PLATFORM == event_code) {
		എങ്കിൽ (!pmu_dev ||
		    !pmu_dev->fw_counter_write_മൂല്യം ||
		    !pmu_dev->fw_counter_start) {
			തിരികെ SBI_EINVAL;
		}

		എങ്കിൽ (ival_update)
			pmu_dev->fw_counter_write_മൂല്യം(phs->hartid,
							cidx - അക്കം_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_start(phs->hartid,
						 cidx - അക്കം_hw_ctrs,
						 event_data);
		എങ്കിൽ (ret)
			തിരികെ ret;
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (ival_update)
			phs->fw_counters_data[cidx - അക്കം_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= BIT(cidx - അക്കം_hw_ctrs);

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം pmu_update_inhibit_flags(unsigned long flags, uint64_t *mhpmevent_val)
{
	എങ്കിൽ (flags & SBI_PMU_CFG_FLAG_SET_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	എങ്കിൽ (flags & SBI_PMU_CFG_FLAG_SET_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	എങ്കിൽ (flags & SBI_PMU_CFG_FLAG_SET_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	എങ്കിൽ (flags & SBI_PMU_CFG_FLAG_SET_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_update_hw_mhpmevent(ഘടന sbi_pmu_hw_event *hw_evt, പൂർണ്ണസംഖ്യ ctr_idx,
				   unsigned long flags, unsigned long eindex,
				   uint64_t data)
{
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	സ്ഥിരം ഘടന sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_to_mhpmevent(plat, eindex, data);

	എങ്കിൽ (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		തിരികെ SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	എങ്കിൽ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_MASK) |
				 MHPMEVENT_MINH | MHPMEVENT_OF;

	എങ്കിൽ (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	എങ്കിൽ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	എങ്കിൽ (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, ctr_idx);

#എങ്കിൽ __riscv_xlen == 32
	csr_write_അക്കം(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	എങ്കിൽ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_അക്കം(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> BITS_PER_LONG);
#അല്ലെങ്കിൽ
	csr_write_അക്കം(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#endif

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_pmu_ctr_start(unsigned long cbase, unsigned long cmask,
		      unsigned long flags, uint64_t ival)
{
	ഘടന sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	എങ്കിൽ (unlikely(!phs))
		തിരികെ SBI_EINVAL;

	പൂർണ്ണസംഖ്യ event_idx_type;
	uint32_t event_code;
	പൂർണ്ണസംഖ്യ ret = SBI_EINVAL;
	bool bUpdate = false;
	പൂർണ്ണസംഖ്യ i, cidx;
	uint64_t edata;

	എങ്കിൽ (!pmu_ctr_idx_validate(cbase, cmask))
		തിരികെ ret;

	എങ്കിൽ (flags & ~SBI_PMU_START_FLAGS_MASK)
		തിരികെ SBI_ERR_INVALID_PARAM;

	എങ്കിൽ (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		തിരികെ SBI_ENO_SHMEM;

	എങ്കിൽ (flags & SBI_PMU_START_FLAG_SET_INIT_മൂല്യം)
		bUpdate = true;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		എങ്കിൽ (event_idx_type < 0)
			/* Continue the start operation for other counters */
			തുടരുക;
		അല്ലെങ്കിൽ എങ്കിൽ (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_code == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_data[cidx - അക്കം_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_start_fw(phs, cidx, event_code, edata,
					       ival, bUpdate);
			എങ്കിൽ (ret)
				തിരികെ ret;
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (cidx >= 3) {
				ഘടന sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->active_events[cidx],
							ev_cfg->event_data);
				എങ്കിൽ (ret)
					തിരികെ ret;
			}
			ret = pmu_ctr_start_hw(cidx, ival, bUpdate);
			എങ്കിൽ (ret)
				തിരികെ ret;
		}
	}

	തിരികെ ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_ctr_stop_hw(uint32_t cidx)
{
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	എങ്കിൽ (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11)
		തിരികെ 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	എങ്കിൽ (cidx >= അക്കം_hw_ctrs || cidx == 1)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		എങ്കിൽ (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		തിരികെ 0;
	} അല്ലെങ്കിൽ
		തിരികെ SBI_EALREADY_STOPPED;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_ctr_stop_fw(ഘടന sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_code)
{
	പൂർണ്ണസംഖ്യ ret;

	എങ്കിൽ ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (!(phs->fw_counters_started & BIT(cidx - അക്കം_hw_ctrs)))
		തിരികെ SBI_EALREADY_STOPPED;

	എങ്കിൽ (SBI_PMU_FW_PLATFORM == event_code &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - അക്കം_hw_ctrs);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	phs->fw_counters_started &= ~BIT(cidx - അക്കം_hw_ctrs);

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_reset_hw_mhpmevent(പൂർണ്ണസംഖ്യ ctr_idx)
{
	എങ്കിൽ (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		തിരികെ SBI_EFAIL;
#എങ്കിൽ __riscv_xlen == 32
	csr_write_അക്കം(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	എങ്കിൽ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_അക്കം(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#അല്ലെങ്കിൽ
	csr_write_അക്കം(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#endif

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_pmu_ctr_stop(unsigned long cbase, unsigned long cmask,
		     unsigned long flag)
{
	ഘടന sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	എങ്കിൽ (unlikely(!phs))
		തിരികെ SBI_EINVAL;

	പൂർണ്ണസംഖ്യ ret = SBI_EINVAL;
	പൂർണ്ണസംഖ്യ event_idx_type;
	uint32_t event_code;
	പൂർണ്ണസംഖ്യ i, cidx;

	എങ്കിൽ (!pmu_ctr_idx_validate(cbase, cmask))
		തിരികെ ret;

	എങ്കിൽ (flag & ~SBI_PMU_STOP_FLAGS_MASK)
		തിരികെ SBI_ERR_INVALID_PARAM;

	എങ്കിൽ (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		തിരികെ SBI_ENO_SHMEM;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		എങ്കിൽ (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			തുടരുക;

		അല്ലെങ്കിൽ എങ്കിൽ (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_code);
		അല്ലെങ്കിൽ
			ret = pmu_ctr_stop_hw(cidx);

		എങ്കിൽ(ret)
			തിരികെ ret;

		എങ്കിൽ (cidx > (CSR_INSTRET - CSR_CYCLE) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->active_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	എങ്കിൽ (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_mask());

	തിരികെ ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_fixed_ctr_update_inhibit_bits(പൂർണ്ണസംഖ്യ fixed_ctr, unsigned long flags)
{
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#എങ്കിൽ __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#endif
	എങ്കിൽ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_mode))
		തിരികെ fixed_ctr;

	തിരഞ്ഞെടുപ്പ് (fixed_ctr) {
	അവസ്ഥ 0:
		cfg_csr_no = CSR_MCYCLECFG;
#എങ്കിൽ __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#endif
		നിർത്തുക;
	അവസ്ഥ 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#എങ്കിൽ __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#endif
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		തിരികെ SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	എങ്കിൽ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#എങ്കിൽ __riscv_xlen == 32
		csr_write_അക്കം(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_അക്കം(cfgh_csr_no, cfg_val >> BITS_PER_LONG);
#അല്ലെങ്കിൽ
		csr_write_അക്കം(cfg_csr_no, cfg_val);
#endif
	}
	എങ്കിൽ (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, fixed_ctr);
	തിരികെ fixed_ctr;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_ctr_find_fixed_hw(unsigned long evt_idx_code)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	എങ്കിൽ (evt_idx_code == SBI_PMU_HW_CPU_CYCLES)
		തിരികെ 0;
	അല്ലെങ്കിൽ എങ്കിൽ (evt_idx_code == SBI_PMU_HW_INSTRUCTIONS)
		തിരികെ 2;
	അല്ലെങ്കിൽ
		തിരികെ SBI_EINVAL;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_ctr_find_hw(ഘടന sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   unsigned long flags,
			   unsigned long event_idx, uint64_t data)
{
	unsigned long ctr_mask;
	പൂർണ്ണസംഖ്യ i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	ഘടന sbi_pmu_hw_event *temp;
	unsigned long mctr_inhbt = 0;
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	എങ്കിൽ (cbase >= അക്കം_hw_ctrs)
		തിരികെ SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_find_fixed_hw(event_idx);
	എങ്കിൽ (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		തിരികെ pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);

	എങ്കിൽ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	വേണ്ടി (i = 0; i < അക്കം_hw_events; i++) {
		temp = &hw_event_map[i];
		എങ്കിൽ ((temp->start_idx > event_idx && event_idx < temp->end_idx) ||
		    (temp->start_idx < event_idx && event_idx > temp->end_idx))
			തുടരുക;

		/* For raw events, event data is used as the select value */
		എങ്കിൽ (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_mask = temp->select_mask;

			/* The non-event map bits of data should match the selector */
			എങ്കിൽ (temp->select != (data & select_mask))
				തുടരുക;
		}
		/* Fixed counters should not be part of the search */
		ctr_mask = temp->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_MASK);
		for_each_set_bit_from(cbase, &ctr_mask, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			എങ്കിൽ (phs->active_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				തുടരുക;
			/* If mcountinhibit is supported, the bit must be enabled */
			എങ്കിൽ ((sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				തുടരുക;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			നിർത്തുക;
		}
	}

	എങ്കിൽ (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		എങ്കിൽ (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			തിരികെ SBI_EFAIL;

		തിരികെ pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(temp, ctr_idx, flags, event_idx, data);

	എങ്കിൽ (!ret)
		ret = ctr_idx;

	തിരികെ ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ pmu_ctr_find_fw(ഘടന sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   uint32_t event_code, uint64_t edata)
{
	പൂർണ്ണസംഖ്യ i, cidx;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		എങ്കിൽ (cidx < അക്കം_hw_ctrs || total_ctrs <= cidx)
			തുടരുക;
		എങ്കിൽ (phs->active_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			തുടരുക;
		എങ്കിൽ (SBI_PMU_FW_PLATFORM == event_code &&
		    pmu_dev && pmu_dev->fw_counter_match_encoding) {
			എങ്കിൽ (!pmu_dev->fw_counter_match_encoding(phs->hartid,
							    cidx - അക്കം_hw_ctrs,
							    edata))
				തുടരുക;
		}

		തിരികെ cidx;
	}

	തിരികെ SBI_ENOTSUPP;
}

പൂർണ്ണസംഖ്യ sbi_pmu_ctr_cfg_match(unsigned long cidx_base, unsigned long cidx_mask,
			  unsigned long flags, unsigned long event_idx,
			  uint64_t event_data)
{
	ഘടന sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	എങ്കിൽ (unlikely(!phs))
		തിരികെ SBI_EINVAL;

	പൂർണ്ണസംഖ്യ ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_code;

	എങ്കിൽ (!pmu_ctr_idx_validate(cidx_base, cidx_mask))
		തിരികെ SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_data);
	എങ്കിൽ (event_type < 0)
		തിരികെ SBI_EINVAL;
	event_code = get_cidx_code(event_idx);

	എങ്കിൽ (flags & SBI_PMU_CFG_FLAG_SKIP_MATCH) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		unsigned long cidx_first = cidx_base + sbi_ffs(cidx_mask);

		എങ്കിൽ (cidx_first >= total_ctrs)
			തിരികെ SBI_EINVAL;

		എങ്കിൽ (phs->active_events[cidx_first] == SBI_PMU_EVENT_IDX_INVALID)
			തിരികെ SBI_EINVAL;
		ctr_idx = cidx_first;
		പോകുക skip_match;
	}

	എങ്കിൽ (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_find_fw(phs, cidx_base, cidx_mask,
					  event_code, event_data);
		എങ്കിൽ ((event_code == SBI_PMU_FW_PLATFORM) && (ctr_idx >= അക്കം_hw_ctrs))
			phs->fw_counters_data[ctr_idx - അക്കം_hw_ctrs] =
								event_data;
	} അല്ലെങ്കിൽ {
		ctr_idx = pmu_ctr_find_hw(phs, cidx_base, cidx_mask, flags,
					  event_idx, event_data);
		എങ്കിൽ (ctr_idx >= 0) {
			ഘടന sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_data = event_data;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_MASK;
		}
	}

	എങ്കിൽ (ctr_idx < 0)
		തിരികെ SBI_ENOTSUPP;

	phs->active_events[ctr_idx] = event_idx;
skip_match:
	എങ്കിൽ (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		എങ്കിൽ (flags & SBI_PMU_CFG_FLAG_CLEAR_മൂല്യം)
			pmu_ctr_write_hw(ctr_idx, 0);
		എങ്കിൽ (flags & SBI_PMU_CFG_FLAG_AUTO_START)
			pmu_ctr_start_hw(ctr_idx, 0, false);
	} അല്ലെങ്കിൽ എങ്കിൽ (event_type == SBI_PMU_EVENT_TYPE_FW) {
		എങ്കിൽ (flags & SBI_PMU_CFG_FLAG_CLEAR_മൂല്യം)
			phs->fw_counters_data[ctr_idx - അക്കം_hw_ctrs] = 0;
		എങ്കിൽ (flags & SBI_PMU_CFG_FLAG_AUTO_START) {
			എങ്കിൽ (SBI_PMU_FW_PLATFORM == event_code &&
			    pmu_dev && pmu_dev->fw_counter_start) {
				ret = pmu_dev->fw_counter_start(
					phs->hartid,
					ctr_idx - അക്കം_hw_ctrs, event_data);
				എങ്കിൽ (ret)
					തിരികെ ret;
			}
			phs->fw_counters_started |= BIT(ctr_idx - അക്കം_hw_ctrs);
		}
	}

	തിരികെ ctr_idx;
}

പൂർണ്ണസംഖ്യ sbi_pmu_ctr_incr_fw(എണ്ണൽ sbi_pmu_fw_event_code_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = ശൂന്യ;
	ഘടന sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	എങ്കിൽ (unlikely(!phs))
		തിരികെ 0;

	എങ്കിൽ (likely(!phs->fw_counters_started))
		തിരികെ 0;

	എങ്കിൽ (unlikely(fw_id >= SBI_PMU_FW_MAX))
		തിരികെ SBI_EINVAL;

	വേണ്ടി (cidx = അക്കം_hw_ctrs; cidx < total_ctrs; cidx++) {
		എങ്കിൽ (get_cidx_code(phs->active_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & BIT(cidx - അക്കം_hw_ctrs))) {
			fcounter = &phs->fw_counters_data[cidx - അക്കം_hw_ctrs];
			നിർത്തുക;
		}
	}

	എങ്കിൽ (fcounter)
		(*fcounter)++;

	തിരികെ 0;
}

unsigned long sbi_pmu_അക്കം_ctr(ശൂന്യം)
{
	തിരികെ (അക്കം_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

പൂർണ്ണസംഖ്യ sbi_pmu_ctr_get_info(uint32_t cidx, unsigned long *ctr_info)
{
	പൂർണ്ണസംഖ്യ width;
	സംഘം sbi_pmu_ctr_info cinfo = {0};
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long counter_mask = (unsigned long)sbi_hart_mhpm_mask(scratch) |
				     SBI_PMU_CY_IR_MASK;

	/* Sanity check */
	എങ്കിൽ (cidx >= total_ctrs)
		തിരികെ SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	എങ്കിൽ (cidx < അക്കം_hw_ctrs) {
		എങ്കിൽ (!(__test_bit(cidx, &counter_mask)))
			തിരികെ SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_CYCLE + cidx;
		/* mcycle & minstret are always 64 bit */
		എങ്കിൽ (cidx == 0 || cidx == 2)
			cinfo.width = 63;
		അല്ലെങ്കിൽ
			cinfo.width = sbi_hart_mhpm_bits(scratch) - 1;
	} അല്ലെങ്കിൽ {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.width = 63;
		എങ്കിൽ (pmu_dev && pmu_dev->fw_counter_width) {
			width = pmu_dev->fw_counter_width();
			എങ്കിൽ (width)
				cinfo.width = width - 1;
		}
	}

	*ctr_info = cinfo.മൂല്യം;

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_pmu_event_get_info(unsigned long shmem_phys_lo, unsigned long shmem_phys_hi,
			   unsigned long അക്കം_events, unsigned long flags)
{
	unsigned long shmem_size = അക്കം_events * വലുപ്പം(ഘടന sbi_pmu_event_info);
	പൂർണ്ണസംഖ്യ i, j, event_type;
	ഘടന sbi_pmu_event_info *einfo;
	ഘടന sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	ഘടന sbi_pmu_hw_event *temp;
	bool found = false;

	എങ്കിൽ (flags != 0)
		തിരികെ SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	എങ്കിൽ (!അക്കം_events || (shmem_phys_lo & 0xF))
		തിരികെ SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	എങ്കിൽ (shmem_phys_hi)
		തിരികെ SBI_EINVALID_ADDR;

	എങ്കിൽ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_size, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		തിരികെ SBI_ERR_INVALID_ADDRESS;

	sbi_hart_protection_temp_map_range(shmem_phys_lo, shmem_size);

	einfo = (ഘടന sbi_pmu_event_info *)(shmem_phys_lo);
	വേണ്ടി (i = 0; i < അക്കം_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		എങ്കിൽ (event_idx & SBI_PMU_EVENT_IDX_MBZ_MASK)
			തിരികെ SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_data);
		എങ്കിൽ (event_type < 0) {
			einfo[i].output = 0;
		} അല്ലെങ്കിൽ എങ്കിൽ (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} അല്ലെങ്കിൽ {
			വേണ്ടി (j = 0; j < അക്കം_hw_events; j++) {
				temp = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				എങ്കിൽ (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					എങ്കിൽ (temp->start_idx > event_idx ||
					    event_idx > temp->end_idx)
						തുടരുക;
					/* just match the selector */
					എങ്കിൽ (temp->select == (einfo[i].event_data &
									temp->select_mask)) {
						found = true;
						നിർത്തുക;
					}
				} അല്ലെങ്കിൽ എങ്കിൽ (temp->start_idx <= event_idx &&
					   event_idx <= temp->end_idx) {
					found = true;
					നിർത്തുക;
				}
			}
			എങ്കിൽ (found)
				einfo[i].output = 1;
			അല്ലെങ്കിൽ
				einfo[i].output = 0;
			found = false;
		}
	}

	sbi_hart_protection_temp_unmap_range(shmem_phys_lo, shmem_size);

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം pmu_reset_event_map(ഘടന sbi_pmu_hart_state *phs)
{
	പൂർണ്ണസംഖ്യ j;

	/* Initialize the counter to event mapping table */
	വേണ്ടി (j = 3; j < total_ctrs; j++)
		phs->active_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	വേണ്ടി (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_data[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

സ്ഥിരം ഘടന sbi_pmu_device *sbi_pmu_get_device(ശൂന്യം)
{
	തിരികെ pmu_dev;
}

ശൂന്യം sbi_pmu_set_device(സ്ഥിരം ഘടന sbi_pmu_device *dev)
{
	എങ്കിൽ (!dev || pmu_dev)
		തിരികെ;

	pmu_dev = dev;
}

ശൂന്യം sbi_pmu_exit(ഘടന sbi_scratch *scratch)
{
	ഘടന sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	എങ്കിൽ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	എങ്കിൽ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	എങ്കിൽ (unlikely(!phs))
		തിരികെ;

	pmu_reset_event_map(phs);
}

സ്ഥിതം ശൂന്യം pmu_sse_enable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_set(CSR_MIE, irq_mask);
}

സ്ഥിതം ശൂന്യം pmu_sse_disable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_clear(CSR_MIE, irq_mask);
	csr_clear(CSR_MIP, irq_mask);
}

സ്ഥിതം ശൂന്യം pmu_sse_complete(uint32_t event_id)
{
	csr_set(CSR_MIE, sbi_pmu_irq_mask());
}

സ്ഥിതം ശൂന്യം pmu_sse_register(uint32_t event_id)
{
	ഘടന sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_mask());
}

സ്ഥിതം ശൂന്യം pmu_sse_unregister(uint32_t event_id)
{
	ഘടന sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_set(CSR_MIDELEG, sbi_pmu_irq_mask());
}

സ്ഥിതം സ്ഥിരം ഘടന sbi_sse_cb_ops pmu_sse_cb_ops = {
	.register_cb = pmu_sse_register,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

പൂർണ്ണസംഖ്യ sbi_pmu_init(ഘടന sbi_scratch *scratch, bool cold_boot)
{
	പൂർണ്ണസംഖ്യ hpm_എണ്ണം = sbi_fls(sbi_hart_mhpm_mask(scratch));
	ഘടന sbi_pmu_hart_state *phs;
	സ്ഥിരം ഘടന sbi_platform *plat;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (cold_boot) {
		hw_event_map = sbi_calloc(വലുപ്പം(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		എങ്കിൽ (!hw_event_map)
			തിരികെ SBI_ENOMEM;

		phs_ptr_offset = sbi_scratch_alloc_type_offset(ശൂന്യം *);
		എങ്കിൽ (!phs_ptr_offset) {
			sbi_free(hw_event_map);
			തിരികെ SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		എങ്കിൽ (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		എങ്കിൽ (!hpm_എണ്ണം)
			/* Only CY, TM & IR are implemented in the hw */
			അക്കം_hw_ctrs = 3;
		അല്ലെങ്കിൽ
			അക്കം_hw_ctrs = hpm_എണ്ണം + 1;

		എങ്കിൽ (അക്കം_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			തിരികെ SBI_EINVAL;

		total_ctrs = അക്കം_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		എങ്കിൽ (sbi_pmu_irq_bit() >= 0)
			sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	എങ്കിൽ (!phs) {
		phs = sbi_zalloc(വലുപ്പം(*phs));
		എങ്കിൽ (!phs)
			തിരികെ SBI_ENOMEM;
		phs->hartid = current_hartid();
		pmu_set_hart_state_ptr(scratch, phs);
	}

	pmu_reset_event_map(phs);

	/* First three counters are fixed by the priv spec and we enable it by default */
	phs->active_events[0] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_OFFSET) |
				SBI_PMU_HW_CPU_CYCLES;
	phs->active_events[1] = SBI_PMU_EVENT_IDX_INVALID;
	phs->active_events[2] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_OFFSET) |
				SBI_PMU_HW_INSTRUCTIONS;

	തിരികെ 0;
}
