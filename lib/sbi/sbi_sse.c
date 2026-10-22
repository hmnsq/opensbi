/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Rivos Systems Inc.
 *
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_barrier.h>
#അന്തര്ഭാവ <sbi/riscv_encoding.h>
#അന്തര്ഭാവ <sbi/riscv_locks.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_fifo.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_hart_protection.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_hsm.h>
#അന്തര്ഭാവ <sbi/sbi_ipi.h>
#അന്തര്ഭാവ <sbi/sbi_list.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_pmu.h>
#അന്തര്ഭാവ <sbi/sbi_sse.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_slist.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>

#അന്തര്ഭാവ <sbi/sbi_console.h>

#define sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), ശൂന്യം *, shs_ptr_off)

#define sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define sse_set_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), ശൂന്യം *, shs_ptr_off, (__sse_state))

#define EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_BIT)

#define sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		സ്ഥിരം ഘടന sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		എങ്കിൽ (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

ഘടന sse_entry_state {
	/** entry pc */
	unsigned long pc;
	/** a6 register state */
	unsigned long arg;
};

ഘടന sse_interrupted_state {
	/** sepc register state */
	unsigned long sepc;
	/** flags register state */
	unsigned long flags;
	/** a6 register state */
	unsigned long a6;
	/** a7 register state */
	unsigned long a7;
};

ഘടന sse_ipi_inject_data {
	uint32_t event_id;
};

ഘടന sbi_sse_event_attrs {
	unsigned long status;
	unsigned long prio;
	unsigned long config;
	unsigned long hartid;
	ഘടന sse_entry_state entry;
	ഘടന sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)                  \
	_Static_assert(                                          \
		((offsetof(ഘടന sbi_sse_event_attrs, field)) / \
		 വലുപ്പം(unsigned long)) == attr_offset,          \
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

ഘടന sbi_sse_event {
	ഘടന sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	ഘടന sse_event_info *info;
	ഘടന sbi_dlist കണ്ണി;
};

/** Per-hart state */
ഘടന sse_hart_state {
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
	ഘടന sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_lock);

	/**
	 * List of local events allocated at boot time.
	 */
	ഘടന sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
ഘടന sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t lock;

	/**
	 * global event struct
	 */
	ഘടന sbi_sse_event event GUARDED_BY(&lock);
};

ഘടന sse_event_info {
	uint32_t event_id;
	സ്ഥിരം ഘടന sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_കണ്ണി(sse_event_info);
};

സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ local_event_എണ്ണം;
സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ global_event_എണ്ണം;
സ്ഥിതം ഘടന sse_global_event *global_events;

സ്ഥിതം unsigned long sse_inject_fifo_off;
സ്ഥിതം unsigned long sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
സ്ഥിതം unsigned long shs_ptr_off;

സ്ഥിതം u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

സ്ഥിതം പൂർണ്ണസംഖ്യ sse_ipi_inject_send(unsigned long hartid, uint32_t event_id);

ഘടന sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_കണ്ണി_INIT(ശൂന്യ),
};

ഘടന sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_കണ്ണി_INIT(&global_software_event),
};

സ്ഥിതം SBI_SLIST_HEAD(supported_events, sse_event_info) =
				SBI_SLIST_HEAD_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
സ്ഥിതം സ്ഥിരം uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_മുകളിലെ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_മുകളിലെ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW,
	SBI_SSE_EVENT_LOCAL_താഴെയുള്ള_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_താഴെയുള്ള_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

സ്ഥിതം bool sse_is_standard_event(uint32_t event_id)
{
	പൂർണ്ണസംഖ്യ i;

	വേണ്ടി (i = 0; i < array_size(standard_events); i++) {
		എങ്കിൽ (event_id == standard_events[i])
			തിരികെ true;
	}

	തിരികെ false;
}

സ്ഥിതം ഘടന sse_event_info *sse_event_info_get(uint32_t event_id)
{
	ഘടന sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		എങ്കിൽ (info->event_id == event_id)
			തിരികെ info;
	}

	തിരികെ ശൂന്യ;
}

സ്ഥിതം unsigned long sse_event_state(ഘടന sbi_sse_event *e)
{
	തിരികെ e->attrs.status & SBI_SSE_ATTR_STATUS_STATE_MASK;
}

സ്ഥിതം unsigned long sse_event_pending(ഘടന sbi_sse_event *e)
{
	തിരികെ !!(e->attrs.status & BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET));
}

സ്ഥിതം bool sse_event_is_global(ഘടന sbi_sse_event *e)
{
	തിരികെ EVENT_IS_GLOBAL(e->event_id);
}

സ്ഥിതം bool sse_event_is_local(ഘടന sbi_sse_event *e)
{
	തിരികെ !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
സ്ഥിതം ഘടന sse_hart_state *sse_get_hart_state(ഘടന sbi_sse_event *e)
{
	ഘടന sbi_scratch *s = sbi_hartindex_to_scratch(e->hartindex);

	തിരികെ sse_get_hart_state_ptr(s);
}

സ്ഥിതം ഘടന sse_global_event *sse_get_global_event(ഘടന sbi_sse_event *e)
{
	തിരികെ container_of(e, ഘടന sse_global_event, event);
}

സ്ഥിതം ശൂന്യം sse_event_set_state(ഘടന sbi_sse_event *e,
				unsigned long new_state)
{
	e->attrs.status &= ~SBI_SSE_ATTR_STATUS_STATE_MASK;
	e->attrs.status |= new_state;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sse_event_get(uint32_t event_id,
			 ഘടന sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	unsigned പൂർണ്ണസംഖ്യ i;
	ഘടന sbi_sse_event *e;
	ഘടന sse_hart_state *shs;

	എങ്കിൽ (!eret)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (EVENT_IS_GLOBAL(event_id)) {
		വേണ്ടി (i = 0; i < global_event_എണ്ണം; i++) {
			e = &global_events[i].event;
			എങ്കിൽ (e->event_id == event_id) {
				spin_lock(&global_events[i].lock);
				*eret = e;
				തിരികെ SBI_SUCCESS;
			}
		}
	} അല്ലെങ്കിൽ {
		shs = sse_thishart_state_ptr();
		വേണ്ടി (i = 0; i < local_event_എണ്ണം; i++) {
			e = &shs->local_events[i];
			എങ്കിൽ (e->event_id == event_id) {
				*eret = e;
				തിരികെ SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	എങ്കിൽ (sse_is_standard_event(event_id))
		തിരികെ SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	തിരികെ SBI_EINVAL;
}

സ്ഥിതം ശൂന്യം sse_event_put(ഘടന sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	ഘടന sse_global_event *ge;

	എങ്കിൽ (sse_event_is_local(e))
		തിരികെ;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->lock);
}

സ്ഥിതം ശൂന്യം sse_event_remove_from_list(ഘടന sbi_sse_event *e)
{
	sbi_list_del(&e->കണ്ണി);
}

/**
 * Must be called under owner hart lock
 */
സ്ഥിതം ശൂന്യം sse_event_add_to_list(ഘടന sse_hart_state *state,
				   ഘടന sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_lock)
{
	ഘടന sbi_sse_event *tmp;

	sbi_list_for_each_entry(tmp, &state->enabled_event_list, കണ്ണി) {
		എങ്കിൽ (e->attrs.prio < tmp->attrs.prio)
			നിർത്തുക;
		എങ്കിൽ (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			നിർത്തുക;
	}
	sbi_list_add_tail(&e->കണ്ണി, &tmp->കണ്ണി);
}

/**
 * Must be called under owner hart lock
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ sse_event_disable(ഘടന sse_hart_state *shs,
			      ഘടന sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	എങ്കിൽ (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		തിരികെ SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sse_event_set_hart_id_check(ഘടന sbi_sse_event *e,
				       unsigned long new_hartid)
{
	പൂർണ്ണസംഖ്യ hstate;
	unsigned പൂർണ്ണസംഖ്യ hartid = (uint32_t)new_hartid;
	ഘടന sbi_domain *hd = sbi_domain_thishart_ptr();

	എങ്കിൽ (!sse_event_is_global(e))
		തിരികെ SBI_EDENIED;

	എങ്കിൽ (!sbi_domain_is_assigned_hart(hd, sbi_hartid_to_hartindex(hartid)))
		തിരികെ SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	എങ്കിൽ (hstate != SBI_HSM_STATE_STARTED)
		തിരികെ SBI_EINVAL;

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sse_event_set_attr_check(ഘടന sbi_sse_event *e, uint32_t attr_id,
				    unsigned long val)
{
	തിരഞ്ഞെടുപ്പ് (attr_id) {
	അവസ്ഥ SBI_SSE_ATTR_CONFIG:
		എങ്കിൽ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			തിരികെ SBI_EINVALID_STATE;

		എങ്കിൽ (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			തിരികെ SBI_EINVAL;

		തിരികെ SBI_OK;
	അവസ്ഥ SBI_SSE_ATTR_PRIO:
		എങ്കിൽ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			തിരികെ SBI_EINVALID_STATE;

#എങ്കിൽ __riscv_xlen > 32
		എങ്കിൽ (val != (uint32_t)val)
			തിരികെ SBI_EINVAL;
#endif
		തിരികെ SBI_OK;
	അവസ്ഥ SBI_SSE_ATTR_PREFERRED_HART:
		എങ്കിൽ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			തിരികെ SBI_EINVALID_STATE;

		തിരികെ sse_event_set_hart_id_check(e, val);
	അവസ്ഥ SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		എങ്കിൽ (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			തിരികെ SBI_EINVAL;
		__attribute__((__fallthrough__));
	അവസ്ഥ SBI_SSE_ATTR_INTERRUPTED_SEPC:
	അവസ്ഥ SBI_SSE_ATTR_INTERRUPTED_A6:
	അവസ്ഥ SBI_SSE_ATTR_INTERRUPTED_A7:
		എങ്കിൽ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			തിരികെ SBI_EINVALID_STATE;

		എങ്കിൽ (current_hartid() != e->attrs.hartid)
			തിരികെ SBI_EINVAL;

		തിരികെ SBI_OK;
	സ്ഥിരസ്ഥിതി:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		തിരികെ SBI_EDENIED;
	}
}

സ്ഥിതം ശൂന്യം sse_event_set_attr(ഘടന sbi_sse_event *e, uint32_t attr_id,
			       unsigned long val)
{
	തിരഞ്ഞെടുപ്പ് (attr_id) {
	അവസ്ഥ SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		നിർത്തുക;
	അവസ്ഥ SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		നിർത്തുക;
	അവസ്ഥ SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_to_hartindex(val);
		sse_event_invoke_cb(e, set_hartid_cb, val);
		നിർത്തുക;

	അവസ്ഥ SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		നിർത്തുക;
	അവസ്ഥ SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		നിർത്തുക;
	അവസ്ഥ SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		നിർത്തുക;
	അവസ്ഥ SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		നിർത്തുക;
	}
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sse_event_register(ഘടന sbi_sse_event *e,
			      unsigned long handler_entry_pc,
			      unsigned long handler_entry_arg)
{
	എങ്കിൽ (sse_event_state(e) != SBI_SSE_STATE_UNUSED)
		തിരികെ SBI_EINVALID_STATE;

	e->attrs.entry.pc = handler_entry_pc;
	e->attrs.entry.arg = handler_entry_arg;

	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, register_cb);

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sse_event_unregister(ഘടന sbi_sse_event *e)
{
	എങ്കിൽ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		തിരികെ SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

	തിരികെ 0;
}

സ്ഥിതം unsigned long sse_interrupted_flags(unsigned long mstatus)
{
	unsigned long hstatus, flags = 0;

	എങ്കിൽ (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	എങ്കിൽ (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	എങ്കിൽ (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	എങ്കിൽ (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	എങ്കിൽ (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		എങ്കിൽ (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		എങ്കിൽ (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	തിരികെ flags;
}

സ്ഥിതം ശൂന്യം sse_event_inject(ഘടന sbi_sse_event *e,
			     ഘടന sbi_trap_regs *regs)
{
	ഘടന sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_set_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.status &= ~BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	എങ്കിൽ (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	എങ്കിൽ (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	എങ്കിൽ (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
		എങ്കിൽ (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		അല്ലെങ്കിൽ
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		എങ്കിൽ (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
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

#എങ്കിൽ __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#അല്ലെങ്കിലെങ്കിൽ __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#അല്ലെങ്കിൽ
#error "Unexpected __riscv_xlen"
#endif

}

സ്ഥിതം ശൂന്യം sse_event_resume(ഘടന sbi_sse_event *e,
			     ഘടന sbi_trap_regs *regs)
{
	ഘടന sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	എങ്കിൽ (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

	എങ്കിൽ (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
#എങ്കിൽ __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		എങ്കിൽ (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#അല്ലെങ്കിലെങ്കിൽ __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		എങ്കിൽ (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#അല്ലെങ്കിൽ
#error "Unexpected __riscv_xlen"
#endif
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		എങ്കിൽ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		എങ്കിൽ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	എങ്കിൽ (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	എങ്കിൽ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	എങ്കിൽ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	എങ്കിൽ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	എങ്കിൽ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

സ്ഥിതം bool sse_event_is_ready(ഘടന sbi_sse_event *e)
{
	എങ്കിൽ (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		തിരികെ false;
	}

	തിരികെ true;
}

സ്ഥിതം bool sse_event_check_inject(ഘടന sbi_sse_event *e,
				   ഘടന sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	എങ്കിൽ (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		തിരികെ true;
	}

	എങ്കിൽ (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		തിരികെ true;
	}

	തിരികെ false;
}

/* Return true if an event has been injected, false otherwise */
ശൂന്യം sbi_sse_process_pending_events(ഘടന sbi_trap_regs *regs)
{
	bool ret;
	ഘടന sbi_sse_event *e;
	ഘടന sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	എങ്കിൽ (state->masked)
		തിരികെ;

	spin_lock(&state->enabled_event_lock);

	sbi_list_for_each_entry(e, &state->enabled_event_list, കണ്ണി) {
		ret = sse_event_check_inject(e, regs);
		എങ്കിൽ (ret)
			പോകുക out;
	}

out:
	spin_unlock(&state->enabled_event_lock);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sse_event_set_pending(ഘടന sbi_sse_event *e)
{
	എങ്കിൽ (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		തിരികെ SBI_EINVALID_STATE;

	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	തിരികെ SBI_OK;
}

സ്ഥിതം ശൂന്യം sse_ipi_inject_process(ഘടന sbi_scratch *scratch)
{
	ഘടന sbi_sse_event *e;
	ഘടന sse_ipi_inject_data evt;
	ഘടന sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	ആകുമ്പോൾ (!sbi_fifo_dequeue(sse_inject_fifo_r, &evt)) {
		എങ്കിൽ (sse_event_get(evt.event_id, &e))
			തുടരുക;

		sse_event_set_pending(e);
		sse_event_put(e);
	}
}

സ്ഥിതം ഘടന sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ sse_ipi_inject_send(unsigned long hartid, uint32_t event_id)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന sbi_scratch *remote_scratch = ശൂന്യ;
	ഘടന sse_ipi_inject_data evt = {event_id};
	ഘടന sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_to_scratch(hartid);
	എങ്കിൽ (!remote_scratch)
		തിരികെ SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_offset_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_enqueue(sse_inject_fifo_r, &evt, false);
	എങ്കിൽ (ret)
		തിരികെ SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, ശൂന്യ);
	എങ്കിൽ (ret)
		തിരികെ SBI_EFAIL;

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sse_inject_event(uint32_t event_id, unsigned long hartid)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	എങ്കിൽ (ret)
		തിരികെ ret;

	/* In case of global event, provided hart_id is ignored */
	എങ്കിൽ (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	എങ്കിൽ (hartid != current_hartid()) {
		sse_event_put(e);
		തിരികെ sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_set_pending(e);
	sse_event_put(e);
	എങ്കിൽ (ret)
		തിരികെ ret;

	തിരികെ SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ sse_event_enable(ഘടന sse_hart_state *shs,
			     ഘടന sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	എങ്കിൽ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		തിരികെ SBI_EINVALID_STATE;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_add_to_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	എങ്കിൽ (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  ശൂന്യ);

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sse_event_complete(ഘടന sse_hart_state *shs,
			      ഘടന sbi_sse_event *e,
			      ഘടന sbi_trap_regs *regs,
			      ഘടന sbi_ecall_return *out)
	MUST_HOLD(&shs->enabled_event_lock)
{
	എങ്കിൽ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		തിരികെ SBI_EINVALID_STATE;

	എങ്കിൽ (e->attrs.hartid != current_hartid())
		തിരികെ SBI_EINVAL;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	എങ്കിൽ (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	out->skip_regs_update = true;

	തിരികെ SBI_OK;
}

പൂർണ്ണസംഖ്യ sbi_sse_complete(ഘടന sbi_trap_regs *regs, ഘടന sbi_ecall_return *out)
{
	പൂർണ്ണസംഖ്യ ret = SBI_OK;
	ഘടന sbi_sse_event *tmp;
	ഘടന sse_hart_state *state = sse_thishart_state_ptr();

	spin_lock(&state->enabled_event_lock);
	sbi_list_for_each_entry(tmp, &state->enabled_event_list, കണ്ണി) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		എങ്കിൽ (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, out);
			നിർത്തുക;
		}
	}
	spin_unlock(&state->enabled_event_lock);

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ sbi_sse_enable(uint32_t event_id)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന sbi_sse_event *e;
	ഘടന sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	എങ്കിൽ (ret)
		തിരികെ ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_lock);
	sse_event_put(e);

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ sbi_sse_disable(uint32_t event_id)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന sbi_sse_event *e;
	ഘടന sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	എങ്കിൽ (ret)
		തിരികെ ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_lock);

	sse_event_put(e);

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ sbi_sse_hart_mask(ശൂന്യം)
{
	ഘടന sse_hart_state *state = sse_thishart_state_ptr();

	എങ്കിൽ (!state)
		തിരികെ SBI_EFAIL;

	എങ്കിൽ (state->masked)
		തിരികെ SBI_EALREADY_STOPPED;

	state->masked = true;

	തിരികെ SBI_SUCCESS;
}

പൂർണ്ണസംഖ്യ sbi_sse_hart_unmask(ശൂന്യം)
{
	ഘടന sse_hart_state *state = sse_thishart_state_ptr();

	എങ്കിൽ (!state)
		തിരികെ SBI_EFAIL;

	എങ്കിൽ (!state->masked)
		തിരികെ SBI_EALREADY_STARTED;

	state->masked = false;

	തിരികെ SBI_SUCCESS;
}

പൂർണ്ണസംഖ്യ sbi_sse_inject_from_ecall(uint32_t event_id, unsigned long hartid,
			      ഘടന sbi_ecall_return *out)
{
	എങ്കിൽ (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_to_hartindex(hartid)))
		തിരികെ SBI_EINVAL;

	തിരികെ sse_inject_event(event_id, hartid);
}

പൂർണ്ണസംഖ്യ sbi_sse_inject_event(uint32_t event_id)
{
	തിരികെ sse_inject_event(event_id, current_hartid());
}

പൂർണ്ണസംഖ്യ sbi_sse_add_event(uint32_t event_id, സ്ഥിരം ഘടന sbi_sse_cb_ops *cb_ops)
{
	ഘടന sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	എങ്കിൽ (info)
		തിരികെ SBI_EALREADY;

	എങ്കിൽ (cb_ops && cb_ops->set_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		തിരികെ SBI_EINVAL;

	info = sbi_zalloc(വലുപ്പം(*info));
	എങ്കിൽ (!info)
		തിരികെ SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ADD(info, supported_events);

	തിരികെ SBI_OK;
}

പൂർണ്ണസംഖ്യ sbi_sse_attr_check(uint32_t base_attr_id, uint32_t attr_എണ്ണം,
		       unsigned long phys_lo, unsigned long phys_hi,
		       unsigned long access)
{
	സ്ഥിരം unsigned align = __riscv_xlen >> 3;
	uint64_t end_id = (uint64_t)base_attr_id + (attr_എണ്ണം - 1);

	എങ്കിൽ (attr_എണ്ണം == 0)
		തിരികെ SBI_ERR_INVALID_PARAM;

	എങ്കിൽ (end_id >= SBI_SSE_ATTR_MAX)
		തിരികെ SBI_EBAD_RANGE;

	എങ്കിൽ (phys_lo & (align - 1))
		തിരികെ SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	എങ്കിൽ (phys_hi)
		തിരികെ SBI_EINVALID_ADDR;

	എങ്കിൽ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 വലുപ്പം(unsigned long) * attr_എണ്ണം,
					 PRV_S, access))
		തിരികെ SBI_EINVALID_ADDR;

	തിരികെ SBI_OK;
}

സ്ഥിതം ശൂന്യം copy_attrs(unsigned long *out, സ്ഥിരം unsigned long *in,
		       unsigned പൂർണ്ണസംഖ്യ long_എണ്ണം)
{
	പൂർണ്ണസംഖ്യ i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	വേണ്ടി (i = 0; i < long_എണ്ണം; i++)
		out[i] = in[i];
}

പൂർണ്ണസംഖ്യ sbi_sse_read_attrs(uint32_t event_id, uint32_t base_attr_id,
		       uint32_t attr_എണ്ണം, unsigned long output_phys_lo,
		       unsigned long output_phys_hi)
{
	പൂർണ്ണസംഖ്യ ret;
	unsigned long *e_attrs;
	ഘടന sbi_sse_event *e;
	unsigned long *attrs;

	ret = sbi_sse_attr_check(base_attr_id, attr_എണ്ണം, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	എങ്കിൽ (ret)
		തിരികെ ret;

	ret = sse_event_get(event_id, &e);
	എങ്കിൽ (ret)
		തിരികെ ret;

	sbi_hart_protection_temp_map_range(output_phys_lo, വലുപ്പം(unsigned long) * attr_എണ്ണം);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (unsigned long *)&e->attrs;
	attrs = (unsigned long *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[base_attr_id], attr_എണ്ണം);

	sbi_hart_protection_temp_unmap_range(output_phys_lo, വലുപ്പം(unsigned long) * attr_എണ്ണം);

	sse_event_put(e);

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sse_write_attrs(ഘടന sbi_sse_event *e, uint32_t base_attr_id,
			   uint32_t attr_എണ്ണം, unsigned long നിവേശം_phys)
{
	പൂർണ്ണസംഖ്യ ret = 0;
	unsigned long attr = 0, val;
	uint32_t id, end_id = base_attr_id + attr_എണ്ണം;
	unsigned long *attrs = (unsigned long *)നിവേശം_phys;

	sbi_hart_protection_temp_map_range(നിവേശം_phys, വലുപ്പം(unsigned long) * attr_എണ്ണം);

	വേണ്ടി (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		ret = sse_event_set_attr_check(e, id, val);
		എങ്കിൽ (ret)
			പോകുക out;
	}

	attr = 0;
	വേണ്ടി (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		sse_event_set_attr(e, id, val);
	}

out:
	sbi_hart_protection_temp_unmap_range(നിവേശം_phys, വലുപ്പം(unsigned long) * attr_എണ്ണം);

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ sbi_sse_write_attrs(uint32_t event_id, uint32_t base_attr_id,
			uint32_t attr_എണ്ണം, unsigned long നിവേശം_phys_lo,
			unsigned long നിവേശം_phys_hi)
{
	പൂർണ്ണസംഖ്യ ret = 0;
	ഘടന sbi_sse_event *e;

	ret = sbi_sse_attr_check(base_attr_id, attr_എണ്ണം, നിവേശം_phys_lo,
				 നിവേശം_phys_hi, SBI_DOMAIN_READ);
	എങ്കിൽ (ret)
		തിരികെ ret;

	ret = sse_event_get(event_id, &e);
	എങ്കിൽ (ret)
		തിരികെ ret;

	ret = sse_write_attrs(e, base_attr_id, attr_എണ്ണം, നിവേശം_phys_lo);
	sse_event_put(e);

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ sbi_sse_register(uint32_t event_id, unsigned long handler_entry_pc,
		     unsigned long handler_entry_arg)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന sbi_sse_event *e;

	എങ്കിൽ (handler_entry_pc & 0x1)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_entry_pc,
					 വലുപ്പം(unsigned long), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		തിരികെ SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	എങ്കിൽ (ret)
		തിരികെ ret;

	ret = sse_event_register(e, handler_entry_pc, handler_entry_arg);
	sse_event_put(e);

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ sbi_sse_unregister(uint32_t event_id)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	എങ്കിൽ (ret)
		തിരികെ ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	തിരികെ ret;
}

സ്ഥിതം ശൂന്യം sse_event_init(ഘടന sbi_sse_event *e, ഘടന sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_INJECT_OFFSET);
}

സ്ഥിതം ശൂന്യം sse_event_എണ്ണം_init()
{
	ഘടന sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		എങ്കിൽ (EVENT_IS_GLOBAL(info->event_id))
			global_event_എണ്ണം++;
		അല്ലെങ്കിൽ
			local_event_എണ്ണം++;
	}
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sse_global_init()
{
	ഘടന sbi_sse_event *e;
	unsigned പൂർണ്ണസംഖ്യ ev = 0;
	ഘടന sse_event_info *info;

	global_events = sbi_zalloc(വലുപ്പം(*global_events) * global_event_എണ്ണം);
	എങ്കിൽ (!global_events)
		തിരികെ SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		എങ്കിൽ (!EVENT_IS_GLOBAL(info->event_id))
			തുടരുക;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_LOCK_INIT(global_events[ev].lock);

		ev++;
	}

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം sse_local_init(ഘടന sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	unsigned പൂർണ്ണസംഖ്യ ev = 0;
	ഘടന sse_event_info *info;

	SBI_INIT_LIST_HEAD(&shs->enabled_event_list);
	SPIN_LOCK_INIT(shs->enabled_event_lock);

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		എങ്കിൽ (EVENT_IS_GLOBAL(info->event_id))
			തുടരുക;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

പൂർണ്ണസംഖ്യ sbi_sse_init(ഘടന sbi_scratch *scratch, bool cold_boot)
{
	പൂർണ്ണസംഖ്യ ret;
	ശൂന്യം *sse_inject_mem;
	ഘടന sse_hart_state *shs;
	ഘടന sbi_fifo *sse_inject_q;

	എങ്കിൽ (cold_boot) {
		sse_event_എണ്ണം_init();

		ret = sse_global_init();
		എങ്കിൽ (ret)
			തിരികെ ret;

		shs_ptr_off = sbi_scratch_alloc_offset(വലുപ്പം(ശൂന്യം *));
		എങ്കിൽ (!shs_ptr_off)
			തിരികെ SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_offset(വലുപ്പം(*sse_inject_q));
		എങ്കിൽ (!sse_inject_fifo_off) {
			sbi_scratch_free_offset(shs_ptr_off);
			തിരികെ SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_offset(
			(global_event_എണ്ണം + local_event_എണ്ണം) *
			വലുപ്പം(ഘടന sse_ipi_inject_data));
		എങ്കിൽ (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_offset(sse_inject_fifo_off);
			sbi_scratch_free_offset(shs_ptr_off);
			തിരികെ SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		എങ്കിൽ (ret < 0) {
			sbi_scratch_free_offset(shs_ptr_off);
			തിരികെ ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	എങ്കിൽ (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(വലുപ്പം(*shs) + വലുപ്പം(ഘടന sbi_sse_event) *
							local_event_എണ്ണം);
		എങ്കിൽ (!shs)
			തിരികെ SBI_ENOMEM;

		shs->local_events = (ഘടന sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_set_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_എണ്ണം + local_event_എണ്ണം),
		      വലുപ്പം(ഘടന sse_ipi_inject_data));

	തിരികെ 0;
}

ശൂന്യം sbi_sse_exit(ഘടന sbi_scratch *scratch)
{
	ഘടന sbi_sse_event *e;
	ഘടന sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		എങ്കിൽ (sse_event_get(info->event_id, &e))
			തുടരുക;

		എങ്കിൽ (e->attrs.hartid != current_hartid())
			പോകുക skip;

		എങ്കിൽ (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

skip:
		sse_event_put(e);
	}
}
