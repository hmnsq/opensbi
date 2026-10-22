/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Rivos Systems Inc.
 *
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_barrier.h>
#અન્તર્ભાવ <sbi/riscv_encoding.h>
#અન્તર્ભાવ <sbi/riscv_locks.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_fifo.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_hart_protection.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_hsm.h>
#અન્તર્ભાવ <sbi/sbi_ipi.h>
#અન્તર્ભાવ <sbi/sbi_list.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_pmu.h>
#અન્તર્ભાવ <sbi/sbi_sse.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_slist.h>
#અન્તર્ભાવ <sbi/sbi_string.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>

#અન્તર્ભાવ <sbi/sbi_console.h>

#define sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), શૂન્ય *, shs_ptr_off)

#define sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define sse_set_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), શૂન્ય *, shs_ptr_off, (__sse_state))

#define EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_BIT)

#define sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		અચલ સંરચના sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		જો (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

સંરચના sse_entry_state {
	/** entry pc */
	unsigned long pc;
	/** a6 register state */
	unsigned long arg;
};

સંરચના sse_interrupted_state {
	/** sepc register state */
	unsigned long sepc;
	/** flags register state */
	unsigned long flags;
	/** a6 register state */
	unsigned long a6;
	/** a7 register state */
	unsigned long a7;
};

સંરચના sse_ipi_inject_data {
	uint32_t event_id;
};

સંરચના sbi_sse_event_attrs {
	unsigned long status;
	unsigned long prio;
	unsigned long config;
	unsigned long hartid;
	સંરચના sse_entry_state entry;
	સંરચના sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)                  \
	_Static_assert(                                          \
		((offsetof(સંરચના sbi_sse_event_attrs, field)) / \
		 કદ(unsigned long)) == attr_offset,          \
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

સંરચના sbi_sse_event {
	સંરચના sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	સંરચના sse_event_info *info;
	સંરચના sbi_dlist ગાંઠ;
};

/** Per-hart state */
સંરચના sse_hart_state {
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
	સંરચના sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_lock);

	/**
	 * List of local events allocated at boot time.
	 */
	સંરચના sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
સંરચના sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t lock;

	/**
	 * global event struct
	 */
	સંરચના sbi_sse_event event GUARDED_BY(&lock);
};

સંરચના sse_event_info {
	uint32_t event_id;
	અચલ સંરચના sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_ગાંઠ(sse_event_info);
};

સ્થિર unsigned પૂર્ણાંક local_event_ગણતરી;
સ્થિર unsigned પૂર્ણાંક global_event_ગણતરી;
સ્થિર સંરચના sse_global_event *global_events;

સ્થિર unsigned long sse_inject_fifo_off;
સ્થિર unsigned long sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
સ્થિર unsigned long shs_ptr_off;

સ્થિર u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

સ્થિર પૂર્ણાંક sse_ipi_inject_send(unsigned long hartid, uint32_t event_id);

સંરચના sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_ગાંઠ_INIT(ખાલી),
};

સંરચના sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_ગાંઠ_INIT(&global_software_event),
};

સ્થિર SBI_SLIST_HEAD(supported_events, sse_event_info) =
				SBI_SLIST_HEAD_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
સ્થિર અચલ uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_ઉપલું_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_ઉપલું_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW,
	SBI_SSE_EVENT_LOCAL_નીચલું_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_નીચલું_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

સ્થિર bool sse_is_standard_event(uint32_t event_id)
{
	પૂર્ણાંક i;

	માટે (i = 0; i < array_size(standard_events); i++) {
		જો (event_id == standard_events[i])
			પાછા true;
	}

	પાછા false;
}

સ્થિર સંરચના sse_event_info *sse_event_info_get(uint32_t event_id)
{
	સંરચના sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		જો (info->event_id == event_id)
			પાછા info;
	}

	પાછા ખાલી;
}

સ્થિર unsigned long sse_event_state(સંરચના sbi_sse_event *e)
{
	પાછા e->attrs.status & SBI_SSE_ATTR_STATUS_STATE_MASK;
}

સ્થિર unsigned long sse_event_pending(સંરચના sbi_sse_event *e)
{
	પાછા !!(e->attrs.status & BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET));
}

સ્થિર bool sse_event_is_global(સંરચના sbi_sse_event *e)
{
	પાછા EVENT_IS_GLOBAL(e->event_id);
}

સ્થિર bool sse_event_is_local(સંરચના sbi_sse_event *e)
{
	પાછા !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
સ્થિર સંરચના sse_hart_state *sse_get_hart_state(સંરચના sbi_sse_event *e)
{
	સંરચના sbi_scratch *s = sbi_hartindex_to_scratch(e->hartindex);

	પાછા sse_get_hart_state_ptr(s);
}

સ્થિર સંરચના sse_global_event *sse_get_global_event(સંરચના sbi_sse_event *e)
{
	પાછા container_of(e, સંરચના sse_global_event, event);
}

સ્થિર શૂન્ય sse_event_set_state(સંરચના sbi_sse_event *e,
				unsigned long new_state)
{
	e->attrs.status &= ~SBI_SSE_ATTR_STATUS_STATE_MASK;
	e->attrs.status |= new_state;
}

સ્થિર પૂર્ણાંક sse_event_get(uint32_t event_id,
			 સંરચના sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	unsigned પૂર્ણાંક i;
	સંરચના sbi_sse_event *e;
	સંરચના sse_hart_state *shs;

	જો (!eret)
		પાછા SBI_EINVAL;

	જો (EVENT_IS_GLOBAL(event_id)) {
		માટે (i = 0; i < global_event_ગણતરી; i++) {
			e = &global_events[i].event;
			જો (e->event_id == event_id) {
				spin_lock(&global_events[i].lock);
				*eret = e;
				પાછા SBI_SUCCESS;
			}
		}
	} નહીંતર {
		shs = sse_thishart_state_ptr();
		માટે (i = 0; i < local_event_ગણતરી; i++) {
			e = &shs->local_events[i];
			જો (e->event_id == event_id) {
				*eret = e;
				પાછા SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	જો (sse_is_standard_event(event_id))
		પાછા SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	પાછા SBI_EINVAL;
}

સ્થિર શૂન્ય sse_event_put(સંરચના sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	સંરચના sse_global_event *ge;

	જો (sse_event_is_local(e))
		પાછા;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->lock);
}

સ્થિર શૂન્ય sse_event_remove_from_list(સંરચના sbi_sse_event *e)
{
	sbi_list_del(&e->ગાંઠ);
}

/**
 * Must be called under owner hart lock
 */
સ્થિર શૂન્ય sse_event_add_to_list(સંરચના sse_hart_state *state,
				   સંરચના sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_lock)
{
	સંરચના sbi_sse_event *tmp;

	sbi_list_for_each_entry(tmp, &state->enabled_event_list, ગાંઠ) {
		જો (e->attrs.prio < tmp->attrs.prio)
			તોડો;
		જો (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			તોડો;
	}
	sbi_list_add_tail(&e->ગાંઠ, &tmp->ગાંઠ);
}

/**
 * Must be called under owner hart lock
 */
સ્થિર પૂર્ણાંક sse_event_disable(સંરચના sse_hart_state *shs,
			      સંરચના sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	જો (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		પાછા SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક sse_event_set_hart_id_check(સંરચના sbi_sse_event *e,
				       unsigned long new_hartid)
{
	પૂર્ણાંક hstate;
	unsigned પૂર્ણાંક hartid = (uint32_t)new_hartid;
	સંરચના sbi_domain *hd = sbi_domain_thishart_ptr();

	જો (!sse_event_is_global(e))
		પાછા SBI_EDENIED;

	જો (!sbi_domain_is_assigned_hart(hd, sbi_hartid_to_hartindex(hartid)))
		પાછા SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	જો (hstate != SBI_HSM_STATE_STARTED)
		પાછા SBI_EINVAL;

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક sse_event_set_attr_check(સંરચના sbi_sse_event *e, uint32_t attr_id,
				    unsigned long val)
{
	પસંદગી (attr_id) {
	સ્થિતિ SBI_SSE_ATTR_CONFIG:
		જો (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			પાછા SBI_EINVALID_STATE;

		જો (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			પાછા SBI_EINVAL;

		પાછા SBI_OK;
	સ્થિતિ SBI_SSE_ATTR_PRIO:
		જો (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			પાછા SBI_EINVALID_STATE;

#જો __riscv_xlen > 32
		જો (val != (uint32_t)val)
			પાછા SBI_EINVAL;
#endif
		પાછા SBI_OK;
	સ્થિતિ SBI_SSE_ATTR_PREFERRED_HART:
		જો (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			પાછા SBI_EINVALID_STATE;

		પાછા sse_event_set_hart_id_check(e, val);
	સ્થિતિ SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		જો (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			પાછા SBI_EINVAL;
		__attribute__((__fallthrough__));
	સ્થિતિ SBI_SSE_ATTR_INTERRUPTED_SEPC:
	સ્થિતિ SBI_SSE_ATTR_INTERRUPTED_A6:
	સ્થિતિ SBI_SSE_ATTR_INTERRUPTED_A7:
		જો (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			પાછા SBI_EINVALID_STATE;

		જો (current_hartid() != e->attrs.hartid)
			પાછા SBI_EINVAL;

		પાછા SBI_OK;
	મૂળભૂત:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		પાછા SBI_EDENIED;
	}
}

સ્થિર શૂન્ય sse_event_set_attr(સંરચના sbi_sse_event *e, uint32_t attr_id,
			       unsigned long val)
{
	પસંદગી (attr_id) {
	સ્થિતિ SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		તોડો;
	સ્થિતિ SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		તોડો;
	સ્થિતિ SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_to_hartindex(val);
		sse_event_invoke_cb(e, set_hartid_cb, val);
		તોડો;

	સ્થિતિ SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		તોડો;
	સ્થિતિ SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		તોડો;
	સ્થિતિ SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		તોડો;
	સ્થિતિ SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		તોડો;
	}
}

સ્થિર પૂર્ણાંક sse_event_register(સંરચના sbi_sse_event *e,
			      unsigned long handler_entry_pc,
			      unsigned long handler_entry_arg)
{
	જો (sse_event_state(e) != SBI_SSE_STATE_UNUSED)
		પાછા SBI_EINVALID_STATE;

	e->attrs.entry.pc = handler_entry_pc;
	e->attrs.entry.arg = handler_entry_arg;

	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, register_cb);

	પાછા 0;
}

સ્થિર પૂર્ણાંક sse_event_unregister(સંરચના sbi_sse_event *e)
{
	જો (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		પાછા SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

	પાછા 0;
}

સ્થિર unsigned long sse_interrupted_flags(unsigned long mstatus)
{
	unsigned long hstatus, flags = 0;

	જો (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	જો (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	જો (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	જો (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	જો (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		જો (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		જો (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	પાછા flags;
}

સ્થિર શૂન્ય sse_event_inject(સંરચના sbi_sse_event *e,
			     સંરચના sbi_trap_regs *regs)
{
	સંરચના sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_set_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.status &= ~BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	જો (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	જો (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	જો (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
		જો (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		નહીંતર
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		જો (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
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

#જો __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#અથવાજો __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#નહીંતર
#error "Unexpected __riscv_xlen"
#endif

}

સ્થિર શૂન્ય sse_event_resume(સંરચના sbi_sse_event *e,
			     સંરચના sbi_trap_regs *regs)
{
	સંરચના sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	જો (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

	જો (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
#જો __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		જો (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#અથવાજો __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		જો (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#નહીંતર
#error "Unexpected __riscv_xlen"
#endif
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		જો (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		જો (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	જો (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	જો (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	જો (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	જો (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	જો (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

સ્થિર bool sse_event_is_ready(સંરચના sbi_sse_event *e)
{
	જો (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		પાછા false;
	}

	પાછા true;
}

સ્થિર bool sse_event_check_inject(સંરચના sbi_sse_event *e,
				   સંરચના sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	જો (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		પાછા true;
	}

	જો (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		પાછા true;
	}

	પાછા false;
}

/* Return true if an event has been injected, false otherwise */
શૂન્ય sbi_sse_process_pending_events(સંરચના sbi_trap_regs *regs)
{
	bool ret;
	સંરચના sbi_sse_event *e;
	સંરચના sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	જો (state->masked)
		પાછા;

	spin_lock(&state->enabled_event_lock);

	sbi_list_for_each_entry(e, &state->enabled_event_list, ગાંઠ) {
		ret = sse_event_check_inject(e, regs);
		જો (ret)
			જાઓ out;
	}

out:
	spin_unlock(&state->enabled_event_lock);
}

સ્થિર પૂર્ણાંક sse_event_set_pending(સંરચના sbi_sse_event *e)
{
	જો (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		પાછા SBI_EINVALID_STATE;

	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	પાછા SBI_OK;
}

સ્થિર શૂન્ય sse_ipi_inject_process(સંરચના sbi_scratch *scratch)
{
	સંરચના sbi_sse_event *e;
	સંરચના sse_ipi_inject_data evt;
	સંરચના sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	જ્યારે (!sbi_fifo_dequeue(sse_inject_fifo_r, &evt)) {
		જો (sse_event_get(evt.event_id, &e))
			ચાલુ;

		sse_event_set_pending(e);
		sse_event_put(e);
	}
}

સ્થિર સંરચના sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

સ્થિર પૂર્ણાંક sse_ipi_inject_send(unsigned long hartid, uint32_t event_id)
{
	પૂર્ણાંક ret;
	સંરચના sbi_scratch *remote_scratch = ખાલી;
	સંરચના sse_ipi_inject_data evt = {event_id};
	સંરચના sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_to_scratch(hartid);
	જો (!remote_scratch)
		પાછા SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_offset_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_enqueue(sse_inject_fifo_r, &evt, false);
	જો (ret)
		પાછા SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, ખાલી);
	જો (ret)
		પાછા SBI_EFAIL;

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક sse_inject_event(uint32_t event_id, unsigned long hartid)
{
	પૂર્ણાંક ret;
	સંરચના sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	જો (ret)
		પાછા ret;

	/* In case of global event, provided hart_id is ignored */
	જો (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	જો (hartid != current_hartid()) {
		sse_event_put(e);
		પાછા sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_set_pending(e);
	sse_event_put(e);
	જો (ret)
		પાછા ret;

	પાછા SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
સ્થિર પૂર્ણાંક sse_event_enable(સંરચના sse_hart_state *shs,
			     સંરચના sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	જો (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		પાછા SBI_EINVALID_STATE;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_add_to_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	જો (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  ખાલી);

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક sse_event_complete(સંરચના sse_hart_state *shs,
			      સંરચના sbi_sse_event *e,
			      સંરચના sbi_trap_regs *regs,
			      સંરચના sbi_ecall_return *out)
	MUST_HOLD(&shs->enabled_event_lock)
{
	જો (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		પાછા SBI_EINVALID_STATE;

	જો (e->attrs.hartid != current_hartid())
		પાછા SBI_EINVAL;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	જો (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	out->skip_regs_update = true;

	પાછા SBI_OK;
}

પૂર્ણાંક sbi_sse_complete(સંરચના sbi_trap_regs *regs, સંરચના sbi_ecall_return *out)
{
	પૂર્ણાંક ret = SBI_OK;
	સંરચના sbi_sse_event *tmp;
	સંરચના sse_hart_state *state = sse_thishart_state_ptr();

	spin_lock(&state->enabled_event_lock);
	sbi_list_for_each_entry(tmp, &state->enabled_event_list, ગાંઠ) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		જો (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, out);
			તોડો;
		}
	}
	spin_unlock(&state->enabled_event_lock);

	પાછા ret;
}

પૂર્ણાંક sbi_sse_enable(uint32_t event_id)
{
	પૂર્ણાંક ret;
	સંરચના sbi_sse_event *e;
	સંરચના sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	જો (ret)
		પાછા ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_lock);
	sse_event_put(e);

	પાછા ret;
}

પૂર્ણાંક sbi_sse_disable(uint32_t event_id)
{
	પૂર્ણાંક ret;
	સંરચના sbi_sse_event *e;
	સંરચના sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	જો (ret)
		પાછા ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_lock);

	sse_event_put(e);

	પાછા ret;
}

પૂર્ણાંક sbi_sse_hart_mask(શૂન્ય)
{
	સંરચના sse_hart_state *state = sse_thishart_state_ptr();

	જો (!state)
		પાછા SBI_EFAIL;

	જો (state->masked)
		પાછા SBI_EALREADY_STOPPED;

	state->masked = true;

	પાછા SBI_SUCCESS;
}

પૂર્ણાંક sbi_sse_hart_unmask(શૂન્ય)
{
	સંરચના sse_hart_state *state = sse_thishart_state_ptr();

	જો (!state)
		પાછા SBI_EFAIL;

	જો (!state->masked)
		પાછા SBI_EALREADY_STARTED;

	state->masked = false;

	પાછા SBI_SUCCESS;
}

પૂર્ણાંક sbi_sse_inject_from_ecall(uint32_t event_id, unsigned long hartid,
			      સંરચના sbi_ecall_return *out)
{
	જો (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_to_hartindex(hartid)))
		પાછા SBI_EINVAL;

	પાછા sse_inject_event(event_id, hartid);
}

પૂર્ણાંક sbi_sse_inject_event(uint32_t event_id)
{
	પાછા sse_inject_event(event_id, current_hartid());
}

પૂર્ણાંક sbi_sse_add_event(uint32_t event_id, અચલ સંરચના sbi_sse_cb_ops *cb_ops)
{
	સંરચના sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	જો (info)
		પાછા SBI_EALREADY;

	જો (cb_ops && cb_ops->set_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		પાછા SBI_EINVAL;

	info = sbi_zalloc(કદ(*info));
	જો (!info)
		પાછા SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ADD(info, supported_events);

	પાછા SBI_OK;
}

પૂર્ણાંક sbi_sse_attr_check(uint32_t base_attr_id, uint32_t attr_ગણતરી,
		       unsigned long phys_lo, unsigned long phys_hi,
		       unsigned long access)
{
	અચલ unsigned align = __riscv_xlen >> 3;
	uint64_t end_id = (uint64_t)base_attr_id + (attr_ગણતરી - 1);

	જો (attr_ગણતરી == 0)
		પાછા SBI_ERR_INVALID_PARAM;

	જો (end_id >= SBI_SSE_ATTR_MAX)
		પાછા SBI_EBAD_RANGE;

	જો (phys_lo & (align - 1))
		પાછા SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	જો (phys_hi)
		પાછા SBI_EINVALID_ADDR;

	જો (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 કદ(unsigned long) * attr_ગણતરી,
					 PRV_S, access))
		પાછા SBI_EINVALID_ADDR;

	પાછા SBI_OK;
}

સ્થિર શૂન્ય copy_attrs(unsigned long *out, અચલ unsigned long *in,
		       unsigned પૂર્ણાંક long_ગણતરી)
{
	પૂર્ણાંક i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	માટે (i = 0; i < long_ગણતરી; i++)
		out[i] = in[i];
}

પૂર્ણાંક sbi_sse_read_attrs(uint32_t event_id, uint32_t base_attr_id,
		       uint32_t attr_ગણતરી, unsigned long output_phys_lo,
		       unsigned long output_phys_hi)
{
	પૂર્ણાંક ret;
	unsigned long *e_attrs;
	સંરચના sbi_sse_event *e;
	unsigned long *attrs;

	ret = sbi_sse_attr_check(base_attr_id, attr_ગણતરી, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	જો (ret)
		પાછા ret;

	ret = sse_event_get(event_id, &e);
	જો (ret)
		પાછા ret;

	sbi_hart_protection_temp_map_range(output_phys_lo, કદ(unsigned long) * attr_ગણતરી);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (unsigned long *)&e->attrs;
	attrs = (unsigned long *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[base_attr_id], attr_ગણતરી);

	sbi_hart_protection_temp_unmap_range(output_phys_lo, કદ(unsigned long) * attr_ગણતરી);

	sse_event_put(e);

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક sse_write_attrs(સંરચના sbi_sse_event *e, uint32_t base_attr_id,
			   uint32_t attr_ગણતરી, unsigned long આગત_phys)
{
	પૂર્ણાંક ret = 0;
	unsigned long attr = 0, val;
	uint32_t id, end_id = base_attr_id + attr_ગણતરી;
	unsigned long *attrs = (unsigned long *)આગત_phys;

	sbi_hart_protection_temp_map_range(આગત_phys, કદ(unsigned long) * attr_ગણતરી);

	માટે (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		ret = sse_event_set_attr_check(e, id, val);
		જો (ret)
			જાઓ out;
	}

	attr = 0;
	માટે (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		sse_event_set_attr(e, id, val);
	}

out:
	sbi_hart_protection_temp_unmap_range(આગત_phys, કદ(unsigned long) * attr_ગણતરી);

	પાછા ret;
}

પૂર્ણાંક sbi_sse_write_attrs(uint32_t event_id, uint32_t base_attr_id,
			uint32_t attr_ગણતરી, unsigned long આગત_phys_lo,
			unsigned long આગત_phys_hi)
{
	પૂર્ણાંક ret = 0;
	સંરચના sbi_sse_event *e;

	ret = sbi_sse_attr_check(base_attr_id, attr_ગણતરી, આગત_phys_lo,
				 આગત_phys_hi, SBI_DOMAIN_READ);
	જો (ret)
		પાછા ret;

	ret = sse_event_get(event_id, &e);
	જો (ret)
		પાછા ret;

	ret = sse_write_attrs(e, base_attr_id, attr_ગણતરી, આગત_phys_lo);
	sse_event_put(e);

	પાછા ret;
}

પૂર્ણાંક sbi_sse_register(uint32_t event_id, unsigned long handler_entry_pc,
		     unsigned long handler_entry_arg)
{
	પૂર્ણાંક ret;
	સંરચના sbi_sse_event *e;

	જો (handler_entry_pc & 0x1)
		પાછા SBI_EINVAL;

	જો (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_entry_pc,
					 કદ(unsigned long), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		પાછા SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	જો (ret)
		પાછા ret;

	ret = sse_event_register(e, handler_entry_pc, handler_entry_arg);
	sse_event_put(e);

	પાછા ret;
}

પૂર્ણાંક sbi_sse_unregister(uint32_t event_id)
{
	પૂર્ણાંક ret;
	સંરચના sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	જો (ret)
		પાછા ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	પાછા ret;
}

સ્થિર શૂન્ય sse_event_init(સંરચના sbi_sse_event *e, સંરચના sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_INJECT_OFFSET);
}

સ્થિર શૂન્ય sse_event_ગણતરી_init()
{
	સંરચના sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		જો (EVENT_IS_GLOBAL(info->event_id))
			global_event_ગણતરી++;
		નહીંતર
			local_event_ગણતરી++;
	}
}

સ્થિર પૂર્ણાંક sse_global_init()
{
	સંરચના sbi_sse_event *e;
	unsigned પૂર્ણાંક ev = 0;
	સંરચના sse_event_info *info;

	global_events = sbi_zalloc(કદ(*global_events) * global_event_ગણતરી);
	જો (!global_events)
		પાછા SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		જો (!EVENT_IS_GLOBAL(info->event_id))
			ચાલુ;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_LOCK_INIT(global_events[ev].lock);

		ev++;
	}

	પાછા 0;
}

સ્થિર શૂન્ય sse_local_init(સંરચના sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	unsigned પૂર્ણાંક ev = 0;
	સંરચના sse_event_info *info;

	SBI_INIT_LIST_HEAD(&shs->enabled_event_list);
	SPIN_LOCK_INIT(shs->enabled_event_lock);

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		જો (EVENT_IS_GLOBAL(info->event_id))
			ચાલુ;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

પૂર્ણાંક sbi_sse_init(સંરચના sbi_scratch *scratch, bool cold_boot)
{
	પૂર્ણાંક ret;
	શૂન્ય *sse_inject_mem;
	સંરચના sse_hart_state *shs;
	સંરચના sbi_fifo *sse_inject_q;

	જો (cold_boot) {
		sse_event_ગણતરી_init();

		ret = sse_global_init();
		જો (ret)
			પાછા ret;

		shs_ptr_off = sbi_scratch_alloc_offset(કદ(શૂન્ય *));
		જો (!shs_ptr_off)
			પાછા SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_offset(કદ(*sse_inject_q));
		જો (!sse_inject_fifo_off) {
			sbi_scratch_free_offset(shs_ptr_off);
			પાછા SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_offset(
			(global_event_ગણતરી + local_event_ગણતરી) *
			કદ(સંરચના sse_ipi_inject_data));
		જો (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_offset(sse_inject_fifo_off);
			sbi_scratch_free_offset(shs_ptr_off);
			પાછા SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		જો (ret < 0) {
			sbi_scratch_free_offset(shs_ptr_off);
			પાછા ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	જો (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(કદ(*shs) + કદ(સંરચના sbi_sse_event) *
							local_event_ગણતરી);
		જો (!shs)
			પાછા SBI_ENOMEM;

		shs->local_events = (સંરચના sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_set_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_ગણતરી + local_event_ગણતરી),
		      કદ(સંરચના sse_ipi_inject_data));

	પાછા 0;
}

શૂન્ય sbi_sse_exit(સંરચના sbi_scratch *scratch)
{
	સંરચના sbi_sse_event *e;
	સંરચના sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		જો (sse_event_get(info->event_id, &e))
			ચાલુ;

		જો (e->attrs.hartid != current_hartid())
			જાઓ skip;

		જો (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

skip:
		sse_event_put(e);
	}
}
