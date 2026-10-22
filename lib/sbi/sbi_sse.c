/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Rivos Systems Inc.
 *
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_barrier.h>
#समावेश <sbi/riscv_encoding.h>
#समावेश <sbi/riscv_locks.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_fifo.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_hart_protection.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <sbi/sbi_ipi.h>
#समावेश <sbi/sbi_list.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_pmu.h>
#समावेश <sbi/sbi_sse.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_slist.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_trap.h>

#समावेश <sbi/sbi_console.h>

#घोषणा sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), व्योम *, shs_ptr_off)

#घोषणा sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#घोषणा sse_set_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), व्योम *, shs_ptr_off, (__sse_state))

#घोषणा EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_BIT)

#घोषणा sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		स्थिर काष्ठा sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		अगर (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

काष्ठा sse_entry_state {
	/** entry pc */
	अचिन्हित दीर्घ pc;
	/** a6 register state */
	अचिन्हित दीर्घ arg;
};

काष्ठा sse_interrupted_state {
	/** sepc register state */
	अचिन्हित दीर्घ sepc;
	/** flags register state */
	अचिन्हित दीर्घ flags;
	/** a6 register state */
	अचिन्हित दीर्घ a6;
	/** a7 register state */
	अचिन्हित दीर्घ a7;
};

काष्ठा sse_ipi_inject_तथ्य {
	uint32_t event_id;
};

काष्ठा sbi_sse_event_attrs {
	अचिन्हित दीर्घ status;
	अचिन्हित दीर्घ prio;
	अचिन्हित दीर्घ config;
	अचिन्हित दीर्घ hartid;
	काष्ठा sse_entry_state entry;
	काष्ठा sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#घोषणा assert_field_offset(field, attr_offset)                  \
	_Static_assert(                                          \
		((दुरत्व(काष्ठा sbi_sse_event_attrs, field)) / \
		 माप(अचिन्हित दीर्घ)) == attr_offset,          \
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

काष्ठा sbi_sse_event {
	काष्ठा sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	काष्ठा sse_event_info *info;
	काष्ठा sbi_dlist गाँठ;
};

/** Per-hart state */
काष्ठा sse_hart_state {
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
	काष्ठा sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_lock);

	/**
	 * List of local events allocated at boot time.
	 */
	काष्ठा sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
काष्ठा sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t lock;

	/**
	 * global event struct
	 */
	काष्ठा sbi_sse_event event GUARDED_BY(&lock);
};

काष्ठा sse_event_info {
	uint32_t event_id;
	स्थिर काष्ठा sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_गाँठ(sse_event_info);
};

अटल अचिन्हित पूर्णांक local_event_गिनती;
अटल अचिन्हित पूर्णांक global_event_गिनती;
अटल काष्ठा sse_global_event *global_events;

अटल अचिन्हित दीर्घ sse_inject_fifo_off;
अटल अचिन्हित दीर्घ sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
अटल अचिन्हित दीर्घ shs_ptr_off;

अटल u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

अटल पूर्णांक sse_ipi_inject_send(अचिन्हित दीर्घ hartid, uint32_t event_id);

काष्ठा sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_गाँठ_INIT(शून्य),
};

काष्ठा sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_गाँठ_INIT(&global_software_event),
};

अटल SBI_SLIST_शीर्ष(supported_events, sse_event_info) =
				SBI_SLIST_शीर्ष_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
अटल स्थिर uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_ऊपरी_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_ऊपरी_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW,
	SBI_SSE_EVENT_LOCAL_निचला_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_निचला_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

अटल bool sse_is_standard_event(uint32_t event_id)
{
	पूर्णांक i;

	क्रम (i = 0; i < array_आकार(standard_events); i++) {
		अगर (event_id == standard_events[i])
			वापस true;
	}

	वापस false;
}

अटल काष्ठा sse_event_info *sse_event_info_get(uint32_t event_id)
{
	काष्ठा sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		अगर (info->event_id == event_id)
			वापस info;
	}

	वापस शून्य;
}

अटल अचिन्हित दीर्घ sse_event_state(काष्ठा sbi_sse_event *e)
{
	वापस e->attrs.status & SBI_SSE_ATTR_STATUS_STATE_MASK;
}

अटल अचिन्हित दीर्घ sse_event_pending(काष्ठा sbi_sse_event *e)
{
	वापस !!(e->attrs.status & BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET));
}

अटल bool sse_event_is_global(काष्ठा sbi_sse_event *e)
{
	वापस EVENT_IS_GLOBAL(e->event_id);
}

अटल bool sse_event_is_local(काष्ठा sbi_sse_event *e)
{
	वापस !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
अटल काष्ठा sse_hart_state *sse_get_hart_state(काष्ठा sbi_sse_event *e)
{
	काष्ठा sbi_scratch *s = sbi_hartindex_to_scratch(e->hartindex);

	वापस sse_get_hart_state_ptr(s);
}

अटल काष्ठा sse_global_event *sse_get_global_event(काष्ठा sbi_sse_event *e)
{
	वापस container_of(e, काष्ठा sse_global_event, event);
}

अटल व्योम sse_event_set_state(काष्ठा sbi_sse_event *e,
				अचिन्हित दीर्घ new_state)
{
	e->attrs.status &= ~SBI_SSE_ATTR_STATUS_STATE_MASK;
	e->attrs.status |= new_state;
}

अटल पूर्णांक sse_event_get(uint32_t event_id,
			 काष्ठा sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	अचिन्हित पूर्णांक i;
	काष्ठा sbi_sse_event *e;
	काष्ठा sse_hart_state *shs;

	अगर (!eret)
		वापस SBI_EINVAL;

	अगर (EVENT_IS_GLOBAL(event_id)) {
		क्रम (i = 0; i < global_event_गिनती; i++) {
			e = &global_events[i].event;
			अगर (e->event_id == event_id) {
				spin_lock(&global_events[i].lock);
				*eret = e;
				वापस SBI_SUCCESS;
			}
		}
	} अन्यथा {
		shs = sse_thishart_state_ptr();
		क्रम (i = 0; i < local_event_गिनती; i++) {
			e = &shs->local_events[i];
			अगर (e->event_id == event_id) {
				*eret = e;
				वापस SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	अगर (sse_is_standard_event(event_id))
		वापस SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	वापस SBI_EINVAL;
}

अटल व्योम sse_event_put(काष्ठा sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	काष्ठा sse_global_event *ge;

	अगर (sse_event_is_local(e))
		वापस;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->lock);
}

अटल व्योम sse_event_remove_from_list(काष्ठा sbi_sse_event *e)
{
	sbi_list_del(&e->गाँठ);
}

/**
 * Must be called under owner hart lock
 */
अटल व्योम sse_event_add_to_list(काष्ठा sse_hart_state *state,
				   काष्ठा sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_lock)
{
	काष्ठा sbi_sse_event *tmp;

	sbi_list_for_each_entry(tmp, &state->enabled_event_list, गाँठ) {
		अगर (e->attrs.prio < tmp->attrs.prio)
			अवरोध;
		अगर (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			अवरोध;
	}
	sbi_list_add_पुच्छ(&e->गाँठ, &tmp->गाँठ);
}

/**
 * Must be called under owner hart lock
 */
अटल पूर्णांक sse_event_disable(काष्ठा sse_hart_state *shs,
			      काष्ठा sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	अगर (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		वापस SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	वापस SBI_OK;
}

अटल पूर्णांक sse_event_set_hart_id_check(काष्ठा sbi_sse_event *e,
				       अचिन्हित दीर्घ new_hartid)
{
	पूर्णांक hstate;
	अचिन्हित पूर्णांक hartid = (uint32_t)new_hartid;
	काष्ठा sbi_domain *hd = sbi_domain_thishart_ptr();

	अगर (!sse_event_is_global(e))
		वापस SBI_EDENIED;

	अगर (!sbi_domain_is_assigned_hart(hd, sbi_hartid_to_hartindex(hartid)))
		वापस SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	अगर (hstate != SBI_HSM_STATE_STARTED)
		वापस SBI_EINVAL;

	वापस SBI_OK;
}

अटल पूर्णांक sse_event_set_attr_check(काष्ठा sbi_sse_event *e, uint32_t attr_id,
				    अचिन्हित दीर्घ val)
{
	चयन (attr_id) {
	हाल SBI_SSE_ATTR_CONFIG:
		अगर (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			वापस SBI_EINVALID_STATE;

		अगर (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			वापस SBI_EINVAL;

		वापस SBI_OK;
	हाल SBI_SSE_ATTR_PRIO:
		अगर (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			वापस SBI_EINVALID_STATE;

#अगर __riscv_xlen > 32
		अगर (val != (uint32_t)val)
			वापस SBI_EINVAL;
#पूर्ण_अगर
		वापस SBI_OK;
	हाल SBI_SSE_ATTR_PREFERRED_HART:
		अगर (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			वापस SBI_EINVALID_STATE;

		वापस sse_event_set_hart_id_check(e, val);
	हाल SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		अगर (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			वापस SBI_EINVAL;
		__attribute__((__fallthrough__));
	हाल SBI_SSE_ATTR_INTERRUPTED_SEPC:
	हाल SBI_SSE_ATTR_INTERRUPTED_A6:
	हाल SBI_SSE_ATTR_INTERRUPTED_A7:
		अगर (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			वापस SBI_EINVALID_STATE;

		अगर (current_hartid() != e->attrs.hartid)
			वापस SBI_EINVAL;

		वापस SBI_OK;
	शेष:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		वापस SBI_EDENIED;
	}
}

अटल व्योम sse_event_set_attr(काष्ठा sbi_sse_event *e, uint32_t attr_id,
			       अचिन्हित दीर्घ val)
{
	चयन (attr_id) {
	हाल SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		अवरोध;
	हाल SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		अवरोध;
	हाल SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_to_hartindex(val);
		sse_event_invoke_cb(e, set_hartid_cb, val);
		अवरोध;

	हाल SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		अवरोध;
	हाल SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		अवरोध;
	हाल SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		अवरोध;
	हाल SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		अवरोध;
	}
}

अटल पूर्णांक sse_event_register(काष्ठा sbi_sse_event *e,
			      अचिन्हित दीर्घ handler_entry_pc,
			      अचिन्हित दीर्घ handler_entry_arg)
{
	अगर (sse_event_state(e) != SBI_SSE_STATE_UNUSED)
		वापस SBI_EINVALID_STATE;

	e->attrs.entry.pc = handler_entry_pc;
	e->attrs.entry.arg = handler_entry_arg;

	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, register_cb);

	वापस 0;
}

अटल पूर्णांक sse_event_unregister(काष्ठा sbi_sse_event *e)
{
	अगर (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		वापस SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

	वापस 0;
}

अटल अचिन्हित दीर्घ sse_interrupted_flags(अचिन्हित दीर्घ mstatus)
{
	अचिन्हित दीर्घ hstatus, flags = 0;

	अगर (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	अगर (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	अगर (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	अगर (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	अगर (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		अगर (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		अगर (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	वापस flags;
}

अटल व्योम sse_event_inject(काष्ठा sbi_sse_event *e,
			     काष्ठा sbi_trap_regs *regs)
{
	काष्ठा sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_set_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.status &= ~BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	अगर (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	अगर (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	अगर (misa_extension('H')) {
		अचिन्हित दीर्घ hstatus = csr_read(CSR_HSTATUS);
		अगर (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		अन्यथा
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		अगर (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
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

#अगर __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#या_अगर __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#अन्यथा
#त्रुटि "Unexpected __riscv_xlen"
#पूर्ण_अगर

}

अटल व्योम sse_event_resume(काष्ठा sbi_sse_event *e,
			     काष्ठा sbi_trap_regs *regs)
{
	काष्ठा sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	अगर (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

	अगर (misa_extension('H')) {
		अचिन्हित दीर्घ hstatus = csr_read(CSR_HSTATUS);
#अगर __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		अगर (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#या_अगर __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		अगर (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#अन्यथा
#त्रुटि "Unexpected __riscv_xlen"
#पूर्ण_अगर
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		अगर (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		अगर (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	अगर (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	अगर (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	अगर (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	अगर (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	अगर (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

अटल bool sse_event_is_ready(काष्ठा sbi_sse_event *e)
{
	अगर (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		वापस false;
	}

	वापस true;
}

अटल bool sse_event_check_inject(काष्ठा sbi_sse_event *e,
				   काष्ठा sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	अगर (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		वापस true;
	}

	अगर (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		वापस true;
	}

	वापस false;
}

/* Return true if an event has been injected, false otherwise */
व्योम sbi_sse_process_pending_events(काष्ठा sbi_trap_regs *regs)
{
	bool ret;
	काष्ठा sbi_sse_event *e;
	काष्ठा sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	अगर (state->masked)
		वापस;

	spin_lock(&state->enabled_event_lock);

	sbi_list_for_each_entry(e, &state->enabled_event_list, गाँठ) {
		ret = sse_event_check_inject(e, regs);
		अगर (ret)
			जाओ out;
	}

out:
	spin_unlock(&state->enabled_event_lock);
}

अटल पूर्णांक sse_event_set_pending(काष्ठा sbi_sse_event *e)
{
	अगर (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		वापस SBI_EINVALID_STATE;

	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	वापस SBI_OK;
}

अटल व्योम sse_ipi_inject_process(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_sse_event *e;
	काष्ठा sse_ipi_inject_तथ्य evt;
	काष्ठा sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	जबतक (!sbi_fifo_dequeue(sse_inject_fifo_r, &evt)) {
		अगर (sse_event_get(evt.event_id, &e))
			जारी;

		sse_event_set_pending(e);
		sse_event_put(e);
	}
}

अटल काष्ठा sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

अटल पूर्णांक sse_ipi_inject_send(अचिन्हित दीर्घ hartid, uint32_t event_id)
{
	पूर्णांक ret;
	काष्ठा sbi_scratch *remote_scratch = शून्य;
	काष्ठा sse_ipi_inject_तथ्य evt = {event_id};
	काष्ठा sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_to_scratch(hartid);
	अगर (!remote_scratch)
		वापस SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_offset_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_enqueue(sse_inject_fifo_r, &evt, false);
	अगर (ret)
		वापस SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, शून्य);
	अगर (ret)
		वापस SBI_EFAIL;

	वापस SBI_OK;
}

अटल पूर्णांक sse_inject_event(uint32_t event_id, अचिन्हित दीर्घ hartid)
{
	पूर्णांक ret;
	काष्ठा sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	अगर (ret)
		वापस ret;

	/* In case of global event, provided hart_id is ignored */
	अगर (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	अगर (hartid != current_hartid()) {
		sse_event_put(e);
		वापस sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_set_pending(e);
	sse_event_put(e);
	अगर (ret)
		वापस ret;

	वापस SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
अटल पूर्णांक sse_event_enable(काष्ठा sse_hart_state *shs,
			     काष्ठा sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	अगर (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		वापस SBI_EINVALID_STATE;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_add_to_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	अगर (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  शून्य);

	वापस SBI_OK;
}

अटल पूर्णांक sse_event_complete(काष्ठा sse_hart_state *shs,
			      काष्ठा sbi_sse_event *e,
			      काष्ठा sbi_trap_regs *regs,
			      काष्ठा sbi_ecall_return *out)
	MUST_HOLD(&shs->enabled_event_lock)
{
	अगर (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		वापस SBI_EINVALID_STATE;

	अगर (e->attrs.hartid != current_hartid())
		वापस SBI_EINVAL;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	अगर (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	out->skip_regs_update = true;

	वापस SBI_OK;
}

पूर्णांक sbi_sse_complete(काष्ठा sbi_trap_regs *regs, काष्ठा sbi_ecall_return *out)
{
	पूर्णांक ret = SBI_OK;
	काष्ठा sbi_sse_event *tmp;
	काष्ठा sse_hart_state *state = sse_thishart_state_ptr();

	spin_lock(&state->enabled_event_lock);
	sbi_list_for_each_entry(tmp, &state->enabled_event_list, गाँठ) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		अगर (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, out);
			अवरोध;
		}
	}
	spin_unlock(&state->enabled_event_lock);

	वापस ret;
}

पूर्णांक sbi_sse_enable(uint32_t event_id)
{
	पूर्णांक ret;
	काष्ठा sbi_sse_event *e;
	काष्ठा sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	अगर (ret)
		वापस ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_lock);
	sse_event_put(e);

	वापस ret;
}

पूर्णांक sbi_sse_disable(uint32_t event_id)
{
	पूर्णांक ret;
	काष्ठा sbi_sse_event *e;
	काष्ठा sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	अगर (ret)
		वापस ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_lock);

	sse_event_put(e);

	वापस ret;
}

पूर्णांक sbi_sse_hart_mask(व्योम)
{
	काष्ठा sse_hart_state *state = sse_thishart_state_ptr();

	अगर (!state)
		वापस SBI_EFAIL;

	अगर (state->masked)
		वापस SBI_EALREADY_STOPPED;

	state->masked = true;

	वापस SBI_SUCCESS;
}

पूर्णांक sbi_sse_hart_unmask(व्योम)
{
	काष्ठा sse_hart_state *state = sse_thishart_state_ptr();

	अगर (!state)
		वापस SBI_EFAIL;

	अगर (!state->masked)
		वापस SBI_EALREADY_STARTED;

	state->masked = false;

	वापस SBI_SUCCESS;
}

पूर्णांक sbi_sse_inject_from_ecall(uint32_t event_id, अचिन्हित दीर्घ hartid,
			      काष्ठा sbi_ecall_return *out)
{
	अगर (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_to_hartindex(hartid)))
		वापस SBI_EINVAL;

	वापस sse_inject_event(event_id, hartid);
}

पूर्णांक sbi_sse_inject_event(uint32_t event_id)
{
	वापस sse_inject_event(event_id, current_hartid());
}

पूर्णांक sbi_sse_add_event(uint32_t event_id, स्थिर काष्ठा sbi_sse_cb_ops *cb_ops)
{
	काष्ठा sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	अगर (info)
		वापस SBI_EALREADY;

	अगर (cb_ops && cb_ops->set_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		वापस SBI_EINVAL;

	info = sbi_zalloc(माप(*info));
	अगर (!info)
		वापस SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ADD(info, supported_events);

	वापस SBI_OK;
}

पूर्णांक sbi_sse_attr_check(uint32_t base_attr_id, uint32_t attr_गिनती,
		       अचिन्हित दीर्घ phys_lo, अचिन्हित दीर्घ phys_hi,
		       अचिन्हित दीर्घ access)
{
	स्थिर अचिन्हित align = __riscv_xlen >> 3;
	uint64_t end_id = (uint64_t)base_attr_id + (attr_गिनती - 1);

	अगर (attr_गिनती == 0)
		वापस SBI_ERR_INVALID_PARAM;

	अगर (end_id >= SBI_SSE_ATTR_MAX)
		वापस SBI_EBAD_RANGE;

	अगर (phys_lo & (align - 1))
		वापस SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	अगर (phys_hi)
		वापस SBI_EINVALID_ADDR;

	अगर (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 माप(अचिन्हित दीर्घ) * attr_गिनती,
					 PRV_S, access))
		वापस SBI_EINVALID_ADDR;

	वापस SBI_OK;
}

अटल व्योम copy_attrs(अचिन्हित दीर्घ *out, स्थिर अचिन्हित दीर्घ *in,
		       अचिन्हित पूर्णांक long_गिनती)
{
	पूर्णांक i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	क्रम (i = 0; i < long_गिनती; i++)
		out[i] = in[i];
}

पूर्णांक sbi_sse_read_attrs(uint32_t event_id, uint32_t base_attr_id,
		       uint32_t attr_गिनती, अचिन्हित दीर्घ output_phys_lo,
		       अचिन्हित दीर्घ output_phys_hi)
{
	पूर्णांक ret;
	अचिन्हित दीर्घ *e_attrs;
	काष्ठा sbi_sse_event *e;
	अचिन्हित दीर्घ *attrs;

	ret = sbi_sse_attr_check(base_attr_id, attr_गिनती, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	अगर (ret)
		वापस ret;

	ret = sse_event_get(event_id, &e);
	अगर (ret)
		वापस ret;

	sbi_hart_protection_अस्थायी_map_range(output_phys_lo, माप(अचिन्हित दीर्घ) * attr_गिनती);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (अचिन्हित दीर्घ *)&e->attrs;
	attrs = (अचिन्हित दीर्घ *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[base_attr_id], attr_गिनती);

	sbi_hart_protection_अस्थायी_unmap_range(output_phys_lo, माप(अचिन्हित दीर्घ) * attr_गिनती);

	sse_event_put(e);

	वापस SBI_OK;
}

अटल पूर्णांक sse_write_attrs(काष्ठा sbi_sse_event *e, uint32_t base_attr_id,
			   uint32_t attr_गिनती, अचिन्हित दीर्घ निवेश_phys)
{
	पूर्णांक ret = 0;
	अचिन्हित दीर्घ attr = 0, val;
	uint32_t id, end_id = base_attr_id + attr_गिनती;
	अचिन्हित दीर्घ *attrs = (अचिन्हित दीर्घ *)निवेश_phys;

	sbi_hart_protection_अस्थायी_map_range(निवेश_phys, माप(अचिन्हित दीर्घ) * attr_गिनती);

	क्रम (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		ret = sse_event_set_attr_check(e, id, val);
		अगर (ret)
			जाओ out;
	}

	attr = 0;
	क्रम (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		sse_event_set_attr(e, id, val);
	}

out:
	sbi_hart_protection_अस्थायी_unmap_range(निवेश_phys, माप(अचिन्हित दीर्घ) * attr_गिनती);

	वापस ret;
}

पूर्णांक sbi_sse_write_attrs(uint32_t event_id, uint32_t base_attr_id,
			uint32_t attr_गिनती, अचिन्हित दीर्घ निवेश_phys_lo,
			अचिन्हित दीर्घ निवेश_phys_hi)
{
	पूर्णांक ret = 0;
	काष्ठा sbi_sse_event *e;

	ret = sbi_sse_attr_check(base_attr_id, attr_गिनती, निवेश_phys_lo,
				 निवेश_phys_hi, SBI_DOMAIN_READ);
	अगर (ret)
		वापस ret;

	ret = sse_event_get(event_id, &e);
	अगर (ret)
		वापस ret;

	ret = sse_write_attrs(e, base_attr_id, attr_गिनती, निवेश_phys_lo);
	sse_event_put(e);

	वापस ret;
}

पूर्णांक sbi_sse_register(uint32_t event_id, अचिन्हित दीर्घ handler_entry_pc,
		     अचिन्हित दीर्घ handler_entry_arg)
{
	पूर्णांक ret;
	काष्ठा sbi_sse_event *e;

	अगर (handler_entry_pc & 0x1)
		वापस SBI_EINVAL;

	अगर (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_entry_pc,
					 माप(अचिन्हित दीर्घ), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		वापस SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	अगर (ret)
		वापस ret;

	ret = sse_event_register(e, handler_entry_pc, handler_entry_arg);
	sse_event_put(e);

	वापस ret;
}

पूर्णांक sbi_sse_unregister(uint32_t event_id)
{
	पूर्णांक ret;
	काष्ठा sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	अगर (ret)
		वापस ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	वापस ret;
}

अटल व्योम sse_event_init(काष्ठा sbi_sse_event *e, काष्ठा sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_INJECT_OFFSET);
}

अटल व्योम sse_event_गिनती_init()
{
	काष्ठा sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		अगर (EVENT_IS_GLOBAL(info->event_id))
			global_event_गिनती++;
		अन्यथा
			local_event_गिनती++;
	}
}

अटल पूर्णांक sse_global_init()
{
	काष्ठा sbi_sse_event *e;
	अचिन्हित पूर्णांक ev = 0;
	काष्ठा sse_event_info *info;

	global_events = sbi_zalloc(माप(*global_events) * global_event_गिनती);
	अगर (!global_events)
		वापस SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		अगर (!EVENT_IS_GLOBAL(info->event_id))
			जारी;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_LOCK_INIT(global_events[ev].lock);

		ev++;
	}

	वापस 0;
}

अटल व्योम sse_local_init(काष्ठा sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	अचिन्हित पूर्णांक ev = 0;
	काष्ठा sse_event_info *info;

	SBI_INIT_LIST_शीर्ष(&shs->enabled_event_list);
	SPIN_LOCK_INIT(shs->enabled_event_lock);

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		अगर (EVENT_IS_GLOBAL(info->event_id))
			जारी;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

पूर्णांक sbi_sse_init(काष्ठा sbi_scratch *scratch, bool cold_boot)
{
	पूर्णांक ret;
	व्योम *sse_inject_mem;
	काष्ठा sse_hart_state *shs;
	काष्ठा sbi_fifo *sse_inject_q;

	अगर (cold_boot) {
		sse_event_गिनती_init();

		ret = sse_global_init();
		अगर (ret)
			वापस ret;

		shs_ptr_off = sbi_scratch_alloc_offset(माप(व्योम *));
		अगर (!shs_ptr_off)
			वापस SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_offset(माप(*sse_inject_q));
		अगर (!sse_inject_fifo_off) {
			sbi_scratch_free_offset(shs_ptr_off);
			वापस SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_offset(
			(global_event_गिनती + local_event_गिनती) *
			माप(काष्ठा sse_ipi_inject_तथ्य));
		अगर (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_offset(sse_inject_fifo_off);
			sbi_scratch_free_offset(shs_ptr_off);
			वापस SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		अगर (ret < 0) {
			sbi_scratch_free_offset(shs_ptr_off);
			वापस ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	अगर (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(माप(*shs) + माप(काष्ठा sbi_sse_event) *
							local_event_गिनती);
		अगर (!shs)
			वापस SBI_ENOMEM;

		shs->local_events = (काष्ठा sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_set_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_गिनती + local_event_गिनती),
		      माप(काष्ठा sse_ipi_inject_तथ्य));

	वापस 0;
}

व्योम sbi_sse_exit(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_sse_event *e;
	काष्ठा sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		अगर (sse_event_get(info->event_id, &e))
			जारी;

		अगर (e->attrs.hartid != current_hartid())
			जाओ skip;

		अगर (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

skip:
		sse_event_put(e);
	}
}
