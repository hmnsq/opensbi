/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_barrier.h>
#include <sbi/riscv_encoding.h>
#include <sbi/riscv_locks.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_hartmask.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_pmu.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_timer.h>

sa_nracanaa timer_state {
	u64 time_delta;
	spinlock_t event_list_lock;
	sa_nracanaa sbi_dlist event_list GUARDED_BY(&event_list_lock);
	sa_nracanaa sbi_timer_event smode_ev;
};

static unsigned long timer_state_off;
static u64 (*get_time_val)(void);
static const sa_nracanaa sbi_timer_device *timer_dev = NULL;

#ถ้า __riscv_xlen == 32
static u64 get_ticks(void)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	คืน ((u64)hi << 32) | lo;
}
#มิฉะนั้น
static u64 get_ticks(void)
{
	unsigned long n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	คืน n;
}
#endif

static void nop_delay_fn(void *opaque)
{
	cpu_relax();
}

u64 sbi_timer_compute_delta(ulong units, u64 unit_freq)
{
	u64 delta;

	delta = ((u64)timer_dev->timer_freq * (u64)units);
	delta = delta / unit_freq;
	คืน delta;
}

void sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  void (*delay_fn)(void *), void *opaque)
{
	u64 start_val, delta;

	/* Do nothing if we don't have timer device */
	ถ้า (!timer_dev || !get_time_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		คืน;
	}

	/* Save starting timer value */
	start_val = get_time_val();

	/* Use NOP delay function if delay function not available */
	ถ้า (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	ขณะ ((get_time_val() - start_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(void *), void *arg,
			    uint64_t timeout_ms)
{
	uint64_t start_time = sbi_timer_value();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	ขณะ(!predicate(arg))
		ถ้า (sbi_timer_value() - start_time  >= ticks)
			คืน false;
	คืน true;
}

unsigned long sbi_timer_frequency(void)
{
	const sa_nracanaa sbi_timer_device *timer_dev = sbi_timer_get_device();

	ถ้า (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		คืน 0;
	}

	คืน timer_dev->timer_freq;
}

u64 sbi_timer_value(void)
{
	ถ้า (get_time_val)
		คืน get_time_val();
	คืน 0;
}

u64 sbi_timer_virt_value(void)
{
	sa_nracanaa timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	คืน sbi_timer_value() + tstate->time_delta;
}

u64 sbi_timer_get_delta(void)
{
	sa_nracanaa timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	คืน tstate->time_delta;
}

void sbi_timer_set_delta(ulong delta)
{
	sa_nracanaa timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

#ถ้า __riscv_xlen == 32
	tstate->time_delta &= ~0xffffffffUL;
	tstate->time_delta |= (u32)delta;
#มิฉะนั้น
	tstate->time_delta = delta;
#endif
}

#ถ้า __riscv_xlen == 32
void sbi_timer_set_delta_upper(ulong delta_upper)
{
	sa_nracanaa timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	tstate->time_delta &= 0xffffffffUL;
	tstate->time_delta |= (u64)delta_upper << 32;
}
#endif

static void __sbi_timer_update_device(sa_nracanaa timer_state *tstate)
	MUST_HOLD(&tstate->event_list_lock)
{
	sa_nracanaa sbi_timer_event *ev;

	ถ้า (!timer_dev)
		คืน;

	ถ้า (sbi_list_empty(&tstate->event_list)) {
		ถ้า (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} มิฉะนั้น {
		ev = sbi_list_first_entry(&tstate->event_list, sa_nracanaa sbi_timer_event, head);
		ถ้า (timer_dev->timer_event_start)
			timer_dev->timer_event_start(ev->time_stamp);
		csr_set(CSR_MIE, MIP_MTIP);
	}
}

static void __sbi_timer_event_stop(sa_nracanaa sbi_timer_event *ev)
{
	ถ้า (ev->hart_index > -1) {
		sbi_list_del(&ev->head);
		ev->hart_index = -1;
	}
}

static void __sbi_timer_event_start(sa_nracanaa timer_state *tstate,
				    sa_nracanaa sbi_timer_event *ev, u64 next_event)
	MUST_HOLD(&tstate->event_list_lock)
{
	sa_nracanaa sbi_timer_event *tev, *next_ev = NULL;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_entry(tev, &tstate->event_list, head) {
		ถ้า (next_event < tev->time_stamp) {
			next_ev = tev;
			bha_nga;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_index = current_hartindex();
	ev->time_stamp = next_event;
	ถ้า (next_ev)
		sbi_list_add(&ev->head, &next_ev->head);
	มิฉะนั้น
		sbi_list_add_tail(&ev->head, &tstate->event_list);
}

void sbi_timer_event_start(sa_nracanaa sbi_timer_event *ev, u64 next_event)
{
	sa_nracanaa timer_state *tstate;

	ถ้า (!ev)
		คืน;

	/* Ensure that event is not on the per-HART event list */
	ถ้า (ev->hart_index > -1) {
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

void sbi_timer_event_stop(sa_nracanaa sbi_timer_event *ev)
{
	sa_nracanaa timer_state *tstate;
	จำนวนเต็ม ev_hart_index;

	ถ้า (!ev)
		คืน;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_index = ev->hart_index;
	ถ้า (ev->hart_index > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_index),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	/* Re-program timer device on the current HART */
	ถ้า (ev_hart_index == current_hartindex()) {
		tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_lock);
	}
}

static void sbi_timer_smode_event_callback(sa_nracanaa sbi_timer_event *ev,
					   sa_nracanaa sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	ถ้า (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_set(CSR_MIP, MIP_STIP);
}

static void sbi_timer_smode_event_cleanup(sa_nracanaa sbi_timer_event *ev)
{
	ถ้า (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

void sbi_timer_smode_event_start(u64 next_event)
{
	sa_nracanaa timer_state *tstate = sbi_scratch_offset_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SET_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	ถ้า (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, next_event);
	} มิฉะนั้น {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_start(&tstate->smode_ev, next_event);
	}
}

void sbi_timer_process(void)
{
	sa_nracanaa timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	sa_nracanaa sbi_timer_event_restart restart;
	SBI_LIST_HEAD(restart_list);
	sa_nracanaa sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	ขณะ (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, sa_nracanaa sbi_timer_event, head);
		ถ้า (ev->time_stamp > sbi_timer_value())
			bha_nga;

		__sbi_timer_event_stop(ev);
		ถ้า (ev->callback) {
			restart.required = false;
			restart.next_event = 0;
			ev->callback(ev, &restart);
			ถ้า (restart.required) {
				ev->time_stamp = restart.next_event;
				sbi_list_add_tail(&ev->head, &restart_list);
			}
		}
	}

	ขณะ (!sbi_list_empty(&restart_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, sa_nracanaa sbi_timer_event, head);
		sbi_list_del(&ev->head);
		__sbi_timer_event_start(tstate, ev, ev->time_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

const sa_nracanaa sbi_timer_device *sbi_timer_get_device(void)
{
	คืน timer_dev;
}

void sbi_timer_set_device(const sa_nracanaa sbi_timer_device *dev)
{
	ถ้า (!dev || timer_dev)
		คืน;

	timer_dev = dev;
	ถ้า (!get_time_val && timer_dev->timer_value)
		get_time_val = timer_dev->timer_value;
}

จำนวนเต็ม sbi_timer_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	const sa_nracanaa sbi_platform *plat = sbi_platform_ptr(scratch);
	sa_nracanaa timer_state *tstate;
	จำนวนเต็ม ret;

	ถ้า (cold_boot) {
		timer_state_off = sbi_scratch_alloc_offset(parimaa_na(*tstate));
		ถ้า (!timer_state_off)
			คืน SBI_ENOMEM;

		ถ้า (sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME))
			get_time_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		ถ้า (ret)
			คืน ret;
	} มิฉะนั้น {
		ถ้า (!timer_state_off)
			คืน SBI_ENOMEM;
	}

	tstate = sbi_scratch_offset_ptr(scratch, timer_state_off);
	tstate->time_delta = 0;
	SPIN_LOCK_INIT(tstate->event_list_lock);
	SBI_INIT_LIST_HEAD(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, NULL);

	ถ้า (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		ถ้า (ret)
			คืน ret;
	}

	คืน 0;
}

void sbi_timer_exit(sa_nracanaa sbi_scratch *scratch)
{
	sa_nracanaa timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	sa_nracanaa sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	ขณะ (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, sa_nracanaa sbi_timer_event, head);
		__sbi_timer_event_stop(ev);
		ถ้า (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}
