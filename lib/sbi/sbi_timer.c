/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_barrier.h>
#समावेश <sbi/riscv_encoding.h>
#समावेश <sbi/riscv_locks.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_hartmask.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_pmu.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_timer.h>

काष्ठा timer_state {
	u64 time_delta;
	spinlock_t event_list_lock;
	काष्ठा sbi_dlist event_list GUARDED_BY(&event_list_lock);
	काष्ठा sbi_timer_event smode_ev;
};

अटल अचिन्हित दीर्घ timer_state_off;
अटल u64 (*get_time_val)(व्योम);
अटल स्थिर काष्ठा sbi_timer_device *timer_dev = शून्य;

#अगर __riscv_xlen == 32
अटल u64 get_ticks(व्योम)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	वापस ((u64)hi << 32) | lo;
}
#अन्यथा
अटल u64 get_ticks(व्योम)
{
	अचिन्हित दीर्घ n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	वापस n;
}
#पूर्ण_अगर

अटल व्योम nop_delay_fn(व्योम *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	वापस delta;
}

व्योम sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  व्योम (*delay_fn)(व्योम *), व्योम *opaque)
{
	u64 start_val, delta;

	/* Do nothing if we don't have timer device */
	अगर (!timer_dev || !get_time_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		वापस;
	}

	/* Save starting timer value */
	start_val = get_time_val();

	/* Use NOP delay function if delay function not available */
	अगर (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	जबतक ((get_time_val() - start_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(व्योम *), व्योम *arg,
			    uint64_t timeout_ms)
{
	uint64_t start_time = sbi_timer_मूल्य();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	जबतक(!predicate(arg))
		अगर (sbi_timer_मूल्य() - start_time  >= ticks)
			वापस false;
	वापस true;
}

अचिन्हित दीर्घ sbi_timer_frequency(व्योम)
{
	स्थिर काष्ठा sbi_timer_device *timer_dev = sbi_timer_get_device();

	अगर (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		वापस 0;
	}

	वापस timer_dev->timer_freq;
}

u64 sbi_timer_मूल्य(व्योम)
{
	अगर (get_time_val)
		वापस get_time_val();
	वापस 0;
}

u64 sbi_timer_virt_मूल्य(व्योम)
{
	काष्ठा timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	वापस sbi_timer_मूल्य() + tstate->time_delta;
}

u64 sbi_timer_get_delta(व्योम)
{
	काष्ठा timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	वापस tstate->time_delta;
}

व्योम sbi_timer_set_delta(ulong delta)
{
	काष्ठा timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

#अगर __riscv_xlen == 32
	tstate->time_delta &= ~0xffffffffUL;
	tstate->time_delta |= (u32)delta;
#अन्यथा
	tstate->time_delta = delta;
#पूर्ण_अगर
}

#अगर __riscv_xlen == 32
व्योम sbi_timer_set_delta_upper(ulong delta_upper)
{
	काष्ठा timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	tstate->time_delta &= 0xffffffffUL;
	tstate->time_delta |= (u64)delta_upper << 32;
}
#पूर्ण_अगर

अटल व्योम __sbi_timer_update_device(काष्ठा timer_state *tstate)
	MUST_HOLD(&tstate->event_list_lock)
{
	काष्ठा sbi_timer_event *ev;

	अगर (!timer_dev)
		वापस;

	अगर (sbi_list_empty(&tstate->event_list)) {
		अगर (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} अन्यथा {
		ev = sbi_list_first_entry(&tstate->event_list, काष्ठा sbi_timer_event, शीर्ष);
		अगर (timer_dev->timer_event_start)
			timer_dev->timer_event_start(ev->time_stamp);
		csr_set(CSR_MIE, MIP_MTIP);
	}
}

अटल व्योम __sbi_timer_event_stop(काष्ठा sbi_timer_event *ev)
{
	अगर (ev->hart_सूचकांक > -1) {
		sbi_list_del(&ev->शीर्ष);
		ev->hart_सूचकांक = -1;
	}
}

अटल व्योम __sbi_timer_event_start(काष्ठा timer_state *tstate,
				    काष्ठा sbi_timer_event *ev, u64 अग्र_event)
	MUST_HOLD(&tstate->event_list_lock)
{
	काष्ठा sbi_timer_event *tev, *अग्र_ev = शून्य;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_entry(tev, &tstate->event_list, शीर्ष) {
		अगर (अग्र_event < tev->time_stamp) {
			अग्र_ev = tev;
			अवरोध;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_सूचकांक = current_hartindex();
	ev->time_stamp = अग्र_event;
	अगर (अग्र_ev)
		sbi_list_add(&ev->शीर्ष, &अग्र_ev->शीर्ष);
	अन्यथा
		sbi_list_add_पुच्छ(&ev->शीर्ष, &tstate->event_list);
}

व्योम sbi_timer_event_start(काष्ठा sbi_timer_event *ev, u64 अग्र_event)
{
	काष्ठा timer_state *tstate;

	अगर (!ev)
		वापस;

	/* Ensure that event is not on the per-HART event list */
	अगर (ev->hart_सूचकांक > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_सूचकांक),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	spin_lock(&tstate->event_list_lock);

	__sbi_timer_event_start(tstate, ev, अग्र_event);
	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

व्योम sbi_timer_event_stop(काष्ठा sbi_timer_event *ev)
{
	काष्ठा timer_state *tstate;
	पूर्णांक ev_hart_सूचकांक;

	अगर (!ev)
		वापस;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_सूचकांक = ev->hart_सूचकांक;
	अगर (ev->hart_सूचकांक > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_सूचकांक),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	/* Re-program timer device on the current HART */
	अगर (ev_hart_सूचकांक == current_hartindex()) {
		tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_lock);
	}
}

अटल व्योम sbi_timer_smode_event_callback(काष्ठा sbi_timer_event *ev,
					   काष्ठा sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	अगर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_set(CSR_MIP, MIP_STIP);
}

अटल व्योम sbi_timer_smode_event_cleanup(काष्ठा sbi_timer_event *ev)
{
	अगर (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

व्योम sbi_timer_smode_event_start(u64 अग्र_event)
{
	काष्ठा timer_state *tstate = sbi_scratch_offset_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SET_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	अगर (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, अग्र_event);
	} अन्यथा {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_start(&tstate->smode_ev, अग्र_event);
	}
}

व्योम sbi_timer_process(व्योम)
{
	काष्ठा timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	काष्ठा sbi_timer_event_restart restart;
	SBI_LIST_शीर्ष(restart_list);
	काष्ठा sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	जबतक (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, काष्ठा sbi_timer_event, शीर्ष);
		अगर (ev->time_stamp > sbi_timer_मूल्य())
			अवरोध;

		__sbi_timer_event_stop(ev);
		अगर (ev->callback) {
			restart.required = false;
			restart.अग्र_event = 0;
			ev->callback(ev, &restart);
			अगर (restart.required) {
				ev->time_stamp = restart.अग्र_event;
				sbi_list_add_पुच्छ(&ev->शीर्ष, &restart_list);
			}
		}
	}

	जबतक (!sbi_list_empty(&restart_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, काष्ठा sbi_timer_event, शीर्ष);
		sbi_list_del(&ev->शीर्ष);
		__sbi_timer_event_start(tstate, ev, ev->time_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

स्थिर काष्ठा sbi_timer_device *sbi_timer_get_device(व्योम)
{
	वापस timer_dev;
}

व्योम sbi_timer_set_device(स्थिर काष्ठा sbi_timer_device *dev)
{
	अगर (!dev || timer_dev)
		वापस;

	timer_dev = dev;
	अगर (!get_time_val && timer_dev->timer_मूल्य)
		get_time_val = timer_dev->timer_मूल्य;
}

पूर्णांक sbi_timer_init(काष्ठा sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	स्थिर काष्ठा sbi_platform *plat = sbi_platform_ptr(scratch);
	काष्ठा timer_state *tstate;
	पूर्णांक ret;

	अगर (cold_boot) {
		timer_state_off = sbi_scratch_alloc_offset(माप(*tstate));
		अगर (!timer_state_off)
			वापस SBI_ENOMEM;

		अगर (sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME))
			get_time_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		अगर (ret)
			वापस ret;
	} अन्यथा {
		अगर (!timer_state_off)
			वापस SBI_ENOMEM;
	}

	tstate = sbi_scratch_offset_ptr(scratch, timer_state_off);
	tstate->time_delta = 0;
	SPIN_LOCK_INIT(tstate->event_list_lock);
	SBI_INIT_LIST_शीर्ष(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, शून्य);

	अगर (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		अगर (ret)
			वापस ret;
	}

	वापस 0;
}

व्योम sbi_timer_exit(काष्ठा sbi_scratch *scratch)
{
	काष्ठा timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	काष्ठा sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	जबतक (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, काष्ठा sbi_timer_event, शीर्ष);
		__sbi_timer_event_stop(ev);
		अगर (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}
