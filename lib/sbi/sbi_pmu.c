/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/sbi_bitops.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_hart_protection.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_pmu.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi/sbi_sse.h>

/** Information about hardware counters */
সংরচনা sbi_pmu_hw_event {
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
সঙ্ঘ sbi_pmu_ctr_info {
	unsigned long মান;
	সংরচনা {
		unsigned long csr:12;
		unsigned long width:6;
#যদি __riscv_xlen == 32
		unsigned long reserved:13;
#অন্যথা
		unsigned long reserved:45;
#endif
		unsigned long type:1;
	};
};

#যদি SBI_PMU_FW_CTR_MAX >= BITS_PER_LONG
#error "Can't handle firmware counters beyond BITS_PER_LONG"
#endif

/** HW event configuration parameters */
সংরচনা sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_data;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
সংরচনা sbi_pmu_hart_state {
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
	সংরচনা sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
স্থির unsigned long phs_ptr_offset;

#define pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_offset ? sbi_scratch_read_type((__scratch), শূন্য *, phs_ptr_offset) : রিক্ত

#define pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define pmu_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), শূন্য *, phs_ptr_offset, (__phs))

/* Platform specific PMU device */
স্থির ধ্রুব সংরচনা sbi_pmu_device *pmu_dev = রিক্ত;

/* Mapping between event range and possible counters  */
স্থির সংরচনা sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
স্থির uint32_t অঙ্ক_hw_events;
/* Maximum number of hardware counters available */
স্থির uint32_t অঙ্ক_hw_ctrs;

/* Maximum number of counters available */
স্থির uint32_t total_ctrs;

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
স্থির bool pmu_event_range_overlap(সংরচনা sbi_pmu_hw_event *evtA,
				    সংরচনা sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	যদি (((evtA->end_idx < evtB->start_idx) && (evtA->end_idx < evtB->end_idx)) ||
	   ((evtA->start_idx > evtB->start_idx) && (evtA->start_idx > evtB->end_idx)))
		প্রত্যাগম false;
	প্রত্যাগম true;
}

স্থির bool pmu_event_select_overlap(সংরচনা sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_mask)
{
	যদি ((evt->select == select_val) && (evt->select_mask == select_mask))
		প্রত্যাগম true;

	প্রত্যাগম false;
}

স্থির পূর্ণাঙ্ক pmu_event_validate(সংরচনা sbi_pmu_hart_state *phs,
			      unsigned long event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_code = get_cidx_code(event_idx);
	uint32_t event_idx_code_max = -1;
	uint32_t cache_ops_result, cache_ops_id, cache_id;

	বিকল্প(event_idx_type) {
	পক্ষ SBI_PMU_EVENT_TYPE_HW:
		event_idx_code_max = SBI_PMU_HW_GENERAL_MAX;
		ভঙ্গ;
	পক্ষ SBI_PMU_EVENT_TYPE_FW:
		যদি ((event_idx_code >= SBI_PMU_FW_MAX &&
		    event_idx_code <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_code > SBI_PMU_FW_PLATFORM)
			প্রত্যাগম SBI_EINVAL;

		যদি (SBI_PMU_FW_PLATFORM == event_idx_code &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			প্রত্যাগম pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		অন্যথা
			event_idx_code_max = SBI_PMU_FW_MAX;
		ভঙ্গ;
	পক্ষ SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_result = event_idx_code &
					SBI_PMU_EVENT_HW_CACHE_OPS_RESULT;
		cache_ops_id = (event_idx_code &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_MASK) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_OFFSET;
		cache_id = (event_idx_code &
			    SBI_PMU_EVENT_HW_CACHE_ID_MASK) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_OFFSET;
		যদি ((cache_ops_result < SBI_PMU_HW_CACHE_RESULT_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			প্রত্যাগম event_idx_type;
		অন্যথা
			প্রত্যাগম SBI_EINVAL;
		ভঙ্গ;
	পক্ষ SBI_PMU_EVENT_TYPE_HW_RAW:
	পক্ষ SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_code_max = 1; // event_idx.code should be zero
		ভঙ্গ;
	স্বভাব:
		প্রত্যাগম SBI_EINVAL;
	}

	যদি (event_idx_code < event_idx_code_max)
		প্রত্যাগম event_idx_type;

	প্রত্যাগম SBI_EINVAL;
}

স্থির পূর্ণাঙ্ক pmu_ctr_validate(সংরচনা sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_code)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	যদি (cidx >= total_ctrs)
		প্রত্যাগম SBI_EINVAL;

	event_idx_val = phs->active_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	যদি (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		প্রত্যাগম SBI_EINVAL;

	*event_idx_code = get_cidx_code(event_idx_val);

	প্রত্যাগম event_idx_type;
}

স্থির bool pmu_ctr_idx_validate(unsigned long cbase, unsigned long cmask)
{
	unsigned long last;

	যদি (!cmask)
		প্রত্যাগম false;

	last = sbi_fls(cmask);
	যদি (cbase > -1UL - last)
		প্রত্যাগম false;

	প্রত্যাগম (cbase + last) < total_ctrs;
}

পূর্ণাঙ্ক sbi_pmu_ctr_fw_read(unsigned long cidx, uint64_t *cval, bool উপরের_bits)
{
	পূর্ণাঙ্ক event_idx_type;
	uint32_t event_code;
	সংরচনা sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	যদি (unlikely(!phs))
		প্রত্যাগম SBI_EINVAL;

	যদি (cidx < অঙ্ক_hw_ctrs || cidx >= total_ctrs)
		প্রত্যাগম SBI_EINVAL;

#যদি __riscv_xlen > 32
	যদি (উপরের_bits)
		প্রত্যাগম 0;
#endif

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
	যদি (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		প্রত্যাগম SBI_EINVAL;

	যদি ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		প্রত্যাগম SBI_EINVAL;

	যদি (SBI_PMU_FW_PLATFORM == event_code) {
		যদি (pmu_dev && pmu_dev->fw_counter_read_মান)
			*cval = pmu_dev->fw_counter_read_মান(phs->hartid,
							       cidx -
							       অঙ্ক_hw_ctrs);
		অন্যথা
			*cval = 0;
	} অন্যথা
		*cval = phs->fw_counters_data[cidx - অঙ্ক_hw_ctrs];

	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক pmu_add_hw_event_map(u32 eidx_start, u32 eidx_end, u32 cmap,
				uint64_t select, uint64_t select_mask)
{
	পূর্ণাঙ্ক i = 0;
	bool is_overlap;
	সংরচনা sbi_pmu_hw_event *event = &hw_event_map[অঙ্ক_hw_events];
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_mask = sbi_hart_mhpm_mask(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	যদি (eidx_start > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_MASK))
		প্রত্যাগম SBI_EDENIED;

	যদি (অঙ্ক_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		প্রত্যাগম SBI_EFAIL;
	}

	event->start_idx = eidx_start;
	event->end_idx = eidx_end;

	/* Sanity check */
	প্রতি (i = 0; i < অঙ্ক_hw_events; i++) {
		যদি (eidx_start == SBI_PMU_EVENT_RAW_IDX ||
			eidx_start == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_mask);
		অন্যথা
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		যদি (is_overlap)
			গচ্ছ reset_event;
	}

	event->select_mask = select_mask;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_mask;
	event->select = select;
	অঙ্ক_hw_events++;

	প্রত্যাগম 0;

reset_event:
	event->start_idx = 0;
	event->end_idx = 0;
	প্রত্যাগম SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
পূর্ণাঙ্ক sbi_pmu_add_hw_event_counter_map(u32 eidx_start, u32 eidx_end, u32 cmap)
{
	যদি ((eidx_start > eidx_end) || eidx_start >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_end >= SBI_PMU_EVENT_RAW_IDX)
		প্রত্যাগম SBI_EINVAL;

	প্রত্যাগম pmu_add_hw_event_map(eidx_start, eidx_end, cmap, 0, 0);
}

পূর্ণাঙ্ক sbi_pmu_add_raw_event_counter_map(uint64_t select, uint64_t select_mask, u32 cmap)
{
	প্রত্যাগম pmu_add_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_mask);
}

শূন্য sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_mask());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW);
}

স্থির পূর্ণাঙ্ক pmu_ctr_enable_irq_hw(পূর্ণাঙ্ক ctr_idx)
{
	unsigned long mhpmevent_csr;
	unsigned long mhpmevent_curr;
	unsigned long mip_val;
	unsigned long of_mask;

	যদি (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		প্রত্যাগম SBI_EFAIL;

#যদি __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	of_mask = (uint32_t)~MHPMEVENTH_OF;
#অন্যথা
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	of_mask = ~MHPMEVENT_OF;
#endif

	mhpmevent_curr = csr_read_অঙ্ক(mhpmevent_csr);
	mip_val = csr_read(CSR_MIP);
	/**
	 * Clear out the OF bit so that next interrupt can be enabled.
	 * This should be done only when the corresponding overflow interrupt
	 * bit is cleared. That indicates that software has already handled the
	 * previous interrupts or the hardware yet to set an overflow interrupt.
	 * Otherwise, there will be race conditions where we may clear the bit
	 * the software is yet to handle the interrupt.
	 */
	যদি (!(mip_val & sbi_pmu_irq_mask())) {
		mhpmevent_curr &= of_mask;
		csr_write_অঙ্ক(mhpmevent_csr, mhpmevent_curr);
	}

	প্রত্যাগম 0;
}

স্থির শূন্য pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#যদি __riscv_xlen == 32
	csr_write_অঙ্ক(CSR_MCYCLE + cidx, 0);
	csr_write_অঙ্ক(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_অঙ্ক(CSR_MCYCLEH + cidx, ival >> BITS_PER_LONG);
#অন্যথা
	csr_write_অঙ্ক(CSR_MCYCLE + cidx, ival);
#endif
}

স্থির পূর্ণাঙ্ক pmu_ctr_start_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	যদি (cidx >= অঙ্ক_hw_ctrs || cidx == 1)
		প্রত্যাগম SBI_EINVAL;

	যদি (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11) {
		যদি (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		প্রত্যাগম 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	যদি (!__test_bit(cidx, &mctr_inhbt))
		প্রত্যাগম SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	যদি (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	যদি (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	যদি (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক sbi_pmu_irq_bit(শূন্য)
{
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	যদি (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		প্রত্যাগম IRQ_PMU_OVF;
	যদি (pmu_dev && pmu_dev->hw_counter_irq_bit)
		প্রত্যাগম pmu_dev->hw_counter_irq_bit();

	প্রত্যাগম -1;
}

unsigned long sbi_pmu_irq_mask(শূন্য)
{
	পূর্ণাঙ্ক irq_bit = sbi_pmu_irq_bit();

	যদি (irq_bit < 0)
		প্রত্যাগম 0;

	প্রত্যাগম BIT(irq_bit);
}

স্থির পূর্ণাঙ্ক pmu_ctr_start_fw(সংরচনা sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_code,
			    uint64_t event_data, uint64_t ival,
			    bool ival_update)
{
	পূর্ণাঙ্ক ret;

	যদি ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		প্রত্যাগম SBI_EINVAL;

	যদি (phs->fw_counters_started & BIT(cidx - অঙ্ক_hw_ctrs))
		প্রত্যাগম SBI_EALREADY_STARTED;

	যদি (SBI_PMU_FW_PLATFORM == event_code) {
		যদি (!pmu_dev ||
		    !pmu_dev->fw_counter_write_মান ||
		    !pmu_dev->fw_counter_start) {
			প্রত্যাগম SBI_EINVAL;
		}

		যদি (ival_update)
			pmu_dev->fw_counter_write_মান(phs->hartid,
							cidx - অঙ্ক_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_start(phs->hartid,
						 cidx - অঙ্ক_hw_ctrs,
						 event_data);
		যদি (ret)
			প্রত্যাগম ret;
	} অন্যথা {
		যদি (ival_update)
			phs->fw_counters_data[cidx - অঙ্ক_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= BIT(cidx - অঙ্ক_hw_ctrs);

	প্রত্যাগম 0;
}

স্থির শূন্য pmu_update_inhibit_flags(unsigned long flags, uint64_t *mhpmevent_val)
{
	যদি (flags & SBI_PMU_CFG_FLAG_SET_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	যদি (flags & SBI_PMU_CFG_FLAG_SET_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	যদি (flags & SBI_PMU_CFG_FLAG_SET_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	যদি (flags & SBI_PMU_CFG_FLAG_SET_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

স্থির পূর্ণাঙ্ক pmu_update_hw_mhpmevent(সংরচনা sbi_pmu_hw_event *hw_evt, পূর্ণাঙ্ক ctr_idx,
				   unsigned long flags, unsigned long eindex,
				   uint64_t data)
{
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ধ্রুব সংরচনা sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_to_mhpmevent(plat, eindex, data);

	যদি (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		প্রত্যাগম SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	যদি (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_MASK) |
				 MHPMEVENT_MINH | MHPMEVENT_OF;

	যদি (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	যদি (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	যদি (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, ctr_idx);

#যদি __riscv_xlen == 32
	csr_write_অঙ্ক(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	যদি (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_অঙ্ক(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> BITS_PER_LONG);
#অন্যথা
	csr_write_অঙ্ক(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#endif

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক sbi_pmu_ctr_start(unsigned long cbase, unsigned long cmask,
		      unsigned long flags, uint64_t ival)
{
	সংরচনা sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	যদি (unlikely(!phs))
		প্রত্যাগম SBI_EINVAL;

	পূর্ণাঙ্ক event_idx_type;
	uint32_t event_code;
	পূর্ণাঙ্ক ret = SBI_EINVAL;
	bool bUpdate = false;
	পূর্ণাঙ্ক i, cidx;
	uint64_t edata;

	যদি (!pmu_ctr_idx_validate(cbase, cmask))
		প্রত্যাগম ret;

	যদি (flags & ~SBI_PMU_START_FLAGS_MASK)
		প্রত্যাগম SBI_ERR_INVALID_PARAM;

	যদি (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		প্রত্যাগম SBI_ENO_SHMEM;

	যদি (flags & SBI_PMU_START_FLAG_SET_INIT_মান)
		bUpdate = true;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		যদি (event_idx_type < 0)
			/* Continue the start operation for other counters */
			অনুবর্ত;
		অন্যথা যদি (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_code == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_data[cidx - অঙ্ক_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_start_fw(phs, cidx, event_code, edata,
					       ival, bUpdate);
			যদি (ret)
				প্রত্যাগম ret;
		} অন্যথা {
			যদি (cidx >= 3) {
				সংরচনা sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->active_events[cidx],
							ev_cfg->event_data);
				যদি (ret)
					প্রত্যাগম ret;
			}
			ret = pmu_ctr_start_hw(cidx, ival, bUpdate);
			যদি (ret)
				প্রত্যাগম ret;
		}
	}

	প্রত্যাগম ret;
}

স্থির পূর্ণাঙ্ক pmu_ctr_stop_hw(uint32_t cidx)
{
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	যদি (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11)
		প্রত্যাগম 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	যদি (cidx >= অঙ্ক_hw_ctrs || cidx == 1)
		প্রত্যাগম SBI_EINVAL;

	যদি (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		যদি (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		প্রত্যাগম 0;
	} অন্যথা
		প্রত্যাগম SBI_EALREADY_STOPPED;
}

স্থির পূর্ণাঙ্ক pmu_ctr_stop_fw(সংরচনা sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_code)
{
	পূর্ণাঙ্ক ret;

	যদি ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		প্রত্যাগম SBI_EINVAL;

	যদি (!(phs->fw_counters_started & BIT(cidx - অঙ্ক_hw_ctrs)))
		প্রত্যাগম SBI_EALREADY_STOPPED;

	যদি (SBI_PMU_FW_PLATFORM == event_code &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - অঙ্ক_hw_ctrs);
		যদি (ret)
			প্রত্যাগম ret;
	}

	phs->fw_counters_started &= ~BIT(cidx - অঙ্ক_hw_ctrs);

	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক pmu_reset_hw_mhpmevent(পূর্ণাঙ্ক ctr_idx)
{
	যদি (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		প্রত্যাগম SBI_EFAIL;
#যদি __riscv_xlen == 32
	csr_write_অঙ্ক(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	যদি (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_অঙ্ক(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#অন্যথা
	csr_write_অঙ্ক(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#endif

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক sbi_pmu_ctr_stop(unsigned long cbase, unsigned long cmask,
		     unsigned long flag)
{
	সংরচনা sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	যদি (unlikely(!phs))
		প্রত্যাগম SBI_EINVAL;

	পূর্ণাঙ্ক ret = SBI_EINVAL;
	পূর্ণাঙ্ক event_idx_type;
	uint32_t event_code;
	পূর্ণাঙ্ক i, cidx;

	যদি (!pmu_ctr_idx_validate(cbase, cmask))
		প্রত্যাগম ret;

	যদি (flag & ~SBI_PMU_STOP_FLAGS_MASK)
		প্রত্যাগম SBI_ERR_INVALID_PARAM;

	যদি (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		প্রত্যাগম SBI_ENO_SHMEM;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		যদি (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			অনুবর্ত;

		অন্যথা যদি (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_code);
		অন্যথা
			ret = pmu_ctr_stop_hw(cidx);

		যদি(ret)
			প্রত্যাগম ret;

		যদি (cidx > (CSR_INSTRET - CSR_CYCLE) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->active_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	যদি (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_mask());

	প্রত্যাগম ret;
}

স্থির পূর্ণাঙ্ক pmu_fixed_ctr_update_inhibit_bits(পূর্ণাঙ্ক fixed_ctr, unsigned long flags)
{
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#যদি __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#endif
	যদি (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_mode))
		প্রত্যাগম fixed_ctr;

	বিকল্প (fixed_ctr) {
	পক্ষ 0:
		cfg_csr_no = CSR_MCYCLECFG;
#যদি __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#endif
		ভঙ্গ;
	পক্ষ 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#যদি __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#endif
		ভঙ্গ;
	স্বভাব:
		প্রত্যাগম SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	যদি (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#যদি __riscv_xlen == 32
		csr_write_অঙ্ক(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_অঙ্ক(cfgh_csr_no, cfg_val >> BITS_PER_LONG);
#অন্যথা
		csr_write_অঙ্ক(cfg_csr_no, cfg_val);
#endif
	}
	যদি (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, fixed_ctr);
	প্রত্যাগম fixed_ctr;
}

স্থির পূর্ণাঙ্ক pmu_ctr_find_fixed_hw(unsigned long evt_idx_code)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	যদি (evt_idx_code == SBI_PMU_HW_CPU_CYCLES)
		প্রত্যাগম 0;
	অন্যথা যদি (evt_idx_code == SBI_PMU_HW_INSTRUCTIONS)
		প্রত্যাগম 2;
	অন্যথা
		প্রত্যাগম SBI_EINVAL;
}

স্থির পূর্ণাঙ্ক pmu_ctr_find_hw(সংরচনা sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   unsigned long flags,
			   unsigned long event_idx, uint64_t data)
{
	unsigned long ctr_mask;
	পূর্ণাঙ্ক i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	সংরচনা sbi_pmu_hw_event *temp;
	unsigned long mctr_inhbt = 0;
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	যদি (cbase >= অঙ্ক_hw_ctrs)
		প্রত্যাগম SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_find_fixed_hw(event_idx);
	যদি (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		প্রত্যাগম pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);

	যদি (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	প্রতি (i = 0; i < অঙ্ক_hw_events; i++) {
		temp = &hw_event_map[i];
		যদি ((temp->start_idx > event_idx && event_idx < temp->end_idx) ||
		    (temp->start_idx < event_idx && event_idx > temp->end_idx))
			অনুবর্ত;

		/* For raw events, event data is used as the select value */
		যদি (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_mask = temp->select_mask;

			/* The non-event map bits of data should match the selector */
			যদি (temp->select != (data & select_mask))
				অনুবর্ত;
		}
		/* Fixed counters should not be part of the search */
		ctr_mask = temp->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_MASK);
		for_each_set_bit_from(cbase, &ctr_mask, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			যদি (phs->active_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				অনুবর্ত;
			/* If mcountinhibit is supported, the bit must be enabled */
			যদি ((sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				অনুবর্ত;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			ভঙ্গ;
		}
	}

	যদি (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		যদি (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			প্রত্যাগম SBI_EFAIL;

		প্রত্যাগম pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(temp, ctr_idx, flags, event_idx, data);

	যদি (!ret)
		ret = ctr_idx;

	প্রত্যাগম ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
স্থির পূর্ণাঙ্ক pmu_ctr_find_fw(সংরচনা sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   uint32_t event_code, uint64_t edata)
{
	পূর্ণাঙ্ক i, cidx;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		যদি (cidx < অঙ্ক_hw_ctrs || total_ctrs <= cidx)
			অনুবর্ত;
		যদি (phs->active_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			অনুবর্ত;
		যদি (SBI_PMU_FW_PLATFORM == event_code &&
		    pmu_dev && pmu_dev->fw_counter_match_encoding) {
			যদি (!pmu_dev->fw_counter_match_encoding(phs->hartid,
							    cidx - অঙ্ক_hw_ctrs,
							    edata))
				অনুবর্ত;
		}

		প্রত্যাগম cidx;
	}

	প্রত্যাগম SBI_ENOTSUPP;
}

পূর্ণাঙ্ক sbi_pmu_ctr_cfg_match(unsigned long cidx_base, unsigned long cidx_mask,
			  unsigned long flags, unsigned long event_idx,
			  uint64_t event_data)
{
	সংরচনা sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	যদি (unlikely(!phs))
		প্রত্যাগম SBI_EINVAL;

	পূর্ণাঙ্ক ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_code;

	যদি (!pmu_ctr_idx_validate(cidx_base, cidx_mask))
		প্রত্যাগম SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_data);
	যদি (event_type < 0)
		প্রত্যাগম SBI_EINVAL;
	event_code = get_cidx_code(event_idx);

	যদি (flags & SBI_PMU_CFG_FLAG_SKIP_MATCH) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		unsigned long cidx_first = cidx_base + sbi_ffs(cidx_mask);

		যদি (cidx_first >= total_ctrs)
			প্রত্যাগম SBI_EINVAL;

		যদি (phs->active_events[cidx_first] == SBI_PMU_EVENT_IDX_INVALID)
			প্রত্যাগম SBI_EINVAL;
		ctr_idx = cidx_first;
		গচ্ছ skip_match;
	}

	যদি (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_find_fw(phs, cidx_base, cidx_mask,
					  event_code, event_data);
		যদি ((event_code == SBI_PMU_FW_PLATFORM) && (ctr_idx >= অঙ্ক_hw_ctrs))
			phs->fw_counters_data[ctr_idx - অঙ্ক_hw_ctrs] =
								event_data;
	} অন্যথা {
		ctr_idx = pmu_ctr_find_hw(phs, cidx_base, cidx_mask, flags,
					  event_idx, event_data);
		যদি (ctr_idx >= 0) {
			সংরচনা sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_data = event_data;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_MASK;
		}
	}

	যদি (ctr_idx < 0)
		প্রত্যাগম SBI_ENOTSUPP;

	phs->active_events[ctr_idx] = event_idx;
skip_match:
	যদি (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		যদি (flags & SBI_PMU_CFG_FLAG_CLEAR_মান)
			pmu_ctr_write_hw(ctr_idx, 0);
		যদি (flags & SBI_PMU_CFG_FLAG_AUTO_START)
			pmu_ctr_start_hw(ctr_idx, 0, false);
	} অন্যথা যদি (event_type == SBI_PMU_EVENT_TYPE_FW) {
		যদি (flags & SBI_PMU_CFG_FLAG_CLEAR_মান)
			phs->fw_counters_data[ctr_idx - অঙ্ক_hw_ctrs] = 0;
		যদি (flags & SBI_PMU_CFG_FLAG_AUTO_START) {
			যদি (SBI_PMU_FW_PLATFORM == event_code &&
			    pmu_dev && pmu_dev->fw_counter_start) {
				ret = pmu_dev->fw_counter_start(
					phs->hartid,
					ctr_idx - অঙ্ক_hw_ctrs, event_data);
				যদি (ret)
					প্রত্যাগম ret;
			}
			phs->fw_counters_started |= BIT(ctr_idx - অঙ্ক_hw_ctrs);
		}
	}

	প্রত্যাগম ctr_idx;
}

পূর্ণাঙ্ক sbi_pmu_ctr_incr_fw(গণনা sbi_pmu_fw_event_code_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = রিক্ত;
	সংরচনা sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	যদি (unlikely(!phs))
		প্রত্যাগম 0;

	যদি (likely(!phs->fw_counters_started))
		প্রত্যাগম 0;

	যদি (unlikely(fw_id >= SBI_PMU_FW_MAX))
		প্রত্যাগম SBI_EINVAL;

	প্রতি (cidx = অঙ্ক_hw_ctrs; cidx < total_ctrs; cidx++) {
		যদি (get_cidx_code(phs->active_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & BIT(cidx - অঙ্ক_hw_ctrs))) {
			fcounter = &phs->fw_counters_data[cidx - অঙ্ক_hw_ctrs];
			ভঙ্গ;
		}
	}

	যদি (fcounter)
		(*fcounter)++;

	প্রত্যাগম 0;
}

unsigned long sbi_pmu_অঙ্ক_ctr(শূন্য)
{
	প্রত্যাগম (অঙ্ক_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

পূর্ণাঙ্ক sbi_pmu_ctr_get_info(uint32_t cidx, unsigned long *ctr_info)
{
	পূর্ণাঙ্ক width;
	সঙ্ঘ sbi_pmu_ctr_info cinfo = {0};
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long counter_mask = (unsigned long)sbi_hart_mhpm_mask(scratch) |
				     SBI_PMU_CY_IR_MASK;

	/* Sanity check */
	যদি (cidx >= total_ctrs)
		প্রত্যাগম SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	যদি (cidx < অঙ্ক_hw_ctrs) {
		যদি (!(__test_bit(cidx, &counter_mask)))
			প্রত্যাগম SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_CYCLE + cidx;
		/* mcycle & minstret are always 64 bit */
		যদি (cidx == 0 || cidx == 2)
			cinfo.width = 63;
		অন্যথা
			cinfo.width = sbi_hart_mhpm_bits(scratch) - 1;
	} অন্যথা {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.width = 63;
		যদি (pmu_dev && pmu_dev->fw_counter_width) {
			width = pmu_dev->fw_counter_width();
			যদি (width)
				cinfo.width = width - 1;
		}
	}

	*ctr_info = cinfo.মান;

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক sbi_pmu_event_get_info(unsigned long shmem_phys_lo, unsigned long shmem_phys_hi,
			   unsigned long অঙ্ক_events, unsigned long flags)
{
	unsigned long shmem_size = অঙ্ক_events * পরিমাণ(সংরচনা sbi_pmu_event_info);
	পূর্ণাঙ্ক i, j, event_type;
	সংরচনা sbi_pmu_event_info *einfo;
	সংরচনা sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	সংরচনা sbi_pmu_hw_event *temp;
	bool found = false;

	যদি (flags != 0)
		প্রত্যাগম SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	যদি (!অঙ্ক_events || (shmem_phys_lo & 0xF))
		প্রত্যাগম SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	যদি (shmem_phys_hi)
		প্রত্যাগম SBI_EINVALID_ADDR;

	যদি (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_size, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		প্রত্যাগম SBI_ERR_INVALID_ADDRESS;

	sbi_hart_protection_temp_map_range(shmem_phys_lo, shmem_size);

	einfo = (সংরচনা sbi_pmu_event_info *)(shmem_phys_lo);
	প্রতি (i = 0; i < অঙ্ক_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		যদি (event_idx & SBI_PMU_EVENT_IDX_MBZ_MASK)
			প্রত্যাগম SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_data);
		যদি (event_type < 0) {
			einfo[i].output = 0;
		} অন্যথা যদি (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} অন্যথা {
			প্রতি (j = 0; j < অঙ্ক_hw_events; j++) {
				temp = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				যদি (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					যদি (temp->start_idx > event_idx ||
					    event_idx > temp->end_idx)
						অনুবর্ত;
					/* just match the selector */
					যদি (temp->select == (einfo[i].event_data &
									temp->select_mask)) {
						found = true;
						ভঙ্গ;
					}
				} অন্যথা যদি (temp->start_idx <= event_idx &&
					   event_idx <= temp->end_idx) {
					found = true;
					ভঙ্গ;
				}
			}
			যদি (found)
				einfo[i].output = 1;
			অন্যথা
				einfo[i].output = 0;
			found = false;
		}
	}

	sbi_hart_protection_temp_unmap_range(shmem_phys_lo, shmem_size);

	প্রত্যাগম 0;
}

স্থির শূন্য pmu_reset_event_map(সংরচনা sbi_pmu_hart_state *phs)
{
	পূর্ণাঙ্ক j;

	/* Initialize the counter to event mapping table */
	প্রতি (j = 3; j < total_ctrs; j++)
		phs->active_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	প্রতি (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_data[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

ধ্রুব সংরচনা sbi_pmu_device *sbi_pmu_get_device(শূন্য)
{
	প্রত্যাগম pmu_dev;
}

শূন্য sbi_pmu_set_device(ধ্রুব সংরচনা sbi_pmu_device *dev)
{
	যদি (!dev || pmu_dev)
		প্রত্যাগম;

	pmu_dev = dev;
}

শূন্য sbi_pmu_exit(সংরচনা sbi_scratch *scratch)
{
	সংরচনা sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	যদি (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	যদি (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	যদি (unlikely(!phs))
		প্রত্যাগম;

	pmu_reset_event_map(phs);
}

স্থির শূন্য pmu_sse_enable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_set(CSR_MIE, irq_mask);
}

স্থির শূন্য pmu_sse_disable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_clear(CSR_MIE, irq_mask);
	csr_clear(CSR_MIP, irq_mask);
}

স্থির শূন্য pmu_sse_complete(uint32_t event_id)
{
	csr_set(CSR_MIE, sbi_pmu_irq_mask());
}

স্থির শূন্য pmu_sse_register(uint32_t event_id)
{
	সংরচনা sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_mask());
}

স্থির শূন্য pmu_sse_unregister(uint32_t event_id)
{
	সংরচনা sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_set(CSR_MIDELEG, sbi_pmu_irq_mask());
}

স্থির ধ্রুব সংরচনা sbi_sse_cb_ops pmu_sse_cb_ops = {
	.register_cb = pmu_sse_register,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

পূর্ণাঙ্ক sbi_pmu_init(সংরচনা sbi_scratch *scratch, bool cold_boot)
{
	পূর্ণাঙ্ক hpm_গুনতি = sbi_fls(sbi_hart_mhpm_mask(scratch));
	সংরচনা sbi_pmu_hart_state *phs;
	ধ্রুব সংরচনা sbi_platform *plat;
	পূর্ণাঙ্ক rc;

	যদি (cold_boot) {
		hw_event_map = sbi_calloc(পরিমাণ(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		যদি (!hw_event_map)
			প্রত্যাগম SBI_ENOMEM;

		phs_ptr_offset = sbi_scratch_alloc_type_offset(শূন্য *);
		যদি (!phs_ptr_offset) {
			sbi_free(hw_event_map);
			প্রত্যাগম SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		যদি (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		যদি (!hpm_গুনতি)
			/* Only CY, TM & IR are implemented in the hw */
			অঙ্ক_hw_ctrs = 3;
		অন্যথা
			অঙ্ক_hw_ctrs = hpm_গুনতি + 1;

		যদি (অঙ্ক_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			প্রত্যাগম SBI_EINVAL;

		total_ctrs = অঙ্ক_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		যদি (sbi_pmu_irq_bit() >= 0)
			sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	যদি (!phs) {
		phs = sbi_zalloc(পরিমাণ(*phs));
		যদি (!phs)
			প্রত্যাগম SBI_ENOMEM;
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

	প্রত্যাগম 0;
}
