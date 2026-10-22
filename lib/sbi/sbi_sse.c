/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Rivos Systems Inc.
 *
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_barrier.h>
#include <sbi/riscv_encoding.h>
#include <sbi/riscv_locks.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_fifo.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_hart_protection.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_hsm.h>
#include <sbi/sbi_ipi.h>
#include <sbi/sbi_list.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_pmu.h>
#include <sbi/sbi_sse.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_slist.h>
#include <sbi/sbi_string.h>
#include <sbi/sbi_trap.h>

#include <sbi/sbi_console.h>

#define sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), void *, shs_ptr_off)

#define sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define sse_set_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), void *, shs_ptr_off, (__sse_state))

#define EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_BIT)

#define sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		const struct sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		nếu (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

struct sse_entry_state {
	/** entry pc */
	unsigned long pc;
	/** a6 register state */
	unsigned long arg;
};

struct sse_interrupted_state {
	/** sepc register state */
	unsigned long sepc;
	/** flags register state */
	unsigned long flags;
	/** a6 register state */
	unsigned long a6;
	/** a7 register state */
	unsigned long a7;
};

struct sse_ipi_inject_data {
	uint32_t event_id;
};

struct sbi_sse_event_attrs {
	unsigned long status;
	unsigned long prio;
	unsigned long config;
	unsigned long hartid;
	struct sse_entry_state entry;
	struct sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)                  \
	_Static_assert(                                          \
		((offsetof(struct sbi_sse_event_attrs, field)) / \
		 sizeof(unsigned long)) == attr_offset,          \
		"field " #field                                  \
		" from struct sbi_sse_event_attrs invalid offset, expected " #attr_offset)

assert_field_offset(status, SBI_SSE_ATTR_STATUS);
assert_field_offset(prio, SBI_SSE_ATTR_PRIO);
assert_field_offset(config, SBI_SSE_ATTR_CONFIG);
assert_field_offset(hartid, SBI_SSE_ATTR_PREFERRED_HART);
assert_field_offset(entry.pc, SBI_SSE_ATTR_ENTRY_PC);
assert_field_offset(entry.arg, SBI_SSE_ATTR_ENTRY_ARG);
assert_field_offset(interrupted.sepc, SBI_SSE_ATTR_INTERRUPTED_SEPC);
assert_field_offset(interrupted.flags, SBI_SSE_ATTR_INTERRUPTED_FLAGS);
assert_field_offset(interrupted.a6, SBI_SSE_ATTR_INTERRUPTED_A6);
assert_field_offset(interrupted.a7, SBI_SSE_ATTR_INTERRUPTED_A7);

struct sbi_sse_event {
	struct sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	struct sse_event_info *info;
	struct sbi_dlist node;
};

/** Per-hart state */
struct sse_hart_state {
	/**
	 * Lock that protects enabled_event_list
	 */
	spinlock_t enabled_event_lock;

	/* Priority sorted list of enabled events (global and local in >=
	 * ENABLED state). This list is protected by the enabled_event_lock.
	 *
	 * Global events can also be inserted in this list. Since these events
	 * can be accessed by all harts, we actually need to lock independently
	 * (see sse_global_event).
	 *
	 * Local events do not actually need to be locked since, we do
	 * not have preemption and there are solely accessed by the current
	 * hart. So when inserting a local event in this list, we just need to
	 * lock the list at insertion/removal time.
	 *
	 * When an event is in a state >= ENABLED, then it is inserted in the
	 * this enabled_event_list and thus can only be removed from this
	 * list upon disable ecall or on complete with ONESHOT flag.
	 */
	struct sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_lock);

	/**
	 * List of local events allocated at boot time.
	 */
	struct sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
struct sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t lock;

	/**
	 * global event struct
	 */
	struct sbi_sse_event event GUARDED_BY(&lock);
};

struct sse_event_info {
	uint32_t event_id;
	const struct sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_NODE(sse_event_info);
};

static unsigned số_nguyên local_event_count;
static unsigned số_nguyên global_event_count;
static struct sse_global_event *global_events;

static unsigned long sse_inject_fifo_off;
static unsigned long sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
static unsigned long shs_ptr_off;

static u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

static số_nguyên sse_ipi_inject_send(unsigned long hartid, uint32_t event_id);

struct sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_NODE_INIT(NULL),
};

struct sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_NODE_INIT(&global_software_event),
};

static SBI_SLIST_HEAD(supported_events, sse_event_info) =
				SBI_SLIST_HEAD_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
static const uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_HIGH_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_HIGH_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW,
	SBI_SSE_EVENT_LOCAL_LOW_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_LOW_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

static bool sse_is_standard_event(uint32_t event_id)
{
	số_nguyên i;

	cho (i = 0; i < array_size(standard_events); i++) {
		nếu (event_id == standard_events[i])
			trả_về true;
	}

	trả_về false;
}

static struct sse_event_info *sse_event_info_get(uint32_t event_id)
{
	struct sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		nếu (info->event_id == event_id)
			trả_về info;
	}

	trả_về NULL;
}

static unsigned long sse_event_state(struct sbi_sse_event *e)
{
	trả_về e->attrs.status & SBI_SSE_ATTR_STATUS_STATE_MASK;
}

static unsigned long sse_event_pending(struct sbi_sse_event *e)
{
	trả_về !!(e->attrs.status & BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET));
}

static bool sse_event_is_global(struct sbi_sse_event *e)
{
	trả_về EVENT_IS_GLOBAL(e->event_id);
}

static bool sse_event_is_local(struct sbi_sse_event *e)
{
	trả_về !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
static struct sse_hart_state *sse_get_hart_state(struct sbi_sse_event *e)
{
	struct sbi_scratch *s = sbi_hartindex_to_scratch(e->hartindex);

	trả_về sse_get_hart_state_ptr(s);
}

static struct sse_global_event *sse_get_global_event(struct sbi_sse_event *e)
{
	trả_về container_of(e, struct sse_global_event, event);
}

static void sse_event_set_state(struct sbi_sse_event *e,
				unsigned long new_state)
{
	e->attrs.status &= ~SBI_SSE_ATTR_STATUS_STATE_MASK;
	e->attrs.status |= new_state;
}

static số_nguyên sse_event_get(uint32_t event_id,
			 struct sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	unsigned số_nguyên i;
	struct sbi_sse_event *e;
	struct sse_hart_state *shs;

	nếu (!eret)
		trả_về SBI_EINVAL;

	nếu (EVENT_IS_GLOBAL(event_id)) {
		cho (i = 0; i < global_event_count; i++) {
			e = &global_events[i].event;
			nếu (e->event_id == event_id) {
				spin_lock(&global_events[i].lock);
				*eret = e;
				trả_về SBI_SUCCESS;
			}
		}
	} không_thì {
		shs = sse_thishart_state_ptr();
		cho (i = 0; i < local_event_count; i++) {
			e = &shs->local_events[i];
			nếu (e->event_id == event_id) {
				*eret = e;
				trả_về SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	nếu (sse_is_standard_event(event_id))
		trả_về SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	trả_về SBI_EINVAL;
}

static void sse_event_put(struct sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	struct sse_global_event *ge;

	nếu (sse_event_is_local(e))
		trả_về;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->lock);
}

static void sse_event_remove_from_list(struct sbi_sse_event *e)
{
	sbi_list_del(&e->node);
}

/**
 * Must be called under owner hart lock
 */
static void sse_event_add_to_list(struct sse_hart_state *state,
				   struct sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_lock)
{
	struct sbi_sse_event *tmp;

	sbi_list_for_each_entry(tmp, &state->enabled_event_list, node) {
		nếu (e->attrs.prio < tmp->attrs.prio)
			break;
		nếu (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			break;
	}
	sbi_list_add_tail(&e->node, &tmp->node);
}

/**
 * Must be called under owner hart lock
 */
static số_nguyên sse_event_disable(struct sse_hart_state *shs,
			      struct sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	nếu (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		trả_về SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	trả_về SBI_OK;
}

static số_nguyên sse_event_set_hart_id_check(struct sbi_sse_event *e,
				       unsigned long new_hartid)
{
	số_nguyên hstate;
	unsigned số_nguyên hartid = (uint32_t)new_hartid;
	struct sbi_domain *hd = sbi_domain_thishart_ptr();

	nếu (!sse_event_is_global(e))
		trả_về SBI_EDENIED;

	nếu (!sbi_domain_is_assigned_hart(hd, sbi_hartid_to_hartindex(hartid)))
		trả_về SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	nếu (hstate != SBI_HSM_STATE_STARTED)
		trả_về SBI_EINVAL;

	trả_về SBI_OK;
}

static số_nguyên sse_event_set_attr_check(struct sbi_sse_event *e, uint32_t attr_id,
				    unsigned long val)
{
	switch (attr_id) {
	case SBI_SSE_ATTR_CONFIG:
		nếu (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			trả_về SBI_EINVALID_STATE;

		nếu (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			trả_về SBI_EINVAL;

		trả_về SBI_OK;
	case SBI_SSE_ATTR_PRIO:
		nếu (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			trả_về SBI_EINVALID_STATE;

#nếu __riscv_xlen > 32
		nếu (val != (uint32_t)val)
			trả_về SBI_EINVAL;
#endif
		trả_về SBI_OK;
	case SBI_SSE_ATTR_PREFERRED_HART:
		nếu (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			trả_về SBI_EINVALID_STATE;

		trả_về sse_event_set_hart_id_check(e, val);
	case SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		nếu (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			trả_về SBI_EINVAL;
		__attribute__((__fallthrough__));
	case SBI_SSE_ATTR_INTERRUPTED_SEPC:
	case SBI_SSE_ATTR_INTERRUPTED_A6:
	case SBI_SSE_ATTR_INTERRUPTED_A7:
		nếu (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			trả_về SBI_EINVALID_STATE;

		nếu (current_hartid() != e->attrs.hartid)
			trả_về SBI_EINVAL;

		trả_về SBI_OK;
	default:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		trả_về SBI_EDENIED;
	}
}

static void sse_event_set_attr(struct sbi_sse_event *e, uint32_t attr_id,
			       unsigned long val)
{
	switch (attr_id) {
	case SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		break;
	case SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		break;
	case SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_to_hartindex(val);
		sse_event_invoke_cb(e, set_hartid_cb, val);
		break;

	case SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		break;
	case SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		break;
	case SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		break;
	case SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		break;
	}
}

static số_nguyên sse_event_register(struct sbi_sse_event *e,
			      unsigned long handler_entry_pc,
			      unsigned long handler_entry_arg)
{
	nếu (sse_event_state(e) != SBI_SSE_STATE_UNUSED)
		trả_về SBI_EINVALID_STATE;

	e->attrs.entry.pc = handler_entry_pc;
	e->attrs.entry.arg = handler_entry_arg;

	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, register_cb);

	trả_về 0;
}

static số_nguyên sse_event_unregister(struct sbi_sse_event *e)
{
	nếu (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		trả_về SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

	trả_về 0;
}

static unsigned long sse_interrupted_flags(unsigned long mstatus)
{
	unsigned long hstatus, flags = 0;

	nếu (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	nếu (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	nếu (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	nếu (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	nếu (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		nếu (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		nếu (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	trả_về flags;
}

static void sse_event_inject(struct sbi_sse_event *e,
			     struct sbi_trap_regs *regs)
{
	struct sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_set_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.status &= ~BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	nếu (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	nếu (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	nếu (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
		nếu (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		không_thì
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		nếu (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}
	csr_write(CSR_SEPC, regs->mepc);

	/* Setup entry context */
	regs->a6 = current_hartid();
	regs->a7 = e->attrs.entry.arg;
	regs->mepc = e->attrs.entry.pc;

	/*
	 * Return to S-mode with virtualization disabled, not expected landing
	 * pad, supervisor trap disabled.
	 */
	regs->mstatus &= ~(MSTATUS_MPP | MSTATUS_SIE | MSTATUS_SPELP);
	regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);
	regs->mstatus |= MSTATUS_SDT;

#nếu __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#elif __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#không_thì
#error "Unexpected __riscv_xlen"
#endif

}

static void sse_event_resume(struct sbi_sse_event *e,
			     struct sbi_trap_regs *regs)
{
	struct sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	nếu (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

	nếu (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
#nếu __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		nếu (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#elif __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		nếu (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#không_thì
#error "Unexpected __riscv_xlen"
#endif
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		nếu (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		nếu (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	nếu (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	nếu (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	nếu (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	nếu (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	nếu (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

static bool sse_event_is_ready(struct sbi_sse_event *e)
{
	nếu (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		trả_về false;
	}

	trả_về true;
}

static bool sse_event_check_inject(struct sbi_sse_event *e,
				   struct sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	nếu (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		trả_về true;
	}

	nếu (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		trả_về true;
	}

	trả_về false;
}

/* Return true if an event has been injected, false otherwise */
void sbi_sse_process_pending_events(struct sbi_trap_regs *regs)
{
	bool ret;
	struct sbi_sse_event *e;
	struct sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	nếu (state->masked)
		trả_về;

	spin_lock(&state->enabled_event_lock);

	sbi_list_for_each_entry(e, &state->enabled_event_list, node) {
		ret = sse_event_check_inject(e, regs);
		nếu (ret)
			goto out;
	}

out:
	spin_unlock(&state->enabled_event_lock);
}

static số_nguyên sse_event_set_pending(struct sbi_sse_event *e)
{
	nếu (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		trả_về SBI_EINVALID_STATE;

	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	trả_về SBI_OK;
}

static void sse_ipi_inject_process(struct sbi_scratch *scratch)
{
	struct sbi_sse_event *e;
	struct sse_ipi_inject_data evt;
	struct sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	trong_khi (!sbi_fifo_dequeue(sse_inject_fifo_r, &evt)) {
		nếu (sse_event_get(evt.event_id, &e))
			continue;

		sse_event_set_pending(e);
		sse_event_put(e);
	}
}

static struct sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

static số_nguyên sse_ipi_inject_send(unsigned long hartid, uint32_t event_id)
{
	số_nguyên ret;
	struct sbi_scratch *remote_scratch = NULL;
	struct sse_ipi_inject_data evt = {event_id};
	struct sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_to_scratch(hartid);
	nếu (!remote_scratch)
		trả_về SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_offset_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_enqueue(sse_inject_fifo_r, &evt, false);
	nếu (ret)
		trả_về SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, NULL);
	nếu (ret)
		trả_về SBI_EFAIL;

	trả_về SBI_OK;
}

static số_nguyên sse_inject_event(uint32_t event_id, unsigned long hartid)
{
	số_nguyên ret;
	struct sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	nếu (ret)
		trả_về ret;

	/* In case of global event, provided hart_id is ignored */
	nếu (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	nếu (hartid != current_hartid()) {
		sse_event_put(e);
		trả_về sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_set_pending(e);
	sse_event_put(e);
	nếu (ret)
		trả_về ret;

	trả_về SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
static số_nguyên sse_event_enable(struct sse_hart_state *shs,
			     struct sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	nếu (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		trả_về SBI_EINVALID_STATE;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_add_to_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	nếu (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  NULL);

	trả_về SBI_OK;
}

static số_nguyên sse_event_complete(struct sse_hart_state *shs,
			      struct sbi_sse_event *e,
			      struct sbi_trap_regs *regs,
			      struct sbi_ecall_return *out)
	MUST_HOLD(&shs->enabled_event_lock)
{
	nếu (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		trả_về SBI_EINVALID_STATE;

	nếu (e->attrs.hartid != current_hartid())
		trả_về SBI_EINVAL;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	nếu (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	out->skip_regs_update = true;

	trả_về SBI_OK;
}

số_nguyên sbi_sse_complete(struct sbi_trap_regs *regs, struct sbi_ecall_return *out)
{
	số_nguyên ret = SBI_OK;
	struct sbi_sse_event *tmp;
	struct sse_hart_state *state = sse_thishart_state_ptr();

	spin_lock(&state->enabled_event_lock);
	sbi_list_for_each_entry(tmp, &state->enabled_event_list, node) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		nếu (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, out);
			break;
		}
	}
	spin_unlock(&state->enabled_event_lock);

	trả_về ret;
}

số_nguyên sbi_sse_enable(uint32_t event_id)
{
	số_nguyên ret;
	struct sbi_sse_event *e;
	struct sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	nếu (ret)
		trả_về ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_lock);
	sse_event_put(e);

	trả_về ret;
}

số_nguyên sbi_sse_disable(uint32_t event_id)
{
	số_nguyên ret;
	struct sbi_sse_event *e;
	struct sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	nếu (ret)
		trả_về ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_lock);

	sse_event_put(e);

	trả_về ret;
}

số_nguyên sbi_sse_hart_mask(void)
{
	struct sse_hart_state *state = sse_thishart_state_ptr();

	nếu (!state)
		trả_về SBI_EFAIL;

	nếu (state->masked)
		trả_về SBI_EALREADY_STOPPED;

	state->masked = true;

	trả_về SBI_SUCCESS;
}

số_nguyên sbi_sse_hart_unmask(void)
{
	struct sse_hart_state *state = sse_thishart_state_ptr();

	nếu (!state)
		trả_về SBI_EFAIL;

	nếu (!state->masked)
		trả_về SBI_EALREADY_STARTED;

	state->masked = false;

	trả_về SBI_SUCCESS;
}

số_nguyên sbi_sse_inject_from_ecall(uint32_t event_id, unsigned long hartid,
			      struct sbi_ecall_return *out)
{
	nếu (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_to_hartindex(hartid)))
		trả_về SBI_EINVAL;

	trả_về sse_inject_event(event_id, hartid);
}

số_nguyên sbi_sse_inject_event(uint32_t event_id)
{
	trả_về sse_inject_event(event_id, current_hartid());
}

số_nguyên sbi_sse_add_event(uint32_t event_id, const struct sbi_sse_cb_ops *cb_ops)
{
	struct sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	nếu (info)
		trả_về SBI_EALREADY;

	nếu (cb_ops && cb_ops->set_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		trả_về SBI_EINVAL;

	info = sbi_zalloc(sizeof(*info));
	nếu (!info)
		trả_về SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ADD(info, supported_events);

	trả_về SBI_OK;
}

số_nguyên sbi_sse_attr_check(uint32_t base_attr_id, uint32_t attr_count,
		       unsigned long phys_lo, unsigned long phys_hi,
		       unsigned long access)
{
	const unsigned align = __riscv_xlen >> 3;
	uint64_t end_id = (uint64_t)base_attr_id + (attr_count - 1);

	nếu (attr_count == 0)
		trả_về SBI_ERR_INVALID_PARAM;

	nếu (end_id >= SBI_SSE_ATTR_MAX)
		trả_về SBI_EBAD_RANGE;

	nếu (phys_lo & (align - 1))
		trả_về SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	nếu (phys_hi)
		trả_về SBI_EINVALID_ADDR;

	nếu (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 sizeof(unsigned long) * attr_count,
					 PRV_S, access))
		trả_về SBI_EINVALID_ADDR;

	trả_về SBI_OK;
}

static void copy_attrs(unsigned long *out, const unsigned long *in,
		       unsigned số_nguyên long_count)
{
	số_nguyên i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	cho (i = 0; i < long_count; i++)
		out[i] = in[i];
}

số_nguyên sbi_sse_read_attrs(uint32_t event_id, uint32_t base_attr_id,
		       uint32_t attr_count, unsigned long output_phys_lo,
		       unsigned long output_phys_hi)
{
	số_nguyên ret;
	unsigned long *e_attrs;
	struct sbi_sse_event *e;
	unsigned long *attrs;

	ret = sbi_sse_attr_check(base_attr_id, attr_count, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	nếu (ret)
		trả_về ret;

	ret = sse_event_get(event_id, &e);
	nếu (ret)
		trả_về ret;

	sbi_hart_protection_temp_map_range(output_phys_lo, sizeof(unsigned long) * attr_count);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (unsigned long *)&e->attrs;
	attrs = (unsigned long *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[base_attr_id], attr_count);

	sbi_hart_protection_temp_unmap_range(output_phys_lo, sizeof(unsigned long) * attr_count);

	sse_event_put(e);

	trả_về SBI_OK;
}

static số_nguyên sse_write_attrs(struct sbi_sse_event *e, uint32_t base_attr_id,
			   uint32_t attr_count, unsigned long input_phys)
{
	số_nguyên ret = 0;
	unsigned long attr = 0, val;
	uint32_t id, end_id = base_attr_id + attr_count;
	unsigned long *attrs = (unsigned long *)input_phys;

	sbi_hart_protection_temp_map_range(input_phys, sizeof(unsigned long) * attr_count);

	cho (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		ret = sse_event_set_attr_check(e, id, val);
		nếu (ret)
			goto out;
	}

	attr = 0;
	cho (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		sse_event_set_attr(e, id, val);
	}

out:
	sbi_hart_protection_temp_unmap_range(input_phys, sizeof(unsigned long) * attr_count);

	trả_về ret;
}

số_nguyên sbi_sse_write_attrs(uint32_t event_id, uint32_t base_attr_id,
			uint32_t attr_count, unsigned long input_phys_lo,
			unsigned long input_phys_hi)
{
	số_nguyên ret = 0;
	struct sbi_sse_event *e;

	ret = sbi_sse_attr_check(base_attr_id, attr_count, input_phys_lo,
				 input_phys_hi, SBI_DOMAIN_READ);
	nếu (ret)
		trả_về ret;

	ret = sse_event_get(event_id, &e);
	nếu (ret)
		trả_về ret;

	ret = sse_write_attrs(e, base_attr_id, attr_count, input_phys_lo);
	sse_event_put(e);

	trả_về ret;
}

số_nguyên sbi_sse_register(uint32_t event_id, unsigned long handler_entry_pc,
		     unsigned long handler_entry_arg)
{
	số_nguyên ret;
	struct sbi_sse_event *e;

	nếu (handler_entry_pc & 0x1)
		trả_về SBI_EINVAL;

	nếu (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_entry_pc,
					 sizeof(unsigned long), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		trả_về SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	nếu (ret)
		trả_về ret;

	ret = sse_event_register(e, handler_entry_pc, handler_entry_arg);
	sse_event_put(e);

	trả_về ret;
}

số_nguyên sbi_sse_unregister(uint32_t event_id)
{
	số_nguyên ret;
	struct sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	nếu (ret)
		trả_về ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	trả_về ret;
}

static void sse_event_init(struct sbi_sse_event *e, struct sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_INJECT_OFFSET);
}

static void sse_event_count_init()
{
	struct sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		nếu (EVENT_IS_GLOBAL(info->event_id))
			global_event_count++;
		không_thì
			local_event_count++;
	}
}

static số_nguyên sse_global_init()
{
	struct sbi_sse_event *e;
	unsigned số_nguyên ev = 0;
	struct sse_event_info *info;

	global_events = sbi_zalloc(sizeof(*global_events) * global_event_count);
	nếu (!global_events)
		trả_về SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		nếu (!EVENT_IS_GLOBAL(info->event_id))
			continue;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_LOCK_INIT(global_events[ev].lock);

		ev++;
	}

	trả_về 0;
}

static void sse_local_init(struct sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	unsigned số_nguyên ev = 0;
	struct sse_event_info *info;

	SBI_INIT_LIST_HEAD(&shs->enabled_event_list);
	SPIN_LOCK_INIT(shs->enabled_event_lock);

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		nếu (EVENT_IS_GLOBAL(info->event_id))
			continue;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

số_nguyên sbi_sse_init(struct sbi_scratch *scratch, bool cold_boot)
{
	số_nguyên ret;
	void *sse_inject_mem;
	struct sse_hart_state *shs;
	struct sbi_fifo *sse_inject_q;

	nếu (cold_boot) {
		sse_event_count_init();

		ret = sse_global_init();
		nếu (ret)
			trả_về ret;

		shs_ptr_off = sbi_scratch_alloc_offset(sizeof(void *));
		nếu (!shs_ptr_off)
			trả_về SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_offset(sizeof(*sse_inject_q));
		nếu (!sse_inject_fifo_off) {
			sbi_scratch_free_offset(shs_ptr_off);
			trả_về SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_offset(
			(global_event_count + local_event_count) *
			sizeof(struct sse_ipi_inject_data));
		nếu (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_offset(sse_inject_fifo_off);
			sbi_scratch_free_offset(shs_ptr_off);
			trả_về SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		nếu (ret < 0) {
			sbi_scratch_free_offset(shs_ptr_off);
			trả_về ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	nếu (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(sizeof(*shs) + sizeof(struct sbi_sse_event) *
							local_event_count);
		nếu (!shs)
			trả_về SBI_ENOMEM;

		shs->local_events = (struct sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_set_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_count + local_event_count),
		      sizeof(struct sse_ipi_inject_data));

	trả_về 0;
}

void sbi_sse_exit(struct sbi_scratch *scratch)
{
	struct sbi_sse_event *e;
	struct sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		nếu (sse_event_get(info->event_id, &e))
			continue;

		nếu (e->attrs.hartid != current_hartid())
			goto skip;

		nếu (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

skip:
		sse_event_put(e);
	}
}
