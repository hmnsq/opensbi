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
	uint32_t شروع_idx;
	uint32_t سرا_idx;
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
	بنانشانی لما قدر;
	بنية {
		بنانشانی لما csr:12;
		بنانشانی لما چوڑائی:6;
#إذا __riscv_xlen == 32
		بنانشانی لما reserved:13;
#وإلا
		بنانشانی لما reserved:45;
#جے_انت
		بنانشانی لما type:1;
	};
};

#إذا SBI_PMU_FW_CTR_MAX >= بٹ_فی_LONG
#غلطی "Can't handle firmware counters beyond BITS_PER_LONG"
#جے_انت

/** HW event configuration parameters */
بنية sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_انکڑے;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
بنية sbi_pmu_hart_state {
	/* HART to which this state belongs */
	uint32_t hartid;
	/* Counter to enabled event mapping */
	uint32_t سرگرم_events[SBI_PMU_HW_CTR_MAX + SBI_PMU_FW_CTR_MAX];
	/* Bitmap of firmware counters started */
	بنانشانی لما fw_counters_started;
	/* if true, SSE is enabled */
	bool sse_enabled;
	/*
	 * Counter values for SBI firmware events and event codes
	 * for platform firmware events. Both are mutually exclusive
	 * and hence can optimally share the same memory.
	 */
	uint64_t fw_counters_انکڑے[SBI_PMU_FW_CTR_MAX];
	/* HW events configuration parameters from
	 * sbi_pmu_ctr_cfg_match() command which are
	 * used for restoring RAW hardware events after
	 * cpu suspending.
	 */
	بنية sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
ساكن بنانشانی لما phs_ptr_وتھ;

#تعریف_کرو pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_وتھ ? sbi_scratch_read_type((__scratch), فراغ *, phs_ptr_وتھ) : عدم

#تعریف_کرو pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#تعریف_کرو pmu_سیٹ_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), فراغ *, phs_ptr_وتھ, (__phs))

/* Platform specific PMU device */
ساكن ثابت بنية sbi_pmu_device *pmu_dev = عدم;

/* Mapping between event range and possible counters  */
ساكن بنية sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
ساكن uint32_t عدد_hw_events;
/* Maximum number of hardware counters available */
ساكن uint32_t عدد_hw_ctrs;

/* Maximum number of counters available */
ساكن uint32_t کل_ctrs;

/* Helper macros to retrieve event idx and code type */
#تعریف_کرو get_cidx_type(x) \
  (((x) & SBI_PMU_EVENT_IDX_TYPE_نقاب) >> SBI_PMU_EVENT_IDX_TYPE_وتھ)
#تعریف_کرو get_cidx_کوڈ(x) (x & SBI_PMU_EVENT_IDX_کوڈ_نقاب)

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
	إذا (((evtA->سرا_idx < evtB->شروع_idx) && (evtA->سرا_idx < evtB->سرا_idx)) ||
	   ((evtA->شروع_idx > evtB->شروع_idx) && (evtA->شروع_idx > evtB->سرا_idx)))
		أرجع false;
	أرجع true;
}

ساكن bool pmu_event_select_overlap(بنية sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_نقاب)
{
	إذا ((evt->select == select_val) && (evt->select_نقاب == select_نقاب))
		أرجع true;

	أرجع false;
}

ساكن صحيح pmu_event_validate(بنية sbi_pmu_hart_state *phs,
			      بنانشانی لما event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_کوڈ = get_cidx_کوڈ(event_idx);
	uint32_t event_idx_کوڈ_max = -1;
	uint32_t cache_ops_نتیجہ, cache_ops_id, cache_id;

	بدل(event_idx_type) {
	حالة SBI_PMU_EVENT_TYPE_HW:
		event_idx_کوڈ_max = SBI_PMU_HW_GENERAL_MAX;
		اكسر;
	حالة SBI_PMU_EVENT_TYPE_FW:
		إذا ((event_idx_کوڈ >= SBI_PMU_FW_MAX &&
		    event_idx_کوڈ <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_کوڈ > SBI_PMU_FW_PLATFORM)
			أرجع SBI_EINVAL;

		إذا (SBI_PMU_FW_PLATFORM == event_idx_کوڈ &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			أرجع pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		وإلا
			event_idx_کوڈ_max = SBI_PMU_FW_MAX;
		اكسر;
	حالة SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_نتیجہ = event_idx_کوڈ &
					SBI_PMU_EVENT_HW_CACHE_OPS_نتیجہ;
		cache_ops_id = (event_idx_کوڈ &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_نقاب) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_وتھ;
		cache_id = (event_idx_کوڈ &
			    SBI_PMU_EVENT_HW_CACHE_ID_نقاب) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_وتھ;
		إذا ((cache_ops_نتیجہ < SBI_PMU_HW_CACHE_نتیجہ_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			أرجع event_idx_type;
		وإلا
			أرجع SBI_EINVAL;
		اكسر;
	حالة SBI_PMU_EVENT_TYPE_HW_RAW:
	حالة SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_کوڈ_max = 1; // event_idx.code should be zero
		اكسر;
	افتراضي:
		أرجع SBI_EINVAL;
	}

	إذا (event_idx_کوڈ < event_idx_کوڈ_max)
		أرجع event_idx_type;

	أرجع SBI_EINVAL;
}

ساكن صحيح pmu_ctr_validate(بنية sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_کوڈ)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	إذا (cidx >= کل_ctrs)
		أرجع SBI_EINVAL;

	event_idx_val = phs->سرگرم_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	إذا (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		أرجع SBI_EINVAL;

	*event_idx_کوڈ = get_cidx_کوڈ(event_idx_val);

	أرجع event_idx_type;
}

ساكن bool pmu_ctr_idx_validate(بنانشانی لما cbase, بنانشانی لما cmask)
{
	بنانشانی لما آخری;

	إذا (!cmask)
		أرجع false;

	آخری = sbi_fls(cmask);
	إذا (cbase > -1UL - آخری)
		أرجع false;

	أرجع (cbase + آخری) < کل_ctrs;
}

صحيح sbi_pmu_ctr_fw_read(بنانشانی لما cidx, uint64_t *cval, bool بلند_بٹ)
{
	صحيح event_idx_type;
	uint32_t event_کوڈ;
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	إذا (unlikely(!phs))
		أرجع SBI_EINVAL;

	إذا (cidx < عدد_hw_ctrs || cidx >= کل_ctrs)
		أرجع SBI_EINVAL;

#إذا __riscv_xlen > 32
	إذا (بلند_بٹ)
		أرجع 0;
#جے_انت

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_کوڈ);
	إذا (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		أرجع SBI_EINVAL;

	إذا ((event_کوڈ >= SBI_PMU_FW_MAX &&
	    event_کوڈ <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_کوڈ > SBI_PMU_FW_PLATFORM)
		أرجع SBI_EINVAL;

	إذا (SBI_PMU_FW_PLATFORM == event_کوڈ) {
		إذا (pmu_dev && pmu_dev->fw_counter_read_قدر)
			*cval = pmu_dev->fw_counter_read_قدر(phs->hartid,
							       cidx -
							       عدد_hw_ctrs);
		وإلا
			*cval = 0;
	} وإلا
		*cval = phs->fw_counters_انکڑے[cidx - عدد_hw_ctrs];

	أرجع 0;
}

ساكن صحيح pmu_جوڑو_hw_event_map(u32 eidx_شروع, u32 eidx_سرا, u32 cmap,
				uint64_t select, uint64_t select_نقاب)
{
	صحيح i = 0;
	bool is_overlap;
	بنية sbi_pmu_hw_event *event = &hw_event_map[عدد_hw_events];
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_نقاب = sbi_hart_mhpm_نقاب(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	إذا (eidx_شروع > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_نقاب))
		أرجع SBI_EDENIED;

	إذا (عدد_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		أرجع SBI_EFAIL;
	}

	event->شروع_idx = eidx_شروع;
	event->سرا_idx = eidx_سرا;

	/* Sanity check */
	لكل (i = 0; i < عدد_hw_events; i++) {
		إذا (eidx_شروع == SBI_PMU_EVENT_RAW_IDX ||
			eidx_شروع == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_نقاب);
		وإلا
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		إذا (is_overlap)
			اذهب reset_event;
	}

	event->select_نقاب = select_نقاب;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_نقاب;
	event->select = select;
	عدد_hw_events++;

	أرجع 0;

reset_event:
	event->شروع_idx = 0;
	event->سرا_idx = 0;
	أرجع SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
صحيح sbi_pmu_جوڑو_hw_event_counter_map(u32 eidx_شروع, u32 eidx_سرا, u32 cmap)
{
	إذا ((eidx_شروع > eidx_سرا) || eidx_شروع >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_سرا >= SBI_PMU_EVENT_RAW_IDX)
		أرجع SBI_EINVAL;

	أرجع pmu_جوڑو_hw_event_map(eidx_شروع, eidx_سرا, cmap, 0, 0);
}

صحيح sbi_pmu_جوڑو_raw_event_counter_map(uint64_t select, uint64_t select_نقاب, u32 cmap)
{
	أرجع pmu_جوڑو_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_نقاب);
}

فراغ sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_نقاب());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_حد_توں_ودھ);
}

ساكن صحيح pmu_ctr_enable_irq_hw(صحيح ctr_idx)
{
	بنانشانی لما mhpmevent_csr;
	بنانشانی لما mhpmevent_curr;
	بنانشانی لما mip_val;
	بنانشانی لما دا_نقاب;

	إذا (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		أرجع SBI_EFAIL;

#إذا __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	دا_نقاب = (uint32_t)~MHPMEVENTH_دا;
#وإلا
	mhpmevent_csr = CSR_MHPMEVENT3 + ctr_idx - 3;
	دا_نقاب = ~MHPMEVENT_دا;
#جے_انت

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
	إذا (!(mip_val & sbi_pmu_irq_نقاب())) {
		mhpmevent_curr &= دا_نقاب;
		csr_write_عدد(mhpmevent_csr, mhpmevent_curr);
	}

	أرجع 0;
}

ساكن فراغ pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#إذا __riscv_xlen == 32
	csr_write_عدد(CSR_MCYCLE + cidx, 0);
	csr_write_عدد(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_عدد(CSR_MCYCLEH + cidx, ival >> بٹ_فی_LONG);
#وإلا
	csr_write_عدد(CSR_MCYCLE + cidx, ival);
#جے_انت
}

ساكن صحيح pmu_ctr_شروع_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنانشانی لما mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	إذا (cidx >= عدد_hw_ctrs || cidx == 1)
		أرجع SBI_EINVAL;

	إذا (sbi_hart_priv_نسخہ(scratch) < SBI_HART_PRIV_VER_1_11) {
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

صحيح sbi_pmu_irq_ثنائی_ہندسہ(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		أرجع IRQ_PMU_OVF;
	إذا (pmu_dev && pmu_dev->hw_counter_irq_ثنائی_ہندسہ)
		أرجع pmu_dev->hw_counter_irq_ثنائی_ہندسہ();

	أرجع -1;
}

بنانشانی لما sbi_pmu_irq_نقاب(فراغ)
{
	صحيح irq_ثنائی_ہندسہ = sbi_pmu_irq_ثنائی_ہندسہ();

	إذا (irq_ثنائی_ہندسہ < 0)
		أرجع 0;

	أرجع ثنائی_ہندسہ(irq_ثنائی_ہندسہ);
}

ساكن صحيح pmu_ctr_شروع_fw(بنية sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_کوڈ,
			    uint64_t event_انکڑے, uint64_t ival,
			    bool ival_update)
{
	صحيح ret;

	إذا ((event_کوڈ >= SBI_PMU_FW_MAX &&
	    event_کوڈ <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_کوڈ > SBI_PMU_FW_PLATFORM)
		أرجع SBI_EINVAL;

	إذا (phs->fw_counters_started & ثنائی_ہندسہ(cidx - عدد_hw_ctrs))
		أرجع SBI_EALREADY_STARTED;

	إذا (SBI_PMU_FW_PLATFORM == event_کوڈ) {
		إذا (!pmu_dev ||
		    !pmu_dev->fw_counter_write_قدر ||
		    !pmu_dev->fw_counter_شروع) {
			أرجع SBI_EINVAL;
		}

		إذا (ival_update)
			pmu_dev->fw_counter_write_قدر(phs->hartid,
							cidx - عدد_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_شروع(phs->hartid,
						 cidx - عدد_hw_ctrs,
						 event_انکڑے);
		إذا (ret)
			أرجع ret;
	} وإلا {
		إذا (ival_update)
			phs->fw_counters_انکڑے[cidx - عدد_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= ثنائی_ہندسہ(cidx - عدد_hw_ctrs);

	أرجع 0;
}

ساكن فراغ pmu_update_inhibit_flags(بنانشانی لما flags, uint64_t *mhpmevent_val)
{
	إذا (flags & SBI_PMU_CFG_FLAG_سیٹ_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	إذا (flags & SBI_PMU_CFG_FLAG_سیٹ_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	إذا (flags & SBI_PMU_CFG_FLAG_سیٹ_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	إذا (flags & SBI_PMU_CFG_FLAG_سیٹ_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

ساكن صحيح pmu_update_hw_mhpmevent(بنية sbi_pmu_hw_event *hw_evt, صحيح ctr_idx,
				   بنانشانی لما flags, بنانشانی لما eindex,
				   uint64_t انکڑے)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_منزل_mhpmevent(plat, eindex, انکڑے);

	إذا (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		أرجع SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_نقاب) |
				 MHPMEVENT_MINH | MHPMEVENT_دا;

	إذا (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	إذا (pmu_dev && pmu_dev->hw_counter_filter_منوال)
		pmu_dev->hw_counter_filter_منوال(flags, ctr_idx);

#إذا __riscv_xlen == 32
	csr_write_عدد(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_عدد(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> بٹ_فی_LONG);
#وإلا
	csr_write_عدد(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#جے_انت

	أرجع 0;
}

صحيح sbi_pmu_ctr_شروع(بنانشانی لما cbase, بنانشانی لما cmask,
		      بنانشانی لما flags, uint64_t ival)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	إذا (unlikely(!phs))
		أرجع SBI_EINVAL;

	صحيح event_idx_type;
	uint32_t event_کوڈ;
	صحيح ret = SBI_EINVAL;
	bool bUpdate = false;
	صحيح i, cidx;
	uint64_t edata;

	إذا (!pmu_ctr_idx_validate(cbase, cmask))
		أرجع ret;

	إذا (flags & ~SBI_PMU_شروع_FLAGS_نقاب)
		أرجع SBI_ERR_INVALID_PARAM;

	إذا (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		أرجع SBI_ENO_SHMEM;

	إذا (flags & SBI_PMU_شروع_FLAG_سیٹ_INIT_قدر)
		bUpdate = true;

	for_each_سیٹ_ثنائی_ہندسہ(i, &cmask, بٹ_فی_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_کوڈ);
		إذا (event_idx_type < 0)
			/* Continue the start operation for other counters */
			استمر;
		وإلا إذا (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_کوڈ == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_انکڑے[cidx - عدد_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_شروع_fw(phs, cidx, event_کوڈ, edata,
					       ival, bUpdate);
			إذا (ret)
				أرجع ret;
		} وإلا {
			إذا (cidx >= 3) {
				بنية sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->سرگرم_events[cidx],
							ev_cfg->event_انکڑے);
				إذا (ret)
					أرجع ret;
			}
			ret = pmu_ctr_شروع_hw(cidx, ival, bUpdate);
			إذا (ret)
				أرجع ret;
		}
	}

	أرجع ret;
}

ساكن صحيح pmu_ctr_stop_hw(uint32_t cidx)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنانشانی لما mctr_inhbt;

	إذا (sbi_hart_priv_نسخہ(scratch) < SBI_HART_PRIV_VER_1_11)
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
			   uint32_t cidx, uint32_t event_کوڈ)
{
	صحيح ret;

	إذا ((event_کوڈ >= SBI_PMU_FW_MAX &&
	    event_کوڈ <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_کوڈ > SBI_PMU_FW_PLATFORM)
		أرجع SBI_EINVAL;

	إذا (!(phs->fw_counters_started & ثنائی_ہندسہ(cidx - عدد_hw_ctrs)))
		أرجع SBI_EALREADY_STOPPED;

	إذا (SBI_PMU_FW_PLATFORM == event_کوڈ &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - عدد_hw_ctrs);
		إذا (ret)
			أرجع ret;
	}

	phs->fw_counters_started &= ~ثنائی_ہندسہ(cidx - عدد_hw_ctrs);

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
#جے_انت

	أرجع 0;
}

صحيح sbi_pmu_ctr_stop(بنانشانی لما cbase, بنانشانی لما cmask,
		     بنانشانی لما flag)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	إذا (unlikely(!phs))
		أرجع SBI_EINVAL;

	صحيح ret = SBI_EINVAL;
	صحيح event_idx_type;
	uint32_t event_کوڈ;
	صحيح i, cidx;

	إذا (!pmu_ctr_idx_validate(cbase, cmask))
		أرجع ret;

	إذا (flag & ~SBI_PMU_STOP_FLAGS_نقاب)
		أرجع SBI_ERR_INVALID_PARAM;

	إذا (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		أرجع SBI_ENO_SHMEM;

	for_each_سیٹ_ثنائی_ہندسہ(i, &cmask, بٹ_فی_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_کوڈ);
		إذا (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			استمر;

		وإلا إذا (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_کوڈ);
		وإلا
			ret = pmu_ctr_stop_hw(cidx);

		إذا(ret)
			أرجع ret;

		إذا (cidx > (CSR_INSTRET - CSR_چکر) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->سرگرم_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	إذا (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_نقاب());

	أرجع ret;
}

ساكن صحيح pmu_fixed_ctr_update_inhibit_بٹ(صحيح fixed_ctr, بنانشانی لما flags)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#إذا __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#جے_انت
	إذا (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_منوال))
		أرجع fixed_ctr;

	بدل (fixed_ctr) {
	حالة 0:
		cfg_csr_no = CSR_MCYCLECFG;
#إذا __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#جے_انت
		اكسر;
	حالة 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#إذا __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#جے_انت
		اكسر;
	افتراضي:
		أرجع SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#إذا __riscv_xlen == 32
		csr_write_عدد(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_عدد(cfgh_csr_no, cfg_val >> بٹ_فی_LONG);
#وإلا
		csr_write_عدد(cfg_csr_no, cfg_val);
#جے_انت
	}
	إذا (pmu_dev && pmu_dev->hw_counter_filter_منوال)
		pmu_dev->hw_counter_filter_منوال(flags, fixed_ctr);
	أرجع fixed_ctr;
}

ساكن صحيح pmu_ctr_لبھو_fixed_hw(بنانشانی لما evt_idx_کوڈ)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	إذا (evt_idx_کوڈ == SBI_PMU_HW_CPU_چکراں)
		أرجع 0;
	وإلا إذا (evt_idx_کوڈ == SBI_PMU_HW_INSTRUCTIONS)
		أرجع 2;
	وإلا
		أرجع SBI_EINVAL;
}

ساكن صحيح pmu_ctr_لبھو_hw(بنية sbi_pmu_hart_state *phs,
			   بنانشانی لما cbase, بنانشانی لما cmask,
			   بنانشانی لما flags,
			   بنانشانی لما event_idx, uint64_t انکڑے)
{
	بنانشانی لما ctr_نقاب;
	صحيح i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	بنية sbi_pmu_hw_event *عارضی;
	بنانشانی لما mctr_inhbt = 0;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	إذا (cbase >= عدد_hw_ctrs)
		أرجع SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_لبھو_fixed_hw(event_idx);
	إذا (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		أرجع pmu_fixed_ctr_update_inhibit_بٹ(fixed_ctr, flags);

	إذا (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	لكل (i = 0; i < عدد_hw_events; i++) {
		عارضی = &hw_event_map[i];
		إذا ((عارضی->شروع_idx > event_idx && event_idx < عارضی->سرا_idx) ||
		    (عارضی->شروع_idx < event_idx && event_idx > عارضی->سرا_idx))
			استمر;

		/* For raw events, event data is used as the select value */
		إذا (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_نقاب = عارضی->select_نقاب;

			/* The non-event map bits of data should match the selector */
			إذا (عارضی->select != (انکڑے & select_نقاب))
				استمر;
		}
		/* Fixed counters should not be part of the search */
		ctr_نقاب = عارضی->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_نقاب);
		for_each_سیٹ_ثنائی_ہندسہ_from(cbase, &ctr_نقاب, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			إذا (phs->سرگرم_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				استمر;
			/* If mcountinhibit is supported, the bit must be enabled */
			إذا ((sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_11) &&
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

		أرجع pmu_fixed_ctr_update_inhibit_بٹ(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(عارضی, ctr_idx, flags, event_idx, انکڑے);

	إذا (!ret)
		ret = ctr_idx;

	أرجع ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
ساكن صحيح pmu_ctr_لبھو_fw(بنية sbi_pmu_hart_state *phs,
			   بنانشانی لما cbase, بنانشانی لما cmask,
			   uint32_t event_کوڈ, uint64_t edata)
{
	صحيح i, cidx;

	for_each_سیٹ_ثنائی_ہندسہ(i, &cmask, بٹ_فی_LONG) {
		cidx = i + cbase;
		إذا (cidx < عدد_hw_ctrs || کل_ctrs <= cidx)
			استمر;
		إذا (phs->سرگرم_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			استمر;
		إذا (SBI_PMU_FW_PLATFORM == event_کوڈ &&
		    pmu_dev && pmu_dev->fw_counter_میل_encoding) {
			إذا (!pmu_dev->fw_counter_میل_encoding(phs->hartid,
							    cidx - عدد_hw_ctrs,
							    edata))
				استمر;
		}

		أرجع cidx;
	}

	أرجع SBI_ENOTSUPP;
}

صحيح sbi_pmu_ctr_cfg_میل(بنانشانی لما cidx_ادھار, بنانشانی لما cidx_نقاب,
			  بنانشانی لما flags, بنانشانی لما event_idx,
			  uint64_t event_انکڑے)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	إذا (unlikely(!phs))
		أرجع SBI_EINVAL;

	صحيح ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_کوڈ;

	إذا (!pmu_ctr_idx_validate(cidx_ادھار, cidx_نقاب))
		أرجع SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_انکڑے);
	إذا (event_type < 0)
		أرجع SBI_EINVAL;
	event_کوڈ = get_cidx_کوڈ(event_idx);

	إذا (flags & SBI_PMU_CFG_FLAG_SKIP_میل) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		بنانشانی لما cidx_پہلا = cidx_ادھار + sbi_ffs(cidx_نقاب);

		إذا (cidx_پہلا >= کل_ctrs)
			أرجع SBI_EINVAL;

		إذا (phs->سرگرم_events[cidx_پہلا] == SBI_PMU_EVENT_IDX_INVALID)
			أرجع SBI_EINVAL;
		ctr_idx = cidx_پہلا;
		اذهب skip_میل;
	}

	إذا (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_لبھو_fw(phs, cidx_ادھار, cidx_نقاب,
					  event_کوڈ, event_انکڑے);
		إذا ((event_کوڈ == SBI_PMU_FW_PLATFORM) && (ctr_idx >= عدد_hw_ctrs))
			phs->fw_counters_انکڑے[ctr_idx - عدد_hw_ctrs] =
								event_انکڑے;
	} وإلا {
		ctr_idx = pmu_ctr_لبھو_hw(phs, cidx_ادھار, cidx_نقاب, flags,
					  event_idx, event_انکڑے);
		إذا (ctr_idx >= 0) {
			بنية sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_انکڑے = event_انکڑے;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_نقاب;
		}
	}

	إذا (ctr_idx < 0)
		أرجع SBI_ENOTSUPP;

	phs->سرگرم_events[ctr_idx] = event_idx;
skip_میل:
	إذا (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		إذا (flags & SBI_PMU_CFG_FLAG_CLEAR_قدر)
			pmu_ctr_write_hw(ctr_idx, 0);
		إذا (flags & SBI_PMU_CFG_FLAG_AUTO_شروع)
			pmu_ctr_شروع_hw(ctr_idx, 0, false);
	} وإلا إذا (event_type == SBI_PMU_EVENT_TYPE_FW) {
		إذا (flags & SBI_PMU_CFG_FLAG_CLEAR_قدر)
			phs->fw_counters_انکڑے[ctr_idx - عدد_hw_ctrs] = 0;
		إذا (flags & SBI_PMU_CFG_FLAG_AUTO_شروع) {
			إذا (SBI_PMU_FW_PLATFORM == event_کوڈ &&
			    pmu_dev && pmu_dev->fw_counter_شروع) {
				ret = pmu_dev->fw_counter_شروع(
					phs->hartid,
					ctr_idx - عدد_hw_ctrs, event_انکڑے);
				إذا (ret)
					أرجع ret;
			}
			phs->fw_counters_started |= ثنائی_ہندسہ(ctr_idx - عدد_hw_ctrs);
		}
	}

	أرجع ctr_idx;
}

صحيح sbi_pmu_ctr_incr_fw(تعداد sbi_pmu_fw_event_کوڈ_id fw_id)
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

	لكل (cidx = عدد_hw_ctrs; cidx < کل_ctrs; cidx++) {
		إذا (get_cidx_کوڈ(phs->سرگرم_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & ثنائی_ہندسہ(cidx - عدد_hw_ctrs))) {
			fcounter = &phs->fw_counters_انکڑے[cidx - عدد_hw_ctrs];
			اكسر;
		}
	}

	إذا (fcounter)
		(*fcounter)++;

	أرجع 0;
}

بنانشانی لما sbi_pmu_عدد_ctr(فراغ)
{
	أرجع (عدد_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

صحيح sbi_pmu_ctr_get_info(uint32_t cidx, بنانشانی لما *ctr_info)
{
	صحيح چوڑائی;
	اتحاد sbi_pmu_ctr_info cinfo = {0};
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنانشانی لما counter_نقاب = (بنانشانی لما)sbi_hart_mhpm_نقاب(scratch) |
				     SBI_PMU_CY_IR_نقاب;

	/* Sanity check */
	إذا (cidx >= کل_ctrs)
		أرجع SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	إذا (cidx < عدد_hw_ctrs) {
		إذا (!(__test_bit(cidx, &counter_نقاب)))
			أرجع SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_چکر + cidx;
		/* mcycle & minstret are always 64 bit */
		إذا (cidx == 0 || cidx == 2)
			cinfo.چوڑائی = 63;
		وإلا
			cinfo.چوڑائی = sbi_hart_mhpm_بٹ(scratch) - 1;
	} وإلا {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.چوڑائی = 63;
		إذا (pmu_dev && pmu_dev->fw_counter_چوڑائی) {
			چوڑائی = pmu_dev->fw_counter_چوڑائی();
			إذا (چوڑائی)
				cinfo.چوڑائی = چوڑائی - 1;
		}
	}

	*ctr_info = cinfo.قدر;

	أرجع 0;
}

صحيح sbi_pmu_event_get_info(بنانشانی لما shmem_phys_lo, بنانشانی لما shmem_phys_hi,
			   بنانشانی لما عدد_events, بنانشانی لما flags)
{
	بنانشانی لما shmem_ناپ = عدد_events * حجم(بنية sbi_pmu_event_info);
	صحيح i, j, event_type;
	بنية sbi_pmu_event_info *einfo;
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	بنية sbi_pmu_hw_event *عارضی;
	bool لبھیا = false;

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
					 shmem_phys_lo, shmem_ناپ, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		أرجع SBI_ERR_INVALID_پتہ;

	sbi_hart_protection_عارضی_map_range(shmem_phys_lo, shmem_ناپ);

	einfo = (بنية sbi_pmu_event_info *)(shmem_phys_lo);
	لكل (i = 0; i < عدد_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		إذا (event_idx & SBI_PMU_EVENT_IDX_MBZ_نقاب)
			أرجع SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_انکڑے);
		إذا (event_type < 0) {
			einfo[i].output = 0;
		} وإلا إذا (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} وإلا {
			لكل (j = 0; j < عدد_hw_events; j++) {
				عارضی = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				إذا (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					إذا (عارضی->شروع_idx > event_idx ||
					    event_idx > عارضی->سرا_idx)
						استمر;
					/* just match the selector */
					إذا (عارضی->select == (einfo[i].event_انکڑے &
									عارضی->select_نقاب)) {
						لبھیا = true;
						اكسر;
					}
				} وإلا إذا (عارضی->شروع_idx <= event_idx &&
					   event_idx <= عارضی->سرا_idx) {
					لبھیا = true;
					اكسر;
				}
			}
			إذا (لبھیا)
				einfo[i].output = 1;
			وإلا
				einfo[i].output = 0;
			لبھیا = false;
		}
	}

	sbi_hart_protection_عارضی_unmap_range(shmem_phys_lo, shmem_ناپ);

	أرجع 0;
}

ساكن فراغ pmu_reset_event_map(بنية sbi_pmu_hart_state *phs)
{
	صحيح j;

	/* Initialize the counter to event mapping table */
	لكل (j = 3; j < کل_ctrs; j++)
		phs->سرگرم_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	لكل (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_انکڑے[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

ثابت بنية sbi_pmu_device *sbi_pmu_get_device(فراغ)
{
	أرجع pmu_dev;
}

فراغ sbi_pmu_سیٹ_device(ثابت بنية sbi_pmu_device *dev)
{
	إذا (!dev || pmu_dev)
		أرجع;

	pmu_dev = dev;
}

فراغ sbi_pmu_exit(بنية sbi_scratch *scratch)
{
	بنية sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	إذا (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	إذا (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	إذا (unlikely(!phs))
		أرجع;

	pmu_reset_event_map(phs);
}

ساكن فراغ pmu_sse_enable(uint32_t event_id)
{
	بنانشانی لما irq_نقاب = sbi_pmu_irq_نقاب();

	csr_سیٹ(CSR_MIE, irq_نقاب);
}

ساكن فراغ pmu_sse_disable(uint32_t event_id)
{
	بنانشانی لما irq_نقاب = sbi_pmu_irq_نقاب();

	csr_clear(CSR_MIE, irq_نقاب);
	csr_clear(CSR_MIP, irq_نقاب);
}

ساكن فراغ pmu_sse_complete(uint32_t event_id)
{
	csr_سیٹ(CSR_MIE, sbi_pmu_irq_نقاب());
}

ساكن فراغ pmu_sse_رجسٹر(uint32_t event_id)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_نقاب());
}

ساكن فراغ pmu_sse_unregister(uint32_t event_id)
{
	بنية sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_سیٹ(CSR_MIDELEG, sbi_pmu_irq_نقاب());
}

ساكن ثابت بنية sbi_sse_cb_ops pmu_sse_cb_ops = {
	.رجسٹر_cb = pmu_sse_رجسٹر,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

صحيح sbi_pmu_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	صحيح hpm_گنتی = sbi_fls(sbi_hart_mhpm_نقاب(scratch));
	بنية sbi_pmu_hart_state *phs;
	ثابت بنية sbi_platform *plat;
	صحيح rc;

	إذا (cold_boot) {
		hw_event_map = sbi_calloc(حجم(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		إذا (!hw_event_map)
			أرجع SBI_ENOMEM;

		phs_ptr_وتھ = sbi_scratch_alloc_type_وتھ(فراغ *);
		إذا (!phs_ptr_وتھ) {
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
		إذا (!hpm_گنتی)
			/* Only CY, TM & IR are implemented in the hw */
			عدد_hw_ctrs = 3;
		وإلا
			عدد_hw_ctrs = hpm_گنتی + 1;

		إذا (عدد_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			أرجع SBI_EINVAL;

		کل_ctrs = عدد_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		إذا (sbi_pmu_irq_ثنائی_ہندسہ() >= 0)
			sbi_sse_جوڑو_event(SBI_SSE_EVENT_LOCAL_PMU_حد_توں_ودھ, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	إذا (!phs) {
		phs = sbi_zalloc(حجم(*phs));
		إذا (!phs)
			أرجع SBI_ENOMEM;
		phs->hartid = current_hartid();
		pmu_سیٹ_hart_state_ptr(scratch, phs);
	}

	pmu_reset_event_map(phs);

	/* First three counters are fixed by the priv spec and we enable it by default */
	phs->سرگرم_events[0] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_وتھ) |
				SBI_PMU_HW_CPU_چکراں;
	phs->سرگرم_events[1] = SBI_PMU_EVENT_IDX_INVALID;
	phs->سرگرم_events[2] = (SBI_PMU_EVENT_TYPE_HW << SBI_PMU_EVENT_IDX_TYPE_وتھ) |
				SBI_PMU_HW_INSTRUCTIONS;

	أرجع 0;
}
