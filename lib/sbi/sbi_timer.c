/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_barrier.h>
#અન્તર્ભાવ <sbi/riscv_encoding.h>
#અન્તર્ભાવ <sbi/riscv_locks.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_hartmask.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_pmu.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>

સંરચના timer_state {
	u64 time_delta;
	spinlock_t event_list_lock;
	સંરચના sbi_dlist event_list GUARDED_BY(&event_list_lock);
	સંરચના sbi_timer_event smode_ev;
};

સ્થિર unsigned long timer_state_off;
સ્થિર u64 (*get_time_val)(શૂન્ય);
સ્થિર અચલ સંરચના sbi_timer_device *timer_dev = ખાલી;

#જો __riscv_xlen == 32
સ્થિર u64 get_ticks(શૂન્ય)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	પાછા ((u64)hi << 32) | lo;
}
#નહીંતર
સ્થિર u64 get_ticks(શૂન્ય)
{
	unsigned long n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	પાછા n;
}
#endif

સ્થિર શૂન્ય nop_delay_fn(શૂન્ય *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	પાછા delta;
}

શૂન્ય sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  શૂન્ય (*delay_fn)(શૂન્ય *), શૂન્ય *opaque)
{
	u64 start_val, delta;

	/* Do nothing if we don't have timer device */
	જો (!timer_dev || !get_time_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		પાછા;
	}

	/* Save starting timer value */
	start_val = get_time_val();

	/* Use NOP delay function if delay function not available */
	જો (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	જ્યારે ((get_time_val() - start_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(શૂન્ય *), શૂન્ય *arg,
			    uint64_t timeout_ms)
{
	uint64_t start_time = sbi_timer_કિંમત();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	જ્યારે(!predicate(arg))
		જો (sbi_timer_કિંમત() - start_time  >= ticks)
			પાછા false;
	પાછા true;
}

unsigned long sbi_timer_frequency(શૂન્ય)
{
	અચલ સંરચના sbi_timer_device *timer_dev = sbi_timer_get_device();

	જો (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		પાછા 0;
	}

	પાછા timer_dev->timer_freq;
}

u64 sbi_timer_કિંમત(શૂન્ય)
{
	જો (get_time_val)
		પાછા get_time_val();
	પાછા 0;
}

u64 sbi_timer_virt_કિંમત(શૂન્ય)
{
	સંરચના timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	પાછા sbi_timer_કિંમત() + tstate->time_delta;
}

u64 sbi_timer_get_delta(શૂન્ય)
{
	સંરચના timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	પાછા tstate->time_delta;
}

શૂન્ય sbi_timer_set_delta(ulong delta)
{
	સંરચના timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

#જો __riscv_xlen == 32
	tstate->time_delta &= ~0xffffffffUL;
	tstate->time_delta |= (u32)delta;
#નહીંતર
	tstate->time_delta = delta;
#endif
}

#જો __riscv_xlen == 32
શૂન્ય sbi_timer_set_delta_upper(ulong delta_upper)
{
	સંરચના timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	tstate->time_delta &= 0xffffffffUL;
	tstate->time_delta |= (u64)delta_upper << 32;
}
#endif

સ્થિર શૂન્ય __sbi_timer_update_device(સંરચના timer_state *tstate)
	MUST_HOLD(&tstate->event_list_lock)
{
	સંરચના sbi_timer_event *ev;

	જો (!timer_dev)
		પાછા;

	જો (sbi_list_empty(&tstate->event_list)) {
		જો (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} નહીંતર {
		ev = sbi_list_first_entry(&tstate->event_list, સંરચના sbi_timer_event, head);
		જો (timer_dev->timer_event_start)
			timer_dev->timer_event_start(ev->time_stamp);
		csr_set(CSR_MIE, MIP_MTIP);
	}
}

સ્થિર શૂન્ય __sbi_timer_event_stop(સંરચના sbi_timer_event *ev)
{
	જો (ev->hart_index > -1) {
		sbi_list_del(&ev->head);
		ev->hart_index = -1;
	}
}

સ્થિર શૂન્ય __sbi_timer_event_start(સંરચના timer_state *tstate,
				    સંરચના sbi_timer_event *ev, u64 next_event)
	MUST_HOLD(&tstate->event_list_lock)
{
	સંરચના sbi_timer_event *tev, *next_ev = ખાલી;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_entry(tev, &tstate->event_list, head) {
		જો (next_event < tev->time_stamp) {
			next_ev = tev;
			તોડો;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_index = current_hartindex();
	ev->time_stamp = next_event;
	જો (next_ev)
		sbi_list_add(&ev->head, &next_ev->head);
	નહીંતર
		sbi_list_add_tail(&ev->head, &tstate->event_list);
}

શૂન્ય sbi_timer_event_start(સંરચના sbi_timer_event *ev, u64 next_event)
{
	સંરચના timer_state *tstate;

	જો (!ev)
		પાછા;

	/* Ensure that event is not on the per-HART event list */
	જો (ev->hart_index > -1) {
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

શૂન્ય sbi_timer_event_stop(સંરચના sbi_timer_event *ev)
{
	સંરચના timer_state *tstate;
	પૂર્ણાંક ev_hart_index;

	જો (!ev)
		પાછા;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_index = ev->hart_index;
	જો (ev->hart_index > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_index),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	/* Re-program timer device on the current HART */
	જો (ev_hart_index == current_hartindex()) {
		tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_lock);
	}
}

સ્થિર શૂન્ય sbi_timer_smode_event_callback(સંરચના sbi_timer_event *ev,
					   સંરચના sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	જો (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_set(CSR_MIP, MIP_STIP);
}

સ્થિર શૂન્ય sbi_timer_smode_event_cleanup(સંરચના sbi_timer_event *ev)
{
	જો (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

શૂન્ય sbi_timer_smode_event_start(u64 next_event)
{
	સંરચના timer_state *tstate = sbi_scratch_offset_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SET_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	જો (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, next_event);
	} નહીંતર {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_start(&tstate->smode_ev, next_event);
	}
}

શૂન્ય sbi_timer_process(શૂન્ય)
{
	સંરચના timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	સંરચના sbi_timer_event_restart restart;
	SBI_LIST_HEAD(restart_list);
	સંરચના sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	જ્યારે (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, સંરચના sbi_timer_event, head);
		જો (ev->time_stamp > sbi_timer_કિંમત())
			તોડો;

		__sbi_timer_event_stop(ev);
		જો (ev->callback) {
			restart.required = false;
			restart.next_event = 0;
			ev->callback(ev, &restart);
			જો (restart.required) {
				ev->time_stamp = restart.next_event;
				sbi_list_add_tail(&ev->head, &restart_list);
			}
		}
	}

	જ્યારે (!sbi_list_empty(&restart_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, સંરચના sbi_timer_event, head);
		sbi_list_del(&ev->head);
		__sbi_timer_event_start(tstate, ev, ev->time_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

અચલ સંરચના sbi_timer_device *sbi_timer_get_device(શૂન્ય)
{
	પાછા timer_dev;
}

શૂન્ય sbi_timer_set_device(અચલ સંરચના sbi_timer_device *dev)
{
	જો (!dev || timer_dev)
		પાછા;

	timer_dev = dev;
	જો (!get_time_val && timer_dev->timer_કિંમત)
		get_time_val = timer_dev->timer_કિંમત;
}

પૂર્ણાંક sbi_timer_init(સંરચના sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	અચલ સંરચના sbi_platform *plat = sbi_platform_ptr(scratch);
	સંરચના timer_state *tstate;
	પૂર્ણાંક ret;

	જો (cold_boot) {
		timer_state_off = sbi_scratch_alloc_offset(કદ(*tstate));
		જો (!timer_state_off)
			પાછા SBI_ENOMEM;

		જો (sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME))
			get_time_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		જો (ret)
			પાછા ret;
	} નહીંતર {
		જો (!timer_state_off)
			પાછા SBI_ENOMEM;
	}

	tstate = sbi_scratch_offset_ptr(scratch, timer_state_off);
	tstate->time_delta = 0;
	SPIN_LOCK_INIT(tstate->event_list_lock);
	SBI_INIT_LIST_HEAD(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, ખાલી);

	જો (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		જો (ret)
			પાછા ret;
	}

	પાછા 0;
}

શૂન્ય sbi_timer_exit(સંરચના sbi_scratch *scratch)
{
	સંરચના timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	સંરચના sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	જ્યારે (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, સંરચના sbi_timer_event, head);
		__sbi_timer_event_stop(ev);
		જો (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}
