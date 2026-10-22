/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_hart_protection.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_pmu.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_sse.h>

/** Information about hardware counters */
काष्ठा sbi_pmu_hw_event {
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
जोड़ sbi_pmu_ctr_info {
	अचिन्हित दीर्घ मूल्य;
	काष्ठा {
		अचिन्हित दीर्घ csr:12;
		अचिन्हित दीर्घ width:6;
#अगर __riscv_xlen == 32
		अचिन्हित दीर्घ reserved:13;
#अन्यथा
		अचिन्हित दीर्घ reserved:45;
#पूर्ण_अगर
		अचिन्हित दीर्घ type:1;
	};
};

#अगर SBI_PMU_FW_CTR_MAX >= BITS_PER_LONG
#त्रुटि "Can't handle firmware counters beyond BITS_PER_LONG"
#पूर्ण_अगर

/** HW event configuration parameters */
काष्ठा sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_तथ्य;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
काष्ठा sbi_pmu_hart_state {
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
	uint64_t fw_counters_तथ्य[SBI_PMU_FW_CTR_MAX];
	/* HW events configuration parameters from
	 * sbi_pmu_ctr_cfg_match() command which are
	 * used for restoring RAW hardware events after
	 * cpu suspending.
	 */
	काष्ठा sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
अटल अचिन्हित दीर्घ phs_ptr_offset;

#घोषणा pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_offset ? sbi_scratch_read_type((__scratch), व्योम *, phs_ptr_offset) : शून्य

#घोषणा pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#घोषणा pmu_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), व्योम *, phs_ptr_offset, (__phs))

/* Platform specific PMU device */
अटल स्थिर काष्ठा sbi_pmu_device *pmu_dev = शून्य;

/* Mapping between event range and possible counters  */
अटल काष्ठा sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
अटल uint32_t अंक_hw_events;
/* Maximum number of hardware counters available */
अटल uint32_t अंक_hw_ctrs;

/* Maximum number of counters available */
अटल uint32_t योग_ctrs;

/* Helper macros to retrieve event idx and code type */
#घोषणा get_cidx_type(x) \
  (((x) & SBI_PMU_EVENT_IDX_TYPE_MASK) >> SBI_PMU_EVENT_IDX_TYPE_OFFSET)
#घोषणा get_cidx_code(x) (x & SBI_PMU_EVENT_IDX_CODE_MASK)

/**
 * Perform a sanity check on event & counter mappings with event range overlap check
 * @param evtA Pointer to the existing hw event structure
 * @param evtB Pointer to the new hw event structure
 *
 * Return false if the range doesn't overlap, true otherwise
 */
अटल bool pmu_event_range_overlap(काष्ठा sbi_pmu_hw_event *evtA,
				    काष्ठा sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	अगर (((evtA->end_idx < evtB->start_idx) && (evtA->end_idx < evtB->end_idx)) ||
	   ((evtA->start_idx > evtB->start_idx) && (evtA->start_idx > evtB->end_idx)))
		वापस false;
	वापस true;
}

अटल bool pmu_event_select_overlap(काष्ठा sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_mask)
{
	अगर ((evt->select == select_val) && (evt->select_mask == select_mask))
		वापस true;

	वापस false;
}

अटल पूर्णांक pmu_event_validate(काष्ठा sbi_pmu_hart_state *phs,
			      अचिन्हित दीर्घ event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_code = get_cidx_code(event_idx);
	uint32_t event_idx_code_max = -1;
	uint32_t cache_ops_परिणाम, cache_ops_id, cache_id;

	चयन(event_idx_type) {
	हाल SBI_PMU_EVENT_TYPE_HW:
		event_idx_code_max = SBI_PMU_HW_GENERAL_MAX;
		अवरोध;
	हाल SBI_PMU_EVENT_TYPE_FW:
		अगर ((event_idx_code >= SBI_PMU_FW_MAX &&
		    event_idx_code <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_code > SBI_PMU_FW_PLATFORM)
			वापस SBI_EINVAL;

		अगर (SBI_PMU_FW_PLATFORM == event_idx_code &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			वापस pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		अन्यथा
			event_idx_code_max = SBI_PMU_FW_MAX;
		अवरोध;
	हाल SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_परिणाम = event_idx_code &
					SBI_PMU_EVENT_HW_CACHE_OPS_परिणाम;
		cache_ops_id = (event_idx_code &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_MASK) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_OFFSET;
		cache_id = (event_idx_code &
			    SBI_PMU_EVENT_HW_CACHE_ID_MASK) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_OFFSET;
		अगर ((cache_ops_परिणाम < SBI_PMU_HW_CACHE_परिणाम_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			वापस event_idx_type;
		अन्यथा
			वापस SBI_EINVAL;
		अवरोध;
	हाल SBI_PMU_EVENT_TYPE_HW_RAW:
	हाल SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_code_max = 1; // event_idx.code should be zero
		अवरोध;
	शेष:
		वापस SBI_EINVAL;
	}

	अगर (event_idx_code < event_idx_code_max)
		वापस event_idx_type;

	वापस SBI_EINVAL;
}

अटल पूर्णांक pmu_ctr_validate(काष्ठा sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_code)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	अगर (cidx >= योग_ctrs)
		वापस SBI_EINVAL;

	event_idx_val = phs->active_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	अगर (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		वापस SBI_EINVAL;

	*event_idx_code = get_cidx_code(event_idx_val);

	वापस event_idx_type;
}

अटल bool pmu_ctr_idx_validate(अचिन्हित दीर्घ cbase, अचिन्हित दीर्घ cmask)
{
	अचिन्हित दीर्घ last;

	अगर (!cmask)
		वापस false;

	last = sbi_fls(cmask);
	अगर (cbase > -1UL - last)
		वापस false;

	वापस (cbase + last) < योग_ctrs;
}

पूर्णांक sbi_pmu_ctr_fw_read(अचिन्हित दीर्घ cidx, uint64_t *cval, bool ऊपरी_bits)
{
	पूर्णांक event_idx_type;
	uint32_t event_code;
	काष्ठा sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	अगर (unlikely(!phs))
		वापस SBI_EINVAL;

	अगर (cidx < अंक_hw_ctrs || cidx >= योग_ctrs)
		वापस SBI_EINVAL;

#अगर __riscv_xlen > 32
	अगर (ऊपरी_bits)
		वापस 0;
#पूर्ण_अगर

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
	अगर (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		वापस SBI_EINVAL;

	अगर ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		वापस SBI_EINVAL;

	अगर (SBI_PMU_FW_PLATFORM == event_code) {
		अगर (pmu_dev && pmu_dev->fw_गणक_read_मूल्य)
			*cval = pmu_dev->fw_गणक_read_मूल्य(phs->hartid,
							       cidx -
							       अंक_hw_ctrs);
		अन्यथा
			*cval = 0;
	} अन्यथा
		*cval = phs->fw_counters_तथ्य[cidx - अंक_hw_ctrs];

	वापस 0;
}

अटल पूर्णांक pmu_add_hw_event_map(u32 eidx_start, u32 eidx_end, u32 cmap,
				uint64_t select, uint64_t select_mask)
{
	पूर्णांक i = 0;
	bool is_overlap;
	काष्ठा sbi_pmu_hw_event *event = &hw_event_map[अंक_hw_events];
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_mask = sbi_hart_mhpm_mask(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	अगर (eidx_start > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_MASK))
		वापस SBI_EDENIED;

	अगर (अंक_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		वापस SBI_EFAIL;
	}

	event->start_idx = eidx_start;
	event->end_idx = eidx_end;

	/* Sanity check */
	क्रम (i = 0; i < अंक_hw_events; i++) {
		अगर (eidx_start == SBI_PMU_EVENT_RAW_IDX ||
			eidx_start == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_mask);
		अन्यथा
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		अगर (is_overlap)
			जाओ reset_event;
	}

	event->select_mask = select_mask;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_mask;
	event->select = select;
	अंक_hw_events++;

	वापस 0;

reset_event:
	event->start_idx = 0;
	event->end_idx = 0;
	वापस SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
पूर्णांक sbi_pmu_add_hw_event_गणक_map(u32 eidx_start, u32 eidx_end, u32 cmap)
{
	अगर ((eidx_start > eidx_end) || eidx_start >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_end >= SBI_PMU_EVENT_RAW_IDX)
		वापस SBI_EINVAL;

	वापस pmu_add_hw_event_map(eidx_start, eidx_end, cmap, 0, 0);
}

पूर्णांक sbi_pmu_add_raw_event_गणक_map(uint64_t select, uint64_t select_mask, u32 cmap)
{
	वापस pmu_add_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_mask);
}

व्योम sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_mask());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW);
}

अटल पूर्णांक pmu_ctr_enable_irq_hw(पूर्णांक ctr_idx)
{
	अचिन्हित दीर्घ mhpmevent_csr;
	अचिन्हित दीर्घ mhpmevent_curr;
	अचिन्हित दीर्घ mip_val;
	अचिन्हित दीर्घ of_mask;

	अगर (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		वापस SBI_EFAIL;

#अगर __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	of_mask = (uint32_t)~MHPMEVENTH_OF;
#अन्यथा
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	of_mask = ~MHPMEVENT_OF;
#पूर्ण_अगर

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
	अगर (!(mip_val & sbi_pmu_irq_mask())) {
		mhpmevent_curr &= of_mask;
		csr_write_अंक(mhpmevent_csr, mhpmevent_curr);
	}

	वापस 0;
}

अटल व्योम pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#अगर __riscv_xlen == 32
	csr_write_अंक(CSR_MCYCLE + cidx, 0);
	csr_write_अंक(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_अंक(CSR_MCYCLEH + cidx, ival >> BITS_PER_LONG);
#अन्यथा
	csr_write_अंक(CSR_MCYCLE + cidx, ival);
#पूर्ण_अगर
}

अटल पूर्णांक pmu_ctr_start_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	अचिन्हित दीर्घ mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	अगर (cidx >= अंक_hw_ctrs || cidx == 1)
		वापस SBI_EINVAL;

	अगर (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11) {
		अगर (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		वापस 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	अगर (!__test_bit(cidx, &mctr_inhbt))
		वापस SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	अगर (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	अगर (pmu_dev && pmu_dev->hw_गणक_enable_irq)
		pmu_dev->hw_गणक_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	वापस 0;
}

पूर्णांक sbi_pmu_irq_bit(व्योम)
{
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		वापस IRQ_PMU_OVF;
	अगर (pmu_dev && pmu_dev->hw_गणक_irq_bit)
		वापस pmu_dev->hw_गणक_irq_bit();

	वापस -1;
}

अचिन्हित दीर्घ sbi_pmu_irq_mask(व्योम)
{
	पूर्णांक irq_bit = sbi_pmu_irq_bit();

	अगर (irq_bit < 0)
		वापस 0;

	वापस BIT(irq_bit);
}

अटल पूर्णांक pmu_ctr_start_fw(काष्ठा sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_code,
			    uint64_t event_तथ्य, uint64_t ival,
			    bool ival_update)
{
	पूर्णांक ret;

	अगर ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		वापस SBI_EINVAL;

	अगर (phs->fw_counters_started & BIT(cidx - अंक_hw_ctrs))
		वापस SBI_EALREADY_STARTED;

	अगर (SBI_PMU_FW_PLATFORM == event_code) {
		अगर (!pmu_dev ||
		    !pmu_dev->fw_गणक_write_मूल्य ||
		    !pmu_dev->fw_गणक_start) {
			वापस SBI_EINVAL;
		}

		अगर (ival_update)
			pmu_dev->fw_गणक_write_मूल्य(phs->hartid,
							cidx - अंक_hw_ctrs,
							ival);

		ret = pmu_dev->fw_गणक_start(phs->hartid,
						 cidx - अंक_hw_ctrs,
						 event_तथ्य);
		अगर (ret)
			वापस ret;
	} अन्यथा {
		अगर (ival_update)
			phs->fw_counters_तथ्य[cidx - अंक_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= BIT(cidx - अंक_hw_ctrs);

	वापस 0;
}

अटल व्योम pmu_update_inhibit_flags(अचिन्हित दीर्घ flags, uint64_t *mhpmevent_val)
{
	अगर (flags & SBI_PMU_CFG_FLAG_SET_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	अगर (flags & SBI_PMU_CFG_FLAG_SET_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	अगर (flags & SBI_PMU_CFG_FLAG_SET_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	अगर (flags & SBI_PMU_CFG_FLAG_SET_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

अटल पूर्णांक pmu_update_hw_mhpmevent(काष्ठा sbi_pmu_hw_event *hw_evt, पूर्णांक ctr_idx,
				   अचिन्हित दीर्घ flags, अचिन्हित दीर्घ eindex,
				   uint64_t तथ्य)
{
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	स्थिर काष्ठा sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_to_mhpmevent(plat, eindex, तथ्य);

	अगर (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		वापस SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_MASK) |
				 MHPMEVENT_MINH | MHPMEVENT_OF;

	अगर (pmu_dev && pmu_dev->hw_गणक_disable_irq)
		pmu_dev->hw_गणक_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	अगर (pmu_dev && pmu_dev->hw_गणक_filter_mode)
		pmu_dev->hw_गणक_filter_mode(flags, ctr_idx);

#अगर __riscv_xlen == 32
	csr_write_अंक(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_अंक(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> BITS_PER_LONG);
#अन्यथा
	csr_write_अंक(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#पूर्ण_अगर

	वापस 0;
}

पूर्णांक sbi_pmu_ctr_start(अचिन्हित दीर्घ cbase, अचिन्हित दीर्घ cmask,
		      अचिन्हित दीर्घ flags, uint64_t ival)
{
	काष्ठा sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	अगर (unlikely(!phs))
		वापस SBI_EINVAL;

	पूर्णांक event_idx_type;
	uint32_t event_code;
	पूर्णांक ret = SBI_EINVAL;
	bool bUpdate = false;
	पूर्णांक i, cidx;
	uint64_t edata;

	अगर (!pmu_ctr_idx_validate(cbase, cmask))
		वापस ret;

	अगर (flags & ~SBI_PMU_START_FLAGS_MASK)
		वापस SBI_ERR_INVALID_PARAM;

	अगर (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		वापस SBI_ENO_SHMEM;

	अगर (flags & SBI_PMU_START_FLAG_SET_INIT_मूल्य)
		bUpdate = true;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		अगर (event_idx_type < 0)
			/* Continue the start operation for other counters */
			जारी;
		अन्यथा अगर (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_code == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_तथ्य[cidx - अंक_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_start_fw(phs, cidx, event_code, edata,
					       ival, bUpdate);
			अगर (ret)
				वापस ret;
		} अन्यथा {
			अगर (cidx >= 3) {
				काष्ठा sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->active_events[cidx],
							ev_cfg->event_तथ्य);
				अगर (ret)
					वापस ret;
			}
			ret = pmu_ctr_start_hw(cidx, ival, bUpdate);
			अगर (ret)
				वापस ret;
		}
	}

	वापस ret;
}

अटल पूर्णांक pmu_ctr_stop_hw(uint32_t cidx)
{
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	अचिन्हित दीर्घ mctr_inhbt;

	अगर (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11)
		वापस 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	अगर (cidx >= अंक_hw_ctrs || cidx == 1)
		वापस SBI_EINVAL;

	अगर (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		अगर (pmu_dev && pmu_dev->hw_गणक_disable_irq) {
			pmu_dev->hw_गणक_disable_irq(cidx);
		}
		वापस 0;
	} अन्यथा
		वापस SBI_EALREADY_STOPPED;
}

अटल पूर्णांक pmu_ctr_stop_fw(काष्ठा sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_code)
{
	पूर्णांक ret;

	अगर ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		वापस SBI_EINVAL;

	अगर (!(phs->fw_counters_started & BIT(cidx - अंक_hw_ctrs)))
		वापस SBI_EALREADY_STOPPED;

	अगर (SBI_PMU_FW_PLATFORM == event_code &&
	    pmu_dev && pmu_dev->fw_गणक_stop) {
		ret = pmu_dev->fw_गणक_stop(phs->hartid, cidx - अंक_hw_ctrs);
		अगर (ret)
			वापस ret;
	}

	phs->fw_counters_started &= ~BIT(cidx - अंक_hw_ctrs);

	वापस 0;
}

अटल पूर्णांक pmu_reset_hw_mhpmevent(पूर्णांक ctr_idx)
{
	अगर (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		वापस SBI_EFAIL;
#अगर __riscv_xlen == 32
	csr_write_अंक(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	अगर (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_अंक(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#अन्यथा
	csr_write_अंक(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#पूर्ण_अगर

	वापस 0;
}

पूर्णांक sbi_pmu_ctr_stop(अचिन्हित दीर्घ cbase, अचिन्हित दीर्घ cmask,
		     अचिन्हित दीर्घ flag)
{
	काष्ठा sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	अगर (unlikely(!phs))
		वापस SBI_EINVAL;

	पूर्णांक ret = SBI_EINVAL;
	पूर्णांक event_idx_type;
	uint32_t event_code;
	पूर्णांक i, cidx;

	अगर (!pmu_ctr_idx_validate(cbase, cmask))
		वापस ret;

	अगर (flag & ~SBI_PMU_STOP_FLAGS_MASK)
		वापस SBI_ERR_INVALID_PARAM;

	अगर (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		वापस SBI_ENO_SHMEM;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		अगर (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			जारी;

		अन्यथा अगर (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_code);
		अन्यथा
			ret = pmu_ctr_stop_hw(cidx);

		अगर(ret)
			वापस ret;

		अगर (cidx > (CSR_INSTRET - CSR_CYCLE) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->active_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	अगर (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_mask());

	वापस ret;
}

अटल पूर्णांक pmu_fixed_ctr_update_inhibit_bits(पूर्णांक fixed_ctr, अचिन्हित दीर्घ flags)
{
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#अगर __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#पूर्ण_अगर
	अगर (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_गणक_filter_mode))
		वापस fixed_ctr;

	चयन (fixed_ctr) {
	हाल 0:
		cfg_csr_no = CSR_MCYCLECFG;
#अगर __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#पूर्ण_अगर
		अवरोध;
	हाल 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#अगर __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#पूर्ण_अगर
		अवरोध;
	शेष:
		वापस SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#अगर __riscv_xlen == 32
		csr_write_अंक(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_अंक(cfgh_csr_no, cfg_val >> BITS_PER_LONG);
#अन्यथा
		csr_write_अंक(cfg_csr_no, cfg_val);
#पूर्ण_अगर
	}
	अगर (pmu_dev && pmu_dev->hw_गणक_filter_mode)
		pmu_dev->hw_गणक_filter_mode(flags, fixed_ctr);
	वापस fixed_ctr;
}

अटल पूर्णांक pmu_ctr_find_fixed_hw(अचिन्हित दीर्घ evt_idx_code)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	अगर (evt_idx_code == SBI_PMU_HW_CPU_CYCLES)
		वापस 0;
	अन्यथा अगर (evt_idx_code == SBI_PMU_HW_INSTRUCTIONS)
		वापस 2;
	अन्यथा
		वापस SBI_EINVAL;
}

अटल पूर्णांक pmu_ctr_find_hw(काष्ठा sbi_pmu_hart_state *phs,
			   अचिन्हित दीर्घ cbase, अचिन्हित दीर्घ cmask,
			   अचिन्हित दीर्घ flags,
			   अचिन्हित दीर्घ event_idx, uint64_t तथ्य)
{
	अचिन्हित दीर्घ ctr_mask;
	पूर्णांक i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	काष्ठा sbi_pmu_hw_event *अस्थायी;
	अचिन्हित दीर्घ mctr_inhbt = 0;
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	अगर (cbase >= अंक_hw_ctrs)
		वापस SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_find_fixed_hw(event_idx);
	अगर (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		वापस pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);

	अगर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	क्रम (i = 0; i < अंक_hw_events; i++) {
		अस्थायी = &hw_event_map[i];
		अगर ((अस्थायी->start_idx > event_idx && event_idx < अस्थायी->end_idx) ||
		    (अस्थायी->start_idx < event_idx && event_idx > अस्थायी->end_idx))
			जारी;

		/* For raw events, event data is used as the select value */
		अगर (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_mask = अस्थायी->select_mask;

			/* The non-event map bits of data should match the selector */
			अगर (अस्थायी->select != (तथ्य & select_mask))
				जारी;
		}
		/* Fixed counters should not be part of the search */
		ctr_mask = अस्थायी->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_MASK);
		for_each_set_bit_from(cbase, &ctr_mask, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			अगर (phs->active_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				जारी;
			/* If mcountinhibit is supported, the bit must be enabled */
			अगर ((sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				जारी;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			अवरोध;
		}
	}

	अगर (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		अगर (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			वापस SBI_EFAIL;

		वापस pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(अस्थायी, ctr_idx, flags, event_idx, तथ्य);

	अगर (!ret)
		ret = ctr_idx;

	वापस ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
अटल पूर्णांक pmu_ctr_find_fw(काष्ठा sbi_pmu_hart_state *phs,
			   अचिन्हित दीर्घ cbase, अचिन्हित दीर्घ cmask,
			   uint32_t event_code, uint64_t edata)
{
	पूर्णांक i, cidx;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		अगर (cidx < अंक_hw_ctrs || योग_ctrs <= cidx)
			जारी;
		अगर (phs->active_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			जारी;
		अगर (SBI_PMU_FW_PLATFORM == event_code &&
		    pmu_dev && pmu_dev->fw_गणक_match_encoding) {
			अगर (!pmu_dev->fw_गणक_match_encoding(phs->hartid,
							    cidx - अंक_hw_ctrs,
							    edata))
				जारी;
		}

		वापस cidx;
	}

	वापस SBI_ENOTSUPP;
}

पूर्णांक sbi_pmu_ctr_cfg_match(अचिन्हित दीर्घ cidx_base, अचिन्हित दीर्घ cidx_mask,
			  अचिन्हित दीर्घ flags, अचिन्हित दीर्घ event_idx,
			  uint64_t event_तथ्य)
{
	काष्ठा sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	अगर (unlikely(!phs))
		वापस SBI_EINVAL;

	पूर्णांक ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_code;

	अगर (!pmu_ctr_idx_validate(cidx_base, cidx_mask))
		वापस SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_तथ्य);
	अगर (event_type < 0)
		वापस SBI_EINVAL;
	event_code = get_cidx_code(event_idx);

	अगर (flags & SBI_PMU_CFG_FLAG_SKIP_MATCH) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		अचिन्हित दीर्घ cidx_first = cidx_base + sbi_ffs(cidx_mask);

		अगर (cidx_first >= योग_ctrs)
			वापस SBI_EINVAL;

		अगर (phs->active_events[cidx_first] == SBI_PMU_EVENT_IDX_INVALID)
			वापस SBI_EINVAL;
		ctr_idx = cidx_first;
		जाओ skip_match;
	}

	अगर (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_find_fw(phs, cidx_base, cidx_mask,
					  event_code, event_तथ्य);
		अगर ((event_code == SBI_PMU_FW_PLATFORM) && (ctr_idx >= अंक_hw_ctrs))
			phs->fw_counters_तथ्य[ctr_idx - अंक_hw_ctrs] =
								event_तथ्य;
	} अन्यथा {
		ctr_idx = pmu_ctr_find_hw(phs, cidx_base, cidx_mask, flags,
					  event_idx, event_तथ्य);
		अगर (ctr_idx >= 0) {
			काष्ठा sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_तथ्य = event_तथ्य;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_MASK;
		}
	}

	अगर (ctr_idx < 0)
		वापस SBI_ENOTSUPP;

	phs->active_events[ctr_idx] = event_idx;
skip_match:
	अगर (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		अगर (flags & SBI_PMU_CFG_FLAG_CLEAR_मूल्य)
			pmu_ctr_write_hw(ctr_idx, 0);
		अगर (flags & SBI_PMU_CFG_FLAG_AUTO_START)
			pmu_ctr_start_hw(ctr_idx, 0, false);
	} अन्यथा अगर (event_type == SBI_PMU_EVENT_TYPE_FW) {
		अगर (flags & SBI_PMU_CFG_FLAG_CLEAR_मूल्य)
			phs->fw_counters_तथ्य[ctr_idx - अंक_hw_ctrs] = 0;
		अगर (flags & SBI_PMU_CFG_FLAG_AUTO_START) {
			अगर (SBI_PMU_FW_PLATFORM == event_code &&
			    pmu_dev && pmu_dev->fw_गणक_start) {
				ret = pmu_dev->fw_गणक_start(
					phs->hartid,
					ctr_idx - अंक_hw_ctrs, event_तथ्य);
				अगर (ret)
					वापस ret;
			}
			phs->fw_counters_started |= BIT(ctr_idx - अंक_hw_ctrs);
		}
	}

	वापस ctr_idx;
}

पूर्णांक sbi_pmu_ctr_incr_fw(क्रमागत sbi_pmu_fw_event_code_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = शून्य;
	काष्ठा sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	अगर (unlikely(!phs))
		वापस 0;

	अगर (likely(!phs->fw_counters_started))
		वापस 0;

	अगर (unlikely(fw_id >= SBI_PMU_FW_MAX))
		वापस SBI_EINVAL;

	क्रम (cidx = अंक_hw_ctrs; cidx < योग_ctrs; cidx++) {
		अगर (get_cidx_code(phs->active_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & BIT(cidx - अंक_hw_ctrs))) {
			fcounter = &phs->fw_counters_तथ्य[cidx - अंक_hw_ctrs];
			अवरोध;
		}
	}

	अगर (fcounter)
		(*fcounter)++;

	वापस 0;
}

अचिन्हित दीर्घ sbi_pmu_अंक_ctr(व्योम)
{
	वापस (अंक_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

पूर्णांक sbi_pmu_ctr_get_info(uint32_t cidx, अचिन्हित दीर्घ *ctr_info)
{
	पूर्णांक width;
	जोड़ sbi_pmu_ctr_info cinfo = {0};
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	अचिन्हित दीर्घ गणक_mask = (अचिन्हित दीर्घ)sbi_hart_mhpm_mask(scratch) |
				     SBI_PMU_CY_IR_MASK;

	/* Sanity check */
	अगर (cidx >= योग_ctrs)
		वापस SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	अगर (cidx < अंक_hw_ctrs) {
		अगर (!(__test_bit(cidx, &गणक_mask)))
			वापस SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_CYCLE + cidx;
		/* mcycle & minstret are always 64 bit */
		अगर (cidx == 0 || cidx == 2)
			cinfo.width = 63;
		अन्यथा
			cinfo.width = sbi_hart_mhpm_bits(scratch) - 1;
	} अन्यथा {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.width = 63;
		अगर (pmu_dev && pmu_dev->fw_गणक_width) {
			width = pmu_dev->fw_गणक_width();
			अगर (width)
				cinfo.width = width - 1;
		}
	}

	*ctr_info = cinfo.मूल्य;

	वापस 0;
}

पूर्णांक sbi_pmu_event_get_info(अचिन्हित दीर्घ shmem_phys_lo, अचिन्हित दीर्घ shmem_phys_hi,
			   अचिन्हित दीर्घ अंक_events, अचिन्हित दीर्घ flags)
{
	अचिन्हित दीर्घ shmem_आकार = अंक_events * माप(काष्ठा sbi_pmu_event_info);
	पूर्णांक i, j, event_type;
	काष्ठा sbi_pmu_event_info *einfo;
	काष्ठा sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	काष्ठा sbi_pmu_hw_event *अस्थायी;
	bool found = false;

	अगर (flags != 0)
		वापस SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	अगर (!अंक_events || (shmem_phys_lo & 0xF))
		वापस SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	अगर (shmem_phys_hi)
		वापस SBI_EINVALID_ADDR;

	अगर (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_आकार, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		वापस SBI_ERR_INVALID_ADDRESS;

	sbi_hart_protection_अस्थायी_map_range(shmem_phys_lo, shmem_आकार);

	einfo = (काष्ठा sbi_pmu_event_info *)(shmem_phys_lo);
	क्रम (i = 0; i < अंक_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		अगर (event_idx & SBI_PMU_EVENT_IDX_MBZ_MASK)
			वापस SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_तथ्य);
		अगर (event_type < 0) {
			einfo[i].output = 0;
		} अन्यथा अगर (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} अन्यथा {
			क्रम (j = 0; j < अंक_hw_events; j++) {
				अस्थायी = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				अगर (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					अगर (अस्थायी->start_idx > event_idx ||
					    event_idx > अस्थायी->end_idx)
						जारी;
					/* just match the selector */
					अगर (अस्थायी->select == (einfo[i].event_तथ्य &
									अस्थायी->select_mask)) {
						found = true;
						अवरोध;
					}
				} अन्यथा अगर (अस्थायी->start_idx <= event_idx &&
					   event_idx <= अस्थायी->end_idx) {
					found = true;
					अवरोध;
				}
			}
			अगर (found)
				einfo[i].output = 1;
			अन्यथा
				einfo[i].output = 0;
			found = false;
		}
	}

	sbi_hart_protection_अस्थायी_unmap_range(shmem_phys_lo, shmem_आकार);

	वापस 0;
}

अटल व्योम pmu_reset_event_map(काष्ठा sbi_pmu_hart_state *phs)
{
	पूर्णांक j;

	/* Initialize the counter to event mapping table */
	क्रम (j = 3; j < योग_ctrs; j++)
		phs->active_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	क्रम (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_तथ्य[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

स्थिर काष्ठा sbi_pmu_device *sbi_pmu_get_device(व्योम)
{
	वापस pmu_dev;
}

व्योम sbi_pmu_set_device(स्थिर काष्ठा sbi_pmu_device *dev)
{
	अगर (!dev || pmu_dev)
		वापस;

	pmu_dev = dev;
}

व्योम sbi_pmu_exit(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	अगर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	अगर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	अगर (unlikely(!phs))
		वापस;

	pmu_reset_event_map(phs);
}

अटल व्योम pmu_sse_enable(uint32_t event_id)
{
	अचिन्हित दीर्घ irq_mask = sbi_pmu_irq_mask();

	csr_set(CSR_MIE, irq_mask);
}

अटल व्योम pmu_sse_disable(uint32_t event_id)
{
	अचिन्हित दीर्घ irq_mask = sbi_pmu_irq_mask();

	csr_clear(CSR_MIE, irq_mask);
	csr_clear(CSR_MIP, irq_mask);
}

अटल व्योम pmu_sse_complete(uint32_t event_id)
{
	csr_set(CSR_MIE, sbi_pmu_irq_mask());
}

अटल व्योम pmu_sse_register(uint32_t event_id)
{
	काष्ठा sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_mask());
}

अटल व्योम pmu_sse_unregister(uint32_t event_id)
{
	काष्ठा sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_set(CSR_MIDELEG, sbi_pmu_irq_mask());
}

अटल स्थिर काष्ठा sbi_sse_cb_ops pmu_sse_cb_ops = {
	.register_cb = pmu_sse_register,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

पूर्णांक sbi_pmu_init(काष्ठा sbi_scratch *scratch, bool cold_boot)
{
	पूर्णांक hpm_गिनती = sbi_fls(sbi_hart_mhpm_mask(scratch));
	काष्ठा sbi_pmu_hart_state *phs;
	स्थिर काष्ठा sbi_platform *plat;
	पूर्णांक rc;

	अगर (cold_boot) {
		hw_event_map = sbi_calloc(माप(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		अगर (!hw_event_map)
			वापस SBI_ENOMEM;

		phs_ptr_offset = sbi_scratch_alloc_type_offset(व्योम *);
		अगर (!phs_ptr_offset) {
			sbi_free(hw_event_map);
			वापस SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		अगर (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		अगर (!hpm_गिनती)
			/* Only CY, TM & IR are implemented in the hw */
			अंक_hw_ctrs = 3;
		अन्यथा
			अंक_hw_ctrs = hpm_गिनती + 1;

		अगर (अंक_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			वापस SBI_EINVAL;

		योग_ctrs = अंक_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		अगर (sbi_pmu_irq_bit() >= 0)
			sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	अगर (!phs) {
		phs = sbi_zalloc(माप(*phs));
		अगर (!phs)
			वापस SBI_ENOMEM;
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

	वापस 0;
}
