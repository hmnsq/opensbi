/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_barrier.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/riscv_locks.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_hartmask.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_pmu.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_timer.h>

بنية timer_state {
	u64 time_delta;
	spinlock_t event_list_lock;
	بنية sbi_dlist event_list GUARDED_BY(&event_list_lock);
	بنية sbi_timer_event smode_ev;
};

ساكن unsigned long timer_state_off;
ساكن u64 (*get_time_val)(فراغ);
ساكن ثابت بنية sbi_timer_device *timer_dev = عدم;

#إذا __riscv_xlen == 32
ساكن u64 get_ticks(فراغ)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	إرجاع ((u64)hi << 32) | lo;
}
#وإلا
ساكن u64 get_ticks(فراغ)
{
	unsigned long n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	إرجاع n;
}
#endif

ساكن فراغ nop_delay_fn(فراغ *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	إرجاع delta;
}

فراغ sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  فراغ (*delay_fn)(فراغ *), فراغ *opaque)
{
	u64 start_val, delta;

	/* Do nothing if we don't have timer device */
	إذا (!timer_dev || !get_time_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		إرجاع;
	}

	/* Save starting timer value */
	start_val = get_time_val();

	/* Use NOP delay function if delay function not available */
	إذا (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	بينما ((get_time_val() - start_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(فراغ *), فراغ *arg,
			    uint64_t timeout_ms)
{
	uint64_t start_time = sbi_timer_قيمة();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	بينما(!predicate(arg))
		إذا (sbi_timer_قيمة() - start_time  >= ticks)
			إرجاع false;
	إرجاع true;
}

unsigned long sbi_timer_frequency(فراغ)
{
	ثابت بنية sbi_timer_device *timer_dev = sbi_timer_get_device();

	إذا (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		إرجاع 0;
	}

	إرجاع timer_dev->timer_freq;
}

u64 sbi_timer_قيمة(فراغ)
{
	إذا (get_time_val)
		إرجاع get_time_val();
	إرجاع 0;
}

u64 sbi_timer_virt_قيمة(فراغ)
{
	بنية timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	إرجاع sbi_timer_قيمة() + tstate->time_delta;
}

u64 sbi_timer_get_delta(فراغ)
{
	بنية timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	إرجاع tstate->time_delta;
}

فراغ sbi_timer_set_delta(ulong delta)
{
	بنية timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

#إذا __riscv_xlen == 32
	tstate->time_delta &= ~0xffffffffUL;
	tstate->time_delta |= (u32)delta;
#وإلا
	tstate->time_delta = delta;
#endif
}

#إذا __riscv_xlen == 32
فراغ sbi_timer_set_delta_upper(ulong delta_upper)
{
	بنية timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	tstate->time_delta &= 0xffffffffUL;
	tstate->time_delta |= (u64)delta_upper << 32;
}
#endif

ساكن فراغ __sbi_timer_update_device(بنية timer_state *tstate)
	MUST_HOLD(&tstate->event_list_lock)
{
	بنية sbi_timer_event *ev;

	إذا (!timer_dev)
		إرجاع;

	إذا (sbi_list_empty(&tstate->event_list)) {
		إذا (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} وإلا {
		ev = sbi_list_first_entry(&tstate->event_list, بنية sbi_timer_event, head);
		إذا (timer_dev->timer_event_start)
			timer_dev->timer_event_start(ev->time_stamp);
		csr_set(CSR_MIE, MIP_MTIP);
	}
}

ساكن فراغ __sbi_timer_event_stop(بنية sbi_timer_event *ev)
{
	إذا (ev->hart_index > -1) {
		sbi_list_del(&ev->head);
		ev->hart_index = -1;
	}
}

ساكن فراغ __sbi_timer_event_start(بنية timer_state *tstate,
				    بنية sbi_timer_event *ev, u64 next_event)
	MUST_HOLD(&tstate->event_list_lock)
{
	بنية sbi_timer_event *tev, *next_ev = عدم;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_entry(tev, &tstate->event_list, head) {
		إذا (next_event < tev->time_stamp) {
			next_ev = tev;
			اكسر;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_index = current_hartindex();
	ev->time_stamp = next_event;
	إذا (next_ev)
		sbi_list_add(&ev->head, &next_ev->head);
	وإلا
		sbi_list_add_tail(&ev->head, &tstate->event_list);
}

فراغ sbi_timer_event_start(بنية sbi_timer_event *ev, u64 next_event)
{
	بنية timer_state *tstate;

	إذا (!ev)
		إرجاع;

	/* Ensure that event is not on the per-HART event list */
	إذا (ev->hart_index > -1) {
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

فراغ sbi_timer_event_stop(بنية sbi_timer_event *ev)
{
	بنية timer_state *tstate;
	صحيح ev_hart_index;

	إذا (!ev)
		إرجاع;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_index = ev->hart_index;
	إذا (ev->hart_index > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_index),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	/* Re-program timer device on the current HART */
	إذا (ev_hart_index == current_hartindex()) {
		tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_lock);
	}
}

ساكن فراغ sbi_timer_smode_event_callback(بنية sbi_timer_event *ev,
					   بنية sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_set(CSR_MIP, MIP_STIP);
}

ساكن فراغ sbi_timer_smode_event_cleanup(بنية sbi_timer_event *ev)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

فراغ sbi_timer_smode_event_start(u64 next_event)
{
	بنية timer_state *tstate = sbi_scratch_offset_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SET_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	إذا (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, next_event);
	} وإلا {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_start(&tstate->smode_ev, next_event);
	}
}

فراغ sbi_timer_process(فراغ)
{
	بنية timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	بنية sbi_timer_event_restart restart;
	SBI_LIST_HEAD(restart_list);
	بنية sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	بينما (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, بنية sbi_timer_event, head);
		إذا (ev->time_stamp > sbi_timer_قيمة())
			اكسر;

		__sbi_timer_event_stop(ev);
		إذا (ev->callback) {
			restart.required = false;
			restart.next_event = 0;
			ev->callback(ev, &restart);
			إذا (restart.required) {
				ev->time_stamp = restart.next_event;
				sbi_list_add_tail(&ev->head, &restart_list);
			}
		}
	}

	بينما (!sbi_list_empty(&restart_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, بنية sbi_timer_event, head);
		sbi_list_del(&ev->head);
		__sbi_timer_event_start(tstate, ev, ev->time_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

ثابت بنية sbi_timer_device *sbi_timer_get_device(فراغ)
{
	إرجاع timer_dev;
}

فراغ sbi_timer_set_device(ثابت بنية sbi_timer_device *dev)
{
	إذا (!dev || timer_dev)
		إرجاع;

	timer_dev = dev;
	إذا (!get_time_val && timer_dev->timer_قيمة)
		get_time_val = timer_dev->timer_قيمة;
}

صحيح sbi_timer_init(بنية sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	بنية timer_state *tstate;
	صحيح ret;

	إذا (cold_boot) {
		timer_state_off = sbi_scratch_alloc_offset(حجم(*tstate));
		إذا (!timer_state_off)
			إرجاع SBI_ENOMEM;

		إذا (sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME))
			get_time_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		إذا (ret)
			إرجاع ret;
	} وإلا {
		إذا (!timer_state_off)
			إرجاع SBI_ENOMEM;
	}

	tstate = sbi_scratch_offset_ptr(scratch, timer_state_off);
	tstate->time_delta = 0;
	SPIN_LOCK_INIT(tstate->event_list_lock);
	SBI_INIT_LIST_HEAD(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, عدم);

	إذا (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		إذا (ret)
			إرجاع ret;
	}

	إرجاع 0;
}

فراغ sbi_timer_exit(بنية sbi_scratch *scratch)
{
	بنية timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	بنية sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	بينما (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, بنية sbi_timer_event, head);
		__sbi_timer_event_stop(ev);
		إذا (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}
