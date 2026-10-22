/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Rivos Systems Inc.
 *
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_barrier.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_locks.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_fifo.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart_protection.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_list.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_sse.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_slist.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), ਖਾਲੀ *, shs_ptr_off)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sse_ਸੈੱਟ_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), ਖਾਲੀ *, shs_ptr_off, (__sse_state))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_ਦੋਆਧਾਰੀ_ਅੰਕ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		ਅਚਲ ਬਣਤਰ sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		ਜੇ (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

ਬਣਤਰ sse_ਇੰਦਰਾਜ_state {
	/** entry pc */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ pc;
	/** a6 register state */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg;
};

ਬਣਤਰ sse_interrupted_state {
	/** sepc register state */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sepc;
	/** flags register state */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags;
	/** a6 register state */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a6;
	/** a7 register state */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a7;
};

ਬਣਤਰ sse_ipi_inject_ਅੰਕੜੇ {
	uint32_t event_id;
};

ਬਣਤਰ sbi_sse_event_attrs {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸਥਿਤੀ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ prio;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ config;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hartid;
	ਬਣਤਰ sse_ਇੰਦਰਾਜ_state ਇੰਦਰਾਜ_2;
	ਬਣਤਰ sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ assert_ਖੇਤਰ_ਵਿੱਥ(ਖੇਤਰ, attr_ਵਿੱਥ)                  \
	_Static_assert(                                          \
		((offsetof(ਬਣਤਰ sbi_sse_event_attrs, ਖੇਤਰ)) / \
		 ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) == attr_ਵਿੱਥ,          \
		"field " #field                                  \
		" from struct sbi_sse_event_attrs invalid offset, expected " #attr_offset)

assert_ਖੇਤਰ_ਵਿੱਥ(ਸਥਿਤੀ, SBI_SSE_ATTR_ਸਥਿਤੀ);
assert_ਖੇਤਰ_ਵਿੱਥ(prio, SBI_SSE_ATTR_PRIO);
assert_ਖੇਤਰ_ਵਿੱਥ(config, SBI_SSE_ATTR_CONFIG);
assert_ਖੇਤਰ_ਵਿੱਥ(hartid, SBI_SSE_ATTR_PREFERRED_HART);
assert_ਖੇਤਰ_ਵਿੱਥ(ਇੰਦਰਾਜ_2.pc, SBI_SSE_ATTR_ਇੰਦਰਾਜ_PC);
assert_ਖੇਤਰ_ਵਿੱਥ(ਇੰਦਰਾਜ_2.arg, SBI_SSE_ATTR_ਇੰਦਰਾਜ_ARG);
assert_ਖੇਤਰ_ਵਿੱਥ(interrupted.sepc, SBI_SSE_ATTR_INTERRUPTED_SEPC);
assert_ਖੇਤਰ_ਵਿੱਥ(interrupted.flags, SBI_SSE_ATTR_INTERRUPTED_FLAGS);
assert_ਖੇਤਰ_ਵਿੱਥ(interrupted.a6, SBI_SSE_ATTR_INTERRUPTED_A6);
assert_ਖੇਤਰ_ਵਿੱਥ(interrupted.a7, SBI_SSE_ATTR_INTERRUPTED_A7);

ਬਣਤਰ sbi_sse_event {
	ਬਣਤਰ sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	ਬਣਤਰ sse_event_info *info;
	ਬਣਤਰ sbi_dlist ਗੰਢ;
};

/** Per-hart state */
ਬਣਤਰ sse_hart_state {
	/**
	 * Lock that protects enabled_event_list
	 */
	spinlock_t enabled_event_ਤਾਲਾ;

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
	ਬਣਤਰ sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_ਤਾਲਾ);

	/**
	 * List of local events allocated at boot time.
	 */
	ਬਣਤਰ sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
ਬਣਤਰ sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t ਤਾਲਾ;

	/**
	 * global event struct
	 */
	ਬਣਤਰ sbi_sse_event event GUARDED_BY(&ਤਾਲਾ);
};

ਬਣਤਰ sse_event_info {
	uint32_t event_id;
	ਅਚਲ ਬਣਤਰ sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_ਗੰਢ(sse_event_info);
};

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ local_event_ਗਿਣਤੀ;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ global_event_ਗਿਣਤੀ;
ਸਥਿਰ ਬਣਤਰ sse_global_event *global_events;

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sse_inject_fifo_off;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ shs_ptr_off;

ਸਥਿਰ u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

ਸਥਿਰ ਪੂਰਨ sse_ipi_inject_send(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hartid, uint32_t event_id);

ਬਣਤਰ sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_ਗੰਢ_INIT(ਨਿਰਾ),
};

ਬਣਤਰ sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_ਗੰਢ_INIT(&global_software_event),
};

ਸਥਿਰ SBI_SLIST_ਸਿਰ(supported_events, sse_event_info) =
				SBI_SLIST_ਸਿਰ_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
ਸਥਿਰ ਅਚਲ uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_ਉੱਚਾ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_ਉੱਚਾ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_ਉਛਾਲ,
	SBI_SSE_EVENT_LOCAL_ਨੀਵਾਂ_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_ਨੀਵਾਂ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

ਸਥਿਰ bool sse_is_standard_event(uint32_t event_id)
{
	ਪੂਰਨ i;

	ਲਈ (i = 0; i < array_ਮਾਪ(standard_events); i++) {
		ਜੇ (event_id == standard_events[i])
			ਵਾਪਸ true;
	}

	ਵਾਪਸ false;
}

ਸਥਿਰ ਬਣਤਰ sse_event_info *sse_event_info_get(uint32_t event_id)
{
	ਬਣਤਰ sse_event_info *info;

	SBI_SLIST_FOR_EACH_ਇੰਦਰਾਜ(info, supported_events) {
		ਜੇ (info->event_id == event_id)
			ਵਾਪਸ info;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sse_event_state(ਬਣਤਰ sbi_sse_event *e)
{
	ਵਾਪਸ e->attrs.ਸਥਿਤੀ & SBI_SSE_ATTR_ਸਥਿਤੀ_STATE_ਨਕਾਬ;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sse_event_pending(ਬਣਤਰ sbi_sse_event *e)
{
	ਵਾਪਸ !!(e->attrs.ਸਥਿਤੀ & ਦੋਆਧਾਰੀ_ਅੰਕ(SBI_SSE_ATTR_ਸਥਿਤੀ_PENDING_ਵਿੱਥ));
}

ਸਥਿਰ bool sse_event_is_global(ਬਣਤਰ sbi_sse_event *e)
{
	ਵਾਪਸ EVENT_IS_GLOBAL(e->event_id);
}

ਸਥਿਰ bool sse_event_is_local(ਬਣਤਰ sbi_sse_event *e)
{
	ਵਾਪਸ !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
ਸਥਿਰ ਬਣਤਰ sse_hart_state *sse_get_hart_state(ਬਣਤਰ sbi_sse_event *e)
{
	ਬਣਤਰ sbi_scratch *s = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(e->hartindex);

	ਵਾਪਸ sse_get_hart_state_ptr(s);
}

ਸਥਿਰ ਬਣਤਰ sse_global_event *sse_get_global_event(ਬਣਤਰ sbi_sse_event *e)
{
	ਵਾਪਸ container_ਦਾ(e, ਬਣਤਰ sse_global_event, event);
}

ਸਥਿਰ ਖਾਲੀ sse_event_ਸੈੱਟ_state(ਬਣਤਰ sbi_sse_event *e,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ new_state)
{
	e->attrs.ਸਥਿਤੀ &= ~SBI_SSE_ATTR_ਸਥਿਤੀ_STATE_ਨਕਾਬ;
	e->attrs.ਸਥਿਤੀ |= new_state;
}

ਸਥਿਰ ਪੂਰਨ sse_event_get(uint32_t event_id,
			 ਬਣਤਰ sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
	ਬਣਤਰ sbi_sse_event *e;
	ਬਣਤਰ sse_hart_state *shs;

	ਜੇ (!eret)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (EVENT_IS_GLOBAL(event_id)) {
		ਲਈ (i = 0; i < global_event_ਗਿਣਤੀ; i++) {
			e = &global_events[i].event;
			ਜੇ (e->event_id == event_id) {
				spin_ਤਾਲਾ(&global_events[i].ਤਾਲਾ);
				*eret = e;
				ਵਾਪਸ SBI_SUCCESS;
			}
		}
	} ਨਹੀਂਤਾਂ {
		shs = sse_thishart_state_ptr();
		ਲਈ (i = 0; i < local_event_ਗਿਣਤੀ; i++) {
			e = &shs->local_events[i];
			ਜੇ (e->event_id == event_id) {
				*eret = e;
				ਵਾਪਸ SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	ਜੇ (sse_is_standard_event(event_id))
		ਵਾਪਸ SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	ਵਾਪਸ SBI_EINVAL;
}

ਸਥਿਰ ਖਾਲੀ sse_event_put(ਬਣਤਰ sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	ਬਣਤਰ sse_global_event *ge;

	ਜੇ (sse_event_is_local(e))
		ਵਾਪਸ;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->ਤਾਲਾ);
}

ਸਥਿਰ ਖਾਲੀ sse_event_remove_from_list(ਬਣਤਰ sbi_sse_event *e)
{
	sbi_list_del(&e->ਗੰਢ);
}

/**
 * Must be called under owner hart lock
 */
ਸਥਿਰ ਖਾਲੀ sse_event_ਜੋੜੋ_ਮੰਜ਼ਿਲ_list(ਬਣਤਰ sse_hart_state *state,
				   ਬਣਤਰ sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_ਤਾਲਾ)
{
	ਬਣਤਰ sbi_sse_event *tmp;

	sbi_list_for_each_ਇੰਦਰਾਜ(tmp, &state->enabled_event_list, ਗੰਢ) {
		ਜੇ (e->attrs.prio < tmp->attrs.prio)
			ਰੋਕੋ;
		ਜੇ (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			ਰੋਕੋ;
	}
	sbi_list_ਜੋੜੋ_ਪੂਛ(&e->ਗੰਢ, &tmp->ਗੰਢ);
}

/**
 * Must be called under owner hart lock
 */
ਸਥਿਰ ਪੂਰਨ sse_event_disable(ਬਣਤਰ sse_hart_state *shs,
			      ਬਣਤਰ sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_ਤਾਲਾ)
{
	ਜੇ (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ਵਾਪਸ SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_ਸੈੱਟ_state(e, SBI_SSE_STATE_REGISTERED);

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ sse_event_ਸੈੱਟ_hart_id_check(ਬਣਤਰ sbi_sse_event *e,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ new_hartid)
{
	ਪੂਰਨ hstate;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hartid = (uint32_t)new_hartid;
	ਬਣਤਰ sbi_domain *hd = sbi_domain_thishart_ptr();

	ਜੇ (!sse_event_is_global(e))
		ਵਾਪਸ SBI_EDENIED;

	ਜੇ (!sbi_domain_is_assigned_hart(hd, sbi_hartid_ਮੰਜ਼ਿਲ_hartindex(hartid)))
		ਵਾਪਸ SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	ਜੇ (hstate != SBI_HSM_STATE_STARTED)
		ਵਾਪਸ SBI_EINVAL;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ sse_event_ਸੈੱਟ_attr_check(ਬਣਤਰ sbi_sse_event *e, uint32_t attr_id,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val)
{
	ਚੋਣ (attr_id) {
	ਹਾਲਤ SBI_SSE_ATTR_CONFIG:
		ਜੇ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ਵਾਪਸ SBI_EINVALID_STATE;

		ਜੇ (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			ਵਾਪਸ SBI_EINVAL;

		ਵਾਪਸ SBI_OK;
	ਹਾਲਤ SBI_SSE_ATTR_PRIO:
		ਜੇ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ਵਾਪਸ SBI_EINVALID_STATE;

#ਜੇ __riscv_xlen > 32
		ਜੇ (val != (uint32_t)val)
			ਵਾਪਸ SBI_EINVAL;
#ਜੇ_ਅੰਤ
		ਵਾਪਸ SBI_OK;
	ਹਾਲਤ SBI_SSE_ATTR_PREFERRED_HART:
		ਜੇ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ਵਾਪਸ SBI_EINVALID_STATE;

		ਵਾਪਸ sse_event_ਸੈੱਟ_hart_id_check(e, val);
	ਹਾਲਤ SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		ਜੇ (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			ਵਾਪਸ SBI_EINVAL;
		__attribute__((__fallthrough__));
	ਹਾਲਤ SBI_SSE_ATTR_INTERRUPTED_SEPC:
	ਹਾਲਤ SBI_SSE_ATTR_INTERRUPTED_A6:
	ਹਾਲਤ SBI_SSE_ATTR_INTERRUPTED_A7:
		ਜੇ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			ਵਾਪਸ SBI_EINVALID_STATE;

		ਜੇ (current_hartid() != e->attrs.hartid)
			ਵਾਪਸ SBI_EINVAL;

		ਵਾਪਸ SBI_OK;
	ਮੂਲ:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		ਵਾਪਸ SBI_EDENIED;
	}
}

ਸਥਿਰ ਖਾਲੀ sse_event_ਸੈੱਟ_attr(ਬਣਤਰ sbi_sse_event *e, uint32_t attr_id,
			       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val)
{
	ਚੋਣ (attr_id) {
	ਹਾਲਤ SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_ਮੰਜ਼ਿਲ_hartindex(val);
		sse_event_invoke_cb(e, ਸੈੱਟ_hartid_cb, val);
		ਰੋਕੋ;

	ਹਾਲਤ SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		ਰੋਕੋ;
	}
}

ਸਥਿਰ ਪੂਰਨ sse_event_ਰਜਿਸਟਰ(ਬਣਤਰ sbi_sse_event *e,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ handler_ਇੰਦਰਾਜ_pc,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ handler_ਇੰਦਰਾਜ_arg)
{
	ਜੇ (sse_event_state(e) != SBI_SSE_STATE_ਅਣਵਰਤਿਆ)
		ਵਾਪਸ SBI_EINVALID_STATE;

	e->attrs.ਇੰਦਰਾਜ_2.pc = handler_ਇੰਦਰਾਜ_pc;
	e->attrs.ਇੰਦਰਾਜ_2.arg = handler_ਇੰਦਰਾਜ_arg;

	sse_event_ਸੈੱਟ_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, ਰਜਿਸਟਰ_cb);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ sse_event_unregister(ਬਣਤਰ sbi_sse_event *e)
{
	ਜੇ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ਵਾਪਸ SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_ਸੈੱਟ_state(e, SBI_SSE_STATE_ਅਣਵਰਤਿਆ);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sse_interrupted_flags(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mstatus)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hstatus, flags = 0;

	ਜੇ (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	ਜੇ (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	ਜੇ (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	ਜੇ (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	ਜੇ (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		ਜੇ (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		ਜੇ (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	ਵਾਪਸ flags;
}

ਸਥਿਰ ਖਾਲੀ sse_event_inject(ਬਣਤਰ sbi_sse_event *e,
			     ਬਣਤਰ sbi_trap_regs *regs)
{
	ਬਣਤਰ sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_ਸੈੱਟ_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.ਸਥਿਤੀ &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(SBI_SSE_ATTR_ਸਥਿਤੀ_PENDING_ਵਿੱਥ);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	ਜੇ (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	ਜੇ (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	ਜੇ (misa_extension('H')) {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hstatus = csr_read(CSR_HSTATUS);
		ਜੇ (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		ਨਹੀਂਤਾਂ
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		ਜੇ (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}
	csr_write(CSR_SEPC, regs->mepc);

	/* Setup entry context */
	regs->a6 = current_hartid();
	regs->a7 = e->attrs.ਇੰਦਰਾਜ_2.arg;
	regs->mepc = e->attrs.ਇੰਦਰਾਜ_2.pc;

	/*
	 * Return to S-mode with virtualization disabled, not expected landing
	 * pad, supervisor trap disabled.
	 */
	regs->mstatus &= ~(MSTATUS_MPP | MSTATUS_SIE | MSTATUS_SPELP);
	regs->mstatus |= (PRV_S << MSTATUS_MPP_ਖਿਸਕਾਓ);
	regs->mstatus |= MSTATUS_SDT;

#ਜੇ __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#ਜਾਂਜੇ __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#ਨਹੀਂਤਾਂ
#ਗਲਤੀ "Unexpected __riscv_xlen"
#ਜੇ_ਅੰਤ

}

ਸਥਿਰ ਖਾਲੀ sse_event_resume(ਬਣਤਰ sbi_sse_event *e,
			     ਬਣਤਰ sbi_trap_regs *regs)
{
	ਬਣਤਰ sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	ਜੇ (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_ਖਿਸਕਾਓ);

	ਜੇ (misa_extension('H')) {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hstatus = csr_read(CSR_HSTATUS);
#ਜੇ __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		ਜੇ (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#ਜਾਂਜੇ __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		ਜੇ (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#ਨਹੀਂਤਾਂ
#ਗਲਤੀ "Unexpected __riscv_xlen"
#ਜੇ_ਅੰਤ
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		ਜੇ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		ਜੇ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	ਜੇ (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	ਜੇ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	ਜੇ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	ਜੇ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	ਜੇ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

ਸਥਿਰ bool sse_event_is_ready(ਬਣਤਰ sbi_sse_event *e)
{
	ਜੇ (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		ਵਾਪਸ false;
	}

	ਵਾਪਸ true;
}

ਸਥਿਰ bool sse_event_check_inject(ਬਣਤਰ sbi_sse_event *e,
				   ਬਣਤਰ sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	ਜੇ (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		ਵਾਪਸ true;
	}

	ਜੇ (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		ਵਾਪਸ true;
	}

	ਵਾਪਸ false;
}

/* Return true if an event has been injected, false otherwise */
ਖਾਲੀ sbi_sse_process_pending_events(ਬਣਤਰ sbi_trap_regs *regs)
{
	bool ret;
	ਬਣਤਰ sbi_sse_event *e;
	ਬਣਤਰ sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	ਜੇ (state->masked)
		ਵਾਪਸ;

	spin_ਤਾਲਾ(&state->enabled_event_ਤਾਲਾ);

	sbi_list_for_each_ਇੰਦਰਾਜ(e, &state->enabled_event_list, ਗੰਢ) {
		ret = sse_event_check_inject(e, regs);
		ਜੇ (ret)
			ਜਾਓ ਬਾਹਰਲਾ;
	}

ਬਾਹਰਲਾ:
	spin_unlock(&state->enabled_event_ਤਾਲਾ);
}

ਸਥਿਰ ਪੂਰਨ sse_event_ਸੈੱਟ_pending(ਬਣਤਰ sbi_sse_event *e)
{
	ਜੇ (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ਵਾਪਸ SBI_EINVALID_STATE;

	e->attrs.ਸਥਿਤੀ |= ਦੋਆਧਾਰੀ_ਅੰਕ(SBI_SSE_ATTR_ਸਥਿਤੀ_PENDING_ਵਿੱਥ);

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਖਾਲੀ sse_ipi_inject_process(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_sse_event *e;
	ਬਣਤਰ sse_ipi_inject_ਅੰਕੜੇ evt;
	ਬਣਤਰ sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_ਵਿੱਥ_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	ਜਦੋਂ (!sbi_fifo_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(sse_inject_fifo_r, &evt)) {
		ਜੇ (sse_event_get(evt.event_id, &e))
			ਜਾਰੀ;

		sse_event_ਸੈੱਟ_pending(e);
		sse_event_put(e);
	}
}

ਸਥਿਰ ਬਣਤਰ sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

ਸਥਿਰ ਪੂਰਨ sse_ipi_inject_send(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hartid, uint32_t event_id)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ sbi_scratch *remote_scratch = ਨਿਰਾ;
	ਬਣਤਰ sse_ipi_inject_ਅੰਕੜੇ evt = {event_id};
	ਬਣਤਰ sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_ਮੰਜ਼ਿਲ_scratch(hartid);
	ਜੇ (!remote_scratch)
		ਵਾਪਸ SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_ਵਿੱਥ_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(sse_inject_fifo_r, &evt, false);
	ਜੇ (ret)
		ਵਾਪਸ SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, ਨਿਰਾ);
	ਜੇ (ret)
		ਵਾਪਸ SBI_EFAIL;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ sse_inject_event(uint32_t event_id, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hartid)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	/* In case of global event, provided hart_id is ignored */
	ਜੇ (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	ਜੇ (hartid != current_hartid()) {
		sse_event_put(e);
		ਵਾਪਸ sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_ਸੈੱਟ_pending(e);
	sse_event_put(e);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ਵਾਪਸ SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
ਸਥਿਰ ਪੂਰਨ sse_event_enable(ਬਣਤਰ sse_hart_state *shs,
			     ਬਣਤਰ sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_ਤਾਲਾ)
{
	ਜੇ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ਵਾਪਸ SBI_EINVALID_STATE;

	sse_event_ਸੈੱਟ_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_ਜੋੜੋ_ਮੰਜ਼ਿਲ_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	ਜੇ (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  ਨਿਰਾ);

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ sse_event_complete(ਬਣਤਰ sse_hart_state *shs,
			      ਬਣਤਰ sbi_sse_event *e,
			      ਬਣਤਰ sbi_trap_regs *regs,
			      ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
	MUST_HOLD(&shs->enabled_event_ਤਾਲਾ)
{
	ਜੇ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		ਵਾਪਸ SBI_EINVALID_STATE;

	ਜੇ (e->attrs.hartid != current_hartid())
		ਵਾਪਸ SBI_EINVAL;

	sse_event_ਸੈੱਟ_state(e, SBI_SSE_STATE_ENABLED);
	ਜੇ (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	ਬਾਹਰਲਾ->skip_regs_update = true;

	ਵਾਪਸ SBI_OK;
}

ਪੂਰਨ sbi_sse_complete(ਬਣਤਰ sbi_trap_regs *regs, ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਪੂਰਨ ret = SBI_OK;
	ਬਣਤਰ sbi_sse_event *tmp;
	ਬਣਤਰ sse_hart_state *state = sse_thishart_state_ptr();

	spin_ਤਾਲਾ(&state->enabled_event_ਤਾਲਾ);
	sbi_list_for_each_ਇੰਦਰਾਜ(tmp, &state->enabled_event_list, ਗੰਢ) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		ਜੇ (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, ਬਾਹਰਲਾ);
			ਰੋਕੋ;
		}
	}
	spin_unlock(&state->enabled_event_ਤਾਲਾ);

	ਵਾਪਸ ret;
}

ਪੂਰਨ sbi_sse_enable(uint32_t event_id)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ sbi_sse_event *e;
	ਬਣਤਰ sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	shs = sse_get_hart_state(e);
	spin_ਤਾਲਾ(&shs->enabled_event_ਤਾਲਾ);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_ਤਾਲਾ);
	sse_event_put(e);

	ਵਾਪਸ ret;
}

ਪੂਰਨ sbi_sse_disable(uint32_t event_id)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ sbi_sse_event *e;
	ਬਣਤਰ sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	shs = sse_get_hart_state(e);
	spin_ਤਾਲਾ(&shs->enabled_event_ਤਾਲਾ);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_ਤਾਲਾ);

	sse_event_put(e);

	ਵਾਪਸ ret;
}

ਪੂਰਨ sbi_sse_hart_ਨਕਾਬ(ਖਾਲੀ)
{
	ਬਣਤਰ sse_hart_state *state = sse_thishart_state_ptr();

	ਜੇ (!state)
		ਵਾਪਸ SBI_EFAIL;

	ਜੇ (state->masked)
		ਵਾਪਸ SBI_EALREADY_STOPPED;

	state->masked = true;

	ਵਾਪਸ SBI_SUCCESS;
}

ਪੂਰਨ sbi_sse_hart_unmask(ਖਾਲੀ)
{
	ਬਣਤਰ sse_hart_state *state = sse_thishart_state_ptr();

	ਜੇ (!state)
		ਵਾਪਸ SBI_EFAIL;

	ਜੇ (!state->masked)
		ਵਾਪਸ SBI_EALREADY_STARTED;

	state->masked = false;

	ਵਾਪਸ SBI_SUCCESS;
}

ਪੂਰਨ sbi_sse_inject_from_ecall(uint32_t event_id, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hartid,
			      ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਜੇ (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_ਮੰਜ਼ਿਲ_hartindex(hartid)))
		ਵਾਪਸ SBI_EINVAL;

	ਵਾਪਸ sse_inject_event(event_id, hartid);
}

ਪੂਰਨ sbi_sse_inject_event(uint32_t event_id)
{
	ਵਾਪਸ sse_inject_event(event_id, current_hartid());
}

ਪੂਰਨ sbi_sse_ਜੋੜੋ_event(uint32_t event_id, ਅਚਲ ਬਣਤਰ sbi_sse_cb_ops *cb_ops)
{
	ਬਣਤਰ sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	ਜੇ (info)
		ਵਾਪਸ SBI_EALREADY;

	ਜੇ (cb_ops && cb_ops->ਸੈੱਟ_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		ਵਾਪਸ SBI_EINVAL;

	info = sbi_zalloc(ਆਕਾਰ(*info));
	ਜੇ (!info)
		ਵਾਪਸ SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ਜੋੜੋ(info, supported_events);

	ਵਾਪਸ SBI_OK;
}

ਪੂਰਨ sbi_sse_attr_check(uint32_t ਅਧਾਰ_attr_id, uint32_t attr_ਗਿਣਤੀ,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ phys_lo, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ phys_hi,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ access)
{
	ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ align = __riscv_xlen >> 3;
	uint64_t ਸਿਰਾ_id = (uint64_t)ਅਧਾਰ_attr_id + (attr_ਗਿਣਤੀ - 1);

	ਜੇ (attr_ਗਿਣਤੀ == 0)
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	ਜੇ (ਸਿਰਾ_id >= SBI_SSE_ATTR_MAX)
		ਵਾਪਸ SBI_EBAD_RANGE;

	ਜੇ (phys_lo & (align - 1))
		ਵਾਪਸ SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	ਜੇ (phys_hi)
		ਵਾਪਸ SBI_EINVALID_ADDR;

	ਜੇ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) * attr_ਗਿਣਤੀ,
					 PRV_S, access))
		ਵਾਪਸ SBI_EINVALID_ADDR;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਖਾਲੀ copy_attrs(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਵਿੱਚ_2,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ long_ਗਿਣਤੀ)
{
	ਪੂਰਨ i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	ਲਈ (i = 0; i < long_ਗਿਣਤੀ; i++)
		ਬਾਹਰਲਾ[i] = ਵਿੱਚ_2[i];
}

ਪੂਰਨ sbi_sse_read_attrs(uint32_t event_id, uint32_t ਅਧਾਰ_attr_id,
		       uint32_t attr_ਗਿਣਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ output_phys_lo,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ output_phys_hi)
{
	ਪੂਰਨ ret;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *e_attrs;
	ਬਣਤਰ sbi_sse_event *e;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *attrs;

	ret = sbi_sse_attr_check(ਅਧਾਰ_attr_id, attr_ਗਿਣਤੀ, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ret = sse_event_get(event_id, &e);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	sbi_hart_protection_ਆਰਜ਼ੀ_map_range(output_phys_lo, ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) * attr_ਗਿਣਤੀ);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *)&e->attrs;
	attrs = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[ਅਧਾਰ_attr_id], attr_ਗਿਣਤੀ);

	sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range(output_phys_lo, ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) * attr_ਗਿਣਤੀ);

	sse_event_put(e);

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ sse_write_attrs(ਬਣਤਰ sbi_sse_event *e, uint32_t ਅਧਾਰ_attr_id,
			   uint32_t attr_ਗਿਣਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਿਵੇਸ਼_phys)
{
	ਪੂਰਨ ret = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ attr = 0, val;
	uint32_t id, ਸਿਰਾ_id = ਅਧਾਰ_attr_id + attr_ਗਿਣਤੀ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *attrs = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *)ਨਿਵੇਸ਼_phys;

	sbi_hart_protection_ਆਰਜ਼ੀ_map_range(ਨਿਵੇਸ਼_phys, ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) * attr_ਗਿਣਤੀ);

	ਲਈ (id = ਅਧਾਰ_attr_id; id < ਸਿਰਾ_id; id++) {
		val = attrs[attr++];
		ret = sse_event_ਸੈੱਟ_attr_check(e, id, val);
		ਜੇ (ret)
			ਜਾਓ ਬਾਹਰਲਾ;
	}

	attr = 0;
	ਲਈ (id = ਅਧਾਰ_attr_id; id < ਸਿਰਾ_id; id++) {
		val = attrs[attr++];
		sse_event_ਸੈੱਟ_attr(e, id, val);
	}

ਬਾਹਰਲਾ:
	sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range(ਨਿਵੇਸ਼_phys, ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) * attr_ਗਿਣਤੀ);

	ਵਾਪਸ ret;
}

ਪੂਰਨ sbi_sse_write_attrs(uint32_t event_id, uint32_t ਅਧਾਰ_attr_id,
			uint32_t attr_ਗਿਣਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਿਵੇਸ਼_phys_lo,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਿਵੇਸ਼_phys_hi)
{
	ਪੂਰਨ ret = 0;
	ਬਣਤਰ sbi_sse_event *e;

	ret = sbi_sse_attr_check(ਅਧਾਰ_attr_id, attr_ਗਿਣਤੀ, ਨਿਵੇਸ਼_phys_lo,
				 ਨਿਵੇਸ਼_phys_hi, SBI_DOMAIN_READ);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ret = sse_event_get(event_id, &e);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ret = sse_write_attrs(e, ਅਧਾਰ_attr_id, attr_ਗਿਣਤੀ, ਨਿਵੇਸ਼_phys_lo);
	sse_event_put(e);

	ਵਾਪਸ ret;
}

ਪੂਰਨ sbi_sse_ਰਜਿਸਟਰ(uint32_t event_id, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ handler_ਇੰਦਰਾਜ_pc,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ handler_ਇੰਦਰਾਜ_arg)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ sbi_sse_event *e;

	ਜੇ (handler_ਇੰਦਰਾਜ_pc & 0x1)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_ਇੰਦਰਾਜ_pc,
					 ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		ਵਾਪਸ SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ret = sse_event_ਰਜਿਸਟਰ(e, handler_ਇੰਦਰਾਜ_pc, handler_ਇੰਦਰਾਜ_arg);
	sse_event_put(e);

	ਵਾਪਸ ret;
}

ਪੂਰਨ sbi_sse_unregister(uint32_t event_id)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	ਵਾਪਸ ret;
}

ਸਥਿਰ ਖਾਲੀ sse_event_init(ਬਣਤਰ sbi_sse_event *e, ਬਣਤਰ sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.ਸਥਿਤੀ |= ਦੋਆਧਾਰੀ_ਅੰਕ(SBI_SSE_ATTR_ਸਥਿਤੀ_INJECT_ਵਿੱਥ);
}

ਸਥਿਰ ਖਾਲੀ sse_event_ਗਿਣਤੀ_init()
{
	ਬਣਤਰ sse_event_info *info;

	SBI_SLIST_FOR_EACH_ਇੰਦਰਾਜ(info, supported_events) {
		ਜੇ (EVENT_IS_GLOBAL(info->event_id))
			global_event_ਗਿਣਤੀ++;
		ਨਹੀਂਤਾਂ
			local_event_ਗਿਣਤੀ++;
	}
}

ਸਥਿਰ ਪੂਰਨ sse_global_init()
{
	ਬਣਤਰ sbi_sse_event *e;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ev = 0;
	ਬਣਤਰ sse_event_info *info;

	global_events = sbi_zalloc(ਆਕਾਰ(*global_events) * global_event_ਗਿਣਤੀ);
	ਜੇ (!global_events)
		ਵਾਪਸ SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ਇੰਦਰਾਜ(info, supported_events) {
		ਜੇ (!EVENT_IS_GLOBAL(info->event_id))
			ਜਾਰੀ;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_ਤਾਲਾ_INIT(global_events[ev].ਤਾਲਾ);

		ev++;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ sse_local_init(ਬਣਤਰ sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ev = 0;
	ਬਣਤਰ sse_event_info *info;

	SBI_INIT_LIST_ਸਿਰ(&shs->enabled_event_list);
	SPIN_ਤਾਲਾ_INIT(shs->enabled_event_ਤਾਲਾ);

	SBI_SLIST_FOR_EACH_ਇੰਦਰਾਜ(info, supported_events) {
		ਜੇ (EVENT_IS_GLOBAL(info->event_id))
			ਜਾਰੀ;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

ਪੂਰਨ sbi_sse_init(ਬਣਤਰ sbi_scratch *scratch, bool cold_boot)
{
	ਪੂਰਨ ret;
	ਖਾਲੀ *sse_inject_mem;
	ਬਣਤਰ sse_hart_state *shs;
	ਬਣਤਰ sbi_fifo *sse_inject_q;

	ਜੇ (cold_boot) {
		sse_event_ਗਿਣਤੀ_init();

		ret = sse_global_init();
		ਜੇ (ret)
			ਵਾਪਸ ret;

		shs_ptr_off = sbi_scratch_alloc_ਵਿੱਥ(ਆਕਾਰ(ਖਾਲੀ *));
		ਜੇ (!shs_ptr_off)
			ਵਾਪਸ SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_ਵਿੱਥ(ਆਕਾਰ(*sse_inject_q));
		ਜੇ (!sse_inject_fifo_off) {
			sbi_scratch_free_ਵਿੱਥ(shs_ptr_off);
			ਵਾਪਸ SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_ਵਿੱਥ(
			(global_event_ਗਿਣਤੀ + local_event_ਗਿਣਤੀ) *
			ਆਕਾਰ(ਬਣਤਰ sse_ipi_inject_ਅੰਕੜੇ));
		ਜੇ (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_ਵਿੱਥ(sse_inject_fifo_off);
			sbi_scratch_free_ਵਿੱਥ(shs_ptr_off);
			ਵਾਪਸ SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		ਜੇ (ret < 0) {
			sbi_scratch_free_ਵਿੱਥ(shs_ptr_off);
			ਵਾਪਸ ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	ਜੇ (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(ਆਕਾਰ(*shs) + ਆਕਾਰ(ਬਣਤਰ sbi_sse_event) *
							local_event_ਗਿਣਤੀ);
		ਜੇ (!shs)
			ਵਾਪਸ SBI_ENOMEM;

		shs->local_events = (ਬਣਤਰ sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_ਸੈੱਟ_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_ਵਿੱਥ_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_ਵਿੱਥ_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_ਗਿਣਤੀ + local_event_ਗਿਣਤੀ),
		      ਆਕਾਰ(ਬਣਤਰ sse_ipi_inject_ਅੰਕੜੇ));

	ਵਾਪਸ 0;
}

ਖਾਲੀ sbi_sse_exit(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_sse_event *e;
	ਬਣਤਰ sse_event_info *info;

	SBI_SLIST_FOR_EACH_ਇੰਦਰਾਜ(info, supported_events) {
		ਜੇ (sse_event_get(info->event_id, &e))
			ਜਾਰੀ;

		ਜੇ (e->attrs.hartid != current_hartid())
			ਜਾਓ skip;

		ਜੇ (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_ਸੈੱਟ_state(e, SBI_SSE_STATE_ਅਣਵਰਤਿਆ);

skip:
		sse_event_put(e);
	}
}
