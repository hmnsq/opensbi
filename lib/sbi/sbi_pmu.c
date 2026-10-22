/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitops.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart_protection.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_pmu.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_sse.h>

/** Information about hardware counters */
ସଂରଚନା sbi_pmu_hw_event {
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
ସଂଘ sbi_pmu_ctr_info {
	unsigned long ମୂଲ୍ୟ;
	ସଂରଚନା {
		unsigned long csr:12;
		unsigned long width:6;
#ଯଦି __riscv_xlen == 32
		unsigned long reserved:13;
#ଅନ୍ୟଥା
		unsigned long reserved:45;
#endif
		unsigned long type:1;
	};
};

#ଯଦି SBI_PMU_FW_CTR_MAX >= BITS_PER_LONG
#error "Can't handle firmware counters beyond BITS_PER_LONG"
#endif

/** HW event configuration parameters */
ସଂରଚନା sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_data;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
ସଂରଚନା sbi_pmu_hart_state {
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
	ସଂରଚନା sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
ସ୍ଥିର unsigned long phs_ptr_offset;

#define pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_offset ? sbi_scratch_read_type((__scratch), ଶୂନ୍ୟ *, phs_ptr_offset) : ଖାଲି

#define pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define pmu_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), ଶୂନ୍ୟ *, phs_ptr_offset, (__phs))

/* Platform specific PMU device */
ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_pmu_device *pmu_dev = ଖାଲି;

/* Mapping between event range and possible counters  */
ସ୍ଥିର ସଂରଚନା sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
ସ୍ଥିର uint32_t ଅଙ୍କ_hw_events;
/* Maximum number of hardware counters available */
ସ୍ଥିର uint32_t ଅଙ୍କ_hw_ctrs;

/* Maximum number of counters available */
ସ୍ଥିର uint32_t total_ctrs;

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
ସ୍ଥିର bool pmu_event_range_overlap(ସଂରଚନା sbi_pmu_hw_event *evtA,
				    ସଂରଚନା sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	ଯଦି (((evtA->end_idx < evtB->start_idx) && (evtA->end_idx < evtB->end_idx)) ||
	   ((evtA->start_idx > evtB->start_idx) && (evtA->start_idx > evtB->end_idx)))
		ଫେରନ୍ତୁ false;
	ଫେରନ୍ତୁ true;
}

ସ୍ଥିର bool pmu_event_select_overlap(ସଂରଚନା sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_mask)
{
	ଯଦି ((evt->select == select_val) && (evt->select_mask == select_mask))
		ଫେରନ୍ତୁ true;

	ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_event_validate(ସଂରଚନା sbi_pmu_hart_state *phs,
			      unsigned long event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_code = get_cidx_code(event_idx);
	uint32_t event_idx_code_max = -1;
	uint32_t cache_ops_result, cache_ops_id, cache_id;

	ବାଛ(event_idx_type) {
	ସ୍ଥିତି SBI_PMU_EVENT_TYPE_HW:
		event_idx_code_max = SBI_PMU_HW_GENERAL_MAX;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_PMU_EVENT_TYPE_FW:
		ଯଦି ((event_idx_code >= SBI_PMU_FW_MAX &&
		    event_idx_code <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_code > SBI_PMU_FW_PLATFORM)
			ଫେରନ୍ତୁ SBI_EINVAL;

		ଯଦି (SBI_PMU_FW_PLATFORM == event_idx_code &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			ଫେରନ୍ତୁ pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		ଅନ୍ୟଥା
			event_idx_code_max = SBI_PMU_FW_MAX;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_result = event_idx_code &
					SBI_PMU_EVENT_HW_CACHE_OPS_RESULT;
		cache_ops_id = (event_idx_code &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_MASK) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_OFFSET;
		cache_id = (event_idx_code &
			    SBI_PMU_EVENT_HW_CACHE_ID_MASK) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_OFFSET;
		ଯଦି ((cache_ops_result < SBI_PMU_HW_CACHE_RESULT_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			ଫେରନ୍ତୁ event_idx_type;
		ଅନ୍ୟଥା
			ଫେରନ୍ତୁ SBI_EINVAL;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_PMU_EVENT_TYPE_HW_RAW:
	ସ୍ଥିତି SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_code_max = 1; // event_idx.code should be zero
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ SBI_EINVAL;
	}

	ଯଦି (event_idx_code < event_idx_code_max)
		ଫେରନ୍ତୁ event_idx_type;

	ଫେରନ୍ତୁ SBI_EINVAL;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_ctr_validate(ସଂରଚନା sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_code)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	ଯଦି (cidx >= total_ctrs)
		ଫେରନ୍ତୁ SBI_EINVAL;

	event_idx_val = phs->active_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	ଯଦି (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		ଫେରନ୍ତୁ SBI_EINVAL;

	*event_idx_code = get_cidx_code(event_idx_val);

	ଫେରନ୍ତୁ event_idx_type;
}

ସ୍ଥିର bool pmu_ctr_idx_validate(unsigned long cbase, unsigned long cmask)
{
	unsigned long last;

	ଯଦି (!cmask)
		ଫେରନ୍ତୁ false;

	last = sbi_fls(cmask);
	ଯଦି (cbase > -1UL - last)
		ଫେରନ୍ତୁ false;

	ଫେରନ୍ତୁ (cbase + last) < total_ctrs;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_pmu_ctr_fw_read(unsigned long cidx, uint64_t *cval, bool ଉପର_bits)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା event_idx_type;
	uint32_t event_code;
	ସଂରଚନା sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ଯଦି (unlikely(!phs))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (cidx < ଅଙ୍କ_hw_ctrs || cidx >= total_ctrs)
		ଫେରନ୍ତୁ SBI_EINVAL;

#ଯଦି __riscv_xlen > 32
	ଯଦି (ଉପର_bits)
		ଫେରନ୍ତୁ 0;
#endif

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
	ଯଦି (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (SBI_PMU_FW_PLATFORM == event_code) {
		ଯଦି (pmu_dev && pmu_dev->fw_counter_read_ମୂଲ୍ୟ)
			*cval = pmu_dev->fw_counter_read_ମୂଲ୍ୟ(phs->hartid,
							       cidx -
							       ଅଙ୍କ_hw_ctrs);
		ଅନ୍ୟଥା
			*cval = 0;
	} ଅନ୍ୟଥା
		*cval = phs->fw_counters_data[cidx - ଅଙ୍କ_hw_ctrs];

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_add_hw_event_map(u32 eidx_start, u32 eidx_end, u32 cmap,
				uint64_t select, uint64_t select_mask)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
	bool is_overlap;
	ସଂରଚନା sbi_pmu_hw_event *event = &hw_event_map[ଅଙ୍କ_hw_events];
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_mask = sbi_hart_mhpm_mask(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	ଯଦି (eidx_start > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_MASK))
		ଫେରନ୍ତୁ SBI_EDENIED;

	ଯଦି (ଅଙ୍କ_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		ଫେରନ୍ତୁ SBI_EFAIL;
	}

	event->start_idx = eidx_start;
	event->end_idx = eidx_end;

	/* Sanity check */
	ପାଇଁ (i = 0; i < ଅଙ୍କ_hw_events; i++) {
		ଯଦି (eidx_start == SBI_PMU_EVENT_RAW_IDX ||
			eidx_start == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_mask);
		ଅନ୍ୟଥା
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		ଯଦି (is_overlap)
			ଯାଅ reset_event;
	}

	event->select_mask = select_mask;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_mask;
	event->select = select;
	ଅଙ୍କ_hw_events++;

	ଫେରନ୍ତୁ 0;

reset_event:
	event->start_idx = 0;
	event->end_idx = 0;
	ଫେରନ୍ତୁ SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_pmu_add_hw_event_counter_map(u32 eidx_start, u32 eidx_end, u32 cmap)
{
	ଯଦି ((eidx_start > eidx_end) || eidx_start >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_end >= SBI_PMU_EVENT_RAW_IDX)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଫେରନ୍ତୁ pmu_add_hw_event_map(eidx_start, eidx_end, cmap, 0, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_pmu_add_raw_event_counter_map(uint64_t select, uint64_t select_mask, u32 cmap)
{
	ଫେରନ୍ତୁ pmu_add_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_mask);
}

ଶୂନ୍ୟ sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_mask());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_ctr_enable_irq_hw(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctr_idx)
{
	unsigned long mhpmevent_csr;
	unsigned long mhpmevent_curr;
	unsigned long mip_val;
	unsigned long of_mask;

	ଯଦି (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		ଫେରନ୍ତୁ SBI_EFAIL;

#ଯଦି __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	of_mask = (uint32_t)~MHPMEVENTH_OF;
#ଅନ୍ୟଥା
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	of_mask = ~MHPMEVENT_OF;
#endif

	mhpmevent_curr = csr_read_ଅଙ୍କ(mhpmevent_csr);
	mip_val = csr_read(CSR_MIP);
	/**
	 * Clear out the OF bit so that next interrupt can be enabled.
	 * This should be done only when the corresponding overflow interrupt
	 * bit is cleared. That indicates that software has already handled the
	 * previous interrupts or the hardware yet to set an overflow interrupt.
	 * Otherwise, there will be race conditions where we may clear the bit
	 * the software is yet to handle the interrupt.
	 */
	ଯଦି (!(mip_val & sbi_pmu_irq_mask())) {
		mhpmevent_curr &= of_mask;
		csr_write_ଅଙ୍କ(mhpmevent_csr, mhpmevent_curr);
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#ଯଦି __riscv_xlen == 32
	csr_write_ଅଙ୍କ(CSR_MCYCLE + cidx, 0);
	csr_write_ଅଙ୍କ(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_ଅଙ୍କ(CSR_MCYCLEH + cidx, ival >> BITS_PER_LONG);
#ଅନ୍ୟଥା
	csr_write_ଅଙ୍କ(CSR_MCYCLE + cidx, ival);
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_ctr_start_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	ଯଦି (cidx >= ଅଙ୍କ_hw_ctrs || cidx == 1)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11) {
		ଯଦି (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		ଫେରନ୍ତୁ 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	ଯଦି (!__test_bit(cidx, &mctr_inhbt))
		ଫେରନ୍ତୁ SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	ଯଦି (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	ଯଦି (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_pmu_irq_bit(ଶୂନ୍ୟ)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		ଫେରନ୍ତୁ IRQ_PMU_OVF;
	ଯଦି (pmu_dev && pmu_dev->hw_counter_irq_bit)
		ଫେରନ୍ତୁ pmu_dev->hw_counter_irq_bit();

	ଫେରନ୍ତୁ -1;
}

unsigned long sbi_pmu_irq_mask(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq_bit = sbi_pmu_irq_bit();

	ଯଦି (irq_bit < 0)
		ଫେରନ୍ତୁ 0;

	ଫେରନ୍ତୁ BIT(irq_bit);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_ctr_start_fw(ସଂରଚନା sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_code,
			    uint64_t event_data, uint64_t ival,
			    bool ival_update)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ଯଦି ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (phs->fw_counters_started & BIT(cidx - ଅଙ୍କ_hw_ctrs))
		ଫେରନ୍ତୁ SBI_EALREADY_STARTED;

	ଯଦି (SBI_PMU_FW_PLATFORM == event_code) {
		ଯଦି (!pmu_dev ||
		    !pmu_dev->fw_counter_write_ମୂଲ୍ୟ ||
		    !pmu_dev->fw_counter_start) {
			ଫେରନ୍ତୁ SBI_EINVAL;
		}

		ଯଦି (ival_update)
			pmu_dev->fw_counter_write_ମୂଲ୍ୟ(phs->hartid,
							cidx - ଅଙ୍କ_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_start(phs->hartid,
						 cidx - ଅଙ୍କ_hw_ctrs,
						 event_data);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	} ଅନ୍ୟଥା {
		ଯଦି (ival_update)
			phs->fw_counters_data[cidx - ଅଙ୍କ_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= BIT(cidx - ଅଙ୍କ_hw_ctrs);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ pmu_update_inhibit_flags(unsigned long flags, uint64_t *mhpmevent_val)
{
	ଯଦି (flags & SBI_PMU_CFG_FLAG_SET_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	ଯଦି (flags & SBI_PMU_CFG_FLAG_SET_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	ଯଦି (flags & SBI_PMU_CFG_FLAG_SET_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	ଯଦି (flags & SBI_PMU_CFG_FLAG_SET_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_update_hw_mhpmevent(ସଂରଚନା sbi_pmu_hw_event *hw_evt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctr_idx,
				   unsigned long flags, unsigned long eindex,
				   uint64_t data)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ଅଚଳ ସଂରଚନା sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_to_mhpmevent(plat, eindex, data);

	ଯଦି (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		ଫେରନ୍ତୁ SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_MASK) |
				 MHPMEVENT_MINH | MHPMEVENT_OF;

	ଯଦି (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	ଯଦି (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, ctr_idx);

#ଯଦି __riscv_xlen == 32
	csr_write_ଅଙ୍କ(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_ଅଙ୍କ(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> BITS_PER_LONG);
#ଅନ୍ୟଥା
	csr_write_ଅଙ୍କ(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#endif

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_pmu_ctr_start(unsigned long cbase, unsigned long cmask,
		      unsigned long flags, uint64_t ival)
{
	ସଂରଚନା sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ଯଦି (unlikely(!phs))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା event_idx_type;
	uint32_t event_code;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = SBI_EINVAL;
	bool bUpdate = false;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, cidx;
	uint64_t edata;

	ଯଦି (!pmu_ctr_idx_validate(cbase, cmask))
		ଫେରନ୍ତୁ ret;

	ଯଦି (flags & ~SBI_PMU_START_FLAGS_MASK)
		ଫେରନ୍ତୁ SBI_ERR_INVALID_PARAM;

	ଯଦି (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		ଫେରନ୍ତୁ SBI_ENO_SHMEM;

	ଯଦି (flags & SBI_PMU_START_FLAG_SET_INIT_ମୂଲ୍ୟ)
		bUpdate = true;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		ଯଦି (event_idx_type < 0)
			/* Continue the start operation for other counters */
			ଜାରି;
		ଅନ୍ୟଥା ଯଦି (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_code == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_data[cidx - ଅଙ୍କ_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_start_fw(phs, cidx, event_code, edata,
					       ival, bUpdate);
			ଯଦି (ret)
				ଫେରନ୍ତୁ ret;
		} ଅନ୍ୟଥା {
			ଯଦି (cidx >= 3) {
				ସଂରଚନା sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->active_events[cidx],
							ev_cfg->event_data);
				ଯଦି (ret)
					ଫେରନ୍ତୁ ret;
			}
			ret = pmu_ctr_start_hw(cidx, ival, bUpdate);
			ଯଦି (ret)
				ଫେରନ୍ତୁ ret;
		}
	}

	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_ctr_stop_hw(uint32_t cidx)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	ଯଦି (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11)
		ଫେରନ୍ତୁ 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	ଯଦି (cidx >= ଅଙ୍କ_hw_ctrs || cidx == 1)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		ଯଦି (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		ଫେରନ୍ତୁ 0;
	} ଅନ୍ୟଥା
		ଫେରନ୍ତୁ SBI_EALREADY_STOPPED;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_ctr_stop_fw(ସଂରଚନା sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_code)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ଯଦି ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (!(phs->fw_counters_started & BIT(cidx - ଅଙ୍କ_hw_ctrs)))
		ଫେରନ୍ତୁ SBI_EALREADY_STOPPED;

	ଯଦି (SBI_PMU_FW_PLATFORM == event_code &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - ଅଙ୍କ_hw_ctrs);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	phs->fw_counters_started &= ~BIT(cidx - ଅଙ୍କ_hw_ctrs);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_reset_hw_mhpmevent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctr_idx)
{
	ଯଦି (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		ଫେରନ୍ତୁ SBI_EFAIL;
#ଯଦି __riscv_xlen == 32
	csr_write_ଅଙ୍କ(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	ଯଦି (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_ଅଙ୍କ(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#ଅନ୍ୟଥା
	csr_write_ଅଙ୍କ(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#endif

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_pmu_ctr_stop(unsigned long cbase, unsigned long cmask,
		     unsigned long flag)
{
	ସଂରଚନା sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ଯଦି (unlikely(!phs))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = SBI_EINVAL;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା event_idx_type;
	uint32_t event_code;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, cidx;

	ଯଦି (!pmu_ctr_idx_validate(cbase, cmask))
		ଫେରନ୍ତୁ ret;

	ଯଦି (flag & ~SBI_PMU_STOP_FLAGS_MASK)
		ଫେରନ୍ତୁ SBI_ERR_INVALID_PARAM;

	ଯଦି (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		ଫେରନ୍ତୁ SBI_ENO_SHMEM;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		ଯଦି (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			ଜାରି;

		ଅନ୍ୟଥା ଯଦି (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_code);
		ଅନ୍ୟଥା
			ret = pmu_ctr_stop_hw(cidx);

		ଯଦି(ret)
			ଫେରନ୍ତୁ ret;

		ଯଦି (cidx > (CSR_INSTRET - CSR_CYCLE) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->active_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	ଯଦି (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_mask());

	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_fixed_ctr_update_inhibit_bits(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fixed_ctr, unsigned long flags)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#ଯଦି __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#endif
	ଯଦି (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_mode))
		ଫେରନ୍ତୁ fixed_ctr;

	ବାଛ (fixed_ctr) {
	ସ୍ଥିତି 0:
		cfg_csr_no = CSR_MCYCLECFG;
#ଯଦି __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#endif
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#ଯଦି __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#endif
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#ଯଦି __riscv_xlen == 32
		csr_write_ଅଙ୍କ(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_ଅଙ୍କ(cfgh_csr_no, cfg_val >> BITS_PER_LONG);
#ଅନ୍ୟଥା
		csr_write_ଅଙ୍କ(cfg_csr_no, cfg_val);
#endif
	}
	ଯଦି (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, fixed_ctr);
	ଫେରନ୍ତୁ fixed_ctr;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_ctr_find_fixed_hw(unsigned long evt_idx_code)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	ଯଦି (evt_idx_code == SBI_PMU_HW_CPU_CYCLES)
		ଫେରନ୍ତୁ 0;
	ଅନ୍ୟଥା ଯଦି (evt_idx_code == SBI_PMU_HW_INSTRUCTIONS)
		ଫେରନ୍ତୁ 2;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ SBI_EINVAL;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_ctr_find_hw(ସଂରଚନା sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   unsigned long flags,
			   unsigned long event_idx, uint64_t data)
{
	unsigned long ctr_mask;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	ସଂରଚନା sbi_pmu_hw_event *temp;
	unsigned long mctr_inhbt = 0;
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ଯଦି (cbase >= ଅଙ୍କ_hw_ctrs)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_find_fixed_hw(event_idx);
	ଯଦି (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		ଫେରନ୍ତୁ pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);

	ଯଦି (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	ପାଇଁ (i = 0; i < ଅଙ୍କ_hw_events; i++) {
		temp = &hw_event_map[i];
		ଯଦି ((temp->start_idx > event_idx && event_idx < temp->end_idx) ||
		    (temp->start_idx < event_idx && event_idx > temp->end_idx))
			ଜାରି;

		/* For raw events, event data is used as the select value */
		ଯଦି (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_mask = temp->select_mask;

			/* The non-event map bits of data should match the selector */
			ଯଦି (temp->select != (data & select_mask))
				ଜାରି;
		}
		/* Fixed counters should not be part of the search */
		ctr_mask = temp->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_MASK);
		for_each_set_bit_from(cbase, &ctr_mask, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			ଯଦି (phs->active_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				ଜାରି;
			/* If mcountinhibit is supported, the bit must be enabled */
			ଯଦି ((sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				ଜାରି;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			ଭାଙ୍ଗ;
		}
	}

	ଯଦି (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		ଯଦି (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			ଫେରନ୍ତୁ SBI_EFAIL;

		ଫେରନ୍ତୁ pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(temp, ctr_idx, flags, event_idx, data);

	ଯଦି (!ret)
		ret = ctr_idx;

	ଫେରନ୍ତୁ ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pmu_ctr_find_fw(ସଂରଚନା sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   uint32_t event_code, uint64_t edata)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, cidx;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		ଯଦି (cidx < ଅଙ୍କ_hw_ctrs || total_ctrs <= cidx)
			ଜାରି;
		ଯଦି (phs->active_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			ଜାରି;
		ଯଦି (SBI_PMU_FW_PLATFORM == event_code &&
		    pmu_dev && pmu_dev->fw_counter_match_encoding) {
			ଯଦି (!pmu_dev->fw_counter_match_encoding(phs->hartid,
							    cidx - ଅଙ୍କ_hw_ctrs,
							    edata))
				ଜାରି;
		}

		ଫେରନ୍ତୁ cidx;
	}

	ଫେରନ୍ତୁ SBI_ENOTSUPP;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_pmu_ctr_cfg_match(unsigned long cidx_base, unsigned long cidx_mask,
			  unsigned long flags, unsigned long event_idx,
			  uint64_t event_data)
{
	ସଂରଚନା sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ଯଦି (unlikely(!phs))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_code;

	ଯଦି (!pmu_ctr_idx_validate(cidx_base, cidx_mask))
		ଫେରନ୍ତୁ SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_data);
	ଯଦି (event_type < 0)
		ଫେରନ୍ତୁ SBI_EINVAL;
	event_code = get_cidx_code(event_idx);

	ଯଦି (flags & SBI_PMU_CFG_FLAG_SKIP_MATCH) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		unsigned long cidx_first = cidx_base + sbi_ffs(cidx_mask);

		ଯଦି (cidx_first >= total_ctrs)
			ଫେରନ୍ତୁ SBI_EINVAL;

		ଯଦି (phs->active_events[cidx_first] == SBI_PMU_EVENT_IDX_INVALID)
			ଫେରନ୍ତୁ SBI_EINVAL;
		ctr_idx = cidx_first;
		ଯାଅ skip_match;
	}

	ଯଦି (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_find_fw(phs, cidx_base, cidx_mask,
					  event_code, event_data);
		ଯଦି ((event_code == SBI_PMU_FW_PLATFORM) && (ctr_idx >= ଅଙ୍କ_hw_ctrs))
			phs->fw_counters_data[ctr_idx - ଅଙ୍କ_hw_ctrs] =
								event_data;
	} ଅନ୍ୟଥା {
		ctr_idx = pmu_ctr_find_hw(phs, cidx_base, cidx_mask, flags,
					  event_idx, event_data);
		ଯଦି (ctr_idx >= 0) {
			ସଂରଚନା sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_data = event_data;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_MASK;
		}
	}

	ଯଦି (ctr_idx < 0)
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	phs->active_events[ctr_idx] = event_idx;
skip_match:
	ଯଦି (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		ଯଦି (flags & SBI_PMU_CFG_FLAG_CLEAR_ମୂଲ୍ୟ)
			pmu_ctr_write_hw(ctr_idx, 0);
		ଯଦି (flags & SBI_PMU_CFG_FLAG_AUTO_START)
			pmu_ctr_start_hw(ctr_idx, 0, false);
	} ଅନ୍ୟଥା ଯଦି (event_type == SBI_PMU_EVENT_TYPE_FW) {
		ଯଦି (flags & SBI_PMU_CFG_FLAG_CLEAR_ମୂଲ୍ୟ)
			phs->fw_counters_data[ctr_idx - ଅଙ୍କ_hw_ctrs] = 0;
		ଯଦି (flags & SBI_PMU_CFG_FLAG_AUTO_START) {
			ଯଦି (SBI_PMU_FW_PLATFORM == event_code &&
			    pmu_dev && pmu_dev->fw_counter_start) {
				ret = pmu_dev->fw_counter_start(
					phs->hartid,
					ctr_idx - ଅଙ୍କ_hw_ctrs, event_data);
				ଯଦି (ret)
					ଫେରନ୍ତୁ ret;
			}
			phs->fw_counters_started |= BIT(ctr_idx - ଅଙ୍କ_hw_ctrs);
		}
	}

	ଫେରନ୍ତୁ ctr_idx;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_pmu_ctr_incr_fw(ଗଣନା sbi_pmu_fw_event_code_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = ଖାଲି;
	ସଂରଚନା sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ଯଦି (unlikely(!phs))
		ଫେରନ୍ତୁ 0;

	ଯଦି (likely(!phs->fw_counters_started))
		ଫେରନ୍ତୁ 0;

	ଯଦି (unlikely(fw_id >= SBI_PMU_FW_MAX))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ପାଇଁ (cidx = ଅଙ୍କ_hw_ctrs; cidx < total_ctrs; cidx++) {
		ଯଦି (get_cidx_code(phs->active_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & BIT(cidx - ଅଙ୍କ_hw_ctrs))) {
			fcounter = &phs->fw_counters_data[cidx - ଅଙ୍କ_hw_ctrs];
			ଭାଙ୍ଗ;
		}
	}

	ଯଦି (fcounter)
		(*fcounter)++;

	ଫେରନ୍ତୁ 0;
}

unsigned long sbi_pmu_ଅଙ୍କ_ctr(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ (ଅଙ୍କ_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_pmu_ctr_get_info(uint32_t cidx, unsigned long *ctr_info)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width;
	ସଂଘ sbi_pmu_ctr_info cinfo = {0};
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long counter_mask = (unsigned long)sbi_hart_mhpm_mask(scratch) |
				     SBI_PMU_CY_IR_MASK;

	/* Sanity check */
	ଯଦି (cidx >= total_ctrs)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	ଯଦି (cidx < ଅଙ୍କ_hw_ctrs) {
		ଯଦି (!(__test_bit(cidx, &counter_mask)))
			ଫେରନ୍ତୁ SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_CYCLE + cidx;
		/* mcycle & minstret are always 64 bit */
		ଯଦି (cidx == 0 || cidx == 2)
			cinfo.width = 63;
		ଅନ୍ୟଥା
			cinfo.width = sbi_hart_mhpm_bits(scratch) - 1;
	} ଅନ୍ୟଥା {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.width = 63;
		ଯଦି (pmu_dev && pmu_dev->fw_counter_width) {
			width = pmu_dev->fw_counter_width();
			ଯଦି (width)
				cinfo.width = width - 1;
		}
	}

	*ctr_info = cinfo.ମୂଲ୍ୟ;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_pmu_event_get_info(unsigned long shmem_phys_lo, unsigned long shmem_phys_hi,
			   unsigned long ଅଙ୍କ_events, unsigned long flags)
{
	unsigned long shmem_size = ଅଙ୍କ_events * ଆକାର(ସଂରଚନା sbi_pmu_event_info);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, j, event_type;
	ସଂରଚନା sbi_pmu_event_info *einfo;
	ସଂରଚନା sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	ସଂରଚନା sbi_pmu_hw_event *temp;
	bool found = false;

	ଯଦି (flags != 0)
		ଫେରନ୍ତୁ SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	ଯଦି (!ଅଙ୍କ_events || (shmem_phys_lo & 0xF))
		ଫେରନ୍ତୁ SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	ଯଦି (shmem_phys_hi)
		ଫେରନ୍ତୁ SBI_EINVALID_ADDR;

	ଯଦି (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_size, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ଫେରନ୍ତୁ SBI_ERR_INVALID_ADDRESS;

	sbi_hart_protection_temp_map_range(shmem_phys_lo, shmem_size);

	einfo = (ସଂରଚନା sbi_pmu_event_info *)(shmem_phys_lo);
	ପାଇଁ (i = 0; i < ଅଙ୍କ_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		ଯଦି (event_idx & SBI_PMU_EVENT_IDX_MBZ_MASK)
			ଫେରନ୍ତୁ SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_data);
		ଯଦି (event_type < 0) {
			einfo[i].output = 0;
		} ଅନ୍ୟଥା ଯଦି (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} ଅନ୍ୟଥା {
			ପାଇଁ (j = 0; j < ଅଙ୍କ_hw_events; j++) {
				temp = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				ଯଦି (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					ଯଦି (temp->start_idx > event_idx ||
					    event_idx > temp->end_idx)
						ଜାରି;
					/* just match the selector */
					ଯଦି (temp->select == (einfo[i].event_data &
									temp->select_mask)) {
						found = true;
						ଭାଙ୍ଗ;
					}
				} ଅନ୍ୟଥା ଯଦି (temp->start_idx <= event_idx &&
					   event_idx <= temp->end_idx) {
					found = true;
					ଭାଙ୍ଗ;
				}
			}
			ଯଦି (found)
				einfo[i].output = 1;
			ଅନ୍ୟଥା
				einfo[i].output = 0;
			found = false;
		}
	}

	sbi_hart_protection_temp_unmap_range(shmem_phys_lo, shmem_size);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ pmu_reset_event_map(ସଂରଚନା sbi_pmu_hart_state *phs)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;

	/* Initialize the counter to event mapping table */
	ପାଇଁ (j = 3; j < total_ctrs; j++)
		phs->active_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	ପାଇଁ (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_data[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

ଅଚଳ ସଂରଚନା sbi_pmu_device *sbi_pmu_get_device(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ pmu_dev;
}

ଶୂନ୍ୟ sbi_pmu_set_device(ଅଚଳ ସଂରଚନା sbi_pmu_device *dev)
{
	ଯଦି (!dev || pmu_dev)
		ଫେରନ୍ତୁ;

	pmu_dev = dev;
}

ଶୂନ୍ୟ sbi_pmu_exit(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	ଯଦି (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	ଯଦି (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	ଯଦି (unlikely(!phs))
		ଫେରନ୍ତୁ;

	pmu_reset_event_map(phs);
}

ସ୍ଥିର ଶୂନ୍ୟ pmu_sse_enable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_set(CSR_MIE, irq_mask);
}

ସ୍ଥିର ଶୂନ୍ୟ pmu_sse_disable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_clear(CSR_MIE, irq_mask);
	csr_clear(CSR_MIP, irq_mask);
}

ସ୍ଥିର ଶୂନ୍ୟ pmu_sse_complete(uint32_t event_id)
{
	csr_set(CSR_MIE, sbi_pmu_irq_mask());
}

ସ୍ଥିର ଶୂନ୍ୟ pmu_sse_register(uint32_t event_id)
{
	ସଂରଚନା sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_mask());
}

ସ୍ଥିର ଶୂନ୍ୟ pmu_sse_unregister(uint32_t event_id)
{
	ସଂରଚନା sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_set(CSR_MIDELEG, sbi_pmu_irq_mask());
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_sse_cb_ops pmu_sse_cb_ops = {
	.register_cb = pmu_sse_register,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_pmu_init(ସଂରଚନା sbi_scratch *scratch, bool cold_boot)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା hpm_ଗଣନା = sbi_fls(sbi_hart_mhpm_mask(scratch));
	ସଂରଚନା sbi_pmu_hart_state *phs;
	ଅଚଳ ସଂରଚନା sbi_platform *plat;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (cold_boot) {
		hw_event_map = sbi_calloc(ଆକାର(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		ଯଦି (!hw_event_map)
			ଫେରନ୍ତୁ SBI_ENOMEM;

		phs_ptr_offset = sbi_scratch_alloc_type_offset(ଶୂନ୍ୟ *);
		ଯଦି (!phs_ptr_offset) {
			sbi_free(hw_event_map);
			ଫେରନ୍ତୁ SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		ଯଦି (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		ଯଦି (!hpm_ଗଣନା)
			/* Only CY, TM & IR are implemented in the hw */
			ଅଙ୍କ_hw_ctrs = 3;
		ଅନ୍ୟଥା
			ଅଙ୍କ_hw_ctrs = hpm_ଗଣନା + 1;

		ଯଦି (ଅଙ୍କ_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			ଫେରନ୍ତୁ SBI_EINVAL;

		total_ctrs = ଅଙ୍କ_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		ଯଦି (sbi_pmu_irq_bit() >= 0)
			sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	ଯଦି (!phs) {
		phs = sbi_zalloc(ଆକାର(*phs));
		ଯଦି (!phs)
			ଫେରନ୍ତୁ SBI_ENOMEM;
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

	ଫେରନ୍ତୁ 0;
}
