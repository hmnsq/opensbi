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
ساخت sbi_pmu_hw_event {
	uint32_t counters;
	uint32_t آغاز_idx;
	uint32_t انتہا_idx;
	/* Event selector value used only for raw events. The event select value
	 * can be a even id or a selector value for set of events encoded in few
	 * bits. In case latter, the bits used for encoding of the events should
	 * be zeroed out in the select value.
	 */
	uint64_t select;
	 /**
	  * The select_mask indicates which bits are encoded for the event(s).
	  */
	uint64_t select_نقاب;
};

/* Information about PMU counters as per SBI specification */
اتحاد sbi_pmu_ctr_info {
	غیرعلامتی طویل قدر;
	ساخت {
		غیرعلامتی طویل csr:12;
		غیرعلامتی طویل چوڑائی:6;
#اگر __riscv_xlen == 32
		غیرعلامتی طویل reserved:13;
#ورنہ
		غیرعلامتی طویل reserved:45;
#اگر_ختم
		غیرعلامتی طویل type:1;
	};
};

#اگر SBI_PMU_FW_CTR_MAX >= بٹ_فی_LONG
#غلطی "Can't handle firmware counters beyond BITS_PER_LONG"
#اگر_ختم

/** HW event configuration parameters */
ساخت sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_ڈیٹا;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
ساخت sbi_pmu_hart_state {
	/* HART to which this state belongs */
	uint32_t hartid;
	/* Counter to enabled event mapping */
	uint32_t فعال_events[SBI_PMU_HW_CTR_MAX + SBI_PMU_FW_CTR_MAX];
	/* Bitmap of firmware counters started */
	غیرعلامتی طویل fw_counters_started;
	/* if true, SSE is enabled */
	bool sse_enabled;
	/*
	 * Counter values for SBI firmware events and event codes
	 * for platform firmware events. Both are mutually exclusive
	 * and hence can optimally share the same memory.
	 */
	uint64_t fw_counters_ڈیٹا[SBI_PMU_FW_CTR_MAX];
	/* HW events configuration parameters from
	 * sbi_pmu_ctr_cfg_match() command which are
	 * used for restoring RAW hardware events after
	 * cpu suspending.
	 */
	ساخت sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
ساکن غیرعلامتی طویل phs_ptr_انحراف;

#تعریف_کرو pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_انحراف ? sbi_scratch_read_type((__scratch), خالی *, phs_ptr_انحراف) : تہی

#تعریف_کرو pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#تعریف_کرو pmu_سیٹ_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), خالی *, phs_ptr_انحراف, (__phs))

/* Platform specific PMU device */
ساکن ثابت ساخت sbi_pmu_device *pmu_dev = تہی;

/* Mapping between event range and possible counters  */
ساکن ساخت sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
ساکن uint32_t عدد_hw_events;
/* Maximum number of hardware counters available */
ساکن uint32_t عدد_hw_ctrs;

/* Maximum number of counters available */
ساکن uint32_t کل_ctrs;

/* Helper macros to retrieve event idx and code type */
#تعریف_کرو get_cidx_type(x) \
  (((x) & SBI_PMU_EVENT_IDX_TYPE_نقاب) >> SBI_PMU_EVENT_IDX_TYPE_انحراف)
#تعریف_کرو get_cidx_رمز(x) (x & SBI_PMU_EVENT_IDX_رمز_نقاب)

/**
 * Perform a sanity check on event & counter mappings with event range overlap check
 * @param evtA Pointer to the existing hw event structure
 * @param evtB Pointer to the new hw event structure
 *
 * Return false if the range doesn't overlap, true otherwise
 */
ساکن bool pmu_event_range_overlap(ساخت sbi_pmu_hw_event *evtA,
				    ساخت sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	اگر (((evtA->انتہا_idx < evtB->آغاز_idx) && (evtA->انتہا_idx < evtB->انتہا_idx)) ||
	   ((evtA->آغاز_idx > evtB->آغاز_idx) && (evtA->آغاز_idx > evtB->انتہا_idx)))
		واپس false;
	واپس true;
}

ساکن bool pmu_event_select_overlap(ساخت sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_نقاب)
{
	اگر ((evt->select == select_val) && (evt->select_نقاب == select_نقاب))
		واپس true;

	واپس false;
}

ساکن صحیح pmu_event_validate(ساخت sbi_pmu_hart_state *phs,
			      غیرعلامتی طویل event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_رمز = get_cidx_رمز(event_idx);
	uint32_t event_idx_رمز_max = -1;
	uint32_t cache_ops_نتیجہ, cache_ops_id, cache_id;

	انتخاب(event_idx_type) {
	حالت SBI_PMU_EVENT_TYPE_HW:
		event_idx_رمز_max = SBI_PMU_HW_GENERAL_MAX;
		توڑو;
	حالت SBI_PMU_EVENT_TYPE_FW:
		اگر ((event_idx_رمز >= SBI_PMU_FW_MAX &&
		    event_idx_رمز <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_رمز > SBI_PMU_FW_PLATFORM)
			واپس SBI_EINVAL;

		اگر (SBI_PMU_FW_PLATFORM == event_idx_رمز &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			واپس pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		ورنہ
			event_idx_رمز_max = SBI_PMU_FW_MAX;
		توڑو;
	حالت SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_نتیجہ = event_idx_رمز &
					SBI_PMU_EVENT_HW_CACHE_OPS_نتیجہ;
		cache_ops_id = (event_idx_رمز &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_نقاب) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_انحراف;
		cache_id = (event_idx_رمز &
			    SBI_PMU_EVENT_HW_CACHE_ID_نقاب) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_انحراف;
		اگر ((cache_ops_نتیجہ < SBI_PMU_HW_CACHE_نتیجہ_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			واپس event_idx_type;
		ورنہ
			واپس SBI_EINVAL;
		توڑو;
	حالت SBI_PMU_EVENT_TYPE_HW_RAW:
	حالت SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_رمز_max = 1; // event_idx.code should be zero
		توڑو;
	طےشدہ:
		واپس SBI_EINVAL;
	}

	اگر (event_idx_رمز < event_idx_رمز_max)
		واپس event_idx_type;

	واپس SBI_EINVAL;
}

ساکن صحیح pmu_ctr_validate(ساخت sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_رمز)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	اگر (cidx >= کل_ctrs)
		واپس SBI_EINVAL;

	event_idx_val = phs->فعال_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	اگر (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		واپس SBI_EINVAL;

	*event_idx_رمز = get_cidx_رمز(event_idx_val);

	واپس event_idx_type;
}

ساکن bool pmu_ctr_idx_validate(غیرعلامتی طویل cbase, غیرعلامتی طویل cmask)
{
	غیرعلامتی طویل آخری;

	اگر (!cmask)
		واپس false;

	آخری = sbi_fls(cmask);
	اگر (cbase > -1UL - آخری)
		واپس false;

	واپس (cbase + آخری) < کل_ctrs;
}

صحیح sbi_pmu_ctr_fw_read(غیرعلامتی طویل cidx, uint64_t *cval, bool بلند_بٹ)
{
	صحیح event_idx_type;
	uint32_t event_رمز;
	ساخت sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	اگر (unlikely(!phs))
		واپس SBI_EINVAL;

	اگر (cidx < عدد_hw_ctrs || cidx >= کل_ctrs)
		واپس SBI_EINVAL;

#اگر __riscv_xlen > 32
	اگر (بلند_بٹ)
		واپس 0;
#اگر_ختم

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_رمز);
	اگر (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		واپس SBI_EINVAL;

	اگر ((event_رمز >= SBI_PMU_FW_MAX &&
	    event_رمز <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_رمز > SBI_PMU_FW_PLATFORM)
		واپس SBI_EINVAL;

	اگر (SBI_PMU_FW_PLATFORM == event_رمز) {
		اگر (pmu_dev && pmu_dev->fw_شمار_read_قدر)
			*cval = pmu_dev->fw_شمار_read_قدر(phs->hartid,
							       cidx -
							       عدد_hw_ctrs);
		ورنہ
			*cval = 0;
	} ورنہ
		*cval = phs->fw_counters_ڈیٹا[cidx - عدد_hw_ctrs];

	واپس 0;
}

ساکن صحیح pmu_جوڑو_hw_event_map(u32 eidx_آغاز, u32 eidx_انتہا, u32 cmap,
				uint64_t select, uint64_t select_نقاب)
{
	صحیح i = 0;
	bool is_overlap;
	ساخت sbi_pmu_hw_event *event = &hw_event_map[عدد_hw_events];
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_نقاب = sbi_hart_mhpm_نقاب(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	اگر (eidx_آغاز > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_نقاب))
		واپس SBI_EDENIED;

	اگر (عدد_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		واپس SBI_EFAIL;
	}

	event->آغاز_idx = eidx_آغاز;
	event->انتہا_idx = eidx_انتہا;

	/* Sanity check */
	برائے (i = 0; i < عدد_hw_events; i++) {
		اگر (eidx_آغاز == SBI_PMU_EVENT_RAW_IDX ||
			eidx_آغاز == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_نقاب);
		ورنہ
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		اگر (is_overlap)
			جاؤ reset_event;
	}

	event->select_نقاب = select_نقاب;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_نقاب;
	event->select = select;
	عدد_hw_events++;

	واپس 0;

reset_event:
	event->آغاز_idx = 0;
	event->انتہا_idx = 0;
	واپس SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
صحیح sbi_pmu_جوڑو_hw_event_شمار_map(u32 eidx_آغاز, u32 eidx_انتہا, u32 cmap)
{
	اگر ((eidx_آغاز > eidx_انتہا) || eidx_آغاز >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_انتہا >= SBI_PMU_EVENT_RAW_IDX)
		واپس SBI_EINVAL;

	واپس pmu_جوڑو_hw_event_map(eidx_آغاز, eidx_انتہا, cmap, 0, 0);
}

صحیح sbi_pmu_جوڑو_raw_event_شمار_map(uint64_t select, uint64_t select_نقاب, u32 cmap)
{
	واپس pmu_جوڑو_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_نقاب);
}

خالی sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_نقاب());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_حد_سے_تجاوز);
}

ساکن صحیح pmu_ctr_enable_irq_hw(صحیح ctr_idx)
{
	غیرعلامتی طویل mhpmevent_csr;
	غیرعلامتی طویل mhpmevent_curr;
	غیرعلامتی طویل mip_val;
	غیرعلامتی طویل کا_نقاب;

	اگر (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		واپس SBI_EFAIL;

#اگر __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	کا_نقاب = (uint32_t)~MHPMEVENTH_کا;
#ورنہ
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	کا_نقاب = ~MHPMEVENT_کا;
#اگر_ختم

	mhpmevent_curr = csr_read_عدد(mhpmevent_csr);
	mip_val = csr_read(CSR_MIP);
	/**
	 * Clear out the OF bit so that next interrupt can be enabled.
	 * This should be done only when the corresponding overflow interrupt
	 * bit is cleared. That indicates that software has already handled the
	 * previous interrupts or the hardware yet to set an overflow interrupt.
	 * Otherwise, there will be race conditions where we may clear the bit
	 * the software is yet to handle the interrupt.
	 */
	اگر (!(mip_val & sbi_pmu_irq_نقاب())) {
		mhpmevent_curr &= کا_نقاب;
		csr_write_عدد(mhpmevent_csr, mhpmevent_curr);
	}

	واپس 0;
}

ساکن خالی pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#اگر __riscv_xlen == 32
	csr_write_عدد(CSR_MCYCLE + cidx, 0);
	csr_write_عدد(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_عدد(CSR_MCYCLEH + cidx, ival >> بٹ_فی_LONG);
#ورنہ
	csr_write_عدد(CSR_MCYCLE + cidx, ival);
#اگر_ختم
}

ساکن صحیح pmu_ctr_آغاز_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	غیرعلامتی طویل mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	اگر (cidx >= عدد_hw_ctrs || cidx == 1)
		واپس SBI_EINVAL;

	اگر (sbi_hart_priv_نسخہ(scratch) < SBI_HART_PRIV_VER_1_11) {
		اگر (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		واپس 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	اگر (!__test_bit(cidx, &mctr_inhbt))
		واپس SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	اگر (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	اگر (pmu_dev && pmu_dev->hw_شمار_enable_irq)
		pmu_dev->hw_شمار_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	واپس 0;
}

صحیح sbi_pmu_irq_ثنائی_ہندسہ(خالی)
{
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		واپس IRQ_PMU_OVF;
	اگر (pmu_dev && pmu_dev->hw_شمار_irq_ثنائی_ہندسہ)
		واپس pmu_dev->hw_شمار_irq_ثنائی_ہندسہ();

	واپس -1;
}

غیرعلامتی طویل sbi_pmu_irq_نقاب(خالی)
{
	صحیح irq_ثنائی_ہندسہ = sbi_pmu_irq_ثنائی_ہندسہ();

	اگر (irq_ثنائی_ہندسہ < 0)
		واپس 0;

	واپس ثنائی_ہندسہ(irq_ثنائی_ہندسہ);
}

ساکن صحیح pmu_ctr_آغاز_fw(ساخت sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_رمز,
			    uint64_t event_ڈیٹا, uint64_t ival,
			    bool ival_update)
{
	صحیح ret;

	اگر ((event_رمز >= SBI_PMU_FW_MAX &&
	    event_رمز <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_رمز > SBI_PMU_FW_PLATFORM)
		واپس SBI_EINVAL;

	اگر (phs->fw_counters_started & ثنائی_ہندسہ(cidx - عدد_hw_ctrs))
		واپس SBI_EALREADY_STARTED;

	اگر (SBI_PMU_FW_PLATFORM == event_رمز) {
		اگر (!pmu_dev ||
		    !pmu_dev->fw_شمار_write_قدر ||
		    !pmu_dev->fw_شمار_آغاز) {
			واپس SBI_EINVAL;
		}

		اگر (ival_update)
			pmu_dev->fw_شمار_write_قدر(phs->hartid,
							cidx - عدد_hw_ctrs,
							ival);

		ret = pmu_dev->fw_شمار_آغاز(phs->hartid,
						 cidx - عدد_hw_ctrs,
						 event_ڈیٹا);
		اگر (ret)
			واپس ret;
	} ورنہ {
		اگر (ival_update)
			phs->fw_counters_ڈیٹا[cidx - عدد_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= ثنائی_ہندسہ(cidx - عدد_hw_ctrs);

	واپس 0;
}

ساکن خالی pmu_update_inhibit_flags(غیرعلامتی طویل flags, uint64_t *mhpmevent_val)
{
	اگر (flags & SBI_PMU_CFG_FLAG_سیٹ_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	اگر (flags & SBI_PMU_CFG_FLAG_سیٹ_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	اگر (flags & SBI_PMU_CFG_FLAG_سیٹ_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	اگر (flags & SBI_PMU_CFG_FLAG_سیٹ_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

ساکن صحیح pmu_update_hw_mhpmevent(ساخت sbi_pmu_hw_event *hw_evt, صحیح ctr_idx,
				   غیرعلامتی طویل flags, غیرعلامتی طویل eindex,
				   uint64_t ڈیٹا)
{
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت ساخت sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_مقصد_mhpmevent(plat, eindex, ڈیٹا);

	اگر (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		واپس SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_نقاب) |
				 MHPMEVENT_MINH | MHPMEVENT_کا;

	اگر (pmu_dev && pmu_dev->hw_شمار_disable_irq)
		pmu_dev->hw_شمار_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	اگر (pmu_dev && pmu_dev->hw_شمار_filter_منوال)
		pmu_dev->hw_شمار_filter_منوال(flags, ctr_idx);

#اگر __riscv_xlen == 32
	csr_write_عدد(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_عدد(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> بٹ_فی_LONG);
#ورنہ
	csr_write_عدد(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#اگر_ختم

	واپس 0;
}

صحیح sbi_pmu_ctr_آغاز(غیرعلامتی طویل cbase, غیرعلامتی طویل cmask,
		      غیرعلامتی طویل flags, uint64_t ival)
{
	ساخت sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	اگر (unlikely(!phs))
		واپس SBI_EINVAL;

	صحیح event_idx_type;
	uint32_t event_رمز;
	صحیح ret = SBI_EINVAL;
	bool bUpdate = false;
	صحیح i, cidx;
	uint64_t edata;

	اگر (!pmu_ctr_idx_validate(cbase, cmask))
		واپس ret;

	اگر (flags & ~SBI_PMU_آغاز_FLAGS_نقاب)
		واپس SBI_ERR_INVALID_PARAM;

	اگر (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		واپس SBI_ENO_SHMEM;

	اگر (flags & SBI_PMU_آغاز_FLAG_سیٹ_INIT_قدر)
		bUpdate = true;

	for_each_سیٹ_ثنائی_ہندسہ(i, &cmask, بٹ_فی_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_رمز);
		اگر (event_idx_type < 0)
			/* Continue the start operation for other counters */
			جاری;
		ورنہ اگر (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_رمز == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_ڈیٹا[cidx - عدد_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_آغاز_fw(phs, cidx, event_رمز, edata,
					       ival, bUpdate);
			اگر (ret)
				واپس ret;
		} ورنہ {
			اگر (cidx >= 3) {
				ساخت sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->فعال_events[cidx],
							ev_cfg->event_ڈیٹا);
				اگر (ret)
					واپس ret;
			}
			ret = pmu_ctr_آغاز_hw(cidx, ival, bUpdate);
			اگر (ret)
				واپس ret;
		}
	}

	واپس ret;
}

ساکن صحیح pmu_ctr_stop_hw(uint32_t cidx)
{
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	غیرعلامتی طویل mctr_inhbt;

	اگر (sbi_hart_priv_نسخہ(scratch) < SBI_HART_PRIV_VER_1_11)
		واپس 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	اگر (cidx >= عدد_hw_ctrs || cidx == 1)
		واپس SBI_EINVAL;

	اگر (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		اگر (pmu_dev && pmu_dev->hw_شمار_disable_irq) {
			pmu_dev->hw_شمار_disable_irq(cidx);
		}
		واپس 0;
	} ورنہ
		واپس SBI_EALREADY_STOPPED;
}

ساکن صحیح pmu_ctr_stop_fw(ساخت sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_رمز)
{
	صحیح ret;

	اگر ((event_رمز >= SBI_PMU_FW_MAX &&
	    event_رمز <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_رمز > SBI_PMU_FW_PLATFORM)
		واپس SBI_EINVAL;

	اگر (!(phs->fw_counters_started & ثنائی_ہندسہ(cidx - عدد_hw_ctrs)))
		واپس SBI_EALREADY_STOPPED;

	اگر (SBI_PMU_FW_PLATFORM == event_رمز &&
	    pmu_dev && pmu_dev->fw_شمار_stop) {
		ret = pmu_dev->fw_شمار_stop(phs->hartid, cidx - عدد_hw_ctrs);
		اگر (ret)
			واپس ret;
	}

	phs->fw_counters_started &= ~ثنائی_ہندسہ(cidx - عدد_hw_ctrs);

	واپس 0;
}

ساکن صحیح pmu_reset_hw_mhpmevent(صحیح ctr_idx)
{
	اگر (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		واپس SBI_EFAIL;
#اگر __riscv_xlen == 32
	csr_write_عدد(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	اگر (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_عدد(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#ورنہ
	csr_write_عدد(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#اگر_ختم

	واپس 0;
}

صحیح sbi_pmu_ctr_stop(غیرعلامتی طویل cbase, غیرعلامتی طویل cmask,
		     غیرعلامتی طویل flag)
{
	ساخت sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	اگر (unlikely(!phs))
		واپس SBI_EINVAL;

	صحیح ret = SBI_EINVAL;
	صحیح event_idx_type;
	uint32_t event_رمز;
	صحیح i, cidx;

	اگر (!pmu_ctr_idx_validate(cbase, cmask))
		واپس ret;

	اگر (flag & ~SBI_PMU_STOP_FLAGS_نقاب)
		واپس SBI_ERR_INVALID_PARAM;

	اگر (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		واپس SBI_ENO_SHMEM;

	for_each_سیٹ_ثنائی_ہندسہ(i, &cmask, بٹ_فی_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_رمز);
		اگر (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			جاری;

		ورنہ اگر (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_رمز);
		ورنہ
			ret = pmu_ctr_stop_hw(cidx);

		اگر(ret)
			واپس ret;

		اگر (cidx > (CSR_INSTRET - CSR_چکر) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->فعال_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	اگر (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_نقاب());

	واپس ret;
}

ساکن صحیح pmu_fixed_ctr_update_inhibit_بٹ(صحیح fixed_ctr, غیرعلامتی طویل flags)
{
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#اگر __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#اگر_ختم
	اگر (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_شمار_filter_منوال))
		واپس fixed_ctr;

	انتخاب (fixed_ctr) {
	حالت 0:
		cfg_csr_no = CSR_MCYCLECFG;
#اگر __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#اگر_ختم
		توڑو;
	حالت 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#اگر __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#اگر_ختم
		توڑو;
	طےشدہ:
		واپس SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#اگر __riscv_xlen == 32
		csr_write_عدد(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_عدد(cfgh_csr_no, cfg_val >> بٹ_فی_LONG);
#ورنہ
		csr_write_عدد(cfg_csr_no, cfg_val);
#اگر_ختم
	}
	اگر (pmu_dev && pmu_dev->hw_شمار_filter_منوال)
		pmu_dev->hw_شمار_filter_منوال(flags, fixed_ctr);
	واپس fixed_ctr;
}

ساکن صحیح pmu_ctr_ڈھونڈو_fixed_hw(غیرعلامتی طویل evt_idx_رمز)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	اگر (evt_idx_رمز == SBI_PMU_HW_CPU_ادوار)
		واپس 0;
	ورنہ اگر (evt_idx_رمز == SBI_PMU_HW_INSTRUCTIONS)
		واپس 2;
	ورنہ
		واپس SBI_EINVAL;
}

ساکن صحیح pmu_ctr_ڈھونڈو_hw(ساخت sbi_pmu_hart_state *phs,
			   غیرعلامتی طویل cbase, غیرعلامتی طویل cmask,
			   غیرعلامتی طویل flags,
			   غیرعلامتی طویل event_idx, uint64_t ڈیٹا)
{
	غیرعلامتی طویل ctr_نقاب;
	صحیح i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	ساخت sbi_pmu_hw_event *عارضی;
	غیرعلامتی طویل mctr_inhbt = 0;
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	اگر (cbase >= عدد_hw_ctrs)
		واپس SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_ڈھونڈو_fixed_hw(event_idx);
	اگر (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		واپس pmu_fixed_ctr_update_inhibit_بٹ(fixed_ctr, flags);

	اگر (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	برائے (i = 0; i < عدد_hw_events; i++) {
		عارضی = &hw_event_map[i];
		اگر ((عارضی->آغاز_idx > event_idx && event_idx < عارضی->انتہا_idx) ||
		    (عارضی->آغاز_idx < event_idx && event_idx > عارضی->انتہا_idx))
			جاری;

		/* For raw events, event data is used as the select value */
		اگر (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_نقاب = عارضی->select_نقاب;

			/* The non-event map bits of data should match the selector */
			اگر (عارضی->select != (ڈیٹا & select_نقاب))
				جاری;
		}
		/* Fixed counters should not be part of the search */
		ctr_نقاب = عارضی->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_نقاب);
		for_each_سیٹ_ثنائی_ہندسہ_from(cbase, &ctr_نقاب, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			اگر (phs->فعال_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				جاری;
			/* If mcountinhibit is supported, the bit must be enabled */
			اگر ((sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				جاری;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			توڑو;
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
			واپس SBI_EFAIL;

		واپس pmu_fixed_ctr_update_inhibit_بٹ(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(عارضی, ctr_idx, flags, event_idx, ڈیٹا);

	اگر (!ret)
		ret = ctr_idx;

	واپس ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
ساکن صحیح pmu_ctr_ڈھونڈو_fw(ساخت sbi_pmu_hart_state *phs,
			   غیرعلامتی طویل cbase, غیرعلامتی طویل cmask,
			   uint32_t event_رمز, uint64_t edata)
{
	صحیح i, cidx;

	for_each_سیٹ_ثنائی_ہندسہ(i, &cmask, بٹ_فی_LONG) {
		cidx = i + cbase;
		اگر (cidx < عدد_hw_ctrs || کل_ctrs <= cidx)
			جاری;
		اگر (phs->فعال_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			جاری;
		اگر (SBI_PMU_FW_PLATFORM == event_رمز &&
		    pmu_dev && pmu_dev->fw_شمار_مطابقت_encoding) {
			اگر (!pmu_dev->fw_شمار_مطابقت_encoding(phs->hartid,
							    cidx - عدد_hw_ctrs,
							    edata))
				جاری;
		}

		واپس cidx;
	}

	واپس SBI_ENOTSUPP;
}

صحیح sbi_pmu_ctr_cfg_مطابقت(غیرعلامتی طویل cidx_بنیاد, غیرعلامتی طویل cidx_نقاب,
			  غیرعلامتی طویل flags, غیرعلامتی طویل event_idx,
			  uint64_t event_ڈیٹا)
{
	ساخت sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	اگر (unlikely(!phs))
		واپس SBI_EINVAL;

	صحیح ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_رمز;

	اگر (!pmu_ctr_idx_validate(cidx_بنیاد, cidx_نقاب))
		واپس SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_ڈیٹا);
	اگر (event_type < 0)
		واپس SBI_EINVAL;
	event_رمز = get_cidx_رمز(event_idx);

	اگر (flags & SBI_PMU_CFG_FLAG_SKIP_مطابقت) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		غیرعلامتی طویل cidx_پہلا = cidx_بنیاد + sbi_ffs(cidx_نقاب);

		اگر (cidx_پہلا >= کل_ctrs)
			واپس SBI_EINVAL;

		اگر (phs->فعال_events[cidx_پہلا] == SBI_PMU_EVENT_IDX_INVALID)
			واپس SBI_EINVAL;
		ctr_idx = cidx_پہلا;
		جاؤ skip_مطابقت;
	}

	اگر (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_ڈھونڈو_fw(phs, cidx_بنیاد, cidx_نقاب,
					  event_رمز, event_ڈیٹا);
		اگر ((event_رمز == SBI_PMU_FW_PLATFORM) && (ctr_idx >= عدد_hw_ctrs))
			phs->fw_counters_ڈیٹا[ctr_idx - عدد_hw_ctrs] =
								event_ڈیٹا;
	} ورنہ {
		ctr_idx = pmu_ctr_ڈھونڈو_hw(phs, cidx_بنیاد, cidx_نقاب, flags,
					  event_idx, event_ڈیٹا);
		اگر (ctr_idx >= 0) {
			ساخت sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_ڈیٹا = event_ڈیٹا;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_نقاب;
		}
	}

	اگر (ctr_idx < 0)
		واپس SBI_ENOTSUPP;

	phs->فعال_events[ctr_idx] = event_idx;
skip_مطابقت:
	اگر (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		اگر (flags & SBI_PMU_CFG_FLAG_CLEAR_قدر)
			pmu_ctr_write_hw(ctr_idx, 0);
		اگر (flags & SBI_PMU_CFG_FLAG_AUTO_آغاز)
			pmu_ctr_آغاز_hw(ctr_idx, 0, false);
	} ورنہ اگر (event_type == SBI_PMU_EVENT_TYPE_FW) {
		اگر (flags & SBI_PMU_CFG_FLAG_CLEAR_قدر)
			phs->fw_counters_ڈیٹا[ctr_idx - عدد_hw_ctrs] = 0;
		اگر (flags & SBI_PMU_CFG_FLAG_AUTO_آغاز) {
			اگر (SBI_PMU_FW_PLATFORM == event_رمز &&
			    pmu_dev && pmu_dev->fw_شمار_آغاز) {
				ret = pmu_dev->fw_شمار_آغاز(
					phs->hartid,
					ctr_idx - عدد_hw_ctrs, event_ڈیٹا);
				اگر (ret)
					واپس ret;
			}
			phs->fw_counters_started |= ثنائی_ہندسہ(ctr_idx - عدد_hw_ctrs);
		}
	}

	واپس ctr_idx;
}

صحیح sbi_pmu_ctr_incr_fw(شمار sbi_pmu_fw_event_رمز_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = تہی;
	ساخت sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	اگر (unlikely(!phs))
		واپس 0;

	اگر (likely(!phs->fw_counters_started))
		واپس 0;

	اگر (unlikely(fw_id >= SBI_PMU_FW_MAX))
		واپس SBI_EINVAL;

	برائے (cidx = عدد_hw_ctrs; cidx < کل_ctrs; cidx++) {
		اگر (get_cidx_رمز(phs->فعال_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & ثنائی_ہندسہ(cidx - عدد_hw_ctrs))) {
			fcounter = &phs->fw_counters_ڈیٹا[cidx - عدد_hw_ctrs];
			توڑو;
		}
	}

	اگر (fcounter)
		(*fcounter)++;

	واپس 0;
}

غیرعلامتی طویل sbi_pmu_عدد_ctr(خالی)
{
	واپس (عدد_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

صحیح sbi_pmu_ctr_get_info(uint32_t cidx, غیرعلامتی طویل *ctr_info)
{
	صحیح چوڑائی;
	اتحاد sbi_pmu_ctr_info cinfo = {0};
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	غیرعلامتی طویل شمار_نقاب = (غیرعلامتی طویل)sbi_hart_mhpm_نقاب(scratch) |
				     SBI_PMU_CY_IR_نقاب;

	/* Sanity check */
	اگر (cidx >= کل_ctrs)
		واپس SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	اگر (cidx < عدد_hw_ctrs) {
		اگر (!(__test_bit(cidx, &شمار_نقاب)))
			واپس SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_چکر + cidx;
		/* mcycle & minstret are always 64 bit */
		اگر (cidx == 0 || cidx == 2)
			cinfo.چوڑائی = 63;
		ورنہ
			cinfo.چوڑائی = sbi_hart_mhpm_بٹ(scratch) - 1;
	} ورنہ {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.چوڑائی = 63;
		اگر (pmu_dev && pmu_dev->fw_شمار_چوڑائی) {
			چوڑائی = pmu_dev->fw_شمار_چوڑائی();
			اگر (چوڑائی)
				cinfo.چوڑائی = چوڑائی - 1;
		}
	}

	*ctr_info = cinfo.قدر;

	واپس 0;
}

صحیح sbi_pmu_event_get_info(غیرعلامتی طویل shmem_phys_lo, غیرعلامتی طویل shmem_phys_hi,
			   غیرعلامتی طویل عدد_events, غیرعلامتی طویل flags)
{
	غیرعلامتی طویل shmem_سائز = عدد_events * حجم(ساخت sbi_pmu_event_info);
	صحیح i, j, event_type;
	ساخت sbi_pmu_event_info *einfo;
	ساخت sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	ساخت sbi_pmu_hw_event *عارضی;
	bool ملا = false;

	اگر (flags != 0)
		واپس SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	اگر (!عدد_events || (shmem_phys_lo & 0xF))
		واپس SBI_ERR_INVALID_PARAM;

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
		واپس SBI_EINVALID_ADDR;

	اگر (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_سائز, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		واپس SBI_ERR_INVALID_پتہ;

	sbi_hart_protection_عارضی_map_range(shmem_phys_lo, shmem_سائز);

	einfo = (ساخت sbi_pmu_event_info *)(shmem_phys_lo);
	برائے (i = 0; i < عدد_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		اگر (event_idx & SBI_PMU_EVENT_IDX_MBZ_نقاب)
			واپس SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_ڈیٹا);
		اگر (event_type < 0) {
			einfo[i].output = 0;
		} ورنہ اگر (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} ورنہ {
			برائے (j = 0; j < عدد_hw_events; j++) {
				عارضی = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				اگر (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					اگر (عارضی->آغاز_idx > event_idx ||
					    event_idx > عارضی->انتہا_idx)
						جاری;
					/* just match the selector */
					اگر (عارضی->select == (einfo[i].event_ڈیٹا &
									عارضی->select_نقاب)) {
						ملا = true;
						توڑو;
					}
				} ورنہ اگر (عارضی->آغاز_idx <= event_idx &&
					   event_idx <= عارضی->انتہا_idx) {
					ملا = true;
					توڑو;
				}
			}
			اگر (ملا)
				einfo[i].output = 1;
			ورنہ
				einfo[i].output = 0;
			ملا = false;
		}
	}

	sbi_hart_protection_عارضی_unmap_range(shmem_phys_lo, shmem_سائز);

	واپس 0;
}

ساکن خالی pmu_reset_event_map(ساخت sbi_pmu_hart_state *phs)
{
	صحیح j;

	/* Initialize the counter to event mapping table */
	برائے (j = 3; j < کل_ctrs; j++)
		phs->فعال_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	برائے (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_ڈیٹا[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

ثابت ساخت sbi_pmu_device *sbi_pmu_get_device(خالی)
{
	واپس pmu_dev;
}

خالی sbi_pmu_سیٹ_device(ثابت ساخت sbi_pmu_device *dev)
{
	اگر (!dev || pmu_dev)
		واپس;

	pmu_dev = dev;
}

خالی sbi_pmu_exit(ساخت sbi_scratch *scratch)
{
	ساخت sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	اگر (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	اگر (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	اگر (unlikely(!phs))
		واپس;

	pmu_reset_event_map(phs);
}

ساکن خالی pmu_sse_enable(uint32_t event_id)
{
	غیرعلامتی طویل irq_نقاب = sbi_pmu_irq_نقاب();

	csr_سیٹ(CSR_MIE, irq_نقاب);
}

ساکن خالی pmu_sse_disable(uint32_t event_id)
{
	غیرعلامتی طویل irq_نقاب = sbi_pmu_irq_نقاب();

	csr_clear(CSR_MIE, irq_نقاب);
	csr_clear(CSR_MIP, irq_نقاب);
}

ساکن خالی pmu_sse_complete(uint32_t event_id)
{
	csr_سیٹ(CSR_MIE, sbi_pmu_irq_نقاب());
}

ساکن خالی pmu_sse_رجسٹر(uint32_t event_id)
{
	ساخت sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_نقاب());
}

ساکن خالی pmu_sse_unregister(uint32_t event_id)
{
	ساخت sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_سیٹ(CSR_MIDELEG, sbi_pmu_irq_نقاب());
}

ساکن ثابت ساخت sbi_sse_cb_ops pmu_sse_cb_ops = {
	.رجسٹر_cb = pmu_sse_رجسٹر,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

صحیح sbi_pmu_init(ساخت sbi_scratch *scratch, bool cold_boot)
{
	صحیح hpm_گنتی = sbi_fls(sbi_hart_mhpm_نقاب(scratch));
	ساخت sbi_pmu_hart_state *phs;
	ثابت ساخت sbi_platform *plat;
	صحیح rc;

	اگر (cold_boot) {
		hw_event_map = sbi_calloc(حجم(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		اگر (!hw_event_map)
			واپس SBI_ENOMEM;

		phs_ptr_انحراف = sbi_scratch_alloc_type_انحراف(خالی *);
		اگر (!phs_ptr_انحراف) {
			sbi_free(hw_event_map);
			واپس SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		اگر (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		اگر (!hpm_گنتی)
			/* Only CY, TM & IR are implemented in the hw */
			عدد_hw_ctrs = 3;
		ورنہ
			عدد_hw_ctrs = hpm_گنتی + 1;

		اگر (عدد_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			واپس SBI_EINVAL;

		کل_ctrs = عدد_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		اگر (sbi_pmu_irq_ثنائی_ہندسہ() >= 0)
			sbi_sse_جوڑو_event(SBI_SSE_EVENT_LOCAL_PMU_حد_سے_تجاوز, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	اگر (!phs) {
		phs = sbi_zalloc(حجم(*phs));
		اگر (!phs)
			واپس SBI_ENOMEM;
		phs->hartid = current_hartid();
		pmu_سیٹ_hart_state_ptr(scratch, phs);
	}

	pmu_reset_event_map(phs);

	/* First three counters are fixed by the priv spec and we enable it by default */
	phs->فعال_events[0] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_انحراف) |
				SBI_PMU_HW_CPU_ادوار;
	phs->فعال_events[1] = SBI_PMU_EVENT_IDX_INVALID;
	phs->فعال_events[2] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_انحراف) |
				SBI_PMU_HW_INSTRUCTIONS;

	واپس 0;
}
