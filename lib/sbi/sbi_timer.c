/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_barrier.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/riscv_locks.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_hartmask.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_pmu.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_timer.h>

संरचना timer_state {
	u64 time_delta;
	spinlock_t event_list_lock;
	संरचना sbi_dlist event_list GUARDED_BY(&event_list_lock);
	संरचना sbi_timer_event smode_ev;
};

स्थिर अचिन्हित दीर्घ timer_state_off;
स्थिर u64 (*get_time_val)(शून्य);
स्थिर स्थिरांक संरचना sbi_timer_device *timer_dev = रिक्त;

#जर __riscv_xlen == 32
स्थिर u64 get_ticks(शून्य)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	परत ((u64)hi << 32) | lo;
}
#अन्यथा
स्थिर u64 get_ticks(शून्य)
{
	अचिन्हित दीर्घ n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	परत n;
}
#endif

स्थिर शून्य nop_delay_fn(शून्य *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	परत delta;
}

शून्य sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  शून्य (*delay_fn)(शून्य *), शून्य *opaque)
{
	u64 start_val, delta;

	/* Do nothing if we don't have timer device */
	जर (!timer_dev || !get_time_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		परत;
	}

	/* Save starting timer value */
	start_val = get_time_val();

	/* Use NOP delay function if delay function not available */
	जर (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	पर्यंत ((get_time_val() - start_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(शून्य *), शून्य *arg,
			    uint64_t timeout_ms)
{
	uint64_t start_time = sbi_timer_मूल्य();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	पर्यंत(!predicate(arg))
		जर (sbi_timer_मूल्य() - start_time  >= ticks)
			परत false;
	परत true;
}

अचिन्हित दीर्घ sbi_timer_frequency(शून्य)
{
	स्थिरांक संरचना sbi_timer_device *timer_dev = sbi_timer_get_device();

	जर (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		परत 0;
	}

	परत timer_dev->timer_freq;
}

u64 sbi_timer_मूल्य(शून्य)
{
	जर (get_time_val)
		परत get_time_val();
	परत 0;
}

u64 sbi_timer_virt_मूल्य(शून्य)
{
	संरचना timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	परत sbi_timer_मूल्य() + tstate->time_delta;
}

u64 sbi_timer_get_delta(शून्य)
{
	संरचना timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	परत tstate->time_delta;
}

शून्य sbi_timer_set_delta(ulong delta)
{
	संरचना timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

#जर __riscv_xlen == 32
	tstate->time_delta &= ~0xffffffffUL;
	tstate->time_delta |= (u32)delta;
#अन्यथा
	tstate->time_delta = delta;
#endif
}

#जर __riscv_xlen == 32
शून्य sbi_timer_set_delta_upper(ulong delta_upper)
{
	संरचना timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	tstate->time_delta &= 0xffffffffUL;
	tstate->time_delta |= (u64)delta_upper << 32;
}
#endif

स्थिर शून्य __sbi_timer_update_device(संरचना timer_state *tstate)
	MUST_HOLD(&tstate->event_list_lock)
{
	संरचना sbi_timer_event *ev;

	जर (!timer_dev)
		परत;

	जर (sbi_list_empty(&tstate->event_list)) {
		जर (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} अन्यथा {
		ev = sbi_list_first_entry(&tstate->event_list, संरचना sbi_timer_event, head);
		जर (timer_dev->timer_event_start)
			timer_dev->timer_event_start(ev->time_stamp);
		csr_set(CSR_MIE, MIP_MTIP);
	}
}

स्थिर शून्य __sbi_timer_event_stop(संरचना sbi_timer_event *ev)
{
	जर (ev->hart_index > -1) {
		sbi_list_del(&ev->head);
		ev->hart_index = -1;
	}
}

स्थिर शून्य __sbi_timer_event_start(संरचना timer_state *tstate,
				    संरचना sbi_timer_event *ev, u64 next_event)
	MUST_HOLD(&tstate->event_list_lock)
{
	संरचना sbi_timer_event *tev, *next_ev = रिक्त;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_entry(tev, &tstate->event_list, head) {
		जर (next_event < tev->time_stamp) {
			next_ev = tev;
			थांबा;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_index = current_hartindex();
	ev->time_stamp = next_event;
	जर (next_ev)
		sbi_list_add(&ev->head, &next_ev->head);
	अन्यथा
		sbi_list_add_tail(&ev->head, &tstate->event_list);
}

शून्य sbi_timer_event_start(संरचना sbi_timer_event *ev, u64 next_event)
{
	संरचना timer_state *tstate;

	जर (!ev)
		परत;

	/* Ensure that event is not on the per-HART event list */
	जर (ev->hart_index > -1) {
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

शून्य sbi_timer_event_stop(संरचना sbi_timer_event *ev)
{
	संरचना timer_state *tstate;
	पूर्णांक ev_hart_index;

	जर (!ev)
		परत;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_index = ev->hart_index;
	जर (ev->hart_index > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_index),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	/* Re-program timer device on the current HART */
	जर (ev_hart_index == current_hartindex()) {
		tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_lock);
	}
}

स्थिर शून्य sbi_timer_smode_event_callback(संरचना sbi_timer_event *ev,
					   संरचना sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	जर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_set(CSR_MIP, MIP_STIP);
}

स्थिर शून्य sbi_timer_smode_event_cleanup(संरचना sbi_timer_event *ev)
{
	जर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

शून्य sbi_timer_smode_event_start(u64 next_event)
{
	संरचना timer_state *tstate = sbi_scratch_offset_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SET_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	जर (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, next_event);
	} अन्यथा {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_start(&tstate->smode_ev, next_event);
	}
}

शून्य sbi_timer_process(शून्य)
{
	संरचना timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	संरचना sbi_timer_event_restart restart;
	SBI_LIST_HEAD(restart_list);
	संरचना sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	पर्यंत (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, संरचना sbi_timer_event, head);
		जर (ev->time_stamp > sbi_timer_मूल्य())
			थांबा;

		__sbi_timer_event_stop(ev);
		जर (ev->callback) {
			restart.required = false;
			restart.next_event = 0;
			ev->callback(ev, &restart);
			जर (restart.required) {
				ev->time_stamp = restart.next_event;
				sbi_list_add_tail(&ev->head, &restart_list);
			}
		}
	}

	पर्यंत (!sbi_list_empty(&restart_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, संरचना sbi_timer_event, head);
		sbi_list_del(&ev->head);
		__sbi_timer_event_start(tstate, ev, ev->time_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

स्थिरांक संरचना sbi_timer_device *sbi_timer_get_device(शून्य)
{
	परत timer_dev;
}

शून्य sbi_timer_set_device(स्थिरांक संरचना sbi_timer_device *dev)
{
	जर (!dev || timer_dev)
		परत;

	timer_dev = dev;
	जर (!get_time_val && timer_dev->timer_मूल्य)
		get_time_val = timer_dev->timer_मूल्य;
}

पूर्णांक sbi_timer_init(संरचना sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	स्थिरांक संरचना sbi_platform *plat = sbi_platform_ptr(scratch);
	संरचना timer_state *tstate;
	पूर्णांक ret;

	जर (cold_boot) {
		timer_state_off = sbi_scratch_alloc_offset(आकार(*tstate));
		जर (!timer_state_off)
			परत SBI_ENOMEM;

		जर (sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME))
			get_time_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		जर (ret)
			परत ret;
	} अन्यथा {
		जर (!timer_state_off)
			परत SBI_ENOMEM;
	}

	tstate = sbi_scratch_offset_ptr(scratch, timer_state_off);
	tstate->time_delta = 0;
	SPIN_LOCK_INIT(tstate->event_list_lock);
	SBI_INIT_LIST_HEAD(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, रिक्त);

	जर (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		जर (ret)
			परत ret;
	}

	परत 0;
}

शून्य sbi_timer_exit(संरचना sbi_scratch *scratch)
{
	संरचना timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	संरचना sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	पर्यंत (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, संरचना sbi_timer_event, head);
		__sbi_timer_event_stop(ev);
		जर (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}
