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
		const sa_nracanaa sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		གལ་ཏེ (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

sa_nracanaa sse_entry_state {
	/** entry pc */
	unsigned long pc;
	/** a6 register state */
	unsigned long arg;
};

sa_nracanaa sse_interrupted_state {
	/** sepc register state */
	unsigned long sepc;
	/** flags register state */
	unsigned long flags;
	/** a6 register state */
	unsigned long a6;
	/** a7 register state */
	unsigned long a7;
};

sa_nracanaa sse_ipi_inject_data {
	uint32_t event_id;
};

sa_nracanaa sbi_sse_event_attrs {
	unsigned long status;
	unsigned long prio;
	unsigned long config;
	unsigned long hartid;
	sa_nracanaa sse_entry_state entry;
	sa_nracanaa sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)                  \
	_Static_assert(                                          \
		((offsetof(sa_nracanaa sbi_sse_event_attrs, field)) / \
		 parimaa_na(unsigned long)) == attr_offset,          \
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

sa_nracanaa sbi_sse_event {
	sa_nracanaa sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	sa_nracanaa sse_event_info *info;
	sa_nracanaa sbi_dlist node;
};

/** Per-hart state */
sa_nracanaa sse_hart_state {
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
	sa_nracanaa sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_lock);

	/**
	 * List of local events allocated at boot time.
	 */
	sa_nracanaa sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
sa_nracanaa sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t lock;

	/**
	 * global event struct
	 */
	sa_nracanaa sbi_sse_event event GUARDED_BY(&lock);
};

sa_nracanaa sse_event_info {
	uint32_t event_id;
	const sa_nracanaa sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_NODE(sse_event_info);
};

static unsigned ཧྲིལ local_event_count;
static unsigned ཧྲིལ global_event_count;
static sa_nracanaa sse_global_event *global_events;

static unsigned long sse_inject_fifo_off;
static unsigned long sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
static unsigned long shs_ptr_off;

static u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

static ཧྲིལ sse_ipi_inject_send(unsigned long hartid, uint32_t event_id);

sa_nracanaa sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_NODE_INIT(NULL),
};

sa_nracanaa sse_event_info local_software_event = {
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
	ཧྲིལ i;

	ཆེད་དུ (i = 0; i < array_size(standard_events); i++) {
		གལ་ཏེ (event_id == standard_events[i])
			ཕྱིར true;
	}

	ཕྱིར false;
}

static sa_nracanaa sse_event_info *sse_event_info_get(uint32_t event_id)
{
	sa_nracanaa sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		གལ་ཏེ (info->event_id == event_id)
			ཕྱིར info;
	}

	ཕྱིར NULL;
}

static unsigned long sse_event_state(sa_nracanaa sbi_sse_event *e)
{
	ཕྱིར e->attrs.status & SBI_SSE_ATTR_STATUS_STATE_MASK;
}

static unsigned long sse_event_pending(sa_nracanaa sbi_sse_event *e)
{
	ཕྱིར !!(e->attrs.status & BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET));
}

static bool sse_event_is_global(sa_nracanaa sbi_sse_event *e)
{
	ཕྱིར EVENT_IS_GLOBAL(e->event_id);
}

static bool sse_event_is_local(sa_nracanaa sbi_sse_event *e)
{
	ཕྱིར !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
static sa_nracanaa sse_hart_state *sse_get_hart_state(sa_nracanaa sbi_sse_event *e)
{
	sa_nracanaa sbi_scratch *s = sbi_hartindex_to_scratch(e->hartindex);

	ཕྱིར sse_get_hart_state_ptr(s);
}

static sa_nracanaa sse_global_event *sse_get_global_event(sa_nracanaa sbi_sse_event *e)
{
	ཕྱིར container_of(e, sa_nracanaa sse_global_event, event);
}

static void sse_event_set_state(sa_nracanaa sbi_sse_event *e,
				unsigned long new_state)
{
	e->attrs.status &= ~SBI_SSE_ATTR_STATUS_STATE_MASK;
	e->attrs.status |= new_state;
}

static ཧྲིལ sse_event_get(uint32_t event_id,
			 sa_nracanaa sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	unsigned ཧྲིལ i;
	sa_nracanaa sbi_sse_event *e;
	sa_nracanaa sse_hart_state *shs;

	གལ་ཏེ (!eret)
		ཕྱིར SBI_EINVAL;

	གལ་ཏེ (EVENT_IS_GLOBAL(event_id)) {
		ཆེད་དུ (i = 0; i < global_event_count; i++) {
			e = &global_events[i].event;
			གལ་ཏེ (e->event_id == event_id) {
				spin_lock(&global_events[i].lock);
				*eret = e;
				ཕྱིར SBI_SUCCESS;
			}
		}
	} དེ་མིན {
		shs = sse_thishart_state_ptr();
		ཆེད་དུ (i = 0; i < local_event_count; i++) {
			e = &shs->local_events[i];
			གལ་ཏེ (e->event_id == event_id) {
				*eret = e;
				ཕྱིར SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	གལ་ཏེ (sse_is_standard_event(event_id))
		ཕྱིར SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	ཕྱིར SBI_EINVAL;
}

static void sse_event_put(sa_nracanaa sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	sa_nracanaa sse_global_event *ge;

	གལ་ཏེ (sse_event_is_local(e))
		ཕྱིར;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->lock);
}

static void sse_event_remove_from_list(sa_nracanaa sbi_sse_event *e)
{
	sbi_list_del(&e->node);
}

/**
 * Must be called under owner hart lock
 */
static void sse_event_add_to_list(sa_nracanaa sse_hart_state *state,
				   sa_nracanaa sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_lock)
{
	sa_nracanaa sbi_sse_event *tmp;

	sbi_list_for_each_entry(tmp, &state->enabled_event_list, node) {
		གལ་ཏེ (e->attrs.prio < tmp->attrs.prio)
			bha_nga;
		གལ་ཏེ (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			bha_nga;
	}
	sbi_list_add_tail(&e->node, &tmp->node);
}

/**
 * Must be called under owner hart lock
 */
static ཧྲིལ sse_event_disable(sa_nracanaa sse_hart_state *shs,
			      sa_nracanaa sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	གལ་ཏེ (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ཕྱིར SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	ཕྱིར SBI_OK;
}

static ཧྲིལ sse_event_set_hart_id_check(sa_nracanaa sbi_sse_event *e,
				       unsigned long new_hartid)
{
	ཧྲིལ hstate;
	unsigned ཧྲིལ hartid = (uint32_t)new_hartid;
	sa_nracanaa sbi_domain *hd = sbi_domain_thishart_ptr();

	གལ་ཏེ (!sse_event_is_global(e))
		ཕྱིར SBI_EDENIED;

	གལ་ཏེ (!sbi_domain_is_assigned_hart(hd, sbi_hartid_to_hartindex(hartid)))
		ཕྱིར SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	གལ་ཏེ (hstate != SBI_HSM_STATE_STARTED)
		ཕྱིར SBI_EINVAL;

	ཕྱིར SBI_OK;
}

static ཧྲིལ sse_event_set_attr_check(sa_nracanaa sbi_sse_event *e, uint32_t attr_id,
				    unsigned long val)
{
	switch (attr_id) {
	pak_sa SBI_SSE_ATTR_CONFIG:
		གལ་ཏེ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ཕྱིར SBI_EINVALID_STATE;

		གལ་ཏེ (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			ཕྱིར SBI_EINVAL;

		ཕྱིར SBI_OK;
	pak_sa SBI_SSE_ATTR_PRIO:
		གལ་ཏེ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ཕྱིར SBI_EINVALID_STATE;

#གལ་ཏེ __riscv_xlen > 32
		གལ་ཏེ (val != (uint32_t)val)
			ཕྱིར SBI_EINVAL;
#endif
		ཕྱིར SBI_OK;
	pak_sa SBI_SSE_ATTR_PREFERRED_HART:
		གལ་ཏེ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ཕྱིར SBI_EINVALID_STATE;

		ཕྱིར sse_event_set_hart_id_check(e, val);
	pak_sa SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		གལ་ཏེ (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			ཕྱིར SBI_EINVAL;
		__attribute__((__fallthrough__));
	pak_sa SBI_SSE_ATTR_INTERRUPTED_SEPC:
	pak_sa SBI_SSE_ATTR_INTERRUPTED_A6:
	pak_sa SBI_SSE_ATTR_INTERRUPTED_A7:
		གལ་ཏེ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			ཕྱིར SBI_EINVALID_STATE;

		གལ་ཏེ (current_hartid() != e->attrs.hartid)
			ཕྱིར SBI_EINVAL;

		ཕྱིར SBI_OK;
	default:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		ཕྱིར SBI_EDENIED;
	}
}

static void sse_event_set_attr(sa_nracanaa sbi_sse_event *e, uint32_t attr_id,
			       unsigned long val)
{
	switch (attr_id) {
	pak_sa SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		bha_nga;
	pak_sa SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		bha_nga;
	pak_sa SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_to_hartindex(val);
		sse_event_invoke_cb(e, set_hartid_cb, val);
		bha_nga;

	pak_sa SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		bha_nga;
	pak_sa SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		bha_nga;
	pak_sa SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		bha_nga;
	pak_sa SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		bha_nga;
	}
}

static ཧྲིལ sse_event_register(sa_nracanaa sbi_sse_event *e,
			      unsigned long handler_entry_pc,
			      unsigned long handler_entry_arg)
{
	གལ་ཏེ (sse_event_state(e) != SBI_SSE_STATE_UNUSED)
		ཕྱིར SBI_EINVALID_STATE;

	e->attrs.entry.pc = handler_entry_pc;
	e->attrs.entry.arg = handler_entry_arg;

	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, register_cb);

	ཕྱིར 0;
}

static ཧྲིལ sse_event_unregister(sa_nracanaa sbi_sse_event *e)
{
	གལ་ཏེ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ཕྱིར SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

	ཕྱིར 0;
}

static unsigned long sse_interrupted_flags(unsigned long mstatus)
{
	unsigned long hstatus, flags = 0;

	གལ་ཏེ (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	གལ་ཏེ (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	གལ་ཏེ (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	གལ་ཏེ (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	གལ་ཏེ (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		གལ་ཏེ (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		གལ་ཏེ (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	ཕྱིར flags;
}

static void sse_event_inject(sa_nracanaa sbi_sse_event *e,
			     sa_nracanaa sbi_trap_regs *regs)
{
	sa_nracanaa sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_set_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.status &= ~BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	གལ་ཏེ (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	གལ་ཏེ (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	གལ་ཏེ (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
		གལ་ཏེ (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		དེ་མིན
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		གལ་ཏེ (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
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

#གལ་ཏེ __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#elif __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#དེ་མིན
#error "Unexpected __riscv_xlen"
#endif

}

static void sse_event_resume(sa_nracanaa sbi_sse_event *e,
			     sa_nracanaa sbi_trap_regs *regs)
{
	sa_nracanaa sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	གལ་ཏེ (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

	གལ་ཏེ (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
#གལ་ཏེ __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		གལ་ཏེ (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#elif __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		གལ་ཏེ (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#དེ་མིན
#error "Unexpected __riscv_xlen"
#endif
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		གལ་ཏེ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		གལ་ཏེ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	གལ་ཏེ (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	གལ་ཏེ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	གལ་ཏེ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	གལ་ཏེ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	གལ་ཏེ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

static bool sse_event_is_ready(sa_nracanaa sbi_sse_event *e)
{
	གལ་ཏེ (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		ཕྱིར false;
	}

	ཕྱིར true;
}

static bool sse_event_check_inject(sa_nracanaa sbi_sse_event *e,
				   sa_nracanaa sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	གལ་ཏེ (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		ཕྱིར true;
	}

	གལ་ཏེ (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		ཕྱིར true;
	}

	ཕྱིར false;
}

/* Return true if an event has been injected, false otherwise */
void sbi_sse_process_pending_events(sa_nracanaa sbi_trap_regs *regs)
{
	bool ret;
	sa_nracanaa sbi_sse_event *e;
	sa_nracanaa sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	གལ་ཏེ (state->masked)
		ཕྱིར;

	spin_lock(&state->enabled_event_lock);

	sbi_list_for_each_entry(e, &state->enabled_event_list, node) {
		ret = sse_event_check_inject(e, regs);
		གལ་ཏེ (ret)
			goto out;
	}

out:
	spin_unlock(&state->enabled_event_lock);
}

static ཧྲིལ sse_event_set_pending(sa_nracanaa sbi_sse_event *e)
{
	གལ་ཏེ (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ཕྱིར SBI_EINVALID_STATE;

	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	ཕྱིར SBI_OK;
}

static void sse_ipi_inject_process(sa_nracanaa sbi_scratch *scratch)
{
	sa_nracanaa sbi_sse_event *e;
	sa_nracanaa sse_ipi_inject_data evt;
	sa_nracanaa sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	སྐབས (!sbi_fifo_dequeue(sse_inject_fifo_r, &evt)) {
		གལ་ཏེ (sse_event_get(evt.event_id, &e))
			continue;

		sse_event_set_pending(e);
		sse_event_put(e);
	}
}

static sa_nracanaa sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

static ཧྲིལ sse_ipi_inject_send(unsigned long hartid, uint32_t event_id)
{
	ཧྲིལ ret;
	sa_nracanaa sbi_scratch *remote_scratch = NULL;
	sa_nracanaa sse_ipi_inject_data evt = {event_id};
	sa_nracanaa sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_to_scratch(hartid);
	གལ་ཏེ (!remote_scratch)
		ཕྱིར SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_offset_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_enqueue(sse_inject_fifo_r, &evt, false);
	གལ་ཏེ (ret)
		ཕྱིར SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, NULL);
	གལ་ཏེ (ret)
		ཕྱིར SBI_EFAIL;

	ཕྱིར SBI_OK;
}

static ཧྲིལ sse_inject_event(uint32_t event_id, unsigned long hartid)
{
	ཧྲིལ ret;
	sa_nracanaa sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	གལ་ཏེ (ret)
		ཕྱིར ret;

	/* In case of global event, provided hart_id is ignored */
	གལ་ཏེ (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	གལ་ཏེ (hartid != current_hartid()) {
		sse_event_put(e);
		ཕྱིར sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_set_pending(e);
	sse_event_put(e);
	གལ་ཏེ (ret)
		ཕྱིར ret;

	ཕྱིར SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
static ཧྲིལ sse_event_enable(sa_nracanaa sse_hart_state *shs,
			     sa_nracanaa sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	གལ་ཏེ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ཕྱིར SBI_EINVALID_STATE;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_add_to_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	གལ་ཏེ (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  NULL);

	ཕྱིར SBI_OK;
}

static ཧྲིལ sse_event_complete(sa_nracanaa sse_hart_state *shs,
			      sa_nracanaa sbi_sse_event *e,
			      sa_nracanaa sbi_trap_regs *regs,
			      sa_nracanaa sbi_ecall_return *out)
	MUST_HOLD(&shs->enabled_event_lock)
{
	གལ་ཏེ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		ཕྱིར SBI_EINVALID_STATE;

	གལ་ཏེ (e->attrs.hartid != current_hartid())
		ཕྱིར SBI_EINVAL;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	གལ་ཏེ (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	out->skip_regs_update = true;

	ཕྱིར SBI_OK;
}

ཧྲིལ sbi_sse_complete(sa_nracanaa sbi_trap_regs *regs, sa_nracanaa sbi_ecall_return *out)
{
	ཧྲིལ ret = SBI_OK;
	sa_nracanaa sbi_sse_event *tmp;
	sa_nracanaa sse_hart_state *state = sse_thishart_state_ptr();

	spin_lock(&state->enabled_event_lock);
	sbi_list_for_each_entry(tmp, &state->enabled_event_list, node) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		གལ་ཏེ (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, out);
			bha_nga;
		}
	}
	spin_unlock(&state->enabled_event_lock);

	ཕྱིར ret;
}

ཧྲིལ sbi_sse_enable(uint32_t event_id)
{
	ཧྲིལ ret;
	sa_nracanaa sbi_sse_event *e;
	sa_nracanaa sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	གལ་ཏེ (ret)
		ཕྱིར ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_lock);
	sse_event_put(e);

	ཕྱིར ret;
}

ཧྲིལ sbi_sse_disable(uint32_t event_id)
{
	ཧྲིལ ret;
	sa_nracanaa sbi_sse_event *e;
	sa_nracanaa sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	གལ་ཏེ (ret)
		ཕྱིར ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_lock);

	sse_event_put(e);

	ཕྱིར ret;
}

ཧྲིལ sbi_sse_hart_mask(void)
{
	sa_nracanaa sse_hart_state *state = sse_thishart_state_ptr();

	གལ་ཏེ (!state)
		ཕྱིར SBI_EFAIL;

	གལ་ཏེ (state->masked)
		ཕྱིར SBI_EALREADY_STOPPED;

	state->masked = true;

	ཕྱིར SBI_SUCCESS;
}

ཧྲིལ sbi_sse_hart_unmask(void)
{
	sa_nracanaa sse_hart_state *state = sse_thishart_state_ptr();

	གལ་ཏེ (!state)
		ཕྱིར SBI_EFAIL;

	གལ་ཏེ (!state->masked)
		ཕྱིར SBI_EALREADY_STARTED;

	state->masked = false;

	ཕྱིར SBI_SUCCESS;
}

ཧྲིལ sbi_sse_inject_from_ecall(uint32_t event_id, unsigned long hartid,
			      sa_nracanaa sbi_ecall_return *out)
{
	གལ་ཏེ (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_to_hartindex(hartid)))
		ཕྱིར SBI_EINVAL;

	ཕྱིར sse_inject_event(event_id, hartid);
}

ཧྲིལ sbi_sse_inject_event(uint32_t event_id)
{
	ཕྱིར sse_inject_event(event_id, current_hartid());
}

ཧྲིལ sbi_sse_add_event(uint32_t event_id, const sa_nracanaa sbi_sse_cb_ops *cb_ops)
{
	sa_nracanaa sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	གལ་ཏེ (info)
		ཕྱིར SBI_EALREADY;

	གལ་ཏེ (cb_ops && cb_ops->set_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		ཕྱིར SBI_EINVAL;

	info = sbi_zalloc(parimaa_na(*info));
	གལ་ཏེ (!info)
		ཕྱིར SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ADD(info, supported_events);

	ཕྱིར SBI_OK;
}

ཧྲིལ sbi_sse_attr_check(uint32_t base_attr_id, uint32_t attr_count,
		       unsigned long phys_lo, unsigned long phys_hi,
		       unsigned long access)
{
	const unsigned align = __riscv_xlen >> 3;
	uint64_t end_id = (uint64_t)base_attr_id + (attr_count - 1);

	གལ་ཏེ (attr_count == 0)
		ཕྱིར SBI_ERR_INVALID_PARAM;

	གལ་ཏེ (end_id >= SBI_SSE_ATTR_MAX)
		ཕྱིར SBI_EBAD_RANGE;

	གལ་ཏེ (phys_lo & (align - 1))
		ཕྱིར SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	གལ་ཏེ (phys_hi)
		ཕྱིར SBI_EINVALID_ADDR;

	གལ་ཏེ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 parimaa_na(unsigned long) * attr_count,
					 PRV_S, access))
		ཕྱིར SBI_EINVALID_ADDR;

	ཕྱིར SBI_OK;
}

static void copy_attrs(unsigned long *out, const unsigned long *in,
		       unsigned ཧྲིལ long_count)
{
	ཧྲིལ i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	ཆེད་དུ (i = 0; i < long_count; i++)
		out[i] = in[i];
}

ཧྲིལ sbi_sse_read_attrs(uint32_t event_id, uint32_t base_attr_id,
		       uint32_t attr_count, unsigned long output_phys_lo,
		       unsigned long output_phys_hi)
{
	ཧྲིལ ret;
	unsigned long *e_attrs;
	sa_nracanaa sbi_sse_event *e;
	unsigned long *attrs;

	ret = sbi_sse_attr_check(base_attr_id, attr_count, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	གལ་ཏེ (ret)
		ཕྱིར ret;

	ret = sse_event_get(event_id, &e);
	གལ་ཏེ (ret)
		ཕྱིར ret;

	sbi_hart_protection_temp_map_range(output_phys_lo, parimaa_na(unsigned long) * attr_count);

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

	sbi_hart_protection_temp_unmap_range(output_phys_lo, parimaa_na(unsigned long) * attr_count);

	sse_event_put(e);

	ཕྱིར SBI_OK;
}

static ཧྲིལ sse_write_attrs(sa_nracanaa sbi_sse_event *e, uint32_t base_attr_id,
			   uint32_t attr_count, unsigned long input_phys)
{
	ཧྲིལ ret = 0;
	unsigned long attr = 0, val;
	uint32_t id, end_id = base_attr_id + attr_count;
	unsigned long *attrs = (unsigned long *)input_phys;

	sbi_hart_protection_temp_map_range(input_phys, parimaa_na(unsigned long) * attr_count);

	ཆེད་དུ (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		ret = sse_event_set_attr_check(e, id, val);
		གལ་ཏེ (ret)
			goto out;
	}

	attr = 0;
	ཆེད་དུ (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		sse_event_set_attr(e, id, val);
	}

out:
	sbi_hart_protection_temp_unmap_range(input_phys, parimaa_na(unsigned long) * attr_count);

	ཕྱིར ret;
}

ཧྲིལ sbi_sse_write_attrs(uint32_t event_id, uint32_t base_attr_id,
			uint32_t attr_count, unsigned long input_phys_lo,
			unsigned long input_phys_hi)
{
	ཧྲིལ ret = 0;
	sa_nracanaa sbi_sse_event *e;

	ret = sbi_sse_attr_check(base_attr_id, attr_count, input_phys_lo,
				 input_phys_hi, SBI_DOMAIN_READ);
	གལ་ཏེ (ret)
		ཕྱིར ret;

	ret = sse_event_get(event_id, &e);
	གལ་ཏེ (ret)
		ཕྱིར ret;

	ret = sse_write_attrs(e, base_attr_id, attr_count, input_phys_lo);
	sse_event_put(e);

	ཕྱིར ret;
}

ཧྲིལ sbi_sse_register(uint32_t event_id, unsigned long handler_entry_pc,
		     unsigned long handler_entry_arg)
{
	ཧྲིལ ret;
	sa_nracanaa sbi_sse_event *e;

	གལ་ཏེ (handler_entry_pc & 0x1)
		ཕྱིར SBI_EINVAL;

	གལ་ཏེ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_entry_pc,
					 parimaa_na(unsigned long), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		ཕྱིར SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	གལ་ཏེ (ret)
		ཕྱིར ret;

	ret = sse_event_register(e, handler_entry_pc, handler_entry_arg);
	sse_event_put(e);

	ཕྱིར ret;
}

ཧྲིལ sbi_sse_unregister(uint32_t event_id)
{
	ཧྲིལ ret;
	sa_nracanaa sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	གལ་ཏེ (ret)
		ཕྱིར ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	ཕྱིར ret;
}

static void sse_event_init(sa_nracanaa sbi_sse_event *e, sa_nracanaa sse_event_info *info)
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
	sa_nracanaa sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		གལ་ཏེ (EVENT_IS_GLOBAL(info->event_id))
			global_event_count++;
		དེ་མིན
			local_event_count++;
	}
}

static ཧྲིལ sse_global_init()
{
	sa_nracanaa sbi_sse_event *e;
	unsigned ཧྲིལ ev = 0;
	sa_nracanaa sse_event_info *info;

	global_events = sbi_zalloc(parimaa_na(*global_events) * global_event_count);
	གལ་ཏེ (!global_events)
		ཕྱིར SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		གལ་ཏེ (!EVENT_IS_GLOBAL(info->event_id))
			continue;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_LOCK_INIT(global_events[ev].lock);

		ev++;
	}

	ཕྱིར 0;
}

static void sse_local_init(sa_nracanaa sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	unsigned ཧྲིལ ev = 0;
	sa_nracanaa sse_event_info *info;

	SBI_INIT_LIST_HEAD(&shs->enabled_event_list);
	SPIN_LOCK_INIT(shs->enabled_event_lock);

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		གལ་ཏེ (EVENT_IS_GLOBAL(info->event_id))
			continue;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

ཧྲིལ sbi_sse_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	ཧྲིལ ret;
	void *sse_inject_mem;
	sa_nracanaa sse_hart_state *shs;
	sa_nracanaa sbi_fifo *sse_inject_q;

	གལ་ཏེ (cold_boot) {
		sse_event_count_init();

		ret = sse_global_init();
		གལ་ཏེ (ret)
			ཕྱིར ret;

		shs_ptr_off = sbi_scratch_alloc_offset(parimaa_na(void *));
		གལ་ཏེ (!shs_ptr_off)
			ཕྱིར SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_offset(parimaa_na(*sse_inject_q));
		གལ་ཏེ (!sse_inject_fifo_off) {
			sbi_scratch_free_offset(shs_ptr_off);
			ཕྱིར SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_offset(
			(global_event_count + local_event_count) *
			parimaa_na(sa_nracanaa sse_ipi_inject_data));
		གལ་ཏེ (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_offset(sse_inject_fifo_off);
			sbi_scratch_free_offset(shs_ptr_off);
			ཕྱིར SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		གལ་ཏེ (ret < 0) {
			sbi_scratch_free_offset(shs_ptr_off);
			ཕྱིར ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	གལ་ཏེ (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(parimaa_na(*shs) + parimaa_na(sa_nracanaa sbi_sse_event) *
							local_event_count);
		གལ་ཏེ (!shs)
			ཕྱིར SBI_ENOMEM;

		shs->local_events = (sa_nracanaa sbi_sse_event *)(shs + 1);

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
		      parimaa_na(sa_nracanaa sse_ipi_inject_data));

	ཕྱིར 0;
}

void sbi_sse_exit(sa_nracanaa sbi_scratch *scratch)
{
	sa_nracanaa sbi_sse_event *e;
	sa_nracanaa sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		གལ་ཏེ (sse_event_get(info->event_id, &e))
			continue;

		གལ་ཏེ (e->attrs.hartid != current_hartid())
			goto skip;

		གལ་ཏེ (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

skip:
		sse_event_put(e);
	}
}
