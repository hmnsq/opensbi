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

struct timer_state {
	u64 time_delta;
	spinlock_t event_list_lock;
	struct sbi_dlist event_list GUARDED_BY(&event_list_lock);
	struct sbi_timer_event smode_ev;
};

static unsigned long timer_state_off;
static u64 (*get_time_val)(void);
static const struct sbi_timer_device *timer_dev = NULL;

#если __riscv_xlen == 32
static u64 get_ticks(void)
{
	u32 lo, hi, tmp;
	__asm__ __volatile__("1:\n"
			     "rdtimeh %0\n"
			     "rdtime %1\n"
			     "rdtimeh %2\n"
			     "bne %0, %2, 1b"
			     : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
	вернуть ((u64)hi << 32) | lo;
}
#иначе
static u64 get_ticks(void)
{
	unsigned long n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	вернуть n;
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
	вернуть delta;
}

void sbi_timer_delay_loop(ulong units, u64 unit_freq,
			  void (*delay_fn)(void *), void *opaque)
{
	u64 start_val, delta;

	/* Do nothing if we don't have timer device */
	если (!timer_dev || !get_time_val) {
		sbi_printf("%s: called without timer device\n", __func__);
		вернуть;
	}

	/* Save starting timer value */
	start_val = get_time_val();

	/* Use NOP delay function if delay function not available */
	если (!delay_fn)
		delay_fn = nop_delay_fn;

	/* Busy loop until desired timer value delta reached */
	delta = sbi_timer_compute_delta(units, unit_freq);
	пока ((get_time_val() - start_val) < delta)
		delay_fn(opaque);
}

bool sbi_timer_waitms_until(bool (*predicate)(void *), void *arg,
			    uint64_t timeout_ms)
{
	uint64_t start_time = sbi_timer_value();
	uint64_t ticks =
		(sbi_timer_get_device()->timer_freq / 1000) *
		timeout_ms;
	пока(!predicate(arg))
		если (sbi_timer_value() - start_time  >= ticks)
			вернуть false;
	вернуть true;
}

unsigned long sbi_timer_frequency(void)
{
	const struct sbi_timer_device *timer_dev = sbi_timer_get_device();

	если (!timer_dev) {
		sbi_printf("%s: called without timer device\n", __func__);
		вернуть 0;
	}

	вернуть timer_dev->timer_freq;
}

u64 sbi_timer_value(void)
{
	если (get_time_val)
		вернуть get_time_val();
	вернуть 0;
}

u64 sbi_timer_virt_value(void)
{
	struct timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	вернуть sbi_timer_value() + tstate->time_delta;
}

u64 sbi_timer_get_delta(void)
{
	struct timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	вернуть tstate->time_delta;
}

void sbi_timer_set_delta(ulong delta)
{
	struct timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

#если __riscv_xlen == 32
	tstate->time_delta &= ~0xffffffffUL;
	tstate->time_delta |= (u32)delta;
#иначе
	tstate->time_delta = delta;
#endif
}

#если __riscv_xlen == 32
void sbi_timer_set_delta_upper(ulong delta_upper)
{
	struct timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);

	tstate->time_delta &= 0xffffffffUL;
	tstate->time_delta |= (u64)delta_upper << 32;
}
#endif

static void __sbi_timer_update_device(struct timer_state *tstate)
	MUST_HOLD(&tstate->event_list_lock)
{
	struct sbi_timer_event *ev;

	если (!timer_dev)
		вернуть;

	если (sbi_list_empty(&tstate->event_list)) {
		если (timer_dev->timer_event_stop)
			timer_dev->timer_event_stop();
		csr_clear(CSR_MIE, MIP_MTIP);
	} иначе {
		ev = sbi_list_first_entry(&tstate->event_list, struct sbi_timer_event, head);
		если (timer_dev->timer_event_start)
			timer_dev->timer_event_start(ev->time_stamp);
		csr_set(CSR_MIE, MIP_MTIP);
	}
}

static void __sbi_timer_event_stop(struct sbi_timer_event *ev)
{
	если (ev->hart_index > -1) {
		sbi_list_del(&ev->head);
		ev->hart_index = -1;
	}
}

static void __sbi_timer_event_start(struct timer_state *tstate,
				    struct sbi_timer_event *ev, u64 next_event)
	MUST_HOLD(&tstate->event_list_lock)
{
	struct sbi_timer_event *tev, *next_ev = NULL;

	/* Find where to insert the event in per-HART event list */
	sbi_list_for_each_entry(tev, &tstate->event_list, head) {
		если (next_event < tev->time_stamp) {
			next_ev = tev;
			break;
		}
	}

	/* Insert the event in per-HART event list */
	ev->hart_index = current_hartindex();
	ev->time_stamp = next_event;
	если (next_ev)
		sbi_list_add(&ev->head, &next_ev->head);
	иначе
		sbi_list_add_tail(&ev->head, &tstate->event_list);
}

void sbi_timer_event_start(struct sbi_timer_event *ev, u64 next_event)
{
	struct timer_state *tstate;

	если (!ev)
		вернуть;

	/* Ensure that event is not on the per-HART event list */
	если (ev->hart_index > -1) {
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

void sbi_timer_event_stop(struct sbi_timer_event *ev)
{
	struct timer_state *tstate;
	целое ev_hart_index;

	если (!ev)
		вернуть;

	/* Ensure that event is not on the per-HART event list */
	ev_hart_index = ev->hart_index;
	если (ev->hart_index > -1) {
		tstate = sbi_scratch_offset_ptr(sbi_hartindex_to_scratch(ev->hart_index),
						timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_event_stop(ev);
		spin_unlock(&tstate->event_list_lock);
	}

	/* Re-program timer device on the current HART */
	если (ev_hart_index == current_hartindex()) {
		tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
		spin_lock(&tstate->event_list_lock);
		__sbi_timer_update_device(tstate);
		spin_unlock(&tstate->event_list_lock);
	}
}

static void sbi_timer_smode_event_callback(struct sbi_timer_event *ev,
					   struct sbi_timer_event_restart *restart)
{
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	если (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_set(CSR_MIP, MIP_STIP);
}

static void sbi_timer_smode_event_cleanup(struct sbi_timer_event *ev)
{
	если (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_clear(CSR_MIP, MIP_STIP);
}

void sbi_timer_smode_event_start(u64 next_event)
{
	struct timer_state *tstate = sbi_scratch_offset_ptr(sbi_scratch_thishart_ptr(),
							    timer_state_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_SET_TIMER);

	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	если (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
		csr_write64(CSR_STIMECMP, next_event);
	} иначе {
		csr_clear(CSR_MIP, MIP_STIP);
		sbi_timer_event_start(&tstate->smode_ev, next_event);
	}
}

void sbi_timer_process(void)
{
	struct timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	struct sbi_timer_event_restart restart;
	SBI_LIST_HEAD(restart_list);
	struct sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	пока (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, struct sbi_timer_event, head);
		если (ev->time_stamp > sbi_timer_value())
			break;

		__sbi_timer_event_stop(ev);
		если (ev->callback) {
			restart.required = false;
			restart.next_event = 0;
			ev->callback(ev, &restart);
			если (restart.required) {
				ev->time_stamp = restart.next_event;
				sbi_list_add_tail(&ev->head, &restart_list);
			}
		}
	}

	пока (!sbi_list_empty(&restart_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, struct sbi_timer_event, head);
		sbi_list_del(&ev->head);
		__sbi_timer_event_start(tstate, ev, ev->time_stamp);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}

const struct sbi_timer_device *sbi_timer_get_device(void)
{
	вернуть timer_dev;
}

void sbi_timer_set_device(const struct sbi_timer_device *dev)
{
	если (!dev || timer_dev)
		вернуть;

	timer_dev = dev;
	если (!get_time_val && timer_dev->timer_value)
		get_time_val = timer_dev->timer_value;
}

целое sbi_timer_init(struct sbi_scratch *scratch, bool cold_boot)
	NO_THREAD_SAFETY_ANALYSIS
{
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	struct timer_state *tstate;
	целое ret;

	если (cold_boot) {
		timer_state_off = sbi_scratch_alloc_offset(sizeof(*tstate));
		если (!timer_state_off)
			вернуть SBI_ENOMEM;

		если (sbi_hart_has_csr(scratch, SBI_HART_CSR_TIME))
			get_time_val = get_ticks;

		ret = sbi_platform_timer_init(plat);
		если (ret)
			вернуть ret;
	} иначе {
		если (!timer_state_off)
			вернуть SBI_ENOMEM;
	}

	tstate = sbi_scratch_offset_ptr(scratch, timer_state_off);
	tstate->time_delta = 0;
	SPIN_LOCK_INIT(tstate->event_list_lock);
	SBI_INIT_LIST_HEAD(&tstate->event_list);
	SBI_INIT_TIMER_EVENT(&tstate->smode_ev,
			     sbi_timer_smode_event_callback,
			     sbi_timer_smode_event_cleanup, NULL);

	если (timer_dev && timer_dev->warm_init) {
		ret = timer_dev->warm_init();
		если (ret)
			вернуть ret;
	}

	вернуть 0;
}

void sbi_timer_exit(struct sbi_scratch *scratch)
{
	struct timer_state *tstate = sbi_scratch_thishart_offset_ptr(timer_state_off);
	struct sbi_timer_event *ev;

	spin_lock(&tstate->event_list_lock);

	пока (!sbi_list_empty(&tstate->event_list)) {
		ev = sbi_list_first_entry(&tstate->event_list, struct sbi_timer_event, head);
		__sbi_timer_event_stop(ev);
		если (ev->cleanup)
			ev->cleanup(ev);
	}

	__sbi_timer_update_device(tstate);

	spin_unlock(&tstate->event_list_lock);
}
