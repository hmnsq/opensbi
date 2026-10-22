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
struct sbi_pmu_hw_event {
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
union sbi_pmu_ctr_info {
	unsigned long value;
	struct {
		unsigned long csr:12;
		unsigned long width:6;
#ако __riscv_xlen == 32
		unsigned long reserved:13;
#инако
		unsigned long reserved:45;
#endif
		unsigned long type:1;
	};
};

#ако SBI_PMU_FW_CTR_MAX >= BITS_PER_LONG
#error "Can't handle firmware counters beyond BITS_PER_LONG"
#endif

/** HW event configuration parameters */
struct sbi_pmu_hw_event_config {
	/* event_data value from sbi_pmu_ctr_cfg_match() */
	uint64_t event_data;
	/* HW events flags from sbi_pmu_ctr_cfg_match() */
	uint64_t flags;
};

/** Per-HART state of the PMU counters */
struct sbi_pmu_hart_state {
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
	struct sbi_pmu_hw_event_config hw_counters_cfg[SBI_PMU_HW_CTR_MAX];
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
static const struct sbi_pmu_device *pmu_dev = NULL;

/* Mapping between event range and possible counters  */
static struct sbi_pmu_hw_event *hw_event_map;

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
static bool pmu_event_range_overlap(struct sbi_pmu_hw_event *evtA,
				    struct sbi_pmu_hw_event *evtB)
{
	/* check if the range of events overlap with a previous entry */
	ако (((evtA->end_idx < evtB->start_idx) && (evtA->end_idx < evtB->end_idx)) ||
	   ((evtA->start_idx > evtB->start_idx) && (evtA->start_idx > evtB->end_idx)))
		врати false;
	врати true;
}

static bool pmu_event_select_overlap(struct sbi_pmu_hw_event *evt,
				     uint64_t select_val, uint64_t select_mask)
{
	ако ((evt->select == select_val) && (evt->select_mask == select_mask))
		врати true;

	врати false;
}

static цео pmu_event_validate(struct sbi_pmu_hart_state *phs,
			      unsigned long event_idx, uint64_t edata)
{
	uint32_t event_idx_type = get_cidx_type(event_idx);
	uint32_t event_idx_code = get_cidx_code(event_idx);
	uint32_t event_idx_code_max = -1;
	uint32_t cache_ops_result, cache_ops_id, cache_id;

	switch(event_idx_type) {
	case SBI_PMU_EVENT_TYPE_HW:
		event_idx_code_max = SBI_PMU_HW_GENERAL_MAX;
		break;
	case SBI_PMU_EVENT_TYPE_FW:
		ако ((event_idx_code >= SBI_PMU_FW_MAX &&
		    event_idx_code <= SBI_PMU_FW_RESERVED_MAX) ||
		    event_idx_code > SBI_PMU_FW_PLATFORM)
			врати SBI_EINVAL;

		ако (SBI_PMU_FW_PLATFORM == event_idx_code &&
		    pmu_dev && pmu_dev->fw_event_validate_encoding)
			врати pmu_dev->fw_event_validate_encoding(phs->hartid,
							           edata);
		инако
			event_idx_code_max = SBI_PMU_FW_MAX;
		break;
	case SBI_PMU_EVENT_TYPE_HW_CACHE:
		cache_ops_result = event_idx_code &
					SBI_PMU_EVENT_HW_CACHE_OPS_RESULT;
		cache_ops_id = (event_idx_code &
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_MASK) >>
				SBI_PMU_EVENT_HW_CACHE_OPS_ID_OFFSET;
		cache_id = (event_idx_code &
			    SBI_PMU_EVENT_HW_CACHE_ID_MASK) >>
			    SBI_PMU_EVENT_HW_CACHE_ID_OFFSET;
		ако ((cache_ops_result < SBI_PMU_HW_CACHE_RESULT_MAX) &&
		    (cache_ops_id < SBI_PMU_HW_CACHE_OP_MAX) &&
		    (cache_id < SBI_PMU_HW_CACHE_MAX))
			врати event_idx_type;
		инако
			врати SBI_EINVAL;
		break;
	case SBI_PMU_EVENT_TYPE_HW_RAW:
	case SBI_PMU_EVENT_TYPE_HW_RAW_V2:
		event_idx_code_max = 1; // event_idx.code should be zero
		break;
	default:
		врати SBI_EINVAL;
	}

	ако (event_idx_code < event_idx_code_max)
		врати event_idx_type;

	врати SBI_EINVAL;
}

static цео pmu_ctr_validate(struct sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t *event_idx_code)
{
	uint32_t event_idx_val;
	uint32_t event_idx_type;

	ако (cidx >= total_ctrs)
		врати SBI_EINVAL;

	event_idx_val = phs->active_events[cidx];
	event_idx_type = get_cidx_type(event_idx_val);
	ако (event_idx_val == SBI_PMU_EVENT_IDX_INVALID ||
	    event_idx_type >= SBI_PMU_EVENT_TYPE_MAX)
		врати SBI_EINVAL;

	*event_idx_code = get_cidx_code(event_idx_val);

	врати event_idx_type;
}

static bool pmu_ctr_idx_validate(unsigned long cbase, unsigned long cmask)
{
	unsigned long last;

	ако (!cmask)
		врати false;

	last = sbi_fls(cmask);
	ако (cbase > -1UL - last)
		врати false;

	врати (cbase + last) < total_ctrs;
}

цео sbi_pmu_ctr_fw_read(unsigned long cidx, uint64_t *cval, bool high_bits)
{
	цео event_idx_type;
	uint32_t event_code;
	struct sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ако (unlikely(!phs))
		врати SBI_EINVAL;

	ако (cidx < num_hw_ctrs || cidx >= total_ctrs)
		врати SBI_EINVAL;

#ако __riscv_xlen > 32
	ако (high_bits)
		врати 0;
#endif

	event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
	ако (event_idx_type != SBI_PMU_EVENT_TYPE_FW)
		врати SBI_EINVAL;

	ако ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		врати SBI_EINVAL;

	ако (SBI_PMU_FW_PLATFORM == event_code) {
		ако (pmu_dev && pmu_dev->fw_counter_read_value)
			*cval = pmu_dev->fw_counter_read_value(phs->hartid,
							       cidx -
							       num_hw_ctrs);
		инако
			*cval = 0;
	} инако
		*cval = phs->fw_counters_data[cidx - num_hw_ctrs];

	врати 0;
}

static цео pmu_add_hw_event_map(u32 eidx_start, u32 eidx_end, u32 cmap,
				uint64_t select, uint64_t select_mask)
{
	цео i = 0;
	bool is_overlap;
	struct sbi_pmu_hw_event *event = &hw_event_map[num_hw_events];
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint32_t ctr_avail_mask = sbi_hart_mhpm_mask(scratch) | 0x7;

	/* The first two counters are reserved by priv spec */
	ако (eidx_start > SBI_PMU_HW_INSTRUCTIONS && (cmap & SBI_PMU_FIXED_CTR_MASK))
		врати SBI_EDENIED;

	ако (num_hw_events >= SBI_PMU_HW_EVENT_MAX - 1) {
		sbi_printf("Can not handle more than %d perf events\n",
			    SBI_PMU_HW_EVENT_MAX);
		врати SBI_EFAIL;
	}

	event->start_idx = eidx_start;
	event->end_idx = eidx_end;

	/* Sanity check */
	за (i = 0; i < num_hw_events; i++) {
		ако (eidx_start == SBI_PMU_EVENT_RAW_IDX ||
			eidx_start == SBI_PMU_EVENT_RAW_V2_IDX)
		/* All raw events have same event idx. Just do sanity check on select */
			is_overlap = pmu_event_select_overlap(&hw_event_map[i],
							      select, select_mask);
		инако
			is_overlap = pmu_event_range_overlap(&hw_event_map[i], event);
		ако (is_overlap)
			goto reset_event;
	}

	event->select_mask = select_mask;
	/* Map the only the counters that are available in the hardware */
	event->counters = cmap & ctr_avail_mask;
	event->select = select;
	num_hw_events++;

	врати 0;

reset_event:
	event->start_idx = 0;
	event->end_idx = 0;
	врати SBI_EINVAL;
}

/**
 * Logical counter ids are assigned to hardware counters are assigned consecutively.
 * E.g. counter0 must count MCYCLE where counter2 must count minstret. Similarly,
 * counterX will mhpmcounterX.
 */
цео sbi_pmu_add_hw_event_counter_map(u32 eidx_start, u32 eidx_end, u32 cmap)
{
	ако ((eidx_start > eidx_end) || eidx_start >= SBI_PMU_EVENT_RAW_IDX ||
	     eidx_end >= SBI_PMU_EVENT_RAW_IDX)
		врати SBI_EINVAL;

	врати pmu_add_hw_event_map(eidx_start, eidx_end, cmap, 0, 0);
}

цео sbi_pmu_add_raw_event_counter_map(uint64_t select, uint64_t select_mask, u32 cmap)
{
	врати pmu_add_hw_event_map(SBI_PMU_EVENT_RAW_IDX,
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

static цео pmu_ctr_enable_irq_hw(цео ctr_idx)
{
	unsigned long mhpmevent_csr;
	unsigned long mhpmevent_curr;
	unsigned long mip_val;
	unsigned long of_mask;

	ако (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		врати SBI_EFAIL;

#ако __riscv_xlen == 32
	mhpmevent_csr = CSR_MHPMEVENT3H  + ctr_idx - 3;
	of_mask = (uint32_t)~MHPMEVENTH_OF;
#инако
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
	ако (!(mip_val & sbi_pmu_irq_mask())) {
		mhpmevent_curr &= of_mask;
		csr_write_num(mhpmevent_csr, mhpmevent_curr);
	}

	врати 0;
}

static void pmu_ctr_write_hw(uint32_t cidx, uint64_t ival)
{
#ако __riscv_xlen == 32
	csr_write_num(CSR_MCYCLE + cidx, 0);
	csr_write_num(CSR_MCYCLE + cidx, ival & 0xFFFFFFFF);
	csr_write_num(CSR_MCYCLEH + cidx, ival >> BITS_PER_LONG);
#инако
	csr_write_num(CSR_MCYCLE + cidx, ival);
#endif
}

static цео pmu_ctr_start_hw(uint32_t cidx, uint64_t ival, bool ival_update)
{
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	/* Make sure the counter index lies within the range and is not TM bit */
	ако (cidx >= num_hw_ctrs || cidx == 1)
		врати SBI_EINVAL;

	ако (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11) {
		ако (ival_update)
			pmu_ctr_write_hw(cidx, ival);
		врати 0;
	}

	/*
	 * Some of the hardware may not support mcountinhibit but perf stat
	 * still can work if supervisor mode programs the initial value.
	 */
	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	ако (!__test_bit(cidx, &mctr_inhbt))
		врати SBI_EALREADY_STARTED;

	__clear_bit(cidx, &mctr_inhbt);

	ако (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_ctr_enable_irq_hw(cidx);
	ако (ival_update)
		pmu_ctr_write_hw(cidx, ival);
	ако (pmu_dev && pmu_dev->hw_counter_enable_irq)
		pmu_dev->hw_counter_enable_irq(cidx);

	csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);

	врати 0;
}

цео sbi_pmu_irq_bit(void)
{
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ако (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		врати IRQ_PMU_OVF;
	ако (pmu_dev && pmu_dev->hw_counter_irq_bit)
		врати pmu_dev->hw_counter_irq_bit();

	врати -1;
}

unsigned long sbi_pmu_irq_mask(void)
{
	цео irq_bit = sbi_pmu_irq_bit();

	ако (irq_bit < 0)
		врати 0;

	врати BIT(irq_bit);
}

static цео pmu_ctr_start_fw(struct sbi_pmu_hart_state *phs,
			    uint32_t cidx, uint32_t event_code,
			    uint64_t event_data, uint64_t ival,
			    bool ival_update)
{
	цео ret;

	ако ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		врати SBI_EINVAL;

	ако (phs->fw_counters_started & BIT(cidx - num_hw_ctrs))
		врати SBI_EALREADY_STARTED;

	ако (SBI_PMU_FW_PLATFORM == event_code) {
		ако (!pmu_dev ||
		    !pmu_dev->fw_counter_write_value ||
		    !pmu_dev->fw_counter_start) {
			врати SBI_EINVAL;
		}

		ако (ival_update)
			pmu_dev->fw_counter_write_value(phs->hartid,
							cidx - num_hw_ctrs,
							ival);

		ret = pmu_dev->fw_counter_start(phs->hartid,
						 cidx - num_hw_ctrs,
						 event_data);
		ако (ret)
			врати ret;
	} инако {
		ако (ival_update)
			phs->fw_counters_data[cidx - num_hw_ctrs] = ival;
	}

	phs->fw_counters_started |= BIT(cidx - num_hw_ctrs);

	врати 0;
}

static void pmu_update_inhibit_flags(unsigned long flags, uint64_t *mhpmevent_val)
{
	ако (flags & SBI_PMU_CFG_FLAG_SET_VUINH)
		*mhpmevent_val |= MHPMEVENT_VUINH;
	ако (flags & SBI_PMU_CFG_FLAG_SET_VSINH)
		*mhpmevent_val |= MHPMEVENT_VSINH;
	ако (flags & SBI_PMU_CFG_FLAG_SET_UINH)
		*mhpmevent_val |= MHPMEVENT_UINH;
	ако (flags & SBI_PMU_CFG_FLAG_SET_SINH)
		*mhpmevent_val |= MHPMEVENT_SINH;
}

static цео pmu_update_hw_mhpmevent(struct sbi_pmu_hw_event *hw_evt, цео ctr_idx,
				   unsigned long flags, unsigned long eindex,
				   uint64_t data)
{
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	uint64_t mhpmevent_val;

	/* Get the final mhpmevent value to be written from platform */
	mhpmevent_val = sbi_platform_pmu_xlate_to_mhpmevent(plat, eindex, data);

	ако (!mhpmevent_val || ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		врати SBI_EFAIL;

	/**
	 * Always set the OVF bit(disable interrupts) and inhibit counting of
	 * events in M-mode. The OVF bit should be enabled during the start call.
	 */
	ако (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		mhpmevent_val = (mhpmevent_val & ~MHPMEVENT_SSCOF_MASK) |
				 MHPMEVENT_MINH | MHPMEVENT_OF;

	ако (pmu_dev && pmu_dev->hw_counter_disable_irq)
		pmu_dev->hw_counter_disable_irq(ctr_idx);

	/* Update the inhibit flags based on inhibit flags received from supervisor */
	ако (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		pmu_update_inhibit_flags(flags, &mhpmevent_val);
	ако (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, ctr_idx);

#ако __riscv_xlen == 32
	csr_write_num(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val & 0xFFFFFFFF);
	ако (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		csr_write_num(CSR_MHPMEVENT3H + ctr_idx - 3,
			      mhpmevent_val >> BITS_PER_LONG);
#инако
	csr_write_num(CSR_MHPMEVENT3 + ctr_idx - 3, mhpmevent_val);
#endif

	врати 0;
}

цео sbi_pmu_ctr_start(unsigned long cbase, unsigned long cmask,
		      unsigned long flags, uint64_t ival)
{
	struct sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ако (unlikely(!phs))
		врати SBI_EINVAL;

	цео event_idx_type;
	uint32_t event_code;
	цео ret = SBI_EINVAL;
	bool bUpdate = false;
	цео i, cidx;
	uint64_t edata;

	ако (!pmu_ctr_idx_validate(cbase, cmask))
		врати ret;

	ако (flags & ~SBI_PMU_START_FLAGS_MASK)
		врати SBI_ERR_INVALID_PARAM;

	ако (flags & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		врати SBI_ENO_SHMEM;

	ако (flags & SBI_PMU_START_FLAG_SET_INIT_VALUE)
		bUpdate = true;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		ако (event_idx_type < 0)
			/* Continue the start operation for other counters */
			continue;
		инако ако (event_idx_type == SBI_PMU_EVENT_TYPE_FW) {
			edata = (event_code == SBI_PMU_FW_PLATFORM) ?
				 phs->fw_counters_data[cidx - num_hw_ctrs]
				 : 0x0;
			ret = pmu_ctr_start_fw(phs, cidx, event_code, edata,
					       ival, bUpdate);
			ако (ret)
				врати ret;
		} инако {
			ако (cidx >= 3) {
				struct sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[cidx];

				ret = pmu_update_hw_mhpmevent(&hw_event_map[cidx], cidx,
							ev_cfg->flags,
							phs->active_events[cidx],
							ev_cfg->event_data);
				ако (ret)
					врати ret;
			}
			ret = pmu_ctr_start_hw(cidx, ival, bUpdate);
			ако (ret)
				врати ret;
		}
	}

	врати ret;
}

static цео pmu_ctr_stop_hw(uint32_t cidx)
{
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long mctr_inhbt;

	ако (sbi_hart_priv_version(scratch) < SBI_HART_PRIV_VER_1_11)
		врати 0;

	mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);

	/* Make sure the counter index lies within the range and is not TM bit */
	ако (cidx >= num_hw_ctrs || cidx == 1)
		врати SBI_EINVAL;

	ако (!__test_bit(cidx, &mctr_inhbt)) {
		__set_bit(cidx, &mctr_inhbt);
		csr_write(CSR_MCOUNTINHIBIT, mctr_inhbt);
		ако (pmu_dev && pmu_dev->hw_counter_disable_irq) {
			pmu_dev->hw_counter_disable_irq(cidx);
		}
		врати 0;
	} инако
		врати SBI_EALREADY_STOPPED;
}

static цео pmu_ctr_stop_fw(struct sbi_pmu_hart_state *phs,
			   uint32_t cidx, uint32_t event_code)
{
	цео ret;

	ако ((event_code >= SBI_PMU_FW_MAX &&
	    event_code <= SBI_PMU_FW_RESERVED_MAX) ||
	    event_code > SBI_PMU_FW_PLATFORM)
		врати SBI_EINVAL;

	ако (!(phs->fw_counters_started & BIT(cidx - num_hw_ctrs)))
		врати SBI_EALREADY_STOPPED;

	ако (SBI_PMU_FW_PLATFORM == event_code &&
	    pmu_dev && pmu_dev->fw_counter_stop) {
		ret = pmu_dev->fw_counter_stop(phs->hartid, cidx - num_hw_ctrs);
		ако (ret)
			врати ret;
	}

	phs->fw_counters_started &= ~BIT(cidx - num_hw_ctrs);

	врати 0;
}

static цео pmu_reset_hw_mhpmevent(цео ctr_idx)
{
	ако (ctr_idx < 3 || ctr_idx >= SBI_PMU_HW_CTR_MAX)
		врати SBI_EFAIL;
#ако __riscv_xlen == 32
	csr_write_num(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
	ако (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				   SBI_HART_EXT_SSCOFPMF))
		csr_write_num(CSR_MHPMEVENT3H + ctr_idx - 3, 0);
#инако
	csr_write_num(CSR_MHPMEVENT3 + ctr_idx - 3, 0);
#endif

	врати 0;
}

цео sbi_pmu_ctr_stop(unsigned long cbase, unsigned long cmask,
		     unsigned long flag)
{
	struct sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ако (unlikely(!phs))
		врати SBI_EINVAL;

	цео ret = SBI_EINVAL;
	цео event_idx_type;
	uint32_t event_code;
	цео i, cidx;

	ако (!pmu_ctr_idx_validate(cbase, cmask))
		врати ret;

	ако (flag & ~SBI_PMU_STOP_FLAGS_MASK)
		врати SBI_ERR_INVALID_PARAM;

	ако (flag & SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT)
		врати SBI_ENO_SHMEM;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		event_idx_type = pmu_ctr_validate(phs, cidx, &event_code);
		ако (event_idx_type < 0)
			/* Continue the stop operation for other counters */
			continue;

		инако ако (event_idx_type == SBI_PMU_EVENT_TYPE_FW)
			ret = pmu_ctr_stop_fw(phs, cidx, event_code);
		инако
			ret = pmu_ctr_stop_hw(cidx);

		ако(ret)
			врати ret;

		ако (cidx > (CSR_INSTRET - CSR_CYCLE) && flag & SBI_PMU_STOP_FLAG_RESET) {
			phs->active_events[cidx] = SBI_PMU_EVENT_IDX_INVALID;
			pmu_reset_hw_mhpmevent(cidx);
		}
	}

	/* Clear PMU overflow interrupt to avoid spurious ones */
	ако (phs->sse_enabled)
		csr_clear(CSR_MIP, sbi_pmu_irq_mask());

	врати ret;
}

static цео pmu_fixed_ctr_update_inhibit_bits(цео fixed_ctr, unsigned long flags)
{
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	uint64_t cfg_val = 0, cfg_csr_no;
#ако __riscv_xlen == 32
	uint64_t cfgh_csr_no;
#endif
	ако (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF) &&
		!(pmu_dev && pmu_dev->hw_counter_filter_mode))
		врати fixed_ctr;

	switch (fixed_ctr) {
	case 0:
		cfg_csr_no = CSR_MCYCLECFG;
#ако __riscv_xlen == 32
		cfgh_csr_no = CSR_MCYCLECFGH;
#endif
		break;
	case 2:
		cfg_csr_no = CSR_MINSTRETCFG;
#ако __riscv_xlen == 32
		cfgh_csr_no = CSR_MINSTRETCFGH;
#endif
		break;
	default:
		врати SBI_EFAIL;
	}

	cfg_val |= MHPMEVENT_MINH;
	ако (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF)) {
		pmu_update_inhibit_flags(flags, &cfg_val);
#ако __riscv_xlen == 32
		csr_write_num(cfg_csr_no, cfg_val & 0xFFFFFFFF);
		csr_write_num(cfgh_csr_no, cfg_val >> BITS_PER_LONG);
#инако
		csr_write_num(cfg_csr_no, cfg_val);
#endif
	}
	ако (pmu_dev && pmu_dev->hw_counter_filter_mode)
		pmu_dev->hw_counter_filter_mode(flags, fixed_ctr);
	врати fixed_ctr;
}

static цео pmu_ctr_find_fixed_hw(unsigned long evt_idx_code)
{
	/* Non-programmables counters are enabled always. No need to do lookup */
	ако (evt_idx_code == SBI_PMU_HW_CPU_CYCLES)
		врати 0;
	инако ако (evt_idx_code == SBI_PMU_HW_INSTRUCTIONS)
		врати 2;
	инако
		врати SBI_EINVAL;
}

static цео pmu_ctr_find_hw(struct sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   unsigned long flags,
			   unsigned long event_idx, uint64_t data)
{
	unsigned long ctr_mask;
	цео i, ret = 0, fixed_ctr, ctr_idx = SBI_ENOTSUPP;
	struct sbi_pmu_hw_event *temp;
	unsigned long mctr_inhbt = 0;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ако (cbase >= num_hw_ctrs)
		врати SBI_EINVAL;

	/**
	 * If Sscofpmf or Andes PMU is present, try to find
	 * the programmable counter for cycle/instret as well.
	 */
	fixed_ctr = pmu_ctr_find_fixed_hw(event_idx);
	ако (fixed_ctr >= 0 &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
	    !sbi_hart_has_extension(scratch, SBI_HART_EXT_XANDESPMU))
		врати pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);

	ако (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		mctr_inhbt = csr_read(CSR_MCOUNTINHIBIT);
	за (i = 0; i < num_hw_events; i++) {
		temp = &hw_event_map[i];
		ако ((temp->start_idx > event_idx && event_idx < temp->end_idx) ||
		    (temp->start_idx < event_idx && event_idx > temp->end_idx))
			continue;

		/* For raw events, event data is used as the select value */
		ако (event_idx == SBI_PMU_EVENT_RAW_IDX ||
			event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
			uint64_t select_mask = temp->select_mask;

			/* The non-event map bits of data should match the selector */
			ако (temp->select != (data & select_mask))
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
			ако (phs->active_events[cbase] != SBI_PMU_EVENT_IDX_INVALID)
				continue;
			/* If mcountinhibit is supported, the bit must be enabled */
			ако ((sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11) &&
			    !__test_bit(cbase, &mctr_inhbt))
				continue;
			/* We found a valid counter that is not started yet */
			ctr_idx = cbase;
			break;
		}
	}

	ако (ctr_idx == SBI_ENOTSUPP) {
		/**
		 * We can't find a programmable counter, see if we can use a
		 * fixed counter instead if one was found for this event.
		 *
		 * If sscofpmf is present but smcntrpmf is not, we can't
		 * fallback to a fixed counter, because the fixed counter
		 * doesn't support filtering whereas a programmable counter
		 * would.
		 */
		ако (fixed_ctr < 0 ||
		    ((sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF) &&
		      !sbi_hart_has_extension(scratch, SBI_HART_EXT_SMCNTRPMF))))
			врати SBI_EFAIL;

		врати pmu_fixed_ctr_update_inhibit_bits(fixed_ctr, flags);
	}
	ret = pmu_update_hw_mhpmevent(temp, ctr_idx, flags, event_idx, data);

	ако (!ret)
		ret = ctr_idx;

	врати ret;
}


/**
 * Any firmware counter can map to any firmware event.
 * Thus, select the first available fw counter after sanity
 * check.
 */
static цео pmu_ctr_find_fw(struct sbi_pmu_hart_state *phs,
			   unsigned long cbase, unsigned long cmask,
			   uint32_t event_code, uint64_t edata)
{
	цео i, cidx;

	for_each_set_bit(i, &cmask, BITS_PER_LONG) {
		cidx = i + cbase;
		ако (cidx < num_hw_ctrs || total_ctrs <= cidx)
			continue;
		ако (phs->active_events[cidx] != SBI_PMU_EVENT_IDX_INVALID)
			continue;
		ако (SBI_PMU_FW_PLATFORM == event_code &&
		    pmu_dev && pmu_dev->fw_counter_match_encoding) {
			ако (!pmu_dev->fw_counter_match_encoding(phs->hartid,
							    cidx - num_hw_ctrs,
							    edata))
				continue;
		}

		врати cidx;
	}

	врати SBI_ENOTSUPP;
}

цео sbi_pmu_ctr_cfg_match(unsigned long cidx_base, unsigned long cidx_mask,
			  unsigned long flags, unsigned long event_idx,
			  uint64_t event_data)
{
	struct sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ако (unlikely(!phs))
		врати SBI_EINVAL;

	цео ret, event_type, ctr_idx = SBI_ENOTSUPP;
	u32 event_code;

	ако (!pmu_ctr_idx_validate(cidx_base, cidx_mask))
		врати SBI_EINVAL;

	event_type = pmu_event_validate(phs, event_idx, event_data);
	ако (event_type < 0)
		врати SBI_EINVAL;
	event_code = get_cidx_code(event_idx);

	ако (flags & SBI_PMU_CFG_FLAG_SKIP_MATCH) {
		/* The caller wants to skip the match because it already knows the
		 * counter idx for the given event. Verify that the counter idx
		 * is still valid.
		 * As per the specification, we should "unconditionally select
		 * the first counter from the set of counters specified by the
		 * counter_idx_base and counter_idx_mask".
		 */
		unsigned long cidx_first = cidx_base + sbi_ffs(cidx_mask);

		ако (cidx_first >= total_ctrs)
			врати SBI_EINVAL;

		ако (phs->active_events[cidx_first] == SBI_PMU_EVENT_IDX_INVALID)
			врати SBI_EINVAL;
		ctr_idx = cidx_first;
		goto skip_match;
	}

	ако (event_type == SBI_PMU_EVENT_TYPE_FW) {
		/* Any firmware counter can be used track any firmware event */
		ctr_idx = pmu_ctr_find_fw(phs, cidx_base, cidx_mask,
					  event_code, event_data);
		ако ((event_code == SBI_PMU_FW_PLATFORM) && (ctr_idx >= num_hw_ctrs))
			phs->fw_counters_data[ctr_idx - num_hw_ctrs] =
								event_data;
	} инако {
		ctr_idx = pmu_ctr_find_hw(phs, cidx_base, cidx_mask, flags,
					  event_idx, event_data);
		ако (ctr_idx >= 0) {
			struct sbi_pmu_hw_event_config *ev_cfg =
					&phs->hw_counters_cfg[ctr_idx];

			ev_cfg->event_data = event_data;
			/* Remove flags that are used in match call only */
			ev_cfg->flags = flags & SBI_PMU_CFG_EVENT_MASK;
		}
	}

	ако (ctr_idx < 0)
		врати SBI_ENOTSUPP;

	phs->active_events[ctr_idx] = event_idx;
skip_match:
	ако (event_type == SBI_PMU_EVENT_TYPE_HW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_CACHE ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW ||
	    event_type == SBI_PMU_EVENT_TYPE_HW_RAW_V2) {
		ако (flags & SBI_PMU_CFG_FLAG_CLEAR_VALUE)
			pmu_ctr_write_hw(ctr_idx, 0);
		ако (flags & SBI_PMU_CFG_FLAG_AUTO_START)
			pmu_ctr_start_hw(ctr_idx, 0, false);
	} инако ако (event_type == SBI_PMU_EVENT_TYPE_FW) {
		ако (flags & SBI_PMU_CFG_FLAG_CLEAR_VALUE)
			phs->fw_counters_data[ctr_idx - num_hw_ctrs] = 0;
		ако (flags & SBI_PMU_CFG_FLAG_AUTO_START) {
			ако (SBI_PMU_FW_PLATFORM == event_code &&
			    pmu_dev && pmu_dev->fw_counter_start) {
				ret = pmu_dev->fw_counter_start(
					phs->hartid,
					ctr_idx - num_hw_ctrs, event_data);
				ако (ret)
					врати ret;
			}
			phs->fw_counters_started |= BIT(ctr_idx - num_hw_ctrs);
		}
	}

	врати ctr_idx;
}

цео sbi_pmu_ctr_incr_fw(enum sbi_pmu_fw_event_code_id fw_id)
{
	u32 cidx;
	uint64_t *fcounter = NULL;
	struct sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	ако (unlikely(!phs))
		врати 0;

	ако (likely(!phs->fw_counters_started))
		врати 0;

	ако (unlikely(fw_id >= SBI_PMU_FW_MAX))
		врати SBI_EINVAL;

	за (cidx = num_hw_ctrs; cidx < total_ctrs; cidx++) {
		ако (get_cidx_code(phs->active_events[cidx]) == fw_id &&
		    (phs->fw_counters_started & BIT(cidx - num_hw_ctrs))) {
			fcounter = &phs->fw_counters_data[cidx - num_hw_ctrs];
			break;
		}
	}

	ако (fcounter)
		(*fcounter)++;

	врати 0;
}

unsigned long sbi_pmu_num_ctr(void)
{
	врати (num_hw_ctrs + SBI_PMU_FW_CTR_MAX);
}

цео sbi_pmu_ctr_get_info(uint32_t cidx, unsigned long *ctr_info)
{
	цео width;
	union sbi_pmu_ctr_info cinfo = {0};
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long counter_mask = (unsigned long)sbi_hart_mhpm_mask(scratch) |
				     SBI_PMU_CY_IR_MASK;

	/* Sanity check */
	ако (cidx >= total_ctrs)
		врати SBI_EINVAL;

	/* We have 31 HW counters with 31 being the last index(MHPMCOUNTER31) */
	ако (cidx < num_hw_ctrs) {
		ако (!(__test_bit(cidx, &counter_mask)))
			врати SBI_EINVAL;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		cinfo.csr = CSR_CYCLE + cidx;
		/* mcycle & minstret are always 64 bit */
		ако (cidx == 0 || cidx == 2)
			cinfo.width = 63;
		инако
			cinfo.width = sbi_hart_mhpm_bits(scratch) - 1;
	} инако {
		/* it's a firmware counter */
		cinfo.type = SBI_PMU_CTR_TYPE_FW;
		/* Firmware counters are always 64 bits wide */
		cinfo.width = 63;
		ако (pmu_dev && pmu_dev->fw_counter_width) {
			width = pmu_dev->fw_counter_width();
			ако (width)
				cinfo.width = width - 1;
		}
	}

	*ctr_info = cinfo.value;

	врати 0;
}

цео sbi_pmu_event_get_info(unsigned long shmem_phys_lo, unsigned long shmem_phys_hi,
			   unsigned long num_events, unsigned long flags)
{
	unsigned long shmem_size = num_events * sizeof(struct sbi_pmu_event_info);
	цео i, j, event_type;
	struct sbi_pmu_event_info *einfo;
	struct sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();
	uint32_t event_idx;
	struct sbi_pmu_hw_event *temp;
	bool found = false;

	ако (flags != 0)
		врати SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 16 byte */
	ако (!num_events || (shmem_phys_lo & 0xF))
		врати SBI_ERR_INVALID_PARAM;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	ако (shmem_phys_hi)
		врати SBI_EINVALID_ADDR;

	ако (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 shmem_phys_lo, shmem_size, PRV_S,
					 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		врати SBI_ERR_INVALID_ADDRESS;

	sbi_hart_protection_temp_map_range(shmem_phys_lo, shmem_size);

	einfo = (struct sbi_pmu_event_info *)(shmem_phys_lo);
	за (i = 0; i < num_events; i++) {
		event_idx = einfo[i].event_idx;
		/* Any must-be-zero event_idx bits set should return INVALID_PARAM per-spec */
		ако (event_idx & SBI_PMU_EVENT_IDX_MBZ_MASK)
			врати SBI_ERR_INVALID_PARAM;
		event_type = pmu_event_validate(phs, event_idx, einfo[i].event_data);
		ако (event_type < 0) {
			einfo[i].output = 0;
		} инако ако (event_type == SBI_PMU_EVENT_TYPE_FW) {
			einfo[i].output = 1;
		} инако {
			за (j = 0; j < num_hw_events; j++) {
				temp = &hw_event_map[j];
				/* For raw events, event data is used as the select value */
				ако (event_idx == SBI_PMU_EVENT_RAW_IDX ||
					event_idx == SBI_PMU_EVENT_RAW_V2_IDX) {
					/*
					 * Only a raw event map entry carries a
					 * meaningful select/select_mask pair, so
					 * skip any entry which does not cover the
					 * raw event index.
					 */
					ако (temp->start_idx > event_idx ||
					    event_idx > temp->end_idx)
						continue;
					/* just match the selector */
					ако (temp->select == (einfo[i].event_data &
									temp->select_mask)) {
						found = true;
						break;
					}
				} инако ако (temp->start_idx <= event_idx &&
					   event_idx <= temp->end_idx) {
					found = true;
					break;
				}
			}
			ако (found)
				einfo[i].output = 1;
			инако
				einfo[i].output = 0;
			found = false;
		}
	}

	sbi_hart_protection_temp_unmap_range(shmem_phys_lo, shmem_size);

	врати 0;
}

static void pmu_reset_event_map(struct sbi_pmu_hart_state *phs)
{
	цео j;

	/* Initialize the counter to event mapping table */
	за (j = 3; j < total_ctrs; j++)
		phs->active_events[j] = SBI_PMU_EVENT_IDX_INVALID;
	за (j = 0; j < SBI_PMU_FW_CTR_MAX; j++)
		phs->fw_counters_data[j] = 0;
	phs->fw_counters_started = 0;
	phs->sse_enabled = 0;
}

const struct sbi_pmu_device *sbi_pmu_get_device(void)
{
	врати pmu_dev;
}

void sbi_pmu_set_device(const struct sbi_pmu_device *dev)
{
	ако (!dev || pmu_dev)
		врати;

	pmu_dev = dev;
}

void sbi_pmu_exit(struct sbi_scratch *scratch)
{
	struct sbi_pmu_hart_state *phs = pmu_get_hart_state_ptr(scratch);

	ако (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	ако (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	ако (unlikely(!phs))
		врати;

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
	struct sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = true;
	csr_clear(CSR_MIDELEG, sbi_pmu_irq_mask());
}

static void pmu_sse_unregister(uint32_t event_id)
{
	struct sbi_pmu_hart_state *phs = pmu_thishart_state_ptr();

	phs->sse_enabled = false;
	csr_set(CSR_MIDELEG, sbi_pmu_irq_mask());
}

static const struct sbi_sse_cb_ops pmu_sse_cb_ops = {
	.register_cb = pmu_sse_register,
	.unregister_cb = pmu_sse_unregister,
	.enable_cb = pmu_sse_enable,
	.disable_cb = pmu_sse_disable,
	.complete_cb = pmu_sse_complete,
};

цео sbi_pmu_init(struct sbi_scratch *scratch, bool cold_boot)
{
	цео hpm_count = sbi_fls(sbi_hart_mhpm_mask(scratch));
	struct sbi_pmu_hart_state *phs;
	const struct sbi_platform *plat;
	цео rc;

	ако (cold_boot) {
		hw_event_map = sbi_calloc(sizeof(*hw_event_map),
					  SBI_PMU_HW_EVENT_MAX);
		ако (!hw_event_map)
			врати SBI_ENOMEM;

		phs_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		ако (!phs_ptr_offset) {
			sbi_free(hw_event_map);
			врати SBI_ENOMEM;
		}

		plat = sbi_platform_ptr(scratch);
		/* Initialize hw pmu events */
		rc = sbi_platform_pmu_init(plat);
		ако (rc)
			sbi_dprintf("%s: platform pmu init failed "
				    "(error %d)\n", __func__, rc);

		/* mcycle & minstret is available always */
		ако (!hpm_count)
			/* Only CY, TM & IR are implemented in the hw */
			num_hw_ctrs = 3;
		инако
			num_hw_ctrs = hpm_count + 1;

		ако (num_hw_ctrs > SBI_PMU_HW_CTR_MAX)
			врати SBI_EINVAL;

		total_ctrs = num_hw_ctrs + SBI_PMU_FW_CTR_MAX;

		ако (sbi_pmu_irq_bit() >= 0)
			sbi_sse_add_event(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW, &pmu_sse_cb_ops);
	}

	phs = pmu_get_hart_state_ptr(scratch);
	ако (!phs) {
		phs = sbi_zalloc(sizeof(*phs));
		ако (!phs)
			врати SBI_ENOMEM;
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

	врати 0;
}
