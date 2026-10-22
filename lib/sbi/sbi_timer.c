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
	spinlock_t event_list_तालकम्;
	संरचना sbi_dlist event_list GUARDED_BY(&event_list_तालकम्);
	संरचना sbi_timer_event smode_ev;
};

स्थिर अचिह्नित दीर्घ timer_state_off;
स्थिर u64 (*get_time_val)(शून्य);
स्थिर ध्रुव संरचना sbi_timer_device *timer_dev = रिक्त;

#यदि __riscv_xlen == 32
स्थिर u64 get_ticks(शून्य)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	प्रत्यागम ((u64)hi << 32) | lo;
}
#अन्यथा
स्थिर u64 get_ticks(शून्य)
{
	अचिह्नित दीर्घ n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	प्रत्यागम n;
}
#यदि_अन्तः

स्थिर शून्य nop_delay_fn(शून्य *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	प्रत्यागम delta;
}

शून्य sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  शून्य (*delay_fn)(शून्य *), शून्य *opaque)
{
	u64 start_val, delta;

	/* Do nothing if we don't have timer device */
	यदि (!timer_dev || !get_time_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		प्रत्यागम;
	}

	/* Save starting timer value */
	start_val = get_time_val();

	/* Use NOP delay function if delay function not available */
	यदि (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	यावत् ((get_time_val() - start_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(शून्य *), शून्य *arg,
			    uint64_t timeout_ms)
{
	uint64_t start_time = sbi_timer_मानम्();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	यावत्(!predicate(arg))
		यदि (sbi_timer_मानम्() - start_time  >= ticks)
			प्रत्यागम false;
	प्रत्यागम true;
}

अचिह्नित दीर्घ sbi_timer_frequency(शून्य)
{
	ध्रुव संरचना sbi_timer_device *timer_dev = sbi_timer_get_device();

	यदि (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		प्रत्यागम 0;
	}

	प्रत्यागम timer_dev->timer_freq;
}

u64 sbi_timer_मानम्(शून्य)
{
	यदि (get_time_val)
		प्रत्यागम get_time_val();
	प्रत्यागम 0;
}

u64 sbi_timer_virt_मानम्(शून्य)
{
	संरचना timer_state *tstate = sbi_scratch_thishart_विस्थापनम्_ptr(timer_state_off);

	प्रत्यागम sbi_timer_मानम्() + tstate->time_delta;
}

u64 sbi_timer_get_delta(शून्य)
{
	संरचना timer_state *tstate = sbi_scratch_thishart_विस्थापनम्_ptr(timer_state_off);

	प्रत्यागम tstate->time_delta;
}

शून्य sbi_timer_समुच्चयः_delta(ulong delta)
{
	संरचना timer_state *tstate = sbi_scratch_thishart_विस्थापनम्_ptr(timer_state_off);

#यदि __riscv_xlen == 32
	tstate->time_delta &= ~0xffffffffUL;
	tstate->time_delta |= (u32)delta;
#अन्यथा
	tstate->time_delta = delta;
#यदि_अन्तः
}

#यदि __riscv_xlen == 32
शून्य sbi_timer_समुच्चयः_delta_ऊर्ध्वस्थम्(ulong delta_ऊर्ध्वस्थम्)
{
	संरचना timer_state *tstate = sbi_scratch_thishart_विस्थापनम्_ptr(timer_state_off);

	tstate->time_delta &= 0xffffffffUL;
	tstate->time_delta |= (u64)delta_ऊर्ध्वस्थम् << 32;
}
#यदि_अन्तः

स्थिर शून्य __sbi_timer_update_device(संरचना timer_state *tstate)
	MUST_HOLD(&tstate->event_list_तालकम्)
{
	संरचना sbi_timer_event *ev;

	यदि (!timer_dev)
		प्रत्यागम;

	यदि (sbi_list_रिक्तानि(&tstate->event_list)) {
		यदि (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} अन्यथा {
		ev = sbi_list_first_प्रविष्टिः(&tstate->event_list, संरचना sbi_timer_event, शीर्ष);
		यदि (timer_dev->timer_event_start)
			timer_dev->timer_event_start(ev->time_stamp);
		csr_समुच्चयः(CSR_MIE, MIP_MTIP);
	}
}

स्थिर शून्य __sbi_timer_event_stop(संरचना sbi_timer_event *ev)
{
	यदि (ev->hart_सूचकांक > -1) {
		sbi_list_del(&ev->शीर्ष);
		ev->hart_सूचकांक = -1;
	}
}

स्थिर शून्य __sbi_timer_event_start(संरचना timer_state *tstate,
				    संरचना sbi_timer_event *ev, u64 अग्र_event)
	MUST_HOLD(&tstate->event_list_तालकम्)
{
	संरचना sbi_timer_event *tev, *अग्र_ev = रिक्त;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_प्रविष्टिः(tev, &tstate->event_list, शीर्ष) {
		यदि (अग्र_event < tev->time_stamp) {
			अग्र_ev = tev;
			भङ्ग;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_सूचकांक = current_hartindex();
	ev->time_stamp = अग्र_event;
	यदि (अग्र_ev)
		sbi_list_सङ्कलय(&ev->शीर्ष, &अग्र_ev->शीर्ष);
	अन्यथा
		sbi_list_सङ्कलय_पुच्छ(&ev->शीर्ष, &tstate->event_list);
}

शून्य sbi_timer_event_start(संरचना sbi_timer_event *ev, u64 अग्र_event)
{
	संरचना timer_state *tstate;

	यदि (!ev)
		प्रत्यागम;

	/* Ensure that event is not on the per-HART event list */
	यदि (ev->hart_सूचकांक > -1) {
		tstate = sbi_scratch_विस्थापनम्_ptr(sbi_hartindex_गन्तव्यम्_scratch(ev->hart_सूचकांक),
						timer_state_off);
		spin_तालकम्(&tstate->event_list_तालकम्);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_तालकम्);
	}

	tstate = sbi_scratch_thishart_विस्थापनम्_ptr(timer_state_off);
	spin_तालकम्(&tstate->event_list_तालकम्);

	__sbi_timer_event_start(tstate, ev, अग्र_event);
	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_तालकम्);
}

शून्य sbi_timer_event_stop(संरचना sbi_timer_event *ev)
{
	संरचना timer_state *tstate;
	पूर्णाङ्क ev_hart_सूचकांक;

	यदि (!ev)
		प्रत्यागम;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_सूचकांक = ev->hart_सूचकांक;
	यदि (ev->hart_सूचकांक > -1) {
		tstate = sbi_scratch_विस्थापनम्_ptr(sbi_hartindex_गन्तव्यम्_scratch(ev->hart_सूचकांक),
						timer_state_off);
		spin_तालकम्(&tstate->event_list_तालकम्);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_तालकम्);
	}

	/* Re-program timer device on the current HART */
	यदि (ev_hart_सूचकांक == current_hartindex()) {
		tstate = sbi_scratch_thishart_विस्थापनम्_ptr(timer_state_off);
		spin_तालकम्(&tstate->event_list_तालकम्);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_तालकम्);
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
	यदि (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_समुच्चयः(CSR_MIP, MIP_STIP);
}

स्थिर शून्य sbi_timer_smode_event_cleanup(संरचना sbi_timer_event *ev)
{
	यदि (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

शून्य sbi_timer_smode_event_start(u64 अग्र_event)
{
	संरचना timer_state *tstate = sbi_scratch_विस्थापनम्_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_समुच्चयः_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	यदि (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, अग्र_event);
	} अन्यथा {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_start(&tstate->smode_ev, अग्र_event);
	}
}

शून्य sbi_timer_process(शून्य)
{
	संरचना timer_state *tstate = sbi_scratch_thishart_विस्थापनम्_ptr(timer_state_off);
	संरचना sbi_timer_event_restart restart;
	SBI_LIST_शीर्ष(restart_list);
	संरचना sbi_timer_event *ev;

	spin_तालकम्(&tstate->event_list_तालकम्);

	यावत् (!sbi_list_रिक्तानि(&tstate->event_list)) {
		ev = sbi_list_first_प्रविष्टिः(&tstate->event_list, संरचना sbi_timer_event, शीर्ष);
		यदि (ev->time_stamp > sbi_timer_मानम्())
			भङ्ग;

		__sbi_timer_event_stop(ev);
		यदि (ev->callback) {
			restart.required = false;
			restart.अग्र_event = 0;
			ev->callback(ev, &restart);
			यदि (restart.required) {
				ev->time_stamp = restart.अग्र_event;
				sbi_list_सङ्कलय_पुच्छ(&ev->शीर्ष, &restart_list);
			}
		}
	}

	यावत् (!sbi_list_रिक्तानि(&restart_list)) {
		ev = sbi_list_first_प्रविष्टिः(&tstate->event_list, संरचना sbi_timer_event, शीर्ष);
		sbi_list_del(&ev->शीर्ष);
		__sbi_timer_event_start(tstate, ev, ev->time_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_तालकम्);
}

ध्रुव संरचना sbi_timer_device *sbi_timer_get_device(शून्य)
{
	प्रत्यागम timer_dev;
}

शून्य sbi_timer_समुच्चयः_device(ध्रुव संरचना sbi_timer_device *dev)
{
	यदि (!dev || timer_dev)
		प्रत्यागम;

	timer_dev = dev;
	यदि (!get_time_val && timer_dev->timer_मानम्)
		get_time_val = timer_dev->timer_मानम्;
}

पूर्णाङ्क sbi_timer_init(संरचना sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	ध्रुव संरचना sbi_platform *plat = sbi_platform_ptr(scratch);
	संरचना timer_state *tstate;
	पूर्णाङ्क ret;

	यदि (cold_boot) {
		timer_state_off = sbi_scratch_alloc_विस्थापनम्(परिमाण(*tstate));
		यदि (!timer_state_off)
			प्रत्यागम SBI_ENOMEM;

		यदि (sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME))
			get_time_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		यदि (ret)
			प्रत्यागम ret;
	} अन्यथा {
		यदि (!timer_state_off)
			प्रत्यागम SBI_ENOMEM;
	}

	tstate = sbi_scratch_विस्थापनम्_ptr(scratch, timer_state_off);
	tstate->time_delta = 0;
	SPIN_तालकम्_INIT(tstate->event_list_तालकम्);
	SBI_INIT_LIST_शीर्ष(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, रिक्त);

	यदि (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		यदि (ret)
			प्रत्यागम ret;
	}

	प्रत्यागम 0;
}

शून्य sbi_timer_exit(संरचना sbi_scratch *scratch)
{
	संरचना timer_state *tstate = sbi_scratch_thishart_विस्थापनम्_ptr(timer_state_off);
	संरचना sbi_timer_event *ev;

	spin_तालकम्(&tstate->event_list_तालकम्);

	यावत् (!sbi_list_रिक्तानि(&tstate->event_list)) {
		ev = sbi_list_first_प्रविष्टिः(&tstate->event_list, संरचना sbi_timer_event, शीर्ष);
		__sbi_timer_event_stop(ev);
		यदि (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_तालकम्);
}
