/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Rivos Systems Inc.
 *
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_barrier.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_encoding.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_locks.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_fifo.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart_protection.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hsm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ipi.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_list.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_pmu.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_sse.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_slist.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>

#define sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), ଶୂନ୍ୟ *, shs_ptr_off)

#define sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define sse_set_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), ଶୂନ୍ୟ *, shs_ptr_off, (__sse_state))

#define EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_BIT)

#define sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		ଅଚଳ ସଂରଚନା sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		ଯଦି (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

ସଂରଚନା sse_entry_state {
	/** entry pc */
	unsigned long pc;
	/** a6 register state */
	unsigned long arg;
};

ସଂରଚନା sse_interrupted_state {
	/** sepc register state */
	unsigned long sepc;
	/** flags register state */
	unsigned long flags;
	/** a6 register state */
	unsigned long a6;
	/** a7 register state */
	unsigned long a7;
};

ସଂରଚନା sse_ipi_inject_data {
	uint32_t event_id;
};

ସଂରଚନା sbi_sse_event_attrs {
	unsigned long status;
	unsigned long prio;
	unsigned long config;
	unsigned long hartid;
	ସଂରଚନା sse_entry_state entry;
	ସଂରଚନା sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)                  \
	_Static_assert(                                          \
		((offsetof(ସଂରଚନା sbi_sse_event_attrs, field)) / \
		 ଆକାର(unsigned long)) == attr_offset,          \
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

ସଂରଚନା sbi_sse_event {
	ସଂରଚନା sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	ସଂରଚନା sse_event_info *info;
	ସଂରଚନା sbi_dlist ଗଣ୍ଠି;
};

/** Per-hart state */
ସଂରଚନା sse_hart_state {
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
	ସଂରଚନା sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_lock);

	/**
	 * List of local events allocated at boot time.
	 */
	ସଂରଚନା sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
ସଂରଚନା sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t lock;

	/**
	 * global event struct
	 */
	ସଂରଚନା sbi_sse_event event GUARDED_BY(&lock);
};

ସଂରଚନା sse_event_info {
	uint32_t event_id;
	ଅଚଳ ସଂରଚନା sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_ଗଣ୍ଠି(sse_event_info);
};

ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା local_event_ଗଣନା;
ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା global_event_ଗଣନା;
ସ୍ଥିର ସଂରଚନା sse_global_event *global_events;

ସ୍ଥିର unsigned long sse_inject_fifo_off;
ସ୍ଥିର unsigned long sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
ସ୍ଥିର unsigned long shs_ptr_off;

ସ୍ଥିର u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_ipi_inject_send(unsigned long hartid, uint32_t event_id);

ସଂରଚନା sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_ଗଣ୍ଠି_INIT(ଖାଲି),
};

ସଂରଚନା sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_ଗଣ୍ଠି_INIT(&global_software_event),
};

ସ୍ଥିର SBI_SLIST_HEAD(supported_events, sse_event_info) =
				SBI_SLIST_HEAD_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
ସ୍ଥିର ଅଚଳ uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_ଉପର_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_ଉପର_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW,
	SBI_SSE_EVENT_LOCAL_ତଳ_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_ତଳ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

ସ୍ଥିର bool sse_is_standard_event(uint32_t event_id)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପାଇଁ (i = 0; i < array_size(standard_events); i++) {
		ଯଦି (event_id == standard_events[i])
			ଫେରନ୍ତୁ true;
	}

	ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ସଂରଚନା sse_event_info *sse_event_info_get(uint32_t event_id)
{
	ସଂରଚନା sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ଯଦି (info->event_id == event_id)
			ଫେରନ୍ତୁ info;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର unsigned long sse_event_state(ସଂରଚନା sbi_sse_event *e)
{
	ଫେରନ୍ତୁ e->attrs.status & SBI_SSE_ATTR_STATUS_STATE_MASK;
}

ସ୍ଥିର unsigned long sse_event_pending(ସଂରଚନା sbi_sse_event *e)
{
	ଫେରନ୍ତୁ !!(e->attrs.status & BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET));
}

ସ୍ଥିର bool sse_event_is_global(ସଂରଚନା sbi_sse_event *e)
{
	ଫେରନ୍ତୁ EVENT_IS_GLOBAL(e->event_id);
}

ସ୍ଥିର bool sse_event_is_local(ସଂରଚନା sbi_sse_event *e)
{
	ଫେରନ୍ତୁ !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
ସ୍ଥିର ସଂରଚନା sse_hart_state *sse_get_hart_state(ସଂରଚନା sbi_sse_event *e)
{
	ସଂରଚନା sbi_scratch *s = sbi_hartindex_to_scratch(e->hartindex);

	ଫେରନ୍ତୁ sse_get_hart_state_ptr(s);
}

ସ୍ଥିର ସଂରଚନା sse_global_event *sse_get_global_event(ସଂରଚନା sbi_sse_event *e)
{
	ଫେରନ୍ତୁ container_of(e, ସଂରଚନା sse_global_event, event);
}

ସ୍ଥିର ଶୂନ୍ୟ sse_event_set_state(ସଂରଚନା sbi_sse_event *e,
				unsigned long new_state)
{
	e->attrs.status &= ~SBI_SSE_ATTR_STATUS_STATE_MASK;
	e->attrs.status |= new_state;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_event_get(uint32_t event_id,
			 ସଂରଚନା sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ସଂରଚନା sbi_sse_event *e;
	ସଂରଚନା sse_hart_state *shs;

	ଯଦି (!eret)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (EVENT_IS_GLOBAL(event_id)) {
		ପାଇଁ (i = 0; i < global_event_ଗଣନା; i++) {
			e = &global_events[i].event;
			ଯଦି (e->event_id == event_id) {
				spin_lock(&global_events[i].lock);
				*eret = e;
				ଫେରନ୍ତୁ SBI_SUCCESS;
			}
		}
	} ଅନ୍ୟଥା {
		shs = sse_thishart_state_ptr();
		ପାଇଁ (i = 0; i < local_event_ଗଣନା; i++) {
			e = &shs->local_events[i];
			ଯଦି (e->event_id == event_id) {
				*eret = e;
				ଫେରନ୍ତୁ SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	ଯଦି (sse_is_standard_event(event_id))
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	ଫେରନ୍ତୁ SBI_EINVAL;
}

ସ୍ଥିର ଶୂନ୍ୟ sse_event_put(ସଂରଚନା sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	ସଂରଚନା sse_global_event *ge;

	ଯଦି (sse_event_is_local(e))
		ଫେରନ୍ତୁ;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->lock);
}

ସ୍ଥିର ଶୂନ୍ୟ sse_event_remove_from_list(ସଂରଚନା sbi_sse_event *e)
{
	sbi_list_del(&e->ଗଣ୍ଠି);
}

/**
 * Must be called under owner hart lock
 */
ସ୍ଥିର ଶୂନ୍ୟ sse_event_add_to_list(ସଂରଚନା sse_hart_state *state,
				   ସଂରଚନା sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_lock)
{
	ସଂରଚନା sbi_sse_event *tmp;

	sbi_list_for_each_entry(tmp, &state->enabled_event_list, ଗଣ୍ଠି) {
		ଯଦି (e->attrs.prio < tmp->attrs.prio)
			ଭାଙ୍ଗ;
		ଯଦି (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			ଭାଙ୍ଗ;
	}
	sbi_list_add_tail(&e->ଗଣ୍ଠି, &tmp->ଗଣ୍ଠି);
}

/**
 * Must be called under owner hart lock
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_event_disable(ସଂରଚନା sse_hart_state *shs,
			      ସଂରଚନା sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	ଯଦି (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ଫେରନ୍ତୁ SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_event_set_hart_id_check(ସଂରଚନା sbi_sse_event *e,
				       unsigned long new_hartid)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା hstate;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hartid = (uint32_t)new_hartid;
	ସଂରଚନା sbi_domain *hd = sbi_domain_thishart_ptr();

	ଯଦି (!sse_event_is_global(e))
		ଫେରନ୍ତୁ SBI_EDENIED;

	ଯଦି (!sbi_domain_is_assigned_hart(hd, sbi_hartid_to_hartindex(hartid)))
		ଫେରନ୍ତୁ SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	ଯଦି (hstate != SBI_HSM_STATE_STARTED)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_event_set_attr_check(ସଂରଚନା sbi_sse_event *e, uint32_t attr_id,
				    unsigned long val)
{
	ବାଛ (attr_id) {
	ସ୍ଥିତି SBI_SSE_ATTR_CONFIG:
		ଯଦି (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ଫେରନ୍ତୁ SBI_EINVALID_STATE;

		ଯଦି (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			ଫେରନ୍ତୁ SBI_EINVAL;

		ଫେରନ୍ତୁ SBI_OK;
	ସ୍ଥିତି SBI_SSE_ATTR_PRIO:
		ଯଦି (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ଫେରନ୍ତୁ SBI_EINVALID_STATE;

#ଯଦି __riscv_xlen > 32
		ଯଦି (val != (uint32_t)val)
			ଫେରନ୍ତୁ SBI_EINVAL;
#endif
		ଫେରନ୍ତୁ SBI_OK;
	ସ୍ଥିତି SBI_SSE_ATTR_PREFERRED_HART:
		ଯଦି (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ଫେରନ୍ତୁ SBI_EINVALID_STATE;

		ଫେରନ୍ତୁ sse_event_set_hart_id_check(e, val);
	ସ୍ଥିତି SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		ଯଦି (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			ଫେରନ୍ତୁ SBI_EINVAL;
		__attribute__((__fallthrough__));
	ସ୍ଥିତି SBI_SSE_ATTR_INTERRUPTED_SEPC:
	ସ୍ଥିତି SBI_SSE_ATTR_INTERRUPTED_A6:
	ସ୍ଥିତି SBI_SSE_ATTR_INTERRUPTED_A7:
		ଯଦି (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			ଫେରନ୍ତୁ SBI_EINVALID_STATE;

		ଯଦି (current_hartid() != e->attrs.hartid)
			ଫେରନ୍ତୁ SBI_EINVAL;

		ଫେରନ୍ତୁ SBI_OK;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		ଫେରନ୍ତୁ SBI_EDENIED;
	}
}

ସ୍ଥିର ଶୂନ୍ୟ sse_event_set_attr(ସଂରଚନା sbi_sse_event *e, uint32_t attr_id,
			       unsigned long val)
{
	ବାଛ (attr_id) {
	ସ୍ଥିତି SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_to_hartindex(val);
		sse_event_invoke_cb(e, set_hartid_cb, val);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		ଭାଙ୍ଗ;
	}
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_event_register(ସଂରଚନା sbi_sse_event *e,
			      unsigned long handler_entry_pc,
			      unsigned long handler_entry_arg)
{
	ଯଦି (sse_event_state(e) != SBI_SSE_STATE_UNUSED)
		ଫେରନ୍ତୁ SBI_EINVALID_STATE;

	e->attrs.entry.pc = handler_entry_pc;
	e->attrs.entry.arg = handler_entry_arg;

	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, register_cb);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_event_unregister(ସଂରଚନା sbi_sse_event *e)
{
	ଯଦି (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ଫେରନ୍ତୁ SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର unsigned long sse_interrupted_flags(unsigned long mstatus)
{
	unsigned long hstatus, flags = 0;

	ଯଦି (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	ଯଦି (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	ଯଦି (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	ଯଦି (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	ଯଦି (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		ଯଦି (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		ଯଦି (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	ଫେରନ୍ତୁ flags;
}

ସ୍ଥିର ଶୂନ୍ୟ sse_event_inject(ସଂରଚନା sbi_sse_event *e,
			     ସଂରଚନା sbi_trap_regs *regs)
{
	ସଂରଚନା sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_set_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.status &= ~BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	ଯଦି (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	ଯଦି (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	ଯଦି (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
		ଯଦି (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		ଅନ୍ୟଥା
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		ଯଦି (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
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

#ଯଦି __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#ନଚେତଯଦି __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#ଅନ୍ୟଥା
#error "Unexpected __riscv_xlen"
#endif

}

ସ୍ଥିର ଶୂନ୍ୟ sse_event_resume(ସଂରଚନା sbi_sse_event *e,
			     ସଂରଚନା sbi_trap_regs *regs)
{
	ସଂରଚନା sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	ଯଦି (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

	ଯଦି (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
#ଯଦି __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		ଯଦି (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#ନଚେତଯଦି __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		ଯଦି (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#ଅନ୍ୟଥା
#error "Unexpected __riscv_xlen"
#endif
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		ଯଦି (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		ଯଦି (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	ଯଦି (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	ଯଦି (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	ଯଦି (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	ଯଦି (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	ଯଦି (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

ସ୍ଥିର bool sse_event_is_ready(ସଂରଚନା sbi_sse_event *e)
{
	ଯଦି (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		ଫେରନ୍ତୁ false;
	}

	ଫେରନ୍ତୁ true;
}

ସ୍ଥିର bool sse_event_check_inject(ସଂରଚନା sbi_sse_event *e,
				   ସଂରଚନା sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	ଯଦି (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		ଫେରନ୍ତୁ true;
	}

	ଯଦି (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		ଫେରନ୍ତୁ true;
	}

	ଫେରନ୍ତୁ false;
}

/* Return true if an event has been injected, false otherwise */
ଶୂନ୍ୟ sbi_sse_process_pending_events(ସଂରଚନା sbi_trap_regs *regs)
{
	bool ret;
	ସଂରଚନା sbi_sse_event *e;
	ସଂରଚନା sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	ଯଦି (state->masked)
		ଫେରନ୍ତୁ;

	spin_lock(&state->enabled_event_lock);

	sbi_list_for_each_entry(e, &state->enabled_event_list, ଗଣ୍ଠି) {
		ret = sse_event_check_inject(e, regs);
		ଯଦି (ret)
			ଯାଅ out;
	}

out:
	spin_unlock(&state->enabled_event_lock);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_event_set_pending(ସଂରଚନା sbi_sse_event *e)
{
	ଯଦି (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ଫେରନ୍ତୁ SBI_EINVALID_STATE;

	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ sse_ipi_inject_process(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_sse_event *e;
	ସଂରଚନା sse_ipi_inject_data evt;
	ସଂରଚନା sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	ଯେତେବେଳେ (!sbi_fifo_dequeue(sse_inject_fifo_r, &evt)) {
		ଯଦି (sse_event_get(evt.event_id, &e))
			ଜାରି;

		sse_event_set_pending(e);
		sse_event_put(e);
	}
}

ସ୍ଥିର ସଂରଚନା sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_ipi_inject_send(unsigned long hartid, uint32_t event_id)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା sbi_scratch *remote_scratch = ଖାଲି;
	ସଂରଚନା sse_ipi_inject_data evt = {event_id};
	ସଂରଚନା sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_to_scratch(hartid);
	ଯଦି (!remote_scratch)
		ଫେରନ୍ତୁ SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_offset_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_enqueue(sse_inject_fifo_r, &evt, false);
	ଯଦି (ret)
		ଫେରନ୍ତୁ SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, ଖାଲି);
	ଯଦି (ret)
		ଫେରନ୍ତୁ SBI_EFAIL;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_inject_event(uint32_t event_id, unsigned long hartid)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	/* In case of global event, provided hart_id is ignored */
	ଯଦି (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	ଯଦି (hartid != current_hartid()) {
		sse_event_put(e);
		ଫେରନ୍ତୁ sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_set_pending(e);
	sse_event_put(e);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	ଫେରନ୍ତୁ SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_event_enable(ସଂରଚନା sse_hart_state *shs,
			     ସଂରଚନା sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	ଯଦି (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ଫେରନ୍ତୁ SBI_EINVALID_STATE;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_add_to_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	ଯଦି (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  ଖାଲି);

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_event_complete(ସଂରଚନା sse_hart_state *shs,
			      ସଂରଚନା sbi_sse_event *e,
			      ସଂରଚନା sbi_trap_regs *regs,
			      ସଂରଚନା sbi_ecall_return *out)
	MUST_HOLD(&shs->enabled_event_lock)
{
	ଯଦି (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		ଫେରନ୍ତୁ SBI_EINVALID_STATE;

	ଯଦି (e->attrs.hartid != current_hartid())
		ଫେରନ୍ତୁ SBI_EINVAL;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	ଯଦି (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	out->skip_regs_update = true;

	ଫେରନ୍ତୁ SBI_OK;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_complete(ସଂରଚନା sbi_trap_regs *regs, ସଂରଚନା sbi_ecall_return *out)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = SBI_OK;
	ସଂରଚନା sbi_sse_event *tmp;
	ସଂରଚନା sse_hart_state *state = sse_thishart_state_ptr();

	spin_lock(&state->enabled_event_lock);
	sbi_list_for_each_entry(tmp, &state->enabled_event_list, ଗଣ୍ଠି) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		ଯଦି (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, out);
			ଭାଙ୍ଗ;
		}
	}
	spin_unlock(&state->enabled_event_lock);

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_enable(uint32_t event_id)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା sbi_sse_event *e;
	ସଂରଚନା sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_lock);
	sse_event_put(e);

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_disable(uint32_t event_id)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା sbi_sse_event *e;
	ସଂରଚନା sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_lock);

	sse_event_put(e);

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_hart_mask(ଶୂନ୍ୟ)
{
	ସଂରଚନା sse_hart_state *state = sse_thishart_state_ptr();

	ଯଦି (!state)
		ଫେରନ୍ତୁ SBI_EFAIL;

	ଯଦି (state->masked)
		ଫେରନ୍ତୁ SBI_EALREADY_STOPPED;

	state->masked = true;

	ଫେରନ୍ତୁ SBI_SUCCESS;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_hart_unmask(ଶୂନ୍ୟ)
{
	ସଂରଚନା sse_hart_state *state = sse_thishart_state_ptr();

	ଯଦି (!state)
		ଫେରନ୍ତୁ SBI_EFAIL;

	ଯଦି (!state->masked)
		ଫେରନ୍ତୁ SBI_EALREADY_STARTED;

	state->masked = false;

	ଫେରନ୍ତୁ SBI_SUCCESS;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_inject_from_ecall(uint32_t event_id, unsigned long hartid,
			      ସଂରଚନା sbi_ecall_return *out)
{
	ଯଦି (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_to_hartindex(hartid)))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଫେରନ୍ତୁ sse_inject_event(event_id, hartid);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_inject_event(uint32_t event_id)
{
	ଫେରନ୍ତୁ sse_inject_event(event_id, current_hartid());
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_add_event(uint32_t event_id, ଅଚଳ ସଂରଚନା sbi_sse_cb_ops *cb_ops)
{
	ସଂରଚନା sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	ଯଦି (info)
		ଫେରନ୍ତୁ SBI_EALREADY;

	ଯଦି (cb_ops && cb_ops->set_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		ଫେରନ୍ତୁ SBI_EINVAL;

	info = sbi_zalloc(ଆକାର(*info));
	ଯଦି (!info)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ADD(info, supported_events);

	ଫେରନ୍ତୁ SBI_OK;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_attr_check(uint32_t base_attr_id, uint32_t attr_ଗଣନା,
		       unsigned long phys_lo, unsigned long phys_hi,
		       unsigned long access)
{
	ଅଚଳ unsigned align = __riscv_xlen >> 3;
	uint64_t end_id = (uint64_t)base_attr_id + (attr_ଗଣନା - 1);

	ଯଦି (attr_ଗଣନା == 0)
		ଫେରନ୍ତୁ SBI_ERR_INVALID_PARAM;

	ଯଦି (end_id >= SBI_SSE_ATTR_MAX)
		ଫେରନ୍ତୁ SBI_EBAD_RANGE;

	ଯଦି (phys_lo & (align - 1))
		ଫେରନ୍ତୁ SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	ଯଦି (phys_hi)
		ଫେରନ୍ତୁ SBI_EINVALID_ADDR;

	ଯଦି (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 ଆକାର(unsigned long) * attr_ଗଣନା,
					 PRV_S, access))
		ଫେରନ୍ତୁ SBI_EINVALID_ADDR;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ copy_attrs(unsigned long *out, ଅଚଳ unsigned long *in,
		       unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା long_ଗଣନା)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	ପାଇଁ (i = 0; i < long_ଗଣନା; i++)
		out[i] = in[i];
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_read_attrs(uint32_t event_id, uint32_t base_attr_id,
		       uint32_t attr_ଗଣନା, unsigned long output_phys_lo,
		       unsigned long output_phys_hi)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	unsigned long *e_attrs;
	ସଂରଚନା sbi_sse_event *e;
	unsigned long *attrs;

	ret = sbi_sse_attr_check(base_attr_id, attr_ଗଣନା, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	ret = sse_event_get(event_id, &e);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	sbi_hart_protection_temp_map_range(output_phys_lo, ଆକାର(unsigned long) * attr_ଗଣନା);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (unsigned long *)&e->attrs;
	attrs = (unsigned long *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[base_attr_id], attr_ଗଣନା);

	sbi_hart_protection_temp_unmap_range(output_phys_lo, ଆକାର(unsigned long) * attr_ଗଣନା);

	sse_event_put(e);

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_write_attrs(ସଂରଚନା sbi_sse_event *e, uint32_t base_attr_id,
			   uint32_t attr_ଗଣନା, unsigned long ନିବେଶ_phys)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
	unsigned long attr = 0, val;
	uint32_t id, end_id = base_attr_id + attr_ଗଣନା;
	unsigned long *attrs = (unsigned long *)ନିବେଶ_phys;

	sbi_hart_protection_temp_map_range(ନିବେଶ_phys, ଆକାର(unsigned long) * attr_ଗଣନା);

	ପାଇଁ (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		ret = sse_event_set_attr_check(e, id, val);
		ଯଦି (ret)
			ଯାଅ out;
	}

	attr = 0;
	ପାଇଁ (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		sse_event_set_attr(e, id, val);
	}

out:
	sbi_hart_protection_temp_unmap_range(ନିବେଶ_phys, ଆକାର(unsigned long) * attr_ଗଣନା);

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_write_attrs(uint32_t event_id, uint32_t base_attr_id,
			uint32_t attr_ଗଣନା, unsigned long ନିବେଶ_phys_lo,
			unsigned long ନିବେଶ_phys_hi)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
	ସଂରଚନା sbi_sse_event *e;

	ret = sbi_sse_attr_check(base_attr_id, attr_ଗଣନା, ନିବେଶ_phys_lo,
				 ନିବେଶ_phys_hi, SBI_DOMAIN_READ);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	ret = sse_event_get(event_id, &e);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	ret = sse_write_attrs(e, base_attr_id, attr_ଗଣନା, ନିବେଶ_phys_lo);
	sse_event_put(e);

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_register(uint32_t event_id, unsigned long handler_entry_pc,
		     unsigned long handler_entry_arg)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା sbi_sse_event *e;

	ଯଦି (handler_entry_pc & 0x1)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_entry_pc,
					 ଆକାର(unsigned long), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		ଫେରନ୍ତୁ SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	ret = sse_event_register(e, handler_entry_pc, handler_entry_arg);
	sse_event_put(e);

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_unregister(uint32_t event_id)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ଶୂନ୍ୟ sse_event_init(ସଂରଚନା sbi_sse_event *e, ସଂରଚନା sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_INJECT_OFFSET);
}

ସ୍ଥିର ଶୂନ୍ୟ sse_event_ଗଣନା_init()
{
	ସଂରଚନା sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ଯଦି (EVENT_IS_GLOBAL(info->event_id))
			global_event_ଗଣନା++;
		ଅନ୍ୟଥା
			local_event_ଗଣନା++;
	}
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sse_global_init()
{
	ସଂରଚନା sbi_sse_event *e;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ev = 0;
	ସଂରଚନା sse_event_info *info;

	global_events = sbi_zalloc(ଆକାର(*global_events) * global_event_ଗଣନା);
	ଯଦି (!global_events)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ଯଦି (!EVENT_IS_GLOBAL(info->event_id))
			ଜାରି;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_LOCK_INIT(global_events[ev].lock);

		ev++;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ sse_local_init(ସଂରଚନା sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ev = 0;
	ସଂରଚନା sse_event_info *info;

	SBI_INIT_LIST_HEAD(&shs->enabled_event_list);
	SPIN_LOCK_INIT(shs->enabled_event_lock);

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ଯଦି (EVENT_IS_GLOBAL(info->event_id))
			ଜାରି;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sse_init(ସଂରଚନା sbi_scratch *scratch, bool cold_boot)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ଶୂନ୍ୟ *sse_inject_mem;
	ସଂରଚନା sse_hart_state *shs;
	ସଂରଚନା sbi_fifo *sse_inject_q;

	ଯଦି (cold_boot) {
		sse_event_ଗଣନା_init();

		ret = sse_global_init();
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;

		shs_ptr_off = sbi_scratch_alloc_offset(ଆକାର(ଶୂନ୍ୟ *));
		ଯଦି (!shs_ptr_off)
			ଫେରନ୍ତୁ SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_offset(ଆକାର(*sse_inject_q));
		ଯଦି (!sse_inject_fifo_off) {
			sbi_scratch_free_offset(shs_ptr_off);
			ଫେରନ୍ତୁ SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_offset(
			(global_event_ଗଣନା + local_event_ଗଣନା) *
			ଆକାର(ସଂରଚନା sse_ipi_inject_data));
		ଯଦି (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_offset(sse_inject_fifo_off);
			sbi_scratch_free_offset(shs_ptr_off);
			ଫେରନ୍ତୁ SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		ଯଦି (ret < 0) {
			sbi_scratch_free_offset(shs_ptr_off);
			ଫେରନ୍ତୁ ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	ଯଦି (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(ଆକାର(*shs) + ଆକାର(ସଂରଚନା sbi_sse_event) *
							local_event_ଗଣନା);
		ଯଦି (!shs)
			ଫେରନ୍ତୁ SBI_ENOMEM;

		shs->local_events = (ସଂରଚନା sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_set_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_ଗଣନା + local_event_ଗଣନା),
		      ଆକାର(ସଂରଚନା sse_ipi_inject_data));

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ sbi_sse_exit(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_sse_event *e;
	ସଂରଚନା sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ଯଦି (sse_event_get(info->event_id, &e))
			ଜାରି;

		ଯଦି (e->attrs.hartid != current_hartid())
			ଯାଅ skip;

		ଯଦି (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

skip:
		sse_event_put(e);
	}
}
