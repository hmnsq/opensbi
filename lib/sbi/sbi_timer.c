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
	u64 ویلا_delta;
	spinlock_t event_list_جندرا;
	بنية sbi_dlist event_list GUARDED_BY(&event_list_جندرا);
	بنية sbi_timer_event smode_ev;
};

ساكن بنانشانی لما timer_state_off;
ساكن u64 (*get_ویلا_val)(فراغ);
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
	بنانشانی لما n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	أرجع n;
}
#جے_انت

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
	u64 شروع_val, delta;

	/* Do nothing if we don't have timer device */
	إذا (!timer_dev || !get_ویلا_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		أرجع;
	}

	/* Save starting timer value */
	شروع_val = get_ویلا_val();

	/* Use NOP delay function if delay function not available */
	إذا (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	طالما ((get_ویلا_val() - شروع_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(فراغ *), فراغ *arg,
			    uint64_t timeout_ms)
{
	uint64_t شروع_ویلا = sbi_timer_قدر();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	طالما(!predicate(arg))
		إذا (sbi_timer_قدر() - شروع_ویلا  >= ticks)
			أرجع false;
	أرجع true;
}

بنانشانی لما sbi_timer_frequency(فراغ)
{
	ثابت بنية sbi_timer_device *timer_dev = sbi_timer_get_device();

	إذا (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		أرجع 0;
	}

	أرجع timer_dev->timer_freq;
}

u64 sbi_timer_قدر(فراغ)
{
	إذا (get_ویلا_val)
		أرجع get_ویلا_val();
	أرجع 0;
}

u64 sbi_timer_virt_قدر(فراغ)
{
	بنية timer_state *tstate = sbi_scratch_thishart_وتھ_ptr(timer_state_off);

	أرجع sbi_timer_قدر() + tstate->ویلا_delta;
}

u64 sbi_timer_get_delta(فراغ)
{
	بنية timer_state *tstate = sbi_scratch_thishart_وتھ_ptr(timer_state_off);

	أرجع tstate->ویلا_delta;
}

فراغ sbi_timer_سیٹ_delta(ulong delta)
{
	بنية timer_state *tstate = sbi_scratch_thishart_وتھ_ptr(timer_state_off);

#إذا __riscv_xlen == 32
	tstate->ویلا_delta &= ~0xffffffffUL;
	tstate->ویلا_delta |= (u32)delta;
#وإلا
	tstate->ویلا_delta = delta;
#جے_انت
}

#إذا __riscv_xlen == 32
فراغ sbi_timer_سیٹ_delta_اتلا(ulong delta_اتلا)
{
	بنية timer_state *tstate = sbi_scratch_thishart_وتھ_ptr(timer_state_off);

	tstate->ویلا_delta &= 0xffffffffUL;
	tstate->ویلا_delta |= (u64)delta_اتلا << 32;
}
#جے_انت

ساكن فراغ __sbi_timer_update_device(بنية timer_state *tstate)
	MUST_HOLD(&tstate->event_list_جندرا)
{
	بنية sbi_timer_event *ev;

	إذا (!timer_dev)
		أرجع;

	إذا (sbi_list_خالی_تھاواں(&tstate->event_list)) {
		إذا (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} وإلا {
		ev = sbi_list_پہلا_اندراج(&tstate->event_list, بنية sbi_timer_event, سر);
		إذا (timer_dev->timer_event_شروع)
			timer_dev->timer_event_شروع(ev->ویلا_stamp);
		csr_سیٹ(CSR_MIE, MIP_MTIP);
	}
}

ساكن فراغ __sbi_timer_event_stop(بنية sbi_timer_event *ev)
{
	إذا (ev->hart_اشاریہ > -1) {
		sbi_list_del(&ev->سر);
		ev->hart_اشاریہ = -1;
	}
}

ساكن فراغ __sbi_timer_event_start(بنية timer_state *tstate,
				    بنية sbi_timer_event *ev, u64 اگلا_event)
	MUST_HOLD(&tstate->event_list_جندرا)
{
	بنية sbi_timer_event *tev, *اگلا_ev = عدم;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_اندراج(tev, &tstate->event_list, سر) {
		إذا (اگلا_event < tev->ویلا_stamp) {
			اگلا_ev = tev;
			اكسر;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_اشاریہ = current_hartindex();
	ev->ویلا_stamp = اگلا_event;
	إذا (اگلا_ev)
		sbi_list_جوڑو(&ev->سر, &اگلا_ev->سر);
	وإلا
		sbi_list_جوڑو_پوچھ(&ev->سر, &tstate->event_list);
}

فراغ sbi_timer_event_شروع(بنية sbi_timer_event *ev, u64 اگلا_event)
{
	بنية timer_state *tstate;

	إذا (!ev)
		أرجع;

	/* Ensure that event is not on the per-HART event list */
	إذا (ev->hart_اشاریہ > -1) {
		tstate = sbi_scratch_وتھ_ptr(sbi_hartindex_منزل_scratch(ev->hart_اشاریہ),
						timer_state_off);
		spin_جندرا(&tstate->event_list_جندرا);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_جندرا);
	}

	tstate = sbi_scratch_thishart_وتھ_ptr(timer_state_off);
	spin_جندرا(&tstate->event_list_جندرا);

	__sbi_timer_event_start(tstate, ev, اگلا_event);
	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_جندرا);
}

فراغ sbi_timer_event_stop(بنية sbi_timer_event *ev)
{
	بنية timer_state *tstate;
	صحيح ev_hart_اشاریہ;

	إذا (!ev)
		أرجع;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_اشاریہ = ev->hart_اشاریہ;
	إذا (ev->hart_اشاریہ > -1) {
		tstate = sbi_scratch_وتھ_ptr(sbi_hartindex_منزل_scratch(ev->hart_اشاریہ),
						timer_state_off);
		spin_جندرا(&tstate->event_list_جندرا);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_جندرا);
	}

	/* Re-program timer device on the current HART */
	إذا (ev_hart_اشاریہ == current_hartindex()) {
		tstate = sbi_scratch_thishart_وتھ_ptr(timer_state_off);
		spin_جندرا(&tstate->event_list_جندرا);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_جندرا);
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
		csr_سیٹ(CSR_MIP, MIP_STIP);
}

ساكن فراغ sbi_timer_smode_event_cleanup(بنية sbi_timer_event *ev)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

فراغ sbi_timer_smode_event_شروع(u64 اگلا_event)
{
	بنية timer_state *tstate = sbi_scratch_وتھ_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_سیٹ_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	إذا (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, اگلا_event);
	} وإلا {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_شروع(&tstate->smode_ev, اگلا_event);
	}
}

فراغ sbi_timer_process(فراغ)
{
	بنية timer_state *tstate = sbi_scratch_thishart_وتھ_ptr(timer_state_off);
	بنية sbi_timer_event_restart restart;
	SBI_LIST_سر(restart_list);
	بنية sbi_timer_event *ev;

	spin_جندرا(&tstate->event_list_جندرا);

	طالما (!sbi_list_خالی_تھاواں(&tstate->event_list)) {
		ev = sbi_list_پہلا_اندراج(&tstate->event_list, بنية sbi_timer_event, سر);
		إذا (ev->ویلا_stamp > sbi_timer_قدر())
			اكسر;

		__sbi_timer_event_stop(ev);
		إذا (ev->callback) {
			restart.required = false;
			restart.اگلا_event = 0;
			ev->callback(ev, &restart);
			إذا (restart.required) {
				ev->ویلا_stamp = restart.اگلا_event;
				sbi_list_جوڑو_پوچھ(&ev->سر, &restart_list);
			}
		}
	}

	طالما (!sbi_list_خالی_تھاواں(&restart_list)) {
		ev = sbi_list_پہلا_اندراج(&tstate->event_list, بنية sbi_timer_event, سر);
		sbi_list_del(&ev->سر);
		__sbi_timer_event_start(tstate, ev, ev->ویلا_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_جندرا);
}

ثابت بنية sbi_timer_device *sbi_timer_get_device(فراغ)
{
	أرجع timer_dev;
}

فراغ sbi_timer_سیٹ_device(ثابت بنية sbi_timer_device *dev)
{
	إذا (!dev || timer_dev)
		أرجع;

	timer_dev = dev;
	إذا (!get_ویلا_val && timer_dev->timer_قدر)
		get_ویلا_val = timer_dev->timer_قدر;
}

صحيح sbi_timer_init(بنية sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	بنية timer_state *tstate;
	صحيح ret;

	إذا (cold_boot) {
		timer_state_off = sbi_scratch_alloc_وتھ(حجم(*tstate));
		إذا (!timer_state_off)
			أرجع SBI_ENOMEM;

		إذا (sbi_hart_has_csr(scratch, SBI_HART_CSR_ویلا))
			get_ویلا_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		إذا (ret)
			أرجع ret;
	} وإلا {
		إذا (!timer_state_off)
			أرجع SBI_ENOMEM;
	}

	tstate = sbi_scratch_وتھ_ptr(scratch, timer_state_off);
	tstate->ویلا_delta = 0;
	SPIN_جندرا_INIT(tstate->event_list_جندرا);
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
	بنية timer_state *tstate = sbi_scratch_thishart_وتھ_ptr(timer_state_off);
	بنية sbi_timer_event *ev;

	spin_جندرا(&tstate->event_list_جندرا);

	طالما (!sbi_list_خالی_تھاواں(&tstate->event_list)) {
		ev = sbi_list_پہلا_اندراج(&tstate->event_list, بنية sbi_timer_event, سر);
		__sbi_timer_event_stop(ev);
		إذا (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_جندرا);
}
