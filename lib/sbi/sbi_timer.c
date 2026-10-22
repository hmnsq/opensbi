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

ರಚನೆ timer_state {
	u64 ಸಮಯ_delta;
	spinlock_t event_list_ಬೀಗ;
	ರಚನೆ sbi_dlist event_list GUARDED_BY(&event_list_ಬೀಗ);
	ರಚನೆ sbi_timer_event smode_ev;
};

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ timer_state_off;
ಸ್ಥಿರ u64 (*get_ಸಮಯ_val)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_timer_device *timer_dev = ಖಾಲಿ;

#ಆದರೆ __riscv_xlen == 32
ಸ್ಥಿರ u64 get_ticks(ಶೂನ್ಯ)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	ಹಿಂತಿರುಗಿ ((u64)hi << 32) | lo;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ u64 get_ticks(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	ಹಿಂತಿರುಗಿ n;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ nop_delay_fn(ಶೂನ್ಯ *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	ಹಿಂತಿರುಗಿ delta;
}

ಶೂನ್ಯ sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  ಶೂನ್ಯ (*delay_fn)(ಶೂನ್ಯ *), ಶೂನ್ಯ *opaque)
{
	u64 ಆರಂಭ_val, delta;

	/* Do nothing if we don't have timer device */
	ಆದರೆ (!timer_dev || !get_ಸಮಯ_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		ಹಿಂತಿರುಗಿ;
	}

	/* Save starting timer value */
	ಆರಂಭ_val = get_ಸಮಯ_val();

	/* Use NOP delay function if delay function not available */
	ಆದರೆ (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	ಆಗ ((get_ಸಮಯ_val() - ಆರಂಭ_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(ಶೂನ್ಯ *), ಶೂನ್ಯ *arg,
			    uint64_t timeout_ms)
{
	uint64_t ಆರಂಭ_ಸಮಯ = sbi_timer_ಮೌಲ್ಯ();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	ಆಗ(!predicate(arg))
		ಆದರೆ (sbi_timer_ಮೌಲ್ಯ() - ಆರಂಭ_ಸಮಯ  >= ticks)
			ಹಿಂತಿರುಗಿ false;
	ಹಿಂತಿರುಗಿ true;
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_timer_frequency(ಶೂನ್ಯ)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_timer_device *timer_dev = sbi_timer_get_device();

	ಆದರೆ (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		ಹಿಂತಿರುಗಿ 0;
	}

	ಹಿಂತಿರುಗಿ timer_dev->timer_freq;
}

u64 sbi_timer_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
	ಆದರೆ (get_ಸಮಯ_val)
		ಹಿಂತಿರುಗಿ get_ಸಮಯ_val();
	ಹಿಂತಿರುಗಿ 0;
}

u64 sbi_timer_virt_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
	ರಚನೆ timer_state *tstate = sbi_scratch_thishart_ಸ್ಥಾನಾಂತರ_ptr(timer_state_off);

	ಹಿಂತಿರುಗಿ sbi_timer_ಮೌಲ್ಯ() + tstate->ಸಮಯ_delta;
}

u64 sbi_timer_get_delta(ಶೂನ್ಯ)
{
	ರಚನೆ timer_state *tstate = sbi_scratch_thishart_ಸ್ಥಾನಾಂತರ_ptr(timer_state_off);

	ಹಿಂತಿರುಗಿ tstate->ಸಮಯ_delta;
}

ಶೂನ್ಯ sbi_timer_ಗಣ_delta(ulong delta)
{
	ರಚನೆ timer_state *tstate = sbi_scratch_thishart_ಸ್ಥಾನಾಂತರ_ptr(timer_state_off);

#ಆದರೆ __riscv_xlen == 32
	tstate->ಸಮಯ_delta &= ~0xffffffffUL;
	tstate->ಸಮಯ_delta |= (u32)delta;
#ಇಲ್ಲದಿದ್ದರೆ
	tstate->ಸಮಯ_delta = delta;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ __riscv_xlen == 32
ಶೂನ್ಯ sbi_timer_ಗಣ_delta_ಊರ್ಧ್ವ(ulong delta_ಊರ್ಧ್ವ)
{
	ರಚನೆ timer_state *tstate = sbi_scratch_thishart_ಸ್ಥಾನಾಂತರ_ptr(timer_state_off);

	tstate->ಸಮಯ_delta &= 0xffffffffUL;
	tstate->ಸಮಯ_delta |= (u64)delta_ಊರ್ಧ್ವ << 32;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ __sbi_timer_update_device(ರಚನೆ timer_state *tstate)
	MUST_HOLD(&tstate->event_list_ಬೀಗ)
{
	ರಚನೆ sbi_timer_event *ev;

	ಆದರೆ (!timer_dev)
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (sbi_list_ಖಾಲಿಗಳು(&tstate->event_list)) {
		ಆದರೆ (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ev = sbi_list_ಮೊದಲ_ನಮೂದು(&tstate->event_list, ರಚನೆ sbi_timer_event, ತಲೆ);
		ಆದರೆ (timer_dev->timer_event_ಆರಂಭ)
			timer_dev->timer_event_ಆರಂಭ(ev->ಸಮಯ_stamp);
		csr_ಗಣ(CSR_MIE, MIP_MTIP);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ __sbi_timer_event_stop(ರಚನೆ sbi_timer_event *ev)
{
	ಆದರೆ (ev->hart_ಸೂಚ್ಯಂಕ > -1) {
		sbi_list_del(&ev->ತಲೆ);
		ev->hart_ಸೂಚ್ಯಂಕ = -1;
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ __sbi_timer_event_start(ರಚನೆ timer_state *tstate,
				    ರಚನೆ sbi_timer_event *ev, u64 ಮುಂದಿನ_event)
	MUST_HOLD(&tstate->event_list_ಬೀಗ)
{
	ರಚನೆ sbi_timer_event *tev, *ಮುಂದಿನ_ev = ಖಾಲಿ;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_ನಮೂದು(tev, &tstate->event_list, ತಲೆ) {
		ಆದರೆ (ಮುಂದಿನ_event < tev->ಸಮಯ_stamp) {
			ಮುಂದಿನ_ev = tev;
			ನಿಲ್ಲಿಸು;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_ಸೂಚ್ಯಂಕ = current_hartindex();
	ev->ಸಮಯ_stamp = ಮುಂದಿನ_event;
	ಆದರೆ (ಮುಂದಿನ_ev)
		sbi_list_ಕೂಡಿಸು(&ev->ತಲೆ, &ಮುಂದಿನ_ev->ತಲೆ);
	ಇಲ್ಲದಿದ್ದರೆ
		sbi_list_ಕೂಡಿಸು_ಬಾಲ(&ev->ತಲೆ, &tstate->event_list);
}

ಶೂನ್ಯ sbi_timer_event_ಆರಂಭ(ರಚನೆ sbi_timer_event *ev, u64 ಮುಂದಿನ_event)
{
	ರಚನೆ timer_state *tstate;

	ಆದರೆ (!ev)
		ಹಿಂತಿರುಗಿ;

	/* Ensure that event is not on the per-HART event list */
	ಆದರೆ (ev->hart_ಸೂಚ್ಯಂಕ > -1) {
		tstate = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(sbi_hartindex_ಗಮ್ಯ_scratch(ev->hart_ಸೂಚ್ಯಂಕ),
						timer_state_off);
		spin_ಬೀಗ(&tstate->event_list_ಬೀಗ);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_ಬೀಗ);
	}

	tstate = sbi_scratch_thishart_ಸ್ಥಾನಾಂತರ_ptr(timer_state_off);
	spin_ಬೀಗ(&tstate->event_list_ಬೀಗ);

	__sbi_timer_event_start(tstate, ev, ಮುಂದಿನ_event);
	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_ಬೀಗ);
}

ಶೂನ್ಯ sbi_timer_event_stop(ರಚನೆ sbi_timer_event *ev)
{
	ರಚನೆ timer_state *tstate;
	ಪೂರ್ಣಾಂಕ ev_hart_ಸೂಚ್ಯಂಕ;

	ಆದರೆ (!ev)
		ಹಿಂತಿರುಗಿ;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_ಸೂಚ್ಯಂಕ = ev->hart_ಸೂಚ್ಯಂಕ;
	ಆದರೆ (ev->hart_ಸೂಚ್ಯಂಕ > -1) {
		tstate = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(sbi_hartindex_ಗಮ್ಯ_scratch(ev->hart_ಸೂಚ್ಯಂಕ),
						timer_state_off);
		spin_ಬೀಗ(&tstate->event_list_ಬೀಗ);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_ಬೀಗ);
	}

	/* Re-program timer device on the current HART */
	ಆದರೆ (ev_hart_ಸೂಚ್ಯಂಕ == current_hartindex()) {
		tstate = sbi_scratch_thishart_ಸ್ಥಾನಾಂತರ_ptr(timer_state_off);
		spin_ಬೀಗ(&tstate->event_list_ಬೀಗ);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_ಬೀಗ);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_timer_smode_event_callback(ರಚನೆ sbi_timer_event *ev,
					   ರಚನೆ sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	ಆದರೆ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_ಗಣ(CSR_MIP, MIP_STIP);
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_timer_smode_event_cleanup(ರಚನೆ sbi_timer_event *ev)
{
	ಆದರೆ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

ಶೂನ್ಯ sbi_timer_smode_event_ಆರಂಭ(u64 ಮುಂದಿನ_event)
{
	ರಚನೆ timer_state *tstate = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_ಗಣ_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	ಆದರೆ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, ಮುಂದಿನ_event);
	} ಇಲ್ಲದಿದ್ದರೆ {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_ಆರಂಭ(&tstate->smode_ev, ಮುಂದಿನ_event);
	}
}

ಶೂನ್ಯ sbi_timer_process(ಶೂನ್ಯ)
{
	ರಚನೆ timer_state *tstate = sbi_scratch_thishart_ಸ್ಥಾನಾಂತರ_ptr(timer_state_off);
	ರಚನೆ sbi_timer_event_restart restart;
	SBI_LIST_ತಲೆ(restart_list);
	ರಚನೆ sbi_timer_event *ev;

	spin_ಬೀಗ(&tstate->event_list_ಬೀಗ);

	ಆಗ (!sbi_list_ಖಾಲಿಗಳು(&tstate->event_list)) {
		ev = sbi_list_ಮೊದಲ_ನಮೂದು(&tstate->event_list, ರಚನೆ sbi_timer_event, ತಲೆ);
		ಆದರೆ (ev->ಸಮಯ_stamp > sbi_timer_ಮೌಲ್ಯ())
			ನಿಲ್ಲಿಸು;

		__sbi_timer_event_stop(ev);
		ಆದರೆ (ev->callback) {
			restart.required = false;
			restart.ಮುಂದಿನ_event = 0;
			ev->callback(ev, &restart);
			ಆದರೆ (restart.required) {
				ev->ಸಮಯ_stamp = restart.ಮುಂದಿನ_event;
				sbi_list_ಕೂಡಿಸು_ಬಾಲ(&ev->ತಲೆ, &restart_list);
			}
		}
	}

	ಆಗ (!sbi_list_ಖಾಲಿಗಳು(&restart_list)) {
		ev = sbi_list_ಮೊದಲ_ನಮೂದು(&tstate->event_list, ರಚನೆ sbi_timer_event, ತಲೆ);
		sbi_list_del(&ev->ತಲೆ);
		__sbi_timer_event_start(tstate, ev, ev->ಸಮಯ_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_ಬೀಗ);
}

ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_timer_device *sbi_timer_get_device(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ timer_dev;
}

ಶೂನ್ಯ sbi_timer_ಗಣ_device(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_timer_device *dev)
{
	ಆದರೆ (!dev || timer_dev)
		ಹಿಂತಿರುಗಿ;

	timer_dev = dev;
	ಆದರೆ (!get_ಸಮಯ_val && timer_dev->timer_ಮೌಲ್ಯ)
		get_ಸಮಯ_val = timer_dev->timer_ಮೌಲ್ಯ;
}

ಪೂರ್ಣಾಂಕ sbi_timer_init(ರಚನೆ sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_ptr(scratch);
	ರಚನೆ timer_state *tstate;
	ಪೂರ್ಣಾಂಕ ret;

	ಆದರೆ (cold_boot) {
		timer_state_off = sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(ಗಾತ್ರ(*tstate));
		ಆದರೆ (!timer_state_off)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;

		ಆದರೆ (sbi_hart_has_csr(scratch, SBI_HART_CSR_ಸಮಯ))
			get_ಸಮಯ_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (!timer_state_off)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	tstate = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, timer_state_off);
	tstate->ಸಮಯ_delta = 0;
	SPIN_ಬೀಗ_INIT(tstate->event_list_ಬೀಗ);
	SBI_INIT_LIST_ತಲೆ(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, ಖಾಲಿ);

	ಆದರೆ (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ sbi_timer_exit(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ timer_state *tstate = sbi_scratch_thishart_ಸ್ಥಾನಾಂತರ_ptr(timer_state_off);
	ರಚನೆ sbi_timer_event *ev;

	spin_ಬೀಗ(&tstate->event_list_ಬೀಗ);

	ಆಗ (!sbi_list_ಖಾಲಿಗಳು(&tstate->event_list)) {
		ev = sbi_list_ಮೊದಲ_ನಮೂದು(&tstate->event_list, ರಚನೆ sbi_timer_event, ತಲೆ);
		__sbi_timer_event_stop(ev);
		ಆದರೆ (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_ಬೀಗ);
}
