/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Rivos Systems Inc.
 *
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_barrier.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/riscv_locks.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_fifo.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_hart_protection.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_hsm.h>
#अन्तर्भाव <sbi/sbi_ipi.h>
#अन्तर्भाव <sbi/sbi_list.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_pmu.h>
#अन्तर्भाव <sbi/sbi_sse.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_slist.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_trap.h>

#अन्तर्भाव <sbi/sbi_console.h>

#define sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), शून्य *, shs_ptr_off)

#define sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define sse_set_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), शून्य *, shs_ptr_off, (__sse_state))

#define EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_BIT)

#define sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		ध्रुव संरचना sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		सचे (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

संरचना sse_entry_state {
	/** entry pc */
	unsigned long pc;
	/** a6 register state */
	unsigned long arg;
};

संरचना sse_interrupted_state {
	/** sepc register state */
	unsigned long sepc;
	/** flags register state */
	unsigned long flags;
	/** a6 register state */
	unsigned long a6;
	/** a7 register state */
	unsigned long a7;
};

संरचना sse_ipi_inject_data {
	uint32_t event_id;
};

संरचना sbi_sse_event_attrs {
	unsigned long status;
	unsigned long prio;
	unsigned long config;
	unsigned long hartid;
	संरचना sse_entry_state entry;
	संरचना sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)                  \
	_Static_assert(                                          \
		((offsetof(संरचना sbi_sse_event_attrs, field)) / \
		 परिमाण(unsigned long)) == attr_offset,          \
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

संरचना sbi_sse_event {
	संरचना sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	संरचना sse_event_info *info;
	संरचना sbi_dlist ग्रन्थिः;
};

/** Per-hart state */
संरचना sse_hart_state {
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
	संरचना sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_lock);

	/**
	 * List of local events allocated at boot time.
	 */
	संरचना sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
संरचना sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t lock;

	/**
	 * global event struct
	 */
	संरचना sbi_sse_event event GUARDED_BY(&lock);
};

संरचना sse_event_info {
	uint32_t event_id;
	ध्रुव संरचना sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_ग्रन्थिः(sse_event_info);
};

स्थिर unsigned पूर्णाङ्क local_event_गणनम्;
स्थिर unsigned पूर्णाङ्क global_event_गणनम्;
स्थिर संरचना sse_global_event *global_events;

स्थिर unsigned long sse_inject_fifo_off;
स्थिर unsigned long sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
स्थिर unsigned long shs_ptr_off;

स्थिर u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

स्थिर पूर्णाङ्क sse_ipi_inject_send(unsigned long hartid, uint32_t event_id);

संरचना sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_ग्रन्थिः_INIT(रिक्त),
};

संरचना sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_ग्रन्थिः_INIT(&global_software_event),
};

स्थिर SBI_SLIST_HEAD(supported_events, sse_event_info) =
				SBI_SLIST_HEAD_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
स्थिर ध्रुव uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_उच्चः_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_उच्चः_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW,
	SBI_SSE_EVENT_LOCAL_नीचः_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_नीचः_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

स्थिर bool sse_is_standard_event(uint32_t event_id)
{
	पूर्णाङ्क i;

	अत्थाय (i = 0; i < array_size(standard_events); i++) {
		सचे (event_id == standard_events[i])
			पटिनिवत्त true;
	}

	पटिनिवत्त false;
}

स्थिर संरचना sse_event_info *sse_event_info_get(uint32_t event_id)
{
	संरचना sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		सचे (info->event_id == event_id)
			पटिनिवत्त info;
	}

	पटिनिवत्त रिक्त;
}

स्थिर unsigned long sse_event_state(संरचना sbi_sse_event *e)
{
	पटिनिवत्त e->attrs.status & SBI_SSE_ATTR_STATUS_STATE_MASK;
}

स्थिर unsigned long sse_event_pending(संरचना sbi_sse_event *e)
{
	पटिनिवत्त !!(e->attrs.status & BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET));
}

स्थिर bool sse_event_is_global(संरचना sbi_sse_event *e)
{
	पटिनिवत्त EVENT_IS_GLOBAL(e->event_id);
}

स्थिर bool sse_event_is_local(संरचना sbi_sse_event *e)
{
	पटिनिवत्त !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
स्थिर संरचना sse_hart_state *sse_get_hart_state(संरचना sbi_sse_event *e)
{
	संरचना sbi_scratch *s = sbi_hartindex_to_scratch(e->hartindex);

	पटिनिवत्त sse_get_hart_state_ptr(s);
}

स्थिर संरचना sse_global_event *sse_get_global_event(संरचना sbi_sse_event *e)
{
	पटिनिवत्त container_of(e, संरचना sse_global_event, event);
}

स्थिर शून्य sse_event_set_state(संरचना sbi_sse_event *e,
				unsigned long new_state)
{
	e->attrs.status &= ~SBI_SSE_ATTR_STATUS_STATE_MASK;
	e->attrs.status |= new_state;
}

स्थिर पूर्णाङ्क sse_event_get(uint32_t event_id,
			 संरचना sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	unsigned पूर्णाङ्क i;
	संरचना sbi_sse_event *e;
	संरचना sse_hart_state *shs;

	सचे (!eret)
		पटिनिवत्त SBI_EINVAL;

	सचे (EVENT_IS_GLOBAL(event_id)) {
		अत्थाय (i = 0; i < global_event_गणनम्; i++) {
			e = &global_events[i].event;
			सचे (e->event_id == event_id) {
				spin_lock(&global_events[i].lock);
				*eret = e;
				पटिनिवत्त SBI_SUCCESS;
			}
		}
	} अञ्ञथा {
		shs = sse_thishart_state_ptr();
		अत्थाय (i = 0; i < local_event_गणनम्; i++) {
			e = &shs->local_events[i];
			सचे (e->event_id == event_id) {
				*eret = e;
				पटिनिवत्त SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	सचे (sse_is_standard_event(event_id))
		पटिनिवत्त SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	पटिनिवत्त SBI_EINVAL;
}

स्थिर शून्य sse_event_put(संरचना sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	संरचना sse_global_event *ge;

	सचे (sse_event_is_local(e))
		पटिनिवत्त;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->lock);
}

स्थिर शून्य sse_event_remove_from_list(संरचना sbi_sse_event *e)
{
	sbi_list_del(&e->ग्रन्थिः);
}

/**
 * Must be called under owner hart lock
 */
स्थिर शून्य sse_event_add_to_list(संरचना sse_hart_state *state,
				   संरचना sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_lock)
{
	संरचना sbi_sse_event *tmp;

	sbi_list_for_each_entry(tmp, &state->enabled_event_list, ग्रन्थिः) {
		सचे (e->attrs.prio < tmp->attrs.prio)
			भङ्ग;
		सचे (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			भङ्ग;
	}
	sbi_list_add_tail(&e->ग्रन्थिः, &tmp->ग्रन्थिः);
}

/**
 * Must be called under owner hart lock
 */
स्थिर पूर्णाङ्क sse_event_disable(संरचना sse_hart_state *shs,
			      संरचना sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	सचे (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		पटिनिवत्त SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क sse_event_set_hart_id_check(संरचना sbi_sse_event *e,
				       unsigned long new_hartid)
{
	पूर्णाङ्क hstate;
	unsigned पूर्णाङ्क hartid = (uint32_t)new_hartid;
	संरचना sbi_domain *hd = sbi_domain_thishart_ptr();

	सचे (!sse_event_is_global(e))
		पटिनिवत्त SBI_EDENIED;

	सचे (!sbi_domain_is_assigned_hart(hd, sbi_hartid_to_hartindex(hartid)))
		पटिनिवत्त SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	सचे (hstate != SBI_HSM_STATE_STARTED)
		पटिनिवत्त SBI_EINVAL;

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क sse_event_set_attr_check(संरचना sbi_sse_event *e, uint32_t attr_id,
				    unsigned long val)
{
	विकल्प (attr_id) {
	पक्ष SBI_SSE_ATTR_CONFIG:
		सचे (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			पटिनिवत्त SBI_EINVALID_STATE;

		सचे (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			पटिनिवत्त SBI_EINVAL;

		पटिनिवत्त SBI_OK;
	पक्ष SBI_SSE_ATTR_PRIO:
		सचे (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			पटिनिवत्त SBI_EINVALID_STATE;

#सचे __riscv_xlen > 32
		सचे (val != (uint32_t)val)
			पटिनिवत्त SBI_EINVAL;
#endif
		पटिनिवत्त SBI_OK;
	पक्ष SBI_SSE_ATTR_PREFERRED_HART:
		सचे (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			पटिनिवत्त SBI_EINVALID_STATE;

		पटिनिवत्त sse_event_set_hart_id_check(e, val);
	पक्ष SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		सचे (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			पटिनिवत्त SBI_EINVAL;
		__attribute__((__fallthrough__));
	पक्ष SBI_SSE_ATTR_INTERRUPTED_SEPC:
	पक्ष SBI_SSE_ATTR_INTERRUPTED_A6:
	पक्ष SBI_SSE_ATTR_INTERRUPTED_A7:
		सचे (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			पटिनिवत्त SBI_EINVALID_STATE;

		सचे (current_hartid() != e->attrs.hartid)
			पटिनिवत्त SBI_EINVAL;

		पटिनिवत्त SBI_OK;
	स्वभाव:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		पटिनिवत्त SBI_EDENIED;
	}
}

स्थिर शून्य sse_event_set_attr(संरचना sbi_sse_event *e, uint32_t attr_id,
			       unsigned long val)
{
	विकल्प (attr_id) {
	पक्ष SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		भङ्ग;
	पक्ष SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		भङ्ग;
	पक्ष SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_to_hartindex(val);
		sse_event_invoke_cb(e, set_hartid_cb, val);
		भङ्ग;

	पक्ष SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		भङ्ग;
	पक्ष SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		भङ्ग;
	पक्ष SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		भङ्ग;
	पक्ष SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		भङ्ग;
	}
}

स्थिर पूर्णाङ्क sse_event_register(संरचना sbi_sse_event *e,
			      unsigned long handler_entry_pc,
			      unsigned long handler_entry_arg)
{
	सचे (sse_event_state(e) != SBI_SSE_STATE_UNUSED)
		पटिनिवत्त SBI_EINVALID_STATE;

	e->attrs.entry.pc = handler_entry_pc;
	e->attrs.entry.arg = handler_entry_arg;

	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, register_cb);

	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क sse_event_unregister(संरचना sbi_sse_event *e)
{
	सचे (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		पटिनिवत्त SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

	पटिनिवत्त 0;
}

स्थिर unsigned long sse_interrupted_flags(unsigned long mstatus)
{
	unsigned long hstatus, flags = 0;

	सचे (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	सचे (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	सचे (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	सचे (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	सचे (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		सचे (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		सचे (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	पटिनिवत्त flags;
}

स्थिर शून्य sse_event_inject(संरचना sbi_sse_event *e,
			     संरचना sbi_trap_regs *regs)
{
	संरचना sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_set_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.status &= ~BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	सचे (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	सचे (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	सचे (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
		सचे (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		अञ्ञथा
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		सचे (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
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

#सचे __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#नोचेद्यदि __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#अञ्ञथा
#error "Unexpected __riscv_xlen"
#endif

}

स्थिर शून्य sse_event_resume(संरचना sbi_sse_event *e,
			     संरचना sbi_trap_regs *regs)
{
	संरचना sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	सचे (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

	सचे (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
#सचे __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		सचे (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#नोचेद्यदि __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		सचे (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#अञ्ञथा
#error "Unexpected __riscv_xlen"
#endif
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		सचे (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		सचे (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	सचे (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	सचे (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	सचे (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	सचे (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	सचे (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

स्थिर bool sse_event_is_ready(संरचना sbi_sse_event *e)
{
	सचे (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		पटिनिवत्त false;
	}

	पटिनिवत्त true;
}

स्थिर bool sse_event_check_inject(संरचना sbi_sse_event *e,
				   संरचना sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	सचे (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		पटिनिवत्त true;
	}

	सचे (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		पटिनिवत्त true;
	}

	पटिनिवत्त false;
}

/* Return true if an event has been injected, false otherwise */
शून्य sbi_sse_process_pending_events(संरचना sbi_trap_regs *regs)
{
	bool ret;
	संरचना sbi_sse_event *e;
	संरचना sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	सचे (state->masked)
		पटिनिवत्त;

	spin_lock(&state->enabled_event_lock);

	sbi_list_for_each_entry(e, &state->enabled_event_list, ग्रन्थिः) {
		ret = sse_event_check_inject(e, regs);
		सचे (ret)
			गच्छ out;
	}

out:
	spin_unlock(&state->enabled_event_lock);
}

स्थिर पूर्णाङ्क sse_event_set_pending(संरचना sbi_sse_event *e)
{
	सचे (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		पटिनिवत्त SBI_EINVALID_STATE;

	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	पटिनिवत्त SBI_OK;
}

स्थिर शून्य sse_ipi_inject_process(संरचना sbi_scratch *scratch)
{
	संरचना sbi_sse_event *e;
	संरचना sse_ipi_inject_data evt;
	संरचना sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	याव (!sbi_fifo_dequeue(sse_inject_fifo_r, &evt)) {
		सचे (sse_event_get(evt.event_id, &e))
			अनुवर्त;

		sse_event_set_pending(e);
		sse_event_put(e);
	}
}

स्थिर संरचना sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

स्थिर पूर्णाङ्क sse_ipi_inject_send(unsigned long hartid, uint32_t event_id)
{
	पूर्णाङ्क ret;
	संरचना sbi_scratch *remote_scratch = रिक्त;
	संरचना sse_ipi_inject_data evt = {event_id};
	संरचना sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_to_scratch(hartid);
	सचे (!remote_scratch)
		पटिनिवत्त SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_offset_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_enqueue(sse_inject_fifo_r, &evt, false);
	सचे (ret)
		पटिनिवत्त SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, रिक्त);
	सचे (ret)
		पटिनिवत्त SBI_EFAIL;

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क sse_inject_event(uint32_t event_id, unsigned long hartid)
{
	पूर्णाङ्क ret;
	संरचना sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	सचे (ret)
		पटिनिवत्त ret;

	/* In case of global event, provided hart_id is ignored */
	सचे (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	सचे (hartid != current_hartid()) {
		sse_event_put(e);
		पटिनिवत्त sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_set_pending(e);
	sse_event_put(e);
	सचे (ret)
		पटिनिवत्त ret;

	पटिनिवत्त SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
स्थिर पूर्णाङ्क sse_event_enable(संरचना sse_hart_state *shs,
			     संरचना sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	सचे (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		पटिनिवत्त SBI_EINVALID_STATE;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_add_to_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	सचे (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  रिक्त);

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क sse_event_complete(संरचना sse_hart_state *shs,
			      संरचना sbi_sse_event *e,
			      संरचना sbi_trap_regs *regs,
			      संरचना sbi_ecall_return *out)
	MUST_HOLD(&shs->enabled_event_lock)
{
	सचे (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		पटिनिवत्त SBI_EINVALID_STATE;

	सचे (e->attrs.hartid != current_hartid())
		पटिनिवत्त SBI_EINVAL;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	सचे (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	out->skip_regs_update = true;

	पटिनिवत्त SBI_OK;
}

पूर्णाङ्क sbi_sse_complete(संरचना sbi_trap_regs *regs, संरचना sbi_ecall_return *out)
{
	पूर्णाङ्क ret = SBI_OK;
	संरचना sbi_sse_event *tmp;
	संरचना sse_hart_state *state = sse_thishart_state_ptr();

	spin_lock(&state->enabled_event_lock);
	sbi_list_for_each_entry(tmp, &state->enabled_event_list, ग्रन्थिः) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		सचे (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, out);
			भङ्ग;
		}
	}
	spin_unlock(&state->enabled_event_lock);

	पटिनिवत्त ret;
}

पूर्णाङ्क sbi_sse_enable(uint32_t event_id)
{
	पूर्णाङ्क ret;
	संरचना sbi_sse_event *e;
	संरचना sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	सचे (ret)
		पटिनिवत्त ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_lock);
	sse_event_put(e);

	पटिनिवत्त ret;
}

पूर्णाङ्क sbi_sse_disable(uint32_t event_id)
{
	पूर्णाङ्क ret;
	संरचना sbi_sse_event *e;
	संरचना sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	सचे (ret)
		पटिनिवत्त ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_lock);

	sse_event_put(e);

	पटिनिवत्त ret;
}

पूर्णाङ्क sbi_sse_hart_mask(शून्य)
{
	संरचना sse_hart_state *state = sse_thishart_state_ptr();

	सचे (!state)
		पटिनिवत्त SBI_EFAIL;

	सचे (state->masked)
		पटिनिवत्त SBI_EALREADY_STOPPED;

	state->masked = true;

	पटिनिवत्त SBI_SUCCESS;
}

पूर्णाङ्क sbi_sse_hart_unmask(शून्य)
{
	संरचना sse_hart_state *state = sse_thishart_state_ptr();

	सचे (!state)
		पटिनिवत्त SBI_EFAIL;

	सचे (!state->masked)
		पटिनिवत्त SBI_EALREADY_STARTED;

	state->masked = false;

	पटिनिवत्त SBI_SUCCESS;
}

पूर्णाङ्क sbi_sse_inject_from_ecall(uint32_t event_id, unsigned long hartid,
			      संरचना sbi_ecall_return *out)
{
	सचे (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_to_hartindex(hartid)))
		पटिनिवत्त SBI_EINVAL;

	पटिनिवत्त sse_inject_event(event_id, hartid);
}

पूर्णाङ्क sbi_sse_inject_event(uint32_t event_id)
{
	पटिनिवत्त sse_inject_event(event_id, current_hartid());
}

पूर्णाङ्क sbi_sse_add_event(uint32_t event_id, ध्रुव संरचना sbi_sse_cb_ops *cb_ops)
{
	संरचना sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	सचे (info)
		पटिनिवत्त SBI_EALREADY;

	सचे (cb_ops && cb_ops->set_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		पटिनिवत्त SBI_EINVAL;

	info = sbi_zalloc(परिमाण(*info));
	सचे (!info)
		पटिनिवत्त SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ADD(info, supported_events);

	पटिनिवत्त SBI_OK;
}

पूर्णाङ्क sbi_sse_attr_check(uint32_t base_attr_id, uint32_t attr_गणनम्,
		       unsigned long phys_lo, unsigned long phys_hi,
		       unsigned long access)
{
	ध्रुव unsigned align = __riscv_xlen >> 3;
	uint64_t end_id = (uint64_t)base_attr_id + (attr_गणनम् - 1);

	सचे (attr_गणनम् == 0)
		पटिनिवत्त SBI_ERR_INVALID_PARAM;

	सचे (end_id >= SBI_SSE_ATTR_MAX)
		पटिनिवत्त SBI_EBAD_RANGE;

	सचे (phys_lo & (align - 1))
		पटिनिवत्त SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	सचे (phys_hi)
		पटिनिवत्त SBI_EINVALID_ADDR;

	सचे (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 परिमाण(unsigned long) * attr_गणनम्,
					 PRV_S, access))
		पटिनिवत्त SBI_EINVALID_ADDR;

	पटिनिवत्त SBI_OK;
}

स्थिर शून्य copy_attrs(unsigned long *out, ध्रुव unsigned long *in,
		       unsigned पूर्णाङ्क long_गणनम्)
{
	पूर्णाङ्क i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	अत्थाय (i = 0; i < long_गणनम्; i++)
		out[i] = in[i];
}

पूर्णाङ्क sbi_sse_read_attrs(uint32_t event_id, uint32_t base_attr_id,
		       uint32_t attr_गणनम्, unsigned long output_phys_lo,
		       unsigned long output_phys_hi)
{
	पूर्णाङ्क ret;
	unsigned long *e_attrs;
	संरचना sbi_sse_event *e;
	unsigned long *attrs;

	ret = sbi_sse_attr_check(base_attr_id, attr_गणनम्, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	सचे (ret)
		पटिनिवत्त ret;

	ret = sse_event_get(event_id, &e);
	सचे (ret)
		पटिनिवत्त ret;

	sbi_hart_protection_temp_map_range(output_phys_lo, परिमाण(unsigned long) * attr_गणनम्);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (unsigned long *)&e->attrs;
	attrs = (unsigned long *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[base_attr_id], attr_गणनम्);

	sbi_hart_protection_temp_unmap_range(output_phys_lo, परिमाण(unsigned long) * attr_गणनम्);

	sse_event_put(e);

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क sse_write_attrs(संरचना sbi_sse_event *e, uint32_t base_attr_id,
			   uint32_t attr_गणनम्, unsigned long निवेशः_phys)
{
	पूर्णाङ्क ret = 0;
	unsigned long attr = 0, val;
	uint32_t id, end_id = base_attr_id + attr_गणनम्;
	unsigned long *attrs = (unsigned long *)निवेशः_phys;

	sbi_hart_protection_temp_map_range(निवेशः_phys, परिमाण(unsigned long) * attr_गणनम्);

	अत्थाय (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		ret = sse_event_set_attr_check(e, id, val);
		सचे (ret)
			गच्छ out;
	}

	attr = 0;
	अत्थाय (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		sse_event_set_attr(e, id, val);
	}

out:
	sbi_hart_protection_temp_unmap_range(निवेशः_phys, परिमाण(unsigned long) * attr_गणनम्);

	पटिनिवत्त ret;
}

पूर्णाङ्क sbi_sse_write_attrs(uint32_t event_id, uint32_t base_attr_id,
			uint32_t attr_गणनम्, unsigned long निवेशः_phys_lo,
			unsigned long निवेशः_phys_hi)
{
	पूर्णाङ्क ret = 0;
	संरचना sbi_sse_event *e;

	ret = sbi_sse_attr_check(base_attr_id, attr_गणनम्, निवेशः_phys_lo,
				 निवेशः_phys_hi, SBI_DOMAIN_READ);
	सचे (ret)
		पटिनिवत्त ret;

	ret = sse_event_get(event_id, &e);
	सचे (ret)
		पटिनिवत्त ret;

	ret = sse_write_attrs(e, base_attr_id, attr_गणनम्, निवेशः_phys_lo);
	sse_event_put(e);

	पटिनिवत्त ret;
}

पूर्णाङ्क sbi_sse_register(uint32_t event_id, unsigned long handler_entry_pc,
		     unsigned long handler_entry_arg)
{
	पूर्णाङ्क ret;
	संरचना sbi_sse_event *e;

	सचे (handler_entry_pc & 0x1)
		पटिनिवत्त SBI_EINVAL;

	सचे (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_entry_pc,
					 परिमाण(unsigned long), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		पटिनिवत्त SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	सचे (ret)
		पटिनिवत्त ret;

	ret = sse_event_register(e, handler_entry_pc, handler_entry_arg);
	sse_event_put(e);

	पटिनिवत्त ret;
}

पूर्णाङ्क sbi_sse_unregister(uint32_t event_id)
{
	पूर्णाङ्क ret;
	संरचना sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	सचे (ret)
		पटिनिवत्त ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	पटिनिवत्त ret;
}

स्थिर शून्य sse_event_init(संरचना sbi_sse_event *e, संरचना sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_INJECT_OFFSET);
}

स्थिर शून्य sse_event_गणनम्_init()
{
	संरचना sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		सचे (EVENT_IS_GLOBAL(info->event_id))
			global_event_गणनम्++;
		अञ्ञथा
			local_event_गणनम्++;
	}
}

स्थिर पूर्णाङ्क sse_global_init()
{
	संरचना sbi_sse_event *e;
	unsigned पूर्णाङ्क ev = 0;
	संरचना sse_event_info *info;

	global_events = sbi_zalloc(परिमाण(*global_events) * global_event_गणनम्);
	सचे (!global_events)
		पटिनिवत्त SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		सचे (!EVENT_IS_GLOBAL(info->event_id))
			अनुवर्त;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_LOCK_INIT(global_events[ev].lock);

		ev++;
	}

	पटिनिवत्त 0;
}

स्थिर शून्य sse_local_init(संरचना sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	unsigned पूर्णाङ्क ev = 0;
	संरचना sse_event_info *info;

	SBI_INIT_LIST_HEAD(&shs->enabled_event_list);
	SPIN_LOCK_INIT(shs->enabled_event_lock);

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		सचे (EVENT_IS_GLOBAL(info->event_id))
			अनुवर्त;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

पूर्णाङ्क sbi_sse_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	पूर्णाङ्क ret;
	शून्य *sse_inject_mem;
	संरचना sse_hart_state *shs;
	संरचना sbi_fifo *sse_inject_q;

	सचे (cold_boot) {
		sse_event_गणनम्_init();

		ret = sse_global_init();
		सचे (ret)
			पटिनिवत्त ret;

		shs_ptr_off = sbi_scratch_alloc_offset(परिमाण(शून्य *));
		सचे (!shs_ptr_off)
			पटिनिवत्त SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_offset(परिमाण(*sse_inject_q));
		सचे (!sse_inject_fifo_off) {
			sbi_scratch_free_offset(shs_ptr_off);
			पटिनिवत्त SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_offset(
			(global_event_गणनम् + local_event_गणनम्) *
			परिमाण(संरचना sse_ipi_inject_data));
		सचे (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_offset(sse_inject_fifo_off);
			sbi_scratch_free_offset(shs_ptr_off);
			पटिनिवत्त SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		सचे (ret < 0) {
			sbi_scratch_free_offset(shs_ptr_off);
			पटिनिवत्त ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	सचे (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(परिमाण(*shs) + परिमाण(संरचना sbi_sse_event) *
							local_event_गणनम्);
		सचे (!shs)
			पटिनिवत्त SBI_ENOMEM;

		shs->local_events = (संरचना sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_set_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_गणनम् + local_event_गणनम्),
		      परिमाण(संरचना sse_ipi_inject_data));

	पटिनिवत्त 0;
}

शून्य sbi_sse_exit(संरचना sbi_scratch *scratch)
{
	संरचना sbi_sse_event *e;
	संरचना sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		सचे (sse_event_get(info->event_id, &e))
			अनुवर्त;

		सचे (e->attrs.hartid != current_hartid())
			गच्छ skip;

		सचे (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

skip:
		sse_event_put(e);
	}
}
