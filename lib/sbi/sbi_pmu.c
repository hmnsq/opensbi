/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/sbi_bitops.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_hart_protection.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_pmu.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_string.h>
#include <sbi/sbi_sse.h>

/** Information about hardware counters */
sa_nracanaa sbi_pmu_hw_event {
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
sa_ngha sbi_pmu_ctr_info {
	unsigned long value;
	sa_nracanaa {
		unsigned long csr:12;
		unsigned long width:6;
#ပေႃး __riscv_xlen == 32
		unsigned long reserved:13;
#ပေႃးဢမ်ႇ
		unsigned long reserved:45;
#endif
		unsigned long type:1;
	};
};

#ပေႃး SBI_PMU_FW_CTR_MAX >= BITS_PER_LONG
#error "Can't handle firmware counters beyond BITS_PER_LONG"
#endif

/** HW event configuration parameters */
sa_nracanaa sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_data;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
sa_nracanaa sbi_pmu_hart_state {
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
	sa_nracanaa sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
};

/** Offset of pointer to PMU HART state in scratch space */
static unsigned long phs_ptr_offset;

#define pmu_get_hart_state_ptr(__scratch)				\
	phs_ptr_offset ? sbi_scratch_read_type((__scratch), void *, phs_ptr_offset) : NULL

#define pmu_thishart_state_ptr()					\
	pmu_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define pmu_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), void *, phs_ptr_offset, (__phs))

/* Platform specific PMU device */
static const sa_nracanaa sbi_pmu_device *pmu_dev = NULL;

/* Mapping between event range and possible counters  */
static sa_nracanaa sbi_pmu_hw_event *hw_event_map;

/* Maximum number of hardware events available */
static uint32_t num_hw_events;
/* Maximum number of hardware counters available */
static uint32_t num_hw_ctrs;

/* Maximum number of counters available */
static uint32_t total_ctrs;

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
static bool pmu_event_range_overlap(sa_nracanaa sbi_pmu_hw_event *evtA,
				    sa_nracanaa sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	ပေႃး (((evtA->end_idx < evtB->start_idx) && (evtA->end_idx < evtB->end_idx)) ||
	   ((evtA->start_idx > evtB->start_idx) && (evtA->start_idx > evtB->end_idx)))
		ပွၵ်ႈ false;
	ပွၵ်ႈ true;
}

static bool pmu_event_select_overlap(sa_nracanaa sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_mask)
{
	ပေႃး ((evt->select == select_val) && (evt->select_mask == select_mask))
		ပွၵ်ႈ true;

	ပွၵ်ႈ false;
}

static puur_naa_nka pmu_event_validate(sa_nracanaa sbi_pmu_hart_state *phs,
			      unsigned long event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_code = get_cidx_code(event_idx);
	uint32_t event_idx_code_max = -1;
	uint32_t cache_ops_result, cache_ops_id, cache_id;

	switch(event_idx_type) {
	pak_sa SBI_PMU_EVENT_TYPE_HW:
		event_idx_code_max = SBI_PMU_HW_GENERAL_MAX;
		bha_nga;
	pak_sa SBI_PMU_EVENT_TYPE_FW:
		ပေႃး ((event_idx_code >= SBI_PMU_FW_MAX &&
		    event_idx_code <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_code > SBI_PMU_FW_PLATFORM)
			ပွၵ်ႈ SBI_EINVAL;

		ပေႃး (SBI_PMU_FW_PLATFORM == event_idx_code &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			ပွၵ်ႈ pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		ပေႃးဢမ်ႇ
			event_idx_code_max = SBI_PMU_FW_MAX;
		bha_nga;
	pak_sa SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_result = event_idx_code &
					SBI_PMU_EVENT_HW_CACHE_OPS_RESULT;
		cache_ops_id = (event_idx_code &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_MASK) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_OFFSET;
		cache_id = (event_idx_code &
			    SBI_PMU_EVENT_HW_CACHE_ID_MASK) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_OFFSET;
		ပေႃး ((cache_ops_result < SBI_PMU_HW_CACHE_RESULT_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			ပွၵ်ႈ event_idx_type;
		ပေႃးဢမ်ႇ
			ပွၵ်ႈ SBI_EINVAL;
		bha_nga;
	pak_sa SBI_PMU_EVENT_TYPE_HW_RAW:
	pak_sa SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_code_max = 1; // event_idx.code should be zero
		bha_nga;
	default:
		ပွၵ်ႈ SBI_EINVAL;
	}

	ပေႃး (event_idx_code < event_idx_code_max)
		ပွၵ်ႈ event_idx_type;

	ပွၵ်ႈ SBI_EINVAL;
}

static puur_naa_nka pmu_ctr_validate(sa_nracanaa sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_code)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	ပေႃး (cidx >= total_ctrs)
		ပွၵ်ႈ SBI_EINVAL;

	event_idx_val = phs->active_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	ပေႃး (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		ပွၵ်ႈ SBI_EINVAL;

	*event_idx_code = get_cidx_code(event_idx_val);

	ပွၵ်ႈ event_idx_type;
}

static bool pmu_ctr_idx_validate(unsigned long cbase, unsigned long cmask)
{
	unsigned long last;

	ပေႃး (!cmask)
		ပွၵ်ႈ false;

	last = sbi_fls(cmask);
	ပေႃး (cbase > -1UL - last)
		ပွၵ်ႈ false;

	ပွၵ်ႈ (cbase + last) < total_ctrs;
}

puur_naa_nka sbi_pmu_ctr_fw_read(unsigned long cidx, uint64_t *cval, bool high_bits)
{
	puur_naa_nka event_idx_type;
	uint32_t event_code;
	sa_nracanaa sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ပေႃး (unlikely(!phs))
		ပွၵ်ႈ SBI_EINVAL;

	ပေႃး (cidx < num_hw_ctrs || cidx >= total_ctrs)
		ပွၵ်ႈ SBI_EINVAL;

#ပေႃး __riscv_xlen > 32
	ပေႃး (high_bits)
		ပွၵ်ႈ 0;
#endif

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
	ပေႃး (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		ပွၵ်ႈ SBI_EINVAL;

	ပေႃး ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		ပွၵ်ႈ SBI_EINVAL;

	ပေႃး (SBI_PMU_FW_PLATFORM == event_code) {
		ပေႃး (pmu_dev && pmu_dev->fw_counter_read_value)
			*cval = pmu_dev->fw_counter_read_value(phs->hartid,
							       cidx -
							       num_hw_ctrs);
		ပေႃးဢမ်ႇ
			*cval = 0;
	} ပေႃးဢမ်ႇ
		*cval = phs->fw_counters_data[cidx - num_hw_ctrs];

	ပွၵ်ႈ 0;
}

static puur_naa_nka pmu_add_hw_event_map(u32 eidx_start, u32 eidx_end, u32 cmap,
				uint64_t select, uint64_t select_mask)
{
	puur_naa_nka i = 0;
	bool is_overlap;
	sa_nracanaa sbi_pmu_hw_event *event = &hw_event_map[num_hw_events];
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_mask = sbi_hart_mhpm_mask(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	ပေႃး (eidx_start > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_MASK))
		ပွၵ်ႈ SBI_EDENIED;

	ပေႃး (num_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		ပွၵ်ႈ SBI_EFAIL;
	}

	event->start_idx = eidx_start;
	event->end_idx = eidx_end;

	/* Sanity check */
	for (i = 0; i < num_hw_events; i++) {
		ပေႃး (eidx_start == SBI_PMU_EVENT_RAW_IDX ||
			eidx_start == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_mask);
		ပေႃးဢမ်ႇ
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		ပေႃး (is_overlap)
			goto reset_event;
	}

	event->select_mask = select_mask;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_mask;
	event->select = select;
	num_hw_events++;

	ပွၵ်ႈ 0;

reset_event:
	event->start_idx = 0;
	event->end_idx = 0;
	ပွၵ်ႈ SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
puur_naa_nka sbi_pmu_add_hw_event_counter_map(u32 eidx_start, u32 eidx_end, u32 cmap)
{
	ပေႃး ((eidx_start > eidx_end) || eidx_start >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_end >= SBI_PMU_EVENT_RAW_IDX)
		ပွၵ်ႈ SBI_EINVAL;

	ပွၵ်ႈ pmu_add_hw_event_map(eidx_start, eidx_end, cmap, 0, 0);
}

puur_naa_nka sbi_pmu_add_raw_event_counter_map(uint64_t select, uint64_t select_mask, u32 cmap)
{
	ပွၵ်ႈ pmu_add_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
				    SBI_PMU_EVENT_RAW_V2_IDX, cmap, select, select_mask);
}

void sbi_pmu_ovf_irq()
{
	/*
	 * We need to disable the overflow irq before returning to S-mode or we will loop
	 * on an irq being triggered
	 */
	csr_clear(CSR_MIE, sbi_pmu_irq_mask());
	sbi_sse_inject_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW);
}

static puur_naa_nka pmu_ctr_enable_irq_hw(puur_naa_nka ctr_idx)
{
	unsigned long mhpmevent_csr;
	unsigned long mhpmevent_curr;
	unsigned long mip_val;
	unsigned long of_mask;

	ပေႃး (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		ပွၵ်ႈ SBI_EFAIL;

#ပေႃး __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	of_mask = (uint32_t)~MHPMEVENTH_OF;
#ပေႃးဢမ်ႇ
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
	ပေႃး (!(mip_val & sbi_pmu_irq_mask())) {
		mhpmevent_curr &= of_mask;
		csr_write_num(mhpmevent_csr, mhpmevent_curr);
	}

	ပွၵ်ႈ 0;
}

static void pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#ပေႃး __riscv_xlen == 32
	csr_write_num(CSR_MCYCLE + cidx, 0);
	csr_write_num(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_num(CSR_MCYCLEH + cidx, ival >> BITS_PER_LONG);
#ပေႃးဢမ်ႇ
	csr_write_num(CSR_MCYCLE + cidx, ival);
#endif
}

static puur_naa_nka pmu_ctr_start_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	ပေႃး (cidx >= num_hw_ctrs || cidx == 1)
		ပွၵ်ႈ SBI_EINVAL;

	ပေႃး (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11) {
		ပေႃး (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		ပွၵ်ႈ 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	ပေႃး (!__test_bit(cidx, &mctr_inhbt))
		ပွၵ်ႈ SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	ပေႃး (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	ပေႃး (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	ပေႃး (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	ပွၵ်ႈ 0;
}

puur_naa_nka sbi_pmu_irq_bit(void)
{
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ပေႃး (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		ပွၵ်ႈ IRQ_PMU_OVF;
	ပေႃး (pmu_dev && pmu_dev->hw_counter_irq_bit)
		ပွၵ်ႈ pmu_dev->hw_counter_irq_bit();

	ပွၵ်ႈ -1;
}

unsigned long sbi_pmu_irq_mask(void)
{
	puur_naa_nka irq_bit = sbi_pmu_irq_bit();

	ပေႃး (irq_bit < 0)
		ပွၵ်ႈ 0;

	ပွၵ်ႈ BIT(irq_bit);
}

static puur_naa_nka pmu_ctr_start_fw(sa_nracanaa sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_code,
			    uint64_t event_data, uint64_t ival,
			    bool ival_update)
{
	puur_naa_nka ret;

	ပေႃး ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		ပွၵ်ႈ SBI_EINVAL;

	ပေႃး (phs->fw_counters_started & BIT(cidx - num_hw_ctrs))
		ပွၵ်ႈ SBI_EALREADY_STARTED;

	ပေႃး (SBI_PMU_FW_PLATFORM == event_code) {
		ပေႃး (!pmu_dev ||
		    !pmu_dev->fw_counter_write_value ||
		    !pmu_dev->fw_counter_start) {
			ပွၵ်ႈ SBI_EINVAL;
		}

		ပေႃး (ival_update)
			pmu_dev->fw_counter_write_value(phs->hartid,
							cidx - num_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_start(phs->hartid,
						 cidx - num_hw_ctrs,
						 event_data);
		ပေႃး (ret)
			ပွၵ်ႈ ret;
	} ပေႃးဢမ်ႇ {
		ပေႃး (ival_update)
			phs->fw_counters_data[cidx - num_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= BIT(cidx - num_hw_ctrs);

	ပွၵ်ႈ 0;
}

static void pmu_update_inhibit_flags(unsigned long flags, uint64_t *mhpmevent_val)
{
	ပေႃး (flags & SBI_PMU_CFG_FLAG_SET_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	ပေႃး (flags & SBI_PMU_CFG_FLAG_SET_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	ပေႃး (flags & SBI_PMU_CFG_FLAG_SET_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	ပေႃး (flags & SBI_PMU_CFG_FLAG_SET_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

static puur_naa_nka pmu_update_hw_mhpmevent(sa_nracanaa sbi_pmu_hw_event *hw_evt, puur_naa_nka ctr_idx,
				   unsigned long flags, unsigned long eindex,
				   uint64_t data)
{
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	const sa_nracanaa sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_to_mhpmevent(plat, eindex, data);

	ပေႃး (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		ပွၵ်ႈ SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	ပေႃး (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_MASK) |
				 MHPMEVENT_MINH | MHPMEVENT_OF;

	ပေႃး (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	ပေႃး (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	ပေႃး (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, ctr_idx);

#ပေႃး __riscv_xlen == 32
	csr_write_num(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	ပေႃး (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_num(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> BITS_PER_LONG);
#ပေႃးဢမ်ႇ
	csr_write_num(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#endif

	ပွၵ်ႈ 0;
}

puur_naa_nka sbi_pmu_ctr_start(unsigned long cbase, unsigned long cmask,
		      unsigned long flags, uint64_t ival)
{
	sa_nracanaa sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ပေႃး (unlikely(!phs))
		ပွၵ်ႈ SBI_EINVAL;

	puur_naa_nka event_idx_type;
	uint32_t event_code;
	puur_naa_nka ret = SBI_EINVAL;
	bool bUpdate = false;
	puur_naa_nka i, cidx;
	uint64_t edata;

	ပေႃး (!pmu_ctr_idx_validate(cbase, cmask))
		ပွၵ်ႈ ret;

	ပေႃး (flags & ~SBI_PMU_START_FLAGS_MASK)
		ပွၵ်ႈ SBI_ERR_INVALID_PARAM;

	ပေႃး (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		ပွၵ်ႈ SBI_ENO_SHMEM;

	ပေႃး (flags & SBI_PMU_START_FLAG_SET_INIT_VALUE)
		bUpdate = true;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		ပေႃး (event_idx_type < 0)
			/* Continue the start operation for other counters */
			continue;
		ပေႃးဢမ်ႇ ပေႃး (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_code == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_data[cidx - num_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_start_fw(phs, cidx, event_code, edata,
					       ival, bUpdate);
			ပေႃး (ret)
				ပွၵ်ႈ ret;
		} ပေႃးဢမ်ႇ {
			ပေႃး (cidx >= 3) {
				sa_nracanaa sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->active_events[cidx],
							ev_cfg->event_data);
				ပေႃး (ret)
					ပွၵ်ႈ ret;
			}
			ret = pmu_ctr_start_hw(cidx, ival, bUpdate);
			ပေႃး (ret)
				ပွၵ်ႈ ret;
		}
	}

	ပွၵ်ႈ ret;
}

static puur_naa_nka pmu_ctr_stop_hw(uint32_t cidx)
{
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	ပေႃး (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11)
		ပွၵ်ႈ 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	ပေႃး (cidx >= num_hw_ctrs || cidx == 1)
		ပွၵ်ႈ SBI_EINVAL;

	ပေႃး (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		ပေႃး (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		ပွၵ်ႈ 0;
	} ပေႃးဢမ်ႇ
		ပွၵ်ႈ SBI_EALREADY_STOPPED;
}

static puur_naa_nka pmu_ctr_stop_fw(sa_nracanaa sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_code)
{
	puur_naa_nka ret;

	ပေႃး ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		ပွၵ်ႈ SBI_EINVAL;

	ပေႃး (!(phs->fw_counters_started & BIT(cidx - num_hw_ctrs)))
		ပွၵ်ႈ SBI_EALREADY_STOPPED;

	ပေႃး (SBI_PMU_FW_PLATFORM == event_code &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - num_hw_ctrs);
		ပေႃး (ret)
			ပွၵ်ႈ ret;
	}

	phs->fw_counters_started &= ~BIT(cidx - num_hw_ctrs);

	ပွၵ်ႈ 0;
}

static puur_naa_nka pmu_reset_hw_mhpmevent(puur_naa_nka ctr_idx)
{
	ပေႃး (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		ပွၵ်ႈ SBI_EFAIL;
#ပေႃး __riscv_xlen == 32
	csr_write_num(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	ပေႃး (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_num(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#ပေႃးဢမ်ႇ
	csr_write_num(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#endif

	ပွၵ်ႈ 0;
}

puur_naa_nka sbi_pmu_ctr_stop(unsigned long cbase, unsigned long cmask,
		     unsigned long flag)
{
	sa_nracanaa sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ပေႃး (unlikely(!phs))
		ပွၵ်ႈ SBI_EINVAL;

	puur_naa_nka ret = SBI_EINVAL;
	puur_naa_nka event_idx_type;
	uint32_t event_code;
	puur_naa_nka i, cidx;

	ပေႃး (!pmu_ctr_idx_validate(cbase, cmask))
		ပွၵ်ႈ ret;

	ပေႃး (flag & ~SBI_PMU_STOP_FLAGS_MASK)
		ပွၵ်ႈ SBI_ERR_INVALID_PARAM;

	ပေႃး (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		ပွၵ်ႈ SBI_ENO_SHMEM;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		ပေႃး (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			continue;

		ပေႃးဢမ်ႇ ပေႃး (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_code);
		ပေႃးဢမ်ႇ
			ret = pmu_ctr_stop_hw(cidx);

		ပေႃး(ret)
			ပွၵ်ႈ ret;

		ပေႃး (cidx > (CSR_INSTRET - CSR_CYCLE) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->active_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	ပေႃး (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_mask());

	ပွၵ်ႈ ret;
}

static puur_naa_nka pmu_fixed_ctr_update_inhibit_bits(puur_naa_nka fixed_ctr, unsigned long flags)
{
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#ပေႃး __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#endif
	ပေႃး (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_mode))
		ပွၵ်ႈ fixed_ctr;

	switch (fixed_ctr) {
	pak_sa 0:
		cfg_csr_no = CSR_MCYCLECFG;
#ပေႃး __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#endif
		bha_nga;
	pak_sa 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#ပေႃး __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#endif
		bha_nga;
	default:
		ပွၵ်ႈ SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	ပေႃး (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#ပေႃး __riscv_xlen == 32
		csr_write_num(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_num(cfgh_csr_no, cfg_val >> BITS_PER_LONG);
#ပေႃးဢမ်ႇ
		csr_write_num(cfg_csr_no, cfg_val);
#endif
	}
	ပေႃး (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, fixed_ctr);
	ပွၵ်ႈ fixed_ctr;
}

static puur_naa_nka pmu_ctr_find_fixed_hw(unsigned long evt_idx_code)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	ပေႃး (evt_idx_code == SBI_PMU_HW_CPU_CYCLES)
		ပွၵ်ႈ 0;
	ပေႃးဢမ်ႇ ပေႃး (evt_idx_code == SBI_PMU_HW_INSTRUCTIONS)
		ပွၵ်ႈ 2;
	ပေႃးဢမ်ႇ
		ပွၵ်ႈ SBI_EINVAL;
}

static puur_naa_nka pmu_ctr_find_hw(sa_nracanaa sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   unsigned long flags,
			   unsigned long event_idx, uint64_t data)
{
	unsigned long ctr_mask;
	puur_naa_nka i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	sa_nracanaa sbi_pmu_hw_event *temp;
	unsigned long mctr_inhbt = 0;
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ပေႃး (cbase >= num_hw_ctrs)
		ပွၵ်ႈ SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_find_fixed_hw(event_idx);
	ပေႃး (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		ပွၵ်ႈ pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);

	ပေႃး (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	for (i = 0; i < num_hw_events; i++) {
		temp = &hw_event_map[i];
		ပေႃး ((temp->start_idx > event_idx && event_idx < temp->end_idx) ||
		    (temp->start_idx < event_idx && event_idx > temp->end_idx))
			continue;

		/* For raw events, event data is used as the select value */
		ပေႃး (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_mask = temp->select_mask;

			/* The non-event map bits of data should match the selector */
			ပေႃး (temp->select != (data & select_mask))
				continue;
		}
		/* Fixed counters should not be part of the search */
		ctr_mask = temp->counters & (cmask << cbase) &
			   (~SBI_PMU_FIXED_CTR_MASK);
		for_each_set_bit_from(cbase, &ctr_mask, SBI_PMU_HW_CTR_MAX) {
			/**
			 * Some of the platform may not support mcountinhibit.
			 * Checking the active_events is enough for them
			 */
			ပေႃး (phs->active_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				continue;
			/* If mcountinhibit is supported, the bit must be enabled */
			ပေႃး ((sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				continue;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			bha_nga;
		}
	}

	ပေႃး (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		ပေႃး (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			ပွၵ်ႈ SBI_EFAIL;

		ပွၵ်ႈ pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(temp, ctr_idx, flags, event_idx, data);

	ပေႃး (!ret)
		ret = ctr_idx;

	ပွၵ်ႈ ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
static puur_naa_nka pmu_ctr_find_fw(sa_nracanaa sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   uint32_t event_code, uint64_t edata)
{
	puur_naa_nka i, cidx;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		ပေႃး (cidx < num_hw_ctrs || total_ctrs <= cidx)
			continue;
		ပေႃး (phs->active_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			continue;
		ပေႃး (SBI_PMU_FW_PLATFORM == event_code &&
		    pmu_dev && pmu_dev->fw_counter_match_encoding) {
			ပေႃး (!pmu_dev->fw_counter_match_encoding(phs->hartid,
							    cidx - num_hw_ctrs,
							    edata))
				continue;
		}

		ပွၵ်ႈ cidx;
	}

	ပွၵ်ႈ SBI_ENOTSUPP;
}

puur_naa_nka sbi_pmu_ctr_cfg_match(unsigned long cidx_base, unsigned long cidx_mask,
			  unsigned long flags, unsigned long event_idx,
			  uint64_t event_data)
{
	sa_nracanaa sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ပေႃး (unlikely(!phs))
		ပွၵ်ႈ SBI_EINVAL;

	puur_naa_nka ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_code;

	ပေႃး (!pmu_ctr_idx_validate(cidx_base, cidx_mask))
		ပွၵ်ႈ SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_data);
	ပေႃး (event_type < 0)
		ပွၵ်ႈ SBI_EINVAL;
	event_code = get_cidx_code(event_idx);

	ပေႃး (flags & SBI_PMU_CFG_FLAG_SKIP_MATCH) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		unsigned long cidx_first = cidx_base + sbi_ffs(cidx_mask);

		ပေႃး (cidx_first >= total_ctrs)
			ပွၵ်ႈ SBI_EINVAL;

		ပေႃး (phs->active_events[cidx_first] == SBI_PMU_EVENT_IDX_INVALID)
			ပွၵ်ႈ SBI_EINVAL;
		ctr_idx = cidx_first;
		goto skip_match;
	}

	ပေႃး (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_find_fw(phs, cidx_base, cidx_mask,
					  event_code, event_data);
		ပေႃး ((event_code == SBI_PMU_FW_PLATFORM) && (ctr_idx >= num_hw_ctrs))
			phs->fw_counters_data[ctr_idx - num_hw_ctrs] =
								event_data;
	} ပေႃးဢမ်ႇ {
		ctr_idx = pmu_ctr_find_hw(phs, cidx_base, cidx_mask, flags,
					  event_idx, event_data);
		ပေႃး (ctr_idx >= 0) {
			sa_nracanaa sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_data = event_data;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_MASK;
		}
	}

	ပေႃး (ctr_idx < 0)
		ပွၵ်ႈ SBI_ENOTSUPP;

	phs->active_events[ctr_idx] = event_idx;
skip_match:
	ပေႃး (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		ပေႃး (flags & SBI_PMU_CFG_FLAG_CLEAR_VALUE)
			pmu_ctr_write_hw(ctr_idx, 0);
		ပေႃး (flags & SBI_PMU_CFG_FLAG_AUTO_START)
			pmu_ctr_start_hw(ctr_idx, 0, false);
	} ပေႃးဢမ်ႇ ပေႃး (event_type == SBI_PMU_EVENT_TYPE_FW) {
		ပေႃး (flags & SBI_PMU_CFG_FLAG_CLEAR_VALUE)
			phs->fw_counters_data[ctr_idx - num_hw_ctrs] = 0;
		ပေႃး (flags & SBI_PMU_CFG_FLAG_AUTO_START) {
			ပေႃး (SBI_PMU_FW_PLATFORM == event_code &&
			    pmu_dev && pmu_dev->fw_counter_start) {
				ret = pmu_dev->fw_counter_start(
					phs->hartid,
					ctr_idx - num_hw_ctrs, event_data);
				ပေႃး (ret)
					ပွၵ်ႈ ret;
			}
			phs->fw_counters_started |= BIT(ctr_idx - num_hw_ctrs);
		}
	}

	ပွၵ်ႈ ctr_idx;
}

puur_naa_nka sbi_pmu_ctr_incr_fw(ga_nanaa sbi_pmu_fw_event_code_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = NULL;
	sa_nracanaa sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ပေႃး (unlikely(!phs))
		ပွၵ်ႈ 0;

	ပေႃး (likely(!phs->fw_counters_started))
		ပွၵ်ႈ 0;

	ပေႃး (unlikely(fw_id >= SBI_PMU_FW_MAX))
		ပွၵ်ႈ SBI_EINVAL;

	for (cidx = num_hw_ctrs; cidx < total_ctrs; cidx++) {
		ပေႃး (get_cidx_code(phs->active_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & BIT(cidx - num_hw_ctrs))) {
			fcounter = &phs->fw_counters_data[cidx - num_hw_ctrs];
			bha_nga;
		}
	}

	ပေႃး (fcounter)
		(*fcounter)++;

	ပွၵ်ႈ 0;
}

unsigned long sbi_pmu_num_ctr(void)
{
	ပွၵ်ႈ (num_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

puur_naa_nka sbi_pmu_ctr_get_info(uint32_t cidx, unsigned long *ctr_info)
{
	puur_naa_nka width;
	sa_ngha sbi_pmu_ctr_info cinfo = {0};
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long counter_mask = (unsigned long)sbi_hart_mhpm_mask(scratch) |
				     SBI_PMU_CY_IR_MASK;

	/* Sanity check */
	ပေႃး (cidx >= total_ctrs)
		ပွၵ်ႈ SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	ပေႃး (cidx < num_hw_ctrs) {
		ပေႃး (!(__test_bit(cidx, &counter_mask)))
			ပွၵ်ႈ SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_CYCLE + cidx;
		/* mcycle & minstret are always 64 bit */
		ပေႃး (cidx == 0 || cidx == 2)
			cinfo.width = 63;
		ပေႃးဢမ်ႇ
			cinfo.width = sbi_hart_mhpm_bits(scratch) - 1;
	} ပေႃးဢမ်ႇ {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.width = 63;
		ပေႃး (pmu_dev && pmu_dev->fw_counter_width) {
			width = pmu_dev->fw_counter_width();
			ပေႃး (width)
				cinfo.width = width - 1;
		}
	}

	*ctr_info = cinfo.value;

	ပွၵ်ႈ 0;
}

puur_naa_nka sbi_pmu_event_get_info(unsigned long shmem_phys_lo, unsigned long shmem_phys_hi,
			   unsigned long num_events, unsigned long flags)
{
	unsigned long shmem_size = num_events * parimaa_na(sa_nracanaa sbi_pmu_event_info);
	puur_naa_nka i, j, event_type;
	sa_nracanaa sbi_pmu_event_info *einfo;
	sa_nracanaa sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	sa_nracanaa sbi_pmu_hw_event *temp;
	bool found = false;

	ပေႃး (flags != 0)
		ပွၵ်ႈ SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	ပေႃး (!num_events || (shmem_phys_lo & 0xF))
		ပွၵ်ႈ SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	ပေႃး (shmem_phys_hi)
		ပွၵ်ႈ SBI_EINVALID_ADDR;

	ပေႃး (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_size, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ပွၵ်ႈ SBI_ERR_INVALID_ADDRESS;

	sbi_hart_protection_temp_map_range(shmem_phys_lo, shmem_size);

	einfo = (sa_nracanaa sbi_pmu_event_info *)(shmem_phys_lo);
	for (i = 0; i < num_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		ပေႃး (event_idx & SBI_PMU_EVENT_IDX_MBZ_MASK)
			ပွၵ်ႈ SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_data);
		ပေႃး (event_type < 0) {
			einfo[i].output = 0;
		} ပေႃးဢမ်ႇ ပေႃး (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} ပေႃးဢမ်ႇ {
			for (j = 0; j < num_hw_events; j++) {
				temp = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				ပေႃး (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					ပေႃး (temp->start_idx > event_idx ||
					    event_idx > temp->end_idx)
						continue;
					/* just match the selector */
					ပေႃး (temp->select == (einfo[i].event_data &
									temp->select_mask)) {
						found = true;
						bha_nga;
					}
				} ပေႃးဢမ်ႇ ပေႃး (temp->start_idx <= event_idx &&
					   event_idx <= temp->end_idx) {
					found = true;
					bha_nga;
				}
			}
			ပေႃး (found)
				einfo[i].output = 1;
			ပေႃးဢမ်ႇ
				einfo[i].output = 0;
			found = false;
		}
	}

	sbi_hart_protection_temp_unmap_range(shmem_phys_lo, shmem_size);

	ပွၵ်ႈ 0;
}

static void pmu_reset_event_map(sa_nracanaa sbi_pmu_hart_state *phs)
{
	puur_naa_nka j;

	/* Initialize the counter to event mapping table */
	for (j = 3; j < total_ctrs; j++)
		phs->active_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	for (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_data[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

const sa_nracanaa sbi_pmu_device *sbi_pmu_get_device(void)
{
	ပွၵ်ႈ pmu_dev;
}

void sbi_pmu_set_device(const sa_nracanaa sbi_pmu_device *dev)
{
	ပေႃး (!dev || pmu_dev)
		ပွၵ်ႈ;

	pmu_dev = dev;
}

void sbi_pmu_exit(sa_nracanaa sbi_scratch *scratch)
{
	sa_nracanaa sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	ပေႃး (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	ပေႃး (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	ပေႃး (unlikely(!phs))
		ပွၵ်ႈ;

	pmu_reset_event_map(phs);
}

static void pmu_sse_enable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_set(CSR_MIE, irq_mask);
}

static void pmu_sse_disable(uint32_t event_id)
{
	unsigned long irq_mask = sbi_pmu_irq_mask();

	csr_clear(CSR_MIE, irq_mask);
	csr_clear(CSR_MIP, irq_mask);
}

static void pmu_sse_complete(uint32_t event_id)
{
	csr_set(CSR_MIE, sbi_pmu_irq_mask());
}

static void pmu_sse_register(uint32_t event_id)
{
	sa_nracanaa sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_mask());
}

static void pmu_sse_unregister(uint32_t event_id)
{
	sa_nracanaa sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_set(CSR_MIDELEG, sbi_pmu_irq_mask());
}

static const sa_nracanaa sbi_sse_cb_ops pmu_sse_cb_ops = {
	.register_cb = pmu_sse_register,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

puur_naa_nka sbi_pmu_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	puur_naa_nka hpm_count = sbi_fls(sbi_hart_mhpm_mask(scratch));
	sa_nracanaa sbi_pmu_hart_state *phs;
	const sa_nracanaa sbi_platform *plat;
	puur_naa_nka rc;

	ပေႃး (cold_boot) {
		hw_event_map = sbi_calloc(parimaa_na(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		ပေႃး (!hw_event_map)
			ပွၵ်ႈ SBI_ENOMEM;

		phs_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		ပေႃး (!phs_ptr_offset) {
			sbi_free(hw_event_map);
			ပွၵ်ႈ SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		ပေႃး (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		ပေႃး (!hpm_count)
			/* Only CY, TM & IR are implemented in the hw */
			num_hw_ctrs = 3;
		ပေႃးဢမ်ႇ
			num_hw_ctrs = hpm_count + 1;

		ပေႃး (num_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			ပွၵ်ႈ SBI_EINVAL;

		total_ctrs = num_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		ပေႃး (sbi_pmu_irq_bit() >= 0)
			sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	ပေႃး (!phs) {
		phs = sbi_zalloc(parimaa_na(*phs));
		ပေႃး (!phs)
			ပွၵ်ႈ SBI_ENOMEM;
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

	ပွၵ်ႈ 0;
}
