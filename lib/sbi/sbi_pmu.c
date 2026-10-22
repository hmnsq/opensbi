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
	uint32_t अन्तः_idx;
	/* Event selector value used only for raw events. The event select value
	 * can be a even id or a selector value for set of events encoded in few
	 * bits. In case latter, the bits used for encoding of the events should
	 * be zeroed out in the select value.
	 */
	uint64_t select;
	 /**
	  * The select_mask indicates which bits are encoded for the event(s).
	  */
	uint64_t select_आवरणम्;
};

/* Information about PMU counters as per SBI specification */
सङ्घ sbi_pmu_ctr_info {
	अचिह्नित दीर्घ मानम्;
	संरचना {
		अचिह्नित दीर्घ csr:12;
		अचिह्नित दीर्घ विस्तारः:6;
#यदि __riscv_xlen == 32
		अचिह्नित दीर्घ reserved:13;
#अन्यथा
		अचिह्नित दीर्घ reserved:45;
#यदि_अन्तः
		अचिह्नित दीर्घ type:1;
	};
};

#यदि SBI_PMU_FW_CTR_MAX >= द्वयङ्काः_प्रत्येकम्_LONG
#दोषः "Can't handle firmware counters beyond BITS_PER_LONG"
#यदि_अन्तः

/** HW event configuration parameters */
संरचना sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_दत्त;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
संरचना sbi_pmu_hart_state {
	/* HART to which this state belongs */
	uint32_t hartid;
	/* Counter to enabled event mapping */
	uint32_t सक्रिय_events[SBI_PMU_HW_CTR_MAX + SBI_PMU_FW_CTR_MAX];
	/* Bitmap of firmware counters started */
	अचिह्नित दीर्घ fw_counters_started;
	/* if true, SSE is enabled */
	bool sse_enabled;
	/*
	 * Counter values for SBI firmware events and event codes
	 * for platform firmware events. Both are mutually exclusive
	 * and hence can optimally share the same memory.
	 */
	uint64_t fw_counters_दत्त[SBI_PMU_FW_CTR_MAX];
	/* HW events configuration parameters from
	 * sbi_pmu_ctr_cfg_match() command which are
	 * used for restoring RAW hardware events after
	 * cpu suspending.
	 */
	संरचना sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
स्थिर अचिह्नित दीर्घ phs_ptr_विस्थापनम्;

#परिभाषय pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_विस्थापनम् ? sbi_scratch_read_type((__scratch), शून्य *, phs_ptr_विस्थापनम्) : रिक्त

#परिभाषय pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#परिभाषय pmu_समुच्चयः_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), शून्य *, phs_ptr_विस्थापनम्, (__phs))

/* Platform specific PMU device */
स्थिर ध्रुव संरचना sbi_pmu_device *pmu_dev = रिक्त;

/* Mapping between event range and possible counters  */
स्थिर संरचना sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
स्थिर uint32_t अङ्कः_hw_events;
/* Maximum number of hardware counters available */
स्थिर uint32_t अङ्कः_hw_ctrs;

/* Maximum number of counters available */
स्थिर uint32_t योग_ctrs;

/* Helper macros to retrieve event idx and code type */
#परिभाषय get_cidx_type(x) \
  (((x) & SBI_PMU_EVENT_IDX_TYPE_आवरणम्) >> SBI_PMU_EVENT_IDX_TYPE_विस्थापनम्)
#परिभाषय get_cidx_संहिता(x) (x & SBI_PMU_EVENT_IDX_संहिता_आवरणम्)

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
	यदि (((evtA->अन्तः_idx < evtB->start_idx) && (evtA->अन्तः_idx < evtB->अन्तः_idx)) ||
	   ((evtA->start_idx > evtB->start_idx) && (evtA->start_idx > evtB->अन्तः_idx)))
		प्रत्यागम false;
	प्रत्यागम true;
}

स्थिर bool pmu_event_select_overlap(संरचना sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_आवरणम्)
{
	यदि ((evt->select == select_val) && (evt->select_आवरणम् == select_आवरणम्))
		प्रत्यागम true;

	प्रत्यागम false;
}

स्थिर पूर्णाङ्क pmu_event_validate(संरचना sbi_pmu_hart_state *phs,
			      अचिह्नित दीर्घ event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_संहिता = get_cidx_संहिता(event_idx);
	uint32_t event_idx_संहिता_max = -1;
	uint32_t cache_ops_परिणाम, cache_ops_id, cache_id;

	विकल्प(event_idx_type) {
	पक्ष SBI_PMU_EVENT_TYPE_HW:
		event_idx_संहिता_max = SBI_PMU_HW_GENERAL_MAX;
		भङ्ग;
	पक्ष SBI_PMU_EVENT_TYPE_FW:
		यदि ((event_idx_संहिता >= SBI_PMU_FW_MAX &&
		    event_idx_संहिता <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_संहिता > SBI_PMU_FW_PLATFORM)
			प्रत्यागम SBI_EINVAL;

		यदि (SBI_PMU_FW_PLATFORM == event_idx_संहिता &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			प्रत्यागम pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		अन्यथा
			event_idx_संहिता_max = SBI_PMU_FW_MAX;
		भङ्ग;
	पक्ष SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_परिणाम = event_idx_संहिता &
					SBI_PMU_EVENT_HW_CACHE_OPS_परिणाम;
		cache_ops_id = (event_idx_संहिता &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_आवरणम्) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_विस्थापनम्;
		cache_id = (event_idx_संहिता &
			    SBI_PMU_EVENT_HW_CACHE_ID_आवरणम्) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_विस्थापनम्;
		यदि ((cache_ops_परिणाम < SBI_PMU_HW_CACHE_परिणाम_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			प्रत्यागम event_idx_type;
		अन्यथा
			प्रत्यागम SBI_EINVAL;
		भङ्ग;
	पक्ष SBI_PMU_EVENT_TYPE_HW_RAW:
	पक्ष SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_संहिता_max = 1; // event_idx.code should be zero
		भङ्ग;
	स्वभाव:
		प्रत्यागम SBI_EINVAL;
	}

	यदि (event_idx_संहिता < event_idx_संहिता_max)
		प्रत्यागम event_idx_type;

	प्रत्यागम SBI_EINVAL;
}

स्थिर पूर्णाङ्क pmu_ctr_validate(संरचना sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_संहिता)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	यदि (cidx >= योग_ctrs)
		प्रत्यागम SBI_EINVAL;

	event_idx_val = phs->सक्रिय_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	यदि (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		प्रत्यागम SBI_EINVAL;

	*event_idx_संहिता = get_cidx_संहिता(event_idx_val);

	प्रत्यागम event_idx_type;
}

स्थिर bool pmu_ctr_idx_validate(अचिह्नित दीर्घ cbase, अचिह्नित दीर्घ cmask)
{
	अचिह्नित दीर्घ चरमः;

	यदि (!cmask)
		प्रत्यागम false;

	चरमः = sbi_fls(cmask);
	यदि (cbase > -1UL - चरमः)
		प्रत्यागम false;

	प्रत्यागम (cbase + चरमः) < योग_ctrs;
}

पूर्णाङ्क sbi_pmu_ctr_fw_read(अचिह्नित दीर्घ cidx, uint64_t *cval, bool उच्चः_द्वयङ्काः)
{
	पूर्णाङ्क event_idx_type;
	uint32_t event_संहिता;
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	यदि (unlikely(!phs))
		प्रत्यागम SBI_EINVAL;

	यदि (cidx < अङ्कः_hw_ctrs || cidx >= योग_ctrs)
		प्रत्यागम SBI_EINVAL;

#यदि __riscv_xlen > 32
	यदि (उच्चः_द्वयङ्काः)
		प्रत्यागम 0;
#यदि_अन्तः

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_संहिता);
	यदि (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		प्रत्यागम SBI_EINVAL;

	यदि ((event_संहिता >= SBI_PMU_FW_MAX &&
	    event_संहिता <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_संहिता > SBI_PMU_FW_PLATFORM)
		प्रत्यागम SBI_EINVAL;

	यदि (SBI_PMU_FW_PLATFORM == event_संहिता) {
		यदि (pmu_dev && pmu_dev->fw_गणक_read_मानम्)
			*cval = pmu_dev->fw_गणक_read_मानम्(phs->hartid,
							       cidx -
							       अङ्कः_hw_ctrs);
		अन्यथा
			*cval = 0;
	} अन्यथा
		*cval = phs->fw_counters_दत्त[cidx - अङ्कः_hw_ctrs];

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क pmu_सङ्कलय_hw_event_map(u32 eidx_start, u32 eidx_अन्तः, u32 cmap,
				uint64_t select, uint64_t select_आवरणम्)
{
	पूर्णाङ्क i = 0;
	bool is_overlap;
	संरचना sbi_pmu_hw_event *event = &hw_event_map[अङ्कः_hw_events];
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_आवरणम् = sbi_hart_mhpm_आवरणम्(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	यदि (eidx_start > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_आवरणम्))
		प्रत्यागम SBI_EDENIED;

	यदि (अङ्कः_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		प्रत्यागम SBI_EFAIL;
	}

	event->start_idx = eidx_start;
	event->अन्तः_idx = eidx_अन्तः;

	/* Sanity check */
	प्रति (i = 0; i < अङ्कः_hw_events; i++) {
		यदि (eidx_start == SBI_PMU_EVENT_RAW_IDX ||
			eidx_start == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_आवरणम्);
		अन्यथा
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		यदि (is_overlap)
			गच्छ reset_event;
	}

	event->select_आवरणम् = select_आवरणम्;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_आवरणम्;
	event->select = select;
	अङ्कः_hw_events++;

	प्रत्यागम 0;

reset_event:
	event->start_idx = 0;
	event->अन्तः_idx = 0;
	प्रत्यागम SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
पूर्णाङ्क sbi_pmu_सङ्कलय_hw_event_गणक_map(u32 eidx_start, u32 eidx_अन्तः, u32 cmap)
{
	यदि ((eidx_start > eidx_अन्तः) || eidx_start >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_अन्तः >= SBI_PMU_EVENT_RAW_IDX)
		प्रत्यागम SBI_EINVAL;

	प्रत्यागम pmu_सङ्कलय_hw_event_map(eidx_start, eidx_अन्तः, cmap, 0, 0);
}

पूर्णाङ्क sbi_pmu_सङ्कलय_raw_event_गणक_map(uint64_t select, uint64_t select_आवरणम्, u32 cmap)
{
	प्रत्यागम pmu_सङ्कलय_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_आवरणम्);
}

शून्य sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_आवरणम्());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_अतिप्रवाहः);
}

स्थिर पूर्णाङ्क pmu_ctr_enable_irq_hw(पूर्णाङ्क ctr_idx)
{
	अचिह्नित दीर्घ mhpmevent_csr;
	अचिह्नित दीर्घ mhpmevent_curr;
	अचिह्नित दीर्घ mip_val;
	अचिह्नित दीर्घ सम्बन्धि_आवरणम्;

	यदि (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		प्रत्यागम SBI_EFAIL;

#यदि __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	सम्बन्धि_आवरणम् = (uint32_t)~MHPMEVENTH_सम्बन्धि;
#अन्यथा
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	सम्बन्धि_आवरणम् = ~MHPMEVENT_सम्बन्धि;
#यदि_अन्तः

	mhpmevent_curr = csr_read_अङ्कः(mhpmevent_csr);
	mip_val = csr_read(CSR_MIP);
	/**
	 * Clear out the OF bit so that next interrupt can be enabled.
	 * This should be done only when the corresponding overflow interrupt
	 * bit is cleared. That indicates that software has already handled the
	 * previous interrupts or the hardware yet to set an overflow interrupt.
	 * Otherwise, there will be race conditions where we may clear the bit
	 * the software is yet to handle the interrupt.
	 */
	यदि (!(mip_val & sbi_pmu_irq_आवरणम्())) {
		mhpmevent_curr &= सम्बन्धि_आवरणम्;
		csr_write_अङ्कः(mhpmevent_csr, mhpmevent_curr);
	}

	प्रत्यागम 0;
}

स्थिर शून्य pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#यदि __riscv_xlen == 32
	csr_write_अङ्कः(CSR_MCYCLE + cidx, 0);
	csr_write_अङ्कः(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_अङ्कः(CSR_MCYCLEH + cidx, ival >> द्वयङ्काः_प्रत्येकम्_LONG);
#अन्यथा
	csr_write_अङ्कः(CSR_MCYCLE + cidx, ival);
#यदि_अन्तः
}

स्थिर पूर्णाङ्क pmu_ctr_start_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	अचिह्नित दीर्घ mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	यदि (cidx >= अङ्कः_hw_ctrs || cidx == 1)
		प्रत्यागम SBI_EINVAL;

	यदि (sbi_hart_priv_संस्करणम्(scratch) < SBI_HART_PRIV_VER_1_11) {
		यदि (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		प्रत्यागम 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	यदि (!__test_bit(cidx, &mctr_inhbt))
		प्रत्यागम SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	यदि (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	यदि (pmu_dev && pmu_dev->hw_गणक_enable_irq)
		pmu_dev->hw_गणक_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_pmu_irq_bit(शून्य)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		प्रत्यागम IRQ_PMU_OVF;
	यदि (pmu_dev && pmu_dev->hw_गणक_irq_bit)
		प्रत्यागम pmu_dev->hw_गणक_irq_bit();

	प्रत्यागम -1;
}

अचिह्नित दीर्घ sbi_pmu_irq_आवरणम्(शून्य)
{
	पूर्णाङ्क irq_bit = sbi_pmu_irq_bit();

	यदि (irq_bit < 0)
		प्रत्यागम 0;

	प्रत्यागम BIT(irq_bit);
}

स्थिर पूर्णाङ्क pmu_ctr_start_fw(संरचना sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_संहिता,
			    uint64_t event_दत्त, uint64_t ival,
			    bool ival_update)
{
	पूर्णाङ्क ret;

	यदि ((event_संहिता >= SBI_PMU_FW_MAX &&
	    event_संहिता <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_संहिता > SBI_PMU_FW_PLATFORM)
		प्रत्यागम SBI_EINVAL;

	यदि (phs->fw_counters_started & BIT(cidx - अङ्कः_hw_ctrs))
		प्रत्यागम SBI_EALREADY_STARTED;

	यदि (SBI_PMU_FW_PLATFORM == event_संहिता) {
		यदि (!pmu_dev ||
		    !pmu_dev->fw_गणक_write_मानम् ||
		    !pmu_dev->fw_गणक_start) {
			प्रत्यागम SBI_EINVAL;
		}

		यदि (ival_update)
			pmu_dev->fw_गणक_write_मानम्(phs->hartid,
							cidx - अङ्कः_hw_ctrs,
							ival);

		ret = pmu_dev->fw_गणक_start(phs->hartid,
						 cidx - अङ्कः_hw_ctrs,
						 event_दत्त);
		यदि (ret)
			प्रत्यागम ret;
	} अन्यथा {
		यदि (ival_update)
			phs->fw_counters_दत्त[cidx - अङ्कः_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= BIT(cidx - अङ्कः_hw_ctrs);

	प्रत्यागम 0;
}

स्थिर शून्य pmu_update_inhibit_flags(अचिह्नित दीर्घ flags, uint64_t *mhpmevent_val)
{
	यदि (flags & SBI_PMU_CFG_FLAG_समुच्चयः_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	यदि (flags & SBI_PMU_CFG_FLAG_समुच्चयः_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	यदि (flags & SBI_PMU_CFG_FLAG_समुच्चयः_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	यदि (flags & SBI_PMU_CFG_FLAG_समुच्चयः_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

स्थिर पूर्णाङ्क pmu_update_hw_mhpmevent(संरचना sbi_pmu_hw_event *hw_evt, पूर्णाङ्क ctr_idx,
				   अचिह्नित दीर्घ flags, अचिह्नित दीर्घ eindex,
				   uint64_t दत्त)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ध्रुव संरचना sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_गन्तव्यम्_mhpmevent(plat, eindex, दत्त);

	यदि (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		प्रत्यागम SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_आवरणम्) |
				 MHPMEVENT_MINH | MHPMEVENT_सम्बन्धि;

	यदि (pmu_dev && pmu_dev->hw_गणक_disable_irq)
		pmu_dev->hw_गणक_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	यदि (pmu_dev && pmu_dev->hw_गणक_filter_बहुलकम्)
		pmu_dev->hw_गणक_filter_बहुलकम्(flags, ctr_idx);

#यदि __riscv_xlen == 32
	csr_write_अङ्कः(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_अङ्कः(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> द्वयङ्काः_प्रत्येकम्_LONG);
#अन्यथा
	csr_write_अङ्कः(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#यदि_अन्तः

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_pmu_ctr_start(अचिह्नित दीर्घ cbase, अचिह्नित दीर्घ cmask,
		      अचिह्नित दीर्घ flags, uint64_t ival)
{
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	यदि (unlikely(!phs))
		प्रत्यागम SBI_EINVAL;

	पूर्णाङ्क event_idx_type;
	uint32_t event_संहिता;
	पूर्णाङ्क ret = SBI_EINVAL;
	bool bUpdate = false;
	पूर्णाङ्क i, cidx;
	uint64_t edata;

	यदि (!pmu_ctr_idx_validate(cbase, cmask))
		प्रत्यागम ret;

	यदि (flags & ~SBI_PMU_START_FLAGS_आवरणम्)
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	यदि (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		प्रत्यागम SBI_ENO_SHMEM;

	यदि (flags & SBI_PMU_START_FLAG_समुच्चयः_INIT_मानम्)
		bUpdate = true;

	for_each_समुच्चयः_bit(i, &cmask, द्वयङ्काः_प्रत्येकम्_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_संहिता);
		यदि (event_idx_type < 0)
			/* Continue the start operation for other counters */
			अनुवर्त;
		अन्यथा यदि (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_संहिता == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_दत्त[cidx - अङ्कः_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_start_fw(phs, cidx, event_संहिता, edata,
					       ival, bUpdate);
			यदि (ret)
				प्रत्यागम ret;
		} अन्यथा {
			यदि (cidx >= 3) {
				संरचना sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->सक्रिय_events[cidx],
							ev_cfg->event_दत्त);
				यदि (ret)
					प्रत्यागम ret;
			}
			ret = pmu_ctr_start_hw(cidx, ival, bUpdate);
			यदि (ret)
				प्रत्यागम ret;
		}
	}

	प्रत्यागम ret;
}

स्थिर पूर्णाङ्क pmu_ctr_stop_hw(uint32_t cidx)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	अचिह्नित दीर्घ mctr_inhbt;

	यदि (sbi_hart_priv_संस्करणम्(scratch) < SBI_HART_PRIV_VER_1_11)
		प्रत्यागम 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	यदि (cidx >= अङ्कः_hw_ctrs || cidx == 1)
		प्रत्यागम SBI_EINVAL;

	यदि (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		यदि (pmu_dev && pmu_dev->hw_गणक_disable_irq) {
			pmu_dev->hw_गणक_disable_irq(cidx);
		}
		प्रत्यागम 0;
	} अन्यथा
		प्रत्यागम SBI_EALREADY_STOPPED;
}

स्थिर पूर्णाङ्क pmu_ctr_stop_fw(संरचना sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_संहिता)
{
	पूर्णाङ्क ret;

	यदि ((event_संहिता >= SBI_PMU_FW_MAX &&
	    event_संहिता <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_संहिता > SBI_PMU_FW_PLATFORM)
		प्रत्यागम SBI_EINVAL;

	यदि (!(phs->fw_counters_started & BIT(cidx - अङ्कः_hw_ctrs)))
		प्रत्यागम SBI_EALREADY_STOPPED;

	यदि (SBI_PMU_FW_PLATFORM == event_संहिता &&
	    pmu_dev && pmu_dev->fw_गणक_stop) {
		ret = pmu_dev->fw_गणक_stop(phs->hartid, cidx - अङ्कः_hw_ctrs);
		यदि (ret)
			प्रत्यागम ret;
	}

	phs->fw_counters_started &= ~BIT(cidx - अङ्कः_hw_ctrs);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क pmu_reset_hw_mhpmevent(पूर्णाङ्क ctr_idx)
{
	यदि (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		प्रत्यागम SBI_EFAIL;
#यदि __riscv_xlen == 32
	csr_write_अङ्कः(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	यदि (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_अङ्कः(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#अन्यथा
	csr_write_अङ्कः(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#यदि_अन्तः

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_pmu_ctr_stop(अचिह्नित दीर्घ cbase, अचिह्नित दीर्घ cmask,
		     अचिह्नित दीर्घ flag)
{
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	यदि (unlikely(!phs))
		प्रत्यागम SBI_EINVAL;

	पूर्णाङ्क ret = SBI_EINVAL;
	पूर्णाङ्क event_idx_type;
	uint32_t event_संहिता;
	पूर्णाङ्क i, cidx;

	यदि (!pmu_ctr_idx_validate(cbase, cmask))
		प्रत्यागम ret;

	यदि (flag & ~SBI_PMU_STOP_FLAGS_आवरणम्)
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	यदि (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		प्रत्यागम SBI_ENO_SHMEM;

	for_each_समुच्चयः_bit(i, &cmask, द्वयङ्काः_प्रत्येकम्_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_संहिता);
		यदि (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			अनुवर्त;

		अन्यथा यदि (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_संहिता);
		अन्यथा
			ret = pmu_ctr_stop_hw(cidx);

		यदि(ret)
			प्रत्यागम ret;

		यदि (cidx > (CSR_INSTRET - CSR_आवृत्तिः) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->सक्रिय_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	यदि (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_आवरणम्());

	प्रत्यागम ret;
}

स्थिर पूर्णाङ्क pmu_fixed_ctr_update_inhibit_द्वयङ्काः(पूर्णाङ्क fixed_ctr, अचिह्नित दीर्घ flags)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#यदि __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#यदि_अन्तः
	यदि (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_गणक_filter_बहुलकम्))
		प्रत्यागम fixed_ctr;

	विकल्प (fixed_ctr) {
	पक्ष 0:
		cfg_csr_no = CSR_MCYCLECFG;
#यदि __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#यदि_अन्तः
		भङ्ग;
	पक्ष 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#यदि __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#यदि_अन्तः
		भङ्ग;
	स्वभाव:
		प्रत्यागम SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	यदि (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#यदि __riscv_xlen == 32
		csr_write_अङ्कः(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_अङ्कः(cfgh_csr_no, cfg_val >> द्वयङ्काः_प्रत्येकम्_LONG);
#अन्यथा
		csr_write_अङ्कः(cfg_csr_no, cfg_val);
#यदि_अन्तः
	}
	यदि (pmu_dev && pmu_dev->hw_गणक_filter_बहुलकम्)
		pmu_dev->hw_गणक_filter_बहुलकम्(flags, fixed_ctr);
	प्रत्यागम fixed_ctr;
}

स्थिर पूर्णाङ्क pmu_ctr_find_fixed_hw(अचिह्नित दीर्घ evt_idx_संहिता)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	यदि (evt_idx_संहिता == SBI_PMU_HW_CPU_आवृत्तयः)
		प्रत्यागम 0;
	अन्यथा यदि (evt_idx_संहिता == SBI_PMU_HW_INSTRUCTIONS)
		प्रत्यागम 2;
	अन्यथा
		प्रत्यागम SBI_EINVAL;
}

स्थिर पूर्णाङ्क pmu_ctr_find_hw(संरचना sbi_pmu_hart_state *phs,
			   अचिह्नित दीर्घ cbase, अचिह्नित दीर्घ cmask,
			   अचिह्नित दीर्घ flags,
			   अचिह्नित दीर्घ event_idx, uint64_t दत्त)
{
	अचिह्नित दीर्घ ctr_आवरणम्;
	पूर्णाङ्क i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	संरचना sbi_pmu_hw_event *अस्थायी;
	अचिह्नित दीर्घ mctr_inhbt = 0;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	यदि (cbase >= अङ्कः_hw_ctrs)
		प्रत्यागम SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_find_fixed_hw(event_idx);
	यदि (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		प्रत्यागम pmu_fixed_ctr_update_inhibit_द्वयङ्काः(fixed_ctr, flags);

	यदि (sbi_hart_priv_संस्करणम्(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	प्रति (i = 0; i < अङ्कः_hw_events; i++) {
		अस्थायी = &hw_event_map[i];
		यदि ((अस्थायी->start_idx > event_idx && event_idx < अस्थायी->अन्तः_idx) ||
		    (अस्थायी->start_idx < event_idx && event_idx > अस्थायी->अन्तः_idx))
			अनुवर्त;

		/* For raw events, event data is used as the select value */
		यदि (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_आवरणम् = अस्थायी->select_आवरणम्;

			/* The non-event map bits of data should match the selector */
			यदि (अस्थायी->select != (दत्त & select_आवरणम्))
				अनुवर्त;
		}
		/* Fixed counters should not be part of the search */
		ctr_आवरणम् = अस्थायी->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_आवरणम्);
		for_each_समुच्चयः_bit_from(cbase, &ctr_आवरणम्, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			यदि (phs->सक्रिय_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				अनुवर्त;
			/* If mcountinhibit is supported, the bit must be enabled */
			यदि ((sbi_hart_priv_संस्करणम्(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				अनुवर्त;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			भङ्ग;
		}
	}

	यदि (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		यदि (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			प्रत्यागम SBI_EFAIL;

		प्रत्यागम pmu_fixed_ctr_update_inhibit_द्वयङ्काः(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(अस्थायी, ctr_idx, flags, event_idx, दत्त);

	यदि (!ret)
		ret = ctr_idx;

	प्रत्यागम ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
स्थिर पूर्णाङ्क pmu_ctr_find_fw(संरचना sbi_pmu_hart_state *phs,
			   अचिह्नित दीर्घ cbase, अचिह्नित दीर्घ cmask,
			   uint32_t event_संहिता, uint64_t edata)
{
	पूर्णाङ्क i, cidx;

	for_each_समुच्चयः_bit(i, &cmask, द्वयङ्काः_प्रत्येकम्_LONG) {
		cidx = i + cbase;
		यदि (cidx < अङ्कः_hw_ctrs || योग_ctrs <= cidx)
			अनुवर्त;
		यदि (phs->सक्रिय_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			अनुवर्त;
		यदि (SBI_PMU_FW_PLATFORM == event_संहिता &&
		    pmu_dev && pmu_dev->fw_गणक_मेलनम्_encoding) {
			यदि (!pmu_dev->fw_गणक_मेलनम्_encoding(phs->hartid,
							    cidx - अङ्कः_hw_ctrs,
							    edata))
				अनुवर्त;
		}

		प्रत्यागम cidx;
	}

	प्रत्यागम SBI_ENOTSUPP;
}

पूर्णाङ्क sbi_pmu_ctr_cfg_मेलनम्(अचिह्नित दीर्घ cidx_आधारः, अचिह्नित दीर्घ cidx_आवरणम्,
			  अचिह्नित दीर्घ flags, अचिह्नित दीर्घ event_idx,
			  uint64_t event_दत्त)
{
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	यदि (unlikely(!phs))
		प्रत्यागम SBI_EINVAL;

	पूर्णाङ्क ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_संहिता;

	यदि (!pmu_ctr_idx_validate(cidx_आधारः, cidx_आवरणम्))
		प्रत्यागम SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_दत्त);
	यदि (event_type < 0)
		प्रत्यागम SBI_EINVAL;
	event_संहिता = get_cidx_संहिता(event_idx);

	यदि (flags & SBI_PMU_CFG_FLAG_SKIP_मेलनम्) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		अचिह्नित दीर्घ cidx_first = cidx_आधारः + sbi_ffs(cidx_आवरणम्);

		यदि (cidx_first >= योग_ctrs)
			प्रत्यागम SBI_EINVAL;

		यदि (phs->सक्रिय_events[cidx_first] == SBI_PMU_EVENT_IDX_INVALID)
			प्रत्यागम SBI_EINVAL;
		ctr_idx = cidx_first;
		गच्छ skip_मेलनम्;
	}

	यदि (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_find_fw(phs, cidx_आधारः, cidx_आवरणम्,
					  event_संहिता, event_दत्त);
		यदि ((event_संहिता == SBI_PMU_FW_PLATFORM) && (ctr_idx >= अङ्कः_hw_ctrs))
			phs->fw_counters_दत्त[ctr_idx - अङ्कः_hw_ctrs] =
								event_दत्त;
	} अन्यथा {
		ctr_idx = pmu_ctr_find_hw(phs, cidx_आधारः, cidx_आवरणम्, flags,
					  event_idx, event_दत्त);
		यदि (ctr_idx >= 0) {
			संरचना sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_दत्त = event_दत्त;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_आवरणम्;
		}
	}

	यदि (ctr_idx < 0)
		प्रत्यागम SBI_ENOTSUPP;

	phs->सक्रिय_events[ctr_idx] = event_idx;
skip_मेलनम्:
	यदि (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		यदि (flags & SBI_PMU_CFG_FLAG_CLEAR_मानम्)
			pmu_ctr_write_hw(ctr_idx, 0);
		यदि (flags & SBI_PMU_CFG_FLAG_AUTO_START)
			pmu_ctr_start_hw(ctr_idx, 0, false);
	} अन्यथा यदि (event_type == SBI_PMU_EVENT_TYPE_FW) {
		यदि (flags & SBI_PMU_CFG_FLAG_CLEAR_मानम्)
			phs->fw_counters_दत्त[ctr_idx - अङ्कः_hw_ctrs] = 0;
		यदि (flags & SBI_PMU_CFG_FLAG_AUTO_START) {
			यदि (SBI_PMU_FW_PLATFORM == event_संहिता &&
			    pmu_dev && pmu_dev->fw_गणक_start) {
				ret = pmu_dev->fw_गणक_start(
					phs->hartid,
					ctr_idx - अङ्कः_hw_ctrs, event_दत्त);
				यदि (ret)
					प्रत्यागम ret;
			}
			phs->fw_counters_started |= BIT(ctr_idx - अङ्कः_hw_ctrs);
		}
	}

	प्रत्यागम ctr_idx;
}

पूर्णाङ्क sbi_pmu_ctr_incr_fw(गणना sbi_pmu_fw_event_संहिता_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = रिक्त;
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	यदि (unlikely(!phs))
		प्रत्यागम 0;

	यदि (likely(!phs->fw_counters_started))
		प्रत्यागम 0;

	यदि (unlikely(fw_id >= SBI_PMU_FW_MAX))
		प्रत्यागम SBI_EINVAL;

	प्रति (cidx = अङ्कः_hw_ctrs; cidx < योग_ctrs; cidx++) {
		यदि (get_cidx_संहिता(phs->सक्रिय_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & BIT(cidx - अङ्कः_hw_ctrs))) {
			fcounter = &phs->fw_counters_दत्त[cidx - अङ्कः_hw_ctrs];
			भङ्ग;
		}
	}

	यदि (fcounter)
		(*fcounter)++;

	प्रत्यागम 0;
}

अचिह्नित दीर्घ sbi_pmu_अङ्कः_ctr(शून्य)
{
	प्रत्यागम (अङ्कः_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

पूर्णाङ्क sbi_pmu_ctr_get_info(uint32_t cidx, अचिह्नित दीर्घ *ctr_info)
{
	पूर्णाङ्क विस्तारः;
	सङ्घ sbi_pmu_ctr_info cinfo = {0};
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	अचिह्नित दीर्घ गणक_आवरणम् = (अचिह्नित दीर्घ)sbi_hart_mhpm_आवरणम्(scratch) |
				     SBI_PMU_CY_IR_आवरणम्;

	/* Sanity check */
	यदि (cidx >= योग_ctrs)
		प्रत्यागम SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	यदि (cidx < अङ्कः_hw_ctrs) {
		यदि (!(__test_bit(cidx, &गणक_आवरणम्)))
			प्रत्यागम SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_आवृत्तिः + cidx;
		/* mcycle & minstret are always 64 bit */
		यदि (cidx == 0 || cidx == 2)
			cinfo.विस्तारः = 63;
		अन्यथा
			cinfo.विस्तारः = sbi_hart_mhpm_द्वयङ्काः(scratch) - 1;
	} अन्यथा {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.विस्तारः = 63;
		यदि (pmu_dev && pmu_dev->fw_गणक_विस्तारः) {
			विस्तारः = pmu_dev->fw_गणक_विस्तारः();
			यदि (विस्तारः)
				cinfo.विस्तारः = विस्तारः - 1;
		}
	}

	*ctr_info = cinfo.मानम्;

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_pmu_event_get_info(अचिह्नित दीर्घ shmem_phys_lo, अचिह्नित दीर्घ shmem_phys_hi,
			   अचिह्नित दीर्घ अङ्कः_events, अचिह्नित दीर्घ flags)
{
	अचिह्नित दीर्घ shmem_आकार = अङ्कः_events * परिमाण(संरचना sbi_pmu_event_info);
	पूर्णाङ्क i, j, event_type;
	संरचना sbi_pmu_event_info *einfo;
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	संरचना sbi_pmu_hw_event *अस्थायी;
	bool प्राप्तम् = false;

	यदि (flags != 0)
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	यदि (!अङ्कः_events || (shmem_phys_lo & 0xF))
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	यदि (shmem_phys_hi)
		प्रत्यागम SBI_EINVALID_ADDR;

	यदि (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_आकार, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		प्रत्यागम SBI_ERR_INVALID_स्थानाङ्कः;

	sbi_hart_protection_अस्थायी_map_range(shmem_phys_lo, shmem_आकार);

	einfo = (संरचना sbi_pmu_event_info *)(shmem_phys_lo);
	प्रति (i = 0; i < अङ्कः_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		यदि (event_idx & SBI_PMU_EVENT_IDX_MBZ_आवरणम्)
			प्रत्यागम SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_दत्त);
		यदि (event_type < 0) {
			einfo[i].output = 0;
		} अन्यथा यदि (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} अन्यथा {
			प्रति (j = 0; j < अङ्कः_hw_events; j++) {
				अस्थायी = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				यदि (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					यदि (अस्थायी->start_idx > event_idx ||
					    event_idx > अस्थायी->अन्तः_idx)
						अनुवर्त;
					/* just match the selector */
					यदि (अस्थायी->select == (einfo[i].event_दत्त &
									अस्थायी->select_आवरणम्)) {
						प्राप्तम् = true;
						भङ्ग;
					}
				} अन्यथा यदि (अस्थायी->start_idx <= event_idx &&
					   event_idx <= अस्थायी->अन्तः_idx) {
					प्राप्तम् = true;
					भङ्ग;
				}
			}
			यदि (प्राप्तम्)
				einfo[i].output = 1;
			अन्यथा
				einfo[i].output = 0;
			प्राप्तम् = false;
		}
	}

	sbi_hart_protection_अस्थायी_unmap_range(shmem_phys_lo, shmem_आकार);

	प्रत्यागम 0;
}

स्थिर शून्य pmu_reset_event_map(संरचना sbi_pmu_hart_state *phs)
{
	पूर्णाङ्क j;

	/* Initialize the counter to event mapping table */
	प्रति (j = 3; j < योग_ctrs; j++)
		phs->सक्रिय_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	प्रति (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_दत्त[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

ध्रुव संरचना sbi_pmu_device *sbi_pmu_get_device(शून्य)
{
	प्रत्यागम pmu_dev;
}

शून्य sbi_pmu_समुच्चयः_device(ध्रुव संरचना sbi_pmu_device *dev)
{
	यदि (!dev || pmu_dev)
		प्रत्यागम;

	pmu_dev = dev;
}

शून्य sbi_pmu_exit(संरचना sbi_scratch *scratch)
{
	संरचना sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	यदि (sbi_hart_priv_संस्करणम्(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	यदि (sbi_hart_priv_संस्करणम्(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	यदि (unlikely(!phs))
		प्रत्यागम;

	pmu_reset_event_map(phs);
}

स्थिर शून्य pmu_sse_enable(uint32_t event_id)
{
	अचिह्नित दीर्घ irq_आवरणम् = sbi_pmu_irq_आवरणम्();

	csr_समुच्चयः(CSR_MIE, irq_आवरणम्);
}

स्थिर शून्य pmu_sse_disable(uint32_t event_id)
{
	अचिह्नित दीर्घ irq_आवरणम् = sbi_pmu_irq_आवरणम्();

	csr_clear(CSR_MIE, irq_आवरणम्);
	csr_clear(CSR_MIP, irq_आवरणम्);
}

स्थिर शून्य pmu_sse_complete(uint32_t event_id)
{
	csr_समुच्चयः(CSR_MIE, sbi_pmu_irq_आवरणम्());
}

स्थिर शून्य pmu_sse_पञ्जिकाकोशः(uint32_t event_id)
{
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_आवरणम्());
}

स्थिर शून्य pmu_sse_unregister(uint32_t event_id)
{
	संरचना sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_समुच्चयः(CSR_MIDELEG, sbi_pmu_irq_आवरणम्());
}

स्थिर ध्रुव संरचना sbi_sse_cb_ops pmu_sse_cb_ops = {
	.पञ्जिकाकोशः_cb = pmu_sse_पञ्जिकाकोशः,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

पूर्णाङ्क sbi_pmu_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	पूर्णाङ्क hpm_गणनम् = sbi_fls(sbi_hart_mhpm_आवरणम्(scratch));
	संरचना sbi_pmu_hart_state *phs;
	ध्रुव संरचना sbi_platform *plat;
	पूर्णाङ्क rc;

	यदि (cold_boot) {
		hw_event_map = sbi_calloc(परिमाण(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		यदि (!hw_event_map)
			प्रत्यागम SBI_ENOMEM;

		phs_ptr_विस्थापनम् = sbi_scratch_alloc_type_विस्थापनम्(शून्य *);
		यदि (!phs_ptr_विस्थापनम्) {
			sbi_free(hw_event_map);
			प्रत्यागम SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		यदि (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		यदि (!hpm_गणनम्)
			/* Only CY, TM & IR are implemented in the hw */
			अङ्कः_hw_ctrs = 3;
		अन्यथा
			अङ्कः_hw_ctrs = hpm_गणनम् + 1;

		यदि (अङ्कः_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			प्रत्यागम SBI_EINVAL;

		योग_ctrs = अङ्कः_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		यदि (sbi_pmu_irq_bit() >= 0)
			sbi_sse_सङ्कलय_event(SBI_SSE_EVENT_LOCAL_PMU_अतिप्रवाहः, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	यदि (!phs) {
		phs = sbi_zalloc(परिमाण(*phs));
		यदि (!phs)
			प्रत्यागम SBI_ENOMEM;
		phs->hartid = current_hartid();
		pmu_समुच्चयः_hart_state_ptr(scratch, phs);
	}

	pmu_reset_event_map(phs);

	/* First three counters are fixed by the priv spec and we enable it by default */
	phs->सक्रिय_events[0] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_विस्थापनम्) |
				SBI_PMU_HW_CPU_आवृत्तयः;
	phs->सक्रिय_events[1] = SBI_PMU_EVENT_IDX_INVALID;
	phs->सक्रिय_events[2] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_विस्थापनम्) |
				SBI_PMU_HW_INSTRUCTIONS;

	प्रत्यागम 0;
}
