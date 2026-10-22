/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_barrier.h>
#அந்தர்பாவ <sbi/riscv_encoding.h>
#அந்தர்பாவ <sbi/riscv_locks.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_hartmask.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_pmu.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_timer.h>

கட்டமைப்பு timer_state {
	u64 time_delta;
	spinlock_t event_list_lock;
	கட்டமைப்பு sbi_dlist event_list GUARDED_BY(&event_list_lock);
	கட்டமைப்பு sbi_timer_event smode_ev;
};

நிலையான unsigned long timer_state_off;
நிலையான u64 (*get_time_val)(வெறுமை);
நிலையான மாறிலி கட்டமைப்பு sbi_timer_device *timer_dev = வெற்று;

#எனில் __riscv_xlen == 32
நிலையான u64 get_ticks(வெறுமை)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	திரும்பு ((u64)hi << 32) | lo;
}
#இல்லையெனில்
நிலையான u64 get_ticks(வெறுமை)
{
	unsigned long n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	திரும்பு n;
}
#endif

நிலையான வெறுமை nop_delay_fn(வெறுமை *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	திரும்பு delta;
}

வெறுமை sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  வெறுமை (*delay_fn)(வெறுமை *), வெறுமை *opaque)
{
	u64 start_val, delta;

	/* Do nothing if we don't have timer device */
	எனில் (!timer_dev || !get_time_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		திரும்பு;
	}

	/* Save starting timer value */
	start_val = get_time_val();

	/* Use NOP delay function if delay function not available */
	எனில் (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	போது ((get_time_val() - start_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(வெறுமை *), வெறுமை *arg,
			    uint64_t timeout_ms)
{
	uint64_t start_time = sbi_timer_மதிப்பு();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	போது(!predicate(arg))
		எனில் (sbi_timer_மதிப்பு() - start_time  >= ticks)
			திரும்பு false;
	திரும்பு true;
}

unsigned long sbi_timer_frequency(வெறுமை)
{
	மாறிலி கட்டமைப்பு sbi_timer_device *timer_dev = sbi_timer_get_device();

	எனில் (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		திரும்பு 0;
	}

	திரும்பு timer_dev->timer_freq;
}

u64 sbi_timer_மதிப்பு(வெறுமை)
{
	எனில் (get_time_val)
		திரும்பு get_time_val();
	திரும்பு 0;
}

u64 sbi_timer_virt_மதிப்பு(வெறுமை)
{
	கட்டமைப்பு timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	திரும்பு sbi_timer_மதிப்பு() + tstate->time_delta;
}

u64 sbi_timer_get_delta(வெறுமை)
{
	கட்டமைப்பு timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	திரும்பு tstate->time_delta;
}

வெறுமை sbi_timer_set_delta(ulong delta)
{
	கட்டமைப்பு timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

#எனில் __riscv_xlen == 32
	tstate->time_delta &= ~0xffffffffUL;
	tstate->time_delta |= (u32)delta;
#இல்லையெனில்
	tstate->time_delta = delta;
#endif
}

#எனில் __riscv_xlen == 32
வெறுமை sbi_timer_set_delta_upper(ulong delta_upper)
{
	கட்டமைப்பு timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	tstate->time_delta &= 0xffffffffUL;
	tstate->time_delta |= (u64)delta_upper << 32;
}
#endif

நிலையான வெறுமை __sbi_timer_update_device(கட்டமைப்பு timer_state *tstate)
	MUST_HOLD(&tstate->event_list_lock)
{
	கட்டமைப்பு sbi_timer_event *ev;

	எனில் (!timer_dev)
		திரும்பு;

	எனில் (sbi_list_empty(&tstate->event_list)) {
		எனில் (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} இல்லையெனில் {
		ev = sbi_list_first_entry(&tstate->event_list, கட்டமைப்பு sbi_timer_event, head);
		எனில் (timer_dev->timer_event_start)
			timer_dev->timer_event_start(ev->time_stamp);
		csr_set(CSR_MIE, MIP_MTIP);
	}
}

நிலையான வெறுமை __sbi_timer_event_stop(கட்டமைப்பு sbi_timer_event *ev)
{
	எனில் (ev->hart_index > -1) {
		sbi_list_del(&ev->head);
		ev->hart_index = -1;
	}
}

நிலையான வெறுமை __sbi_timer_event_start(கட்டமைப்பு timer_state *tstate,
				    கட்டமைப்பு sbi_timer_event *ev, u64 next_event)
	MUST_HOLD(&tstate->event_list_lock)
{
	கட்டமைப்பு sbi_timer_event *tev, *next_ev = வெற்று;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_entry(tev, &tstate->event_list, head) {
		எனில் (next_event < tev->time_stamp) {
			next_ev = tev;
			நிறுத்து;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_index = current_hartindex();
	ev->time_stamp = next_event;
	எனில் (next_ev)
		sbi_list_add(&ev->head, &next_ev->head);
	இல்லையெனில்
		sbi_list_add_tail(&ev->head, &tstate->event_list);
}

வெறுமை sbi_timer_event_start(கட்டமைப்பு sbi_timer_event *ev, u64 next_event)
{
	கட்டமைப்பு timer_state *tstate;

	எனில் (!ev)
		திரும்பு;

	/* Ensure that event is not on the per-HART event list */
	எனில் (ev->hart_index > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_index),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	spin_lock(&tstate->event_list_lock);

	__sbi_timer_event_start(tstate, ev, next_event);
	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

வெறுமை sbi_timer_event_stop(கட்டமைப்பு sbi_timer_event *ev)
{
	கட்டமைப்பு timer_state *tstate;
	முழு ev_hart_index;

	எனில் (!ev)
		திரும்பு;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_index = ev->hart_index;
	எனில் (ev->hart_index > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_index),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	/* Re-program timer device on the current HART */
	எனில் (ev_hart_index == current_hartindex()) {
		tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_lock);
	}
}

நிலையான வெறுமை sbi_timer_smode_event_callback(கட்டமைப்பு sbi_timer_event *ev,
					   கட்டமைப்பு sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	எனில் (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_set(CSR_MIP, MIP_STIP);
}

நிலையான வெறுமை sbi_timer_smode_event_cleanup(கட்டமைப்பு sbi_timer_event *ev)
{
	எனில் (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

வெறுமை sbi_timer_smode_event_start(u64 next_event)
{
	கட்டமைப்பு timer_state *tstate = sbi_scratch_offset_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SET_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	எனில் (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, next_event);
	} இல்லையெனில் {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_start(&tstate->smode_ev, next_event);
	}
}

வெறுமை sbi_timer_process(வெறுமை)
{
	கட்டமைப்பு timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	கட்டமைப்பு sbi_timer_event_restart restart;
	SBI_LIST_HEAD(restart_list);
	கட்டமைப்பு sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	போது (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, கட்டமைப்பு sbi_timer_event, head);
		எனில் (ev->time_stamp > sbi_timer_மதிப்பு())
			நிறுத்து;

		__sbi_timer_event_stop(ev);
		எனில் (ev->callback) {
			restart.required = false;
			restart.next_event = 0;
			ev->callback(ev, &restart);
			எனில் (restart.required) {
				ev->time_stamp = restart.next_event;
				sbi_list_add_tail(&ev->head, &restart_list);
			}
		}
	}

	போது (!sbi_list_empty(&restart_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, கட்டமைப்பு sbi_timer_event, head);
		sbi_list_del(&ev->head);
		__sbi_timer_event_start(tstate, ev, ev->time_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

மாறிலி கட்டமைப்பு sbi_timer_device *sbi_timer_get_device(வெறுமை)
{
	திரும்பு timer_dev;
}

வெறுமை sbi_timer_set_device(மாறிலி கட்டமைப்பு sbi_timer_device *dev)
{
	எனில் (!dev || timer_dev)
		திரும்பு;

	timer_dev = dev;
	எனில் (!get_time_val && timer_dev->timer_மதிப்பு)
		get_time_val = timer_dev->timer_மதிப்பு;
}

முழு sbi_timer_init(கட்டமைப்பு sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	மாறிலி கட்டமைப்பு sbi_platform *plat = sbi_platform_ptr(scratch);
	கட்டமைப்பு timer_state *tstate;
	முழு ret;

	எனில் (cold_boot) {
		timer_state_off = sbi_scratch_alloc_offset(அளவு(*tstate));
		எனில் (!timer_state_off)
			திரும்பு SBI_ENOMEM;

		எனில் (sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME))
			get_time_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		எனில் (ret)
			திரும்பு ret;
	} இல்லையெனில் {
		எனில் (!timer_state_off)
			திரும்பு SBI_ENOMEM;
	}

	tstate = sbi_scratch_offset_ptr(scratch, timer_state_off);
	tstate->time_delta = 0;
	SPIN_LOCK_INIT(tstate->event_list_lock);
	SBI_INIT_LIST_HEAD(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, வெற்று);

	எனில் (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		எனில் (ret)
			திரும்பு ret;
	}

	திரும்பு 0;
}

வெறுமை sbi_timer_exit(கட்டமைப்பு sbi_scratch *scratch)
{
	கட்டமைப்பு timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	கட்டமைப்பு sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	போது (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, கட்டமைப்பு sbi_timer_event, head);
		__sbi_timer_event_stop(ev);
		எனில் (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}
