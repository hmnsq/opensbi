/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hart_protection.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_pmu.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_sse.h>

/** Information about hardware counters */
ساختار sbi_pmu_hw_event {
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
اجتماع sbi_pmu_ctr_info {
	unsigned long مقدار;
	ساختار {
		unsigned long csr:12;
		unsigned long width:6;
#اگر __riscv_xlen == 32
		unsigned long reserved:13;
#وگرنه
		unsigned long reserved:45;
#endif
		unsigned long type:1;
	};
};

#اگر SBI_PMU_FW_CTR_MAX >= BITS_PER_LONG
#error "Can't handle firmware counters beyond BITS_PER_LONG"
#endif

/** HW event configuration parameters */
ساختار sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_data;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
ساختار sbi_pmu_hart_state {
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
	ساختار sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
ایستا unsigned long phs_ptr_offset;

#define pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_offset ? sbi_scratch_read_type((__scratch), تهی *, phs_ptr_offset) : پوچ

#define pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define pmu_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), تهی *, phs_ptr_offset, (__phs))

/* Platform specific PMU device */
ایستا ثابت ساختار sbi_pmu_device *pmu_dev = پوچ;

/* Mapping between event range and possible counters  */
ایستا ساختار sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
ایستا uint32_t رقم_hw_events;
/* Maximum number of hardware counters available */
ایستا uint32_t رقم_hw_ctrs;

/* Maximum number of counters available */
ایستا uint32_t total_ctrs;

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
ایستا bool pmu_event_range_overlap(ساختار sbi_pmu_hw_event *evtA,
				    ساختار sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	اگر (((evtA->end_idx < evtB->start_idx) && (evtA->end_idx < evtB->end_idx)) ||
	   ((evtA->start_idx > evtB->start_idx) && (evtA->start_idx > evtB->end_idx)))
		بازگشت false;
	بازگشت true;
}

ایستا bool pmu_event_select_overlap(ساختار sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_mask)
{
	اگر ((evt->select == select_val) && (evt->select_mask == select_mask))
		بازگشت true;

	بازگشت false;
}

ایستا صحیح pmu_event_validate(ساختار sbi_pmu_hart_state *phs,
			      unsigned long event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_code = get_cidx_code(event_idx);
	uint32_t event_idx_code_max = -1;
	uint32_t cache_ops_result, cache_ops_id, cache_id;

	انتخاب(event_idx_type) {
	حالت SBI_PMU_EVENT_TYPE_HW:
		event_idx_code_max = SBI_PMU_HW_GENERAL_MAX;
		بشکن;
	حالت SBI_PMU_EVENT_TYPE_FW:
		اگر ((event_idx_code >= SBI_PMU_FW_MAX &&
		    event_idx_code <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_code > SBI_PMU_FW_PLATFORM)
			بازگشت SBI_EINVAL;

		اگر (SBI_PMU_FW_PLATFORM == event_idx_code &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			بازگشت pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		وگرنه
			event_idx_code_max = SBI_PMU_FW_MAX;
		بشکن;
	حالت SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_result = event_idx_code &
					SBI_PMU_EVENT_HW_CACHE_OPS_RESULT;
		cache_ops_id = (event_idx_code &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_MASK) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_OFFSET;
		cache_id = (event_idx_code &
			    SBI_PMU_EVENT_HW_CACHE_ID_MASK) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_OFFSET;
		اگر ((cache_ops_result < SBI_PMU_HW_CACHE_RESULT_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			بازگشت event_idx_type;
		وگرنه
			بازگشت SBI_EINVAL;
		بشکن;
	حالت SBI_PMU_EVENT_TYPE_HW_RAW:
	حالت SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_code_max = 1; // event_idx.code should be zero
		بشکن;
	پیشفرض:
		بازگشت SBI_EINVAL;
	}

	اگر (event_idx_code < event_idx_code_max)
		بازگشت event_idx_type;

	بازگشت SBI_EINVAL;
}

ایستا صحیح pmu_ctr_validate(ساختار sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_code)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	اگر (cidx >= total_ctrs)
		بازگشت SBI_EINVAL;

	event_idx_val = phs->active_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	اگر (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		بازگشت SBI_EINVAL;

	*event_idx_code = get_cidx_code(event_idx_val);

	بازگشت event_idx_type;
}

ایستا bool pmu_ctr_idx_validate(unsigned long cbase, unsigned long cmask)
{
	unsigned long last;

	اگر (!cmask)
		بازگشت false;

	last = sbi_fls(cmask);
	اگر (cbase > -1UL - last)
		بازگشت false;

	بازگشت (cbase + last) < total_ctrs;
}

صحیح sbi_pmu_ctr_fw_read(unsigned long cidx, uint64_t *cval, bool بالا_bits)
{
	صحیح event_idx_type;
	uint32_t event_code;
	ساختار sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	اگر (unlikely(!phs))
		بازگشت SBI_EINVAL;

	اگر (cidx < رقم_hw_ctrs || cidx >= total_ctrs)
		بازگشت SBI_EINVAL;

#اگر __riscv_xlen > 32
	اگر (بالا_bits)
		بازگشت 0;
#endif

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
	اگر (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		بازگشت SBI_EINVAL;

	اگر ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		بازگشت SBI_EINVAL;

	اگر (SBI_PMU_FW_PLATFORM == event_code) {
		اگر (pmu_dev && pmu_dev->fw_counter_read_مقدار)
			*cval = pmu_dev->fw_counter_read_مقدار(phs->hartid,
							       cidx -
							       رقم_hw_ctrs);
		وگرنه
			*cval = 0;
	} وگرنه
		*cval = phs->fw_counters_data[cidx - رقم_hw_ctrs];

	بازگشت 0;
}

ایستا صحیح pmu_add_hw_event_map(u32 eidx_start, u32 eidx_end, u32 cmap,
				uint64_t select, uint64_t select_mask)
{
	صحیح i = 0;
	bool is_overlap;
	ساختار sbi_pmu_hw_event *event = &hw_event_map[رقم_hw_events];
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_mask = sbi_hart_mhpm_mask(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	اگر (eidx_start > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_MASK))
		بازگشت SBI_EDENIED;

	اگر (رقم_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		بازگشت SBI_EFAIL;
	}

	event->start_idx = eidx_start;
	event->end_idx = eidx_end;

	/* Sanity check */
	برای (i = 0; i < رقم_hw_events; i++) {
		اگر (eidx_start == SBI_PMU_EVENT_RAW_IDX ||
			eidx_start == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_mask);
		وگرنه
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		اگر (is_overlap)
			برو reset_event;
	}

	event->select_mask = select_mask;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_mask;
	event->select = select;
	رقم_hw_events++;

	بازگشت 0;

reset_event:
	event->start_idx = 0;
	event->end_idx = 0;
	بازگشت SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
صحیح sbi_pmu_add_hw_event_counter_map(u32 eidx_start, u32 eidx_end, u32 cmap)
{
	اگر ((eidx_start > eidx_end) || eidx_start >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_end >= SBI_PMU_EVENT_RAW_IDX)
		بازگشت SBI_EINVAL;

	بازگشت pmu_add_hw_event_map(eidx_start, eidx_end, cmap, 0, 0);
}

صحیح sbi_pmu_add_raw_event_counter_map(uint64_t select, uint64_t select_mask, u32 cmap)
{
	بازگشت pmu_add_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_mask);
}

تهی sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_mask());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW);
}

ایستا صحیح pmu_ctr_enable_irq_hw(صحیح ctr_idx)
{
	unsigned long mhpmevent_csr;
	unsigned long mhpmevent_curr;
	unsigned long mip_val;
	unsigned long of_mask;

	اگر (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		بازگشت SBI_EFAIL;

#اگر __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	of_mask = (uint32_t)~MHPMEVENTH_OF;
#وگرنه
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	of_mask = ~MHPMEVENT_OF;
#endif

	mhpmevent_curr = csr_read_رقم(mhpmevent_csr);
	mip_val = csr_read(CSR_MIP);
	/**
	 * Clear out the OF bit so that next interrupt can be enabled.
	 * This should be done only when the corresponding overflow interrupt
	 * bit is cleared. That indicates that software has already handled the
	 * previous interrupts or the hardware yet to set an overflow interrupt.
	 * Otherwise, there will be race conditions where we may clear the bit
	 * the software is yet to handle the interrupt.
	 */
	اگر (!(mip_val & sbi_pmu_irq_mask())) {
		mhpmevent_curr &= of_mask;
		csr_write_رقم(mhpmevent_csr, mhpmevent_curr);
	}

	بازگشت 0;
}

ایستا تهی pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#اگر __riscv_xlen == 32
	csr_write_رقم(CSR_MCYCLE + cidx, 0);
	csr_write_رقم(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_رقم(CSR_MCYCLEH + cidx, ival >> BITS_PER_LONG);
#وگرنه
	csr_write_رقم(CSR_MCYCLE + cidx, ival);
#endif
}

ایستا صحیح pmu_ctr_start_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	اگر (cidx >= رقم_hw_ctrs || cidx == 1)
		بازگشت SBI_EINVAL;

	اگر (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11) {
		اگر (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		بازگشت 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	اگر (!__test_bit(cidx, &mctr_inhbt))
		بازگشت SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	اگر (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	اگر (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	بازگشت 0;
}

صحیح sbi_pmu_irq_bit(تهی)
{
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		بازگشت IRQ_PMU_OVF;
	اگر (pmu_dev && pmu_dev->hw_counter_irq_bit)
		بازگشت pmu_dev->hw_counter_irq_bit();

	بازگشت -1;
}

unsigned long sbi_pmu_irq_mask(تهی)
{
	صحیح irq_bit = sbi_pmu_irq_bit();

	اگر (irq_bit < 0)
		بازگشت 0;

	بازگشت BIT(irq_bit);
}

ایستا صحیح pmu_ctr_start_fw(ساختار sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_code,
			    uint64_t event_data, uint64_t ival,
			    bool ival_update)
{
	صحیح ret;

	اگر ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		بازگشت SBI_EINVAL;

	اگر (phs->fw_counters_started & BIT(cidx - رقم_hw_ctrs))
		بازگشت SBI_EALREADY_STARTED;

	اگر (SBI_PMU_FW_PLATFORM == event_code) {
		اگر (!pmu_dev ||
		    !pmu_dev->fw_counter_write_مقدار ||
		    !pmu_dev->fw_counter_start) {
			بازگشت SBI_EINVAL;
		}

		اگر (ival_update)
			pmu_dev->fw_counter_write_مقدار(phs->hartid,
							cidx - رقم_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_start(phs->hartid,
						 cidx - رقم_hw_ctrs,
						 event_data);
		اگر (ret)
			بازگشت ret;
	} وگرنه {
		اگر (ival_update)
			phs->fw_counters_data[cidx - رقم_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= BIT(cidx - رقم_hw_ctrs);

	بازگشت 0;
}

ایستا تهی pmu_update_inhibit_flags(unsigned long flags, uint64_t *mhpmevent_val)
{
	اگر (flags & SBI_PMU_CFG_FLAG_SET_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	اگر (flags & SBI_PMU_CFG_FLAG_SET_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	اگر (flags & SBI_PMU_CFG_FLAG_SET_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	اگر (flags & SBI_PMU_CFG_FLAG_SET_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

ایستا صحیح pmu_update_hw_mhpmevent(ساختار sbi_pmu_hw_event *hw_evt, صحیح ctr_idx,
				   unsigned long flags, unsigned long eindex,
				   uint64_t data)
{
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت ساختار sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_to_mhpmevent(plat, eindex, data);

	اگر (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		بازگشت SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_MASK) |
				 MHPMEVENT_MINH | MHPMEVENT_OF;

	اگر (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	اگر (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, ctr_idx);

#اگر __riscv_xlen == 32
	csr_write_رقم(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_رقم(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> BITS_PER_LONG);
#وگرنه
	csr_write_رقم(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#endif

	بازگشت 0;
}

صحیح sbi_pmu_ctr_start(unsigned long cbase, unsigned long cmask,
		      unsigned long flags, uint64_t ival)
{
	ساختار sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	اگر (unlikely(!phs))
		بازگشت SBI_EINVAL;

	صحیح event_idx_type;
	uint32_t event_code;
	صحیح ret = SBI_EINVAL;
	bool bUpdate = false;
	صحیح i, cidx;
	uint64_t edata;

	اگر (!pmu_ctr_idx_validate(cbase, cmask))
		بازگشت ret;

	اگر (flags & ~SBI_PMU_START_FLAGS_MASK)
		بازگشت SBI_ERR_INVALID_PARAM;

	اگر (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		بازگشت SBI_ENO_SHMEM;

	اگر (flags & SBI_PMU_START_FLAG_SET_INIT_مقدار)
		bUpdate = true;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		اگر (event_idx_type < 0)
			/* Continue the start operation for other counters */
			ادامه;
		وگرنه اگر (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_code == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_data[cidx - رقم_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_start_fw(phs, cidx, event_code, edata,
					       ival, bUpdate);
			اگر (ret)
				بازگشت ret;
		} وگرنه {
			اگر (cidx >= 3) {
				ساختار sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->active_events[cidx],
							ev_cfg->event_data);
				اگر (ret)
					بازگشت ret;
			}
			ret = pmu_ctr_start_hw(cidx, ival, bUpdate);
			اگر (ret)
				بازگشت ret;
		}
	}

	بازگشت ret;
}

ایستا صحیح pmu_ctr_stop_hw(uint32_t cidx)
{
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	اگر (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11)
		بازگشت 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	اگر (cidx >= رقم_hw_ctrs || cidx == 1)
		بازگشت SBI_EINVAL;

	اگر (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		اگر (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		بازگشت 0;
	} وگرنه
		بازگشت SBI_EALREADY_STOPPED;
}

ایستا صحیح pmu_ctr_stop_fw(ساختار sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_code)
{
	صحیح ret;

	اگر ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		بازگشت SBI_EINVAL;

	اگر (!(phs->fw_counters_started & BIT(cidx - رقم_hw_ctrs)))
		بازگشت SBI_EALREADY_STOPPED;

	اگر (SBI_PMU_FW_PLATFORM == event_code &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - رقم_hw_ctrs);
		اگر (ret)
			بازگشت ret;
	}

	phs->fw_counters_started &= ~BIT(cidx - رقم_hw_ctrs);

	بازگشت 0;
}

ایستا صحیح pmu_reset_hw_mhpmevent(صحیح ctr_idx)
{
	اگر (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		بازگشت SBI_EFAIL;
#اگر __riscv_xlen == 32
	csr_write_رقم(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	اگر (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_رقم(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#وگرنه
	csr_write_رقم(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#endif

	بازگشت 0;
}

صحیح sbi_pmu_ctr_stop(unsigned long cbase, unsigned long cmask,
		     unsigned long flag)
{
	ساختار sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	اگر (unlikely(!phs))
		بازگشت SBI_EINVAL;

	صحیح ret = SBI_EINVAL;
	صحیح event_idx_type;
	uint32_t event_code;
	صحیح i, cidx;

	اگر (!pmu_ctr_idx_validate(cbase, cmask))
		بازگشت ret;

	اگر (flag & ~SBI_PMU_STOP_FLAGS_MASK)
		بازگشت SBI_ERR_INVALID_PARAM;

	اگر (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		بازگشت SBI_ENO_SHMEM;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		اگر (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			ادامه;

		وگرنه اگر (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_code);
		وگرنه
			ret = pmu_ctr_stop_hw(cidx);

		اگر(ret)
			بازگشت ret;

		اگر (cidx > (CSR_INSTRET - CSR_CYCLE) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->active_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	اگر (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_mask());

	بازگشت ret;
}

ایستا صحیح pmu_fixed_ctr_update_inhibit_bits(صحیح fixed_ctr, unsigned long flags)
{
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#اگر __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#endif
	اگر (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_mode))
		بازگشت fixed_ctr;

	انتخاب (fixed_ctr) {
	حالت 0:
		cfg_csr_no = CSR_MCYCLECFG;
#اگر __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#endif
		بشکن;
	حالت 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#اگر __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#endif
		بشکن;
	پیشفرض:
		بازگشت SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#اگر __riscv_xlen == 32
		csr_write_رقم(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_رقم(cfgh_csr_no, cfg_val >> BITS_PER_LONG);
#وگرنه
		csr_write_رقم(cfg_csr_no, cfg_val);
#endif
	}
	اگر (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, fixed_ctr);
	بازگشت fixed_ctr;
}

ایستا صحیح pmu_ctr_find_fixed_hw(unsigned long evt_idx_code)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	اگر (evt_idx_code == SBI_PMU_HW_CPU_CYCLES)
		بازگشت 0;
	وگرنه اگر (evt_idx_code == SBI_PMU_HW_INSTRUCTIONS)
		بازگشت 2;
	وگرنه
		بازگشت SBI_EINVAL;
}

ایستا صحیح pmu_ctr_find_hw(ساختار sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   unsigned long flags,
			   unsigned long event_idx, uint64_t data)
{
	unsigned long ctr_mask;
	صحیح i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	ساختار sbi_pmu_hw_event *temp;
	unsigned long mctr_inhbt = 0;
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	اگر (cbase >= رقم_hw_ctrs)
		بازگشت SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_find_fixed_hw(event_idx);
	اگر (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		بازگشت pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);

	اگر (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	برای (i = 0; i < رقم_hw_events; i++) {
		temp = &hw_event_map[i];
		اگر ((temp->start_idx > event_idx && event_idx < temp->end_idx) ||
		    (temp->start_idx < event_idx && event_idx > temp->end_idx))
			ادامه;

		/* For raw events, event data is used as the select value */
		اگر (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_mask = temp->select_mask;

			/* The non-event map bits of data should match the selector */
			اگر (temp->select != (data & select_mask))
				ادامه;
		}
		/* Fixed counters should not be part of the search */
		ctr_mask = temp->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_MASK);
		for_each_set_bit_from(cbase, &ctr_mask, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			اگر (phs->active_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				ادامه;
			/* If mcountinhibit is supported, the bit must be enabled */
			اگر ((sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				ادامه;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			بشکن;
		}
	}

	اگر (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		اگر (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			بازگشت SBI_EFAIL;

		بازگشت pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(temp, ctr_idx, flags, event_idx, data);

	اگر (!ret)
		ret = ctr_idx;

	بازگشت ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
ایستا صحیح pmu_ctr_find_fw(ساختار sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   uint32_t event_code, uint64_t edata)
{
	صحیح i, cidx;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		اگر (cidx < رقم_hw_ctrs || total_ctrs <= cidx)
			ادامه;
		اگر (phs->active_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			ادامه;
		اگر (SBI_PMU_FW_PLATFORM == event_code &&
		    pmu_dev && pmu_dev->fw_counter_match_encoding) {
			اگر (!pmu_dev->fw_counter_match_encoding(phs->hartid,
							    cidx - رقم_hw_ctrs,
							    edata))
				ادامه;
		}

		بازگشت cidx;
	}

	بازگشت SBI_ENOTSUPP;
}

صحیح sbi_pmu_ctr_cfg_match(unsigned long cidx_base, unsigned long cidx_mask,
			  unsigned long flags, unsigned long event_idx,
			  uint64_t event_data)
{
	ساختار sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	اگر (unlikely(!phs))
		بازگشت SBI_EINVAL;

	صحیح ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_code;

	اگر (!pmu_ctr_idx_validate(cidx_base, cidx_mask))
		بازگشت SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_data);
	اگر (event_type < 0)
		بازگشت SBI_EINVAL;
	event_code = get_cidx_code(event_idx);

	اگر (flags & SBI_PMU_CFG_FLAG_SKIP_MATCH) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		unsigned long cidx_first = cidx_base + sbi_ffs(cidx_mask);

		اگر (cidx_first >= total_ctrs)
			بازگشت SBI_EINVAL;

		اگر (phs->active_events[cidx_first] == SBI_PMU_EVENT_IDX_INVALID)
			بازگشت SBI_EINVAL;
		ctr_idx = cidx_first;
		برو skip_match;
	}

	اگر (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_find_fw(phs, cidx_base, cidx_mask,
					  event_code, event_data);
		اگر ((event_code == SBI_PMU_FW_PLATFORM) && (ctr_idx >= رقم_hw_ctrs))
			phs->fw_counters_data[ctr_idx - رقم_hw_ctrs] =
								event_data;
	} وگرنه {
		ctr_idx = pmu_ctr_find_hw(phs, cidx_base, cidx_mask, flags,
					  event_idx, event_data);
		اگر (ctr_idx >= 0) {
			ساختار sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_data = event_data;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_MASK;
		}
	}

	اگر (ctr_idx < 0)
		بازگشت SBI_ENOTSUPP;

	phs->active_events[ctr_idx] = event_idx;
skip_match:
	اگر (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		اگر (flags & SBI_PMU_CFG_FLAG_CLEAR_مقدار)
			pmu_ctr_write_hw(ctr_idx, 0);
		اگر (flags & SBI_PMU_CFG_FLAG_AUTO_START)
			pmu_ctr_start_hw(ctr_idx, 0, false);
	} وگرنه اگر (event_type == SBI_PMU_EVENT_TYPE_FW) {
		اگر (flags & SBI_PMU_CFG_FLAG_CLEAR_مقدار)
			phs->fw_counters_data[ctr_idx - رقم_hw_ctrs] = 0;
		اگر (flags & SBI_PMU_CFG_FLAG_AUTO_START) {
			اگر (SBI_PMU_FW_PLATFORM == event_code &&
			    pmu_dev && pmu_dev->fw_counter_start) {
				ret = pmu_dev->fw_counter_start(
					phs->hartid,
					ctr_idx - رقم_hw_ctrs, event_data);
				اگر (ret)
					بازگشت ret;
			}
			phs->fw_counters_started |= BIT(ctr_idx - رقم_hw_ctrs);
		}
	}

	بازگشت ctr_idx;
}

صحیح sbi_pmu_ctr_incr_fw(شمارش sbi_pmu_fw_event_code_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = پوچ;
	ساختار sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	اگر (unlikely(!phs))
		بازگشت 0;

	اگر (likely(!phs->fw_counters_started))
		بازگشت 0;

	اگر (unlikely(fw_id >= SBI_PMU_FW_MAX))
		بازگشت SBI_EINVAL;

	برای (cidx = رقم_hw_ctrs; cidx < total_ctrs; cidx++) {
		اگر (get_cidx_code(phs->active_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & BIT(cidx - رقم_hw_ctrs))) {
			fcounter = &phs->fw_counters_data[cidx - رقم_hw_ctrs];
			بشکن;
		}
	}

	اگر (fcounter)
		(*fcounter)++;

	بازگشت 0;
}

unsigned long sbi_pmu_رقم_ctr(تهی)
{
	بازگشت (رقم_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

صحیح sbi_pmu_ctr_get_info(uint32_t cidx, unsigned long *ctr_info)
{
	صحیح width;
	اجتماع sbi_pmu_ctr_info cinfo = {0};
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long counter_mask = (unsigned long)sbi_hart_mhpm_mask(scratch) |
				     SBI_PMU_CY_IR_MASK;

	/* Sanity check */
	اگر (cidx >= total_ctrs)
		بازگشت SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	اگر (cidx < رقم_hw_ctrs) {
		اگر (!(__test_bit(cidx, &counter_mask)))
			بازگشت SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_CYCLE + cidx;
		/* mcycle & minstret are always 64 bit */
		اگر (cidx == 0 || cidx == 2)
			cinfo.width = 63;
		وگرنه
			cinfo.width = sbi_hart_mhpm_bits(scratch) - 1;
	} وگرنه {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.width = 63;
		اگر (pmu_dev && pmu_dev->fw_counter_width) {
			width = pmu_dev->fw_counter_width();
			اگر (width)
				cinfo.width = width - 1;
		}
	}

	*ctr_info = cinfo.مقدار;

	بازگشت 0;
}

صحیح sbi_pmu_event_get_info(unsigned long shmem_phys_lo, unsigned long shmem_phys_hi,
			   unsigned long رقم_events, unsigned long flags)
{
	unsigned long shmem_size = رقم_events * اندازه(ساختار sbi_pmu_event_info);
	صحیح i, j, event_type;
	ساختار sbi_pmu_event_info *einfo;
	ساختار sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	ساختار sbi_pmu_hw_event *temp;
	bool found = false;

	اگر (flags != 0)
		بازگشت SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	اگر (!رقم_events || (shmem_phys_lo & 0xF))
		بازگشت SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	اگر (shmem_phys_hi)
		بازگشت SBI_EINVALID_ADDR;

	اگر (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_size, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		بازگشت SBI_ERR_INVALID_ADDRESS;

	sbi_hart_protection_temp_map_range(shmem_phys_lo, shmem_size);

	einfo = (ساختار sbi_pmu_event_info *)(shmem_phys_lo);
	برای (i = 0; i < رقم_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		اگر (event_idx & SBI_PMU_EVENT_IDX_MBZ_MASK)
			بازگشت SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_data);
		اگر (event_type < 0) {
			einfo[i].output = 0;
		} وگرنه اگر (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} وگرنه {
			برای (j = 0; j < رقم_hw_events; j++) {
				temp = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				اگر (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					اگر (temp->start_idx > event_idx ||
					    event_idx > temp->end_idx)
						ادامه;
					/* just match the selector */
					اگر (temp->select == (einfo[i].event_data &
									temp->select_mask)) {
						found = true;
						بشکن;
					}
				} وگرنه اگر (temp->start_idx <= event_idx &&
					   event_idx <= temp->end_idx) {
					found = true;
					بشکن;
				}
			}
			اگر (found)
				einfo[i].output = 1;
			وگرنه
				einfo[i].output = 0;
			found = false;
		}
	}

	sbi_hart_protection_temp_unmap_range(shmem_phys_lo, shmem_size);

	بازگشت 0;
}

ایستا تهی pmu_reset_event_map(ساختار sbi_pmu_hart_state *phs)
{
	صحیح j;

	/* Initialize the counter to event mapping table */
	برای (j = 3; j < total_ctrs; j++)
		phs->active_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	برای (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_data[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

ثابت ساختار sbi_pmu_device *sbi_pmu_get_device(تهی)
{
	بازگشت pmu_dev;
}

تهی sbi_pmu_set_device(ثابت ساختار sbi_pmu_device *dev)
{
	اگر (!dev || pmu_dev)
		بازگشت;

	pmu_dev = dev;
}

تهی sbi_pmu_exit(ساختار sbi_scratch *scratch)
{
	ساختار sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	اگر (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	اگر (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	اگر (unlikely(!phs))
		بازگشت;

	pmu_reset_event_map(phs);
}

ایستا تهی pmu_sse_enable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_set(CSR_MIE, irq_mask);
}

ایستا تهی pmu_sse_disable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_clear(CSR_MIE, irq_mask);
	csr_clear(CSR_MIP, irq_mask);
}

ایستا تهی pmu_sse_complete(uint32_t event_id)
{
	csr_set(CSR_MIE, sbi_pmu_irq_mask());
}

ایستا تهی pmu_sse_register(uint32_t event_id)
{
	ساختار sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_mask());
}

ایستا تهی pmu_sse_unregister(uint32_t event_id)
{
	ساختار sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_set(CSR_MIDELEG, sbi_pmu_irq_mask());
}

ایستا ثابت ساختار sbi_sse_cb_ops pmu_sse_cb_ops = {
	.register_cb = pmu_sse_register,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

صحیح sbi_pmu_init(ساختار sbi_scratch *scratch, bool cold_boot)
{
	صحیح hpm_شمار = sbi_fls(sbi_hart_mhpm_mask(scratch));
	ساختار sbi_pmu_hart_state *phs;
	ثابت ساختار sbi_platform *plat;
	صحیح rc;

	اگر (cold_boot) {
		hw_event_map = sbi_calloc(اندازه(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		اگر (!hw_event_map)
			بازگشت SBI_ENOMEM;

		phs_ptr_offset = sbi_scratch_alloc_type_offset(تهی *);
		اگر (!phs_ptr_offset) {
			sbi_free(hw_event_map);
			بازگشت SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		اگر (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		اگر (!hpm_شمار)
			/* Only CY, TM & IR are implemented in the hw */
			رقم_hw_ctrs = 3;
		وگرنه
			رقم_hw_ctrs = hpm_شمار + 1;

		اگر (رقم_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			بازگشت SBI_EINVAL;

		total_ctrs = رقم_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		اگر (sbi_pmu_irq_bit() >= 0)
			sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	اگر (!phs) {
		phs = sbi_zalloc(اندازه(*phs));
		اگر (!phs)
			بازگشت SBI_ENOMEM;
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

	بازگشت 0;
}
