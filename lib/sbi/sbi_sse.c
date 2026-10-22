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
		यदि (__ops && __ops->_cb)                                      \
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
	संरचना sbi_dlist गाँठ;
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
	SBI_SLIST_गाँठ(sse_event_info);
};

स्थिर unsigned पूर्णाङ्क local_event_गिनती;
स्थिर unsigned पूर्णाङ्क global_event_गिनती;
स्थिर संरचना sse_global_event *global_events;

स्थिर unsigned long sse_inject_fifo_off;
स्थिर unsigned long sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
स्थिर unsigned long shs_ptr_off;

स्थिर u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

स्थिर पूर्णाङ्क sse_ipi_inject_send(unsigned long hartid, uint32_t event_id);

संरचना sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_गाँठ_INIT(रिक्त),
};

संरचना sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_गाँठ_INIT(&global_software_event),
};

स्थिर SBI_SLIST_HEAD(supported_events, sse_event_info) =
				SBI_SLIST_HEAD_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
स्थिर ध्रुव uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_ऊपरी_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_ऊपरी_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW,
	SBI_SSE_EVENT_LOCAL_निचला_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_निचला_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

स्थिर bool sse_is_standard_event(uint32_t event_id)
{
	पूर्णाङ्क i;

	प्रति (i = 0; i < array_size(standard_events); i++) {
		यदि (event_id == standard_events[i])
			प्रत्यागम true;
	}

	प्रत्यागम false;
}

स्थिर संरचना sse_event_info *sse_event_info_get(uint32_t event_id)
{
	संरचना sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		यदि (info->event_id == event_id)
			प्रत्यागम info;
	}

	प्रत्यागम रिक्त;
}

स्थिर unsigned long sse_event_state(संरचना sbi_sse_event *e)
{
	प्रत्यागम e->attrs.status & SBI_SSE_ATTR_STATUS_STATE_MASK;
}

स्थिर unsigned long sse_event_pending(संरचना sbi_sse_event *e)
{
	प्रत्यागम !!(e->attrs.status & BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET));
}

स्थिर bool sse_event_is_global(संरचना sbi_sse_event *e)
{
	प्रत्यागम EVENT_IS_GLOBAL(e->event_id);
}

स्थिर bool sse_event_is_local(संरचना sbi_sse_event *e)
{
	प्रत्यागम !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
स्थिर संरचना sse_hart_state *sse_get_hart_state(संरचना sbi_sse_event *e)
{
	संरचना sbi_scratch *s = sbi_hartindex_to_scratch(e->hartindex);

	प्रत्यागम sse_get_hart_state_ptr(s);
}

स्थिर संरचना sse_global_event *sse_get_global_event(संरचना sbi_sse_event *e)
{
	प्रत्यागम container_of(e, संरचना sse_global_event, event);
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

	यदि (!eret)
		प्रत्यागम SBI_EINVAL;

	यदि (EVENT_IS_GLOBAL(event_id)) {
		प्रति (i = 0; i < global_event_गिनती; i++) {
			e = &global_events[i].event;
			यदि (e->event_id == event_id) {
				spin_lock(&global_events[i].lock);
				*eret = e;
				प्रत्यागम SBI_SUCCESS;
			}
		}
	} अन्यथा {
		shs = sse_thishart_state_ptr();
		प्रति (i = 0; i < local_event_गिनती; i++) {
			e = &shs->local_events[i];
			यदि (e->event_id == event_id) {
				*eret = e;
				प्रत्यागम SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	यदि (sse_is_standard_event(event_id))
		प्रत्यागम SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	प्रत्यागम SBI_EINVAL;
}

स्थिर शून्य sse_event_put(संरचना sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	संरचना sse_global_event *ge;

	यदि (sse_event_is_local(e))
		प्रत्यागम;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->lock);
}

स्थिर शून्य sse_event_remove_from_list(संरचना sbi_sse_event *e)
{
	sbi_list_del(&e->गाँठ);
}

/**
 * Must be called under owner hart lock
 */
स्थिर शून्य sse_event_add_to_list(संरचना sse_hart_state *state,
				   संरचना sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_lock)
{
	संरचना sbi_sse_event *tmp;

	sbi_list_for_each_entry(tmp, &state->enabled_event_list, गाँठ) {
		यदि (e->attrs.prio < tmp->attrs.prio)
			भङ्ग;
		यदि (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			भङ्ग;
	}
	sbi_list_add_tail(&e->गाँठ, &tmp->गाँठ);
}

/**
 * Must be called under owner hart lock
 */
स्थिर पूर्णाङ्क sse_event_disable(संरचना sse_hart_state *shs,
			      संरचना sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	यदि (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		प्रत्यागम SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क sse_event_set_hart_id_check(संरचना sbi_sse_event *e,
				       unsigned long new_hartid)
{
	पूर्णाङ्क hstate;
	unsigned पूर्णाङ्क hartid = (uint32_t)new_hartid;
	संरचना sbi_domain *hd = sbi_domain_thishart_ptr();

	यदि (!sse_event_is_global(e))
		प्रत्यागम SBI_EDENIED;

	यदि (!sbi_domain_is_assigned_hart(hd, sbi_hartid_to_hartindex(hartid)))
		प्रत्यागम SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	यदि (hstate != SBI_HSM_STATE_STARTED)
		प्रत्यागम SBI_EINVAL;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क sse_event_set_attr_check(संरचना sbi_sse_event *e, uint32_t attr_id,
				    unsigned long val)
{
	विकल्प (attr_id) {
	पक्ष SBI_SSE_ATTR_CONFIG:
		यदि (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			प्रत्यागम SBI_EINVALID_STATE;

		यदि (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			प्रत्यागम SBI_EINVAL;

		प्रत्यागम SBI_OK;
	पक्ष SBI_SSE_ATTR_PRIO:
		यदि (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			प्रत्यागम SBI_EINVALID_STATE;

#यदि __riscv_xlen > 32
		यदि (val != (uint32_t)val)
			प्रत्यागम SBI_EINVAL;
#endif
		प्रत्यागम SBI_OK;
	पक्ष SBI_SSE_ATTR_PREFERRED_HART:
		यदि (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			प्रत्यागम SBI_EINVALID_STATE;

		प्रत्यागम sse_event_set_hart_id_check(e, val);
	पक्ष SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		यदि (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			प्रत्यागम SBI_EINVAL;
		__attribute__((__fallthrough__));
	पक्ष SBI_SSE_ATTR_INTERRUPTED_SEPC:
	पक्ष SBI_SSE_ATTR_INTERRUPTED_A6:
	पक्ष SBI_SSE_ATTR_INTERRUPTED_A7:
		यदि (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			प्रत्यागम SBI_EINVALID_STATE;

		यदि (current_hartid() != e->attrs.hartid)
			प्रत्यागम SBI_EINVAL;

		प्रत्यागम SBI_OK;
	स्वभाव:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		प्रत्यागम SBI_EDENIED;
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
	यदि (sse_event_state(e) != SBI_SSE_STATE_UNUSED)
		प्रत्यागम SBI_EINVALID_STATE;

	e->attrs.entry.pc = handler_entry_pc;
	e->attrs.entry.arg = handler_entry_arg;

	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, register_cb);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क sse_event_unregister(संरचना sbi_sse_event *e)
{
	यदि (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		प्रत्यागम SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

	प्रत्यागम 0;
}

स्थिर unsigned long sse_interrupted_flags(unsigned long mstatus)
{
	unsigned long hstatus, flags = 0;

	यदि (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	यदि (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	यदि (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	यदि (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	यदि (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		यदि (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		यदि (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	प्रत्यागम flags;
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
	यदि (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	यदि (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	यदि (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
		यदि (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		अन्यथा
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		यदि (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
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

#यदि __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#नोचेद्यदि __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#अन्यथा
#error "Unexpected __riscv_xlen"
#endif

}

स्थिर शून्य sse_event_resume(संरचना sbi_sse_event *e,
			     संरचना sbi_trap_regs *regs)
{
	संरचना sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	यदि (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

	यदि (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
#यदि __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		यदि (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#नोचेद्यदि __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		यदि (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#अन्यथा
#error "Unexpected __riscv_xlen"
#endif
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		यदि (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		यदि (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	यदि (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	यदि (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	यदि (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	यदि (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	यदि (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

स्थिर bool sse_event_is_ready(संरचना sbi_sse_event *e)
{
	यदि (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		प्रत्यागम false;
	}

	प्रत्यागम true;
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
	यदि (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		प्रत्यागम true;
	}

	यदि (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		प्रत्यागम true;
	}

	प्रत्यागम false;
}

/* Return true if an event has been injected, false otherwise */
शून्य sbi_sse_process_pending_events(संरचना sbi_trap_regs *regs)
{
	bool ret;
	संरचना sbi_sse_event *e;
	संरचना sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	यदि (state->masked)
		प्रत्यागम;

	spin_lock(&state->enabled_event_lock);

	sbi_list_for_each_entry(e, &state->enabled_event_list, गाँठ) {
		ret = sse_event_check_inject(e, regs);
		यदि (ret)
			गच्छ out;
	}

out:
	spin_unlock(&state->enabled_event_lock);
}

स्थिर पूर्णाङ्क sse_event_set_pending(संरचना sbi_sse_event *e)
{
	यदि (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		प्रत्यागम SBI_EINVALID_STATE;

	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	प्रत्यागम SBI_OK;
}

स्थिर शून्य sse_ipi_inject_process(संरचना sbi_scratch *scratch)
{
	संरचना sbi_sse_event *e;
	संरचना sse_ipi_inject_data evt;
	संरचना sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	यावत् (!sbi_fifo_dequeue(sse_inject_fifo_r, &evt)) {
		यदि (sse_event_get(evt.event_id, &e))
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
	यदि (!remote_scratch)
		प्रत्यागम SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_offset_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_enqueue(sse_inject_fifo_r, &evt, false);
	यदि (ret)
		प्रत्यागम SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, रिक्त);
	यदि (ret)
		प्रत्यागम SBI_EFAIL;

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क sse_inject_event(uint32_t event_id, unsigned long hartid)
{
	पूर्णाङ्क ret;
	संरचना sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	यदि (ret)
		प्रत्यागम ret;

	/* In case of global event, provided hart_id is ignored */
	यदि (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	यदि (hartid != current_hartid()) {
		sse_event_put(e);
		प्रत्यागम sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_set_pending(e);
	sse_event_put(e);
	यदि (ret)
		प्रत्यागम ret;

	प्रत्यागम SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
स्थिर पूर्णाङ्क sse_event_enable(संरचना sse_hart_state *shs,
			     संरचना sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	यदि (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		प्रत्यागम SBI_EINVALID_STATE;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_add_to_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	यदि (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  रिक्त);

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क sse_event_complete(संरचना sse_hart_state *shs,
			      संरचना sbi_sse_event *e,
			      संरचना sbi_trap_regs *regs,
			      संरचना sbi_ecall_return *out)
	MUST_HOLD(&shs->enabled_event_lock)
{
	यदि (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		प्रत्यागम SBI_EINVALID_STATE;

	यदि (e->attrs.hartid != current_hartid())
		प्रत्यागम SBI_EINVAL;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	यदि (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	out->skip_regs_update = true;

	प्रत्यागम SBI_OK;
}

पूर्णाङ्क sbi_sse_complete(संरचना sbi_trap_regs *regs, संरचना sbi_ecall_return *out)
{
	पूर्णाङ्क ret = SBI_OK;
	संरचना sbi_sse_event *tmp;
	संरचना sse_hart_state *state = sse_thishart_state_ptr();

	spin_lock(&state->enabled_event_lock);
	sbi_list_for_each_entry(tmp, &state->enabled_event_list, गाँठ) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		यदि (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, out);
			भङ्ग;
		}
	}
	spin_unlock(&state->enabled_event_lock);

	प्रत्यागम ret;
}

पूर्णाङ्क sbi_sse_enable(uint32_t event_id)
{
	पूर्णाङ्क ret;
	संरचना sbi_sse_event *e;
	संरचना sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	यदि (ret)
		प्रत्यागम ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_lock);
	sse_event_put(e);

	प्रत्यागम ret;
}

पूर्णाङ्क sbi_sse_disable(uint32_t event_id)
{
	पूर्णाङ्क ret;
	संरचना sbi_sse_event *e;
	संरचना sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	यदि (ret)
		प्रत्यागम ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_lock);

	sse_event_put(e);

	प्रत्यागम ret;
}

पूर्णाङ्क sbi_sse_hart_mask(शून्य)
{
	संरचना sse_hart_state *state = sse_thishart_state_ptr();

	यदि (!state)
		प्रत्यागम SBI_EFAIL;

	यदि (state->masked)
		प्रत्यागम SBI_EALREADY_STOPPED;

	state->masked = true;

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_sse_hart_unmask(शून्य)
{
	संरचना sse_hart_state *state = sse_thishart_state_ptr();

	यदि (!state)
		प्रत्यागम SBI_EFAIL;

	यदि (!state->masked)
		प्रत्यागम SBI_EALREADY_STARTED;

	state->masked = false;

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_sse_inject_from_ecall(uint32_t event_id, unsigned long hartid,
			      संरचना sbi_ecall_return *out)
{
	यदि (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_to_hartindex(hartid)))
		प्रत्यागम SBI_EINVAL;

	प्रत्यागम sse_inject_event(event_id, hartid);
}

पूर्णाङ्क sbi_sse_inject_event(uint32_t event_id)
{
	प्रत्यागम sse_inject_event(event_id, current_hartid());
}

पूर्णाङ्क sbi_sse_add_event(uint32_t event_id, ध्रुव संरचना sbi_sse_cb_ops *cb_ops)
{
	संरचना sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	यदि (info)
		प्रत्यागम SBI_EALREADY;

	यदि (cb_ops && cb_ops->set_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		प्रत्यागम SBI_EINVAL;

	info = sbi_zalloc(परिमाण(*info));
	यदि (!info)
		प्रत्यागम SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ADD(info, supported_events);

	प्रत्यागम SBI_OK;
}

पूर्णाङ्क sbi_sse_attr_check(uint32_t base_attr_id, uint32_t attr_गिनती,
		       unsigned long phys_lo, unsigned long phys_hi,
		       unsigned long access)
{
	ध्रुव unsigned align = __riscv_xlen >> 3;
	uint64_t end_id = (uint64_t)base_attr_id + (attr_गिनती - 1);

	यदि (attr_गिनती == 0)
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	यदि (end_id >= SBI_SSE_ATTR_MAX)
		प्रत्यागम SBI_EBAD_RANGE;

	यदि (phys_lo & (align - 1))
		प्रत्यागम SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	यदि (phys_hi)
		प्रत्यागम SBI_EINVALID_ADDR;

	यदि (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 परिमाण(unsigned long) * attr_गिनती,
					 PRV_S, access))
		प्रत्यागम SBI_EINVALID_ADDR;

	प्रत्यागम SBI_OK;
}

स्थिर शून्य copy_attrs(unsigned long *out, ध्रुव unsigned long *in,
		       unsigned पूर्णाङ्क long_गिनती)
{
	पूर्णाङ्क i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	प्रति (i = 0; i < long_गिनती; i++)
		out[i] = in[i];
}

पूर्णाङ्क sbi_sse_read_attrs(uint32_t event_id, uint32_t base_attr_id,
		       uint32_t attr_गिनती, unsigned long output_phys_lo,
		       unsigned long output_phys_hi)
{
	पूर्णाङ्क ret;
	unsigned long *e_attrs;
	संरचना sbi_sse_event *e;
	unsigned long *attrs;

	ret = sbi_sse_attr_check(base_attr_id, attr_गिनती, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	यदि (ret)
		प्रत्यागम ret;

	ret = sse_event_get(event_id, &e);
	यदि (ret)
		प्रत्यागम ret;

	sbi_hart_protection_temp_map_range(output_phys_lo, परिमाण(unsigned long) * attr_गिनती);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (unsigned long *)&e->attrs;
	attrs = (unsigned long *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[base_attr_id], attr_गिनती);

	sbi_hart_protection_temp_unmap_range(output_phys_lo, परिमाण(unsigned long) * attr_गिनती);

	sse_event_put(e);

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क sse_write_attrs(संरचना sbi_sse_event *e, uint32_t base_attr_id,
			   uint32_t attr_गिनती, unsigned long निवेश_phys)
{
	पूर्णाङ्क ret = 0;
	unsigned long attr = 0, val;
	uint32_t id, end_id = base_attr_id + attr_गिनती;
	unsigned long *attrs = (unsigned long *)निवेश_phys;

	sbi_hart_protection_temp_map_range(निवेश_phys, परिमाण(unsigned long) * attr_गिनती);

	प्रति (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		ret = sse_event_set_attr_check(e, id, val);
		यदि (ret)
			गच्छ out;
	}

	attr = 0;
	प्रति (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		sse_event_set_attr(e, id, val);
	}

out:
	sbi_hart_protection_temp_unmap_range(निवेश_phys, परिमाण(unsigned long) * attr_गिनती);

	प्रत्यागम ret;
}

पूर्णाङ्क sbi_sse_write_attrs(uint32_t event_id, uint32_t base_attr_id,
			uint32_t attr_गिनती, unsigned long निवेश_phys_lo,
			unsigned long निवेश_phys_hi)
{
	पूर्णाङ्क ret = 0;
	संरचना sbi_sse_event *e;

	ret = sbi_sse_attr_check(base_attr_id, attr_गिनती, निवेश_phys_lo,
				 निवेश_phys_hi, SBI_DOMAIN_READ);
	यदि (ret)
		प्रत्यागम ret;

	ret = sse_event_get(event_id, &e);
	यदि (ret)
		प्रत्यागम ret;

	ret = sse_write_attrs(e, base_attr_id, attr_गिनती, निवेश_phys_lo);
	sse_event_put(e);

	प्रत्यागम ret;
}

पूर्णाङ्क sbi_sse_register(uint32_t event_id, unsigned long handler_entry_pc,
		     unsigned long handler_entry_arg)
{
	पूर्णाङ्क ret;
	संरचना sbi_sse_event *e;

	यदि (handler_entry_pc & 0x1)
		प्रत्यागम SBI_EINVAL;

	यदि (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_entry_pc,
					 परिमाण(unsigned long), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		प्रत्यागम SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	यदि (ret)
		प्रत्यागम ret;

	ret = sse_event_register(e, handler_entry_pc, handler_entry_arg);
	sse_event_put(e);

	प्रत्यागम ret;
}

पूर्णाङ्क sbi_sse_unregister(uint32_t event_id)
{
	पूर्णाङ्क ret;
	संरचना sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	यदि (ret)
		प्रत्यागम ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	प्रत्यागम ret;
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

स्थिर शून्य sse_event_गिनती_init()
{
	संरचना sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		यदि (EVENT_IS_GLOBAL(info->event_id))
			global_event_गिनती++;
		अन्यथा
			local_event_गिनती++;
	}
}

स्थिर पूर्णाङ्क sse_global_init()
{
	संरचना sbi_sse_event *e;
	unsigned पूर्णाङ्क ev = 0;
	संरचना sse_event_info *info;

	global_events = sbi_zalloc(परिमाण(*global_events) * global_event_गिनती);
	यदि (!global_events)
		प्रत्यागम SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		यदि (!EVENT_IS_GLOBAL(info->event_id))
			अनुवर्त;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_LOCK_INIT(global_events[ev].lock);

		ev++;
	}

	प्रत्यागम 0;
}

स्थिर शून्य sse_local_init(संरचना sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	unsigned पूर्णाङ्क ev = 0;
	संरचना sse_event_info *info;

	SBI_INIT_LIST_HEAD(&shs->enabled_event_list);
	SPIN_LOCK_INIT(shs->enabled_event_lock);

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		यदि (EVENT_IS_GLOBAL(info->event_id))
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

	यदि (cold_boot) {
		sse_event_गिनती_init();

		ret = sse_global_init();
		यदि (ret)
			प्रत्यागम ret;

		shs_ptr_off = sbi_scratch_alloc_offset(परिमाण(शून्य *));
		यदि (!shs_ptr_off)
			प्रत्यागम SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_offset(परिमाण(*sse_inject_q));
		यदि (!sse_inject_fifo_off) {
			sbi_scratch_free_offset(shs_ptr_off);
			प्रत्यागम SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_offset(
			(global_event_गिनती + local_event_गिनती) *
			परिमाण(संरचना sse_ipi_inject_data));
		यदि (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_offset(sse_inject_fifo_off);
			sbi_scratch_free_offset(shs_ptr_off);
			प्रत्यागम SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		यदि (ret < 0) {
			sbi_scratch_free_offset(shs_ptr_off);
			प्रत्यागम ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	यदि (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(परिमाण(*shs) + परिमाण(संरचना sbi_sse_event) *
							local_event_गिनती);
		यदि (!shs)
			प्रत्यागम SBI_ENOMEM;

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
		      (global_event_गिनती + local_event_गिनती),
		      परिमाण(संरचना sse_ipi_inject_data));

	प्रत्यागम 0;
}

शून्य sbi_sse_exit(संरचना sbi_scratch *scratch)
{
	संरचना sbi_sse_event *e;
	संरचना sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		यदि (sse_event_get(info->event_id, &e))
			अनुवर्त;

		यदि (e->attrs.hartid != current_hartid())
			गच्छ skip;

		यदि (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

skip:
		sse_event_put(e);
	}
}
