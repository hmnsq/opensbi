/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_hart_protection.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_pmu.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_sse.h>

/** Information about hardware counters */
నిర్మాణం sbi_pmu_hw_event {
	uint32_t counters;
	uint32_t ప్రారంభం_idx;
	uint32_t చివర_idx;
	/* Event selector value used only for raw events. The event select value
	 * can be a even id or a selector value for set of events encoded in few
	 * bits. In case latter, the bits used for encoding of the events should
	 * be zeroed out in the select value.
	 */
	uint64_t select;
	 /**
	  * The select_mask indicates which bits are encoded for the event(s).
	  */
	uint64_t select_ముసుగు;
};

/* Information about PMU counters as per SBI specification */
సంఘం sbi_pmu_ctr_info {
	గుర్తులేని దీర్ఘ విలువ;
	నిర్మాణం {
		గుర్తులేని దీర్ఘ csr:12;
		గుర్తులేని దీర్ఘ వెడల్పు:6;
#ఒకవేళ __riscv_xlen == 32
		గుర్తులేని దీర్ఘ reserved:13;
#లేకపోతే
		గుర్తులేని దీర్ఘ reserved:45;
#అయితే_ముగింపు
		గుర్తులేని దీర్ఘ type:1;
	};
};

#ఒకవేళ SBI_PMU_FW_CTR_MAX >= బిట్లు_చొప్పున_LONG
#దోషం "Can't handle firmware counters beyond BITS_PER_LONG"
#అయితే_ముగింపు

/** HW event configuration parameters */
నిర్మాణం sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_దత్తాంశం;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
నిర్మాణం sbi_pmu_hart_state {
	/* HART to which this state belongs */
	uint32_t hartid;
	/* Counter to enabled event mapping */
	uint32_t చురుకైన_events[SBI_PMU_HW_CTR_MAX + SBI_PMU_FW_CTR_MAX];
	/* Bitmap of firmware counters started */
	గుర్తులేని దీర్ఘ fw_counters_started;
	/* if true, SSE is enabled */
	bool sse_enabled;
	/*
	 * Counter values for SBI firmware events and event codes
	 * for platform firmware events. Both are mutually exclusive
	 * and hence can optimally share the same memory.
	 */
	uint64_t fw_counters_దత్తాంశం[SBI_PMU_FW_CTR_MAX];
	/* HW events configuration parameters from
	 * sbi_pmu_ctr_cfg_match() command which are
	 * used for restoring RAW hardware events after
	 * cpu suspending.
	 */
	నిర్మాణం sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
స్థిర గుర్తులేని దీర్ఘ phs_ptr_స్థానభ్రంశం;

#నిర్వచించు pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_స్థానభ్రంశం ? sbi_scratch_read_type((__scratch), శూన్య *, phs_ptr_స్థానభ్రంశం) : ఖాళీ

#నిర్వచించు pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#నిర్వచించు pmu_సమితి_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), శూన్య *, phs_ptr_స్థానభ్రంశం, (__phs))

/* Platform specific PMU device */
స్థిర స్థిరరాశి నిర్మాణం sbi_pmu_device *pmu_dev = ఖాళీ;

/* Mapping between event range and possible counters  */
స్థిర నిర్మాణం sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
స్థిర uint32_t అంకె_hw_events;
/* Maximum number of hardware counters available */
స్థిర uint32_t అంకె_hw_ctrs;

/* Maximum number of counters available */
స్థిర uint32_t మొత్తం_ctrs;

/* Helper macros to retrieve event idx and code type */
#నిర్వచించు get_cidx_type(x) \
  (((x) & SBI_PMU_EVENT_IDX_TYPE_ముసుగు) >> SBI_PMU_EVENT_IDX_TYPE_స్థానభ్రంశం)
#నిర్వచించు get_cidx_సంకేతం(x) (x & SBI_PMU_EVENT_IDX_సంకేతం_ముసుగు)

/**
 * Perform a sanity check on event & counter mappings with event range overlap check
 * @param evtA Pointer to the existing hw event structure
 * @param evtB Pointer to the new hw event structure
 *
 * Return false if the range doesn't overlap, true otherwise
 */
స్థిర bool pmu_event_range_overlap(నిర్మాణం sbi_pmu_hw_event *evtA,
				    నిర్మాణం sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	ఒకవేళ (((evtA->చివర_idx < evtB->ప్రారంభం_idx) && (evtA->చివర_idx < evtB->చివర_idx)) ||
	   ((evtA->ప్రారంభం_idx > evtB->ప్రారంభం_idx) && (evtA->ప్రారంభం_idx > evtB->చివర_idx)))
		తిరిగి false;
	తిరిగి true;
}

స్థిర bool pmu_event_select_overlap(నిర్మాణం sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_ముసుగు)
{
	ఒకవేళ ((evt->select == select_val) && (evt->select_ముసుగు == select_ముసుగు))
		తిరిగి true;

	తిరిగి false;
}

స్థిర పూర్ణాంకం pmu_event_validate(నిర్మాణం sbi_pmu_hart_state *phs,
			      గుర్తులేని దీర్ఘ event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_సంకేతం = get_cidx_సంకేతం(event_idx);
	uint32_t event_idx_సంకేతం_max = -1;
	uint32_t cache_ops_ఫలితం, cache_ops_id, cache_id;

	ఎంపిక(event_idx_type) {
	స్థితి SBI_PMU_EVENT_TYPE_HW:
		event_idx_సంకేతం_max = SBI_PMU_HW_GENERAL_MAX;
		ఆపు;
	స్థితి SBI_PMU_EVENT_TYPE_FW:
		ఒకవేళ ((event_idx_సంకేతం >= SBI_PMU_FW_MAX &&
		    event_idx_సంకేతం <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_సంకేతం > SBI_PMU_FW_PLATFORM)
			తిరిగి SBI_EINVAL;

		ఒకవేళ (SBI_PMU_FW_PLATFORM == event_idx_సంకేతం &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			తిరిగి pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		లేకపోతే
			event_idx_సంకేతం_max = SBI_PMU_FW_MAX;
		ఆపు;
	స్థితి SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_ఫలితం = event_idx_సంకేతం &
					SBI_PMU_EVENT_HW_CACHE_OPS_ఫలితం;
		cache_ops_id = (event_idx_సంకేతం &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_ముసుగు) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_స్థానభ్రంశం;
		cache_id = (event_idx_సంకేతం &
			    SBI_PMU_EVENT_HW_CACHE_ID_ముసుగు) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_స్థానభ్రంశం;
		ఒకవేళ ((cache_ops_ఫలితం < SBI_PMU_HW_CACHE_ఫలితం_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			తిరిగి event_idx_type;
		లేకపోతే
			తిరిగి SBI_EINVAL;
		ఆపు;
	స్థితి SBI_PMU_EVENT_TYPE_HW_RAW:
	స్థితి SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_సంకేతం_max = 1; // event_idx.code should be zero
		ఆపు;
	అప్రమేయ:
		తిరిగి SBI_EINVAL;
	}

	ఒకవేళ (event_idx_సంకేతం < event_idx_సంకేతం_max)
		తిరిగి event_idx_type;

	తిరిగి SBI_EINVAL;
}

స్థిర పూర్ణాంకం pmu_ctr_validate(నిర్మాణం sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_సంకేతం)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	ఒకవేళ (cidx >= మొత్తం_ctrs)
		తిరిగి SBI_EINVAL;

	event_idx_val = phs->చురుకైన_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	ఒకవేళ (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		తిరిగి SBI_EINVAL;

	*event_idx_సంకేతం = get_cidx_సంకేతం(event_idx_val);

	తిరిగి event_idx_type;
}

స్థిర bool pmu_ctr_idx_validate(గుర్తులేని దీర్ఘ cbase, గుర్తులేని దీర్ఘ cmask)
{
	గుర్తులేని దీర్ఘ చివరి;

	ఒకవేళ (!cmask)
		తిరిగి false;

	చివరి = sbi_fls(cmask);
	ఒకవేళ (cbase > -1UL - చివరి)
		తిరిగి false;

	తిరిగి (cbase + చివరి) < మొత్తం_ctrs;
}

పూర్ణాంకం sbi_pmu_ctr_fw_read(గుర్తులేని దీర్ఘ cidx, uint64_t *cval, bool ఎగువ_బిట్లు)
{
	పూర్ణాంకం event_idx_type;
	uint32_t event_సంకేతం;
	నిర్మాణం sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ఒకవేళ (unlikely(!phs))
		తిరిగి SBI_EINVAL;

	ఒకవేళ (cidx < అంకె_hw_ctrs || cidx >= మొత్తం_ctrs)
		తిరిగి SBI_EINVAL;

#ఒకవేళ __riscv_xlen > 32
	ఒకవేళ (ఎగువ_బిట్లు)
		తిరిగి 0;
#అయితే_ముగింపు

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_సంకేతం);
	ఒకవేళ (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		తిరిగి SBI_EINVAL;

	ఒకవేళ ((event_సంకేతం >= SBI_PMU_FW_MAX &&
	    event_సంకేతం <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_సంకేతం > SBI_PMU_FW_PLATFORM)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (SBI_PMU_FW_PLATFORM == event_సంకేతం) {
		ఒకవేళ (pmu_dev && pmu_dev->fw_counter_read_విలువ)
			*cval = pmu_dev->fw_counter_read_విలువ(phs->hartid,
							       cidx -
							       అంకె_hw_ctrs);
		లేకపోతే
			*cval = 0;
	} లేకపోతే
		*cval = phs->fw_counters_దత్తాంశం[cidx - అంకె_hw_ctrs];

	తిరిగి 0;
}

స్థిర పూర్ణాంకం pmu_కూడు_hw_event_map(u32 eidx_ప్రారంభం, u32 eidx_చివర, u32 cmap,
				uint64_t select, uint64_t select_ముసుగు)
{
	పూర్ణాంకం i = 0;
	bool is_overlap;
	నిర్మాణం sbi_pmu_hw_event *event = &hw_event_map[అంకె_hw_events];
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_ముసుగు = sbi_hart_mhpm_ముసుగు(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	ఒకవేళ (eidx_ప్రారంభం > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_ముసుగు))
		తిరిగి SBI_EDENIED;

	ఒకవేళ (అంకె_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		తిరిగి SBI_EFAIL;
	}

	event->ప్రారంభం_idx = eidx_ప్రారంభం;
	event->చివర_idx = eidx_చివర;

	/* Sanity check */
	కొరకు (i = 0; i < అంకె_hw_events; i++) {
		ఒకవేళ (eidx_ప్రారంభం == SBI_PMU_EVENT_RAW_IDX ||
			eidx_ప్రారంభం == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_ముసుగు);
		లేకపోతే
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		ఒకవేళ (is_overlap)
			వెళ్ళు reset_event;
	}

	event->select_ముసుగు = select_ముసుగు;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_ముసుగు;
	event->select = select;
	అంకె_hw_events++;

	తిరిగి 0;

reset_event:
	event->ప్రారంభం_idx = 0;
	event->చివర_idx = 0;
	తిరిగి SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
పూర్ణాంకం sbi_pmu_కూడు_hw_event_counter_map(u32 eidx_ప్రారంభం, u32 eidx_చివర, u32 cmap)
{
	ఒకవేళ ((eidx_ప్రారంభం > eidx_చివర) || eidx_ప్రారంభం >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_చివర >= SBI_PMU_EVENT_RAW_IDX)
		తిరిగి SBI_EINVAL;

	తిరిగి pmu_కూడు_hw_event_map(eidx_ప్రారంభం, eidx_చివర, cmap, 0, 0);
}

పూర్ణాంకం sbi_pmu_కూడు_raw_event_counter_map(uint64_t select, uint64_t select_ముసుగు, u32 cmap)
{
	తిరిగి pmu_కూడు_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_ముసుగు);
}

శూన్య sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_ముసుగు());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_అధికప్రవాహం);
}

స్థిర పూర్ణాంకం pmu_ctr_enable_irq_hw(పూర్ణాంకం ctr_idx)
{
	గుర్తులేని దీర్ఘ mhpmevent_csr;
	గుర్తులేని దీర్ఘ mhpmevent_curr;
	గుర్తులేని దీర్ఘ mip_val;
	గుర్తులేని దీర్ఘ యొక్క_ముసుగు;

	ఒకవేళ (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		తిరిగి SBI_EFAIL;

#ఒకవేళ __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	యొక్క_ముసుగు = (uint32_t)~MHPMEVENTH_యొక్క;
#లేకపోతే
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	యొక్క_ముసుగు = ~MHPMEVENT_యొక్క;
#అయితే_ముగింపు

	mhpmevent_curr = csr_read_అంకె(mhpmevent_csr);
	mip_val = csr_read(CSR_MIP);
	/**
	 * Clear out the OF bit so that next interrupt can be enabled.
	 * This should be done only when the corresponding overflow interrupt
	 * bit is cleared. That indicates that software has already handled the
	 * previous interrupts or the hardware yet to set an overflow interrupt.
	 * Otherwise, there will be race conditions where we may clear the bit
	 * the software is yet to handle the interrupt.
	 */
	ఒకవేళ (!(mip_val & sbi_pmu_irq_ముసుగు())) {
		mhpmevent_curr &= యొక్క_ముసుగు;
		csr_write_అంకె(mhpmevent_csr, mhpmevent_curr);
	}

	తిరిగి 0;
}

స్థిర శూన్య pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#ఒకవేళ __riscv_xlen == 32
	csr_write_అంకె(CSR_MCYCLE + cidx, 0);
	csr_write_అంకె(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_అంకె(CSR_MCYCLEH + cidx, ival >> బిట్లు_చొప్పున_LONG);
#లేకపోతే
	csr_write_అంకె(CSR_MCYCLE + cidx, ival);
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం pmu_ctr_ప్రారంభం_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	గుర్తులేని దీర్ఘ mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	ఒకవేళ (cidx >= అంకె_hw_ctrs || cidx == 1)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) < SBI_HART_PRIV_VER_1_11) {
		ఒకవేళ (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		తిరిగి 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	ఒకవేళ (!__test_bit(cidx, &mctr_inhbt))
		తిరిగి SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	ఒకవేళ (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	ఒకవేళ (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	తిరిగి 0;
}

పూర్ణాంకం sbi_pmu_irq_బిట్టు(శూన్య)
{
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		తిరిగి IRQ_PMU_OVF;
	ఒకవేళ (pmu_dev && pmu_dev->hw_counter_irq_బిట్టు)
		తిరిగి pmu_dev->hw_counter_irq_బిట్టు();

	తిరిగి -1;
}

గుర్తులేని దీర్ఘ sbi_pmu_irq_ముసుగు(శూన్య)
{
	పూర్ణాంకం irq_బిట్టు = sbi_pmu_irq_బిట్టు();

	ఒకవేళ (irq_బిట్టు < 0)
		తిరిగి 0;

	తిరిగి బిట్టు(irq_బిట్టు);
}

స్థిర పూర్ణాంకం pmu_ctr_ప్రారంభం_fw(నిర్మాణం sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_సంకేతం,
			    uint64_t event_దత్తాంశం, uint64_t ival,
			    bool ival_update)
{
	పూర్ణాంకం ret;

	ఒకవేళ ((event_సంకేతం >= SBI_PMU_FW_MAX &&
	    event_సంకేతం <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_సంకేతం > SBI_PMU_FW_PLATFORM)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (phs->fw_counters_started & బిట్టు(cidx - అంకె_hw_ctrs))
		తిరిగి SBI_EALREADY_STARTED;

	ఒకవేళ (SBI_PMU_FW_PLATFORM == event_సంకేతం) {
		ఒకవేళ (!pmu_dev ||
		    !pmu_dev->fw_counter_write_విలువ ||
		    !pmu_dev->fw_counter_ప్రారంభం) {
			తిరిగి SBI_EINVAL;
		}

		ఒకవేళ (ival_update)
			pmu_dev->fw_counter_write_విలువ(phs->hartid,
							cidx - అంకె_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_ప్రారంభం(phs->hartid,
						 cidx - అంకె_hw_ctrs,
						 event_దత్తాంశం);
		ఒకవేళ (ret)
			తిరిగి ret;
	} లేకపోతే {
		ఒకవేళ (ival_update)
			phs->fw_counters_దత్తాంశం[cidx - అంకె_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= బిట్టు(cidx - అంకె_hw_ctrs);

	తిరిగి 0;
}

స్థిర శూన్య pmu_update_inhibit_flags(గుర్తులేని దీర్ఘ flags, uint64_t *mhpmevent_val)
{
	ఒకవేళ (flags & SBI_PMU_CFG_FLAG_సమితి_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	ఒకవేళ (flags & SBI_PMU_CFG_FLAG_సమితి_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	ఒకవేళ (flags & SBI_PMU_CFG_FLAG_సమితి_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	ఒకవేళ (flags & SBI_PMU_CFG_FLAG_సమితి_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

స్థిర పూర్ణాంకం pmu_update_hw_mhpmevent(నిర్మాణం sbi_pmu_hw_event *hw_evt, పూర్ణాంకం ctr_idx,
				   గుర్తులేని దీర్ఘ flags, గుర్తులేని దీర్ఘ eindex,
				   uint64_t దత్తాంశం)
{
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_గమ్యం_mhpmevent(plat, eindex, దత్తాంశం);

	ఒకవేళ (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		తిరిగి SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_ముసుగు) |
				 MHPMEVENT_MINH | MHPMEVENT_యొక్క;

	ఒకవేళ (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	ఒకవేళ (pmu_dev && pmu_dev->hw_counter_filter_బహుళకం)
		pmu_dev->hw_counter_filter_బహుళకం(flags, ctr_idx);

#ఒకవేళ __riscv_xlen == 32
	csr_write_అంకె(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_అంకె(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> బిట్లు_చొప్పున_LONG);
#లేకపోతే
	csr_write_అంకె(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#అయితే_ముగింపు

	తిరిగి 0;
}

పూర్ణాంకం sbi_pmu_ctr_ప్రారంభం(గుర్తులేని దీర్ఘ cbase, గుర్తులేని దీర్ఘ cmask,
		      గుర్తులేని దీర్ఘ flags, uint64_t ival)
{
	నిర్మాణం sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ఒకవేళ (unlikely(!phs))
		తిరిగి SBI_EINVAL;

	పూర్ణాంకం event_idx_type;
	uint32_t event_సంకేతం;
	పూర్ణాంకం ret = SBI_EINVAL;
	bool bUpdate = false;
	పూర్ణాంకం i, cidx;
	uint64_t edata;

	ఒకవేళ (!pmu_ctr_idx_validate(cbase, cmask))
		తిరిగి ret;

	ఒకవేళ (flags & ~SBI_PMU_ప్రారంభం_FLAGS_ముసుగు)
		తిరిగి SBI_ERR_INVALID_PARAM;

	ఒకవేళ (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		తిరిగి SBI_ENO_SHMEM;

	ఒకవేళ (flags & SBI_PMU_ప్రారంభం_FLAG_సమితి_INIT_విలువ)
		bUpdate = true;

	for_each_సమితి_బిట్టు(i, &cmask, బిట్లు_చొప్పున_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_సంకేతం);
		ఒకవేళ (event_idx_type < 0)
			/* Continue the start operation for other counters */
			కొనసాగు;
		లేకపోతే ఒకవేళ (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_సంకేతం == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_దత్తాంశం[cidx - అంకె_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_ప్రారంభం_fw(phs, cidx, event_సంకేతం, edata,
					       ival, bUpdate);
			ఒకవేళ (ret)
				తిరిగి ret;
		} లేకపోతే {
			ఒకవేళ (cidx >= 3) {
				నిర్మాణం sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->చురుకైన_events[cidx],
							ev_cfg->event_దత్తాంశం);
				ఒకవేళ (ret)
					తిరిగి ret;
			}
			ret = pmu_ctr_ప్రారంభం_hw(cidx, ival, bUpdate);
			ఒకవేళ (ret)
				తిరిగి ret;
		}
	}

	తిరిగి ret;
}

స్థిర పూర్ణాంకం pmu_ctr_stop_hw(uint32_t cidx)
{
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	గుర్తులేని దీర్ఘ mctr_inhbt;

	ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) < SBI_HART_PRIV_VER_1_11)
		తిరిగి 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	ఒకవేళ (cidx >= అంకె_hw_ctrs || cidx == 1)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		ఒకవేళ (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		తిరిగి 0;
	} లేకపోతే
		తిరిగి SBI_EALREADY_STOPPED;
}

స్థిర పూర్ణాంకం pmu_ctr_stop_fw(నిర్మాణం sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_సంకేతం)
{
	పూర్ణాంకం ret;

	ఒకవేళ ((event_సంకేతం >= SBI_PMU_FW_MAX &&
	    event_సంకేతం <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_సంకేతం > SBI_PMU_FW_PLATFORM)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (!(phs->fw_counters_started & బిట్టు(cidx - అంకె_hw_ctrs)))
		తిరిగి SBI_EALREADY_STOPPED;

	ఒకవేళ (SBI_PMU_FW_PLATFORM == event_సంకేతం &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - అంకె_hw_ctrs);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	phs->fw_counters_started &= ~బిట్టు(cidx - అంకె_hw_ctrs);

	తిరిగి 0;
}

స్థిర పూర్ణాంకం pmu_reset_hw_mhpmevent(పూర్ణాంకం ctr_idx)
{
	ఒకవేళ (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		తిరిగి SBI_EFAIL;
#ఒకవేళ __riscv_xlen == 32
	csr_write_అంకె(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	ఒకవేళ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_అంకె(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#లేకపోతే
	csr_write_అంకె(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#అయితే_ముగింపు

	తిరిగి 0;
}

పూర్ణాంకం sbi_pmu_ctr_stop(గుర్తులేని దీర్ఘ cbase, గుర్తులేని దీర్ఘ cmask,
		     గుర్తులేని దీర్ఘ flag)
{
	నిర్మాణం sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ఒకవేళ (unlikely(!phs))
		తిరిగి SBI_EINVAL;

	పూర్ణాంకం ret = SBI_EINVAL;
	పూర్ణాంకం event_idx_type;
	uint32_t event_సంకేతం;
	పూర్ణాంకం i, cidx;

	ఒకవేళ (!pmu_ctr_idx_validate(cbase, cmask))
		తిరిగి ret;

	ఒకవేళ (flag & ~SBI_PMU_STOP_FLAGS_ముసుగు)
		తిరిగి SBI_ERR_INVALID_PARAM;

	ఒకవేళ (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		తిరిగి SBI_ENO_SHMEM;

	for_each_సమితి_బిట్టు(i, &cmask, బిట్లు_చొప్పున_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_సంకేతం);
		ఒకవేళ (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			కొనసాగు;

		లేకపోతే ఒకవేళ (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_సంకేతం);
		లేకపోతే
			ret = pmu_ctr_stop_hw(cidx);

		ఒకవేళ(ret)
			తిరిగి ret;

		ఒకవేళ (cidx > (CSR_INSTRET - CSR_చక్రం) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->చురుకైన_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	ఒకవేళ (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_ముసుగు());

	తిరిగి ret;
}

స్థిర పూర్ణాంకం pmu_fixed_ctr_update_inhibit_బిట్లు(పూర్ణాంకం fixed_ctr, గుర్తులేని దీర్ఘ flags)
{
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#ఒకవేళ __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#అయితే_ముగింపు
	ఒకవేళ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_బహుళకం))
		తిరిగి fixed_ctr;

	ఎంపిక (fixed_ctr) {
	స్థితి 0:
		cfg_csr_no = CSR_MCYCLECFG;
#ఒకవేళ __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#అయితే_ముగింపు
		ఆపు;
	స్థితి 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#ఒకవేళ __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#అయితే_ముగింపు
		ఆపు;
	అప్రమేయ:
		తిరిగి SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#ఒకవేళ __riscv_xlen == 32
		csr_write_అంకె(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_అంకె(cfgh_csr_no, cfg_val >> బిట్లు_చొప్పున_LONG);
#లేకపోతే
		csr_write_అంకె(cfg_csr_no, cfg_val);
#అయితే_ముగింపు
	}
	ఒకవేళ (pmu_dev && pmu_dev->hw_counter_filter_బహుళకం)
		pmu_dev->hw_counter_filter_బహుళకం(flags, fixed_ctr);
	తిరిగి fixed_ctr;
}

స్థిర పూర్ణాంకం pmu_ctr_కనుగొను_fixed_hw(గుర్తులేని దీర్ఘ evt_idx_సంకేతం)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	ఒకవేళ (evt_idx_సంకేతం == SBI_PMU_HW_CPU_చక్రాలు)
		తిరిగి 0;
	లేకపోతే ఒకవేళ (evt_idx_సంకేతం == SBI_PMU_HW_INSTRUCTIONS)
		తిరిగి 2;
	లేకపోతే
		తిరిగి SBI_EINVAL;
}

స్థిర పూర్ణాంకం pmu_ctr_కనుగొను_hw(నిర్మాణం sbi_pmu_hart_state *phs,
			   గుర్తులేని దీర్ఘ cbase, గుర్తులేని దీర్ఘ cmask,
			   గుర్తులేని దీర్ఘ flags,
			   గుర్తులేని దీర్ఘ event_idx, uint64_t దత్తాంశం)
{
	గుర్తులేని దీర్ఘ ctr_ముసుగు;
	పూర్ణాంకం i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	నిర్మాణం sbi_pmu_hw_event *తాత్కాలిక;
	గుర్తులేని దీర్ఘ mctr_inhbt = 0;
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ఒకవేళ (cbase >= అంకె_hw_ctrs)
		తిరిగి SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_కనుగొను_fixed_hw(event_idx);
	ఒకవేళ (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		తిరిగి pmu_fixed_ctr_update_inhibit_బిట్లు(fixed_ctr, flags);

	ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	కొరకు (i = 0; i < అంకె_hw_events; i++) {
		తాత్కాలిక = &hw_event_map[i];
		ఒకవేళ ((తాత్కాలిక->ప్రారంభం_idx > event_idx && event_idx < తాత్కాలిక->చివర_idx) ||
		    (తాత్కాలిక->ప్రారంభం_idx < event_idx && event_idx > తాత్కాలిక->చివర_idx))
			కొనసాగు;

		/* For raw events, event data is used as the select value */
		ఒకవేళ (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_ముసుగు = తాత్కాలిక->select_ముసుగు;

			/* The non-event map bits of data should match the selector */
			ఒకవేళ (తాత్కాలిక->select != (దత్తాంశం & select_ముసుగు))
				కొనసాగు;
		}
		/* Fixed counters should not be part of the search */
		ctr_ముసుగు = తాత్కాలిక->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_ముసుగు);
		for_each_సమితి_బిట్టు_from(cbase, &ctr_ముసుగు, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			ఒకవేళ (phs->చురుకైన_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				కొనసాగు;
			/* If mcountinhibit is supported, the bit must be enabled */
			ఒకవేళ ((sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				కొనసాగు;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			ఆపు;
		}
	}

	ఒకవేళ (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		ఒకవేళ (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			తిరిగి SBI_EFAIL;

		తిరిగి pmu_fixed_ctr_update_inhibit_బిట్లు(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(తాత్కాలిక, ctr_idx, flags, event_idx, దత్తాంశం);

	ఒకవేళ (!ret)
		ret = ctr_idx;

	తిరిగి ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
స్థిర పూర్ణాంకం pmu_ctr_కనుగొను_fw(నిర్మాణం sbi_pmu_hart_state *phs,
			   గుర్తులేని దీర్ఘ cbase, గుర్తులేని దీర్ఘ cmask,
			   uint32_t event_సంకేతం, uint64_t edata)
{
	పూర్ణాంకం i, cidx;

	for_each_సమితి_బిట్టు(i, &cmask, బిట్లు_చొప్పున_LONG) {
		cidx = i + cbase;
		ఒకవేళ (cidx < అంకె_hw_ctrs || మొత్తం_ctrs <= cidx)
			కొనసాగు;
		ఒకవేళ (phs->చురుకైన_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			కొనసాగు;
		ఒకవేళ (SBI_PMU_FW_PLATFORM == event_సంకేతం &&
		    pmu_dev && pmu_dev->fw_counter_సరిపోలిక_encoding) {
			ఒకవేళ (!pmu_dev->fw_counter_సరిపోలిక_encoding(phs->hartid,
							    cidx - అంకె_hw_ctrs,
							    edata))
				కొనసాగు;
		}

		తిరిగి cidx;
	}

	తిరిగి SBI_ENOTSUPP;
}

పూర్ణాంకం sbi_pmu_ctr_cfg_సరిపోలిక(గుర్తులేని దీర్ఘ cidx_పునాది, గుర్తులేని దీర్ఘ cidx_ముసుగు,
			  గుర్తులేని దీర్ఘ flags, గుర్తులేని దీర్ఘ event_idx,
			  uint64_t event_దత్తాంశం)
{
	నిర్మాణం sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ఒకవేళ (unlikely(!phs))
		తిరిగి SBI_EINVAL;

	పూర్ణాంకం ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_సంకేతం;

	ఒకవేళ (!pmu_ctr_idx_validate(cidx_పునాది, cidx_ముసుగు))
		తిరిగి SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_దత్తాంశం);
	ఒకవేళ (event_type < 0)
		తిరిగి SBI_EINVAL;
	event_సంకేతం = get_cidx_సంకేతం(event_idx);

	ఒకవేళ (flags & SBI_PMU_CFG_FLAG_SKIP_సరిపోలిక) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		గుర్తులేని దీర్ఘ cidx_మొదటి = cidx_పునాది + sbi_ffs(cidx_ముసుగు);

		ఒకవేళ (cidx_మొదటి >= మొత్తం_ctrs)
			తిరిగి SBI_EINVAL;

		ఒకవేళ (phs->చురుకైన_events[cidx_మొదటి] == SBI_PMU_EVENT_IDX_INVALID)
			తిరిగి SBI_EINVAL;
		ctr_idx = cidx_మొదటి;
		వెళ్ళు skip_సరిపోలిక;
	}

	ఒకవేళ (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_కనుగొను_fw(phs, cidx_పునాది, cidx_ముసుగు,
					  event_సంకేతం, event_దత్తాంశం);
		ఒకవేళ ((event_సంకేతం == SBI_PMU_FW_PLATFORM) && (ctr_idx >= అంకె_hw_ctrs))
			phs->fw_counters_దత్తాంశం[ctr_idx - అంకె_hw_ctrs] =
								event_దత్తాంశం;
	} లేకపోతే {
		ctr_idx = pmu_ctr_కనుగొను_hw(phs, cidx_పునాది, cidx_ముసుగు, flags,
					  event_idx, event_దత్తాంశం);
		ఒకవేళ (ctr_idx >= 0) {
			నిర్మాణం sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_దత్తాంశం = event_దత్తాంశం;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_ముసుగు;
		}
	}

	ఒకవేళ (ctr_idx < 0)
		తిరిగి SBI_ENOTSUPP;

	phs->చురుకైన_events[ctr_idx] = event_idx;
skip_సరిపోలిక:
	ఒకవేళ (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		ఒకవేళ (flags & SBI_PMU_CFG_FLAG_CLEAR_విలువ)
			pmu_ctr_write_hw(ctr_idx, 0);
		ఒకవేళ (flags & SBI_PMU_CFG_FLAG_AUTO_ప్రారంభం)
			pmu_ctr_ప్రారంభం_hw(ctr_idx, 0, false);
	} లేకపోతే ఒకవేళ (event_type == SBI_PMU_EVENT_TYPE_FW) {
		ఒకవేళ (flags & SBI_PMU_CFG_FLAG_CLEAR_విలువ)
			phs->fw_counters_దత్తాంశం[ctr_idx - అంకె_hw_ctrs] = 0;
		ఒకవేళ (flags & SBI_PMU_CFG_FLAG_AUTO_ప్రారంభం) {
			ఒకవేళ (SBI_PMU_FW_PLATFORM == event_సంకేతం &&
			    pmu_dev && pmu_dev->fw_counter_ప్రారంభం) {
				ret = pmu_dev->fw_counter_ప్రారంభం(
					phs->hartid,
					ctr_idx - అంకె_hw_ctrs, event_దత్తాంశం);
				ఒకవేళ (ret)
					తిరిగి ret;
			}
			phs->fw_counters_started |= బిట్టు(ctr_idx - అంకె_hw_ctrs);
		}
	}

	తిరిగి ctr_idx;
}

పూర్ణాంకం sbi_pmu_ctr_incr_fw(గణన sbi_pmu_fw_event_సంకేతం_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = ఖాళీ;
	నిర్మాణం sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ఒకవేళ (unlikely(!phs))
		తిరిగి 0;

	ఒకవేళ (likely(!phs->fw_counters_started))
		తిరిగి 0;

	ఒకవేళ (unlikely(fw_id >= SBI_PMU_FW_MAX))
		తిరిగి SBI_EINVAL;

	కొరకు (cidx = అంకె_hw_ctrs; cidx < మొత్తం_ctrs; cidx++) {
		ఒకవేళ (get_cidx_సంకేతం(phs->చురుకైన_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & బిట్టు(cidx - అంకె_hw_ctrs))) {
			fcounter = &phs->fw_counters_దత్తాంశం[cidx - అంకె_hw_ctrs];
			ఆపు;
		}
	}

	ఒకవేళ (fcounter)
		(*fcounter)++;

	తిరిగి 0;
}

గుర్తులేని దీర్ఘ sbi_pmu_అంకె_ctr(శూన్య)
{
	తిరిగి (అంకె_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

పూర్ణాంకం sbi_pmu_ctr_get_info(uint32_t cidx, గుర్తులేని దీర్ఘ *ctr_info)
{
	పూర్ణాంకం వెడల్పు;
	సంఘం sbi_pmu_ctr_info cinfo = {0};
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	గుర్తులేని దీర్ఘ counter_ముసుగు = (గుర్తులేని దీర్ఘ)sbi_hart_mhpm_ముసుగు(scratch) |
				     SBI_PMU_CY_IR_ముసుగు;

	/* Sanity check */
	ఒకవేళ (cidx >= మొత్తం_ctrs)
		తిరిగి SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	ఒకవేళ (cidx < అంకె_hw_ctrs) {
		ఒకవేళ (!(__test_bit(cidx, &counter_ముసుగు)))
			తిరిగి SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_చక్రం + cidx;
		/* mcycle & minstret are always 64 bit */
		ఒకవేళ (cidx == 0 || cidx == 2)
			cinfo.వెడల్పు = 63;
		లేకపోతే
			cinfo.వెడల్పు = sbi_hart_mhpm_బిట్లు(scratch) - 1;
	} లేకపోతే {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.వెడల్పు = 63;
		ఒకవేళ (pmu_dev && pmu_dev->fw_counter_వెడల్పు) {
			వెడల్పు = pmu_dev->fw_counter_వెడల్పు();
			ఒకవేళ (వెడల్పు)
				cinfo.వెడల్పు = వెడల్పు - 1;
		}
	}

	*ctr_info = cinfo.విలువ;

	తిరిగి 0;
}

పూర్ణాంకం sbi_pmu_event_get_info(గుర్తులేని దీర్ఘ shmem_phys_lo, గుర్తులేని దీర్ఘ shmem_phys_hi,
			   గుర్తులేని దీర్ఘ అంకె_events, గుర్తులేని దీర్ఘ flags)
{
	గుర్తులేని దీర్ఘ shmem_కొలత = అంకె_events * పరిమాణం(నిర్మాణం sbi_pmu_event_info);
	పూర్ణాంకం i, j, event_type;
	నిర్మాణం sbi_pmu_event_info *einfo;
	నిర్మాణం sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	నిర్మాణం sbi_pmu_hw_event *తాత్కాలిక;
	bool దొరికింది = false;

	ఒకవేళ (flags != 0)
		తిరిగి SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	ఒకవేళ (!అంకె_events || (shmem_phys_lo & 0xF))
		తిరిగి SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	ఒకవేళ (shmem_phys_hi)
		తిరిగి SBI_EINVALID_ADDR;

	ఒకవేళ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_కొలత, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		తిరిగి SBI_ERR_INVALID_చిరునామా;

	sbi_hart_protection_తాత్కాలిక_map_range(shmem_phys_lo, shmem_కొలత);

	einfo = (నిర్మాణం sbi_pmu_event_info *)(shmem_phys_lo);
	కొరకు (i = 0; i < అంకె_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		ఒకవేళ (event_idx & SBI_PMU_EVENT_IDX_MBZ_ముసుగు)
			తిరిగి SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_దత్తాంశం);
		ఒకవేళ (event_type < 0) {
			einfo[i].output = 0;
		} లేకపోతే ఒకవేళ (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} లేకపోతే {
			కొరకు (j = 0; j < అంకె_hw_events; j++) {
				తాత్కాలిక = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				ఒకవేళ (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					ఒకవేళ (తాత్కాలిక->ప్రారంభం_idx > event_idx ||
					    event_idx > తాత్కాలిక->చివర_idx)
						కొనసాగు;
					/* just match the selector */
					ఒకవేళ (తాత్కాలిక->select == (einfo[i].event_దత్తాంశం &
									తాత్కాలిక->select_ముసుగు)) {
						దొరికింది = true;
						ఆపు;
					}
				} లేకపోతే ఒకవేళ (తాత్కాలిక->ప్రారంభం_idx <= event_idx &&
					   event_idx <= తాత్కాలిక->చివర_idx) {
					దొరికింది = true;
					ఆపు;
				}
			}
			ఒకవేళ (దొరికింది)
				einfo[i].output = 1;
			లేకపోతే
				einfo[i].output = 0;
			దొరికింది = false;
		}
	}

	sbi_hart_protection_తాత్కాలిక_unmap_range(shmem_phys_lo, shmem_కొలత);

	తిరిగి 0;
}

స్థిర శూన్య pmu_reset_event_map(నిర్మాణం sbi_pmu_hart_state *phs)
{
	పూర్ణాంకం j;

	/* Initialize the counter to event mapping table */
	కొరకు (j = 3; j < మొత్తం_ctrs; j++)
		phs->చురుకైన_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	కొరకు (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_దత్తాంశం[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

స్థిరరాశి నిర్మాణం sbi_pmu_device *sbi_pmu_get_device(శూన్య)
{
	తిరిగి pmu_dev;
}

శూన్య sbi_pmu_సమితి_device(స్థిరరాశి నిర్మాణం sbi_pmu_device *dev)
{
	ఒకవేళ (!dev || pmu_dev)
		తిరిగి;

	pmu_dev = dev;
}

శూన్య sbi_pmu_exit(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	ఒకవేళ (unlikely(!phs))
		తిరిగి;

	pmu_reset_event_map(phs);
}

స్థిర శూన్య pmu_sse_enable(uint32_t event_id)
{
	గుర్తులేని దీర్ఘ irq_ముసుగు = sbi_pmu_irq_ముసుగు();

	csr_సమితి(CSR_MIE, irq_ముసుగు);
}

స్థిర శూన్య pmu_sse_disable(uint32_t event_id)
{
	గుర్తులేని దీర్ఘ irq_ముసుగు = sbi_pmu_irq_ముసుగు();

	csr_clear(CSR_MIE, irq_ముసుగు);
	csr_clear(CSR_MIP, irq_ముసుగు);
}

స్థిర శూన్య pmu_sse_complete(uint32_t event_id)
{
	csr_సమితి(CSR_MIE, sbi_pmu_irq_ముసుగు());
}

స్థిర శూన్య pmu_sse_రిజిస్టరు(uint32_t event_id)
{
	నిర్మాణం sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_ముసుగు());
}

స్థిర శూన్య pmu_sse_unregister(uint32_t event_id)
{
	నిర్మాణం sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_సమితి(CSR_MIDELEG, sbi_pmu_irq_ముసుగు());
}

స్థిర స్థిరరాశి నిర్మాణం sbi_sse_cb_ops pmu_sse_cb_ops = {
	.రిజిస్టరు_cb = pmu_sse_రిజిస్టరు,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

పూర్ణాంకం sbi_pmu_init(నిర్మాణం sbi_scratch *scratch, bool cold_boot)
{
	పూర్ణాంకం hpm_లెక్క = sbi_fls(sbi_hart_mhpm_ముసుగు(scratch));
	నిర్మాణం sbi_pmu_hart_state *phs;
	స్థిరరాశి నిర్మాణం sbi_platform *plat;
	పూర్ణాంకం rc;

	ఒకవేళ (cold_boot) {
		hw_event_map = sbi_calloc(పరిమాణం(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		ఒకవేళ (!hw_event_map)
			తిరిగి SBI_ENOMEM;

		phs_ptr_స్థానభ్రంశం = sbi_scratch_alloc_type_స్థానభ్రంశం(శూన్య *);
		ఒకవేళ (!phs_ptr_స్థానభ్రంశం) {
			sbi_free(hw_event_map);
			తిరిగి SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		ఒకవేళ (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		ఒకవేళ (!hpm_లెక్క)
			/* Only CY, TM & IR are implemented in the hw */
			అంకె_hw_ctrs = 3;
		లేకపోతే
			అంకె_hw_ctrs = hpm_లెక్క + 1;

		ఒకవేళ (అంకె_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			తిరిగి SBI_EINVAL;

		మొత్తం_ctrs = అంకె_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		ఒకవేళ (sbi_pmu_irq_బిట్టు() >= 0)
			sbi_sse_కూడు_event(SBI_SSE_EVENT_LOCAL_PMU_అధికప్రవాహం, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	ఒకవేళ (!phs) {
		phs = sbi_zalloc(పరిమాణం(*phs));
		ఒకవేళ (!phs)
			తిరిగి SBI_ENOMEM;
		phs->hartid = current_hartid();
		pmu_సమితి_hart_state_ptr(scratch, phs);
	}

	pmu_reset_event_map(phs);

	/* First three counters are fixed by the priv spec and we enable it by default */
	phs->చురుకైన_events[0] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_స్థానభ్రంశం) |
				SBI_PMU_HW_CPU_చక్రాలు;
	phs->చురుకైన_events[1] = SBI_PMU_EVENT_IDX_INVALID;
	phs->చురుకైన_events[2] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_స్థానభ్రంశం) |
				SBI_PMU_HW_INSTRUCTIONS;

	తిరిగి 0;
}
