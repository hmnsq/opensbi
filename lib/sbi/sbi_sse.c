/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Rivos Systems Inc.
 *
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_barrier.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/riscv_locks.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_fifo.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hart_protection.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi/sbi_list.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_pmu.h>
#شامل <sbi/sbi_sse.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_slist.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_trap.h>

#شامل <sbi/sbi_console.h>

#define sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), تهی *, shs_ptr_off)

#define sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define sse_set_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), تهی *, shs_ptr_off, (__sse_state))

#define EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_BIT)

#define sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		ثابت ساختار sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		اگر (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

ساختار sse_entry_state {
	/** entry pc */
	unsigned long pc;
	/** a6 register state */
	unsigned long arg;
};

ساختار sse_interrupted_state {
	/** sepc register state */
	unsigned long sepc;
	/** flags register state */
	unsigned long flags;
	/** a6 register state */
	unsigned long a6;
	/** a7 register state */
	unsigned long a7;
};

ساختار sse_ipi_inject_data {
	uint32_t event_id;
};

ساختار sbi_sse_event_attrs {
	unsigned long status;
	unsigned long prio;
	unsigned long config;
	unsigned long hartid;
	ساختار sse_entry_state entry;
	ساختار sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)                  \
	_Static_assert(                                          \
		((offsetof(ساختار sbi_sse_event_attrs, field)) / \
		 اندازه(unsigned long)) == attr_offset,          \
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

ساختار sbi_sse_event {
	ساختار sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	ساختار sse_event_info *info;
	ساختار sbi_dlist گره;
};

/** Per-hart state */
ساختار sse_hart_state {
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
	ساختار sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_lock);

	/**
	 * List of local events allocated at boot time.
	 */
	ساختار sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
ساختار sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t lock;

	/**
	 * global event struct
	 */
	ساختار sbi_sse_event event GUARDED_BY(&lock);
};

ساختار sse_event_info {
	uint32_t event_id;
	ثابت ساختار sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_گره(sse_event_info);
};

ایستا unsigned صحیح local_event_شمار;
ایستا unsigned صحیح global_event_شمار;
ایستا ساختار sse_global_event *global_events;

ایستا unsigned long sse_inject_fifo_off;
ایستا unsigned long sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
ایستا unsigned long shs_ptr_off;

ایستا u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

ایستا صحیح sse_ipi_inject_send(unsigned long hartid, uint32_t event_id);

ساختار sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_گره_INIT(پوچ),
};

ساختار sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_گره_INIT(&global_software_event),
};

ایستا SBI_SLIST_HEAD(supported_events, sse_event_info) =
				SBI_SLIST_HEAD_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
ایستا ثابت uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_بالا_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_بالا_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW,
	SBI_SSE_EVENT_LOCAL_کم_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_کم_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

ایستا bool sse_is_standard_event(uint32_t event_id)
{
	صحیح i;

	برای (i = 0; i < array_size(standard_events); i++) {
		اگر (event_id == standard_events[i])
			بازگشت true;
	}

	بازگشت false;
}

ایستا ساختار sse_event_info *sse_event_info_get(uint32_t event_id)
{
	ساختار sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		اگر (info->event_id == event_id)
			بازگشت info;
	}

	بازگشت پوچ;
}

ایستا unsigned long sse_event_state(ساختار sbi_sse_event *e)
{
	بازگشت e->attrs.status & SBI_SSE_ATTR_STATUS_STATE_MASK;
}

ایستا unsigned long sse_event_pending(ساختار sbi_sse_event *e)
{
	بازگشت !!(e->attrs.status & BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET));
}

ایستا bool sse_event_is_global(ساختار sbi_sse_event *e)
{
	بازگشت EVENT_IS_GLOBAL(e->event_id);
}

ایستا bool sse_event_is_local(ساختار sbi_sse_event *e)
{
	بازگشت !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
ایستا ساختار sse_hart_state *sse_get_hart_state(ساختار sbi_sse_event *e)
{
	ساختار sbi_scratch *s = sbi_hartindex_to_scratch(e->hartindex);

	بازگشت sse_get_hart_state_ptr(s);
}

ایستا ساختار sse_global_event *sse_get_global_event(ساختار sbi_sse_event *e)
{
	بازگشت container_of(e, ساختار sse_global_event, event);
}

ایستا تهی sse_event_set_state(ساختار sbi_sse_event *e,
				unsigned long new_state)
{
	e->attrs.status &= ~SBI_SSE_ATTR_STATUS_STATE_MASK;
	e->attrs.status |= new_state;
}

ایستا صحیح sse_event_get(uint32_t event_id,
			 ساختار sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	unsigned صحیح i;
	ساختار sbi_sse_event *e;
	ساختار sse_hart_state *shs;

	اگر (!eret)
		بازگشت SBI_EINVAL;

	اگر (EVENT_IS_GLOBAL(event_id)) {
		برای (i = 0; i < global_event_شمار; i++) {
			e = &global_events[i].event;
			اگر (e->event_id == event_id) {
				spin_lock(&global_events[i].lock);
				*eret = e;
				بازگشت SBI_SUCCESS;
			}
		}
	} وگرنه {
		shs = sse_thishart_state_ptr();
		برای (i = 0; i < local_event_شمار; i++) {
			e = &shs->local_events[i];
			اگر (e->event_id == event_id) {
				*eret = e;
				بازگشت SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	اگر (sse_is_standard_event(event_id))
		بازگشت SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	بازگشت SBI_EINVAL;
}

ایستا تهی sse_event_put(ساختار sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	ساختار sse_global_event *ge;

	اگر (sse_event_is_local(e))
		بازگشت;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->lock);
}

ایستا تهی sse_event_remove_from_list(ساختار sbi_sse_event *e)
{
	sbi_list_del(&e->گره);
}

/**
 * Must be called under owner hart lock
 */
ایستا تهی sse_event_add_to_list(ساختار sse_hart_state *state,
				   ساختار sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_lock)
{
	ساختار sbi_sse_event *tmp;

	sbi_list_for_each_entry(tmp, &state->enabled_event_list, گره) {
		اگر (e->attrs.prio < tmp->attrs.prio)
			بشکن;
		اگر (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			بشکن;
	}
	sbi_list_add_tail(&e->گره, &tmp->گره);
}

/**
 * Must be called under owner hart lock
 */
ایستا صحیح sse_event_disable(ساختار sse_hart_state *shs,
			      ساختار sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	اگر (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		بازگشت SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	بازگشت SBI_OK;
}

ایستا صحیح sse_event_set_hart_id_check(ساختار sbi_sse_event *e,
				       unsigned long new_hartid)
{
	صحیح hstate;
	unsigned صحیح hartid = (uint32_t)new_hartid;
	ساختار sbi_domain *hd = sbi_domain_thishart_ptr();

	اگر (!sse_event_is_global(e))
		بازگشت SBI_EDENIED;

	اگر (!sbi_domain_is_assigned_hart(hd, sbi_hartid_to_hartindex(hartid)))
		بازگشت SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	اگر (hstate != SBI_HSM_STATE_STARTED)
		بازگشت SBI_EINVAL;

	بازگشت SBI_OK;
}

ایستا صحیح sse_event_set_attr_check(ساختار sbi_sse_event *e, uint32_t attr_id,
				    unsigned long val)
{
	انتخاب (attr_id) {
	حالت SBI_SSE_ATTR_CONFIG:
		اگر (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			بازگشت SBI_EINVALID_STATE;

		اگر (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			بازگشت SBI_EINVAL;

		بازگشت SBI_OK;
	حالت SBI_SSE_ATTR_PRIO:
		اگر (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			بازگشت SBI_EINVALID_STATE;

#اگر __riscv_xlen > 32
		اگر (val != (uint32_t)val)
			بازگشت SBI_EINVAL;
#endif
		بازگشت SBI_OK;
	حالت SBI_SSE_ATTR_PREFERRED_HART:
		اگر (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			بازگشت SBI_EINVALID_STATE;

		بازگشت sse_event_set_hart_id_check(e, val);
	حالت SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		اگر (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			بازگشت SBI_EINVAL;
		__attribute__((__fallthrough__));
	حالت SBI_SSE_ATTR_INTERRUPTED_SEPC:
	حالت SBI_SSE_ATTR_INTERRUPTED_A6:
	حالت SBI_SSE_ATTR_INTERRUPTED_A7:
		اگر (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			بازگشت SBI_EINVALID_STATE;

		اگر (current_hartid() != e->attrs.hartid)
			بازگشت SBI_EINVAL;

		بازگشت SBI_OK;
	پیشفرض:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		بازگشت SBI_EDENIED;
	}
}

ایستا تهی sse_event_set_attr(ساختار sbi_sse_event *e, uint32_t attr_id,
			       unsigned long val)
{
	انتخاب (attr_id) {
	حالت SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		بشکن;
	حالت SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		بشکن;
	حالت SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_to_hartindex(val);
		sse_event_invoke_cb(e, set_hartid_cb, val);
		بشکن;

	حالت SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		بشکن;
	حالت SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		بشکن;
	حالت SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		بشکن;
	حالت SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		بشکن;
	}
}

ایستا صحیح sse_event_register(ساختار sbi_sse_event *e,
			      unsigned long handler_entry_pc,
			      unsigned long handler_entry_arg)
{
	اگر (sse_event_state(e) != SBI_SSE_STATE_UNUSED)
		بازگشت SBI_EINVALID_STATE;

	e->attrs.entry.pc = handler_entry_pc;
	e->attrs.entry.arg = handler_entry_arg;

	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, register_cb);

	بازگشت 0;
}

ایستا صحیح sse_event_unregister(ساختار sbi_sse_event *e)
{
	اگر (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		بازگشت SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

	بازگشت 0;
}

ایستا unsigned long sse_interrupted_flags(unsigned long mstatus)
{
	unsigned long hstatus, flags = 0;

	اگر (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	اگر (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	اگر (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	اگر (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	اگر (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		اگر (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		اگر (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	بازگشت flags;
}

ایستا تهی sse_event_inject(ساختار sbi_sse_event *e,
			     ساختار sbi_trap_regs *regs)
{
	ساختار sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_set_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.status &= ~BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	اگر (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	اگر (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	اگر (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
		اگر (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		وگرنه
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		اگر (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
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

#اگر __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#وگرنهاگر __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#وگرنه
#error "Unexpected __riscv_xlen"
#endif

}

ایستا تهی sse_event_resume(ساختار sbi_sse_event *e,
			     ساختار sbi_trap_regs *regs)
{
	ساختار sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	اگر (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

	اگر (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
#اگر __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		اگر (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#وگرنهاگر __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		اگر (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#وگرنه
#error "Unexpected __riscv_xlen"
#endif
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		اگر (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		اگر (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	اگر (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	اگر (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	اگر (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	اگر (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	اگر (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

ایستا bool sse_event_is_ready(ساختار sbi_sse_event *e)
{
	اگر (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		بازگشت false;
	}

	بازگشت true;
}

ایستا bool sse_event_check_inject(ساختار sbi_sse_event *e,
				   ساختار sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	اگر (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		بازگشت true;
	}

	اگر (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		بازگشت true;
	}

	بازگشت false;
}

/* Return true if an event has been injected, false otherwise */
تهی sbi_sse_process_pending_events(ساختار sbi_trap_regs *regs)
{
	bool ret;
	ساختار sbi_sse_event *e;
	ساختار sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	اگر (state->masked)
		بازگشت;

	spin_lock(&state->enabled_event_lock);

	sbi_list_for_each_entry(e, &state->enabled_event_list, گره) {
		ret = sse_event_check_inject(e, regs);
		اگر (ret)
			برو out;
	}

out:
	spin_unlock(&state->enabled_event_lock);
}

ایستا صحیح sse_event_set_pending(ساختار sbi_sse_event *e)
{
	اگر (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		بازگشت SBI_EINVALID_STATE;

	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	بازگشت SBI_OK;
}

ایستا تهی sse_ipi_inject_process(ساختار sbi_scratch *scratch)
{
	ساختار sbi_sse_event *e;
	ساختار sse_ipi_inject_data evt;
	ساختار sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	وقتی (!sbi_fifo_dequeue(sse_inject_fifo_r, &evt)) {
		اگر (sse_event_get(evt.event_id, &e))
			ادامه;

		sse_event_set_pending(e);
		sse_event_put(e);
	}
}

ایستا ساختار sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

ایستا صحیح sse_ipi_inject_send(unsigned long hartid, uint32_t event_id)
{
	صحیح ret;
	ساختار sbi_scratch *remote_scratch = پوچ;
	ساختار sse_ipi_inject_data evt = {event_id};
	ساختار sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_to_scratch(hartid);
	اگر (!remote_scratch)
		بازگشت SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_offset_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_enqueue(sse_inject_fifo_r, &evt, false);
	اگر (ret)
		بازگشت SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, پوچ);
	اگر (ret)
		بازگشت SBI_EFAIL;

	بازگشت SBI_OK;
}

ایستا صحیح sse_inject_event(uint32_t event_id, unsigned long hartid)
{
	صحیح ret;
	ساختار sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		بازگشت ret;

	/* In case of global event, provided hart_id is ignored */
	اگر (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	اگر (hartid != current_hartid()) {
		sse_event_put(e);
		بازگشت sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_set_pending(e);
	sse_event_put(e);
	اگر (ret)
		بازگشت ret;

	بازگشت SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
ایستا صحیح sse_event_enable(ساختار sse_hart_state *shs,
			     ساختار sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	اگر (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		بازگشت SBI_EINVALID_STATE;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_add_to_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	اگر (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  پوچ);

	بازگشت SBI_OK;
}

ایستا صحیح sse_event_complete(ساختار sse_hart_state *shs,
			      ساختار sbi_sse_event *e,
			      ساختار sbi_trap_regs *regs,
			      ساختار sbi_ecall_return *out)
	MUST_HOLD(&shs->enabled_event_lock)
{
	اگر (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		بازگشت SBI_EINVALID_STATE;

	اگر (e->attrs.hartid != current_hartid())
		بازگشت SBI_EINVAL;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	اگر (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	out->skip_regs_update = true;

	بازگشت SBI_OK;
}

صحیح sbi_sse_complete(ساختار sbi_trap_regs *regs, ساختار sbi_ecall_return *out)
{
	صحیح ret = SBI_OK;
	ساختار sbi_sse_event *tmp;
	ساختار sse_hart_state *state = sse_thishart_state_ptr();

	spin_lock(&state->enabled_event_lock);
	sbi_list_for_each_entry(tmp, &state->enabled_event_list, گره) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		اگر (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, out);
			بشکن;
		}
	}
	spin_unlock(&state->enabled_event_lock);

	بازگشت ret;
}

صحیح sbi_sse_enable(uint32_t event_id)
{
	صحیح ret;
	ساختار sbi_sse_event *e;
	ساختار sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		بازگشت ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_lock);
	sse_event_put(e);

	بازگشت ret;
}

صحیح sbi_sse_disable(uint32_t event_id)
{
	صحیح ret;
	ساختار sbi_sse_event *e;
	ساختار sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		بازگشت ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_lock);

	sse_event_put(e);

	بازگشت ret;
}

صحیح sbi_sse_hart_mask(تهی)
{
	ساختار sse_hart_state *state = sse_thishart_state_ptr();

	اگر (!state)
		بازگشت SBI_EFAIL;

	اگر (state->masked)
		بازگشت SBI_EALREADY_STOPPED;

	state->masked = true;

	بازگشت SBI_SUCCESS;
}

صحیح sbi_sse_hart_unmask(تهی)
{
	ساختار sse_hart_state *state = sse_thishart_state_ptr();

	اگر (!state)
		بازگشت SBI_EFAIL;

	اگر (!state->masked)
		بازگشت SBI_EALREADY_STARTED;

	state->masked = false;

	بازگشت SBI_SUCCESS;
}

صحیح sbi_sse_inject_from_ecall(uint32_t event_id, unsigned long hartid,
			      ساختار sbi_ecall_return *out)
{
	اگر (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_to_hartindex(hartid)))
		بازگشت SBI_EINVAL;

	بازگشت sse_inject_event(event_id, hartid);
}

صحیح sbi_sse_inject_event(uint32_t event_id)
{
	بازگشت sse_inject_event(event_id, current_hartid());
}

صحیح sbi_sse_add_event(uint32_t event_id, ثابت ساختار sbi_sse_cb_ops *cb_ops)
{
	ساختار sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	اگر (info)
		بازگشت SBI_EALREADY;

	اگر (cb_ops && cb_ops->set_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		بازگشت SBI_EINVAL;

	info = sbi_zalloc(اندازه(*info));
	اگر (!info)
		بازگشت SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ADD(info, supported_events);

	بازگشت SBI_OK;
}

صحیح sbi_sse_attr_check(uint32_t base_attr_id, uint32_t attr_شمار,
		       unsigned long phys_lo, unsigned long phys_hi,
		       unsigned long access)
{
	ثابت unsigned align = __riscv_xlen >> 3;
	uint64_t end_id = (uint64_t)base_attr_id + (attr_شمار - 1);

	اگر (attr_شمار == 0)
		بازگشت SBI_ERR_INVALID_PARAM;

	اگر (end_id >= SBI_SSE_ATTR_MAX)
		بازگشت SBI_EBAD_RANGE;

	اگر (phys_lo & (align - 1))
		بازگشت SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	اگر (phys_hi)
		بازگشت SBI_EINVALID_ADDR;

	اگر (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 اندازه(unsigned long) * attr_شمار,
					 PRV_S, access))
		بازگشت SBI_EINVALID_ADDR;

	بازگشت SBI_OK;
}

ایستا تهی copy_attrs(unsigned long *out, ثابت unsigned long *in,
		       unsigned صحیح long_شمار)
{
	صحیح i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	برای (i = 0; i < long_شمار; i++)
		out[i] = in[i];
}

صحیح sbi_sse_read_attrs(uint32_t event_id, uint32_t base_attr_id,
		       uint32_t attr_شمار, unsigned long output_phys_lo,
		       unsigned long output_phys_hi)
{
	صحیح ret;
	unsigned long *e_attrs;
	ساختار sbi_sse_event *e;
	unsigned long *attrs;

	ret = sbi_sse_attr_check(base_attr_id, attr_شمار, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	اگر (ret)
		بازگشت ret;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		بازگشت ret;

	sbi_hart_protection_temp_map_range(output_phys_lo, اندازه(unsigned long) * attr_شمار);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (unsigned long *)&e->attrs;
	attrs = (unsigned long *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[base_attr_id], attr_شمار);

	sbi_hart_protection_temp_unmap_range(output_phys_lo, اندازه(unsigned long) * attr_شمار);

	sse_event_put(e);

	بازگشت SBI_OK;
}

ایستا صحیح sse_write_attrs(ساختار sbi_sse_event *e, uint32_t base_attr_id,
			   uint32_t attr_شمار, unsigned long درون_داد_phys)
{
	صحیح ret = 0;
	unsigned long attr = 0, val;
	uint32_t id, end_id = base_attr_id + attr_شمار;
	unsigned long *attrs = (unsigned long *)درون_داد_phys;

	sbi_hart_protection_temp_map_range(درون_داد_phys, اندازه(unsigned long) * attr_شمار);

	برای (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		ret = sse_event_set_attr_check(e, id, val);
		اگر (ret)
			برو out;
	}

	attr = 0;
	برای (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		sse_event_set_attr(e, id, val);
	}

out:
	sbi_hart_protection_temp_unmap_range(درون_داد_phys, اندازه(unsigned long) * attr_شمار);

	بازگشت ret;
}

صحیح sbi_sse_write_attrs(uint32_t event_id, uint32_t base_attr_id,
			uint32_t attr_شمار, unsigned long درون_داد_phys_lo,
			unsigned long درون_داد_phys_hi)
{
	صحیح ret = 0;
	ساختار sbi_sse_event *e;

	ret = sbi_sse_attr_check(base_attr_id, attr_شمار, درون_داد_phys_lo,
				 درون_داد_phys_hi, SBI_DOMAIN_READ);
	اگر (ret)
		بازگشت ret;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		بازگشت ret;

	ret = sse_write_attrs(e, base_attr_id, attr_شمار, درون_داد_phys_lo);
	sse_event_put(e);

	بازگشت ret;
}

صحیح sbi_sse_register(uint32_t event_id, unsigned long handler_entry_pc,
		     unsigned long handler_entry_arg)
{
	صحیح ret;
	ساختار sbi_sse_event *e;

	اگر (handler_entry_pc & 0x1)
		بازگشت SBI_EINVAL;

	اگر (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_entry_pc,
					 اندازه(unsigned long), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		بازگشت SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		بازگشت ret;

	ret = sse_event_register(e, handler_entry_pc, handler_entry_arg);
	sse_event_put(e);

	بازگشت ret;
}

صحیح sbi_sse_unregister(uint32_t event_id)
{
	صحیح ret;
	ساختار sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		بازگشت ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	بازگشت ret;
}

ایستا تهی sse_event_init(ساختار sbi_sse_event *e, ساختار sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_INJECT_OFFSET);
}

ایستا تهی sse_event_شمار_init()
{
	ساختار sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		اگر (EVENT_IS_GLOBAL(info->event_id))
			global_event_شمار++;
		وگرنه
			local_event_شمار++;
	}
}

ایستا صحیح sse_global_init()
{
	ساختار sbi_sse_event *e;
	unsigned صحیح ev = 0;
	ساختار sse_event_info *info;

	global_events = sbi_zalloc(اندازه(*global_events) * global_event_شمار);
	اگر (!global_events)
		بازگشت SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		اگر (!EVENT_IS_GLOBAL(info->event_id))
			ادامه;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_LOCK_INIT(global_events[ev].lock);

		ev++;
	}

	بازگشت 0;
}

ایستا تهی sse_local_init(ساختار sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	unsigned صحیح ev = 0;
	ساختار sse_event_info *info;

	SBI_INIT_LIST_HEAD(&shs->enabled_event_list);
	SPIN_LOCK_INIT(shs->enabled_event_lock);

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		اگر (EVENT_IS_GLOBAL(info->event_id))
			ادامه;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

صحیح sbi_sse_init(ساختار sbi_scratch *scratch, bool cold_boot)
{
	صحیح ret;
	تهی *sse_inject_mem;
	ساختار sse_hart_state *shs;
	ساختار sbi_fifo *sse_inject_q;

	اگر (cold_boot) {
		sse_event_شمار_init();

		ret = sse_global_init();
		اگر (ret)
			بازگشت ret;

		shs_ptr_off = sbi_scratch_alloc_offset(اندازه(تهی *));
		اگر (!shs_ptr_off)
			بازگشت SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_offset(اندازه(*sse_inject_q));
		اگر (!sse_inject_fifo_off) {
			sbi_scratch_free_offset(shs_ptr_off);
			بازگشت SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_offset(
			(global_event_شمار + local_event_شمار) *
			اندازه(ساختار sse_ipi_inject_data));
		اگر (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_offset(sse_inject_fifo_off);
			sbi_scratch_free_offset(shs_ptr_off);
			بازگشت SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		اگر (ret < 0) {
			sbi_scratch_free_offset(shs_ptr_off);
			بازگشت ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	اگر (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(اندازه(*shs) + اندازه(ساختار sbi_sse_event) *
							local_event_شمار);
		اگر (!shs)
			بازگشت SBI_ENOMEM;

		shs->local_events = (ساختار sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_set_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_شمار + local_event_شمار),
		      اندازه(ساختار sse_ipi_inject_data));

	بازگشت 0;
}

تهی sbi_sse_exit(ساختار sbi_scratch *scratch)
{
	ساختار sbi_sse_event *e;
	ساختار sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		اگر (sse_event_get(info->event_id, &e))
			ادامه;

		اگر (e->attrs.hartid != current_hartid())
			برو skip;

		اگر (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

skip:
		sse_event_put(e);
	}
}
