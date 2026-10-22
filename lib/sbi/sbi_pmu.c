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
	uint32_t پیل_idx;
	uint32_t پای_ټکی_idx;
	/* Event selector value used only for raw events. The event select value
	 * can be a even id or a selector value for set of events encoded in few
	 * bits. In case latter, the bits used for encoding of the events should
	 * be zeroed out in the select value.
	 */
	uint64_t select;
	 /**
	  * The select_mask indicates which bits are encoded for the event(s).
	  */
	uint64_t select_ماسک;
};

/* Information about PMU counters as per SBI specification */
اتحاد sbi_pmu_ctr_info {
	بینښه اوږد ارزښت;
	بنية {
		بینښه اوږد csr:12;
		بینښه اوږد پلنوالی:6;
#إذا __riscv_xlen == 32
		بینښه اوږد reserved:13;
#وإلا
		بینښه اوږد reserved:45;
#د_که_پای
		بینښه اوږد type:1;
	};
};

#إذا SBI_PMU_FW_CTR_MAX >= بټونه_فی_LONG
#تېروتنه "Can't handle firmware counters beyond BITS_PER_LONG"
#د_که_پای

/** HW event configuration parameters */
بنية sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_معلومات;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
بنية sbi_pmu_hart_state {
	/* HART to which this state belongs */
	uint32_t hartid;
	/* Counter to enabled event mapping */
	uint32_t فعال_events[SBI_PMU_HW_CTR_MAX + SBI_PMU_FW_CTR_MAX];
	/* Bitmap of firmware counters started */
	بینښه اوږد fw_counters_started;
	/* if true, SSE is enabled */
	bool sse_enabled;
	/*
	 * Counter values for SBI firmware events and event codes
	 * for platform firmware events. Both are mutually exclusive
	 * and hence can optimally share the same memory.
	 */
	uint64_t fw_counters_معلومات[SBI_PMU_FW_CTR_MAX];
	/* HW events configuration parameters from
	 * sbi_pmu_ctr_cfg_match() command which are
	 * used for restoring RAW hardware events after
	 * cpu suspending.
	 */
	بنية sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
ساكن بینښه اوږد phs_ptr_دننه_واټن;

#تعریف_کړه pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_دننه_واټن ? sbi_scratch_read_type((__scratch), فراغ *, phs_ptr_دننه_واټن) : عدم

#تعریف_کړه pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#تعریف_کړه pmu_سېټ_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), فراغ *, phs_ptr_دننه_واټن, (__phs))

/* Platform specific PMU device */
ساكن ثابت بنية sbi_pmu_device *pmu_dev = عدم;

/* Mapping between event range and possible counters  */
ساكن بنية sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
ساكن uint32_t عدد_hw_events;
/* Maximum number of hardware counters available */
ساكن uint32_t عدد_hw_ctrs;

/* Maximum number of counters available */
ساكن uint32_t ټول_ctrs;

/* Helper macros to retrieve event idx and code type */
#تعریف_کړه get_cidx_type(x) \
  (((x) & SBI_PMU_EVENT_IDX_TYPE_ماسک) >> SBI_PMU_EVENT_IDX_TYPE_دننه_واټن)
#تعریف_کړه get_cidx_کوډ(x) (x & SBI_PMU_EVENT_IDX_کوډ_ماسک)

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
	إذا (((evtA->پای_ټکی_idx < evtB->پیل_idx) && (evtA->پای_ټکی_idx < evtB->پای_ټکی_idx)) ||
	   ((evtA->پیل_idx > evtB->پیل_idx) && (evtA->پیل_idx > evtB->پای_ټکی_idx)))
		أرجع false;
	أرجع true;
}

ساكن bool pmu_event_select_overlap(بنية sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_ماسک)
{
	إذا ((evt->select == select_val) && (evt->select_ماسک == select_ماسک))
		أرجع true;

	أرجع false;
}

ساكن صحيح pmu_event_validate(بنية sbi_pmu_hart_state *phs,
			      بینښه اوږد event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_کوډ = get_cidx_کوډ(event_idx);
	uint32_t event_idx_کوډ_max = -1;
	uint32_t cache_ops_پایله, cache_ops_id, cache_id;

	بدل(event_idx_type) {
	حالة SBI_PMU_EVENT_TYPE_HW:
		event_idx_کوډ_max = SBI_PMU_HW_GENERAL_MAX;
		اكسر;
	حالة SBI_PMU_EVENT_TYPE_FW:
		إذا ((event_idx_کوډ >= SBI_PMU_FW_MAX &&
		    event_idx_کوډ <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_کوډ > SBI_PMU_FW_PLATFORM)
			أرجع SBI_EINVAL;

		إذا (SBI_PMU_FW_PLATFORM == event_idx_کوډ &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			أرجع pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		وإلا
			event_idx_کوډ_max = SBI_PMU_FW_MAX;
		اكسر;
	حالة SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_پایله = event_idx_کوډ &
					SBI_PMU_EVENT_HW_CACHE_OPS_پایله;
		cache_ops_id = (event_idx_کوډ &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_ماسک) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_دننه_واټن;
		cache_id = (event_idx_کوډ &
			    SBI_PMU_EVENT_HW_CACHE_ID_ماسک) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_دننه_واټن;
		إذا ((cache_ops_پایله < SBI_PMU_HW_CACHE_پایله_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			أرجع event_idx_type;
		وإلا
			أرجع SBI_EINVAL;
		اكسر;
	حالة SBI_PMU_EVENT_TYPE_HW_RAW:
	حالة SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_کوډ_max = 1; // event_idx.code should be zero
		اكسر;
	افتراضي:
		أرجع SBI_EINVAL;
	}

	إذا (event_idx_کوډ < event_idx_کوډ_max)
		أرجع event_idx_type;

	أرجع SBI_EINVAL;
}

ساكن صحيح pmu_ctr_validate(بنية sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_کوډ)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	إذا (cidx >= ټول_ctrs)
		أرجع SBI_EINVAL;

	event_idx_val = phs->فعال_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	إذا (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		أرجع SBI_EINVAL;

	*event_idx_کوډ = get_cidx_کوډ(event_idx_val);

	أرجع event_idx_type;
}

ساكن bool pmu_ctr_idx_validate(بینښه اوږد cbase, بینښه اوږد cmask)
{
	بینښه اوږد وروستی;

	إذا (!cmask)
		أرجع false;

	وروستی = sbi_fls(cmask);
	إذا (cbase > -1UL - وروستی)
		أرجع false;

	أرجع (cbase + وروستی) < ټول_ctrs;
}

صحيح sbi_pmu_ctr_fw_read(بینښه اوږد cidx, uint64_t *cval, bool لوړ_بټونه)
{
	صحيح event_idx_type;
	uint32_t event_کوډ;
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	إذا (unlikely(!phs))
		أرجع SBI_EINVAL;

	إذا (cidx < عدد_hw_ctrs || cidx >= ټول_ctrs)
		أرجع SBI_EINVAL;

#إذا __riscv_xlen > 32
	إذا (لوړ_بټونه)
		أرجع 0;
#د_که_پای

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_کوډ);
	إذا (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		أرجع SBI_EINVAL;

	إذا ((event_کوډ >= SBI_PMU_FW_MAX &&
	    event_کوډ <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_کوډ > SBI_PMU_FW_PLATFORM)
		أرجع SBI_EINVAL;

	إذا (SBI_PMU_FW_PLATFORM == event_کوډ) {
		إذا (pmu_dev && pmu_dev->fw_counter_read_ارزښت)
			*cval = pmu_dev->fw_counter_read_ارزښت(phs->hartid,
							       cidx -
							       عدد_hw_ctrs);
		وإلا
			*cval = 0;
	} وإلا
		*cval = phs->fw_counters_معلومات[cidx - عدد_hw_ctrs];

	أرجع 0;
}

ساكن صحيح pmu_جمع_کړه_hw_event_map(u32 eidx_پیل, u32 eidx_پای_ټکی, u32 cmap,
				uint64_t select, uint64_t select_ماسک)
{
	صحيح i = 0;
	bool is_overlap;
	بنية sbi_pmu_hw_event *event = &hw_event_map[عدد_hw_events];
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_ماسک = sbi_hart_mhpm_ماسک(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	إذا (eidx_پیل > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_ماسک))
		أرجع SBI_EDENIED;

	إذا (عدد_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		أرجع SBI_EFAIL;
	}

	event->پیل_idx = eidx_پیل;
	event->پای_ټکی_idx = eidx_پای_ټکی;

	/* Sanity check */
	لكل (i = 0; i < عدد_hw_events; i++) {
		إذا (eidx_پیل == SBI_PMU_EVENT_RAW_IDX ||
			eidx_پیل == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_ماسک);
		وإلا
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		إذا (is_overlap)
			اذهب reset_event;
	}

	event->select_ماسک = select_ماسک;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_ماسک;
	event->select = select;
	عدد_hw_events++;

	أرجع 0;

reset_event:
	event->پیل_idx = 0;
	event->پای_ټکی_idx = 0;
	أرجع SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
صحيح sbi_pmu_جمع_کړه_hw_event_counter_map(u32 eidx_پیل, u32 eidx_پای_ټکی, u32 cmap)
{
	إذا ((eidx_پیل > eidx_پای_ټکی) || eidx_پیل >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_پای_ټکی >= SBI_PMU_EVENT_RAW_IDX)
		أرجع SBI_EINVAL;

	أرجع pmu_جمع_کړه_hw_event_map(eidx_پیل, eidx_پای_ټکی, cmap, 0, 0);
}

صحيح sbi_pmu_جمع_کړه_raw_event_counter_map(uint64_t select, uint64_t select_ماسک, u32 cmap)
{
	أرجع pmu_جمع_کړه_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_ماسک);
}

فراغ sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_ماسک());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_ډکېدنه);
}

ساكن صحيح pmu_ctr_enable_irq_hw(صحيح ctr_idx)
{
	بینښه اوږد mhpmevent_csr;
	بینښه اوږد mhpmevent_curr;
	بینښه اوږد mip_val;
	بینښه اوږد د_ماسک;

	إذا (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		أرجع SBI_EFAIL;

#إذا __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	د_ماسک = (uint32_t)~MHPMEVENTH_د;
#وإلا
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	د_ماسک = ~MHPMEVENT_د;
#د_که_پای

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
	إذا (!(mip_val & sbi_pmu_irq_ماسک())) {
		mhpmevent_curr &= د_ماسک;
		csr_write_عدد(mhpmevent_csr, mhpmevent_curr);
	}

	أرجع 0;
}

ساكن فراغ pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#إذا __riscv_xlen == 32
	csr_write_عدد(CSR_MCYCLE + cidx, 0);
	csr_write_عدد(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_عدد(CSR_MCYCLEH + cidx, ival >> بټونه_فی_LONG);
#وإلا
	csr_write_عدد(CSR_MCYCLE + cidx, ival);
#د_که_پای
}

ساكن صحيح pmu_ctr_پیل_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بینښه اوږد mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	إذا (cidx >= عدد_hw_ctrs || cidx == 1)
		أرجع SBI_EINVAL;

	إذا (sbi_hart_priv_نسخه(scratch) < SBI_HART_PRIV_VER_1_11) {
		إذا (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		أرجع 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	إذا (!__test_bit(cidx, &mctr_inhbt))
		أرجع SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	إذا (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	إذا (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	أرجع 0;
}

صحيح sbi_pmu_irq_بټ(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		أرجع IRQ_PMU_OVF;
	إذا (pmu_dev && pmu_dev->hw_counter_irq_بټ)
		أرجع pmu_dev->hw_counter_irq_بټ();

	أرجع -1;
}

بینښه اوږد sbi_pmu_irq_ماسک(فراغ)
{
	صحيح irq_بټ = sbi_pmu_irq_بټ();

	إذا (irq_بټ < 0)
		أرجع 0;

	أرجع بټ(irq_بټ);
}

ساكن صحيح pmu_ctr_پیل_fw(بنية sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_کوډ,
			    uint64_t event_معلومات, uint64_t ival,
			    bool ival_update)
{
	صحيح ret;

	إذا ((event_کوډ >= SBI_PMU_FW_MAX &&
	    event_کوډ <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_کوډ > SBI_PMU_FW_PLATFORM)
		أرجع SBI_EINVAL;

	إذا (phs->fw_counters_started & بټ(cidx - عدد_hw_ctrs))
		أرجع SBI_EALREADY_STARTED;

	إذا (SBI_PMU_FW_PLATFORM == event_کوډ) {
		إذا (!pmu_dev ||
		    !pmu_dev->fw_counter_write_ارزښت ||
		    !pmu_dev->fw_counter_پیل) {
			أرجع SBI_EINVAL;
		}

		إذا (ival_update)
			pmu_dev->fw_counter_write_ارزښت(phs->hartid,
							cidx - عدد_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_پیل(phs->hartid,
						 cidx - عدد_hw_ctrs,
						 event_معلومات);
		إذا (ret)
			أرجع ret;
	} وإلا {
		إذا (ival_update)
			phs->fw_counters_معلومات[cidx - عدد_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= بټ(cidx - عدد_hw_ctrs);

	أرجع 0;
}

ساكن فراغ pmu_update_inhibit_flags(بینښه اوږد flags, uint64_t *mhpmevent_val)
{
	إذا (flags & SBI_PMU_CFG_FLAG_سېټ_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	إذا (flags & SBI_PMU_CFG_FLAG_سېټ_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	إذا (flags & SBI_PMU_CFG_FLAG_سېټ_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	إذا (flags & SBI_PMU_CFG_FLAG_سېټ_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

ساكن صحيح pmu_update_hw_mhpmevent(بنية sbi_pmu_hw_event *hw_evt, صحيح ctr_idx,
				   بینښه اوږد flags, بینښه اوږد eindex,
				   uint64_t معلومات)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_منزل_mhpmevent(plat, eindex, معلومات);

	إذا (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		أرجع SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_ماسک) |
				 MHPMEVENT_MINH | MHPMEVENT_د;

	إذا (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	إذا (pmu_dev && pmu_dev->hw_counter_filter_ډېرپېښ)
		pmu_dev->hw_counter_filter_ډېرپېښ(flags, ctr_idx);

#إذا __riscv_xlen == 32
	csr_write_عدد(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_عدد(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> بټونه_فی_LONG);
#وإلا
	csr_write_عدد(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#د_که_پای

	أرجع 0;
}

صحيح sbi_pmu_ctr_پیل(بینښه اوږد cbase, بینښه اوږد cmask,
		      بینښه اوږد flags, uint64_t ival)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	إذا (unlikely(!phs))
		أرجع SBI_EINVAL;

	صحيح event_idx_type;
	uint32_t event_کوډ;
	صحيح ret = SBI_EINVAL;
	bool bUpdate = false;
	صحيح i, cidx;
	uint64_t edata;

	إذا (!pmu_ctr_idx_validate(cbase, cmask))
		أرجع ret;

	إذا (flags & ~SBI_PMU_پیل_FLAGS_ماسک)
		أرجع SBI_ERR_INVALID_PARAM;

	إذا (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		أرجع SBI_ENO_SHMEM;

	إذا (flags & SBI_PMU_پیل_FLAG_سېټ_INIT_ارزښت)
		bUpdate = true;

	for_each_سېټ_بټ(i, &cmask, بټونه_فی_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_کوډ);
		إذا (event_idx_type < 0)
			/* Continue the start operation for other counters */
			استمر;
		وإلا إذا (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_کوډ == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_معلومات[cidx - عدد_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_پیل_fw(phs, cidx, event_کوډ, edata,
					       ival, bUpdate);
			إذا (ret)
				أرجع ret;
		} وإلا {
			إذا (cidx >= 3) {
				بنية sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->فعال_events[cidx],
							ev_cfg->event_معلومات);
				إذا (ret)
					أرجع ret;
			}
			ret = pmu_ctr_پیل_hw(cidx, ival, bUpdate);
			إذا (ret)
				أرجع ret;
		}
	}

	أرجع ret;
}

ساكن صحيح pmu_ctr_stop_hw(uint32_t cidx)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بینښه اوږد mctr_inhbt;

	إذا (sbi_hart_priv_نسخه(scratch) < SBI_HART_PRIV_VER_1_11)
		أرجع 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	إذا (cidx >= عدد_hw_ctrs || cidx == 1)
		أرجع SBI_EINVAL;

	إذا (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		إذا (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		أرجع 0;
	} وإلا
		أرجع SBI_EALREADY_STOPPED;
}

ساكن صحيح pmu_ctr_stop_fw(بنية sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_کوډ)
{
	صحيح ret;

	إذا ((event_کوډ >= SBI_PMU_FW_MAX &&
	    event_کوډ <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_کوډ > SBI_PMU_FW_PLATFORM)
		أرجع SBI_EINVAL;

	إذا (!(phs->fw_counters_started & بټ(cidx - عدد_hw_ctrs)))
		أرجع SBI_EALREADY_STOPPED;

	إذا (SBI_PMU_FW_PLATFORM == event_کوډ &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - عدد_hw_ctrs);
		إذا (ret)
			أرجع ret;
	}

	phs->fw_counters_started &= ~بټ(cidx - عدد_hw_ctrs);

	أرجع 0;
}

ساكن صحيح pmu_reset_hw_mhpmevent(صحيح ctr_idx)
{
	إذا (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		أرجع SBI_EFAIL;
#إذا __riscv_xlen == 32
	csr_write_عدد(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	إذا (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_عدد(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#وإلا
	csr_write_عدد(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#د_که_پای

	أرجع 0;
}

صحيح sbi_pmu_ctr_stop(بینښه اوږد cbase, بینښه اوږد cmask,
		     بینښه اوږد flag)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	إذا (unlikely(!phs))
		أرجع SBI_EINVAL;

	صحيح ret = SBI_EINVAL;
	صحيح event_idx_type;
	uint32_t event_کوډ;
	صحيح i, cidx;

	إذا (!pmu_ctr_idx_validate(cbase, cmask))
		أرجع ret;

	إذا (flag & ~SBI_PMU_STOP_FLAGS_ماسک)
		أرجع SBI_ERR_INVALID_PARAM;

	إذا (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		أرجع SBI_ENO_SHMEM;

	for_each_سېټ_بټ(i, &cmask, بټونه_فی_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_کوډ);
		إذا (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			استمر;

		وإلا إذا (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_کوډ);
		وإلا
			ret = pmu_ctr_stop_hw(cidx);

		إذا(ret)
			أرجع ret;

		إذا (cidx > (CSR_INSTRET - CSR_دوره) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->فعال_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	إذا (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_ماسک());

	أرجع ret;
}

ساكن صحيح pmu_fixed_ctr_update_inhibit_بټونه(صحيح fixed_ctr, بینښه اوږد flags)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#إذا __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#د_که_پای
	إذا (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_ډېرپېښ))
		أرجع fixed_ctr;

	بدل (fixed_ctr) {
	حالة 0:
		cfg_csr_no = CSR_MCYCLECFG;
#إذا __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#د_که_پای
		اكسر;
	حالة 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#إذا __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#د_که_پای
		اكسر;
	افتراضي:
		أرجع SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#إذا __riscv_xlen == 32
		csr_write_عدد(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_عدد(cfgh_csr_no, cfg_val >> بټونه_فی_LONG);
#وإلا
		csr_write_عدد(cfg_csr_no, cfg_val);
#د_که_پای
	}
	إذا (pmu_dev && pmu_dev->hw_counter_filter_ډېرپېښ)
		pmu_dev->hw_counter_filter_ډېرپېښ(flags, fixed_ctr);
	أرجع fixed_ctr;
}

ساكن صحيح pmu_ctr_پیدا_کړه_fixed_hw(بینښه اوږد evt_idx_کوډ)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	إذا (evt_idx_کوډ == SBI_PMU_HW_CPU_دورې)
		أرجع 0;
	وإلا إذا (evt_idx_کوډ == SBI_PMU_HW_INSTRUCTIONS)
		أرجع 2;
	وإلا
		أرجع SBI_EINVAL;
}

ساكن صحيح pmu_ctr_پیدا_کړه_hw(بنية sbi_pmu_hart_state *phs,
			   بینښه اوږد cbase, بینښه اوږد cmask,
			   بینښه اوږد flags,
			   بینښه اوږد event_idx, uint64_t معلومات)
{
	بینښه اوږد ctr_ماسک;
	صحيح i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	بنية sbi_pmu_hw_event *لنډمهاله;
	بینښه اوږد mctr_inhbt = 0;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	إذا (cbase >= عدد_hw_ctrs)
		أرجع SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_پیدا_کړه_fixed_hw(event_idx);
	إذا (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		أرجع pmu_fixed_ctr_update_inhibit_بټونه(fixed_ctr, flags);

	إذا (sbi_hart_priv_نسخه(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	لكل (i = 0; i < عدد_hw_events; i++) {
		لنډمهاله = &hw_event_map[i];
		إذا ((لنډمهاله->پیل_idx > event_idx && event_idx < لنډمهاله->پای_ټکی_idx) ||
		    (لنډمهاله->پیل_idx < event_idx && event_idx > لنډمهاله->پای_ټکی_idx))
			استمر;

		/* For raw events, event data is used as the select value */
		إذا (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_ماسک = لنډمهاله->select_ماسک;

			/* The non-event map bits of data should match the selector */
			إذا (لنډمهاله->select != (معلومات & select_ماسک))
				استمر;
		}
		/* Fixed counters should not be part of the search */
		ctr_ماسک = لنډمهاله->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_ماسک);
		for_each_سېټ_بټ_from(cbase, &ctr_ماسک, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			إذا (phs->فعال_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				استمر;
			/* If mcountinhibit is supported, the bit must be enabled */
			إذا ((sbi_hart_priv_نسخه(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				استمر;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			اكسر;
		}
	}

	إذا (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		إذا (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			أرجع SBI_EFAIL;

		أرجع pmu_fixed_ctr_update_inhibit_بټونه(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(لنډمهاله, ctr_idx, flags, event_idx, معلومات);

	إذا (!ret)
		ret = ctr_idx;

	أرجع ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
ساكن صحيح pmu_ctr_پیدا_کړه_fw(بنية sbi_pmu_hart_state *phs,
			   بینښه اوږد cbase, بینښه اوږد cmask,
			   uint32_t event_کوډ, uint64_t edata)
{
	صحيح i, cidx;

	for_each_سېټ_بټ(i, &cmask, بټونه_فی_LONG) {
		cidx = i + cbase;
		إذا (cidx < عدد_hw_ctrs || ټول_ctrs <= cidx)
			استمر;
		إذا (phs->فعال_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			استمر;
		إذا (SBI_PMU_FW_PLATFORM == event_کوډ &&
		    pmu_dev && pmu_dev->fw_counter_سمون_encoding) {
			إذا (!pmu_dev->fw_counter_سمون_encoding(phs->hartid,
							    cidx - عدد_hw_ctrs,
							    edata))
				استمر;
		}

		أرجع cidx;
	}

	أرجع SBI_ENOTSUPP;
}

صحيح sbi_pmu_ctr_cfg_سمون(بینښه اوږد cidx_بنسټ, بینښه اوږد cidx_ماسک,
			  بینښه اوږد flags, بینښه اوږد event_idx,
			  uint64_t event_معلومات)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	إذا (unlikely(!phs))
		أرجع SBI_EINVAL;

	صحيح ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_کوډ;

	إذا (!pmu_ctr_idx_validate(cidx_بنسټ, cidx_ماسک))
		أرجع SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_معلومات);
	إذا (event_type < 0)
		أرجع SBI_EINVAL;
	event_کوډ = get_cidx_کوډ(event_idx);

	إذا (flags & SBI_PMU_CFG_FLAG_SKIP_سمون) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		بینښه اوږد cidx_لومړی = cidx_بنسټ + sbi_ffs(cidx_ماسک);

		إذا (cidx_لومړی >= ټول_ctrs)
			أرجع SBI_EINVAL;

		إذا (phs->فعال_events[cidx_لومړی] == SBI_PMU_EVENT_IDX_INVALID)
			أرجع SBI_EINVAL;
		ctr_idx = cidx_لومړی;
		اذهب skip_سمون;
	}

	إذا (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_پیدا_کړه_fw(phs, cidx_بنسټ, cidx_ماسک,
					  event_کوډ, event_معلومات);
		إذا ((event_کوډ == SBI_PMU_FW_PLATFORM) && (ctr_idx >= عدد_hw_ctrs))
			phs->fw_counters_معلومات[ctr_idx - عدد_hw_ctrs] =
								event_معلومات;
	} وإلا {
		ctr_idx = pmu_ctr_پیدا_کړه_hw(phs, cidx_بنسټ, cidx_ماسک, flags,
					  event_idx, event_معلومات);
		إذا (ctr_idx >= 0) {
			بنية sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_معلومات = event_معلومات;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_ماسک;
		}
	}

	إذا (ctr_idx < 0)
		أرجع SBI_ENOTSUPP;

	phs->فعال_events[ctr_idx] = event_idx;
skip_سمون:
	إذا (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		إذا (flags & SBI_PMU_CFG_FLAG_CLEAR_ارزښت)
			pmu_ctr_write_hw(ctr_idx, 0);
		إذا (flags & SBI_PMU_CFG_FLAG_AUTO_پیل)
			pmu_ctr_پیل_hw(ctr_idx, 0, false);
	} وإلا إذا (event_type == SBI_PMU_EVENT_TYPE_FW) {
		إذا (flags & SBI_PMU_CFG_FLAG_CLEAR_ارزښت)
			phs->fw_counters_معلومات[ctr_idx - عدد_hw_ctrs] = 0;
		إذا (flags & SBI_PMU_CFG_FLAG_AUTO_پیل) {
			إذا (SBI_PMU_FW_PLATFORM == event_کوډ &&
			    pmu_dev && pmu_dev->fw_counter_پیل) {
				ret = pmu_dev->fw_counter_پیل(
					phs->hartid,
					ctr_idx - عدد_hw_ctrs, event_معلومات);
				إذا (ret)
					أرجع ret;
			}
			phs->fw_counters_started |= بټ(ctr_idx - عدد_hw_ctrs);
		}
	}

	أرجع ctr_idx;
}

صحيح sbi_pmu_ctr_incr_fw(تعداد sbi_pmu_fw_event_کوډ_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = عدم;
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	إذا (unlikely(!phs))
		أرجع 0;

	إذا (likely(!phs->fw_counters_started))
		أرجع 0;

	إذا (unlikely(fw_id >= SBI_PMU_FW_MAX))
		أرجع SBI_EINVAL;

	لكل (cidx = عدد_hw_ctrs; cidx < ټول_ctrs; cidx++) {
		إذا (get_cidx_کوډ(phs->فعال_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & بټ(cidx - عدد_hw_ctrs))) {
			fcounter = &phs->fw_counters_معلومات[cidx - عدد_hw_ctrs];
			اكسر;
		}
	}

	إذا (fcounter)
		(*fcounter)++;

	أرجع 0;
}

بینښه اوږد sbi_pmu_عدد_ctr(فراغ)
{
	أرجع (عدد_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

صحيح sbi_pmu_ctr_get_info(uint32_t cidx, بینښه اوږد *ctr_info)
{
	صحيح پلنوالی;
	اتحاد sbi_pmu_ctr_info cinfo = {0};
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بینښه اوږد counter_ماسک = (بینښه اوږد)sbi_hart_mhpm_ماسک(scratch) |
				     SBI_PMU_CY_IR_ماسک;

	/* Sanity check */
	إذا (cidx >= ټول_ctrs)
		أرجع SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	إذا (cidx < عدد_hw_ctrs) {
		إذا (!(__test_bit(cidx, &counter_ماسک)))
			أرجع SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_دوره + cidx;
		/* mcycle & minstret are always 64 bit */
		إذا (cidx == 0 || cidx == 2)
			cinfo.پلنوالی = 63;
		وإلا
			cinfo.پلنوالی = sbi_hart_mhpm_بټونه(scratch) - 1;
	} وإلا {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.پلنوالی = 63;
		إذا (pmu_dev && pmu_dev->fw_counter_پلنوالی) {
			پلنوالی = pmu_dev->fw_counter_پلنوالی();
			إذا (پلنوالی)
				cinfo.پلنوالی = پلنوالی - 1;
		}
	}

	*ctr_info = cinfo.ارزښت;

	أرجع 0;
}

صحيح sbi_pmu_event_get_info(بینښه اوږد shmem_phys_lo, بینښه اوږد shmem_phys_hi,
			   بینښه اوږد عدد_events, بینښه اوږد flags)
{
	بینښه اوږد shmem_اندازه = عدد_events * حجم(بنية sbi_pmu_event_info);
	صحيح i, j, event_type;
	بنية sbi_pmu_event_info *einfo;
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	بنية sbi_pmu_hw_event *لنډمهاله;
	bool وموندل = false;

	إذا (flags != 0)
		أرجع SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	إذا (!عدد_events || (shmem_phys_lo & 0xF))
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
	إذا (shmem_phys_hi)
		أرجع SBI_EINVALID_ADDR;

	إذا (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_اندازه, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		أرجع SBI_ERR_INVALID_پته;

	sbi_hart_protection_لنډمهاله_map_range(shmem_phys_lo, shmem_اندازه);

	einfo = (بنية sbi_pmu_event_info *)(shmem_phys_lo);
	لكل (i = 0; i < عدد_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		إذا (event_idx & SBI_PMU_EVENT_IDX_MBZ_ماسک)
			أرجع SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_معلومات);
		إذا (event_type < 0) {
			einfo[i].output = 0;
		} وإلا إذا (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} وإلا {
			لكل (j = 0; j < عدد_hw_events; j++) {
				لنډمهاله = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				إذا (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					إذا (لنډمهاله->پیل_idx > event_idx ||
					    event_idx > لنډمهاله->پای_ټکی_idx)
						استمر;
					/* just match the selector */
					إذا (لنډمهاله->select == (einfo[i].event_معلومات &
									لنډمهاله->select_ماسک)) {
						وموندل = true;
						اكسر;
					}
				} وإلا إذا (لنډمهاله->پیل_idx <= event_idx &&
					   event_idx <= لنډمهاله->پای_ټکی_idx) {
					وموندل = true;
					اكسر;
				}
			}
			إذا (وموندل)
				einfo[i].output = 1;
			وإلا
				einfo[i].output = 0;
			وموندل = false;
		}
	}

	sbi_hart_protection_لنډمهاله_unmap_range(shmem_phys_lo, shmem_اندازه);

	أرجع 0;
}

ساكن فراغ pmu_reset_event_map(بنية sbi_pmu_hart_state *phs)
{
	صحيح j;

	/* Initialize the counter to event mapping table */
	لكل (j = 3; j < ټول_ctrs; j++)
		phs->فعال_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	لكل (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_معلومات[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

ثابت بنية sbi_pmu_device *sbi_pmu_get_device(فراغ)
{
	أرجع pmu_dev;
}

فراغ sbi_pmu_سېټ_device(ثابت بنية sbi_pmu_device *dev)
{
	إذا (!dev || pmu_dev)
		أرجع;

	pmu_dev = dev;
}

فراغ sbi_pmu_exit(بنية sbi_scratch *scratch)
{
	بنية sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	إذا (sbi_hart_priv_نسخه(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	إذا (sbi_hart_priv_نسخه(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	إذا (unlikely(!phs))
		أرجع;

	pmu_reset_event_map(phs);
}

ساكن فراغ pmu_sse_enable(uint32_t event_id)
{
	بینښه اوږد irq_ماسک = sbi_pmu_irq_ماسک();

	csr_سېټ(CSR_MIE, irq_ماسک);
}

ساكن فراغ pmu_sse_disable(uint32_t event_id)
{
	بینښه اوږد irq_ماسک = sbi_pmu_irq_ماسک();

	csr_clear(CSR_MIE, irq_ماسک);
	csr_clear(CSR_MIP, irq_ماسک);
}

ساكن فراغ pmu_sse_complete(uint32_t event_id)
{
	csr_سېټ(CSR_MIE, sbi_pmu_irq_ماسک());
}

ساكن فراغ pmu_sse_ثبتوونکی(uint32_t event_id)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_ماسک());
}

ساكن فراغ pmu_sse_unregister(uint32_t event_id)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_سېټ(CSR_MIDELEG, sbi_pmu_irq_ماسک());
}

ساكن ثابت بنية sbi_sse_cb_ops pmu_sse_cb_ops = {
	.ثبتوونکی_cb = pmu_sse_ثبتوونکی,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

صحيح sbi_pmu_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	صحيح hpm_شمېر = sbi_fls(sbi_hart_mhpm_ماسک(scratch));
	بنية sbi_pmu_hart_state *phs;
	ثابت بنية sbi_platform *plat;
	صحيح rc;

	إذا (cold_boot) {
		hw_event_map = sbi_calloc(حجم(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		إذا (!hw_event_map)
			أرجع SBI_ENOMEM;

		phs_ptr_دننه_واټن = sbi_scratch_alloc_type_دننه_واټن(فراغ *);
		إذا (!phs_ptr_دننه_واټن) {
			sbi_free(hw_event_map);
			أرجع SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		إذا (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		إذا (!hpm_شمېر)
			/* Only CY, TM & IR are implemented in the hw */
			عدد_hw_ctrs = 3;
		وإلا
			عدد_hw_ctrs = hpm_شمېر + 1;

		إذا (عدد_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			أرجع SBI_EINVAL;

		ټول_ctrs = عدد_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		إذا (sbi_pmu_irq_بټ() >= 0)
			sbi_sse_جمع_کړه_event(SBI_SSE_EVENT_LOCAL_PMU_ډکېدنه, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	إذا (!phs) {
		phs = sbi_zalloc(حجم(*phs));
		إذا (!phs)
			أرجع SBI_ENOMEM;
		phs->hartid = current_hartid();
		pmu_سېټ_hart_state_ptr(scratch, phs);
	}

	pmu_reset_event_map(phs);

	/* First three counters are fixed by the priv spec and we enable it by default */
	phs->فعال_events[0] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_دننه_واټن) |
				SBI_PMU_HW_CPU_دورې;
	phs->فعال_events[1] = SBI_PMU_EVENT_IDX_INVALID;
	phs->فعال_events[2] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_دننه_واټن) |
				SBI_PMU_HW_INSTRUCTIONS;

	أرجع 0;
}
