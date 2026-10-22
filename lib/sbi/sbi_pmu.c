/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_hart_protection.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_pmu.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_sse.h>

/** Information about hardware counters */
بنية sbi_pmu_hw_event {
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
اتحاد sbi_pmu_ctr_info {
	unsigned long value;
	بنية {
		unsigned long csr:12;
		unsigned long width:6;
#𐡄𐡍 __riscv_xlen == 32
		unsigned long reserved:13;
#وإلا
		unsigned long reserved:45;
#endif
		unsigned long type:1;
	};
};

#𐡄𐡍 SBI_PMU_FW_CTR_MAX >= BITS_PER_LONG
#error "Can't handle firmware counters beyond BITS_PER_LONG"
#endif

/** HW event configuration parameters */
بنية sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_data;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
بنية sbi_pmu_hart_state {
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
	بنية sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
ساكن unsigned long phs_ptr_offset;

#define pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_offset ? sbi_scratch_read_type((__scratch), فراغ *, phs_ptr_offset) : عدم

#define pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define pmu_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), فراغ *, phs_ptr_offset, (__phs))

/* Platform specific PMU device */
ساكن ثابت بنية sbi_pmu_device *pmu_dev = عدم;

/* Mapping between event range and possible counters  */
ساكن بنية sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
ساكن uint32_t num_hw_events;
/* Maximum number of hardware counters available */
ساكن uint32_t num_hw_ctrs;

/* Maximum number of counters available */
ساكن uint32_t total_ctrs;

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
ساكن bool pmu_event_range_overlap(بنية sbi_pmu_hw_event *evtA,
				    بنية sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	𐡄𐡍 (((evtA->end_idx < evtB->start_idx) && (evtA->end_idx < evtB->end_idx)) ||
	   ((evtA->start_idx > evtB->start_idx) && (evtA->start_idx > evtB->end_idx)))
		أرجع false;
	أرجع true;
}

ساكن bool pmu_event_select_overlap(بنية sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_mask)
{
	𐡄𐡍 ((evt->select == select_val) && (evt->select_mask == select_mask))
		أرجع true;

	أرجع false;
}

ساكن صحيح pmu_event_validate(بنية sbi_pmu_hart_state *phs,
			      unsigned long event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_code = get_cidx_code(event_idx);
	uint32_t event_idx_code_max = -1;
	uint32_t cache_ops_result, cache_ops_id, cache_id;

	بدل(event_idx_type) {
	حالة SBI_PMU_EVENT_TYPE_HW:
		event_idx_code_max = SBI_PMU_HW_GENERAL_MAX;
		اكسر;
	حالة SBI_PMU_EVENT_TYPE_FW:
		𐡄𐡍 ((event_idx_code >= SBI_PMU_FW_MAX &&
		    event_idx_code <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_code > SBI_PMU_FW_PLATFORM)
			أرجع SBI_EINVAL;

		𐡄𐡍 (SBI_PMU_FW_PLATFORM == event_idx_code &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			أرجع pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		وإلا
			event_idx_code_max = SBI_PMU_FW_MAX;
		اكسر;
	حالة SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_result = event_idx_code &
					SBI_PMU_EVENT_HW_CACHE_OPS_RESULT;
		cache_ops_id = (event_idx_code &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_MASK) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_OFFSET;
		cache_id = (event_idx_code &
			    SBI_PMU_EVENT_HW_CACHE_ID_MASK) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_OFFSET;
		𐡄𐡍 ((cache_ops_result < SBI_PMU_HW_CACHE_RESULT_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			أرجع event_idx_type;
		وإلا
			أرجع SBI_EINVAL;
		اكسر;
	حالة SBI_PMU_EVENT_TYPE_HW_RAW:
	حالة SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_code_max = 1; // event_idx.code should be zero
		اكسر;
	افتراضي:
		أرجع SBI_EINVAL;
	}

	𐡄𐡍 (event_idx_code < event_idx_code_max)
		أرجع event_idx_type;

	أرجع SBI_EINVAL;
}

ساكن صحيح pmu_ctr_validate(بنية sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_code)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	𐡄𐡍 (cidx >= total_ctrs)
		أرجع SBI_EINVAL;

	event_idx_val = phs->active_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	𐡄𐡍 (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		أرجع SBI_EINVAL;

	*event_idx_code = get_cidx_code(event_idx_val);

	أرجع event_idx_type;
}

ساكن bool pmu_ctr_idx_validate(unsigned long cbase, unsigned long cmask)
{
	unsigned long last;

	𐡄𐡍 (!cmask)
		أرجع false;

	last = sbi_fls(cmask);
	𐡄𐡍 (cbase > -1UL - last)
		أرجع false;

	أرجع (cbase + last) < total_ctrs;
}

صحيح sbi_pmu_ctr_fw_read(unsigned long cidx, uint64_t *cval, bool high_bits)
{
	صحيح event_idx_type;
	uint32_t event_code;
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	𐡄𐡍 (unlikely(!phs))
		أرجع SBI_EINVAL;

	𐡄𐡍 (cidx < num_hw_ctrs || cidx >= total_ctrs)
		أرجع SBI_EINVAL;

#𐡄𐡍 __riscv_xlen > 32
	𐡄𐡍 (high_bits)
		أرجع 0;
#endif

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
	𐡄𐡍 (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		أرجع SBI_EINVAL;

	𐡄𐡍 ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		أرجع SBI_EINVAL;

	𐡄𐡍 (SBI_PMU_FW_PLATFORM == event_code) {
		𐡄𐡍 (pmu_dev && pmu_dev->fw_counter_read_value)
			*cval = pmu_dev->fw_counter_read_value(phs->hartid,
							       cidx -
							       num_hw_ctrs);
		وإلا
			*cval = 0;
	} وإلا
		*cval = phs->fw_counters_data[cidx - num_hw_ctrs];

	أرجع 0;
}

ساكن صحيح pmu_add_hw_event_map(u32 eidx_start, u32 eidx_end, u32 cmap,
				uint64_t select, uint64_t select_mask)
{
	صحيح i = 0;
	bool is_overlap;
	بنية sbi_pmu_hw_event *event = &hw_event_map[num_hw_events];
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_mask = sbi_hart_mhpm_mask(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	𐡄𐡍 (eidx_start > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_MASK))
		أرجع SBI_EDENIED;

	𐡄𐡍 (num_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		أرجع SBI_EFAIL;
	}

	event->start_idx = eidx_start;
	event->end_idx = eidx_end;

	/* Sanity check */
	لكل (i = 0; i < num_hw_events; i++) {
		𐡄𐡍 (eidx_start == SBI_PMU_EVENT_RAW_IDX ||
			eidx_start == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_mask);
		وإلا
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		𐡄𐡍 (is_overlap)
			اذهب reset_event;
	}

	event->select_mask = select_mask;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_mask;
	event->select = select;
	num_hw_events++;

	أرجع 0;

reset_event:
	event->start_idx = 0;
	event->end_idx = 0;
	أرجع SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
صحيح sbi_pmu_add_hw_event_counter_map(u32 eidx_start, u32 eidx_end, u32 cmap)
{
	𐡄𐡍 ((eidx_start > eidx_end) || eidx_start >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_end >= SBI_PMU_EVENT_RAW_IDX)
		أرجع SBI_EINVAL;

	أرجع pmu_add_hw_event_map(eidx_start, eidx_end, cmap, 0, 0);
}

صحيح sbi_pmu_add_raw_event_counter_map(uint64_t select, uint64_t select_mask, u32 cmap)
{
	أرجع pmu_add_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_mask);
}

فراغ sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_mask());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW);
}

ساكن صحيح pmu_ctr_enable_irq_hw(صحيح ctr_idx)
{
	unsigned long mhpmevent_csr;
	unsigned long mhpmevent_curr;
	unsigned long mip_val;
	unsigned long of_mask;

	𐡄𐡍 (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		أرجع SBI_EFAIL;

#𐡄𐡍 __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	of_mask = (uint32_t)~MHPMEVENTH_OF;
#وإلا
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	of_mask = ~MHPMEVENT_OF;
#endif

	mhpmevent_curr = csr_read_num(mhpmevent_csr);
	mip_val = csr_read(CSR_MIP);
	/**
	 * Clear out the OF bit so that next interrupt can be enabled.
	 * This should be done only when the corresponding overflow interrupt
	 * bit is cleared. That indicates that software has already handled the
	 * previous interrupts or the hardware yet to set an overflow interrupt.
	 * Otherwise, there will be race conditions where we may clear the bit
	 * the software is yet to handle the interrupt.
	 */
	𐡄𐡍 (!(mip_val & sbi_pmu_irq_mask())) {
		mhpmevent_curr &= of_mask;
		csr_write_num(mhpmevent_csr, mhpmevent_curr);
	}

	أرجع 0;
}

ساكن فراغ pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#𐡄𐡍 __riscv_xlen == 32
	csr_write_num(CSR_MCYCLE + cidx, 0);
	csr_write_num(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_num(CSR_MCYCLEH + cidx, ival >> BITS_PER_LONG);
#وإلا
	csr_write_num(CSR_MCYCLE + cidx, ival);
#endif
}

ساكن صحيح pmu_ctr_start_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	𐡄𐡍 (cidx >= num_hw_ctrs || cidx == 1)
		أرجع SBI_EINVAL;

	𐡄𐡍 (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11) {
		𐡄𐡍 (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		أرجع 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	𐡄𐡍 (!__test_bit(cidx, &mctr_inhbt))
		أرجع SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	𐡄𐡍 (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	𐡄𐡍 (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	𐡄𐡍 (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	أرجع 0;
}

صحيح sbi_pmu_irq_bit(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	𐡄𐡍 (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		أرجع IRQ_PMU_OVF;
	𐡄𐡍 (pmu_dev && pmu_dev->hw_counter_irq_bit)
		أرجع pmu_dev->hw_counter_irq_bit();

	أرجع -1;
}

unsigned long sbi_pmu_irq_mask(فراغ)
{
	صحيح irq_bit = sbi_pmu_irq_bit();

	𐡄𐡍 (irq_bit < 0)
		أرجع 0;

	أرجع BIT(irq_bit);
}

ساكن صحيح pmu_ctr_start_fw(بنية sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_code,
			    uint64_t event_data, uint64_t ival,
			    bool ival_update)
{
	صحيح ret;

	𐡄𐡍 ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		أرجع SBI_EINVAL;

	𐡄𐡍 (phs->fw_counters_started & BIT(cidx - num_hw_ctrs))
		أرجع SBI_EALREADY_STARTED;

	𐡄𐡍 (SBI_PMU_FW_PLATFORM == event_code) {
		𐡄𐡍 (!pmu_dev ||
		    !pmu_dev->fw_counter_write_value ||
		    !pmu_dev->fw_counter_start) {
			أرجع SBI_EINVAL;
		}

		𐡄𐡍 (ival_update)
			pmu_dev->fw_counter_write_value(phs->hartid,
							cidx - num_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_start(phs->hartid,
						 cidx - num_hw_ctrs,
						 event_data);
		𐡄𐡍 (ret)
			أرجع ret;
	} وإلا {
		𐡄𐡍 (ival_update)
			phs->fw_counters_data[cidx - num_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= BIT(cidx - num_hw_ctrs);

	أرجع 0;
}

ساكن فراغ pmu_update_inhibit_flags(unsigned long flags, uint64_t *mhpmevent_val)
{
	𐡄𐡍 (flags & SBI_PMU_CFG_FLAG_SET_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	𐡄𐡍 (flags & SBI_PMU_CFG_FLAG_SET_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	𐡄𐡍 (flags & SBI_PMU_CFG_FLAG_SET_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	𐡄𐡍 (flags & SBI_PMU_CFG_FLAG_SET_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

ساكن صحيح pmu_update_hw_mhpmevent(بنية sbi_pmu_hw_event *hw_evt, صحيح ctr_idx,
				   unsigned long flags, unsigned long eindex,
				   uint64_t data)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_to_mhpmevent(plat, eindex, data);

	𐡄𐡍 (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		أرجع SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	𐡄𐡍 (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_MASK) |
				 MHPMEVENT_MINH | MHPMEVENT_OF;

	𐡄𐡍 (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	𐡄𐡍 (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	𐡄𐡍 (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, ctr_idx);

#𐡄𐡍 __riscv_xlen == 32
	csr_write_num(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	𐡄𐡍 (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_num(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> BITS_PER_LONG);
#وإلا
	csr_write_num(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#endif

	أرجع 0;
}

صحيح sbi_pmu_ctr_start(unsigned long cbase, unsigned long cmask,
		      unsigned long flags, uint64_t ival)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	𐡄𐡍 (unlikely(!phs))
		أرجع SBI_EINVAL;

	صحيح event_idx_type;
	uint32_t event_code;
	صحيح ret = SBI_EINVAL;
	bool bUpdate = false;
	صحيح i, cidx;
	uint64_t edata;

	𐡄𐡍 (!pmu_ctr_idx_validate(cbase, cmask))
		أرجع ret;

	𐡄𐡍 (flags & ~SBI_PMU_START_FLAGS_MASK)
		أرجع SBI_ERR_INVALID_PARAM;

	𐡄𐡍 (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		أرجع SBI_ENO_SHMEM;

	𐡄𐡍 (flags & SBI_PMU_START_FLAG_SET_INIT_VALUE)
		bUpdate = true;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		𐡄𐡍 (event_idx_type < 0)
			/* Continue the start operation for other counters */
			استمر;
		وإلا 𐡄𐡍 (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_code == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_data[cidx - num_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_start_fw(phs, cidx, event_code, edata,
					       ival, bUpdate);
			𐡄𐡍 (ret)
				أرجع ret;
		} وإلا {
			𐡄𐡍 (cidx >= 3) {
				بنية sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->active_events[cidx],
							ev_cfg->event_data);
				𐡄𐡍 (ret)
					أرجع ret;
			}
			ret = pmu_ctr_start_hw(cidx, ival, bUpdate);
			𐡄𐡍 (ret)
				أرجع ret;
		}
	}

	أرجع ret;
}

ساكن صحيح pmu_ctr_stop_hw(uint32_t cidx)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	𐡄𐡍 (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11)
		أرجع 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	𐡄𐡍 (cidx >= num_hw_ctrs || cidx == 1)
		أرجع SBI_EINVAL;

	𐡄𐡍 (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		𐡄𐡍 (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		أرجع 0;
	} وإلا
		أرجع SBI_EALREADY_STOPPED;
}

ساكن صحيح pmu_ctr_stop_fw(بنية sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_code)
{
	صحيح ret;

	𐡄𐡍 ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		أرجع SBI_EINVAL;

	𐡄𐡍 (!(phs->fw_counters_started & BIT(cidx - num_hw_ctrs)))
		أرجع SBI_EALREADY_STOPPED;

	𐡄𐡍 (SBI_PMU_FW_PLATFORM == event_code &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - num_hw_ctrs);
		𐡄𐡍 (ret)
			أرجع ret;
	}

	phs->fw_counters_started &= ~BIT(cidx - num_hw_ctrs);

	أرجع 0;
}

ساكن صحيح pmu_reset_hw_mhpmevent(صحيح ctr_idx)
{
	𐡄𐡍 (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		أرجع SBI_EFAIL;
#𐡄𐡍 __riscv_xlen == 32
	csr_write_num(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	𐡄𐡍 (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_num(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#وإلا
	csr_write_num(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#endif

	أرجع 0;
}

صحيح sbi_pmu_ctr_stop(unsigned long cbase, unsigned long cmask,
		     unsigned long flag)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	𐡄𐡍 (unlikely(!phs))
		أرجع SBI_EINVAL;

	صحيح ret = SBI_EINVAL;
	صحيح event_idx_type;
	uint32_t event_code;
	صحيح i, cidx;

	𐡄𐡍 (!pmu_ctr_idx_validate(cbase, cmask))
		أرجع ret;

	𐡄𐡍 (flag & ~SBI_PMU_STOP_FLAGS_MASK)
		أرجع SBI_ERR_INVALID_PARAM;

	𐡄𐡍 (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		أرجع SBI_ENO_SHMEM;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		𐡄𐡍 (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			استمر;

		وإلا 𐡄𐡍 (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_code);
		وإلا
			ret = pmu_ctr_stop_hw(cidx);

		𐡄𐡍(ret)
			أرجع ret;

		𐡄𐡍 (cidx > (CSR_INSTRET - CSR_CYCLE) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->active_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	𐡄𐡍 (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_mask());

	أرجع ret;
}

ساكن صحيح pmu_fixed_ctr_update_inhibit_bits(صحيح fixed_ctr, unsigned long flags)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#𐡄𐡍 __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#endif
	𐡄𐡍 (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_mode))
		أرجع fixed_ctr;

	بدل (fixed_ctr) {
	حالة 0:
		cfg_csr_no = CSR_MCYCLECFG;
#𐡄𐡍 __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#endif
		اكسر;
	حالة 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#𐡄𐡍 __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#endif
		اكسر;
	افتراضي:
		أرجع SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	𐡄𐡍 (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#𐡄𐡍 __riscv_xlen == 32
		csr_write_num(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_num(cfgh_csr_no, cfg_val >> BITS_PER_LONG);
#وإلا
		csr_write_num(cfg_csr_no, cfg_val);
#endif
	}
	𐡄𐡍 (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, fixed_ctr);
	أرجع fixed_ctr;
}

ساكن صحيح pmu_ctr_find_fixed_hw(unsigned long evt_idx_code)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	𐡄𐡍 (evt_idx_code == SBI_PMU_HW_CPU_CYCLES)
		أرجع 0;
	وإلا 𐡄𐡍 (evt_idx_code == SBI_PMU_HW_INSTRUCTIONS)
		أرجع 2;
	وإلا
		أرجع SBI_EINVAL;
}

ساكن صحيح pmu_ctr_find_hw(بنية sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   unsigned long flags,
			   unsigned long event_idx, uint64_t data)
{
	unsigned long ctr_mask;
	صحيح i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	بنية sbi_pmu_hw_event *temp;
	unsigned long mctr_inhbt = 0;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	𐡄𐡍 (cbase >= num_hw_ctrs)
		أرجع SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_find_fixed_hw(event_idx);
	𐡄𐡍 (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		أرجع pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);

	𐡄𐡍 (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	لكل (i = 0; i < num_hw_events; i++) {
		temp = &hw_event_map[i];
		𐡄𐡍 ((temp->start_idx > event_idx && event_idx < temp->end_idx) ||
		    (temp->start_idx < event_idx && event_idx > temp->end_idx))
			استمر;

		/* For raw events, event data is used as the select value */
		𐡄𐡍 (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_mask = temp->select_mask;

			/* The non-event map bits of data should match the selector */
			𐡄𐡍 (temp->select != (data & select_mask))
				استمر;
		}
		/* Fixed counters should not be part of the search */
		ctr_mask = temp->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_MASK);
		for_each_set_bit_from(cbase, &ctr_mask, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			𐡄𐡍 (phs->active_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				استمر;
			/* If mcountinhibit is supported, the bit must be enabled */
			𐡄𐡍 ((sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				استمر;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			اكسر;
		}
	}

	𐡄𐡍 (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		𐡄𐡍 (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			أرجع SBI_EFAIL;

		أرجع pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(temp, ctr_idx, flags, event_idx, data);

	𐡄𐡍 (!ret)
		ret = ctr_idx;

	أرجع ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
ساكن صحيح pmu_ctr_find_fw(بنية sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   uint32_t event_code, uint64_t edata)
{
	صحيح i, cidx;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		𐡄𐡍 (cidx < num_hw_ctrs || total_ctrs <= cidx)
			استمر;
		𐡄𐡍 (phs->active_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			استمر;
		𐡄𐡍 (SBI_PMU_FW_PLATFORM == event_code &&
		    pmu_dev && pmu_dev->fw_counter_match_encoding) {
			𐡄𐡍 (!pmu_dev->fw_counter_match_encoding(phs->hartid,
							    cidx - num_hw_ctrs,
							    edata))
				استمر;
		}

		أرجع cidx;
	}

	أرجع SBI_ENOTSUPP;
}

صحيح sbi_pmu_ctr_cfg_match(unsigned long cidx_base, unsigned long cidx_mask,
			  unsigned long flags, unsigned long event_idx,
			  uint64_t event_data)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	𐡄𐡍 (unlikely(!phs))
		أرجع SBI_EINVAL;

	صحيح ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_code;

	𐡄𐡍 (!pmu_ctr_idx_validate(cidx_base, cidx_mask))
		أرجع SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_data);
	𐡄𐡍 (event_type < 0)
		أرجع SBI_EINVAL;
	event_code = get_cidx_code(event_idx);

	𐡄𐡍 (flags & SBI_PMU_CFG_FLAG_SKIP_MATCH) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		unsigned long cidx_first = cidx_base + sbi_ffs(cidx_mask);

		𐡄𐡍 (cidx_first >= total_ctrs)
			أرجع SBI_EINVAL;

		𐡄𐡍 (phs->active_events[cidx_first] == SBI_PMU_EVENT_IDX_INVALID)
			أرجع SBI_EINVAL;
		ctr_idx = cidx_first;
		اذهب skip_match;
	}

	𐡄𐡍 (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_find_fw(phs, cidx_base, cidx_mask,
					  event_code, event_data);
		𐡄𐡍 ((event_code == SBI_PMU_FW_PLATFORM) && (ctr_idx >= num_hw_ctrs))
			phs->fw_counters_data[ctr_idx - num_hw_ctrs] =
								event_data;
	} وإلا {
		ctr_idx = pmu_ctr_find_hw(phs, cidx_base, cidx_mask, flags,
					  event_idx, event_data);
		𐡄𐡍 (ctr_idx >= 0) {
			بنية sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_data = event_data;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_MASK;
		}
	}

	𐡄𐡍 (ctr_idx < 0)
		أرجع SBI_ENOTSUPP;

	phs->active_events[ctr_idx] = event_idx;
skip_match:
	𐡄𐡍 (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		𐡄𐡍 (flags & SBI_PMU_CFG_FLAG_CLEAR_VALUE)
			pmu_ctr_write_hw(ctr_idx, 0);
		𐡄𐡍 (flags & SBI_PMU_CFG_FLAG_AUTO_START)
			pmu_ctr_start_hw(ctr_idx, 0, false);
	} وإلا 𐡄𐡍 (event_type == SBI_PMU_EVENT_TYPE_FW) {
		𐡄𐡍 (flags & SBI_PMU_CFG_FLAG_CLEAR_VALUE)
			phs->fw_counters_data[ctr_idx - num_hw_ctrs] = 0;
		𐡄𐡍 (flags & SBI_PMU_CFG_FLAG_AUTO_START) {
			𐡄𐡍 (SBI_PMU_FW_PLATFORM == event_code &&
			    pmu_dev && pmu_dev->fw_counter_start) {
				ret = pmu_dev->fw_counter_start(
					phs->hartid,
					ctr_idx - num_hw_ctrs, event_data);
				𐡄𐡍 (ret)
					أرجع ret;
			}
			phs->fw_counters_started |= BIT(ctr_idx - num_hw_ctrs);
		}
	}

	أرجع ctr_idx;
}

صحيح sbi_pmu_ctr_incr_fw(تعداد sbi_pmu_fw_event_code_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = عدم;
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	𐡄𐡍 (unlikely(!phs))
		أرجع 0;

	𐡄𐡍 (likely(!phs->fw_counters_started))
		أرجع 0;

	𐡄𐡍 (unlikely(fw_id >= SBI_PMU_FW_MAX))
		أرجع SBI_EINVAL;

	لكل (cidx = num_hw_ctrs; cidx < total_ctrs; cidx++) {
		𐡄𐡍 (get_cidx_code(phs->active_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & BIT(cidx - num_hw_ctrs))) {
			fcounter = &phs->fw_counters_data[cidx - num_hw_ctrs];
			اكسر;
		}
	}

	𐡄𐡍 (fcounter)
		(*fcounter)++;

	أرجع 0;
}

unsigned long sbi_pmu_num_ctr(فراغ)
{
	أرجع (num_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

صحيح sbi_pmu_ctr_get_info(uint32_t cidx, unsigned long *ctr_info)
{
	صحيح width;
	اتحاد sbi_pmu_ctr_info cinfo = {0};
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long counter_mask = (unsigned long)sbi_hart_mhpm_mask(scratch) |
				     SBI_PMU_CY_IR_MASK;

	/* Sanity check */
	𐡄𐡍 (cidx >= total_ctrs)
		أرجع SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	𐡄𐡍 (cidx < num_hw_ctrs) {
		𐡄𐡍 (!(__test_bit(cidx, &counter_mask)))
			أرجع SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_CYCLE + cidx;
		/* mcycle & minstret are always 64 bit */
		𐡄𐡍 (cidx == 0 || cidx == 2)
			cinfo.width = 63;
		وإلا
			cinfo.width = sbi_hart_mhpm_bits(scratch) - 1;
	} وإلا {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.width = 63;
		𐡄𐡍 (pmu_dev && pmu_dev->fw_counter_width) {
			width = pmu_dev->fw_counter_width();
			𐡄𐡍 (width)
				cinfo.width = width - 1;
		}
	}

	*ctr_info = cinfo.value;

	أرجع 0;
}

صحيح sbi_pmu_event_get_info(unsigned long shmem_phys_lo, unsigned long shmem_phys_hi,
			   unsigned long num_events, unsigned long flags)
{
	unsigned long shmem_size = num_events * حجم(بنية sbi_pmu_event_info);
	صحيح i, j, event_type;
	بنية sbi_pmu_event_info *einfo;
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	بنية sbi_pmu_hw_event *temp;
	bool found = false;

	𐡄𐡍 (flags != 0)
		أرجع SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	𐡄𐡍 (!num_events || (shmem_phys_lo & 0xF))
		أرجع SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	𐡄𐡍 (shmem_phys_hi)
		أرجع SBI_EINVALID_ADDR;

	𐡄𐡍 (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_size, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		أرجع SBI_ERR_INVALID_ADDRESS;

	sbi_hart_protection_temp_map_range(shmem_phys_lo, shmem_size);

	einfo = (بنية sbi_pmu_event_info *)(shmem_phys_lo);
	لكل (i = 0; i < num_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		𐡄𐡍 (event_idx & SBI_PMU_EVENT_IDX_MBZ_MASK)
			أرجع SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_data);
		𐡄𐡍 (event_type < 0) {
			einfo[i].output = 0;
		} وإلا 𐡄𐡍 (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} وإلا {
			لكل (j = 0; j < num_hw_events; j++) {
				temp = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				𐡄𐡍 (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					𐡄𐡍 (temp->start_idx > event_idx ||
					    event_idx > temp->end_idx)
						استمر;
					/* just match the selector */
					𐡄𐡍 (temp->select == (einfo[i].event_data &
									temp->select_mask)) {
						found = true;
						اكسر;
					}
				} وإلا 𐡄𐡍 (temp->start_idx <= event_idx &&
					   event_idx <= temp->end_idx) {
					found = true;
					اكسر;
				}
			}
			𐡄𐡍 (found)
				einfo[i].output = 1;
			وإلا
				einfo[i].output = 0;
			found = false;
		}
	}

	sbi_hart_protection_temp_unmap_range(shmem_phys_lo, shmem_size);

	أرجع 0;
}

ساكن فراغ pmu_reset_event_map(بنية sbi_pmu_hart_state *phs)
{
	صحيح j;

	/* Initialize the counter to event mapping table */
	لكل (j = 3; j < total_ctrs; j++)
		phs->active_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	لكل (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_data[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

ثابت بنية sbi_pmu_device *sbi_pmu_get_device(فراغ)
{
	أرجع pmu_dev;
}

فراغ sbi_pmu_set_device(ثابت بنية sbi_pmu_device *dev)
{
	𐡄𐡍 (!dev || pmu_dev)
		أرجع;

	pmu_dev = dev;
}

فراغ sbi_pmu_exit(بنية sbi_scratch *scratch)
{
	بنية sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	𐡄𐡍 (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	𐡄𐡍 (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	𐡄𐡍 (unlikely(!phs))
		أرجع;

	pmu_reset_event_map(phs);
}

ساكن فراغ pmu_sse_enable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_set(CSR_MIE, irq_mask);
}

ساكن فراغ pmu_sse_disable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_clear(CSR_MIE, irq_mask);
	csr_clear(CSR_MIP, irq_mask);
}

ساكن فراغ pmu_sse_complete(uint32_t event_id)
{
	csr_set(CSR_MIE, sbi_pmu_irq_mask());
}

ساكن فراغ pmu_sse_register(uint32_t event_id)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_mask());
}

ساكن فراغ pmu_sse_unregister(uint32_t event_id)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_set(CSR_MIDELEG, sbi_pmu_irq_mask());
}

ساكن ثابت بنية sbi_sse_cb_ops pmu_sse_cb_ops = {
	.register_cb = pmu_sse_register,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

صحيح sbi_pmu_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	صحيح hpm_count = sbi_fls(sbi_hart_mhpm_mask(scratch));
	بنية sbi_pmu_hart_state *phs;
	ثابت بنية sbi_platform *plat;
	صحيح rc;

	𐡄𐡍 (cold_boot) {
		hw_event_map = sbi_calloc(حجم(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		𐡄𐡍 (!hw_event_map)
			أرجع SBI_ENOMEM;

		phs_ptr_offset = sbi_scratch_alloc_type_offset(فراغ *);
		𐡄𐡍 (!phs_ptr_offset) {
			sbi_free(hw_event_map);
			أرجع SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		𐡄𐡍 (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		𐡄𐡍 (!hpm_count)
			/* Only CY, TM & IR are implemented in the hw */
			num_hw_ctrs = 3;
		وإلا
			num_hw_ctrs = hpm_count + 1;

		𐡄𐡍 (num_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			أرجع SBI_EINVAL;

		total_ctrs = num_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		𐡄𐡍 (sbi_pmu_irq_bit() >= 0)
			sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	𐡄𐡍 (!phs) {
		phs = sbi_zalloc(حجم(*phs));
		𐡄𐡍 (!phs)
			أرجع SBI_ENOMEM;
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

	أرجع 0;
}
