/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_barrier.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_locks.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hartmask.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmu.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>

ಸಂರಚನಾ timer_state {
	u64 time_delta;
	spinlock_t event_list_lock;
	ಸಂರಚನಾ sbi_dlist event_list GUARDED_BY(&event_list_lock);
	ಸಂರಚನಾ sbi_timer_event smode_ev;
};

ಸ್ಥಿರ unsigned long timer_state_off;
ಸ್ಥಿರ u64 (*get_time_val)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sbi_timer_device *timer_dev = ರಿಕ್ತ;

#ಯದಿ __riscv_xlen == 32
ಸ್ಥಿರ u64 get_ticks(ಶೂನ್ಯ)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	ಪ್ರತ್ಯಾಗಮ ((u64)hi << 32) | lo;
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ u64 get_ticks(ಶೂನ್ಯ)
{
	unsigned long n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	ಪ್ರತ್ಯಾಗಮ n;
}
#endif

ಸ್ಥಿರ ಶೂನ್ಯ nop_delay_fn(ಶೂನ್ಯ *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	ಪ್ರತ್ಯಾಗಮ delta;
}

ಶೂನ್ಯ sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  ಶೂನ್ಯ (*delay_fn)(ಶೂನ್ಯ *), ಶೂನ್ಯ *opaque)
{
	u64 start_val, delta;

	/* Do nothing if we don't have timer device */
	ಯದಿ (!timer_dev || !get_time_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		ಪ್ರತ್ಯಾಗಮ;
	}

	/* Save starting timer value */
	start_val = get_time_val();

	/* Use NOP delay function if delay function not available */
	ಯದಿ (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	ಯಾವತ್ ((get_time_val() - start_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(ಶೂನ್ಯ *), ಶೂನ್ಯ *arg,
			    uint64_t timeout_ms)
{
	uint64_t start_time = sbi_timer_ಮೌಲ್ಯ();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	ಯಾವತ್(!predicate(arg))
		ಯದಿ (sbi_timer_ಮೌಲ್ಯ() - start_time  >= ticks)
			ಪ್ರತ್ಯಾಗಮ false;
	ಪ್ರತ್ಯಾಗಮ true;
}

unsigned long sbi_timer_frequency(ಶೂನ್ಯ)
{
	ಧ್ರುವ ಸಂರಚನಾ sbi_timer_device *timer_dev = sbi_timer_get_device();

	ಯದಿ (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಪ್ರತ್ಯಾಗಮ timer_dev->timer_freq;
}

u64 sbi_timer_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
	ಯದಿ (get_time_val)
		ಪ್ರತ್ಯಾಗಮ get_time_val();
	ಪ್ರತ್ಯಾಗಮ 0;
}

u64 sbi_timer_virt_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	ಪ್ರತ್ಯಾಗಮ sbi_timer_ಮೌಲ್ಯ() + tstate->time_delta;
}

u64 sbi_timer_get_delta(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	ಪ್ರತ್ಯಾಗಮ tstate->time_delta;
}

ಶೂನ್ಯ sbi_timer_set_delta(ulong delta)
{
	ಸಂರಚನಾ timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

#ಯದಿ __riscv_xlen == 32
	tstate->time_delta &= ~0xffffffffUL;
	tstate->time_delta |= (u32)delta;
#ಅನ್ಯಥಾ
	tstate->time_delta = delta;
#endif
}

#ಯದಿ __riscv_xlen == 32
ಶೂನ್ಯ sbi_timer_set_delta_upper(ulong delta_upper)
{
	ಸಂರಚನಾ timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	tstate->time_delta &= 0xffffffffUL;
	tstate->time_delta |= (u64)delta_upper << 32;
}
#endif

ಸ್ಥಿರ ಶೂನ್ಯ __sbi_timer_update_device(ಸಂರಚನಾ timer_state *tstate)
	MUST_HOLD(&tstate->event_list_lock)
{
	ಸಂರಚನಾ sbi_timer_event *ev;

	ಯದಿ (!timer_dev)
		ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (sbi_list_empty(&tstate->event_list)) {
		ಯದಿ (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} ಅನ್ಯಥಾ {
		ev = sbi_list_first_entry(&tstate->event_list, ಸಂರಚನಾ sbi_timer_event, head);
		ಯದಿ (timer_dev->timer_event_start)
			timer_dev->timer_event_start(ev->time_stamp);
		csr_set(CSR_MIE, MIP_MTIP);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ __sbi_timer_event_stop(ಸಂರಚನಾ sbi_timer_event *ev)
{
	ಯದಿ (ev->hart_index > -1) {
		sbi_list_del(&ev->head);
		ev->hart_index = -1;
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ __sbi_timer_event_start(ಸಂರಚನಾ timer_state *tstate,
				    ಸಂರಚನಾ sbi_timer_event *ev, u64 next_event)
	MUST_HOLD(&tstate->event_list_lock)
{
	ಸಂರಚನಾ sbi_timer_event *tev, *next_ev = ರಿಕ್ತ;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_entry(tev, &tstate->event_list, head) {
		ಯದಿ (next_event < tev->time_stamp) {
			next_ev = tev;
			ಭಙ್ಗ;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_index = current_hartindex();
	ev->time_stamp = next_event;
	ಯದಿ (next_ev)
		sbi_list_add(&ev->head, &next_ev->head);
	ಅನ್ಯಥಾ
		sbi_list_add_tail(&ev->head, &tstate->event_list);
}

ಶೂನ್ಯ sbi_timer_event_start(ಸಂರಚನಾ sbi_timer_event *ev, u64 next_event)
{
	ಸಂರಚನಾ timer_state *tstate;

	ಯದಿ (!ev)
		ಪ್ರತ್ಯಾಗಮ;

	/* Ensure that event is not on the per-HART event list */
	ಯದಿ (ev->hart_index > -1) {
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

ಶೂನ್ಯ sbi_timer_event_stop(ಸಂರಚನಾ sbi_timer_event *ev)
{
	ಸಂರಚನಾ timer_state *tstate;
	ಪೂರ್ಣಾಙ್ಕ ev_hart_index;

	ಯದಿ (!ev)
		ಪ್ರತ್ಯಾಗಮ;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_index = ev->hart_index;
	ಯದಿ (ev->hart_index > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_index),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	/* Re-program timer device on the current HART */
	ಯದಿ (ev_hart_index == current_hartindex()) {
		tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_lock);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_timer_smode_event_callback(ಸಂರಚನಾ sbi_timer_event *ev,
					   ಸಂರಚನಾ sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	ಯದಿ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_set(CSR_MIP, MIP_STIP);
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_timer_smode_event_cleanup(ಸಂರಚನಾ sbi_timer_event *ev)
{
	ಯದಿ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

ಶೂನ್ಯ sbi_timer_smode_event_start(u64 next_event)
{
	ಸಂರಚನಾ timer_state *tstate = sbi_scratch_offset_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SET_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	ಯದಿ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, next_event);
	} ಅನ್ಯಥಾ {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_start(&tstate->smode_ev, next_event);
	}
}

ಶೂನ್ಯ sbi_timer_process(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	ಸಂರಚನಾ sbi_timer_event_restart restart;
	SBI_LIST_HEAD(restart_list);
	ಸಂರಚನಾ sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	ಯಾವತ್ (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, ಸಂರಚನಾ sbi_timer_event, head);
		ಯದಿ (ev->time_stamp > sbi_timer_ಮೌಲ್ಯ())
			ಭಙ್ಗ;

		__sbi_timer_event_stop(ev);
		ಯದಿ (ev->callback) {
			restart.required = false;
			restart.next_event = 0;
			ev->callback(ev, &restart);
			ಯದಿ (restart.required) {
				ev->time_stamp = restart.next_event;
				sbi_list_add_tail(&ev->head, &restart_list);
			}
		}
	}

	ಯಾವತ್ (!sbi_list_empty(&restart_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, ಸಂರಚನಾ sbi_timer_event, head);
		sbi_list_del(&ev->head);
		__sbi_timer_event_start(tstate, ev, ev->time_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

ಧ್ರುವ ಸಂರಚನಾ sbi_timer_device *sbi_timer_get_device(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ timer_dev;
}

ಶೂನ್ಯ sbi_timer_set_device(ಧ್ರುವ ಸಂರಚನಾ sbi_timer_device *dev)
{
	ಯದಿ (!dev || timer_dev)
		ಪ್ರತ್ಯಾಗಮ;

	timer_dev = dev;
	ಯದಿ (!get_time_val && timer_dev->timer_ಮೌಲ್ಯ)
		get_time_val = timer_dev->timer_ಮೌಲ್ಯ;
}

ಪೂರ್ಣಾಙ್ಕ sbi_timer_init(ಸಂರಚನಾ sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	ಧ್ರುವ ಸಂರಚನಾ sbi_platform *plat = sbi_platform_ptr(scratch);
	ಸಂರಚನಾ timer_state *tstate;
	ಪೂರ್ಣಾಙ್ಕ ret;

	ಯದಿ (cold_boot) {
		timer_state_off = sbi_scratch_alloc_offset(ಪರಿಮಾಣ(*tstate));
		ಯದಿ (!timer_state_off)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

		ಯದಿ (sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME))
			get_time_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	} ಅನ್ಯಥಾ {
		ಯದಿ (!timer_state_off)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	}

	tstate = sbi_scratch_offset_ptr(scratch, timer_state_off);
	tstate->time_delta = 0;
	SPIN_LOCK_INIT(tstate->event_list_lock);
	SBI_INIT_LIST_HEAD(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, ರಿಕ್ತ);

	ಯದಿ (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ sbi_timer_exit(ಸಂರಚನಾ sbi_scratch *scratch)
{
	ಸಂರಚನಾ timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	ಸಂರಚನಾ sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	ಯಾವತ್ (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, ಸಂರಚನಾ sbi_timer_event, head);
		__sbi_timer_event_stop(ev);
		ಯದಿ (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}
