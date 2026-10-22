/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_hart_protection.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_pmu.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_sse.h>

/** Information about hardware counters */
संरचना sbi_pmu_hw_event {
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
संघ sbi_pmu_ctr_info {
	अचिन्हित दीर्घ मूल्य;
	संरचना {
		अचिन्हित दीर्घ csr:12;
		अचिन्हित दीर्घ width:6;
#जर __riscv_xlen == 32
		अचिन्हित दीर्घ reserved:13;
#अन्यथा
		अचिन्हित दीर्घ reserved:45;
#endif
		अचिन्हित दीर्घ type:1;
	};
};

#जर SBI_PMU_FW_CTR_MAX >= BITS_PER_LONG
#error "Can't handle firmware counters beyond BITS_PER_LONG"
#endif

/** HW event configuration parameters */
संरचना sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_data;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
संरचना sbi_pmu_hart_state {
	/* HART to which this state belongs */
	uint32_t hartid;
	/* Counter to enabled event mapping */
	uint32_t active_events[SBI_PMU_HW_CTR_MAX + SBI_PMU_FW_CTR_MAX];
	/* Bitmap of firmware counters started */
	अचिन्हित दीर्घ fw_counters_started;
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
	संरचना sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
स्थिर अचिन्हित दीर्घ phs_ptr_offset;

#define pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_offset ? sbi_scratch_read_type((__scratch), शून्य *, phs_ptr_offset) : रिक्त

#define pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define pmu_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), शून्य *, phs_ptr_offset, (__phs))

/* Platform specific PMU device */
स्थिर स्थिरांक संरचना sbi_pmu_device *pmu_dev = रिक्त;

/* Mapping between event range and possible counters  */
स्थिर संरचना sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
स्थिर uint32_t अंक_hw_events;
/* Maximum number of hardware counters available */
स्थिर uint32_t अंक_hw_ctrs;

/* Maximum number of counters available */
स्थिर uint32_t total_ctrs;

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
स्थिर bool pmu_event_range_overlap(संरचना sbi_pmu_hw_event *evtA,
				    संरचना sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	जर (((evtA->end_idx < evtB->start_idx) && (evtA->end_idx < evtB->end_idx)) ||
	   ((evtA->start_idx > evtB->start_idx) && (evtA->start_idx > evtB->end_idx)))
		परत false;
	परत true;
}

स्थिर bool pmu_event_select_overlap(संरचना sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_mask)
{
	जर ((evt->select == select_val) && (evt->select_mask == select_mask))
		परत true;

	परत false;
}

स्थिर पूर्णांक pmu_event_validate(संरचना sbi_pmu_hart_state *phs,
			      अचिन्हित दीर्घ event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_code = get_cidx_code(event_idx);
	uint32_t event_idx_code_max = -1;
	uint32_t cache_ops_result, cache_ops_id, cache_id;

	निवड(event_idx_type) {
	स्थिती SBI_PMU_EVENT_TYPE_HW:
		event_idx_code_max = SBI_PMU_HW_GENERAL_MAX;
		थांबा;
	स्थिती SBI_PMU_EVENT_TYPE_FW:
		जर ((event_idx_code >= SBI_PMU_FW_MAX &&
		    event_idx_code <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_code > SBI_PMU_FW_PLATFORM)
			परत SBI_EINVAL;

		जर (SBI_PMU_FW_PLATFORM == event_idx_code &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			परत pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		अन्यथा
			event_idx_code_max = SBI_PMU_FW_MAX;
		थांबा;
	स्थिती SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_result = event_idx_code &
					SBI_PMU_EVENT_HW_CACHE_OPS_RESULT;
		cache_ops_id = (event_idx_code &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_MASK) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_OFFSET;
		cache_id = (event_idx_code &
			    SBI_PMU_EVENT_HW_CACHE_ID_MASK) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_OFFSET;
		जर ((cache_ops_result < SBI_PMU_HW_CACHE_RESULT_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			परत event_idx_type;
		अन्यथा
			परत SBI_EINVAL;
		थांबा;
	स्थिती SBI_PMU_EVENT_TYPE_HW_RAW:
	स्थिती SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_code_max = 1; // event_idx.code should be zero
		थांबा;
	पूर्वनिर्धारित:
		परत SBI_EINVAL;
	}

	जर (event_idx_code < event_idx_code_max)
		परत event_idx_type;

	परत SBI_EINVAL;
}

स्थिर पूर्णांक pmu_ctr_validate(संरचना sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_code)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	जर (cidx >= total_ctrs)
		परत SBI_EINVAL;

	event_idx_val = phs->active_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	जर (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		परत SBI_EINVAL;

	*event_idx_code = get_cidx_code(event_idx_val);

	परत event_idx_type;
}

स्थिर bool pmu_ctr_idx_validate(अचिन्हित दीर्घ cbase, अचिन्हित दीर्घ cmask)
{
	अचिन्हित दीर्घ last;

	जर (!cmask)
		परत false;

	last = sbi_fls(cmask);
	जर (cbase > -1UL - last)
		परत false;

	परत (cbase + last) < total_ctrs;
}

पूर्णांक sbi_pmu_ctr_fw_read(अचिन्हित दीर्घ cidx, uint64_t *cval, bool वरचा_bits)
{
	पूर्णांक event_idx_type;
	uint32_t event_code;
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	जर (unlikely(!phs))
		परत SBI_EINVAL;

	जर (cidx < अंक_hw_ctrs || cidx >= total_ctrs)
		परत SBI_EINVAL;

#जर __riscv_xlen > 32
	जर (वरचा_bits)
		परत 0;
#endif

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
	जर (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		परत SBI_EINVAL;

	जर ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		परत SBI_EINVAL;

	जर (SBI_PMU_FW_PLATFORM == event_code) {
		जर (pmu_dev && pmu_dev->fw_counter_read_मूल्य)
			*cval = pmu_dev->fw_counter_read_मूल्य(phs->hartid,
							       cidx -
							       अंक_hw_ctrs);
		अन्यथा
			*cval = 0;
	} अन्यथा
		*cval = phs->fw_counters_data[cidx - अंक_hw_ctrs];

	परत 0;
}

स्थिर पूर्णांक pmu_add_hw_event_map(u32 eidx_start, u32 eidx_end, u32 cmap,
				uint64_t select, uint64_t select_mask)
{
	पूर्णांक i = 0;
	bool is_overlap;
	संरचना sbi_pmu_hw_event *event = &hw_event_map[अंक_hw_events];
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_mask = sbi_hart_mhpm_mask(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	जर (eidx_start > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_MASK))
		परत SBI_EDENIED;

	जर (अंक_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		परत SBI_EFAIL;
	}

	event->start_idx = eidx_start;
	event->end_idx = eidx_end;

	/* Sanity check */
	साठी (i = 0; i < अंक_hw_events; i++) {
		जर (eidx_start == SBI_PMU_EVENT_RAW_IDX ||
			eidx_start == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_mask);
		अन्यथा
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		जर (is_overlap)
			जा reset_event;
	}

	event->select_mask = select_mask;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_mask;
	event->select = select;
	अंक_hw_events++;

	परत 0;

reset_event:
	event->start_idx = 0;
	event->end_idx = 0;
	परत SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
पूर्णांक sbi_pmu_add_hw_event_counter_map(u32 eidx_start, u32 eidx_end, u32 cmap)
{
	जर ((eidx_start > eidx_end) || eidx_start >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_end >= SBI_PMU_EVENT_RAW_IDX)
		परत SBI_EINVAL;

	परत pmu_add_hw_event_map(eidx_start, eidx_end, cmap, 0, 0);
}

पूर्णांक sbi_pmu_add_raw_event_counter_map(uint64_t select, uint64_t select_mask, u32 cmap)
{
	परत pmu_add_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_mask);
}

शून्य sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_mask());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW);
}

स्थिर पूर्णांक pmu_ctr_enable_irq_hw(पूर्णांक ctr_idx)
{
	अचिन्हित दीर्घ mhpmevent_csr;
	अचिन्हित दीर्घ mhpmevent_curr;
	अचिन्हित दीर्घ mip_val;
	अचिन्हित दीर्घ of_mask;

	जर (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		परत SBI_EFAIL;

#जर __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	of_mask = (uint32_t)~MHPMEVENTH_OF;
#अन्यथा
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	of_mask = ~MHPMEVENT_OF;
#endif

	mhpmevent_curr = csr_read_अंक(mhpmevent_csr);
	mip_val = csr_read(CSR_MIP);
	/**
	 * Clear out the OF bit so that next interrupt can be enabled.
	 * This should be done only when the corresponding overflow interrupt
	 * bit is cleared. That indicates that software has already handled the
	 * previous interrupts or the hardware yet to set an overflow interrupt.
	 * Otherwise, there will be race conditions where we may clear the bit
	 * the software is yet to handle the interrupt.
	 */
	जर (!(mip_val & sbi_pmu_irq_mask())) {
		mhpmevent_curr &= of_mask;
		csr_write_अंक(mhpmevent_csr, mhpmevent_curr);
	}

	परत 0;
}

स्थिर शून्य pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#जर __riscv_xlen == 32
	csr_write_अंक(CSR_MCYCLE + cidx, 0);
	csr_write_अंक(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_अंक(CSR_MCYCLEH + cidx, ival >> BITS_PER_LONG);
#अन्यथा
	csr_write_अंक(CSR_MCYCLE + cidx, ival);
#endif
}

स्थिर पूर्णांक pmu_ctr_start_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	अचिन्हित दीर्घ mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	जर (cidx >= अंक_hw_ctrs || cidx == 1)
		परत SBI_EINVAL;

	जर (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11) {
		जर (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		परत 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	जर (!__test_bit(cidx, &mctr_inhbt))
		परत SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	जर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	जर (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	जर (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	परत 0;
}

पूर्णांक sbi_pmu_irq_bit(शून्य)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	जर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		परत IRQ_PMU_OVF;
	जर (pmu_dev && pmu_dev->hw_counter_irq_bit)
		परत pmu_dev->hw_counter_irq_bit();

	परत -1;
}

अचिन्हित दीर्घ sbi_pmu_irq_mask(शून्य)
{
	पूर्णांक irq_bit = sbi_pmu_irq_bit();

	जर (irq_bit < 0)
		परत 0;

	परत BIT(irq_bit);
}

स्थिर पूर्णांक pmu_ctr_start_fw(संरचना sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_code,
			    uint64_t event_data, uint64_t ival,
			    bool ival_update)
{
	पूर्णांक ret;

	जर ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		परत SBI_EINVAL;

	जर (phs->fw_counters_started & BIT(cidx - अंक_hw_ctrs))
		परत SBI_EALREADY_STARTED;

	जर (SBI_PMU_FW_PLATFORM == event_code) {
		जर (!pmu_dev ||
		    !pmu_dev->fw_counter_write_मूल्य ||
		    !pmu_dev->fw_counter_start) {
			परत SBI_EINVAL;
		}

		जर (ival_update)
			pmu_dev->fw_counter_write_मूल्य(phs->hartid,
							cidx - अंक_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_start(phs->hartid,
						 cidx - अंक_hw_ctrs,
						 event_data);
		जर (ret)
			परत ret;
	} अन्यथा {
		जर (ival_update)
			phs->fw_counters_data[cidx - अंक_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= BIT(cidx - अंक_hw_ctrs);

	परत 0;
}

स्थिर शून्य pmu_update_inhibit_flags(अचिन्हित दीर्घ flags, uint64_t *mhpmevent_val)
{
	जर (flags & SBI_PMU_CFG_FLAG_SET_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	जर (flags & SBI_PMU_CFG_FLAG_SET_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	जर (flags & SBI_PMU_CFG_FLAG_SET_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	जर (flags & SBI_PMU_CFG_FLAG_SET_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

स्थिर पूर्णांक pmu_update_hw_mhpmevent(संरचना sbi_pmu_hw_event *hw_evt, पूर्णांक ctr_idx,
				   अचिन्हित दीर्घ flags, अचिन्हित दीर्घ eindex,
				   uint64_t data)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	स्थिरांक संरचना sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_to_mhpmevent(plat, eindex, data);

	जर (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		परत SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	जर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_MASK) |
				 MHPMEVENT_MINH | MHPMEVENT_OF;

	जर (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	जर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	जर (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, ctr_idx);

#जर __riscv_xlen == 32
	csr_write_अंक(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	जर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_अंक(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> BITS_PER_LONG);
#अन्यथा
	csr_write_अंक(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#endif

	परत 0;
}

पूर्णांक sbi_pmu_ctr_start(अचिन्हित दीर्घ cbase, अचिन्हित दीर्घ cmask,
		      अचिन्हित दीर्घ flags, uint64_t ival)
{
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	जर (unlikely(!phs))
		परत SBI_EINVAL;

	पूर्णांक event_idx_type;
	uint32_t event_code;
	पूर्णांक ret = SBI_EINVAL;
	bool bUpdate = false;
	पूर्णांक i, cidx;
	uint64_t edata;

	जर (!pmu_ctr_idx_validate(cbase, cmask))
		परत ret;

	जर (flags & ~SBI_PMU_START_FLAGS_MASK)
		परत SBI_ERR_INVALID_PARAM;

	जर (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		परत SBI_ENO_SHMEM;

	जर (flags & SBI_PMU_START_FLAG_SET_INIT_मूल्य)
		bUpdate = true;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		जर (event_idx_type < 0)
			/* Continue the start operation for other counters */
			सुरूठेवा;
		अन्यथा जर (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_code == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_data[cidx - अंक_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_start_fw(phs, cidx, event_code, edata,
					       ival, bUpdate);
			जर (ret)
				परत ret;
		} अन्यथा {
			जर (cidx >= 3) {
				संरचना sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->active_events[cidx],
							ev_cfg->event_data);
				जर (ret)
					परत ret;
			}
			ret = pmu_ctr_start_hw(cidx, ival, bUpdate);
			जर (ret)
				परत ret;
		}
	}

	परत ret;
}

स्थिर पूर्णांक pmu_ctr_stop_hw(uint32_t cidx)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	अचिन्हित दीर्घ mctr_inhbt;

	जर (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11)
		परत 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	जर (cidx >= अंक_hw_ctrs || cidx == 1)
		परत SBI_EINVAL;

	जर (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		जर (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		परत 0;
	} अन्यथा
		परत SBI_EALREADY_STOPPED;
}

स्थिर पूर्णांक pmu_ctr_stop_fw(संरचना sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_code)
{
	पूर्णांक ret;

	जर ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		परत SBI_EINVAL;

	जर (!(phs->fw_counters_started & BIT(cidx - अंक_hw_ctrs)))
		परत SBI_EALREADY_STOPPED;

	जर (SBI_PMU_FW_PLATFORM == event_code &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - अंक_hw_ctrs);
		जर (ret)
			परत ret;
	}

	phs->fw_counters_started &= ~BIT(cidx - अंक_hw_ctrs);

	परत 0;
}

स्थिर पूर्णांक pmu_reset_hw_mhpmevent(पूर्णांक ctr_idx)
{
	जर (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		परत SBI_EFAIL;
#जर __riscv_xlen == 32
	csr_write_अंक(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	जर (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_अंक(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#अन्यथा
	csr_write_अंक(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#endif

	परत 0;
}

पूर्णांक sbi_pmu_ctr_stop(अचिन्हित दीर्घ cbase, अचिन्हित दीर्घ cmask,
		     अचिन्हित दीर्घ flag)
{
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	जर (unlikely(!phs))
		परत SBI_EINVAL;

	पूर्णांक ret = SBI_EINVAL;
	पूर्णांक event_idx_type;
	uint32_t event_code;
	पूर्णांक i, cidx;

	जर (!pmu_ctr_idx_validate(cbase, cmask))
		परत ret;

	जर (flag & ~SBI_PMU_STOP_FLAGS_MASK)
		परत SBI_ERR_INVALID_PARAM;

	जर (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		परत SBI_ENO_SHMEM;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		जर (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			सुरूठेवा;

		अन्यथा जर (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_code);
		अन्यथा
			ret = pmu_ctr_stop_hw(cidx);

		जर(ret)
			परत ret;

		जर (cidx > (CSR_INSTRET - CSR_CYCLE) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->active_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	जर (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_mask());

	परत ret;
}

स्थिर पूर्णांक pmu_fixed_ctr_update_inhibit_bits(पूर्णांक fixed_ctr, अचिन्हित दीर्घ flags)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#जर __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#endif
	जर (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_mode))
		परत fixed_ctr;

	निवड (fixed_ctr) {
	स्थिती 0:
		cfg_csr_no = CSR_MCYCLECFG;
#जर __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#endif
		थांबा;
	स्थिती 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#जर __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#endif
		थांबा;
	पूर्वनिर्धारित:
		परत SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	जर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#जर __riscv_xlen == 32
		csr_write_अंक(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_अंक(cfgh_csr_no, cfg_val >> BITS_PER_LONG);
#अन्यथा
		csr_write_अंक(cfg_csr_no, cfg_val);
#endif
	}
	जर (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, fixed_ctr);
	परत fixed_ctr;
}

स्थिर पूर्णांक pmu_ctr_find_fixed_hw(अचिन्हित दीर्घ evt_idx_code)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	जर (evt_idx_code == SBI_PMU_HW_CPU_CYCLES)
		परत 0;
	अन्यथा जर (evt_idx_code == SBI_PMU_HW_INSTRUCTIONS)
		परत 2;
	अन्यथा
		परत SBI_EINVAL;
}

स्थिर पूर्णांक pmu_ctr_find_hw(संरचना sbi_pmu_hart_state *phs,
			   अचिन्हित दीर्घ cbase, अचिन्हित दीर्घ cmask,
			   अचिन्हित दीर्घ flags,
			   अचिन्हित दीर्घ event_idx, uint64_t data)
{
	अचिन्हित दीर्घ ctr_mask;
	पूर्णांक i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	संरचना sbi_pmu_hw_event *temp;
	अचिन्हित दीर्घ mctr_inhbt = 0;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	जर (cbase >= अंक_hw_ctrs)
		परत SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_find_fixed_hw(event_idx);
	जर (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		परत pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);

	जर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	साठी (i = 0; i < अंक_hw_events; i++) {
		temp = &hw_event_map[i];
		जर ((temp->start_idx > event_idx && event_idx < temp->end_idx) ||
		    (temp->start_idx < event_idx && event_idx > temp->end_idx))
			सुरूठेवा;

		/* For raw events, event data is used as the select value */
		जर (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_mask = temp->select_mask;

			/* The non-event map bits of data should match the selector */
			जर (temp->select != (data & select_mask))
				सुरूठेवा;
		}
		/* Fixed counters should not be part of the search */
		ctr_mask = temp->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_MASK);
		for_each_set_bit_from(cbase, &ctr_mask, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			जर (phs->active_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				सुरूठेवा;
			/* If mcountinhibit is supported, the bit must be enabled */
			जर ((sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				सुरूठेवा;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			थांबा;
		}
	}

	जर (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		जर (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			परत SBI_EFAIL;

		परत pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(temp, ctr_idx, flags, event_idx, data);

	जर (!ret)
		ret = ctr_idx;

	परत ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
स्थिर पूर्णांक pmu_ctr_find_fw(संरचना sbi_pmu_hart_state *phs,
			   अचिन्हित दीर्घ cbase, अचिन्हित दीर्घ cmask,
			   uint32_t event_code, uint64_t edata)
{
	पूर्णांक i, cidx;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		जर (cidx < अंक_hw_ctrs || total_ctrs <= cidx)
			सुरूठेवा;
		जर (phs->active_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			सुरूठेवा;
		जर (SBI_PMU_FW_PLATFORM == event_code &&
		    pmu_dev && pmu_dev->fw_counter_match_encoding) {
			जर (!pmu_dev->fw_counter_match_encoding(phs->hartid,
							    cidx - अंक_hw_ctrs,
							    edata))
				सुरूठेवा;
		}

		परत cidx;
	}

	परत SBI_ENOTSUPP;
}

पूर्णांक sbi_pmu_ctr_cfg_match(अचिन्हित दीर्घ cidx_base, अचिन्हित दीर्घ cidx_mask,
			  अचिन्हित दीर्घ flags, अचिन्हित दीर्घ event_idx,
			  uint64_t event_data)
{
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	जर (unlikely(!phs))
		परत SBI_EINVAL;

	पूर्णांक ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_code;

	जर (!pmu_ctr_idx_validate(cidx_base, cidx_mask))
		परत SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_data);
	जर (event_type < 0)
		परत SBI_EINVAL;
	event_code = get_cidx_code(event_idx);

	जर (flags & SBI_PMU_CFG_FLAG_SKIP_MATCH) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		अचिन्हित दीर्घ cidx_first = cidx_base + sbi_ffs(cidx_mask);

		जर (cidx_first >= total_ctrs)
			परत SBI_EINVAL;

		जर (phs->active_events[cidx_first] == SBI_PMU_EVENT_IDX_INVALID)
			परत SBI_EINVAL;
		ctr_idx = cidx_first;
		जा skip_match;
	}

	जर (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_find_fw(phs, cidx_base, cidx_mask,
					  event_code, event_data);
		जर ((event_code == SBI_PMU_FW_PLATFORM) && (ctr_idx >= अंक_hw_ctrs))
			phs->fw_counters_data[ctr_idx - अंक_hw_ctrs] =
								event_data;
	} अन्यथा {
		ctr_idx = pmu_ctr_find_hw(phs, cidx_base, cidx_mask, flags,
					  event_idx, event_data);
		जर (ctr_idx >= 0) {
			संरचना sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_data = event_data;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_MASK;
		}
	}

	जर (ctr_idx < 0)
		परत SBI_ENOTSUPP;

	phs->active_events[ctr_idx] = event_idx;
skip_match:
	जर (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		जर (flags & SBI_PMU_CFG_FLAG_CLEAR_मूल्य)
			pmu_ctr_write_hw(ctr_idx, 0);
		जर (flags & SBI_PMU_CFG_FLAG_AUTO_START)
			pmu_ctr_start_hw(ctr_idx, 0, false);
	} अन्यथा जर (event_type == SBI_PMU_EVENT_TYPE_FW) {
		जर (flags & SBI_PMU_CFG_FLAG_CLEAR_मूल्य)
			phs->fw_counters_data[ctr_idx - अंक_hw_ctrs] = 0;
		जर (flags & SBI_PMU_CFG_FLAG_AUTO_START) {
			जर (SBI_PMU_FW_PLATFORM == event_code &&
			    pmu_dev && pmu_dev->fw_counter_start) {
				ret = pmu_dev->fw_counter_start(
					phs->hartid,
					ctr_idx - अंक_hw_ctrs, event_data);
				जर (ret)
					परत ret;
			}
			phs->fw_counters_started |= BIT(ctr_idx - अंक_hw_ctrs);
		}
	}

	परत ctr_idx;
}

पूर्णांक sbi_pmu_ctr_incr_fw(गणना sbi_pmu_fw_event_code_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = रिक्त;
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	जर (unlikely(!phs))
		परत 0;

	जर (likely(!phs->fw_counters_started))
		परत 0;

	जर (unlikely(fw_id >= SBI_PMU_FW_MAX))
		परत SBI_EINVAL;

	साठी (cidx = अंक_hw_ctrs; cidx < total_ctrs; cidx++) {
		जर (get_cidx_code(phs->active_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & BIT(cidx - अंक_hw_ctrs))) {
			fcounter = &phs->fw_counters_data[cidx - अंक_hw_ctrs];
			थांबा;
		}
	}

	जर (fcounter)
		(*fcounter)++;

	परत 0;
}

अचिन्हित दीर्घ sbi_pmu_अंक_ctr(शून्य)
{
	परत (अंक_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

पूर्णांक sbi_pmu_ctr_get_info(uint32_t cidx, अचिन्हित दीर्घ *ctr_info)
{
	पूर्णांक width;
	संघ sbi_pmu_ctr_info cinfo = {0};
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	अचिन्हित दीर्घ counter_mask = (अचिन्हित दीर्घ)sbi_hart_mhpm_mask(scratch) |
				     SBI_PMU_CY_IR_MASK;

	/* Sanity check */
	जर (cidx >= total_ctrs)
		परत SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	जर (cidx < अंक_hw_ctrs) {
		जर (!(__test_bit(cidx, &counter_mask)))
			परत SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_CYCLE + cidx;
		/* mcycle & minstret are always 64 bit */
		जर (cidx == 0 || cidx == 2)
			cinfo.width = 63;
		अन्यथा
			cinfo.width = sbi_hart_mhpm_bits(scratch) - 1;
	} अन्यथा {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.width = 63;
		जर (pmu_dev && pmu_dev->fw_counter_width) {
			width = pmu_dev->fw_counter_width();
			जर (width)
				cinfo.width = width - 1;
		}
	}

	*ctr_info = cinfo.मूल्य;

	परत 0;
}

पूर्णांक sbi_pmu_event_get_info(अचिन्हित दीर्घ shmem_phys_lo, अचिन्हित दीर्घ shmem_phys_hi,
			   अचिन्हित दीर्घ अंक_events, अचिन्हित दीर्घ flags)
{
	अचिन्हित दीर्घ shmem_size = अंक_events * आकार(संरचना sbi_pmu_event_info);
	पूर्णांक i, j, event_type;
	संरचना sbi_pmu_event_info *einfo;
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	संरचना sbi_pmu_hw_event *temp;
	bool found = false;

	जर (flags != 0)
		परत SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	जर (!अंक_events || (shmem_phys_lo & 0xF))
		परत SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	जर (shmem_phys_hi)
		परत SBI_EINVALID_ADDR;

	जर (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_size, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		परत SBI_ERR_INVALID_ADDRESS;

	sbi_hart_protection_temp_map_range(shmem_phys_lo, shmem_size);

	einfo = (संरचना sbi_pmu_event_info *)(shmem_phys_lo);
	साठी (i = 0; i < अंक_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		जर (event_idx & SBI_PMU_EVENT_IDX_MBZ_MASK)
			परत SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_data);
		जर (event_type < 0) {
			einfo[i].output = 0;
		} अन्यथा जर (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} अन्यथा {
			साठी (j = 0; j < अंक_hw_events; j++) {
				temp = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				जर (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					जर (temp->start_idx > event_idx ||
					    event_idx > temp->end_idx)
						सुरूठेवा;
					/* just match the selector */
					जर (temp->select == (einfo[i].event_data &
									temp->select_mask)) {
						found = true;
						थांबा;
					}
				} अन्यथा जर (temp->start_idx <= event_idx &&
					   event_idx <= temp->end_idx) {
					found = true;
					थांबा;
				}
			}
			जर (found)
				einfo[i].output = 1;
			अन्यथा
				einfo[i].output = 0;
			found = false;
		}
	}

	sbi_hart_protection_temp_unmap_range(shmem_phys_lo, shmem_size);

	परत 0;
}

स्थिर शून्य pmu_reset_event_map(संरचना sbi_pmu_hart_state *phs)
{
	पूर्णांक j;

	/* Initialize the counter to event mapping table */
	साठी (j = 3; j < total_ctrs; j++)
		phs->active_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	साठी (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_data[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

स्थिरांक संरचना sbi_pmu_device *sbi_pmu_get_device(शून्य)
{
	परत pmu_dev;
}

शून्य sbi_pmu_set_device(स्थिरांक संरचना sbi_pmu_device *dev)
{
	जर (!dev || pmu_dev)
		परत;

	pmu_dev = dev;
}

शून्य sbi_pmu_exit(संरचना sbi_scratch *scratch)
{
	संरचना sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	जर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	जर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	जर (unlikely(!phs))
		परत;

	pmu_reset_event_map(phs);
}

स्थिर शून्य pmu_sse_enable(uint32_t event_id)
{
	अचिन्हित दीर्घ irq_mask = sbi_pmu_irq_mask();

	csr_set(CSR_MIE, irq_mask);
}

स्थिर शून्य pmu_sse_disable(uint32_t event_id)
{
	अचिन्हित दीर्घ irq_mask = sbi_pmu_irq_mask();

	csr_clear(CSR_MIE, irq_mask);
	csr_clear(CSR_MIP, irq_mask);
}

स्थिर शून्य pmu_sse_complete(uint32_t event_id)
{
	csr_set(CSR_MIE, sbi_pmu_irq_mask());
}

स्थिर शून्य pmu_sse_register(uint32_t event_id)
{
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_mask());
}

स्थिर शून्य pmu_sse_unregister(uint32_t event_id)
{
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_set(CSR_MIDELEG, sbi_pmu_irq_mask());
}

स्थिर स्थिरांक संरचना sbi_sse_cb_ops pmu_sse_cb_ops = {
	.register_cb = pmu_sse_register,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

पूर्णांक sbi_pmu_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	पूर्णांक hpm_मोजणी = sbi_fls(sbi_hart_mhpm_mask(scratch));
	संरचना sbi_pmu_hart_state *phs;
	स्थिरांक संरचना sbi_platform *plat;
	पूर्णांक rc;

	जर (cold_boot) {
		hw_event_map = sbi_calloc(आकार(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		जर (!hw_event_map)
			परत SBI_ENOMEM;

		phs_ptr_offset = sbi_scratch_alloc_type_offset(शून्य *);
		जर (!phs_ptr_offset) {
			sbi_free(hw_event_map);
			परत SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		जर (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		जर (!hpm_मोजणी)
			/* Only CY, TM & IR are implemented in the hw */
			अंक_hw_ctrs = 3;
		अन्यथा
			अंक_hw_ctrs = hpm_मोजणी + 1;

		जर (अंक_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			परत SBI_EINVAL;

		total_ctrs = अंक_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		जर (sbi_pmu_irq_bit() >= 0)
			sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	जर (!phs) {
		phs = sbi_zalloc(आकार(*phs));
		जर (!phs)
			परत SBI_ENOMEM;
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

	परत 0;
}
