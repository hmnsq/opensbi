/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Rivos Systems Inc.
 *
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_barrier.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/riscv_locks.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_fifo.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_hart_protection.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <sbi/sbi_ipi.h>
#ضمن <sbi/sbi_list.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_pmu.h>
#ضمن <sbi/sbi_sse.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_slist.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_trap.h>

#ضمن <sbi/sbi_console.h>

#define sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), فراغ *, shs_ptr_off)

#define sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define sse_set_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), فراغ *, shs_ptr_off, (__sse_state))

#define EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_BIT)

#define sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		ثابت بنية sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		ܐܢ (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

بنية sse_entry_state {
	/** entry pc */
	unsigned long pc;
	/** a6 register state */
	unsigned long arg;
};

بنية sse_interrupted_state {
	/** sepc register state */
	unsigned long sepc;
	/** flags register state */
	unsigned long flags;
	/** a6 register state */
	unsigned long a6;
	/** a7 register state */
	unsigned long a7;
};

بنية sse_ipi_inject_data {
	uint32_t event_id;
};

بنية sbi_sse_event_attrs {
	unsigned long status;
	unsigned long prio;
	unsigned long config;
	unsigned long hartid;
	بنية sse_entry_state entry;
	بنية sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)                  \
	_Static_assert(                                          \
		((offsetof(بنية sbi_sse_event_attrs, field)) / \
		 حجم(unsigned long)) == attr_offset,          \
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

بنية sbi_sse_event {
	بنية sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	بنية sse_event_info *info;
	بنية sbi_dlist node;
};

/** Per-hart state */
بنية sse_hart_state {
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
	بنية sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_lock);

	/**
	 * List of local events allocated at boot time.
	 */
	بنية sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
بنية sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t lock;

	/**
	 * global event struct
	 */
	بنية sbi_sse_event event GUARDED_BY(&lock);
};

بنية sse_event_info {
	uint32_t event_id;
	ثابت بنية sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_NODE(sse_event_info);
};

ساكن unsigned صحيح local_event_count;
ساكن unsigned صحيح global_event_count;
ساكن بنية sse_global_event *global_events;

ساكن unsigned long sse_inject_fifo_off;
ساكن unsigned long sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
ساكن unsigned long shs_ptr_off;

ساكن u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

ساكن صحيح sse_ipi_inject_send(unsigned long hartid, uint32_t event_id);

بنية sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_NODE_INIT(عدم),
};

بنية sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_NODE_INIT(&global_software_event),
};

ساكن SBI_SLIST_HEAD(supported_events, sse_event_info) =
				SBI_SLIST_HEAD_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
ساكن ثابت uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_HIGH_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_HIGH_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW,
	SBI_SSE_EVENT_LOCAL_LOW_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_LOW_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

ساكن bool sse_is_standard_event(uint32_t event_id)
{
	صحيح i;

	لكل (i = 0; i < array_size(standard_events); i++) {
		ܐܢ (event_id == standard_events[i])
			ܦܢܐ true;
	}

	ܦܢܐ false;
}

ساكن بنية sse_event_info *sse_event_info_get(uint32_t event_id)
{
	بنية sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ܐܢ (info->event_id == event_id)
			ܦܢܐ info;
	}

	ܦܢܐ عدم;
}

ساكن unsigned long sse_event_state(بنية sbi_sse_event *e)
{
	ܦܢܐ e->attrs.status & SBI_SSE_ATTR_STATUS_STATE_MASK;
}

ساكن unsigned long sse_event_pending(بنية sbi_sse_event *e)
{
	ܦܢܐ !!(e->attrs.status & BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET));
}

ساكن bool sse_event_is_global(بنية sbi_sse_event *e)
{
	ܦܢܐ EVENT_IS_GLOBAL(e->event_id);
}

ساكن bool sse_event_is_local(بنية sbi_sse_event *e)
{
	ܦܢܐ !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
ساكن بنية sse_hart_state *sse_get_hart_state(بنية sbi_sse_event *e)
{
	بنية sbi_scratch *s = sbi_hartindex_to_scratch(e->hartindex);

	ܦܢܐ sse_get_hart_state_ptr(s);
}

ساكن بنية sse_global_event *sse_get_global_event(بنية sbi_sse_event *e)
{
	ܦܢܐ container_of(e, بنية sse_global_event, event);
}

ساكن فراغ sse_event_set_state(بنية sbi_sse_event *e,
				unsigned long new_state)
{
	e->attrs.status &= ~SBI_SSE_ATTR_STATUS_STATE_MASK;
	e->attrs.status |= new_state;
}

ساكن صحيح sse_event_get(uint32_t event_id,
			 بنية sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	unsigned صحيح i;
	بنية sbi_sse_event *e;
	بنية sse_hart_state *shs;

	ܐܢ (!eret)
		ܦܢܐ SBI_EINVAL;

	ܐܢ (EVENT_IS_GLOBAL(event_id)) {
		لكل (i = 0; i < global_event_count; i++) {
			e = &global_events[i].event;
			ܐܢ (e->event_id == event_id) {
				spin_lock(&global_events[i].lock);
				*eret = e;
				ܦܢܐ SBI_SUCCESS;
			}
		}
	} ܐܚܪܢܐ {
		shs = sse_thishart_state_ptr();
		لكل (i = 0; i < local_event_count; i++) {
			e = &shs->local_events[i];
			ܐܢ (e->event_id == event_id) {
				*eret = e;
				ܦܢܐ SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	ܐܢ (sse_is_standard_event(event_id))
		ܦܢܐ SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	ܦܢܐ SBI_EINVAL;
}

ساكن فراغ sse_event_put(بنية sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	بنية sse_global_event *ge;

	ܐܢ (sse_event_is_local(e))
		ܦܢܐ;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->lock);
}

ساكن فراغ sse_event_remove_from_list(بنية sbi_sse_event *e)
{
	sbi_list_del(&e->node);
}

/**
 * Must be called under owner hart lock
 */
ساكن فراغ sse_event_add_to_list(بنية sse_hart_state *state,
				   بنية sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_lock)
{
	بنية sbi_sse_event *tmp;

	sbi_list_for_each_entry(tmp, &state->enabled_event_list, node) {
		ܐܢ (e->attrs.prio < tmp->attrs.prio)
			اكسر;
		ܐܢ (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			اكسر;
	}
	sbi_list_add_tail(&e->node, &tmp->node);
}

/**
 * Must be called under owner hart lock
 */
ساكن صحيح sse_event_disable(بنية sse_hart_state *shs,
			      بنية sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	ܐܢ (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ܦܢܐ SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	ܦܢܐ SBI_OK;
}

ساكن صحيح sse_event_set_hart_id_check(بنية sbi_sse_event *e,
				       unsigned long new_hartid)
{
	صحيح hstate;
	unsigned صحيح hartid = (uint32_t)new_hartid;
	بنية sbi_domain *hd = sbi_domain_thishart_ptr();

	ܐܢ (!sse_event_is_global(e))
		ܦܢܐ SBI_EDENIED;

	ܐܢ (!sbi_domain_is_assigned_hart(hd, sbi_hartid_to_hartindex(hartid)))
		ܦܢܐ SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	ܐܢ (hstate != SBI_HSM_STATE_STARTED)
		ܦܢܐ SBI_EINVAL;

	ܦܢܐ SBI_OK;
}

ساكن صحيح sse_event_set_attr_check(بنية sbi_sse_event *e, uint32_t attr_id,
				    unsigned long val)
{
	بدل (attr_id) {
	حالة SBI_SSE_ATTR_CONFIG:
		ܐܢ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ܦܢܐ SBI_EINVALID_STATE;

		ܐܢ (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			ܦܢܐ SBI_EINVAL;

		ܦܢܐ SBI_OK;
	حالة SBI_SSE_ATTR_PRIO:
		ܐܢ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ܦܢܐ SBI_EINVALID_STATE;

#ܐܢ __riscv_xlen > 32
		ܐܢ (val != (uint32_t)val)
			ܦܢܐ SBI_EINVAL;
#endif
		ܦܢܐ SBI_OK;
	حالة SBI_SSE_ATTR_PREFERRED_HART:
		ܐܢ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ܦܢܐ SBI_EINVALID_STATE;

		ܦܢܐ sse_event_set_hart_id_check(e, val);
	حالة SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		ܐܢ (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			ܦܢܐ SBI_EINVAL;
		__attribute__((__fallthrough__));
	حالة SBI_SSE_ATTR_INTERRUPTED_SEPC:
	حالة SBI_SSE_ATTR_INTERRUPTED_A6:
	حالة SBI_SSE_ATTR_INTERRUPTED_A7:
		ܐܢ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			ܦܢܐ SBI_EINVALID_STATE;

		ܐܢ (current_hartid() != e->attrs.hartid)
			ܦܢܐ SBI_EINVAL;

		ܦܢܐ SBI_OK;
	افتراضي:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		ܦܢܐ SBI_EDENIED;
	}
}

ساكن فراغ sse_event_set_attr(بنية sbi_sse_event *e, uint32_t attr_id,
			       unsigned long val)
{
	بدل (attr_id) {
	حالة SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		اكسر;
	حالة SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		اكسر;
	حالة SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_to_hartindex(val);
		sse_event_invoke_cb(e, set_hartid_cb, val);
		اكسر;

	حالة SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		اكسر;
	حالة SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		اكسر;
	حالة SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		اكسر;
	حالة SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		اكسر;
	}
}

ساكن صحيح sse_event_register(بنية sbi_sse_event *e,
			      unsigned long handler_entry_pc,
			      unsigned long handler_entry_arg)
{
	ܐܢ (sse_event_state(e) != SBI_SSE_STATE_UNUSED)
		ܦܢܐ SBI_EINVALID_STATE;

	e->attrs.entry.pc = handler_entry_pc;
	e->attrs.entry.arg = handler_entry_arg;

	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, register_cb);

	ܦܢܐ 0;
}

ساكن صحيح sse_event_unregister(بنية sbi_sse_event *e)
{
	ܐܢ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ܦܢܐ SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

	ܦܢܐ 0;
}

ساكن unsigned long sse_interrupted_flags(unsigned long mstatus)
{
	unsigned long hstatus, flags = 0;

	ܐܢ (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	ܐܢ (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	ܐܢ (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	ܐܢ (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	ܐܢ (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		ܐܢ (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		ܐܢ (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	ܦܢܐ flags;
}

ساكن فراغ sse_event_inject(بنية sbi_sse_event *e,
			     بنية sbi_trap_regs *regs)
{
	بنية sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_set_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.status &= ~BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	ܐܢ (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	ܐܢ (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	ܐܢ (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
		ܐܢ (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		ܐܚܪܢܐ
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		ܐܢ (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
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

#ܐܢ __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#وإلاإذا __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#ܐܚܪܢܐ
#error "Unexpected __riscv_xlen"
#endif

}

ساكن فراغ sse_event_resume(بنية sbi_sse_event *e,
			     بنية sbi_trap_regs *regs)
{
	بنية sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	ܐܢ (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

	ܐܢ (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
#ܐܢ __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		ܐܢ (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#وإلاإذا __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		ܐܢ (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#ܐܚܪܢܐ
#error "Unexpected __riscv_xlen"
#endif
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		ܐܢ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		ܐܢ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	ܐܢ (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	ܐܢ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	ܐܢ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	ܐܢ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	ܐܢ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

ساكن bool sse_event_is_ready(بنية sbi_sse_event *e)
{
	ܐܢ (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		ܦܢܐ false;
	}

	ܦܢܐ true;
}

ساكن bool sse_event_check_inject(بنية sbi_sse_event *e,
				   بنية sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	ܐܢ (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		ܦܢܐ true;
	}

	ܐܢ (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		ܦܢܐ true;
	}

	ܦܢܐ false;
}

/* Return true if an event has been injected, false otherwise */
فراغ sbi_sse_process_pending_events(بنية sbi_trap_regs *regs)
{
	bool ret;
	بنية sbi_sse_event *e;
	بنية sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	ܐܢ (state->masked)
		ܦܢܐ;

	spin_lock(&state->enabled_event_lock);

	sbi_list_for_each_entry(e, &state->enabled_event_list, node) {
		ret = sse_event_check_inject(e, regs);
		ܐܢ (ret)
			اذهب out;
	}

out:
	spin_unlock(&state->enabled_event_lock);
}

ساكن صحيح sse_event_set_pending(بنية sbi_sse_event *e)
{
	ܐܢ (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ܦܢܐ SBI_EINVALID_STATE;

	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	ܦܢܐ SBI_OK;
}

ساكن فراغ sse_ipi_inject_process(بنية sbi_scratch *scratch)
{
	بنية sbi_sse_event *e;
	بنية sse_ipi_inject_data evt;
	بنية sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	ܟܡܐ (!sbi_fifo_dequeue(sse_inject_fifo_r, &evt)) {
		ܐܢ (sse_event_get(evt.event_id, &e))
			استمر;

		sse_event_set_pending(e);
		sse_event_put(e);
	}
}

ساكن بنية sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

ساكن صحيح sse_ipi_inject_send(unsigned long hartid, uint32_t event_id)
{
	صحيح ret;
	بنية sbi_scratch *remote_scratch = عدم;
	بنية sse_ipi_inject_data evt = {event_id};
	بنية sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_to_scratch(hartid);
	ܐܢ (!remote_scratch)
		ܦܢܐ SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_offset_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_enqueue(sse_inject_fifo_r, &evt, false);
	ܐܢ (ret)
		ܦܢܐ SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, عدم);
	ܐܢ (ret)
		ܦܢܐ SBI_EFAIL;

	ܦܢܐ SBI_OK;
}

ساكن صحيح sse_inject_event(uint32_t event_id, unsigned long hartid)
{
	صحيح ret;
	بنية sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	ܐܢ (ret)
		ܦܢܐ ret;

	/* In case of global event, provided hart_id is ignored */
	ܐܢ (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	ܐܢ (hartid != current_hartid()) {
		sse_event_put(e);
		ܦܢܐ sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_set_pending(e);
	sse_event_put(e);
	ܐܢ (ret)
		ܦܢܐ ret;

	ܦܢܐ SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
ساكن صحيح sse_event_enable(بنية sse_hart_state *shs,
			     بنية sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	ܐܢ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ܦܢܐ SBI_EINVALID_STATE;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_add_to_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	ܐܢ (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  عدم);

	ܦܢܐ SBI_OK;
}

ساكن صحيح sse_event_complete(بنية sse_hart_state *shs,
			      بنية sbi_sse_event *e,
			      بنية sbi_trap_regs *regs,
			      بنية sbi_ecall_return *out)
	MUST_HOLD(&shs->enabled_event_lock)
{
	ܐܢ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		ܦܢܐ SBI_EINVALID_STATE;

	ܐܢ (e->attrs.hartid != current_hartid())
		ܦܢܐ SBI_EINVAL;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	ܐܢ (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	out->skip_regs_update = true;

	ܦܢܐ SBI_OK;
}

صحيح sbi_sse_complete(بنية sbi_trap_regs *regs, بنية sbi_ecall_return *out)
{
	صحيح ret = SBI_OK;
	بنية sbi_sse_event *tmp;
	بنية sse_hart_state *state = sse_thishart_state_ptr();

	spin_lock(&state->enabled_event_lock);
	sbi_list_for_each_entry(tmp, &state->enabled_event_list, node) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		ܐܢ (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, out);
			اكسر;
		}
	}
	spin_unlock(&state->enabled_event_lock);

	ܦܢܐ ret;
}

صحيح sbi_sse_enable(uint32_t event_id)
{
	صحيح ret;
	بنية sbi_sse_event *e;
	بنية sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	ܐܢ (ret)
		ܦܢܐ ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_lock);
	sse_event_put(e);

	ܦܢܐ ret;
}

صحيح sbi_sse_disable(uint32_t event_id)
{
	صحيح ret;
	بنية sbi_sse_event *e;
	بنية sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	ܐܢ (ret)
		ܦܢܐ ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_lock);

	sse_event_put(e);

	ܦܢܐ ret;
}

صحيح sbi_sse_hart_mask(فراغ)
{
	بنية sse_hart_state *state = sse_thishart_state_ptr();

	ܐܢ (!state)
		ܦܢܐ SBI_EFAIL;

	ܐܢ (state->masked)
		ܦܢܐ SBI_EALREADY_STOPPED;

	state->masked = true;

	ܦܢܐ SBI_SUCCESS;
}

صحيح sbi_sse_hart_unmask(فراغ)
{
	بنية sse_hart_state *state = sse_thishart_state_ptr();

	ܐܢ (!state)
		ܦܢܐ SBI_EFAIL;

	ܐܢ (!state->masked)
		ܦܢܐ SBI_EALREADY_STARTED;

	state->masked = false;

	ܦܢܐ SBI_SUCCESS;
}

صحيح sbi_sse_inject_from_ecall(uint32_t event_id, unsigned long hartid,
			      بنية sbi_ecall_return *out)
{
	ܐܢ (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_to_hartindex(hartid)))
		ܦܢܐ SBI_EINVAL;

	ܦܢܐ sse_inject_event(event_id, hartid);
}

صحيح sbi_sse_inject_event(uint32_t event_id)
{
	ܦܢܐ sse_inject_event(event_id, current_hartid());
}

صحيح sbi_sse_add_event(uint32_t event_id, ثابت بنية sbi_sse_cb_ops *cb_ops)
{
	بنية sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	ܐܢ (info)
		ܦܢܐ SBI_EALREADY;

	ܐܢ (cb_ops && cb_ops->set_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		ܦܢܐ SBI_EINVAL;

	info = sbi_zalloc(حجم(*info));
	ܐܢ (!info)
		ܦܢܐ SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ADD(info, supported_events);

	ܦܢܐ SBI_OK;
}

صحيح sbi_sse_attr_check(uint32_t base_attr_id, uint32_t attr_count,
		       unsigned long phys_lo, unsigned long phys_hi,
		       unsigned long access)
{
	ثابت unsigned align = __riscv_xlen >> 3;
	uint64_t end_id = (uint64_t)base_attr_id + (attr_count - 1);

	ܐܢ (attr_count == 0)
		ܦܢܐ SBI_ERR_INVALID_PARAM;

	ܐܢ (end_id >= SBI_SSE_ATTR_MAX)
		ܦܢܐ SBI_EBAD_RANGE;

	ܐܢ (phys_lo & (align - 1))
		ܦܢܐ SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	ܐܢ (phys_hi)
		ܦܢܐ SBI_EINVALID_ADDR;

	ܐܢ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 حجم(unsigned long) * attr_count,
					 PRV_S, access))
		ܦܢܐ SBI_EINVALID_ADDR;

	ܦܢܐ SBI_OK;
}

ساكن فراغ copy_attrs(unsigned long *out, ثابت unsigned long *in,
		       unsigned صحيح long_count)
{
	صحيح i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	لكل (i = 0; i < long_count; i++)
		out[i] = in[i];
}

صحيح sbi_sse_read_attrs(uint32_t event_id, uint32_t base_attr_id,
		       uint32_t attr_count, unsigned long output_phys_lo,
		       unsigned long output_phys_hi)
{
	صحيح ret;
	unsigned long *e_attrs;
	بنية sbi_sse_event *e;
	unsigned long *attrs;

	ret = sbi_sse_attr_check(base_attr_id, attr_count, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	ܐܢ (ret)
		ܦܢܐ ret;

	ret = sse_event_get(event_id, &e);
	ܐܢ (ret)
		ܦܢܐ ret;

	sbi_hart_protection_temp_map_range(output_phys_lo, حجم(unsigned long) * attr_count);

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

	sbi_hart_protection_temp_unmap_range(output_phys_lo, حجم(unsigned long) * attr_count);

	sse_event_put(e);

	ܦܢܐ SBI_OK;
}

ساكن صحيح sse_write_attrs(بنية sbi_sse_event *e, uint32_t base_attr_id,
			   uint32_t attr_count, unsigned long input_phys)
{
	صحيح ret = 0;
	unsigned long attr = 0, val;
	uint32_t id, end_id = base_attr_id + attr_count;
	unsigned long *attrs = (unsigned long *)input_phys;

	sbi_hart_protection_temp_map_range(input_phys, حجم(unsigned long) * attr_count);

	لكل (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		ret = sse_event_set_attr_check(e, id, val);
		ܐܢ (ret)
			اذهب out;
	}

	attr = 0;
	لكل (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		sse_event_set_attr(e, id, val);
	}

out:
	sbi_hart_protection_temp_unmap_range(input_phys, حجم(unsigned long) * attr_count);

	ܦܢܐ ret;
}

صحيح sbi_sse_write_attrs(uint32_t event_id, uint32_t base_attr_id,
			uint32_t attr_count, unsigned long input_phys_lo,
			unsigned long input_phys_hi)
{
	صحيح ret = 0;
	بنية sbi_sse_event *e;

	ret = sbi_sse_attr_check(base_attr_id, attr_count, input_phys_lo,
				 input_phys_hi, SBI_DOMAIN_READ);
	ܐܢ (ret)
		ܦܢܐ ret;

	ret = sse_event_get(event_id, &e);
	ܐܢ (ret)
		ܦܢܐ ret;

	ret = sse_write_attrs(e, base_attr_id, attr_count, input_phys_lo);
	sse_event_put(e);

	ܦܢܐ ret;
}

صحيح sbi_sse_register(uint32_t event_id, unsigned long handler_entry_pc,
		     unsigned long handler_entry_arg)
{
	صحيح ret;
	بنية sbi_sse_event *e;

	ܐܢ (handler_entry_pc & 0x1)
		ܦܢܐ SBI_EINVAL;

	ܐܢ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_entry_pc,
					 حجم(unsigned long), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		ܦܢܐ SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	ܐܢ (ret)
		ܦܢܐ ret;

	ret = sse_event_register(e, handler_entry_pc, handler_entry_arg);
	sse_event_put(e);

	ܦܢܐ ret;
}

صحيح sbi_sse_unregister(uint32_t event_id)
{
	صحيح ret;
	بنية sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	ܐܢ (ret)
		ܦܢܐ ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	ܦܢܐ ret;
}

ساكن فراغ sse_event_init(بنية sbi_sse_event *e, بنية sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_INJECT_OFFSET);
}

ساكن فراغ sse_event_count_init()
{
	بنية sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ܐܢ (EVENT_IS_GLOBAL(info->event_id))
			global_event_count++;
		ܐܚܪܢܐ
			local_event_count++;
	}
}

ساكن صحيح sse_global_init()
{
	بنية sbi_sse_event *e;
	unsigned صحيح ev = 0;
	بنية sse_event_info *info;

	global_events = sbi_zalloc(حجم(*global_events) * global_event_count);
	ܐܢ (!global_events)
		ܦܢܐ SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ܐܢ (!EVENT_IS_GLOBAL(info->event_id))
			استمر;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_LOCK_INIT(global_events[ev].lock);

		ev++;
	}

	ܦܢܐ 0;
}

ساكن فراغ sse_local_init(بنية sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	unsigned صحيح ev = 0;
	بنية sse_event_info *info;

	SBI_INIT_LIST_HEAD(&shs->enabled_event_list);
	SPIN_LOCK_INIT(shs->enabled_event_lock);

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ܐܢ (EVENT_IS_GLOBAL(info->event_id))
			استمر;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

صحيح sbi_sse_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	صحيح ret;
	فراغ *sse_inject_mem;
	بنية sse_hart_state *shs;
	بنية sbi_fifo *sse_inject_q;

	ܐܢ (cold_boot) {
		sse_event_count_init();

		ret = sse_global_init();
		ܐܢ (ret)
			ܦܢܐ ret;

		shs_ptr_off = sbi_scratch_alloc_offset(حجم(فراغ *));
		ܐܢ (!shs_ptr_off)
			ܦܢܐ SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_offset(حجم(*sse_inject_q));
		ܐܢ (!sse_inject_fifo_off) {
			sbi_scratch_free_offset(shs_ptr_off);
			ܦܢܐ SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_offset(
			(global_event_count + local_event_count) *
			حجم(بنية sse_ipi_inject_data));
		ܐܢ (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_offset(sse_inject_fifo_off);
			sbi_scratch_free_offset(shs_ptr_off);
			ܦܢܐ SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		ܐܢ (ret < 0) {
			sbi_scratch_free_offset(shs_ptr_off);
			ܦܢܐ ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	ܐܢ (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(حجم(*shs) + حجم(بنية sbi_sse_event) *
							local_event_count);
		ܐܢ (!shs)
			ܦܢܐ SBI_ENOMEM;

		shs->local_events = (بنية sbi_sse_event *)(shs + 1);

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
		      حجم(بنية sse_ipi_inject_data));

	ܦܢܐ 0;
}

فراغ sbi_sse_exit(بنية sbi_scratch *scratch)
{
	بنية sbi_sse_event *e;
	بنية sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ܐܢ (sse_event_get(info->event_id, &e))
			استمر;

		ܐܢ (e->attrs.hartid != current_hartid())
			اذهب skip;

		ܐܢ (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

skip:
		sse_event_put(e);
	}
}
