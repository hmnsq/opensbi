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
	u64 وخت_delta;
	spinlock_t event_list_قلف;
	بنية sbi_dlist event_list GUARDED_BY(&event_list_قلف);
	بنية sbi_timer_event smode_ev;
};

ساكن بینښه اوږد timer_state_off;
ساكن u64 (*get_وخت_val)(فراغ);
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
	أرجع ((u64)hi << 32) | lo;
}
#وإلا
ساكن u64 get_ticks(فراغ)
{
	بینښه اوږد n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	أرجع n;
}
#د_که_پای

ساكن فراغ nop_delay_fn(فراغ *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	أرجع delta;
}

فراغ sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  فراغ (*delay_fn)(فراغ *), فراغ *opaque)
{
	u64 پیل_val, delta;

	/* Do nothing if we don't have timer device */
	إذا (!timer_dev || !get_وخت_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		أرجع;
	}

	/* Save starting timer value */
	پیل_val = get_وخت_val();

	/* Use NOP delay function if delay function not available */
	إذا (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	طالما ((get_وخت_val() - پیل_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(فراغ *), فراغ *arg,
			    uint64_t timeout_ms)
{
	uint64_t پیل_وخت = sbi_timer_ارزښت();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	طالما(!predicate(arg))
		إذا (sbi_timer_ارزښت() - پیل_وخت  >= ticks)
			أرجع false;
	أرجع true;
}

بینښه اوږد sbi_timer_frequency(فراغ)
{
	ثابت بنية sbi_timer_device *timer_dev = sbi_timer_get_device();

	إذا (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		أرجع 0;
	}

	أرجع timer_dev->timer_freq;
}

u64 sbi_timer_ارزښت(فراغ)
{
	إذا (get_وخت_val)
		أرجع get_وخت_val();
	أرجع 0;
}

u64 sbi_timer_virt_ارزښت(فراغ)
{
	بنية timer_state *tstate = sbi_scratch_thishart_دننه_واټن_ptr(timer_state_off);

	أرجع sbi_timer_ارزښت() + tstate->وخت_delta;
}

u64 sbi_timer_get_delta(فراغ)
{
	بنية timer_state *tstate = sbi_scratch_thishart_دننه_واټن_ptr(timer_state_off);

	أرجع tstate->وخت_delta;
}

فراغ sbi_timer_سېټ_delta(ulong delta)
{
	بنية timer_state *tstate = sbi_scratch_thishart_دننه_واټن_ptr(timer_state_off);

#إذا __riscv_xlen == 32
	tstate->وخت_delta &= ~0xffffffffUL;
	tstate->وخت_delta |= (u32)delta;
#وإلا
	tstate->وخت_delta = delta;
#د_که_پای
}

#إذا __riscv_xlen == 32
فراغ sbi_timer_سېټ_delta_پاسنی(ulong delta_پاسنی)
{
	بنية timer_state *tstate = sbi_scratch_thishart_دننه_واټن_ptr(timer_state_off);

	tstate->وخت_delta &= 0xffffffffUL;
	tstate->وخت_delta |= (u64)delta_پاسنی << 32;
}
#د_که_پای

ساكن فراغ __sbi_timer_update_device(بنية timer_state *tstate)
	MUST_HOLD(&tstate->event_list_قلف)
{
	بنية sbi_timer_event *ev;

	إذا (!timer_dev)
		أرجع;

	إذا (sbi_list_تشه(&tstate->event_list)) {
		إذا (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} وإلا {
		ev = sbi_list_لومړی_ثبت(&tstate->event_list, بنية sbi_timer_event, سر);
		إذا (timer_dev->timer_event_پیل)
			timer_dev->timer_event_پیل(ev->وخت_stamp);
		csr_سېټ(CSR_MIE, MIP_MTIP);
	}
}

ساكن فراغ __sbi_timer_event_stop(بنية sbi_timer_event *ev)
{
	إذا (ev->hart_شاخص > -1) {
		sbi_list_del(&ev->سر);
		ev->hart_شاخص = -1;
	}
}

ساكن فراغ __sbi_timer_event_start(بنية timer_state *tstate,
				    بنية sbi_timer_event *ev, u64 بل_event)
	MUST_HOLD(&tstate->event_list_قلف)
{
	بنية sbi_timer_event *tev, *بل_ev = عدم;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_ثبت(tev, &tstate->event_list, سر) {
		إذا (بل_event < tev->وخت_stamp) {
			بل_ev = tev;
			اكسر;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_شاخص = current_hartindex();
	ev->وخت_stamp = بل_event;
	إذا (بل_ev)
		sbi_list_جمع_کړه(&ev->سر, &بل_ev->سر);
	وإلا
		sbi_list_جمع_کړه_لکۍ(&ev->سر, &tstate->event_list);
}

فراغ sbi_timer_event_پیل(بنية sbi_timer_event *ev, u64 بل_event)
{
	بنية timer_state *tstate;

	إذا (!ev)
		أرجع;

	/* Ensure that event is not on the per-HART event list */
	إذا (ev->hart_شاخص > -1) {
		tstate = sbi_scratch_دننه_واټن_ptr(sbi_hartindex_منزل_scratch(ev->hart_شاخص),
						timer_state_off);
		spin_قلف(&tstate->event_list_قلف);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_قلف);
	}

	tstate = sbi_scratch_thishart_دننه_واټن_ptr(timer_state_off);
	spin_قلف(&tstate->event_list_قلف);

	__sbi_timer_event_start(tstate, ev, بل_event);
	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_قلف);
}

فراغ sbi_timer_event_stop(بنية sbi_timer_event *ev)
{
	بنية timer_state *tstate;
	صحيح ev_hart_شاخص;

	إذا (!ev)
		أرجع;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_شاخص = ev->hart_شاخص;
	إذا (ev->hart_شاخص > -1) {
		tstate = sbi_scratch_دننه_واټن_ptr(sbi_hartindex_منزل_scratch(ev->hart_شاخص),
						timer_state_off);
		spin_قلف(&tstate->event_list_قلف);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_قلف);
	}

	/* Re-program timer device on the current HART */
	إذا (ev_hart_شاخص == current_hartindex()) {
		tstate = sbi_scratch_thishart_دننه_واټن_ptr(timer_state_off);
		spin_قلف(&tstate->event_list_قلف);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_قلف);
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
		csr_سېټ(CSR_MIP, MIP_STIP);
}

ساكن فراغ sbi_timer_smode_event_cleanup(بنية sbi_timer_event *ev)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

فراغ sbi_timer_smode_event_پیل(u64 بل_event)
{
	بنية timer_state *tstate = sbi_scratch_دننه_واټن_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_سېټ_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	إذا (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, بل_event);
	} وإلا {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_پیل(&tstate->smode_ev, بل_event);
	}
}

فراغ sbi_timer_process(فراغ)
{
	بنية timer_state *tstate = sbi_scratch_thishart_دننه_واټن_ptr(timer_state_off);
	بنية sbi_timer_event_restart restart;
	SBI_LIST_سر(restart_list);
	بنية sbi_timer_event *ev;

	spin_قلف(&tstate->event_list_قلف);

	طالما (!sbi_list_تشه(&tstate->event_list)) {
		ev = sbi_list_لومړی_ثبت(&tstate->event_list, بنية sbi_timer_event, سر);
		إذا (ev->وخت_stamp > sbi_timer_ارزښت())
			اكسر;

		__sbi_timer_event_stop(ev);
		إذا (ev->callback) {
			restart.required = false;
			restart.بل_event = 0;
			ev->callback(ev, &restart);
			إذا (restart.required) {
				ev->وخت_stamp = restart.بل_event;
				sbi_list_جمع_کړه_لکۍ(&ev->سر, &restart_list);
			}
		}
	}

	طالما (!sbi_list_تشه(&restart_list)) {
		ev = sbi_list_لومړی_ثبت(&tstate->event_list, بنية sbi_timer_event, سر);
		sbi_list_del(&ev->سر);
		__sbi_timer_event_start(tstate, ev, ev->وخت_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_قلف);
}

ثابت بنية sbi_timer_device *sbi_timer_get_device(فراغ)
{
	أرجع timer_dev;
}

فراغ sbi_timer_سېټ_device(ثابت بنية sbi_timer_device *dev)
{
	إذا (!dev || timer_dev)
		أرجع;

	timer_dev = dev;
	إذا (!get_وخت_val && timer_dev->timer_ارزښت)
		get_وخت_val = timer_dev->timer_ارزښت;
}

صحيح sbi_timer_init(بنية sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	بنية timer_state *tstate;
	صحيح ret;

	إذا (cold_boot) {
		timer_state_off = sbi_scratch_alloc_دننه_واټن(حجم(*tstate));
		إذا (!timer_state_off)
			أرجع SBI_ENOMEM;

		إذا (sbi_hart_has_csr(scratch, SBI_HART_CSR_وخت))
			get_وخت_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		إذا (ret)
			أرجع ret;
	} وإلا {
		إذا (!timer_state_off)
			أرجع SBI_ENOMEM;
	}

	tstate = sbi_scratch_دننه_واټن_ptr(scratch, timer_state_off);
	tstate->وخت_delta = 0;
	SPIN_قلف_INIT(tstate->event_list_قلف);
	SBI_INIT_LIST_سر(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, عدم);

	إذا (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		إذا (ret)
			أرجع ret;
	}

	أرجع 0;
}

فراغ sbi_timer_exit(بنية sbi_scratch *scratch)
{
	بنية timer_state *tstate = sbi_scratch_thishart_دننه_واټن_ptr(timer_state_off);
	بنية sbi_timer_event *ev;

	spin_قلف(&tstate->event_list_قلف);

	طالما (!sbi_list_تشه(&tstate->event_list)) {
		ev = sbi_list_لومړی_ثبت(&tstate->event_list, بنية sbi_timer_event, سر);
		__sbi_timer_event_stop(ev);
		إذا (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_قلف);
}
