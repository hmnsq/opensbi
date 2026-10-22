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

ساخت timer_state {
	u64 وقت_delta;
	spinlock_t event_list_تالا;
	ساخت sbi_dlist event_list GUARDED_BY(&event_list_تالا);
	ساخت sbi_timer_event smode_ev;
};

ساکن غیرعلامتی طویل timer_state_off;
ساکن u64 (*get_وقت_val)(خالی);
ساکن ثابت ساخت sbi_timer_device *timer_dev = تہی;

#اگر __riscv_xlen == 32
ساکن u64 get_ticks(خالی)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	واپس ((u64)hi << 32) | lo;
}
#ورنہ
ساکن u64 get_ticks(خالی)
{
	غیرعلامتی طویل n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	واپس n;
}
#اگر_ختم

ساکن خالی nop_delay_fn(خالی *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	واپس delta;
}

خالی sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  خالی (*delay_fn)(خالی *), خالی *opaque)
{
	u64 آغاز_val, delta;

	/* Do nothing if we don't have timer device */
	اگر (!timer_dev || !get_وقت_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		واپس;
	}

	/* Save starting timer value */
	آغاز_val = get_وقت_val();

	/* Use NOP delay function if delay function not available */
	اگر (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	جب ((get_وقت_val() - آغاز_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(خالی *), خالی *arg,
			    uint64_t timeout_ms)
{
	uint64_t آغاز_وقت = sbi_timer_قدر();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	جب(!predicate(arg))
		اگر (sbi_timer_قدر() - آغاز_وقت  >= ticks)
			واپس false;
	واپس true;
}

غیرعلامتی طویل sbi_timer_frequency(خالی)
{
	ثابت ساخت sbi_timer_device *timer_dev = sbi_timer_get_device();

	اگر (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		واپس 0;
	}

	واپس timer_dev->timer_freq;
}

u64 sbi_timer_قدر(خالی)
{
	اگر (get_وقت_val)
		واپس get_وقت_val();
	واپس 0;
}

u64 sbi_timer_virt_قدر(خالی)
{
	ساخت timer_state *tstate = sbi_scratch_thishart_انحراف_ptr(timer_state_off);

	واپس sbi_timer_قدر() + tstate->وقت_delta;
}

u64 sbi_timer_get_delta(خالی)
{
	ساخت timer_state *tstate = sbi_scratch_thishart_انحراف_ptr(timer_state_off);

	واپس tstate->وقت_delta;
}

خالی sbi_timer_سیٹ_delta(ulong delta)
{
	ساخت timer_state *tstate = sbi_scratch_thishart_انحراف_ptr(timer_state_off);

#اگر __riscv_xlen == 32
	tstate->وقت_delta &= ~0xffffffffUL;
	tstate->وقت_delta |= (u32)delta;
#ورنہ
	tstate->وقت_delta = delta;
#اگر_ختم
}

#اگر __riscv_xlen == 32
خالی sbi_timer_سیٹ_delta_بالائی(ulong delta_بالائی)
{
	ساخت timer_state *tstate = sbi_scratch_thishart_انحراف_ptr(timer_state_off);

	tstate->وقت_delta &= 0xffffffffUL;
	tstate->وقت_delta |= (u64)delta_بالائی << 32;
}
#اگر_ختم

ساکن خالی __sbi_timer_update_device(ساخت timer_state *tstate)
	MUST_HOLD(&tstate->event_list_تالا)
{
	ساخت sbi_timer_event *ev;

	اگر (!timer_dev)
		واپس;

	اگر (sbi_list_خالی_خانے(&tstate->event_list)) {
		اگر (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} ورنہ {
		ev = sbi_list_پہلا_اندراج(&tstate->event_list, ساخت sbi_timer_event, سر);
		اگر (timer_dev->timer_event_آغاز)
			timer_dev->timer_event_آغاز(ev->وقت_stamp);
		csr_سیٹ(CSR_MIE, MIP_MTIP);
	}
}

ساکن خالی __sbi_timer_event_stop(ساخت sbi_timer_event *ev)
{
	اگر (ev->hart_اشاریہ > -1) {
		sbi_list_del(&ev->سر);
		ev->hart_اشاریہ = -1;
	}
}

ساکن خالی __sbi_timer_event_start(ساخت timer_state *tstate,
				    ساخت sbi_timer_event *ev, u64 اگلا_event)
	MUST_HOLD(&tstate->event_list_تالا)
{
	ساخت sbi_timer_event *tev, *اگلا_ev = تہی;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_اندراج(tev, &tstate->event_list, سر) {
		اگر (اگلا_event < tev->وقت_stamp) {
			اگلا_ev = tev;
			توڑو;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_اشاریہ = current_hartindex();
	ev->وقت_stamp = اگلا_event;
	اگر (اگلا_ev)
		sbi_list_جوڑو(&ev->سر, &اگلا_ev->سر);
	ورنہ
		sbi_list_جوڑو_دم(&ev->سر, &tstate->event_list);
}

خالی sbi_timer_event_آغاز(ساخت sbi_timer_event *ev, u64 اگلا_event)
{
	ساخت timer_state *tstate;

	اگر (!ev)
		واپس;

	/* Ensure that event is not on the per-HART event list */
	اگر (ev->hart_اشاریہ > -1) {
		tstate = sbi_scratch_انحراف_ptr(sbi_hartindex_مقصد_scratch(ev->hart_اشاریہ),
						timer_state_off);
		spin_تالا(&tstate->event_list_تالا);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_تالا);
	}

	tstate = sbi_scratch_thishart_انحراف_ptr(timer_state_off);
	spin_تالا(&tstate->event_list_تالا);

	__sbi_timer_event_start(tstate, ev, اگلا_event);
	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_تالا);
}

خالی sbi_timer_event_stop(ساخت sbi_timer_event *ev)
{
	ساخت timer_state *tstate;
	صحیح ev_hart_اشاریہ;

	اگر (!ev)
		واپس;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_اشاریہ = ev->hart_اشاریہ;
	اگر (ev->hart_اشاریہ > -1) {
		tstate = sbi_scratch_انحراف_ptr(sbi_hartindex_مقصد_scratch(ev->hart_اشاریہ),
						timer_state_off);
		spin_تالا(&tstate->event_list_تالا);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_تالا);
	}

	/* Re-program timer device on the current HART */
	اگر (ev_hart_اشاریہ == current_hartindex()) {
		tstate = sbi_scratch_thishart_انحراف_ptr(timer_state_off);
		spin_تالا(&tstate->event_list_تالا);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_تالا);
	}
}

ساکن خالی sbi_timer_smode_event_callback(ساخت sbi_timer_event *ev,
					   ساخت sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_سیٹ(CSR_MIP, MIP_STIP);
}

ساکن خالی sbi_timer_smode_event_cleanup(ساخت sbi_timer_event *ev)
{
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

خالی sbi_timer_smode_event_آغاز(u64 اگلا_event)
{
	ساخت timer_state *tstate = sbi_scratch_انحراف_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_سیٹ_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	اگر (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, اگلا_event);
	} ورنہ {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_آغاز(&tstate->smode_ev, اگلا_event);
	}
}

خالی sbi_timer_process(خالی)
{
	ساخت timer_state *tstate = sbi_scratch_thishart_انحراف_ptr(timer_state_off);
	ساخت sbi_timer_event_restart restart;
	SBI_LIST_سر(restart_list);
	ساخت sbi_timer_event *ev;

	spin_تالا(&tstate->event_list_تالا);

	جب (!sbi_list_خالی_خانے(&tstate->event_list)) {
		ev = sbi_list_پہلا_اندراج(&tstate->event_list, ساخت sbi_timer_event, سر);
		اگر (ev->وقت_stamp > sbi_timer_قدر())
			توڑو;

		__sbi_timer_event_stop(ev);
		اگر (ev->callback) {
			restart.required = false;
			restart.اگلا_event = 0;
			ev->callback(ev, &restart);
			اگر (restart.required) {
				ev->وقت_stamp = restart.اگلا_event;
				sbi_list_جوڑو_دم(&ev->سر, &restart_list);
			}
		}
	}

	جب (!sbi_list_خالی_خانے(&restart_list)) {
		ev = sbi_list_پہلا_اندراج(&tstate->event_list, ساخت sbi_timer_event, سر);
		sbi_list_del(&ev->سر);
		__sbi_timer_event_start(tstate, ev, ev->وقت_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_تالا);
}

ثابت ساخت sbi_timer_device *sbi_timer_get_device(خالی)
{
	واپس timer_dev;
}

خالی sbi_timer_سیٹ_device(ثابت ساخت sbi_timer_device *dev)
{
	اگر (!dev || timer_dev)
		واپس;

	timer_dev = dev;
	اگر (!get_وقت_val && timer_dev->timer_قدر)
		get_وقت_val = timer_dev->timer_قدر;
}

صحیح sbi_timer_init(ساخت sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	ثابت ساخت sbi_platform *plat = sbi_platform_ptr(scratch);
	ساخت timer_state *tstate;
	صحیح ret;

	اگر (cold_boot) {
		timer_state_off = sbi_scratch_alloc_انحراف(حجم(*tstate));
		اگر (!timer_state_off)
			واپس SBI_ENOMEM;

		اگر (sbi_hart_has_csr(scratch, SBI_HART_CSR_وقت))
			get_وقت_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		اگر (ret)
			واپس ret;
	} ورنہ {
		اگر (!timer_state_off)
			واپس SBI_ENOMEM;
	}

	tstate = sbi_scratch_انحراف_ptr(scratch, timer_state_off);
	tstate->وقت_delta = 0;
	SPIN_تالا_INIT(tstate->event_list_تالا);
	SBI_INIT_LIST_سر(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, تہی);

	اگر (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		اگر (ret)
			واپس ret;
	}

	واپس 0;
}

خالی sbi_timer_exit(ساخت sbi_scratch *scratch)
{
	ساخت timer_state *tstate = sbi_scratch_thishart_انحراف_ptr(timer_state_off);
	ساخت sbi_timer_event *ev;

	spin_تالا(&tstate->event_list_تالا);

	جب (!sbi_list_خالی_خانے(&tstate->event_list)) {
		ev = sbi_list_پہلا_اندراج(&tstate->event_list, ساخت sbi_timer_event, سر);
		__sbi_timer_event_stop(ev);
		اگر (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_تالا);
}
