/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_barrier.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_locks.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hartmask.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>

ਬਣਤਰ timer_state {
	u64 ਸਮਾਂ_delta;
	spinlock_t event_list_ਤਾਲਾ;
	ਬਣਤਰ sbi_dlist event_list GUARDED_BY(&event_list_ਤਾਲਾ);
	ਬਣਤਰ sbi_timer_event smode_ev;
};

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ timer_state_off;
ਸਥਿਰ u64 (*get_ਸਮਾਂ_val)(ਖਾਲੀ);
ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_timer_device *timer_dev = ਨਿਰਾ;

#ਜੇ __riscv_xlen == 32
ਸਥਿਰ u64 get_ticks(ਖਾਲੀ)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	ਵਾਪਸ ((u64)hi << 32) | lo;
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ u64 get_ticks(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	ਵਾਪਸ n;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ nop_delay_fn(ਖਾਲੀ *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	ਵਾਪਸ delta;
}

ਖਾਲੀ sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  ਖਾਲੀ (*delay_fn)(ਖਾਲੀ *), ਖਾਲੀ *opaque)
{
	u64 ਸ਼ੁਰੂ_val, delta;

	/* Do nothing if we don't have timer device */
	ਜੇ (!timer_dev || !get_ਸਮਾਂ_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		ਵਾਪਸ;
	}

	/* Save starting timer value */
	ਸ਼ੁਰੂ_val = get_ਸਮਾਂ_val();

	/* Use NOP delay function if delay function not available */
	ਜੇ (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	ਜਦੋਂ ((get_ਸਮਾਂ_val() - ਸ਼ੁਰੂ_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(ਖਾਲੀ *), ਖਾਲੀ *arg,
			    uint64_t timeout_ms)
{
	uint64_t ਸ਼ੁਰੂ_ਸਮਾਂ = sbi_timer_ਮੁੱਲ();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	ਜਦੋਂ(!predicate(arg))
		ਜੇ (sbi_timer_ਮੁੱਲ() - ਸ਼ੁਰੂ_ਸਮਾਂ  >= ticks)
			ਵਾਪਸ false;
	ਵਾਪਸ true;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_timer_frequency(ਖਾਲੀ)
{
	ਅਚਲ ਬਣਤਰ sbi_timer_device *timer_dev = sbi_timer_get_device();

	ਜੇ (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		ਵਾਪਸ 0;
	}

	ਵਾਪਸ timer_dev->timer_freq;
}

u64 sbi_timer_ਮੁੱਲ(ਖਾਲੀ)
{
	ਜੇ (get_ਸਮਾਂ_val)
		ਵਾਪਸ get_ਸਮਾਂ_val();
	ਵਾਪਸ 0;
}

u64 sbi_timer_virt_ਮੁੱਲ(ਖਾਲੀ)
{
	ਬਣਤਰ timer_state *tstate = sbi_scratch_thishart_ਵਿੱਥ_ptr(timer_state_off);

	ਵਾਪਸ sbi_timer_ਮੁੱਲ() + tstate->ਸਮਾਂ_delta;
}

u64 sbi_timer_get_delta(ਖਾਲੀ)
{
	ਬਣਤਰ timer_state *tstate = sbi_scratch_thishart_ਵਿੱਥ_ptr(timer_state_off);

	ਵਾਪਸ tstate->ਸਮਾਂ_delta;
}

ਖਾਲੀ sbi_timer_ਸੈੱਟ_delta(ulong delta)
{
	ਬਣਤਰ timer_state *tstate = sbi_scratch_thishart_ਵਿੱਥ_ptr(timer_state_off);

#ਜੇ __riscv_xlen == 32
	tstate->ਸਮਾਂ_delta &= ~0xffffffffUL;
	tstate->ਸਮਾਂ_delta |= (u32)delta;
#ਨਹੀਂਤਾਂ
	tstate->ਸਮਾਂ_delta = delta;
#ਜੇ_ਅੰਤ
}

#ਜੇ __riscv_xlen == 32
ਖਾਲੀ sbi_timer_ਸੈੱਟ_delta_ਉੱਤਲਾ(ulong delta_ਉੱਤਲਾ)
{
	ਬਣਤਰ timer_state *tstate = sbi_scratch_thishart_ਵਿੱਥ_ptr(timer_state_off);

	tstate->ਸਮਾਂ_delta &= 0xffffffffUL;
	tstate->ਸਮਾਂ_delta |= (u64)delta_ਉੱਤਲਾ << 32;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ __sbi_timer_update_device(ਬਣਤਰ timer_state *tstate)
	MUST_HOLD(&tstate->event_list_ਤਾਲਾ)
{
	ਬਣਤਰ sbi_timer_event *ev;

	ਜੇ (!timer_dev)
		ਵਾਪਸ;

	ਜੇ (sbi_list_ਖਾਲੀ_ਥਾਵਾਂ(&tstate->event_list)) {
		ਜੇ (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} ਨਹੀਂਤਾਂ {
		ev = sbi_list_ਪਹਿਲਾ_ਇੰਦਰਾਜ(&tstate->event_list, ਬਣਤਰ sbi_timer_event, ਸਿਰ);
		ਜੇ (timer_dev->timer_event_ਸ਼ੁਰੂ)
			timer_dev->timer_event_ਸ਼ੁਰੂ(ev->ਸਮਾਂ_stamp);
		csr_ਸੈੱਟ(CSR_MIE, MIP_MTIP);
	}
}

ਸਥਿਰ ਖਾਲੀ __sbi_timer_event_stop(ਬਣਤਰ sbi_timer_event *ev)
{
	ਜੇ (ev->hart_ਸੂਚਕਾਂਕ > -1) {
		sbi_list_del(&ev->ਸਿਰ);
		ev->hart_ਸੂਚਕਾਂਕ = -1;
	}
}

ਸਥਿਰ ਖਾਲੀ __sbi_timer_event_start(ਬਣਤਰ timer_state *tstate,
				    ਬਣਤਰ sbi_timer_event *ev, u64 ਅਗਲਾ_event)
	MUST_HOLD(&tstate->event_list_ਤਾਲਾ)
{
	ਬਣਤਰ sbi_timer_event *tev, *ਅਗਲਾ_ev = ਨਿਰਾ;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_ਇੰਦਰਾਜ(tev, &tstate->event_list, ਸਿਰ) {
		ਜੇ (ਅਗਲਾ_event < tev->ਸਮਾਂ_stamp) {
			ਅਗਲਾ_ev = tev;
			ਰੋਕੋ;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_ਸੂਚਕਾਂਕ = current_hartindex();
	ev->ਸਮਾਂ_stamp = ਅਗਲਾ_event;
	ਜੇ (ਅਗਲਾ_ev)
		sbi_list_ਜੋੜੋ(&ev->ਸਿਰ, &ਅਗਲਾ_ev->ਸਿਰ);
	ਨਹੀਂਤਾਂ
		sbi_list_ਜੋੜੋ_ਪੂਛ(&ev->ਸਿਰ, &tstate->event_list);
}

ਖਾਲੀ sbi_timer_event_ਸ਼ੁਰੂ(ਬਣਤਰ sbi_timer_event *ev, u64 ਅਗਲਾ_event)
{
	ਬਣਤਰ timer_state *tstate;

	ਜੇ (!ev)
		ਵਾਪਸ;

	/* Ensure that event is not on the per-HART event list */
	ਜੇ (ev->hart_ਸੂਚਕਾਂਕ > -1) {
		tstate = sbi_scratch_ਵਿੱਥ_ptr(sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(ev->hart_ਸੂਚਕਾਂਕ),
						timer_state_off);
		spin_ਤਾਲਾ(&tstate->event_list_ਤਾਲਾ);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_ਤਾਲਾ);
	}

	tstate = sbi_scratch_thishart_ਵਿੱਥ_ptr(timer_state_off);
	spin_ਤਾਲਾ(&tstate->event_list_ਤਾਲਾ);

	__sbi_timer_event_start(tstate, ev, ਅਗਲਾ_event);
	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_ਤਾਲਾ);
}

ਖਾਲੀ sbi_timer_event_stop(ਬਣਤਰ sbi_timer_event *ev)
{
	ਬਣਤਰ timer_state *tstate;
	ਪੂਰਨ ev_hart_ਸੂਚਕਾਂਕ;

	ਜੇ (!ev)
		ਵਾਪਸ;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_ਸੂਚਕਾਂਕ = ev->hart_ਸੂਚਕਾਂਕ;
	ਜੇ (ev->hart_ਸੂਚਕਾਂਕ > -1) {
		tstate = sbi_scratch_ਵਿੱਥ_ptr(sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(ev->hart_ਸੂਚਕਾਂਕ),
						timer_state_off);
		spin_ਤਾਲਾ(&tstate->event_list_ਤਾਲਾ);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_ਤਾਲਾ);
	}

	/* Re-program timer device on the current HART */
	ਜੇ (ev_hart_ਸੂਚਕਾਂਕ == current_hartindex()) {
		tstate = sbi_scratch_thishart_ਵਿੱਥ_ptr(timer_state_off);
		spin_ਤਾਲਾ(&tstate->event_list_ਤਾਲਾ);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_ਤਾਲਾ);
	}
}

ਸਥਿਰ ਖਾਲੀ sbi_timer_smode_event_callback(ਬਣਤਰ sbi_timer_event *ev,
					   ਬਣਤਰ sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	ਜੇ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_ਸੈੱਟ(CSR_MIP, MIP_STIP);
}

ਸਥਿਰ ਖਾਲੀ sbi_timer_smode_event_cleanup(ਬਣਤਰ sbi_timer_event *ev)
{
	ਜੇ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

ਖਾਲੀ sbi_timer_smode_event_ਸ਼ੁਰੂ(u64 ਅਗਲਾ_event)
{
	ਬਣਤਰ timer_state *tstate = sbi_scratch_ਵਿੱਥ_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_ਸੈੱਟ_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	ਜੇ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, ਅਗਲਾ_event);
	} ਨਹੀਂਤਾਂ {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_ਸ਼ੁਰੂ(&tstate->smode_ev, ਅਗਲਾ_event);
	}
}

ਖਾਲੀ sbi_timer_process(ਖਾਲੀ)
{
	ਬਣਤਰ timer_state *tstate = sbi_scratch_thishart_ਵਿੱਥ_ptr(timer_state_off);
	ਬਣਤਰ sbi_timer_event_restart restart;
	SBI_LIST_ਸਿਰ(restart_list);
	ਬਣਤਰ sbi_timer_event *ev;

	spin_ਤਾਲਾ(&tstate->event_list_ਤਾਲਾ);

	ਜਦੋਂ (!sbi_list_ਖਾਲੀ_ਥਾਵਾਂ(&tstate->event_list)) {
		ev = sbi_list_ਪਹਿਲਾ_ਇੰਦਰਾਜ(&tstate->event_list, ਬਣਤਰ sbi_timer_event, ਸਿਰ);
		ਜੇ (ev->ਸਮਾਂ_stamp > sbi_timer_ਮੁੱਲ())
			ਰੋਕੋ;

		__sbi_timer_event_stop(ev);
		ਜੇ (ev->callback) {
			restart.required = false;
			restart.ਅਗਲਾ_event = 0;
			ev->callback(ev, &restart);
			ਜੇ (restart.required) {
				ev->ਸਮਾਂ_stamp = restart.ਅਗਲਾ_event;
				sbi_list_ਜੋੜੋ_ਪੂਛ(&ev->ਸਿਰ, &restart_list);
			}
		}
	}

	ਜਦੋਂ (!sbi_list_ਖਾਲੀ_ਥਾਵਾਂ(&restart_list)) {
		ev = sbi_list_ਪਹਿਲਾ_ਇੰਦਰਾਜ(&tstate->event_list, ਬਣਤਰ sbi_timer_event, ਸਿਰ);
		sbi_list_del(&ev->ਸਿਰ);
		__sbi_timer_event_start(tstate, ev, ev->ਸਮਾਂ_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_ਤਾਲਾ);
}

ਅਚਲ ਬਣਤਰ sbi_timer_device *sbi_timer_get_device(ਖਾਲੀ)
{
	ਵਾਪਸ timer_dev;
}

ਖਾਲੀ sbi_timer_ਸੈੱਟ_device(ਅਚਲ ਬਣਤਰ sbi_timer_device *dev)
{
	ਜੇ (!dev || timer_dev)
		ਵਾਪਸ;

	timer_dev = dev;
	ਜੇ (!get_ਸਮਾਂ_val && timer_dev->timer_ਮੁੱਲ)
		get_ਸਮਾਂ_val = timer_dev->timer_ਮੁੱਲ;
}

ਪੂਰਨ sbi_timer_init(ਬਣਤਰ sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_ptr(scratch);
	ਬਣਤਰ timer_state *tstate;
	ਪੂਰਨ ret;

	ਜੇ (cold_boot) {
		timer_state_off = sbi_scratch_alloc_ਵਿੱਥ(ਆਕਾਰ(*tstate));
		ਜੇ (!timer_state_off)
			ਵਾਪਸ SBI_ENOMEM;

		ਜੇ (sbi_hart_has_csr(scratch, SBI_HART_CSR_ਸਮਾਂ))
			get_ਸਮਾਂ_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	} ਨਹੀਂਤਾਂ {
		ਜੇ (!timer_state_off)
			ਵਾਪਸ SBI_ENOMEM;
	}

	tstate = sbi_scratch_ਵਿੱਥ_ptr(scratch, timer_state_off);
	tstate->ਸਮਾਂ_delta = 0;
	SPIN_ਤਾਲਾ_INIT(tstate->event_list_ਤਾਲਾ);
	SBI_INIT_LIST_ਸਿਰ(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, ਨਿਰਾ);

	ਜੇ (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਵਾਪਸ 0;
}

ਖਾਲੀ sbi_timer_exit(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ timer_state *tstate = sbi_scratch_thishart_ਵਿੱਥ_ptr(timer_state_off);
	ਬਣਤਰ sbi_timer_event *ev;

	spin_ਤਾਲਾ(&tstate->event_list_ਤਾਲਾ);

	ਜਦੋਂ (!sbi_list_ਖਾਲੀ_ਥਾਵਾਂ(&tstate->event_list)) {
		ev = sbi_list_ਪਹਿਲਾ_ਇੰਦਰਾਜ(&tstate->event_list, ਬਣਤਰ sbi_timer_event, ਸਿਰ);
		__sbi_timer_event_stop(ev);
		ਜੇ (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_ਤਾਲਾ);
}
