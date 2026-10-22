/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/sbi_bitops.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_hart_protection.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_pmu.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_string.h>
#અન્તર્ભાવ <sbi/sbi_sse.h>

/** Information about hardware counters */
સંરચના sbi_pmu_hw_event {
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
સંઘ sbi_pmu_ctr_info {
	unsigned long કિંમત;
	સંરચના {
		unsigned long csr:12;
		unsigned long width:6;
#જો __riscv_xlen == 32
		unsigned long reserved:13;
#નહીંતર
		unsigned long reserved:45;
#endif
		unsigned long type:1;
	};
};

#જો SBI_PMU_FW_CTR_MAX >= BITS_PER_LONG
#error "Can't handle firmware counters beyond BITS_PER_LONG"
#endif

/** HW event configuration parameters */
સંરચના sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_data;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
સંરચના sbi_pmu_hart_state {
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
	સંરચના sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
સ્થિર unsigned long phs_ptr_offset;

#define pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_offset ? sbi_scratch_read_type((__scratch), શૂન્ય *, phs_ptr_offset) : ખાલી

#define pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define pmu_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), શૂન્ય *, phs_ptr_offset, (__phs))

/* Platform specific PMU device */
સ્થિર અચલ સંરચના sbi_pmu_device *pmu_dev = ખાલી;

/* Mapping between event range and possible counters  */
સ્થિર સંરચના sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
સ્થિર uint32_t અંક_hw_events;
/* Maximum number of hardware counters available */
સ્થિર uint32_t અંક_hw_ctrs;

/* Maximum number of counters available */
સ્થિર uint32_t total_ctrs;

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
સ્થિર bool pmu_event_range_overlap(સંરચના sbi_pmu_hw_event *evtA,
				    સંરચના sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	જો (((evtA->end_idx < evtB->start_idx) && (evtA->end_idx < evtB->end_idx)) ||
	   ((evtA->start_idx > evtB->start_idx) && (evtA->start_idx > evtB->end_idx)))
		પાછા false;
	પાછા true;
}

સ્થિર bool pmu_event_select_overlap(સંરચના sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_mask)
{
	જો ((evt->select == select_val) && (evt->select_mask == select_mask))
		પાછા true;

	પાછા false;
}

સ્થિર પૂર્ણાંક pmu_event_validate(સંરચના sbi_pmu_hart_state *phs,
			      unsigned long event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_code = get_cidx_code(event_idx);
	uint32_t event_idx_code_max = -1;
	uint32_t cache_ops_result, cache_ops_id, cache_id;

	પસંદગી(event_idx_type) {
	સ્થિતિ SBI_PMU_EVENT_TYPE_HW:
		event_idx_code_max = SBI_PMU_HW_GENERAL_MAX;
		તોડો;
	સ્થિતિ SBI_PMU_EVENT_TYPE_FW:
		જો ((event_idx_code >= SBI_PMU_FW_MAX &&
		    event_idx_code <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_code > SBI_PMU_FW_PLATFORM)
			પાછા SBI_EINVAL;

		જો (SBI_PMU_FW_PLATFORM == event_idx_code &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			પાછા pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		નહીંતર
			event_idx_code_max = SBI_PMU_FW_MAX;
		તોડો;
	સ્થિતિ SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_result = event_idx_code &
					SBI_PMU_EVENT_HW_CACHE_OPS_RESULT;
		cache_ops_id = (event_idx_code &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_MASK) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_OFFSET;
		cache_id = (event_idx_code &
			    SBI_PMU_EVENT_HW_CACHE_ID_MASK) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_OFFSET;
		જો ((cache_ops_result < SBI_PMU_HW_CACHE_RESULT_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			પાછા event_idx_type;
		નહીંતર
			પાછા SBI_EINVAL;
		તોડો;
	સ્થિતિ SBI_PMU_EVENT_TYPE_HW_RAW:
	સ્થિતિ SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_code_max = 1; // event_idx.code should be zero
		તોડો;
	મૂળભૂત:
		પાછા SBI_EINVAL;
	}

	જો (event_idx_code < event_idx_code_max)
		પાછા event_idx_type;

	પાછા SBI_EINVAL;
}

સ્થિર પૂર્ણાંક pmu_ctr_validate(સંરચના sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_code)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	જો (cidx >= total_ctrs)
		પાછા SBI_EINVAL;

	event_idx_val = phs->active_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	જો (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		પાછા SBI_EINVAL;

	*event_idx_code = get_cidx_code(event_idx_val);

	પાછા event_idx_type;
}

સ્થિર bool pmu_ctr_idx_validate(unsigned long cbase, unsigned long cmask)
{
	unsigned long last;

	જો (!cmask)
		પાછા false;

	last = sbi_fls(cmask);
	જો (cbase > -1UL - last)
		પાછા false;

	પાછા (cbase + last) < total_ctrs;
}

પૂર્ણાંક sbi_pmu_ctr_fw_read(unsigned long cidx, uint64_t *cval, bool ઉપલું_bits)
{
	પૂર્ણાંક event_idx_type;
	uint32_t event_code;
	સંરચના sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	જો (unlikely(!phs))
		પાછા SBI_EINVAL;

	જો (cidx < અંક_hw_ctrs || cidx >= total_ctrs)
		પાછા SBI_EINVAL;

#જો __riscv_xlen > 32
	જો (ઉપલું_bits)
		પાછા 0;
#endif

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
	જો (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		પાછા SBI_EINVAL;

	જો ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		પાછા SBI_EINVAL;

	જો (SBI_PMU_FW_PLATFORM == event_code) {
		જો (pmu_dev && pmu_dev->fw_counter_read_કિંમત)
			*cval = pmu_dev->fw_counter_read_કિંમત(phs->hartid,
							       cidx -
							       અંક_hw_ctrs);
		નહીંતર
			*cval = 0;
	} નહીંતર
		*cval = phs->fw_counters_data[cidx - અંક_hw_ctrs];

	પાછા 0;
}

સ્થિર પૂર્ણાંક pmu_add_hw_event_map(u32 eidx_start, u32 eidx_end, u32 cmap,
				uint64_t select, uint64_t select_mask)
{
	પૂર્ણાંક i = 0;
	bool is_overlap;
	સંરચના sbi_pmu_hw_event *event = &hw_event_map[અંક_hw_events];
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_mask = sbi_hart_mhpm_mask(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	જો (eidx_start > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_MASK))
		પાછા SBI_EDENIED;

	જો (અંક_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		પાછા SBI_EFAIL;
	}

	event->start_idx = eidx_start;
	event->end_idx = eidx_end;

	/* Sanity check */
	માટે (i = 0; i < અંક_hw_events; i++) {
		જો (eidx_start == SBI_PMU_EVENT_RAW_IDX ||
			eidx_start == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_mask);
		નહીંતર
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		જો (is_overlap)
			જાઓ reset_event;
	}

	event->select_mask = select_mask;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_mask;
	event->select = select;
	અંક_hw_events++;

	પાછા 0;

reset_event:
	event->start_idx = 0;
	event->end_idx = 0;
	પાછા SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
પૂર્ણાંક sbi_pmu_add_hw_event_counter_map(u32 eidx_start, u32 eidx_end, u32 cmap)
{
	જો ((eidx_start > eidx_end) || eidx_start >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_end >= SBI_PMU_EVENT_RAW_IDX)
		પાછા SBI_EINVAL;

	પાછા pmu_add_hw_event_map(eidx_start, eidx_end, cmap, 0, 0);
}

પૂર્ણાંક sbi_pmu_add_raw_event_counter_map(uint64_t select, uint64_t select_mask, u32 cmap)
{
	પાછા pmu_add_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_mask);
}

શૂન્ય sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_mask());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW);
}

સ્થિર પૂર્ણાંક pmu_ctr_enable_irq_hw(પૂર્ણાંક ctr_idx)
{
	unsigned long mhpmevent_csr;
	unsigned long mhpmevent_curr;
	unsigned long mip_val;
	unsigned long of_mask;

	જો (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		પાછા SBI_EFAIL;

#જો __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	of_mask = (uint32_t)~MHPMEVENTH_OF;
#નહીંતર
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	of_mask = ~MHPMEVENT_OF;
#endif

	mhpmevent_curr = csr_read_અંક(mhpmevent_csr);
	mip_val = csr_read(CSR_MIP);
	/**
	 * Clear out the OF bit so that next interrupt can be enabled.
	 * This should be done only when the corresponding overflow interrupt
	 * bit is cleared. That indicates that software has already handled the
	 * previous interrupts or the hardware yet to set an overflow interrupt.
	 * Otherwise, there will be race conditions where we may clear the bit
	 * the software is yet to handle the interrupt.
	 */
	જો (!(mip_val & sbi_pmu_irq_mask())) {
		mhpmevent_curr &= of_mask;
		csr_write_અંક(mhpmevent_csr, mhpmevent_curr);
	}

	પાછા 0;
}

સ્થિર શૂન્ય pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#જો __riscv_xlen == 32
	csr_write_અંક(CSR_MCYCLE + cidx, 0);
	csr_write_અંક(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_અંક(CSR_MCYCLEH + cidx, ival >> BITS_PER_LONG);
#નહીંતર
	csr_write_અંક(CSR_MCYCLE + cidx, ival);
#endif
}

સ્થિર પૂર્ણાંક pmu_ctr_start_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	જો (cidx >= અંક_hw_ctrs || cidx == 1)
		પાછા SBI_EINVAL;

	જો (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11) {
		જો (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		પાછા 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	જો (!__test_bit(cidx, &mctr_inhbt))
		પાછા SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	જો (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	જો (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	જો (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	પાછા 0;
}

પૂર્ણાંક sbi_pmu_irq_bit(શૂન્ય)
{
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	જો (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		પાછા IRQ_PMU_OVF;
	જો (pmu_dev && pmu_dev->hw_counter_irq_bit)
		પાછા pmu_dev->hw_counter_irq_bit();

	પાછા -1;
}

unsigned long sbi_pmu_irq_mask(શૂન્ય)
{
	પૂર્ણાંક irq_bit = sbi_pmu_irq_bit();

	જો (irq_bit < 0)
		પાછા 0;

	પાછા BIT(irq_bit);
}

સ્થિર પૂર્ણાંક pmu_ctr_start_fw(સંરચના sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_code,
			    uint64_t event_data, uint64_t ival,
			    bool ival_update)
{
	પૂર્ણાંક ret;

	જો ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		પાછા SBI_EINVAL;

	જો (phs->fw_counters_started & BIT(cidx - અંક_hw_ctrs))
		પાછા SBI_EALREADY_STARTED;

	જો (SBI_PMU_FW_PLATFORM == event_code) {
		જો (!pmu_dev ||
		    !pmu_dev->fw_counter_write_કિંમત ||
		    !pmu_dev->fw_counter_start) {
			પાછા SBI_EINVAL;
		}

		જો (ival_update)
			pmu_dev->fw_counter_write_કિંમત(phs->hartid,
							cidx - અંક_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_start(phs->hartid,
						 cidx - અંક_hw_ctrs,
						 event_data);
		જો (ret)
			પાછા ret;
	} નહીંતર {
		જો (ival_update)
			phs->fw_counters_data[cidx - અંક_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= BIT(cidx - અંક_hw_ctrs);

	પાછા 0;
}

સ્થિર શૂન્ય pmu_update_inhibit_flags(unsigned long flags, uint64_t *mhpmevent_val)
{
	જો (flags & SBI_PMU_CFG_FLAG_SET_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	જો (flags & SBI_PMU_CFG_FLAG_SET_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	જો (flags & SBI_PMU_CFG_FLAG_SET_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	જો (flags & SBI_PMU_CFG_FLAG_SET_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

સ્થિર પૂર્ણાંક pmu_update_hw_mhpmevent(સંરચના sbi_pmu_hw_event *hw_evt, પૂર્ણાંક ctr_idx,
				   unsigned long flags, unsigned long eindex,
				   uint64_t data)
{
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	અચલ સંરચના sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_to_mhpmevent(plat, eindex, data);

	જો (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		પાછા SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	જો (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_MASK) |
				 MHPMEVENT_MINH | MHPMEVENT_OF;

	જો (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	જો (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	જો (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, ctr_idx);

#જો __riscv_xlen == 32
	csr_write_અંક(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	જો (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_અંક(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> BITS_PER_LONG);
#નહીંતર
	csr_write_અંક(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#endif

	પાછા 0;
}

પૂર્ણાંક sbi_pmu_ctr_start(unsigned long cbase, unsigned long cmask,
		      unsigned long flags, uint64_t ival)
{
	સંરચના sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	જો (unlikely(!phs))
		પાછા SBI_EINVAL;

	પૂર્ણાંક event_idx_type;
	uint32_t event_code;
	પૂર્ણાંક ret = SBI_EINVAL;
	bool bUpdate = false;
	પૂર્ણાંક i, cidx;
	uint64_t edata;

	જો (!pmu_ctr_idx_validate(cbase, cmask))
		પાછા ret;

	જો (flags & ~SBI_PMU_START_FLAGS_MASK)
		પાછા SBI_ERR_INVALID_PARAM;

	જો (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		પાછા SBI_ENO_SHMEM;

	જો (flags & SBI_PMU_START_FLAG_SET_INIT_કિંમત)
		bUpdate = true;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		જો (event_idx_type < 0)
			/* Continue the start operation for other counters */
			ચાલુ;
		નહીંતર જો (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_code == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_data[cidx - અંક_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_start_fw(phs, cidx, event_code, edata,
					       ival, bUpdate);
			જો (ret)
				પાછા ret;
		} નહીંતર {
			જો (cidx >= 3) {
				સંરચના sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->active_events[cidx],
							ev_cfg->event_data);
				જો (ret)
					પાછા ret;
			}
			ret = pmu_ctr_start_hw(cidx, ival, bUpdate);
			જો (ret)
				પાછા ret;
		}
	}

	પાછા ret;
}

સ્થિર પૂર્ણાંક pmu_ctr_stop_hw(uint32_t cidx)
{
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	જો (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11)
		પાછા 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	જો (cidx >= અંક_hw_ctrs || cidx == 1)
		પાછા SBI_EINVAL;

	જો (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		જો (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		પાછા 0;
	} નહીંતર
		પાછા SBI_EALREADY_STOPPED;
}

સ્થિર પૂર્ણાંક pmu_ctr_stop_fw(સંરચના sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_code)
{
	પૂર્ણાંક ret;

	જો ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		પાછા SBI_EINVAL;

	જો (!(phs->fw_counters_started & BIT(cidx - અંક_hw_ctrs)))
		પાછા SBI_EALREADY_STOPPED;

	જો (SBI_PMU_FW_PLATFORM == event_code &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - અંક_hw_ctrs);
		જો (ret)
			પાછા ret;
	}

	phs->fw_counters_started &= ~BIT(cidx - અંક_hw_ctrs);

	પાછા 0;
}

સ્થિર પૂર્ણાંક pmu_reset_hw_mhpmevent(પૂર્ણાંક ctr_idx)
{
	જો (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		પાછા SBI_EFAIL;
#જો __riscv_xlen == 32
	csr_write_અંક(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	જો (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_અંક(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#નહીંતર
	csr_write_અંક(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#endif

	પાછા 0;
}

પૂર્ણાંક sbi_pmu_ctr_stop(unsigned long cbase, unsigned long cmask,
		     unsigned long flag)
{
	સંરચના sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	જો (unlikely(!phs))
		પાછા SBI_EINVAL;

	પૂર્ણાંક ret = SBI_EINVAL;
	પૂર્ણાંક event_idx_type;
	uint32_t event_code;
	પૂર્ણાંક i, cidx;

	જો (!pmu_ctr_idx_validate(cbase, cmask))
		પાછા ret;

	જો (flag & ~SBI_PMU_STOP_FLAGS_MASK)
		પાછા SBI_ERR_INVALID_PARAM;

	જો (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		પાછા SBI_ENO_SHMEM;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		જો (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			ચાલુ;

		નહીંતર જો (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_code);
		નહીંતર
			ret = pmu_ctr_stop_hw(cidx);

		જો(ret)
			પાછા ret;

		જો (cidx > (CSR_INSTRET - CSR_CYCLE) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->active_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	જો (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_mask());

	પાછા ret;
}

સ્થિર પૂર્ણાંક pmu_fixed_ctr_update_inhibit_bits(પૂર્ણાંક fixed_ctr, unsigned long flags)
{
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#જો __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#endif
	જો (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_mode))
		પાછા fixed_ctr;

	પસંદગી (fixed_ctr) {
	સ્થિતિ 0:
		cfg_csr_no = CSR_MCYCLECFG;
#જો __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#endif
		તોડો;
	સ્થિતિ 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#જો __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#endif
		તોડો;
	મૂળભૂત:
		પાછા SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	જો (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#જો __riscv_xlen == 32
		csr_write_અંક(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_અંક(cfgh_csr_no, cfg_val >> BITS_PER_LONG);
#નહીંતર
		csr_write_અંક(cfg_csr_no, cfg_val);
#endif
	}
	જો (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, fixed_ctr);
	પાછા fixed_ctr;
}

સ્થિર પૂર્ણાંક pmu_ctr_find_fixed_hw(unsigned long evt_idx_code)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	જો (evt_idx_code == SBI_PMU_HW_CPU_CYCLES)
		પાછા 0;
	નહીંતર જો (evt_idx_code == SBI_PMU_HW_INSTRUCTIONS)
		પાછા 2;
	નહીંતર
		પાછા SBI_EINVAL;
}

સ્થિર પૂર્ણાંક pmu_ctr_find_hw(સંરચના sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   unsigned long flags,
			   unsigned long event_idx, uint64_t data)
{
	unsigned long ctr_mask;
	પૂર્ણાંક i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	સંરચના sbi_pmu_hw_event *temp;
	unsigned long mctr_inhbt = 0;
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	જો (cbase >= અંક_hw_ctrs)
		પાછા SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_find_fixed_hw(event_idx);
	જો (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		પાછા pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);

	જો (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	માટે (i = 0; i < અંક_hw_events; i++) {
		temp = &hw_event_map[i];
		જો ((temp->start_idx > event_idx && event_idx < temp->end_idx) ||
		    (temp->start_idx < event_idx && event_idx > temp->end_idx))
			ચાલુ;

		/* For raw events, event data is used as the select value */
		જો (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_mask = temp->select_mask;

			/* The non-event map bits of data should match the selector */
			જો (temp->select != (data & select_mask))
				ચાલુ;
		}
		/* Fixed counters should not be part of the search */
		ctr_mask = temp->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_MASK);
		for_each_set_bit_from(cbase, &ctr_mask, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			જો (phs->active_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				ચાલુ;
			/* If mcountinhibit is supported, the bit must be enabled */
			જો ((sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				ચાલુ;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			તોડો;
		}
	}

	જો (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		જો (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			પાછા SBI_EFAIL;

		પાછા pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(temp, ctr_idx, flags, event_idx, data);

	જો (!ret)
		ret = ctr_idx;

	પાછા ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
સ્થિર પૂર્ણાંક pmu_ctr_find_fw(સંરચના sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   uint32_t event_code, uint64_t edata)
{
	પૂર્ણાંક i, cidx;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		જો (cidx < અંક_hw_ctrs || total_ctrs <= cidx)
			ચાલુ;
		જો (phs->active_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			ચાલુ;
		જો (SBI_PMU_FW_PLATFORM == event_code &&
		    pmu_dev && pmu_dev->fw_counter_match_encoding) {
			જો (!pmu_dev->fw_counter_match_encoding(phs->hartid,
							    cidx - અંક_hw_ctrs,
							    edata))
				ચાલુ;
		}

		પાછા cidx;
	}

	પાછા SBI_ENOTSUPP;
}

પૂર્ણાંક sbi_pmu_ctr_cfg_match(unsigned long cidx_base, unsigned long cidx_mask,
			  unsigned long flags, unsigned long event_idx,
			  uint64_t event_data)
{
	સંરચના sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	જો (unlikely(!phs))
		પાછા SBI_EINVAL;

	પૂર્ણાંક ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_code;

	જો (!pmu_ctr_idx_validate(cidx_base, cidx_mask))
		પાછા SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_data);
	જો (event_type < 0)
		પાછા SBI_EINVAL;
	event_code = get_cidx_code(event_idx);

	જો (flags & SBI_PMU_CFG_FLAG_SKIP_MATCH) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		unsigned long cidx_first = cidx_base + sbi_ffs(cidx_mask);

		જો (cidx_first >= total_ctrs)
			પાછા SBI_EINVAL;

		જો (phs->active_events[cidx_first] == SBI_PMU_EVENT_IDX_INVALID)
			પાછા SBI_EINVAL;
		ctr_idx = cidx_first;
		જાઓ skip_match;
	}

	જો (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_find_fw(phs, cidx_base, cidx_mask,
					  event_code, event_data);
		જો ((event_code == SBI_PMU_FW_PLATFORM) && (ctr_idx >= અંક_hw_ctrs))
			phs->fw_counters_data[ctr_idx - અંક_hw_ctrs] =
								event_data;
	} નહીંતર {
		ctr_idx = pmu_ctr_find_hw(phs, cidx_base, cidx_mask, flags,
					  event_idx, event_data);
		જો (ctr_idx >= 0) {
			સંરચના sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_data = event_data;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_MASK;
		}
	}

	જો (ctr_idx < 0)
		પાછા SBI_ENOTSUPP;

	phs->active_events[ctr_idx] = event_idx;
skip_match:
	જો (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		જો (flags & SBI_PMU_CFG_FLAG_CLEAR_કિંમત)
			pmu_ctr_write_hw(ctr_idx, 0);
		જો (flags & SBI_PMU_CFG_FLAG_AUTO_START)
			pmu_ctr_start_hw(ctr_idx, 0, false);
	} નહીંતર જો (event_type == SBI_PMU_EVENT_TYPE_FW) {
		જો (flags & SBI_PMU_CFG_FLAG_CLEAR_કિંમત)
			phs->fw_counters_data[ctr_idx - અંક_hw_ctrs] = 0;
		જો (flags & SBI_PMU_CFG_FLAG_AUTO_START) {
			જો (SBI_PMU_FW_PLATFORM == event_code &&
			    pmu_dev && pmu_dev->fw_counter_start) {
				ret = pmu_dev->fw_counter_start(
					phs->hartid,
					ctr_idx - અંક_hw_ctrs, event_data);
				જો (ret)
					પાછા ret;
			}
			phs->fw_counters_started |= BIT(ctr_idx - અંક_hw_ctrs);
		}
	}

	પાછા ctr_idx;
}

પૂર્ણાંક sbi_pmu_ctr_incr_fw(ગણના sbi_pmu_fw_event_code_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = ખાલી;
	સંરચના sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	જો (unlikely(!phs))
		પાછા 0;

	જો (likely(!phs->fw_counters_started))
		પાછા 0;

	જો (unlikely(fw_id >= SBI_PMU_FW_MAX))
		પાછા SBI_EINVAL;

	માટે (cidx = અંક_hw_ctrs; cidx < total_ctrs; cidx++) {
		જો (get_cidx_code(phs->active_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & BIT(cidx - અંક_hw_ctrs))) {
			fcounter = &phs->fw_counters_data[cidx - અંક_hw_ctrs];
			તોડો;
		}
	}

	જો (fcounter)
		(*fcounter)++;

	પાછા 0;
}

unsigned long sbi_pmu_અંક_ctr(શૂન્ય)
{
	પાછા (અંક_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

પૂર્ણાંક sbi_pmu_ctr_get_info(uint32_t cidx, unsigned long *ctr_info)
{
	પૂર્ણાંક width;
	સંઘ sbi_pmu_ctr_info cinfo = {0};
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long counter_mask = (unsigned long)sbi_hart_mhpm_mask(scratch) |
				     SBI_PMU_CY_IR_MASK;

	/* Sanity check */
	જો (cidx >= total_ctrs)
		પાછા SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	જો (cidx < અંક_hw_ctrs) {
		જો (!(__test_bit(cidx, &counter_mask)))
			પાછા SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_CYCLE + cidx;
		/* mcycle & minstret are always 64 bit */
		જો (cidx == 0 || cidx == 2)
			cinfo.width = 63;
		નહીંતર
			cinfo.width = sbi_hart_mhpm_bits(scratch) - 1;
	} નહીંતર {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.width = 63;
		જો (pmu_dev && pmu_dev->fw_counter_width) {
			width = pmu_dev->fw_counter_width();
			જો (width)
				cinfo.width = width - 1;
		}
	}

	*ctr_info = cinfo.કિંમત;

	પાછા 0;
}

પૂર્ણાંક sbi_pmu_event_get_info(unsigned long shmem_phys_lo, unsigned long shmem_phys_hi,
			   unsigned long અંક_events, unsigned long flags)
{
	unsigned long shmem_size = અંક_events * કદ(સંરચના sbi_pmu_event_info);
	પૂર્ણાંક i, j, event_type;
	સંરચના sbi_pmu_event_info *einfo;
	સંરચના sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	સંરચના sbi_pmu_hw_event *temp;
	bool found = false;

	જો (flags != 0)
		પાછા SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	જો (!અંક_events || (shmem_phys_lo & 0xF))
		પાછા SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	જો (shmem_phys_hi)
		પાછા SBI_EINVALID_ADDR;

	જો (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_size, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		પાછા SBI_ERR_INVALID_ADDRESS;

	sbi_hart_protection_temp_map_range(shmem_phys_lo, shmem_size);

	einfo = (સંરચના sbi_pmu_event_info *)(shmem_phys_lo);
	માટે (i = 0; i < અંક_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		જો (event_idx & SBI_PMU_EVENT_IDX_MBZ_MASK)
			પાછા SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_data);
		જો (event_type < 0) {
			einfo[i].output = 0;
		} નહીંતર જો (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} નહીંતર {
			માટે (j = 0; j < અંક_hw_events; j++) {
				temp = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				જો (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					જો (temp->start_idx > event_idx ||
					    event_idx > temp->end_idx)
						ચાલુ;
					/* just match the selector */
					જો (temp->select == (einfo[i].event_data &
									temp->select_mask)) {
						found = true;
						તોડો;
					}
				} નહીંતર જો (temp->start_idx <= event_idx &&
					   event_idx <= temp->end_idx) {
					found = true;
					તોડો;
				}
			}
			જો (found)
				einfo[i].output = 1;
			નહીંતર
				einfo[i].output = 0;
			found = false;
		}
	}

	sbi_hart_protection_temp_unmap_range(shmem_phys_lo, shmem_size);

	પાછા 0;
}

સ્થિર શૂન્ય pmu_reset_event_map(સંરચના sbi_pmu_hart_state *phs)
{
	પૂર્ણાંક j;

	/* Initialize the counter to event mapping table */
	માટે (j = 3; j < total_ctrs; j++)
		phs->active_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	માટે (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_data[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

અચલ સંરચના sbi_pmu_device *sbi_pmu_get_device(શૂન્ય)
{
	પાછા pmu_dev;
}

શૂન્ય sbi_pmu_set_device(અચલ સંરચના sbi_pmu_device *dev)
{
	જો (!dev || pmu_dev)
		પાછા;

	pmu_dev = dev;
}

શૂન્ય sbi_pmu_exit(સંરચના sbi_scratch *scratch)
{
	સંરચના sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	જો (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	જો (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	જો (unlikely(!phs))
		પાછા;

	pmu_reset_event_map(phs);
}

સ્થિર શૂન્ય pmu_sse_enable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_set(CSR_MIE, irq_mask);
}

સ્થિર શૂન્ય pmu_sse_disable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_clear(CSR_MIE, irq_mask);
	csr_clear(CSR_MIP, irq_mask);
}

સ્થિર શૂન્ય pmu_sse_complete(uint32_t event_id)
{
	csr_set(CSR_MIE, sbi_pmu_irq_mask());
}

સ્થિર શૂન્ય pmu_sse_register(uint32_t event_id)
{
	સંરચના sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_mask());
}

સ્થિર શૂન્ય pmu_sse_unregister(uint32_t event_id)
{
	સંરચના sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_set(CSR_MIDELEG, sbi_pmu_irq_mask());
}

સ્થિર અચલ સંરચના sbi_sse_cb_ops pmu_sse_cb_ops = {
	.register_cb = pmu_sse_register,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

પૂર્ણાંક sbi_pmu_init(સંરચના sbi_scratch *scratch, bool cold_boot)
{
	પૂર્ણાંક hpm_ગણતરી = sbi_fls(sbi_hart_mhpm_mask(scratch));
	સંરચના sbi_pmu_hart_state *phs;
	અચલ સંરચના sbi_platform *plat;
	પૂર્ણાંક rc;

	જો (cold_boot) {
		hw_event_map = sbi_calloc(કદ(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		જો (!hw_event_map)
			પાછા SBI_ENOMEM;

		phs_ptr_offset = sbi_scratch_alloc_type_offset(શૂન્ય *);
		જો (!phs_ptr_offset) {
			sbi_free(hw_event_map);
			પાછા SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		જો (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		જો (!hpm_ગણતરી)
			/* Only CY, TM & IR are implemented in the hw */
			અંક_hw_ctrs = 3;
		નહીંતર
			અંક_hw_ctrs = hpm_ગણતરી + 1;

		જો (અંક_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			પાછા SBI_EINVAL;

		total_ctrs = અંક_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		જો (sbi_pmu_irq_bit() >= 0)
			sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	જો (!phs) {
		phs = sbi_zalloc(કદ(*phs));
		જો (!phs)
			પાછા SBI_ENOMEM;
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

	પાછા 0;
}
