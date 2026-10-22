/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_barrier.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/riscv_locks.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hartmask.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_pmu.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_timer.h>

ساختار timer_state {
	u64 time_delta;
	spinlock_t event_list_lock;
	ساختار sbi_dlist event_list GUARDED_BY(&event_list_lock);
	ساختار sbi_timer_event smode_ev;
};

ایستا unsigned long timer_state_off;
ایستا u64 (*get_time_val)(تهی);
ایستا ثابت ساختار sbi_timer_device *timer_dev = پوچ;

#اگر __riscv_xlen == 32
ایستا u64 get_ticks(تهی)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	بازگشت ((u64)hi << 32) | lo;
}
#وگرنه
ایستا u64 get_ticks(تهی)
{
	unsigned long n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	بازگشت n;
}
#endif

ایستا تهی nop_delay_fn(تهی *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	بازگشت delta;
}

تهی sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  تهی (*delay_fn)(تهی *), تهی *opaque)
{
	u64 start_val, delta;

	/* Do nothing if we don't have timer device */
	اگر (!timer_dev || !get_time_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		بازگشت;
	}

	/* Save starting timer value */
	start_val = get_time_val();

	/* Use NOP delay function if delay function not available */
	اگر (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	وقتی ((get_time_val() - start_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(تهی *), تهی *arg,
			    uint64_t timeout_ms)
{
	uint64_t start_time = sbi_timer_مقدار();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	وقتی(!predicate(arg))
		اگر (sbi_timer_مقدار() - start_time  >= ticks)
			بازگشت false;
	بازگشت true;
}

unsigned long sbi_timer_frequency(تهی)
{
	ثابت ساختار sbi_timer_device *timer_dev = sbi_timer_get_device();

	اگر (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		بازگشت 0;
	}

	بازگشت timer_dev->timer_freq;
}

u64 sbi_timer_مقدار(تهی)
{
	اگر (get_time_val)
		بازگشت get_time_val();
	بازگشت 0;
}

u64 sbi_timer_virt_مقدار(تهی)
{
	ساختار timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	بازگشت sbi_timer_مقدار() + tstate->time_delta;
}

u64 sbi_timer_get_delta(تهی)
{
	ساختار timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	بازگشت tstate->time_delta;
}

تهی sbi_timer_set_delta(ulong delta)
{
	ساختار timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

#اگر __riscv_xlen == 32
	tstate->time_delta &= ~0xffffffffUL;
	tstate->time_delta |= (u32)delta;
#وگرنه
	tstate->time_delta = delta;
#endif
}

#اگر __riscv_xlen == 32
تهی sbi_timer_set_delta_upper(ulong delta_upper)
{
	ساختار timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	tstate->time_delta &= 0xffffffffUL;
	tstate->time_delta |= (u64)delta_upper << 32;
}
#endif

ایستا تهی __sbi_timer_update_device(ساختار timer_state *tstate)
	MUST_HOLD(&tstate->event_list_lock)
{
	ساختار sbi_timer_event *ev;

	اگر (!timer_dev)
		بازگشت;

	اگر (sbi_list_empty(&tstate->event_list)) {
		اگر (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} وگرنه {
		ev = sbi_list_first_entry(&tstate->event_list, ساختار sbi_timer_event, head);
		اگر (timer_dev->timer_event_start)
			timer_dev->timer_event_start(ev->time_stamp);
		csr_set(CSR_MIE, MIP_MTIP);
	}
}

ایستا تهی __sbi_timer_event_stop(ساختار sbi_timer_event *ev)
{
	اگر (ev->hart_index > -1) {
		sbi_list_del(&ev->head);
		ev->hart_index = -1;
	}
}

ایستا تهی __sbi_timer_event_start(ساختار timer_state *tstate,
				    ساختار sbi_timer_event *ev, u64 next_event)
	MUST_HOLD(&tstate->event_list_lock)
{
	ساختار sbi_timer_event *tev, *next_ev = پوچ;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_entry(tev, &tstate->event_list, head) {
		اگر (next_event < tev->time_stamp) {
			next_ev = tev;
			بشکن;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_index = current_hartindex();
	ev->time_stamp = next_event;
	اگر (next_ev)
		sbi_list_add(&ev->head, &next_ev->head);
	وگرنه
		sbi_list_add_tail(&ev->head, &tstate->event_list);
}

تهی sbi_timer_event_start(ساختار sbi_timer_event *ev, u64 next_event)
{
	ساختار timer_state *tstate;

	اگر (!ev)
		بازگشت;

	/* Ensure that event is not on the per-HART event list */
	اگر (ev->hart_index > -1) {
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

تهی sbi_timer_event_stop(ساختار sbi_timer_event *ev)
{
	ساختار timer_state *tstate;
	صحیح ev_hart_index;

	اگر (!ev)
		بازگشت;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_index = ev->hart_index;
	اگر (ev->hart_index > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_index),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	/* Re-program timer device on the current HART */
	اگر (ev_hart_index == current_hartindex()) {
		tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_lock);
	}
}

ایستا تهی sbi_timer_smode_event_callback(ساختار sbi_timer_event *ev,
					   ساختار sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_set(CSR_MIP, MIP_STIP);
}

ایستا تهی sbi_timer_smode_event_cleanup(ساختار sbi_timer_event *ev)
{
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

تهی sbi_timer_smode_event_start(u64 next_event)
{
	ساختار timer_state *tstate = sbi_scratch_offset_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SET_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	اگر (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, next_event);
	} وگرنه {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_start(&tstate->smode_ev, next_event);
	}
}

تهی sbi_timer_process(تهی)
{
	ساختار timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	ساختار sbi_timer_event_restart restart;
	SBI_LIST_HEAD(restart_list);
	ساختار sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	وقتی (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, ساختار sbi_timer_event, head);
		اگر (ev->time_stamp > sbi_timer_مقدار())
			بشکن;

		__sbi_timer_event_stop(ev);
		اگر (ev->callback) {
			restart.required = false;
			restart.next_event = 0;
			ev->callback(ev, &restart);
			اگر (restart.required) {
				ev->time_stamp = restart.next_event;
				sbi_list_add_tail(&ev->head, &restart_list);
			}
		}
	}

	وقتی (!sbi_list_empty(&restart_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, ساختار sbi_timer_event, head);
		sbi_list_del(&ev->head);
		__sbi_timer_event_start(tstate, ev, ev->time_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

ثابت ساختار sbi_timer_device *sbi_timer_get_device(تهی)
{
	بازگشت timer_dev;
}

تهی sbi_timer_set_device(ثابت ساختار sbi_timer_device *dev)
{
	اگر (!dev || timer_dev)
		بازگشت;

	timer_dev = dev;
	اگر (!get_time_val && timer_dev->timer_مقدار)
		get_time_val = timer_dev->timer_مقدار;
}

صحیح sbi_timer_init(ساختار sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	ثابت ساختار sbi_platform *plat = sbi_platform_ptr(scratch);
	ساختار timer_state *tstate;
	صحیح ret;

	اگر (cold_boot) {
		timer_state_off = sbi_scratch_alloc_offset(اندازه(*tstate));
		اگر (!timer_state_off)
			بازگشت SBI_ENOMEM;

		اگر (sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME))
			get_time_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		اگر (ret)
			بازگشت ret;
	} وگرنه {
		اگر (!timer_state_off)
			بازگشت SBI_ENOMEM;
	}

	tstate = sbi_scratch_offset_ptr(scratch, timer_state_off);
	tstate->time_delta = 0;
	SPIN_LOCK_INIT(tstate->event_list_lock);
	SBI_INIT_LIST_HEAD(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, پوچ);

	اگر (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		اگر (ret)
			بازگشت ret;
	}

	بازگشت 0;
}

تهی sbi_timer_exit(ساختار sbi_scratch *scratch)
{
	ساختار timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	ساختار sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	وقتی (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, ساختار sbi_timer_event, head);
		__sbi_timer_event_stop(ev);
		اگر (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}
