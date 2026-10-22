/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_barrier.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_encoding.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_locks.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hartmask.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_pmu.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_timer.h>

ସଂରଚନା timer_state {
	u64 time_delta;
	spinlock_t event_list_lock;
	ସଂରଚନା sbi_dlist event_list GUARDED_BY(&event_list_lock);
	ସଂରଚନା sbi_timer_event smode_ev;
};

ସ୍ଥିର unsigned long timer_state_off;
ସ୍ଥିର u64 (*get_time_val)(ଶୂନ୍ୟ);
ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_timer_device *timer_dev = ଖାଲି;

#ଯଦି __riscv_xlen == 32
ସ୍ଥିର u64 get_ticks(ଶୂନ୍ୟ)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	ଫେରନ୍ତୁ ((u64)hi << 32) | lo;
}
#ଅନ୍ୟଥା
ସ୍ଥିର u64 get_ticks(ଶୂନ୍ୟ)
{
	unsigned long n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	ଫେରନ୍ତୁ n;
}
#endif

ସ୍ଥିର ଶୂନ୍ୟ nop_delay_fn(ଶୂନ୍ୟ *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	ଫେରନ୍ତୁ delta;
}

ଶୂନ୍ୟ sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  ଶୂନ୍ୟ (*delay_fn)(ଶୂନ୍ୟ *), ଶୂନ୍ୟ *opaque)
{
	u64 start_val, delta;

	/* Do nothing if we don't have timer device */
	ଯଦି (!timer_dev || !get_time_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		ଫେରନ୍ତୁ;
	}

	/* Save starting timer value */
	start_val = get_time_val();

	/* Use NOP delay function if delay function not available */
	ଯଦି (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	ଯେତେବେଳେ ((get_time_val() - start_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(ଶୂନ୍ୟ *), ଶୂନ୍ୟ *arg,
			    uint64_t timeout_ms)
{
	uint64_t start_time = sbi_timer_ମୂଲ୍ୟ();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	ଯେତେବେଳେ(!predicate(arg))
		ଯଦି (sbi_timer_ମୂଲ୍ୟ() - start_time  >= ticks)
			ଫେରନ୍ତୁ false;
	ଫେରନ୍ତୁ true;
}

unsigned long sbi_timer_frequency(ଶୂନ୍ୟ)
{
	ଅଚଳ ସଂରଚନା sbi_timer_device *timer_dev = sbi_timer_get_device();

	ଯଦି (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		ଫେରନ୍ତୁ 0;
	}

	ଫେରନ୍ତୁ timer_dev->timer_freq;
}

u64 sbi_timer_ମୂଲ୍ୟ(ଶୂନ୍ୟ)
{
	ଯଦି (get_time_val)
		ଫେରନ୍ତୁ get_time_val();
	ଫେରନ୍ତୁ 0;
}

u64 sbi_timer_virt_ମୂଲ୍ୟ(ଶୂନ୍ୟ)
{
	ସଂରଚନା timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	ଫେରନ୍ତୁ sbi_timer_ମୂଲ୍ୟ() + tstate->time_delta;
}

u64 sbi_timer_get_delta(ଶୂନ୍ୟ)
{
	ସଂରଚନା timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	ଫେରନ୍ତୁ tstate->time_delta;
}

ଶୂନ୍ୟ sbi_timer_set_delta(ulong delta)
{
	ସଂରଚନା timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

#ଯଦି __riscv_xlen == 32
	tstate->time_delta &= ~0xffffffffUL;
	tstate->time_delta |= (u32)delta;
#ଅନ୍ୟଥା
	tstate->time_delta = delta;
#endif
}

#ଯଦି __riscv_xlen == 32
ଶୂନ୍ୟ sbi_timer_set_delta_upper(ulong delta_upper)
{
	ସଂରଚନା timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	tstate->time_delta &= 0xffffffffUL;
	tstate->time_delta |= (u64)delta_upper << 32;
}
#endif

ସ୍ଥିର ଶୂନ୍ୟ __sbi_timer_update_device(ସଂରଚନା timer_state *tstate)
	MUST_HOLD(&tstate->event_list_lock)
{
	ସଂରଚନା sbi_timer_event *ev;

	ଯଦି (!timer_dev)
		ଫେରନ୍ତୁ;

	ଯଦି (sbi_list_empty(&tstate->event_list)) {
		ଯଦି (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} ଅନ୍ୟଥା {
		ev = sbi_list_first_entry(&tstate->event_list, ସଂରଚନା sbi_timer_event, head);
		ଯଦି (timer_dev->timer_event_start)
			timer_dev->timer_event_start(ev->time_stamp);
		csr_set(CSR_MIE, MIP_MTIP);
	}
}

ସ୍ଥିର ଶୂନ୍ୟ __sbi_timer_event_stop(ସଂରଚନା sbi_timer_event *ev)
{
	ଯଦି (ev->hart_index > -1) {
		sbi_list_del(&ev->head);
		ev->hart_index = -1;
	}
}

ସ୍ଥିର ଶୂନ୍ୟ __sbi_timer_event_start(ସଂରଚନା timer_state *tstate,
				    ସଂରଚନା sbi_timer_event *ev, u64 next_event)
	MUST_HOLD(&tstate->event_list_lock)
{
	ସଂରଚନା sbi_timer_event *tev, *next_ev = ଖାଲି;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_entry(tev, &tstate->event_list, head) {
		ଯଦି (next_event < tev->time_stamp) {
			next_ev = tev;
			ଭାଙ୍ଗ;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_index = current_hartindex();
	ev->time_stamp = next_event;
	ଯଦି (next_ev)
		sbi_list_add(&ev->head, &next_ev->head);
	ଅନ୍ୟଥା
		sbi_list_add_tail(&ev->head, &tstate->event_list);
}

ଶୂନ୍ୟ sbi_timer_event_start(ସଂରଚନା sbi_timer_event *ev, u64 next_event)
{
	ସଂରଚନା timer_state *tstate;

	ଯଦି (!ev)
		ଫେରନ୍ତୁ;

	/* Ensure that event is not on the per-HART event list */
	ଯଦି (ev->hart_index > -1) {
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

ଶୂନ୍ୟ sbi_timer_event_stop(ସଂରଚନା sbi_timer_event *ev)
{
	ସଂରଚନା timer_state *tstate;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ev_hart_index;

	ଯଦି (!ev)
		ଫେରନ୍ତୁ;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_index = ev->hart_index;
	ଯଦି (ev->hart_index > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_index),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	/* Re-program timer device on the current HART */
	ଯଦି (ev_hart_index == current_hartindex()) {
		tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_lock);
	}
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_timer_smode_event_callback(ସଂରଚନା sbi_timer_event *ev,
					   ସଂରଚନା sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	ଯଦି (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_set(CSR_MIP, MIP_STIP);
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_timer_smode_event_cleanup(ସଂରଚନା sbi_timer_event *ev)
{
	ଯଦି (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

ଶୂନ୍ୟ sbi_timer_smode_event_start(u64 next_event)
{
	ସଂରଚନା timer_state *tstate = sbi_scratch_offset_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SET_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	ଯଦି (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, next_event);
	} ଅନ୍ୟଥା {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_start(&tstate->smode_ev, next_event);
	}
}

ଶୂନ୍ୟ sbi_timer_process(ଶୂନ୍ୟ)
{
	ସଂରଚନା timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	ସଂରଚନା sbi_timer_event_restart restart;
	SBI_LIST_HEAD(restart_list);
	ସଂରଚନା sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	ଯେତେବେଳେ (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, ସଂରଚନା sbi_timer_event, head);
		ଯଦି (ev->time_stamp > sbi_timer_ମୂଲ୍ୟ())
			ଭାଙ୍ଗ;

		__sbi_timer_event_stop(ev);
		ଯଦି (ev->callback) {
			restart.required = false;
			restart.next_event = 0;
			ev->callback(ev, &restart);
			ଯଦି (restart.required) {
				ev->time_stamp = restart.next_event;
				sbi_list_add_tail(&ev->head, &restart_list);
			}
		}
	}

	ଯେତେବେଳେ (!sbi_list_empty(&restart_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, ସଂରଚନା sbi_timer_event, head);
		sbi_list_del(&ev->head);
		__sbi_timer_event_start(tstate, ev, ev->time_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

ଅଚଳ ସଂରଚନା sbi_timer_device *sbi_timer_get_device(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ timer_dev;
}

ଶୂନ୍ୟ sbi_timer_set_device(ଅଚଳ ସଂରଚନା sbi_timer_device *dev)
{
	ଯଦି (!dev || timer_dev)
		ଫେରନ୍ତୁ;

	timer_dev = dev;
	ଯଦି (!get_time_val && timer_dev->timer_ମୂଲ୍ୟ)
		get_time_val = timer_dev->timer_ମୂଲ୍ୟ;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_timer_init(ସଂରଚନା sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	ଅଚଳ ସଂରଚନା sbi_platform *plat = sbi_platform_ptr(scratch);
	ସଂରଚନା timer_state *tstate;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ଯଦି (cold_boot) {
		timer_state_off = sbi_scratch_alloc_offset(ଆକାର(*tstate));
		ଯଦି (!timer_state_off)
			ଫେରନ୍ତୁ SBI_ENOMEM;

		ଯଦି (sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME))
			get_time_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	} ଅନ୍ୟଥା {
		ଯଦି (!timer_state_off)
			ଫେରନ୍ତୁ SBI_ENOMEM;
	}

	tstate = sbi_scratch_offset_ptr(scratch, timer_state_off);
	tstate->time_delta = 0;
	SPIN_LOCK_INIT(tstate->event_list_lock);
	SBI_INIT_LIST_HEAD(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, ଖାଲି);

	ଯଦି (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ sbi_timer_exit(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	ସଂରଚନା sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	ଯେତେବେଳେ (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, ସଂରଚନା sbi_timer_event, head);
		__sbi_timer_event_stop(ev);
		ଯଦି (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}
