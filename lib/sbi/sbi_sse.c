/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Rivos Systems Inc.
 *
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_barrier.h>
#அந்தர்பாவ <sbi/riscv_encoding.h>
#அந்தர்பாவ <sbi/riscv_locks.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_fifo.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_hart_protection.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_hsm.h>
#அந்தர்பாவ <sbi/sbi_ipi.h>
#அந்தர்பாவ <sbi/sbi_list.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_pmu.h>
#அந்தர்பாவ <sbi/sbi_sse.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_slist.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi/sbi_trap.h>

#அந்தர்பாவ <sbi/sbi_console.h>

#define sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), வெறுமை *, shs_ptr_off)

#define sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define sse_set_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), வெறுமை *, shs_ptr_off, (__sse_state))

#define EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_BIT)

#define sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		மாறிலி கட்டமைப்பு sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		எனில் (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

கட்டமைப்பு sse_entry_state {
	/** entry pc */
	unsigned long pc;
	/** a6 register state */
	unsigned long arg;
};

கட்டமைப்பு sse_interrupted_state {
	/** sepc register state */
	unsigned long sepc;
	/** flags register state */
	unsigned long flags;
	/** a6 register state */
	unsigned long a6;
	/** a7 register state */
	unsigned long a7;
};

கட்டமைப்பு sse_ipi_inject_data {
	uint32_t event_id;
};

கட்டமைப்பு sbi_sse_event_attrs {
	unsigned long status;
	unsigned long prio;
	unsigned long config;
	unsigned long hartid;
	கட்டமைப்பு sse_entry_state entry;
	கட்டமைப்பு sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)                  \
	_Static_assert(                                          \
		((offsetof(கட்டமைப்பு sbi_sse_event_attrs, field)) / \
		 அளவு(unsigned long)) == attr_offset,          \
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

கட்டமைப்பு sbi_sse_event {
	கட்டமைப்பு sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	கட்டமைப்பு sse_event_info *info;
	கட்டமைப்பு sbi_dlist முனை;
};

/** Per-hart state */
கட்டமைப்பு sse_hart_state {
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
	கட்டமைப்பு sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_lock);

	/**
	 * List of local events allocated at boot time.
	 */
	கட்டமைப்பு sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
கட்டமைப்பு sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t lock;

	/**
	 * global event struct
	 */
	கட்டமைப்பு sbi_sse_event event GUARDED_BY(&lock);
};

கட்டமைப்பு sse_event_info {
	uint32_t event_id;
	மாறிலி கட்டமைப்பு sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_முனை(sse_event_info);
};

நிலையான unsigned முழு local_event_மொத்தம்;
நிலையான unsigned முழு global_event_மொத்தம்;
நிலையான கட்டமைப்பு sse_global_event *global_events;

நிலையான unsigned long sse_inject_fifo_off;
நிலையான unsigned long sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
நிலையான unsigned long shs_ptr_off;

நிலையான u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

நிலையான முழு sse_ipi_inject_send(unsigned long hartid, uint32_t event_id);

கட்டமைப்பு sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_முனை_INIT(வெற்று),
};

கட்டமைப்பு sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_முனை_INIT(&global_software_event),
};

நிலையான SBI_SLIST_HEAD(supported_events, sse_event_info) =
				SBI_SLIST_HEAD_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
நிலையான மாறிலி uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_உயர்வு_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_உயர்வு_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW,
	SBI_SSE_EVENT_LOCAL_கீழ்_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_கீழ்_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

நிலையான bool sse_is_standard_event(uint32_t event_id)
{
	முழு i;

	க்கு (i = 0; i < array_size(standard_events); i++) {
		எனில் (event_id == standard_events[i])
			திரும்பு true;
	}

	திரும்பு false;
}

நிலையான கட்டமைப்பு sse_event_info *sse_event_info_get(uint32_t event_id)
{
	கட்டமைப்பு sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		எனில் (info->event_id == event_id)
			திரும்பு info;
	}

	திரும்பு வெற்று;
}

நிலையான unsigned long sse_event_state(கட்டமைப்பு sbi_sse_event *e)
{
	திரும்பு e->attrs.status & SBI_SSE_ATTR_STATUS_STATE_MASK;
}

நிலையான unsigned long sse_event_pending(கட்டமைப்பு sbi_sse_event *e)
{
	திரும்பு !!(e->attrs.status & BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET));
}

நிலையான bool sse_event_is_global(கட்டமைப்பு sbi_sse_event *e)
{
	திரும்பு EVENT_IS_GLOBAL(e->event_id);
}

நிலையான bool sse_event_is_local(கட்டமைப்பு sbi_sse_event *e)
{
	திரும்பு !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
நிலையான கட்டமைப்பு sse_hart_state *sse_get_hart_state(கட்டமைப்பு sbi_sse_event *e)
{
	கட்டமைப்பு sbi_scratch *s = sbi_hartindex_to_scratch(e->hartindex);

	திரும்பு sse_get_hart_state_ptr(s);
}

நிலையான கட்டமைப்பு sse_global_event *sse_get_global_event(கட்டமைப்பு sbi_sse_event *e)
{
	திரும்பு container_of(e, கட்டமைப்பு sse_global_event, event);
}

நிலையான வெறுமை sse_event_set_state(கட்டமைப்பு sbi_sse_event *e,
				unsigned long new_state)
{
	e->attrs.status &= ~SBI_SSE_ATTR_STATUS_STATE_MASK;
	e->attrs.status |= new_state;
}

நிலையான முழு sse_event_get(uint32_t event_id,
			 கட்டமைப்பு sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	unsigned முழு i;
	கட்டமைப்பு sbi_sse_event *e;
	கட்டமைப்பு sse_hart_state *shs;

	எனில் (!eret)
		திரும்பு SBI_EINVAL;

	எனில் (EVENT_IS_GLOBAL(event_id)) {
		க்கு (i = 0; i < global_event_மொத்தம்; i++) {
			e = &global_events[i].event;
			எனில் (e->event_id == event_id) {
				spin_lock(&global_events[i].lock);
				*eret = e;
				திரும்பு SBI_SUCCESS;
			}
		}
	} இல்லையெனில் {
		shs = sse_thishart_state_ptr();
		க்கு (i = 0; i < local_event_மொத்தம்; i++) {
			e = &shs->local_events[i];
			எனில் (e->event_id == event_id) {
				*eret = e;
				திரும்பு SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	எனில் (sse_is_standard_event(event_id))
		திரும்பு SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	திரும்பு SBI_EINVAL;
}

நிலையான வெறுமை sse_event_put(கட்டமைப்பு sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	கட்டமைப்பு sse_global_event *ge;

	எனில் (sse_event_is_local(e))
		திரும்பு;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->lock);
}

நிலையான வெறுமை sse_event_remove_from_list(கட்டமைப்பு sbi_sse_event *e)
{
	sbi_list_del(&e->முனை);
}

/**
 * Must be called under owner hart lock
 */
நிலையான வெறுமை sse_event_add_to_list(கட்டமைப்பு sse_hart_state *state,
				   கட்டமைப்பு sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_lock)
{
	கட்டமைப்பு sbi_sse_event *tmp;

	sbi_list_for_each_entry(tmp, &state->enabled_event_list, முனை) {
		எனில் (e->attrs.prio < tmp->attrs.prio)
			நிறுத்து;
		எனில் (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			நிறுத்து;
	}
	sbi_list_add_tail(&e->முனை, &tmp->முனை);
}

/**
 * Must be called under owner hart lock
 */
நிலையான முழு sse_event_disable(கட்டமைப்பு sse_hart_state *shs,
			      கட்டமைப்பு sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	எனில் (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		திரும்பு SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	திரும்பு SBI_OK;
}

நிலையான முழு sse_event_set_hart_id_check(கட்டமைப்பு sbi_sse_event *e,
				       unsigned long new_hartid)
{
	முழு hstate;
	unsigned முழு hartid = (uint32_t)new_hartid;
	கட்டமைப்பு sbi_domain *hd = sbi_domain_thishart_ptr();

	எனில் (!sse_event_is_global(e))
		திரும்பு SBI_EDENIED;

	எனில் (!sbi_domain_is_assigned_hart(hd, sbi_hartid_to_hartindex(hartid)))
		திரும்பு SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	எனில் (hstate != SBI_HSM_STATE_STARTED)
		திரும்பு SBI_EINVAL;

	திரும்பு SBI_OK;
}

நிலையான முழு sse_event_set_attr_check(கட்டமைப்பு sbi_sse_event *e, uint32_t attr_id,
				    unsigned long val)
{
	தேர்வு (attr_id) {
	நிலை SBI_SSE_ATTR_CONFIG:
		எனில் (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			திரும்பு SBI_EINVALID_STATE;

		எனில் (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			திரும்பு SBI_EINVAL;

		திரும்பு SBI_OK;
	நிலை SBI_SSE_ATTR_PRIO:
		எனில் (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			திரும்பு SBI_EINVALID_STATE;

#எனில் __riscv_xlen > 32
		எனில் (val != (uint32_t)val)
			திரும்பு SBI_EINVAL;
#endif
		திரும்பு SBI_OK;
	நிலை SBI_SSE_ATTR_PREFERRED_HART:
		எனில் (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			திரும்பு SBI_EINVALID_STATE;

		திரும்பு sse_event_set_hart_id_check(e, val);
	நிலை SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		எனில் (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			திரும்பு SBI_EINVAL;
		__attribute__((__fallthrough__));
	நிலை SBI_SSE_ATTR_INTERRUPTED_SEPC:
	நிலை SBI_SSE_ATTR_INTERRUPTED_A6:
	நிலை SBI_SSE_ATTR_INTERRUPTED_A7:
		எனில் (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			திரும்பு SBI_EINVALID_STATE;

		எனில் (current_hartid() != e->attrs.hartid)
			திரும்பு SBI_EINVAL;

		திரும்பு SBI_OK;
	இயல்பு:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		திரும்பு SBI_EDENIED;
	}
}

நிலையான வெறுமை sse_event_set_attr(கட்டமைப்பு sbi_sse_event *e, uint32_t attr_id,
			       unsigned long val)
{
	தேர்வு (attr_id) {
	நிலை SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		நிறுத்து;
	நிலை SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		நிறுத்து;
	நிலை SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_to_hartindex(val);
		sse_event_invoke_cb(e, set_hartid_cb, val);
		நிறுத்து;

	நிலை SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		நிறுத்து;
	நிலை SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		நிறுத்து;
	நிலை SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		நிறுத்து;
	நிலை SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		நிறுத்து;
	}
}

நிலையான முழு sse_event_register(கட்டமைப்பு sbi_sse_event *e,
			      unsigned long handler_entry_pc,
			      unsigned long handler_entry_arg)
{
	எனில் (sse_event_state(e) != SBI_SSE_STATE_UNUSED)
		திரும்பு SBI_EINVALID_STATE;

	e->attrs.entry.pc = handler_entry_pc;
	e->attrs.entry.arg = handler_entry_arg;

	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, register_cb);

	திரும்பு 0;
}

நிலையான முழு sse_event_unregister(கட்டமைப்பு sbi_sse_event *e)
{
	எனில் (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		திரும்பு SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

	திரும்பு 0;
}

நிலையான unsigned long sse_interrupted_flags(unsigned long mstatus)
{
	unsigned long hstatus, flags = 0;

	எனில் (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	எனில் (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	எனில் (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	எனில் (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	எனில் (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		எனில் (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		எனில் (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	திரும்பு flags;
}

நிலையான வெறுமை sse_event_inject(கட்டமைப்பு sbi_sse_event *e,
			     கட்டமைப்பு sbi_trap_regs *regs)
{
	கட்டமைப்பு sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_set_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.status &= ~BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	எனில் (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	எனில் (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	எனில் (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
		எனில் (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		இல்லையெனில்
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		எனில் (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
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

#எனில் __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#அல்லதெனில் __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#இல்லையெனில்
#error "Unexpected __riscv_xlen"
#endif

}

நிலையான வெறுமை sse_event_resume(கட்டமைப்பு sbi_sse_event *e,
			     கட்டமைப்பு sbi_trap_regs *regs)
{
	கட்டமைப்பு sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	எனில் (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

	எனில் (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
#எனில் __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		எனில் (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#அல்லதெனில் __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		எனில் (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#இல்லையெனில்
#error "Unexpected __riscv_xlen"
#endif
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		எனில் (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		எனில் (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	எனில் (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	எனில் (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	எனில் (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	எனில் (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	எனில் (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

நிலையான bool sse_event_is_ready(கட்டமைப்பு sbi_sse_event *e)
{
	எனில் (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		திரும்பு false;
	}

	திரும்பு true;
}

நிலையான bool sse_event_check_inject(கட்டமைப்பு sbi_sse_event *e,
				   கட்டமைப்பு sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	எனில் (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		திரும்பு true;
	}

	எனில் (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		திரும்பு true;
	}

	திரும்பு false;
}

/* Return true if an event has been injected, false otherwise */
வெறுமை sbi_sse_process_pending_events(கட்டமைப்பு sbi_trap_regs *regs)
{
	bool ret;
	கட்டமைப்பு sbi_sse_event *e;
	கட்டமைப்பு sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	எனில் (state->masked)
		திரும்பு;

	spin_lock(&state->enabled_event_lock);

	sbi_list_for_each_entry(e, &state->enabled_event_list, முனை) {
		ret = sse_event_check_inject(e, regs);
		எனில் (ret)
			செல் out;
	}

out:
	spin_unlock(&state->enabled_event_lock);
}

நிலையான முழு sse_event_set_pending(கட்டமைப்பு sbi_sse_event *e)
{
	எனில் (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		திரும்பு SBI_EINVALID_STATE;

	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	திரும்பு SBI_OK;
}

நிலையான வெறுமை sse_ipi_inject_process(கட்டமைப்பு sbi_scratch *scratch)
{
	கட்டமைப்பு sbi_sse_event *e;
	கட்டமைப்பு sse_ipi_inject_data evt;
	கட்டமைப்பு sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	போது (!sbi_fifo_dequeue(sse_inject_fifo_r, &evt)) {
		எனில் (sse_event_get(evt.event_id, &e))
			தொடர்;

		sse_event_set_pending(e);
		sse_event_put(e);
	}
}

நிலையான கட்டமைப்பு sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

நிலையான முழு sse_ipi_inject_send(unsigned long hartid, uint32_t event_id)
{
	முழு ret;
	கட்டமைப்பு sbi_scratch *remote_scratch = வெற்று;
	கட்டமைப்பு sse_ipi_inject_data evt = {event_id};
	கட்டமைப்பு sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_to_scratch(hartid);
	எனில் (!remote_scratch)
		திரும்பு SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_offset_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_enqueue(sse_inject_fifo_r, &evt, false);
	எனில் (ret)
		திரும்பு SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, வெற்று);
	எனில் (ret)
		திரும்பு SBI_EFAIL;

	திரும்பு SBI_OK;
}

நிலையான முழு sse_inject_event(uint32_t event_id, unsigned long hartid)
{
	முழு ret;
	கட்டமைப்பு sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	எனில் (ret)
		திரும்பு ret;

	/* In case of global event, provided hart_id is ignored */
	எனில் (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	எனில் (hartid != current_hartid()) {
		sse_event_put(e);
		திரும்பு sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_set_pending(e);
	sse_event_put(e);
	எனில் (ret)
		திரும்பு ret;

	திரும்பு SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
நிலையான முழு sse_event_enable(கட்டமைப்பு sse_hart_state *shs,
			     கட்டமைப்பு sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	எனில் (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		திரும்பு SBI_EINVALID_STATE;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_add_to_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	எனில் (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  வெற்று);

	திரும்பு SBI_OK;
}

நிலையான முழு sse_event_complete(கட்டமைப்பு sse_hart_state *shs,
			      கட்டமைப்பு sbi_sse_event *e,
			      கட்டமைப்பு sbi_trap_regs *regs,
			      கட்டமைப்பு sbi_ecall_return *out)
	MUST_HOLD(&shs->enabled_event_lock)
{
	எனில் (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		திரும்பு SBI_EINVALID_STATE;

	எனில் (e->attrs.hartid != current_hartid())
		திரும்பு SBI_EINVAL;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	எனில் (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	out->skip_regs_update = true;

	திரும்பு SBI_OK;
}

முழு sbi_sse_complete(கட்டமைப்பு sbi_trap_regs *regs, கட்டமைப்பு sbi_ecall_return *out)
{
	முழு ret = SBI_OK;
	கட்டமைப்பு sbi_sse_event *tmp;
	கட்டமைப்பு sse_hart_state *state = sse_thishart_state_ptr();

	spin_lock(&state->enabled_event_lock);
	sbi_list_for_each_entry(tmp, &state->enabled_event_list, முனை) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		எனில் (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, out);
			நிறுத்து;
		}
	}
	spin_unlock(&state->enabled_event_lock);

	திரும்பு ret;
}

முழு sbi_sse_enable(uint32_t event_id)
{
	முழு ret;
	கட்டமைப்பு sbi_sse_event *e;
	கட்டமைப்பு sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	எனில் (ret)
		திரும்பு ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_lock);
	sse_event_put(e);

	திரும்பு ret;
}

முழு sbi_sse_disable(uint32_t event_id)
{
	முழு ret;
	கட்டமைப்பு sbi_sse_event *e;
	கட்டமைப்பு sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	எனில் (ret)
		திரும்பு ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_lock);

	sse_event_put(e);

	திரும்பு ret;
}

முழு sbi_sse_hart_mask(வெறுமை)
{
	கட்டமைப்பு sse_hart_state *state = sse_thishart_state_ptr();

	எனில் (!state)
		திரும்பு SBI_EFAIL;

	எனில் (state->masked)
		திரும்பு SBI_EALREADY_STOPPED;

	state->masked = true;

	திரும்பு SBI_SUCCESS;
}

முழு sbi_sse_hart_unmask(வெறுமை)
{
	கட்டமைப்பு sse_hart_state *state = sse_thishart_state_ptr();

	எனில் (!state)
		திரும்பு SBI_EFAIL;

	எனில் (!state->masked)
		திரும்பு SBI_EALREADY_STARTED;

	state->masked = false;

	திரும்பு SBI_SUCCESS;
}

முழு sbi_sse_inject_from_ecall(uint32_t event_id, unsigned long hartid,
			      கட்டமைப்பு sbi_ecall_return *out)
{
	எனில் (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_to_hartindex(hartid)))
		திரும்பு SBI_EINVAL;

	திரும்பு sse_inject_event(event_id, hartid);
}

முழு sbi_sse_inject_event(uint32_t event_id)
{
	திரும்பு sse_inject_event(event_id, current_hartid());
}

முழு sbi_sse_add_event(uint32_t event_id, மாறிலி கட்டமைப்பு sbi_sse_cb_ops *cb_ops)
{
	கட்டமைப்பு sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	எனில் (info)
		திரும்பு SBI_EALREADY;

	எனில் (cb_ops && cb_ops->set_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		திரும்பு SBI_EINVAL;

	info = sbi_zalloc(அளவு(*info));
	எனில் (!info)
		திரும்பு SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ADD(info, supported_events);

	திரும்பு SBI_OK;
}

முழு sbi_sse_attr_check(uint32_t base_attr_id, uint32_t attr_மொத்தம்,
		       unsigned long phys_lo, unsigned long phys_hi,
		       unsigned long access)
{
	மாறிலி unsigned align = __riscv_xlen >> 3;
	uint64_t end_id = (uint64_t)base_attr_id + (attr_மொத்தம் - 1);

	எனில் (attr_மொத்தம் == 0)
		திரும்பு SBI_ERR_INVALID_PARAM;

	எனில் (end_id >= SBI_SSE_ATTR_MAX)
		திரும்பு SBI_EBAD_RANGE;

	எனில் (phys_lo & (align - 1))
		திரும்பு SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	எனில் (phys_hi)
		திரும்பு SBI_EINVALID_ADDR;

	எனில் (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 அளவு(unsigned long) * attr_மொத்தம்,
					 PRV_S, access))
		திரும்பு SBI_EINVALID_ADDR;

	திரும்பு SBI_OK;
}

நிலையான வெறுமை copy_attrs(unsigned long *out, மாறிலி unsigned long *in,
		       unsigned முழு long_மொத்தம்)
{
	முழு i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	க்கு (i = 0; i < long_மொத்தம்; i++)
		out[i] = in[i];
}

முழு sbi_sse_read_attrs(uint32_t event_id, uint32_t base_attr_id,
		       uint32_t attr_மொத்தம், unsigned long output_phys_lo,
		       unsigned long output_phys_hi)
{
	முழு ret;
	unsigned long *e_attrs;
	கட்டமைப்பு sbi_sse_event *e;
	unsigned long *attrs;

	ret = sbi_sse_attr_check(base_attr_id, attr_மொத்தம், output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	எனில் (ret)
		திரும்பு ret;

	ret = sse_event_get(event_id, &e);
	எனில் (ret)
		திரும்பு ret;

	sbi_hart_protection_temp_map_range(output_phys_lo, அளவு(unsigned long) * attr_மொத்தம்);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (unsigned long *)&e->attrs;
	attrs = (unsigned long *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[base_attr_id], attr_மொத்தம்);

	sbi_hart_protection_temp_unmap_range(output_phys_lo, அளவு(unsigned long) * attr_மொத்தம்);

	sse_event_put(e);

	திரும்பு SBI_OK;
}

நிலையான முழு sse_write_attrs(கட்டமைப்பு sbi_sse_event *e, uint32_t base_attr_id,
			   uint32_t attr_மொத்தம், unsigned long உள்ளீடு_phys)
{
	முழு ret = 0;
	unsigned long attr = 0, val;
	uint32_t id, end_id = base_attr_id + attr_மொத்தம்;
	unsigned long *attrs = (unsigned long *)உள்ளீடு_phys;

	sbi_hart_protection_temp_map_range(உள்ளீடு_phys, அளவு(unsigned long) * attr_மொத்தம்);

	க்கு (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		ret = sse_event_set_attr_check(e, id, val);
		எனில் (ret)
			செல் out;
	}

	attr = 0;
	க்கு (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		sse_event_set_attr(e, id, val);
	}

out:
	sbi_hart_protection_temp_unmap_range(உள்ளீடு_phys, அளவு(unsigned long) * attr_மொத்தம்);

	திரும்பு ret;
}

முழு sbi_sse_write_attrs(uint32_t event_id, uint32_t base_attr_id,
			uint32_t attr_மொத்தம், unsigned long உள்ளீடு_phys_lo,
			unsigned long உள்ளீடு_phys_hi)
{
	முழு ret = 0;
	கட்டமைப்பு sbi_sse_event *e;

	ret = sbi_sse_attr_check(base_attr_id, attr_மொத்தம், உள்ளீடு_phys_lo,
				 உள்ளீடு_phys_hi, SBI_DOMAIN_READ);
	எனில் (ret)
		திரும்பு ret;

	ret = sse_event_get(event_id, &e);
	எனில் (ret)
		திரும்பு ret;

	ret = sse_write_attrs(e, base_attr_id, attr_மொத்தம், உள்ளீடு_phys_lo);
	sse_event_put(e);

	திரும்பு ret;
}

முழு sbi_sse_register(uint32_t event_id, unsigned long handler_entry_pc,
		     unsigned long handler_entry_arg)
{
	முழு ret;
	கட்டமைப்பு sbi_sse_event *e;

	எனில் (handler_entry_pc & 0x1)
		திரும்பு SBI_EINVAL;

	எனில் (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_entry_pc,
					 அளவு(unsigned long), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		திரும்பு SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	எனில் (ret)
		திரும்பு ret;

	ret = sse_event_register(e, handler_entry_pc, handler_entry_arg);
	sse_event_put(e);

	திரும்பு ret;
}

முழு sbi_sse_unregister(uint32_t event_id)
{
	முழு ret;
	கட்டமைப்பு sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	எனில் (ret)
		திரும்பு ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	திரும்பு ret;
}

நிலையான வெறுமை sse_event_init(கட்டமைப்பு sbi_sse_event *e, கட்டமைப்பு sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_INJECT_OFFSET);
}

நிலையான வெறுமை sse_event_மொத்தம்_init()
{
	கட்டமைப்பு sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		எனில் (EVENT_IS_GLOBAL(info->event_id))
			global_event_மொத்தம்++;
		இல்லையெனில்
			local_event_மொத்தம்++;
	}
}

நிலையான முழு sse_global_init()
{
	கட்டமைப்பு sbi_sse_event *e;
	unsigned முழு ev = 0;
	கட்டமைப்பு sse_event_info *info;

	global_events = sbi_zalloc(அளவு(*global_events) * global_event_மொத்தம்);
	எனில் (!global_events)
		திரும்பு SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		எனில் (!EVENT_IS_GLOBAL(info->event_id))
			தொடர்;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_LOCK_INIT(global_events[ev].lock);

		ev++;
	}

	திரும்பு 0;
}

நிலையான வெறுமை sse_local_init(கட்டமைப்பு sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	unsigned முழு ev = 0;
	கட்டமைப்பு sse_event_info *info;

	SBI_INIT_LIST_HEAD(&shs->enabled_event_list);
	SPIN_LOCK_INIT(shs->enabled_event_lock);

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		எனில் (EVENT_IS_GLOBAL(info->event_id))
			தொடர்;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

முழு sbi_sse_init(கட்டமைப்பு sbi_scratch *scratch, bool cold_boot)
{
	முழு ret;
	வெறுமை *sse_inject_mem;
	கட்டமைப்பு sse_hart_state *shs;
	கட்டமைப்பு sbi_fifo *sse_inject_q;

	எனில் (cold_boot) {
		sse_event_மொத்தம்_init();

		ret = sse_global_init();
		எனில் (ret)
			திரும்பு ret;

		shs_ptr_off = sbi_scratch_alloc_offset(அளவு(வெறுமை *));
		எனில் (!shs_ptr_off)
			திரும்பு SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_offset(அளவு(*sse_inject_q));
		எனில் (!sse_inject_fifo_off) {
			sbi_scratch_free_offset(shs_ptr_off);
			திரும்பு SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_offset(
			(global_event_மொத்தம் + local_event_மொத்தம்) *
			அளவு(கட்டமைப்பு sse_ipi_inject_data));
		எனில் (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_offset(sse_inject_fifo_off);
			sbi_scratch_free_offset(shs_ptr_off);
			திரும்பு SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		எனில் (ret < 0) {
			sbi_scratch_free_offset(shs_ptr_off);
			திரும்பு ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	எனில் (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(அளவு(*shs) + அளவு(கட்டமைப்பு sbi_sse_event) *
							local_event_மொத்தம்);
		எனில் (!shs)
			திரும்பு SBI_ENOMEM;

		shs->local_events = (கட்டமைப்பு sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_set_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_மொத்தம் + local_event_மொத்தம்),
		      அளவு(கட்டமைப்பு sse_ipi_inject_data));

	திரும்பு 0;
}

வெறுமை sbi_sse_exit(கட்டமைப்பு sbi_scratch *scratch)
{
	கட்டமைப்பு sbi_sse_event *e;
	கட்டமைப்பு sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		எனில் (sse_event_get(info->event_id, &e))
			தொடர்;

		எனில் (e->attrs.hartid != current_hartid())
			செல் skip;

		எனில் (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

skip:
		sse_event_put(e);
	}
}
