/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_hart_protection.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_pmu.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi/sbi_sse.h>

/** Information about hardware counters */
கட்டமைப்பு sbi_pmu_hw_event {
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
ஒன்றிப்பு sbi_pmu_ctr_info {
	unsigned long மதிப்பு;
	கட்டமைப்பு {
		unsigned long csr:12;
		unsigned long width:6;
#எனில் __riscv_xlen == 32
		unsigned long reserved:13;
#இல்லையெனில்
		unsigned long reserved:45;
#endif
		unsigned long type:1;
	};
};

#எனில் SBI_PMU_FW_CTR_MAX >= BITS_PER_LONG
#error "Can't handle firmware counters beyond BITS_PER_LONG"
#endif

/** HW event configuration parameters */
கட்டமைப்பு sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_data;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
கட்டமைப்பு sbi_pmu_hart_state {
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
	கட்டமைப்பு sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
நிலையான unsigned long phs_ptr_offset;

#define pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_offset ? sbi_scratch_read_type((__scratch), வெறுமை *, phs_ptr_offset) : வெற்று

#define pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define pmu_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), வெறுமை *, phs_ptr_offset, (__phs))

/* Platform specific PMU device */
நிலையான மாறிலி கட்டமைப்பு sbi_pmu_device *pmu_dev = வெற்று;

/* Mapping between event range and possible counters  */
நிலையான கட்டமைப்பு sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
நிலையான uint32_t இலக்கம்_hw_events;
/* Maximum number of hardware counters available */
நிலையான uint32_t இலக்கம்_hw_ctrs;

/* Maximum number of counters available */
நிலையான uint32_t total_ctrs;

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
நிலையான bool pmu_event_range_overlap(கட்டமைப்பு sbi_pmu_hw_event *evtA,
				    கட்டமைப்பு sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	எனில் (((evtA->end_idx < evtB->start_idx) && (evtA->end_idx < evtB->end_idx)) ||
	   ((evtA->start_idx > evtB->start_idx) && (evtA->start_idx > evtB->end_idx)))
		திரும்பு false;
	திரும்பு true;
}

நிலையான bool pmu_event_select_overlap(கட்டமைப்பு sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_mask)
{
	எனில் ((evt->select == select_val) && (evt->select_mask == select_mask))
		திரும்பு true;

	திரும்பு false;
}

நிலையான முழு pmu_event_validate(கட்டமைப்பு sbi_pmu_hart_state *phs,
			      unsigned long event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_code = get_cidx_code(event_idx);
	uint32_t event_idx_code_max = -1;
	uint32_t cache_ops_result, cache_ops_id, cache_id;

	தேர்வு(event_idx_type) {
	நிலை SBI_PMU_EVENT_TYPE_HW:
		event_idx_code_max = SBI_PMU_HW_GENERAL_MAX;
		நிறுத்து;
	நிலை SBI_PMU_EVENT_TYPE_FW:
		எனில் ((event_idx_code >= SBI_PMU_FW_MAX &&
		    event_idx_code <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_code > SBI_PMU_FW_PLATFORM)
			திரும்பு SBI_EINVAL;

		எனில் (SBI_PMU_FW_PLATFORM == event_idx_code &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			திரும்பு pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		இல்லையெனில்
			event_idx_code_max = SBI_PMU_FW_MAX;
		நிறுத்து;
	நிலை SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_result = event_idx_code &
					SBI_PMU_EVENT_HW_CACHE_OPS_RESULT;
		cache_ops_id = (event_idx_code &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_MASK) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_OFFSET;
		cache_id = (event_idx_code &
			    SBI_PMU_EVENT_HW_CACHE_ID_MASK) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_OFFSET;
		எனில் ((cache_ops_result < SBI_PMU_HW_CACHE_RESULT_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			திரும்பு event_idx_type;
		இல்லையெனில்
			திரும்பு SBI_EINVAL;
		நிறுத்து;
	நிலை SBI_PMU_EVENT_TYPE_HW_RAW:
	நிலை SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_code_max = 1; // event_idx.code should be zero
		நிறுத்து;
	இயல்பு:
		திரும்பு SBI_EINVAL;
	}

	எனில் (event_idx_code < event_idx_code_max)
		திரும்பு event_idx_type;

	திரும்பு SBI_EINVAL;
}

நிலையான முழு pmu_ctr_validate(கட்டமைப்பு sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_code)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	எனில் (cidx >= total_ctrs)
		திரும்பு SBI_EINVAL;

	event_idx_val = phs->active_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	எனில் (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		திரும்பு SBI_EINVAL;

	*event_idx_code = get_cidx_code(event_idx_val);

	திரும்பு event_idx_type;
}

நிலையான bool pmu_ctr_idx_validate(unsigned long cbase, unsigned long cmask)
{
	unsigned long last;

	எனில் (!cmask)
		திரும்பு false;

	last = sbi_fls(cmask);
	எனில் (cbase > -1UL - last)
		திரும்பு false;

	திரும்பு (cbase + last) < total_ctrs;
}

முழு sbi_pmu_ctr_fw_read(unsigned long cidx, uint64_t *cval, bool உயர்வு_bits)
{
	முழு event_idx_type;
	uint32_t event_code;
	கட்டமைப்பு sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	எனில் (unlikely(!phs))
		திரும்பு SBI_EINVAL;

	எனில் (cidx < இலக்கம்_hw_ctrs || cidx >= total_ctrs)
		திரும்பு SBI_EINVAL;

#எனில் __riscv_xlen > 32
	எனில் (உயர்வு_bits)
		திரும்பு 0;
#endif

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
	எனில் (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		திரும்பு SBI_EINVAL;

	எனில் ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		திரும்பு SBI_EINVAL;

	எனில் (SBI_PMU_FW_PLATFORM == event_code) {
		எனில் (pmu_dev && pmu_dev->fw_counter_read_மதிப்பு)
			*cval = pmu_dev->fw_counter_read_மதிப்பு(phs->hartid,
							       cidx -
							       இலக்கம்_hw_ctrs);
		இல்லையெனில்
			*cval = 0;
	} இல்லையெனில்
		*cval = phs->fw_counters_data[cidx - இலக்கம்_hw_ctrs];

	திரும்பு 0;
}

நிலையான முழு pmu_add_hw_event_map(u32 eidx_start, u32 eidx_end, u32 cmap,
				uint64_t select, uint64_t select_mask)
{
	முழு i = 0;
	bool is_overlap;
	கட்டமைப்பு sbi_pmu_hw_event *event = &hw_event_map[இலக்கம்_hw_events];
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_mask = sbi_hart_mhpm_mask(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	எனில் (eidx_start > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_MASK))
		திரும்பு SBI_EDENIED;

	எனில் (இலக்கம்_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		திரும்பு SBI_EFAIL;
	}

	event->start_idx = eidx_start;
	event->end_idx = eidx_end;

	/* Sanity check */
	க்கு (i = 0; i < இலக்கம்_hw_events; i++) {
		எனில் (eidx_start == SBI_PMU_EVENT_RAW_IDX ||
			eidx_start == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_mask);
		இல்லையெனில்
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		எனில் (is_overlap)
			செல் reset_event;
	}

	event->select_mask = select_mask;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_mask;
	event->select = select;
	இலக்கம்_hw_events++;

	திரும்பு 0;

reset_event:
	event->start_idx = 0;
	event->end_idx = 0;
	திரும்பு SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
முழு sbi_pmu_add_hw_event_counter_map(u32 eidx_start, u32 eidx_end, u32 cmap)
{
	எனில் ((eidx_start > eidx_end) || eidx_start >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_end >= SBI_PMU_EVENT_RAW_IDX)
		திரும்பு SBI_EINVAL;

	திரும்பு pmu_add_hw_event_map(eidx_start, eidx_end, cmap, 0, 0);
}

முழு sbi_pmu_add_raw_event_counter_map(uint64_t select, uint64_t select_mask, u32 cmap)
{
	திரும்பு pmu_add_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_mask);
}

வெறுமை sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_mask());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW);
}

நிலையான முழு pmu_ctr_enable_irq_hw(முழு ctr_idx)
{
	unsigned long mhpmevent_csr;
	unsigned long mhpmevent_curr;
	unsigned long mip_val;
	unsigned long of_mask;

	எனில் (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		திரும்பு SBI_EFAIL;

#எனில் __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	of_mask = (uint32_t)~MHPMEVENTH_OF;
#இல்லையெனில்
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	of_mask = ~MHPMEVENT_OF;
#endif

	mhpmevent_curr = csr_read_இலக்கம்(mhpmevent_csr);
	mip_val = csr_read(CSR_MIP);
	/**
	 * Clear out the OF bit so that next interrupt can be enabled.
	 * This should be done only when the corresponding overflow interrupt
	 * bit is cleared. That indicates that software has already handled the
	 * previous interrupts or the hardware yet to set an overflow interrupt.
	 * Otherwise, there will be race conditions where we may clear the bit
	 * the software is yet to handle the interrupt.
	 */
	எனில் (!(mip_val & sbi_pmu_irq_mask())) {
		mhpmevent_curr &= of_mask;
		csr_write_இலக்கம்(mhpmevent_csr, mhpmevent_curr);
	}

	திரும்பு 0;
}

நிலையான வெறுமை pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#எனில் __riscv_xlen == 32
	csr_write_இலக்கம்(CSR_MCYCLE + cidx, 0);
	csr_write_இலக்கம்(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_இலக்கம்(CSR_MCYCLEH + cidx, ival >> BITS_PER_LONG);
#இல்லையெனில்
	csr_write_இலக்கம்(CSR_MCYCLE + cidx, ival);
#endif
}

நிலையான முழு pmu_ctr_start_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	எனில் (cidx >= இலக்கம்_hw_ctrs || cidx == 1)
		திரும்பு SBI_EINVAL;

	எனில் (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11) {
		எனில் (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		திரும்பு 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	எனில் (!__test_bit(cidx, &mctr_inhbt))
		திரும்பு SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	எனில் (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	எனில் (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	திரும்பு 0;
}

முழு sbi_pmu_irq_bit(வெறுமை)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		திரும்பு IRQ_PMU_OVF;
	எனில் (pmu_dev && pmu_dev->hw_counter_irq_bit)
		திரும்பு pmu_dev->hw_counter_irq_bit();

	திரும்பு -1;
}

unsigned long sbi_pmu_irq_mask(வெறுமை)
{
	முழு irq_bit = sbi_pmu_irq_bit();

	எனில் (irq_bit < 0)
		திரும்பு 0;

	திரும்பு BIT(irq_bit);
}

நிலையான முழு pmu_ctr_start_fw(கட்டமைப்பு sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_code,
			    uint64_t event_data, uint64_t ival,
			    bool ival_update)
{
	முழு ret;

	எனில் ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		திரும்பு SBI_EINVAL;

	எனில் (phs->fw_counters_started & BIT(cidx - இலக்கம்_hw_ctrs))
		திரும்பு SBI_EALREADY_STARTED;

	எனில் (SBI_PMU_FW_PLATFORM == event_code) {
		எனில் (!pmu_dev ||
		    !pmu_dev->fw_counter_write_மதிப்பு ||
		    !pmu_dev->fw_counter_start) {
			திரும்பு SBI_EINVAL;
		}

		எனில் (ival_update)
			pmu_dev->fw_counter_write_மதிப்பு(phs->hartid,
							cidx - இலக்கம்_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_start(phs->hartid,
						 cidx - இலக்கம்_hw_ctrs,
						 event_data);
		எனில் (ret)
			திரும்பு ret;
	} இல்லையெனில் {
		எனில் (ival_update)
			phs->fw_counters_data[cidx - இலக்கம்_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= BIT(cidx - இலக்கம்_hw_ctrs);

	திரும்பு 0;
}

நிலையான வெறுமை pmu_update_inhibit_flags(unsigned long flags, uint64_t *mhpmevent_val)
{
	எனில் (flags & SBI_PMU_CFG_FLAG_SET_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	எனில் (flags & SBI_PMU_CFG_FLAG_SET_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	எனில் (flags & SBI_PMU_CFG_FLAG_SET_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	எனில் (flags & SBI_PMU_CFG_FLAG_SET_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

நிலையான முழு pmu_update_hw_mhpmevent(கட்டமைப்பு sbi_pmu_hw_event *hw_evt, முழு ctr_idx,
				   unsigned long flags, unsigned long eindex,
				   uint64_t data)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	மாறிலி கட்டமைப்பு sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_to_mhpmevent(plat, eindex, data);

	எனில் (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		திரும்பு SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_MASK) |
				 MHPMEVENT_MINH | MHPMEVENT_OF;

	எனில் (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	எனில் (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, ctr_idx);

#எனில் __riscv_xlen == 32
	csr_write_இலக்கம்(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_இலக்கம்(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> BITS_PER_LONG);
#இல்லையெனில்
	csr_write_இலக்கம்(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#endif

	திரும்பு 0;
}

முழு sbi_pmu_ctr_start(unsigned long cbase, unsigned long cmask,
		      unsigned long flags, uint64_t ival)
{
	கட்டமைப்பு sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	எனில் (unlikely(!phs))
		திரும்பு SBI_EINVAL;

	முழு event_idx_type;
	uint32_t event_code;
	முழு ret = SBI_EINVAL;
	bool bUpdate = false;
	முழு i, cidx;
	uint64_t edata;

	எனில் (!pmu_ctr_idx_validate(cbase, cmask))
		திரும்பு ret;

	எனில் (flags & ~SBI_PMU_START_FLAGS_MASK)
		திரும்பு SBI_ERR_INVALID_PARAM;

	எனில் (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		திரும்பு SBI_ENO_SHMEM;

	எனில் (flags & SBI_PMU_START_FLAG_SET_INIT_மதிப்பு)
		bUpdate = true;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		எனில் (event_idx_type < 0)
			/* Continue the start operation for other counters */
			தொடர்;
		இல்லையெனில் எனில் (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_code == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_data[cidx - இலக்கம்_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_start_fw(phs, cidx, event_code, edata,
					       ival, bUpdate);
			எனில் (ret)
				திரும்பு ret;
		} இல்லையெனில் {
			எனில் (cidx >= 3) {
				கட்டமைப்பு sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->active_events[cidx],
							ev_cfg->event_data);
				எனில் (ret)
					திரும்பு ret;
			}
			ret = pmu_ctr_start_hw(cidx, ival, bUpdate);
			எனில் (ret)
				திரும்பு ret;
		}
	}

	திரும்பு ret;
}

நிலையான முழு pmu_ctr_stop_hw(uint32_t cidx)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	எனில் (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11)
		திரும்பு 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	எனில் (cidx >= இலக்கம்_hw_ctrs || cidx == 1)
		திரும்பு SBI_EINVAL;

	எனில் (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		எனில் (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		திரும்பு 0;
	} இல்லையெனில்
		திரும்பு SBI_EALREADY_STOPPED;
}

நிலையான முழு pmu_ctr_stop_fw(கட்டமைப்பு sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_code)
{
	முழு ret;

	எனில் ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		திரும்பு SBI_EINVAL;

	எனில் (!(phs->fw_counters_started & BIT(cidx - இலக்கம்_hw_ctrs)))
		திரும்பு SBI_EALREADY_STOPPED;

	எனில் (SBI_PMU_FW_PLATFORM == event_code &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - இலக்கம்_hw_ctrs);
		எனில் (ret)
			திரும்பு ret;
	}

	phs->fw_counters_started &= ~BIT(cidx - இலக்கம்_hw_ctrs);

	திரும்பு 0;
}

நிலையான முழு pmu_reset_hw_mhpmevent(முழு ctr_idx)
{
	எனில் (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		திரும்பு SBI_EFAIL;
#எனில் __riscv_xlen == 32
	csr_write_இலக்கம்(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	எனில் (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_இலக்கம்(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#இல்லையெனில்
	csr_write_இலக்கம்(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#endif

	திரும்பு 0;
}

முழு sbi_pmu_ctr_stop(unsigned long cbase, unsigned long cmask,
		     unsigned long flag)
{
	கட்டமைப்பு sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	எனில் (unlikely(!phs))
		திரும்பு SBI_EINVAL;

	முழு ret = SBI_EINVAL;
	முழு event_idx_type;
	uint32_t event_code;
	முழு i, cidx;

	எனில் (!pmu_ctr_idx_validate(cbase, cmask))
		திரும்பு ret;

	எனில் (flag & ~SBI_PMU_STOP_FLAGS_MASK)
		திரும்பு SBI_ERR_INVALID_PARAM;

	எனில் (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		திரும்பு SBI_ENO_SHMEM;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		எனில் (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			தொடர்;

		இல்லையெனில் எனில் (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_code);
		இல்லையெனில்
			ret = pmu_ctr_stop_hw(cidx);

		எனில்(ret)
			திரும்பு ret;

		எனில் (cidx > (CSR_INSTRET - CSR_CYCLE) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->active_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	எனில் (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_mask());

	திரும்பு ret;
}

நிலையான முழு pmu_fixed_ctr_update_inhibit_bits(முழு fixed_ctr, unsigned long flags)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#எனில் __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#endif
	எனில் (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_mode))
		திரும்பு fixed_ctr;

	தேர்வு (fixed_ctr) {
	நிலை 0:
		cfg_csr_no = CSR_MCYCLECFG;
#எனில் __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#endif
		நிறுத்து;
	நிலை 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#எனில் __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#endif
		நிறுத்து;
	இயல்பு:
		திரும்பு SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#எனில் __riscv_xlen == 32
		csr_write_இலக்கம்(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_இலக்கம்(cfgh_csr_no, cfg_val >> BITS_PER_LONG);
#இல்லையெனில்
		csr_write_இலக்கம்(cfg_csr_no, cfg_val);
#endif
	}
	எனில் (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, fixed_ctr);
	திரும்பு fixed_ctr;
}

நிலையான முழு pmu_ctr_find_fixed_hw(unsigned long evt_idx_code)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	எனில் (evt_idx_code == SBI_PMU_HW_CPU_CYCLES)
		திரும்பு 0;
	இல்லையெனில் எனில் (evt_idx_code == SBI_PMU_HW_INSTRUCTIONS)
		திரும்பு 2;
	இல்லையெனில்
		திரும்பு SBI_EINVAL;
}

நிலையான முழு pmu_ctr_find_hw(கட்டமைப்பு sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   unsigned long flags,
			   unsigned long event_idx, uint64_t data)
{
	unsigned long ctr_mask;
	முழு i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	கட்டமைப்பு sbi_pmu_hw_event *temp;
	unsigned long mctr_inhbt = 0;
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	எனில் (cbase >= இலக்கம்_hw_ctrs)
		திரும்பு SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_find_fixed_hw(event_idx);
	எனில் (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		திரும்பு pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);

	எனில் (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	க்கு (i = 0; i < இலக்கம்_hw_events; i++) {
		temp = &hw_event_map[i];
		எனில் ((temp->start_idx > event_idx && event_idx < temp->end_idx) ||
		    (temp->start_idx < event_idx && event_idx > temp->end_idx))
			தொடர்;

		/* For raw events, event data is used as the select value */
		எனில் (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_mask = temp->select_mask;

			/* The non-event map bits of data should match the selector */
			எனில் (temp->select != (data & select_mask))
				தொடர்;
		}
		/* Fixed counters should not be part of the search */
		ctr_mask = temp->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_MASK);
		for_each_set_bit_from(cbase, &ctr_mask, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			எனில் (phs->active_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				தொடர்;
			/* If mcountinhibit is supported, the bit must be enabled */
			எனில் ((sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				தொடர்;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			நிறுத்து;
		}
	}

	எனில் (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		எனில் (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			திரும்பு SBI_EFAIL;

		திரும்பு pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(temp, ctr_idx, flags, event_idx, data);

	எனில் (!ret)
		ret = ctr_idx;

	திரும்பு ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
நிலையான முழு pmu_ctr_find_fw(கட்டமைப்பு sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   uint32_t event_code, uint64_t edata)
{
	முழு i, cidx;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		எனில் (cidx < இலக்கம்_hw_ctrs || total_ctrs <= cidx)
			தொடர்;
		எனில் (phs->active_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			தொடர்;
		எனில் (SBI_PMU_FW_PLATFORM == event_code &&
		    pmu_dev && pmu_dev->fw_counter_match_encoding) {
			எனில் (!pmu_dev->fw_counter_match_encoding(phs->hartid,
							    cidx - இலக்கம்_hw_ctrs,
							    edata))
				தொடர்;
		}

		திரும்பு cidx;
	}

	திரும்பு SBI_ENOTSUPP;
}

முழு sbi_pmu_ctr_cfg_match(unsigned long cidx_base, unsigned long cidx_mask,
			  unsigned long flags, unsigned long event_idx,
			  uint64_t event_data)
{
	கட்டமைப்பு sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	எனில் (unlikely(!phs))
		திரும்பு SBI_EINVAL;

	முழு ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_code;

	எனில் (!pmu_ctr_idx_validate(cidx_base, cidx_mask))
		திரும்பு SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_data);
	எனில் (event_type < 0)
		திரும்பு SBI_EINVAL;
	event_code = get_cidx_code(event_idx);

	எனில் (flags & SBI_PMU_CFG_FLAG_SKIP_MATCH) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		unsigned long cidx_first = cidx_base + sbi_ffs(cidx_mask);

		எனில் (cidx_first >= total_ctrs)
			திரும்பு SBI_EINVAL;

		எனில் (phs->active_events[cidx_first] == SBI_PMU_EVENT_IDX_INVALID)
			திரும்பு SBI_EINVAL;
		ctr_idx = cidx_first;
		செல் skip_match;
	}

	எனில் (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_find_fw(phs, cidx_base, cidx_mask,
					  event_code, event_data);
		எனில் ((event_code == SBI_PMU_FW_PLATFORM) && (ctr_idx >= இலக்கம்_hw_ctrs))
			phs->fw_counters_data[ctr_idx - இலக்கம்_hw_ctrs] =
								event_data;
	} இல்லையெனில் {
		ctr_idx = pmu_ctr_find_hw(phs, cidx_base, cidx_mask, flags,
					  event_idx, event_data);
		எனில் (ctr_idx >= 0) {
			கட்டமைப்பு sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_data = event_data;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_MASK;
		}
	}

	எனில் (ctr_idx < 0)
		திரும்பு SBI_ENOTSUPP;

	phs->active_events[ctr_idx] = event_idx;
skip_match:
	எனில் (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		எனில் (flags & SBI_PMU_CFG_FLAG_CLEAR_மதிப்பு)
			pmu_ctr_write_hw(ctr_idx, 0);
		எனில் (flags & SBI_PMU_CFG_FLAG_AUTO_START)
			pmu_ctr_start_hw(ctr_idx, 0, false);
	} இல்லையெனில் எனில் (event_type == SBI_PMU_EVENT_TYPE_FW) {
		எனில் (flags & SBI_PMU_CFG_FLAG_CLEAR_மதிப்பு)
			phs->fw_counters_data[ctr_idx - இலக்கம்_hw_ctrs] = 0;
		எனில் (flags & SBI_PMU_CFG_FLAG_AUTO_START) {
			எனில் (SBI_PMU_FW_PLATFORM == event_code &&
			    pmu_dev && pmu_dev->fw_counter_start) {
				ret = pmu_dev->fw_counter_start(
					phs->hartid,
					ctr_idx - இலக்கம்_hw_ctrs, event_data);
				எனில் (ret)
					திரும்பு ret;
			}
			phs->fw_counters_started |= BIT(ctr_idx - இலக்கம்_hw_ctrs);
		}
	}

	திரும்பு ctr_idx;
}

முழு sbi_pmu_ctr_incr_fw(எண்ணிக்கை sbi_pmu_fw_event_code_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = வெற்று;
	கட்டமைப்பு sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	எனில் (unlikely(!phs))
		திரும்பு 0;

	எனில் (likely(!phs->fw_counters_started))
		திரும்பு 0;

	எனில் (unlikely(fw_id >= SBI_PMU_FW_MAX))
		திரும்பு SBI_EINVAL;

	க்கு (cidx = இலக்கம்_hw_ctrs; cidx < total_ctrs; cidx++) {
		எனில் (get_cidx_code(phs->active_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & BIT(cidx - இலக்கம்_hw_ctrs))) {
			fcounter = &phs->fw_counters_data[cidx - இலக்கம்_hw_ctrs];
			நிறுத்து;
		}
	}

	எனில் (fcounter)
		(*fcounter)++;

	திரும்பு 0;
}

unsigned long sbi_pmu_இலக்கம்_ctr(வெறுமை)
{
	திரும்பு (இலக்கம்_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

முழு sbi_pmu_ctr_get_info(uint32_t cidx, unsigned long *ctr_info)
{
	முழு width;
	ஒன்றிப்பு sbi_pmu_ctr_info cinfo = {0};
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long counter_mask = (unsigned long)sbi_hart_mhpm_mask(scratch) |
				     SBI_PMU_CY_IR_MASK;

	/* Sanity check */
	எனில் (cidx >= total_ctrs)
		திரும்பு SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	எனில் (cidx < இலக்கம்_hw_ctrs) {
		எனில் (!(__test_bit(cidx, &counter_mask)))
			திரும்பு SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_CYCLE + cidx;
		/* mcycle & minstret are always 64 bit */
		எனில் (cidx == 0 || cidx == 2)
			cinfo.width = 63;
		இல்லையெனில்
			cinfo.width = sbi_hart_mhpm_bits(scratch) - 1;
	} இல்லையெனில் {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.width = 63;
		எனில் (pmu_dev && pmu_dev->fw_counter_width) {
			width = pmu_dev->fw_counter_width();
			எனில் (width)
				cinfo.width = width - 1;
		}
	}

	*ctr_info = cinfo.மதிப்பு;

	திரும்பு 0;
}

முழு sbi_pmu_event_get_info(unsigned long shmem_phys_lo, unsigned long shmem_phys_hi,
			   unsigned long இலக்கம்_events, unsigned long flags)
{
	unsigned long shmem_size = இலக்கம்_events * அளவு(கட்டமைப்பு sbi_pmu_event_info);
	முழு i, j, event_type;
	கட்டமைப்பு sbi_pmu_event_info *einfo;
	கட்டமைப்பு sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	கட்டமைப்பு sbi_pmu_hw_event *temp;
	bool found = false;

	எனில் (flags != 0)
		திரும்பு SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	எனில் (!இலக்கம்_events || (shmem_phys_lo & 0xF))
		திரும்பு SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	எனில் (shmem_phys_hi)
		திரும்பு SBI_EINVALID_ADDR;

	எனில் (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_size, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		திரும்பு SBI_ERR_INVALID_ADDRESS;

	sbi_hart_protection_temp_map_range(shmem_phys_lo, shmem_size);

	einfo = (கட்டமைப்பு sbi_pmu_event_info *)(shmem_phys_lo);
	க்கு (i = 0; i < இலக்கம்_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		எனில் (event_idx & SBI_PMU_EVENT_IDX_MBZ_MASK)
			திரும்பு SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_data);
		எனில் (event_type < 0) {
			einfo[i].output = 0;
		} இல்லையெனில் எனில் (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} இல்லையெனில் {
			க்கு (j = 0; j < இலக்கம்_hw_events; j++) {
				temp = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				எனில் (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					எனில் (temp->start_idx > event_idx ||
					    event_idx > temp->end_idx)
						தொடர்;
					/* just match the selector */
					எனில் (temp->select == (einfo[i].event_data &
									temp->select_mask)) {
						found = true;
						நிறுத்து;
					}
				} இல்லையெனில் எனில் (temp->start_idx <= event_idx &&
					   event_idx <= temp->end_idx) {
					found = true;
					நிறுத்து;
				}
			}
			எனில் (found)
				einfo[i].output = 1;
			இல்லையெனில்
				einfo[i].output = 0;
			found = false;
		}
	}

	sbi_hart_protection_temp_unmap_range(shmem_phys_lo, shmem_size);

	திரும்பு 0;
}

நிலையான வெறுமை pmu_reset_event_map(கட்டமைப்பு sbi_pmu_hart_state *phs)
{
	முழு j;

	/* Initialize the counter to event mapping table */
	க்கு (j = 3; j < total_ctrs; j++)
		phs->active_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	க்கு (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_data[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

மாறிலி கட்டமைப்பு sbi_pmu_device *sbi_pmu_get_device(வெறுமை)
{
	திரும்பு pmu_dev;
}

வெறுமை sbi_pmu_set_device(மாறிலி கட்டமைப்பு sbi_pmu_device *dev)
{
	எனில் (!dev || pmu_dev)
		திரும்பு;

	pmu_dev = dev;
}

வெறுமை sbi_pmu_exit(கட்டமைப்பு sbi_scratch *scratch)
{
	கட்டமைப்பு sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	எனில் (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	எனில் (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	எனில் (unlikely(!phs))
		திரும்பு;

	pmu_reset_event_map(phs);
}

நிலையான வெறுமை pmu_sse_enable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_set(CSR_MIE, irq_mask);
}

நிலையான வெறுமை pmu_sse_disable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_clear(CSR_MIE, irq_mask);
	csr_clear(CSR_MIP, irq_mask);
}

நிலையான வெறுமை pmu_sse_complete(uint32_t event_id)
{
	csr_set(CSR_MIE, sbi_pmu_irq_mask());
}

நிலையான வெறுமை pmu_sse_register(uint32_t event_id)
{
	கட்டமைப்பு sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_mask());
}

நிலையான வெறுமை pmu_sse_unregister(uint32_t event_id)
{
	கட்டமைப்பு sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_set(CSR_MIDELEG, sbi_pmu_irq_mask());
}

நிலையான மாறிலி கட்டமைப்பு sbi_sse_cb_ops pmu_sse_cb_ops = {
	.register_cb = pmu_sse_register,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

முழு sbi_pmu_init(கட்டமைப்பு sbi_scratch *scratch, bool cold_boot)
{
	முழு hpm_மொத்தம் = sbi_fls(sbi_hart_mhpm_mask(scratch));
	கட்டமைப்பு sbi_pmu_hart_state *phs;
	மாறிலி கட்டமைப்பு sbi_platform *plat;
	முழு rc;

	எனில் (cold_boot) {
		hw_event_map = sbi_calloc(அளவு(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		எனில் (!hw_event_map)
			திரும்பு SBI_ENOMEM;

		phs_ptr_offset = sbi_scratch_alloc_type_offset(வெறுமை *);
		எனில் (!phs_ptr_offset) {
			sbi_free(hw_event_map);
			திரும்பு SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		எனில் (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		எனில் (!hpm_மொத்தம்)
			/* Only CY, TM & IR are implemented in the hw */
			இலக்கம்_hw_ctrs = 3;
		இல்லையெனில்
			இலக்கம்_hw_ctrs = hpm_மொத்தம் + 1;

		எனில் (இலக்கம்_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			திரும்பு SBI_EINVAL;

		total_ctrs = இலக்கம்_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		எனில் (sbi_pmu_irq_bit() >= 0)
			sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	எனில் (!phs) {
		phs = sbi_zalloc(அளவு(*phs));
		எனில் (!phs)
			திரும்பு SBI_ENOMEM;
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

	திரும்பு 0;
}
