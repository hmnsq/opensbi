/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_barrier.h>
#অন্তর্ভাব <sbi/riscv_encoding.h>
#অন্তর্ভাব <sbi/riscv_locks.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_hartmask.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_pmu.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_timer.h>

কাঠামো timer_state {
	u64 time_delta;
	spinlock_t event_list_lock;
	কাঠামো sbi_dlist event_list GUARDED_BY(&event_list_lock);
	কাঠামো sbi_timer_event smode_ev;
};

স্থির unsigned long timer_state_off;
স্থির u64 (*get_time_val)(শূন্য);
স্থির ধ্রুবক কাঠামো sbi_timer_device *timer_dev = ফাঁকা;

#যদি __riscv_xlen == 32
স্থির u64 get_ticks(শূন্য)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	ফেরত ((u64)hi << 32) | lo;
}
#অন্যথা
স্থির u64 get_ticks(শূন্য)
{
	unsigned long n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	ফেরত n;
}
#endif

স্থির শূন্য nop_delay_fn(শূন্য *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	ফেরত delta;
}

শূন্য sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  শূন্য (*delay_fn)(শূন্য *), শূন্য *opaque)
{
	u64 start_val, delta;

	/* Do nothing if we don't have timer device */
	যদি (!timer_dev || !get_time_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		ফেরত;
	}

	/* Save starting timer value */
	start_val = get_time_val();

	/* Use NOP delay function if delay function not available */
	যদি (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	যখন ((get_time_val() - start_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(শূন্য *), শূন্য *arg,
			    uint64_t timeout_ms)
{
	uint64_t start_time = sbi_timer_মান();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	যখন(!predicate(arg))
		যদি (sbi_timer_মান() - start_time  >= ticks)
			ফেরত false;
	ফেরত true;
}

unsigned long sbi_timer_frequency(শূন্য)
{
	ধ্রুবক কাঠামো sbi_timer_device *timer_dev = sbi_timer_get_device();

	যদি (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		ফেরত 0;
	}

	ফেরত timer_dev->timer_freq;
}

u64 sbi_timer_মান(শূন্য)
{
	যদি (get_time_val)
		ফেরত get_time_val();
	ফেরত 0;
}

u64 sbi_timer_virt_মান(শূন্য)
{
	কাঠামো timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	ফেরত sbi_timer_মান() + tstate->time_delta;
}

u64 sbi_timer_get_delta(শূন্য)
{
	কাঠামো timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	ফেরত tstate->time_delta;
}

শূন্য sbi_timer_set_delta(ulong delta)
{
	কাঠামো timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

#যদি __riscv_xlen == 32
	tstate->time_delta &= ~0xffffffffUL;
	tstate->time_delta |= (u32)delta;
#অন্যথা
	tstate->time_delta = delta;
#endif
}

#যদি __riscv_xlen == 32
শূন্য sbi_timer_set_delta_upper(ulong delta_upper)
{
	কাঠামো timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	tstate->time_delta &= 0xffffffffUL;
	tstate->time_delta |= (u64)delta_upper << 32;
}
#endif

স্থির শূন্য __sbi_timer_update_device(কাঠামো timer_state *tstate)
	MUST_HOLD(&tstate->event_list_lock)
{
	কাঠামো sbi_timer_event *ev;

	যদি (!timer_dev)
		ফেরত;

	যদি (sbi_list_empty(&tstate->event_list)) {
		যদি (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} অন্যথা {
		ev = sbi_list_first_entry(&tstate->event_list, কাঠামো sbi_timer_event, শীর্ষ);
		যদি (timer_dev->timer_event_start)
			timer_dev->timer_event_start(ev->time_stamp);
		csr_set(CSR_MIE, MIP_MTIP);
	}
}

স্থির শূন্য __sbi_timer_event_stop(কাঠামো sbi_timer_event *ev)
{
	যদি (ev->hart_সূচক > -1) {
		sbi_list_del(&ev->শীর্ষ);
		ev->hart_সূচক = -1;
	}
}

স্থির শূন্য __sbi_timer_event_start(কাঠামো timer_state *tstate,
				    কাঠামো sbi_timer_event *ev, u64 পরবর্তী_event)
	MUST_HOLD(&tstate->event_list_lock)
{
	কাঠামো sbi_timer_event *tev, *পরবর্তী_ev = ফাঁকা;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_entry(tev, &tstate->event_list, শীর্ষ) {
		যদি (পরবর্তী_event < tev->time_stamp) {
			পরবর্তী_ev = tev;
			বিরতি;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_সূচক = current_hartindex();
	ev->time_stamp = পরবর্তী_event;
	যদি (পরবর্তী_ev)
		sbi_list_add(&ev->শীর্ষ, &পরবর্তী_ev->শীর্ষ);
	অন্যথা
		sbi_list_add_লেজ(&ev->শীর্ষ, &tstate->event_list);
}

শূন্য sbi_timer_event_start(কাঠামো sbi_timer_event *ev, u64 পরবর্তী_event)
{
	কাঠামো timer_state *tstate;

	যদি (!ev)
		ফেরত;

	/* Ensure that event is not on the per-HART event list */
	যদি (ev->hart_সূচক > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_সূচক),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	spin_lock(&tstate->event_list_lock);

	__sbi_timer_event_start(tstate, ev, পরবর্তী_event);
	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

শূন্য sbi_timer_event_stop(কাঠামো sbi_timer_event *ev)
{
	কাঠামো timer_state *tstate;
	পূর্ণসংখ্যা ev_hart_সূচক;

	যদি (!ev)
		ফেরত;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_সূচক = ev->hart_সূচক;
	যদি (ev->hart_সূচক > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_সূচক),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	/* Re-program timer device on the current HART */
	যদি (ev_hart_সূচক == current_hartindex()) {
		tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_lock);
	}
}

স্থির শূন্য sbi_timer_smode_event_callback(কাঠামো sbi_timer_event *ev,
					   কাঠামো sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	যদি (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_set(CSR_MIP, MIP_STIP);
}

স্থির শূন্য sbi_timer_smode_event_cleanup(কাঠামো sbi_timer_event *ev)
{
	যদি (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

শূন্য sbi_timer_smode_event_start(u64 পরবর্তী_event)
{
	কাঠামো timer_state *tstate = sbi_scratch_offset_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SET_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	যদি (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, পরবর্তী_event);
	} অন্যথা {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_start(&tstate->smode_ev, পরবর্তী_event);
	}
}

শূন্য sbi_timer_process(শূন্য)
{
	কাঠামো timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	কাঠামো sbi_timer_event_restart restart;
	SBI_LIST_শীর্ষ(restart_list);
	কাঠামো sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	যখন (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, কাঠামো sbi_timer_event, শীর্ষ);
		যদি (ev->time_stamp > sbi_timer_মান())
			বিরতি;

		__sbi_timer_event_stop(ev);
		যদি (ev->callback) {
			restart.required = false;
			restart.পরবর্তী_event = 0;
			ev->callback(ev, &restart);
			যদি (restart.required) {
				ev->time_stamp = restart.পরবর্তী_event;
				sbi_list_add_লেজ(&ev->শীর্ষ, &restart_list);
			}
		}
	}

	যখন (!sbi_list_empty(&restart_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, কাঠামো sbi_timer_event, শীর্ষ);
		sbi_list_del(&ev->শীর্ষ);
		__sbi_timer_event_start(tstate, ev, ev->time_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

ধ্রুবক কাঠামো sbi_timer_device *sbi_timer_get_device(শূন্য)
{
	ফেরত timer_dev;
}

শূন্য sbi_timer_set_device(ধ্রুবক কাঠামো sbi_timer_device *dev)
{
	যদি (!dev || timer_dev)
		ফেরত;

	timer_dev = dev;
	যদি (!get_time_val && timer_dev->timer_মান)
		get_time_val = timer_dev->timer_মান;
}

পূর্ণসংখ্যা sbi_timer_init(কাঠামো sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	ধ্রুবক কাঠামো sbi_platform *plat = sbi_platform_ptr(scratch);
	কাঠামো timer_state *tstate;
	পূর্ণসংখ্যা ret;

	যদি (cold_boot) {
		timer_state_off = sbi_scratch_alloc_offset(আকার(*tstate));
		যদি (!timer_state_off)
			ফেরত SBI_ENOMEM;

		যদি (sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME))
			get_time_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		যদি (ret)
			ফেরত ret;
	} অন্যথা {
		যদি (!timer_state_off)
			ফেরত SBI_ENOMEM;
	}

	tstate = sbi_scratch_offset_ptr(scratch, timer_state_off);
	tstate->time_delta = 0;
	SPIN_LOCK_INIT(tstate->event_list_lock);
	SBI_INIT_LIST_শীর্ষ(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, ফাঁকা);

	যদি (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		যদি (ret)
			ফেরত ret;
	}

	ফেরত 0;
}

শূন্য sbi_timer_exit(কাঠামো sbi_scratch *scratch)
{
	কাঠামো timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	কাঠামো sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	যখন (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, কাঠামো sbi_timer_event, শীর্ষ);
		__sbi_timer_event_stop(ev);
		যদি (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}
