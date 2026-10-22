/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_barrier.h>
#അന്തര്ഭാവ <sbi/riscv_encoding.h>
#അന്തര്ഭാവ <sbi/riscv_locks.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_hartmask.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_pmu.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>

ഘടന timer_state {
	u64 time_delta;
	spinlock_t event_list_lock;
	ഘടന sbi_dlist event_list GUARDED_BY(&event_list_lock);
	ഘടന sbi_timer_event smode_ev;
};

സ്ഥിതം unsigned long timer_state_off;
സ്ഥിതം u64 (*get_time_val)(ശൂന്യം);
സ്ഥിതം സ്ഥിരം ഘടന sbi_timer_device *timer_dev = ശൂന്യ;

#എങ്കിൽ __riscv_xlen == 32
സ്ഥിതം u64 get_ticks(ശൂന്യം)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	തിരികെ ((u64)hi << 32) | lo;
}
#അല്ലെങ്കിൽ
സ്ഥിതം u64 get_ticks(ശൂന്യം)
{
	unsigned long n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	തിരികെ n;
}
#endif

സ്ഥിതം ശൂന്യം nop_delay_fn(ശൂന്യം *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	തിരികെ delta;
}

ശൂന്യം sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  ശൂന്യം (*delay_fn)(ശൂന്യം *), ശൂന്യം *opaque)
{
	u64 start_val, delta;

	/* Do nothing if we don't have timer device */
	എങ്കിൽ (!timer_dev || !get_time_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		തിരികെ;
	}

	/* Save starting timer value */
	start_val = get_time_val();

	/* Use NOP delay function if delay function not available */
	എങ്കിൽ (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	ആകുമ്പോൾ ((get_time_val() - start_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(ശൂന്യം *), ശൂന്യം *arg,
			    uint64_t timeout_ms)
{
	uint64_t start_time = sbi_timer_മൂല്യം();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	ആകുമ്പോൾ(!predicate(arg))
		എങ്കിൽ (sbi_timer_മൂല്യം() - start_time  >= ticks)
			തിരികെ false;
	തിരികെ true;
}

unsigned long sbi_timer_frequency(ശൂന്യം)
{
	സ്ഥിരം ഘടന sbi_timer_device *timer_dev = sbi_timer_get_device();

	എങ്കിൽ (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		തിരികെ 0;
	}

	തിരികെ timer_dev->timer_freq;
}

u64 sbi_timer_മൂല്യം(ശൂന്യം)
{
	എങ്കിൽ (get_time_val)
		തിരികെ get_time_val();
	തിരികെ 0;
}

u64 sbi_timer_virt_മൂല്യം(ശൂന്യം)
{
	ഘടന timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	തിരികെ sbi_timer_മൂല്യം() + tstate->time_delta;
}

u64 sbi_timer_get_delta(ശൂന്യം)
{
	ഘടന timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	തിരികെ tstate->time_delta;
}

ശൂന്യം sbi_timer_set_delta(ulong delta)
{
	ഘടന timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

#എങ്കിൽ __riscv_xlen == 32
	tstate->time_delta &= ~0xffffffffUL;
	tstate->time_delta |= (u32)delta;
#അല്ലെങ്കിൽ
	tstate->time_delta = delta;
#endif
}

#എങ്കിൽ __riscv_xlen == 32
ശൂന്യം sbi_timer_set_delta_upper(ulong delta_upper)
{
	ഘടന timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	tstate->time_delta &= 0xffffffffUL;
	tstate->time_delta |= (u64)delta_upper << 32;
}
#endif

സ്ഥിതം ശൂന്യം __sbi_timer_update_device(ഘടന timer_state *tstate)
	MUST_HOLD(&tstate->event_list_lock)
{
	ഘടന sbi_timer_event *ev;

	എങ്കിൽ (!timer_dev)
		തിരികെ;

	എങ്കിൽ (sbi_list_empty(&tstate->event_list)) {
		എങ്കിൽ (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} അല്ലെങ്കിൽ {
		ev = sbi_list_first_entry(&tstate->event_list, ഘടന sbi_timer_event, head);
		എങ്കിൽ (timer_dev->timer_event_start)
			timer_dev->timer_event_start(ev->time_stamp);
		csr_set(CSR_MIE, MIP_MTIP);
	}
}

സ്ഥിതം ശൂന്യം __sbi_timer_event_stop(ഘടന sbi_timer_event *ev)
{
	എങ്കിൽ (ev->hart_index > -1) {
		sbi_list_del(&ev->head);
		ev->hart_index = -1;
	}
}

സ്ഥിതം ശൂന്യം __sbi_timer_event_start(ഘടന timer_state *tstate,
				    ഘടന sbi_timer_event *ev, u64 next_event)
	MUST_HOLD(&tstate->event_list_lock)
{
	ഘടന sbi_timer_event *tev, *next_ev = ശൂന്യ;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_entry(tev, &tstate->event_list, head) {
		എങ്കിൽ (next_event < tev->time_stamp) {
			next_ev = tev;
			നിർത്തുക;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_index = current_hartindex();
	ev->time_stamp = next_event;
	എങ്കിൽ (next_ev)
		sbi_list_add(&ev->head, &next_ev->head);
	അല്ലെങ്കിൽ
		sbi_list_add_tail(&ev->head, &tstate->event_list);
}

ശൂന്യം sbi_timer_event_start(ഘടന sbi_timer_event *ev, u64 next_event)
{
	ഘടന timer_state *tstate;

	എങ്കിൽ (!ev)
		തിരികെ;

	/* Ensure that event is not on the per-HART event list */
	എങ്കിൽ (ev->hart_index > -1) {
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

ശൂന്യം sbi_timer_event_stop(ഘടന sbi_timer_event *ev)
{
	ഘടന timer_state *tstate;
	പൂർണ്ണസംഖ്യ ev_hart_index;

	എങ്കിൽ (!ev)
		തിരികെ;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_index = ev->hart_index;
	എങ്കിൽ (ev->hart_index > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_index),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	/* Re-program timer device on the current HART */
	എങ്കിൽ (ev_hart_index == current_hartindex()) {
		tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_lock);
	}
}

സ്ഥിതം ശൂന്യം sbi_timer_smode_event_callback(ഘടന sbi_timer_event *ev,
					   ഘടന sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	എങ്കിൽ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_set(CSR_MIP, MIP_STIP);
}

സ്ഥിതം ശൂന്യം sbi_timer_smode_event_cleanup(ഘടന sbi_timer_event *ev)
{
	എങ്കിൽ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

ശൂന്യം sbi_timer_smode_event_start(u64 next_event)
{
	ഘടന timer_state *tstate = sbi_scratch_offset_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SET_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	എങ്കിൽ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, next_event);
	} അല്ലെങ്കിൽ {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_start(&tstate->smode_ev, next_event);
	}
}

ശൂന്യം sbi_timer_process(ശൂന്യം)
{
	ഘടന timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	ഘടന sbi_timer_event_restart restart;
	SBI_LIST_HEAD(restart_list);
	ഘടന sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	ആകുമ്പോൾ (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, ഘടന sbi_timer_event, head);
		എങ്കിൽ (ev->time_stamp > sbi_timer_മൂല്യം())
			നിർത്തുക;

		__sbi_timer_event_stop(ev);
		എങ്കിൽ (ev->callback) {
			restart.required = false;
			restart.next_event = 0;
			ev->callback(ev, &restart);
			എങ്കിൽ (restart.required) {
				ev->time_stamp = restart.next_event;
				sbi_list_add_tail(&ev->head, &restart_list);
			}
		}
	}

	ആകുമ്പോൾ (!sbi_list_empty(&restart_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, ഘടന sbi_timer_event, head);
		sbi_list_del(&ev->head);
		__sbi_timer_event_start(tstate, ev, ev->time_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

സ്ഥിരം ഘടന sbi_timer_device *sbi_timer_get_device(ശൂന്യം)
{
	തിരികെ timer_dev;
}

ശൂന്യം sbi_timer_set_device(സ്ഥിരം ഘടന sbi_timer_device *dev)
{
	എങ്കിൽ (!dev || timer_dev)
		തിരികെ;

	timer_dev = dev;
	എങ്കിൽ (!get_time_val && timer_dev->timer_മൂല്യം)
		get_time_val = timer_dev->timer_മൂല്യം;
}

പൂർണ്ണസംഖ്യ sbi_timer_init(ഘടന sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	സ്ഥിരം ഘടന sbi_platform *plat = sbi_platform_ptr(scratch);
	ഘടന timer_state *tstate;
	പൂർണ്ണസംഖ്യ ret;

	എങ്കിൽ (cold_boot) {
		timer_state_off = sbi_scratch_alloc_offset(വലുപ്പം(*tstate));
		എങ്കിൽ (!timer_state_off)
			തിരികെ SBI_ENOMEM;

		എങ്കിൽ (sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME))
			get_time_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		എങ്കിൽ (ret)
			തിരികെ ret;
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (!timer_state_off)
			തിരികെ SBI_ENOMEM;
	}

	tstate = sbi_scratch_offset_ptr(scratch, timer_state_off);
	tstate->time_delta = 0;
	SPIN_LOCK_INIT(tstate->event_list_lock);
	SBI_INIT_LIST_HEAD(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, ശൂന്യ);

	എങ്കിൽ (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	തിരികെ 0;
}

ശൂന്യം sbi_timer_exit(ഘടന sbi_scratch *scratch)
{
	ഘടന timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	ഘടന sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	ആകുമ്പോൾ (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, ഘടന sbi_timer_event, head);
		__sbi_timer_event_stop(ev);
		എങ്കിൽ (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}
