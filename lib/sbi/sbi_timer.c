/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_barrier.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/riscv_locks.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_hartmask.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_pmu.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_timer.h>

నిర్మాణం timer_state {
	u64 సమయం_delta;
	spinlock_t event_list_తాళం;
	నిర్మాణం sbi_dlist event_list GUARDED_BY(&event_list_తాళం);
	నిర్మాణం sbi_timer_event smode_ev;
};

స్థిర గుర్తులేని దీర్ఘ timer_state_off;
స్థిర u64 (*get_సమయం_val)(శూన్య);
స్థిర స్థిరరాశి నిర్మాణం sbi_timer_device *timer_dev = ఖాళీ;

#ఒకవేళ __riscv_xlen == 32
స్థిర u64 get_ticks(శూన్య)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	తిరిగి ((u64)hi << 32) | lo;
}
#లేకపోతే
స్థిర u64 get_ticks(శూన్య)
{
	గుర్తులేని దీర్ఘ n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	తిరిగి n;
}
#అయితే_ముగింపు

స్థిర శూన్య nop_delay_fn(శూన్య *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	తిరిగి delta;
}

శూన్య sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  శూన్య (*delay_fn)(శూన్య *), శూన్య *opaque)
{
	u64 ప్రారంభం_val, delta;

	/* Do nothing if we don't have timer device */
	ఒకవేళ (!timer_dev || !get_సమయం_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		తిరిగి;
	}

	/* Save starting timer value */
	ప్రారంభం_val = get_సమయం_val();

	/* Use NOP delay function if delay function not available */
	ఒకవేళ (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	అయితే ((get_సమయం_val() - ప్రారంభం_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(శూన్య *), శూన్య *arg,
			    uint64_t timeout_ms)
{
	uint64_t ప్రారంభం_సమయం = sbi_timer_విలువ();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	అయితే(!predicate(arg))
		ఒకవేళ (sbi_timer_విలువ() - ప్రారంభం_సమయం  >= ticks)
			తిరిగి false;
	తిరిగి true;
}

గుర్తులేని దీర్ఘ sbi_timer_frequency(శూన్య)
{
	స్థిరరాశి నిర్మాణం sbi_timer_device *timer_dev = sbi_timer_get_device();

	ఒకవేళ (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		తిరిగి 0;
	}

	తిరిగి timer_dev->timer_freq;
}

u64 sbi_timer_విలువ(శూన్య)
{
	ఒకవేళ (get_సమయం_val)
		తిరిగి get_సమయం_val();
	తిరిగి 0;
}

u64 sbi_timer_virt_విలువ(శూన్య)
{
	నిర్మాణం timer_state *tstate = sbi_scratch_thishart_స్థానభ్రంశం_ptr(timer_state_off);

	తిరిగి sbi_timer_విలువ() + tstate->సమయం_delta;
}

u64 sbi_timer_get_delta(శూన్య)
{
	నిర్మాణం timer_state *tstate = sbi_scratch_thishart_స్థానభ్రంశం_ptr(timer_state_off);

	తిరిగి tstate->సమయం_delta;
}

శూన్య sbi_timer_సమితి_delta(ulong delta)
{
	నిర్మాణం timer_state *tstate = sbi_scratch_thishart_స్థానభ్రంశం_ptr(timer_state_off);

#ఒకవేళ __riscv_xlen == 32
	tstate->సమయం_delta &= ~0xffffffffUL;
	tstate->సమయం_delta |= (u32)delta;
#లేకపోతే
	tstate->సమయం_delta = delta;
#అయితే_ముగింపు
}

#ఒకవేళ __riscv_xlen == 32
శూన్య sbi_timer_సమితి_delta_పై(ulong delta_పై)
{
	నిర్మాణం timer_state *tstate = sbi_scratch_thishart_స్థానభ్రంశం_ptr(timer_state_off);

	tstate->సమయం_delta &= 0xffffffffUL;
	tstate->సమయం_delta |= (u64)delta_పై << 32;
}
#అయితే_ముగింపు

స్థిర శూన్య __sbi_timer_update_device(నిర్మాణం timer_state *tstate)
	MUST_HOLD(&tstate->event_list_తాళం)
{
	నిర్మాణం sbi_timer_event *ev;

	ఒకవేళ (!timer_dev)
		తిరిగి;

	ఒకవేళ (sbi_list_ఖాళీలు(&tstate->event_list)) {
		ఒకవేళ (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} లేకపోతే {
		ev = sbi_list_మొదటి_నమోదు(&tstate->event_list, నిర్మాణం sbi_timer_event, తల);
		ఒకవేళ (timer_dev->timer_event_ప్రారంభం)
			timer_dev->timer_event_ప్రారంభం(ev->సమయం_stamp);
		csr_సమితి(CSR_MIE, MIP_MTIP);
	}
}

స్థిర శూన్య __sbi_timer_event_stop(నిర్మాణం sbi_timer_event *ev)
{
	ఒకవేళ (ev->hart_సూచీ > -1) {
		sbi_list_del(&ev->తల);
		ev->hart_సూచీ = -1;
	}
}

స్థిర శూన్య __sbi_timer_event_start(నిర్మాణం timer_state *tstate,
				    నిర్మాణం sbi_timer_event *ev, u64 తదుపరి_event)
	MUST_HOLD(&tstate->event_list_తాళం)
{
	నిర్మాణం sbi_timer_event *tev, *తదుపరి_ev = ఖాళీ;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_నమోదు(tev, &tstate->event_list, తల) {
		ఒకవేళ (తదుపరి_event < tev->సమయం_stamp) {
			తదుపరి_ev = tev;
			ఆపు;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_సూచీ = current_hartindex();
	ev->సమయం_stamp = తదుపరి_event;
	ఒకవేళ (తదుపరి_ev)
		sbi_list_కూడు(&ev->తల, &తదుపరి_ev->తల);
	లేకపోతే
		sbi_list_కూడు_తోక(&ev->తల, &tstate->event_list);
}

శూన్య sbi_timer_event_ప్రారంభం(నిర్మాణం sbi_timer_event *ev, u64 తదుపరి_event)
{
	నిర్మాణం timer_state *tstate;

	ఒకవేళ (!ev)
		తిరిగి;

	/* Ensure that event is not on the per-HART event list */
	ఒకవేళ (ev->hart_సూచీ > -1) {
		tstate = sbi_scratch_స్థానభ్రంశం_ptr(sbi_hartindex_గమ్యం_scratch(ev->hart_సూచీ),
						timer_state_off);
		spin_తాళం(&tstate->event_list_తాళం);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_తాళం);
	}

	tstate = sbi_scratch_thishart_స్థానభ్రంశం_ptr(timer_state_off);
	spin_తాళం(&tstate->event_list_తాళం);

	__sbi_timer_event_start(tstate, ev, తదుపరి_event);
	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_తాళం);
}

శూన్య sbi_timer_event_stop(నిర్మాణం sbi_timer_event *ev)
{
	నిర్మాణం timer_state *tstate;
	పూర్ణాంకం ev_hart_సూచీ;

	ఒకవేళ (!ev)
		తిరిగి;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_సూచీ = ev->hart_సూచీ;
	ఒకవేళ (ev->hart_సూచీ > -1) {
		tstate = sbi_scratch_స్థానభ్రంశం_ptr(sbi_hartindex_గమ్యం_scratch(ev->hart_సూచీ),
						timer_state_off);
		spin_తాళం(&tstate->event_list_తాళం);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_తాళం);
	}

	/* Re-program timer device on the current HART */
	ఒకవేళ (ev_hart_సూచీ == current_hartindex()) {
		tstate = sbi_scratch_thishart_స్థానభ్రంశం_ptr(timer_state_off);
		spin_తాళం(&tstate->event_list_తాళం);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_తాళం);
	}
}

స్థిర శూన్య sbi_timer_smode_event_callback(నిర్మాణం sbi_timer_event *ev,
					   నిర్మాణం sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	ఒకవేళ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_సమితి(CSR_MIP, MIP_STIP);
}

స్థిర శూన్య sbi_timer_smode_event_cleanup(నిర్మాణం sbi_timer_event *ev)
{
	ఒకవేళ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

శూన్య sbi_timer_smode_event_ప్రారంభం(u64 తదుపరి_event)
{
	నిర్మాణం timer_state *tstate = sbi_scratch_స్థానభ్రంశం_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_సమితి_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	ఒకవేళ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, తదుపరి_event);
	} లేకపోతే {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_ప్రారంభం(&tstate->smode_ev, తదుపరి_event);
	}
}

శూన్య sbi_timer_process(శూన్య)
{
	నిర్మాణం timer_state *tstate = sbi_scratch_thishart_స్థానభ్రంశం_ptr(timer_state_off);
	నిర్మాణం sbi_timer_event_restart restart;
	SBI_LIST_తల(restart_list);
	నిర్మాణం sbi_timer_event *ev;

	spin_తాళం(&tstate->event_list_తాళం);

	అయితే (!sbi_list_ఖాళీలు(&tstate->event_list)) {
		ev = sbi_list_మొదటి_నమోదు(&tstate->event_list, నిర్మాణం sbi_timer_event, తల);
		ఒకవేళ (ev->సమయం_stamp > sbi_timer_విలువ())
			ఆపు;

		__sbi_timer_event_stop(ev);
		ఒకవేళ (ev->callback) {
			restart.required = false;
			restart.తదుపరి_event = 0;
			ev->callback(ev, &restart);
			ఒకవేళ (restart.required) {
				ev->సమయం_stamp = restart.తదుపరి_event;
				sbi_list_కూడు_తోక(&ev->తల, &restart_list);
			}
		}
	}

	అయితే (!sbi_list_ఖాళీలు(&restart_list)) {
		ev = sbi_list_మొదటి_నమోదు(&tstate->event_list, నిర్మాణం sbi_timer_event, తల);
		sbi_list_del(&ev->తల);
		__sbi_timer_event_start(tstate, ev, ev->సమయం_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_తాళం);
}

స్థిరరాశి నిర్మాణం sbi_timer_device *sbi_timer_get_device(శూన్య)
{
	తిరిగి timer_dev;
}

శూన్య sbi_timer_సమితి_device(స్థిరరాశి నిర్మాణం sbi_timer_device *dev)
{
	ఒకవేళ (!dev || timer_dev)
		తిరిగి;

	timer_dev = dev;
	ఒకవేళ (!get_సమయం_val && timer_dev->timer_విలువ)
		get_సమయం_val = timer_dev->timer_విలువ;
}

పూర్ణాంకం sbi_timer_init(నిర్మాణం sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_ptr(scratch);
	నిర్మాణం timer_state *tstate;
	పూర్ణాంకం ret;

	ఒకవేళ (cold_boot) {
		timer_state_off = sbi_scratch_alloc_స్థానభ్రంశం(పరిమాణం(*tstate));
		ఒకవేళ (!timer_state_off)
			తిరిగి SBI_ENOMEM;

		ఒకవేళ (sbi_hart_has_csr(scratch, SBI_HART_CSR_సమయం))
			get_సమయం_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		ఒకవేళ (ret)
			తిరిగి ret;
	} లేకపోతే {
		ఒకవేళ (!timer_state_off)
			తిరిగి SBI_ENOMEM;
	}

	tstate = sbi_scratch_స్థానభ్రంశం_ptr(scratch, timer_state_off);
	tstate->సమయం_delta = 0;
	SPIN_తాళం_INIT(tstate->event_list_తాళం);
	SBI_INIT_LIST_తల(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, ఖాళీ);

	ఒకవేళ (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	తిరిగి 0;
}

శూన్య sbi_timer_exit(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం timer_state *tstate = sbi_scratch_thishart_స్థానభ్రంశం_ptr(timer_state_off);
	నిర్మాణం sbi_timer_event *ev;

	spin_తాళం(&tstate->event_list_తాళం);

	అయితే (!sbi_list_ఖాళీలు(&tstate->event_list)) {
		ev = sbi_list_మొదటి_నమోదు(&tstate->event_list, నిర్మాణం sbi_timer_event, తల);
		__sbi_timer_event_stop(ev);
		ఒకవేళ (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_తాళం);
}
