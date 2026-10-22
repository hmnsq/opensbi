/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Rivos Systems Inc.
 *
 */

#অন্তর্ভাৱ <sbi/riscv_asm.h>
#অন্তর্ভাৱ <sbi/riscv_barrier.h>
#অন্তর্ভাৱ <sbi/riscv_encoding.h>
#অন্তর্ভাৱ <sbi/riscv_locks.h>
#অন্তর্ভাৱ <sbi/sbi_domain.h>
#অন্তর্ভাৱ <sbi/sbi_ecall.h>
#অন্তর্ভাৱ <sbi/sbi_ecall_interface.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_fifo.h>
#অন্তর্ভাৱ <sbi/sbi_hart.h>
#অন্তর্ভাৱ <sbi/sbi_hart_protection.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi/sbi_hsm.h>
#অন্তর্ভাৱ <sbi/sbi_ipi.h>
#অন্তর্ভাৱ <sbi/sbi_list.h>
#অন্তর্ভাৱ <sbi/sbi_platform.h>
#অন্তর্ভাৱ <sbi/sbi_pmu.h>
#অন্তর্ভাৱ <sbi/sbi_sse.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi/sbi_slist.h>
#অন্তর্ভাৱ <sbi/sbi_string.h>
#অন্তর্ভাৱ <sbi/sbi_trap.h>

#অন্তর্ভাৱ <sbi/sbi_console.h>

#define sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), শূন্য *, shs_ptr_off)

#define sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define sse_set_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), শূন্য *, shs_ptr_off, (__sse_state))

#define EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_BIT)

#define sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		ধ্রুৱ সংরচনা sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		যদি (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

সংরচনা sse_entry_state {
	/** entry pc */
	unsigned long pc;
	/** a6 register state */
	unsigned long arg;
};

সংরচনা sse_interrupted_state {
	/** sepc register state */
	unsigned long sepc;
	/** flags register state */
	unsigned long flags;
	/** a6 register state */
	unsigned long a6;
	/** a7 register state */
	unsigned long a7;
};

সংরচনা sse_ipi_inject_data {
	uint32_t event_id;
};

সংরচনা sbi_sse_event_attrs {
	unsigned long status;
	unsigned long prio;
	unsigned long config;
	unsigned long hartid;
	সংরচনা sse_entry_state entry;
	সংরচনা sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)                  \
	_Static_assert(                                          \
		((offsetof(সংরচনা sbi_sse_event_attrs, field)) / \
		 পরিমাণ(unsigned long)) == attr_offset,          \
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

সংরচনা sbi_sse_event {
	সংরচনা sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	সংরচনা sse_event_info *info;
	সংরচনা sbi_dlist গ্ৰন্থি;
};

/** Per-hart state */
সংরচনা sse_hart_state {
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
	সংরচনা sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_lock);

	/**
	 * List of local events allocated at boot time.
	 */
	সংরচনা sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
সংরচনা sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t lock;

	/**
	 * global event struct
	 */
	সংরচনা sbi_sse_event event GUARDED_BY(&lock);
};

সংরচনা sse_event_info {
	uint32_t event_id;
	ধ্রুৱ সংরচনা sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_গ্ৰন্থি(sse_event_info);
};

স্থির unsigned পূর্ণাঙ্ক local_event_গুনতি;
স্থির unsigned পূর্ণাঙ্ক global_event_গুনতি;
স্থির সংরচনা sse_global_event *global_events;

স্থির unsigned long sse_inject_fifo_off;
স্থির unsigned long sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
স্থির unsigned long shs_ptr_off;

স্থির u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

স্থির পূর্ণাঙ্ক sse_ipi_inject_send(unsigned long hartid, uint32_t event_id);

সংরচনা sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_গ্ৰন্থি_INIT(রিক্ত),
};

সংরচনা sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_গ্ৰন্থি_INIT(&global_software_event),
};

স্থির SBI_SLIST_HEAD(supported_events, sse_event_info) =
				SBI_SLIST_HEAD_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
স্থির ধ্রুৱ uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_উপৰেৰ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_উপৰেৰ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW,
	SBI_SSE_EVENT_LOCAL_নিচেৰ_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_নিচেৰ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

স্থির bool sse_is_standard_event(uint32_t event_id)
{
	পূর্ণাঙ্ক i;

	বাবে (i = 0; i < array_size(standard_events); i++) {
		যদি (event_id == standard_events[i])
			ঘূৰাই true;
	}

	ঘূৰাই false;
}

স্থির সংরচনা sse_event_info *sse_event_info_get(uint32_t event_id)
{
	সংরচনা sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		যদি (info->event_id == event_id)
			ঘূৰাই info;
	}

	ঘূৰাই রিক্ত;
}

স্থির unsigned long sse_event_state(সংরচনা sbi_sse_event *e)
{
	ঘূৰাই e->attrs.status & SBI_SSE_ATTR_STATUS_STATE_MASK;
}

স্থির unsigned long sse_event_pending(সংরচনা sbi_sse_event *e)
{
	ঘূৰাই !!(e->attrs.status & BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET));
}

স্থির bool sse_event_is_global(সংরচনা sbi_sse_event *e)
{
	ঘূৰাই EVENT_IS_GLOBAL(e->event_id);
}

স্থির bool sse_event_is_local(সংরচনা sbi_sse_event *e)
{
	ঘূৰাই !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
স্থির সংরচনা sse_hart_state *sse_get_hart_state(সংরচনা sbi_sse_event *e)
{
	সংরচনা sbi_scratch *s = sbi_hartindex_to_scratch(e->hartindex);

	ঘূৰাই sse_get_hart_state_ptr(s);
}

স্থির সংরচনা sse_global_event *sse_get_global_event(সংরচনা sbi_sse_event *e)
{
	ঘূৰাই container_of(e, সংরচনা sse_global_event, event);
}

স্থির শূন্য sse_event_set_state(সংরচনা sbi_sse_event *e,
				unsigned long new_state)
{
	e->attrs.status &= ~SBI_SSE_ATTR_STATUS_STATE_MASK;
	e->attrs.status |= new_state;
}

স্থির পূর্ণাঙ্ক sse_event_get(uint32_t event_id,
			 সংরচনা sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	unsigned পূর্ণাঙ্ক i;
	সংরচনা sbi_sse_event *e;
	সংরচনা sse_hart_state *shs;

	যদি (!eret)
		ঘূৰাই SBI_EINVAL;

	যদি (EVENT_IS_GLOBAL(event_id)) {
		বাবে (i = 0; i < global_event_গুনতি; i++) {
			e = &global_events[i].event;
			যদি (e->event_id == event_id) {
				spin_lock(&global_events[i].lock);
				*eret = e;
				ঘূৰাই SBI_SUCCESS;
			}
		}
	} অন্যথা {
		shs = sse_thishart_state_ptr();
		বাবে (i = 0; i < local_event_গুনতি; i++) {
			e = &shs->local_events[i];
			যদি (e->event_id == event_id) {
				*eret = e;
				ঘূৰাই SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	যদি (sse_is_standard_event(event_id))
		ঘূৰাই SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	ঘূৰাই SBI_EINVAL;
}

স্থির শূন্য sse_event_put(সংরচনা sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	সংরচনা sse_global_event *ge;

	যদি (sse_event_is_local(e))
		ঘূৰাই;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->lock);
}

স্থির শূন্য sse_event_remove_from_list(সংরচনা sbi_sse_event *e)
{
	sbi_list_del(&e->গ্ৰন্থি);
}

/**
 * Must be called under owner hart lock
 */
স্থির শূন্য sse_event_add_to_list(সংরচনা sse_hart_state *state,
				   সংরচনা sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_lock)
{
	সংরচনা sbi_sse_event *tmp;

	sbi_list_for_each_entry(tmp, &state->enabled_event_list, গ্ৰন্থি) {
		যদি (e->attrs.prio < tmp->attrs.prio)
			ভঙ্গ;
		যদি (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			ভঙ্গ;
	}
	sbi_list_add_tail(&e->গ্ৰন্থি, &tmp->গ্ৰন্থি);
}

/**
 * Must be called under owner hart lock
 */
স্থির পূর্ণাঙ্ক sse_event_disable(সংরচনা sse_hart_state *shs,
			      সংরচনা sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	যদি (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ঘূৰাই SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক sse_event_set_hart_id_check(সংরচনা sbi_sse_event *e,
				       unsigned long new_hartid)
{
	পূর্ণাঙ্ক hstate;
	unsigned পূর্ণাঙ্ক hartid = (uint32_t)new_hartid;
	সংরচনা sbi_domain *hd = sbi_domain_thishart_ptr();

	যদি (!sse_event_is_global(e))
		ঘূৰাই SBI_EDENIED;

	যদি (!sbi_domain_is_assigned_hart(hd, sbi_hartid_to_hartindex(hartid)))
		ঘূৰাই SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	যদি (hstate != SBI_HSM_STATE_STARTED)
		ঘূৰাই SBI_EINVAL;

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক sse_event_set_attr_check(সংরচনা sbi_sse_event *e, uint32_t attr_id,
				    unsigned long val)
{
	ৱিকল্প (attr_id) {
	পক্ষ SBI_SSE_ATTR_CONFIG:
		যদি (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ঘূৰাই SBI_EINVALID_STATE;

		যদি (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			ঘূৰাই SBI_EINVAL;

		ঘূৰাই SBI_OK;
	পক্ষ SBI_SSE_ATTR_PRIO:
		যদি (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ঘূৰাই SBI_EINVALID_STATE;

#যদি __riscv_xlen > 32
		যদি (val != (uint32_t)val)
			ঘূৰাই SBI_EINVAL;
#endif
		ঘূৰাই SBI_OK;
	পক্ষ SBI_SSE_ATTR_PREFERRED_HART:
		যদি (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ঘূৰাই SBI_EINVALID_STATE;

		ঘূৰাই sse_event_set_hart_id_check(e, val);
	পক্ষ SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		যদি (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			ঘূৰাই SBI_EINVAL;
		__attribute__((__fallthrough__));
	পক্ষ SBI_SSE_ATTR_INTERRUPTED_SEPC:
	পক্ষ SBI_SSE_ATTR_INTERRUPTED_A6:
	পক্ষ SBI_SSE_ATTR_INTERRUPTED_A7:
		যদি (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			ঘূৰাই SBI_EINVALID_STATE;

		যদি (current_hartid() != e->attrs.hartid)
			ঘূৰাই SBI_EINVAL;

		ঘূৰাই SBI_OK;
	স্ৱভাৱ:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		ঘূৰাই SBI_EDENIED;
	}
}

স্থির শূন্য sse_event_set_attr(সংরচনা sbi_sse_event *e, uint32_t attr_id,
			       unsigned long val)
{
	ৱিকল্প (attr_id) {
	পক্ষ SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		ভঙ্গ;
	পক্ষ SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		ভঙ্গ;
	পক্ষ SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_to_hartindex(val);
		sse_event_invoke_cb(e, set_hartid_cb, val);
		ভঙ্গ;

	পক্ষ SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		ভঙ্গ;
	পক্ষ SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		ভঙ্গ;
	পক্ষ SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		ভঙ্গ;
	পক্ষ SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		ভঙ্গ;
	}
}

স্থির পূর্ণাঙ্ক sse_event_register(সংরচনা sbi_sse_event *e,
			      unsigned long handler_entry_pc,
			      unsigned long handler_entry_arg)
{
	যদি (sse_event_state(e) != SBI_SSE_STATE_UNUSED)
		ঘূৰাই SBI_EINVALID_STATE;

	e->attrs.entry.pc = handler_entry_pc;
	e->attrs.entry.arg = handler_entry_arg;

	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, register_cb);

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক sse_event_unregister(সংরচনা sbi_sse_event *e)
{
	যদি (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ঘূৰাই SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

	ঘূৰাই 0;
}

স্থির unsigned long sse_interrupted_flags(unsigned long mstatus)
{
	unsigned long hstatus, flags = 0;

	যদি (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	যদি (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	যদি (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	যদি (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	যদি (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		যদি (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		যদি (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	ঘূৰাই flags;
}

স্থির শূন্য sse_event_inject(সংরচনা sbi_sse_event *e,
			     সংরচনা sbi_trap_regs *regs)
{
	সংরচনা sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_set_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.status &= ~BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	যদি (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	যদি (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	যদি (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
		যদি (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		অন্যথা
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		যদি (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
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

#যদি __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#নোচেদ্যদি __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#অন্যথা
#error "Unexpected __riscv_xlen"
#endif

}

স্থির শূন্য sse_event_resume(সংরচনা sbi_sse_event *e,
			     সংরচনা sbi_trap_regs *regs)
{
	সংরচনা sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	যদি (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

	যদি (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
#যদি __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		যদি (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#নোচেদ্যদি __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		যদি (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#অন্যথা
#error "Unexpected __riscv_xlen"
#endif
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		যদি (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		যদি (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	যদি (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	যদি (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	যদি (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	যদি (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	যদি (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

স্থির bool sse_event_is_ready(সংরচনা sbi_sse_event *e)
{
	যদি (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		ঘূৰাই false;
	}

	ঘূৰাই true;
}

স্থির bool sse_event_check_inject(সংরচনা sbi_sse_event *e,
				   সংরচনা sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	যদি (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		ঘূৰাই true;
	}

	যদি (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		ঘূৰাই true;
	}

	ঘূৰাই false;
}

/* Return true if an event has been injected, false otherwise */
শূন্য sbi_sse_process_pending_events(সংরচনা sbi_trap_regs *regs)
{
	bool ret;
	সংরচনা sbi_sse_event *e;
	সংরচনা sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	যদি (state->masked)
		ঘূৰাই;

	spin_lock(&state->enabled_event_lock);

	sbi_list_for_each_entry(e, &state->enabled_event_list, গ্ৰন্থি) {
		ret = sse_event_check_inject(e, regs);
		যদি (ret)
			গচ্ছ out;
	}

out:
	spin_unlock(&state->enabled_event_lock);
}

স্থির পূর্ণাঙ্ক sse_event_set_pending(সংরচনা sbi_sse_event *e)
{
	যদি (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ঘূৰাই SBI_EINVALID_STATE;

	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	ঘূৰাই SBI_OK;
}

স্থির শূন্য sse_ipi_inject_process(সংরচনা sbi_scratch *scratch)
{
	সংরচনা sbi_sse_event *e;
	সংরচনা sse_ipi_inject_data evt;
	সংরচনা sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	যেতিয়ালৈ (!sbi_fifo_dequeue(sse_inject_fifo_r, &evt)) {
		যদি (sse_event_get(evt.event_id, &e))
			অনুৱর্ত;

		sse_event_set_pending(e);
		sse_event_put(e);
	}
}

স্থির সংরচনা sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

স্থির পূর্ণাঙ্ক sse_ipi_inject_send(unsigned long hartid, uint32_t event_id)
{
	পূর্ণাঙ্ক ret;
	সংরচনা sbi_scratch *remote_scratch = রিক্ত;
	সংরচনা sse_ipi_inject_data evt = {event_id};
	সংরচনা sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_to_scratch(hartid);
	যদি (!remote_scratch)
		ঘূৰাই SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_offset_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_enqueue(sse_inject_fifo_r, &evt, false);
	যদি (ret)
		ঘূৰাই SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, রিক্ত);
	যদি (ret)
		ঘূৰাই SBI_EFAIL;

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক sse_inject_event(uint32_t event_id, unsigned long hartid)
{
	পূর্ণাঙ্ক ret;
	সংরচনা sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	যদি (ret)
		ঘূৰাই ret;

	/* In case of global event, provided hart_id is ignored */
	যদি (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	যদি (hartid != current_hartid()) {
		sse_event_put(e);
		ঘূৰাই sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_set_pending(e);
	sse_event_put(e);
	যদি (ret)
		ঘূৰাই ret;

	ঘূৰাই SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
স্থির পূর্ণাঙ্ক sse_event_enable(সংরচনা sse_hart_state *shs,
			     সংরচনা sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	যদি (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ঘূৰাই SBI_EINVALID_STATE;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_add_to_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	যদি (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  রিক্ত);

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক sse_event_complete(সংরচনা sse_hart_state *shs,
			      সংরচনা sbi_sse_event *e,
			      সংরচনা sbi_trap_regs *regs,
			      সংরচনা sbi_ecall_return *out)
	MUST_HOLD(&shs->enabled_event_lock)
{
	যদি (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		ঘূৰাই SBI_EINVALID_STATE;

	যদি (e->attrs.hartid != current_hartid())
		ঘূৰাই SBI_EINVAL;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	যদি (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	out->skip_regs_update = true;

	ঘূৰাই SBI_OK;
}

পূর্ণাঙ্ক sbi_sse_complete(সংরচনা sbi_trap_regs *regs, সংরচনা sbi_ecall_return *out)
{
	পূর্ণাঙ্ক ret = SBI_OK;
	সংরচনা sbi_sse_event *tmp;
	সংরচনা sse_hart_state *state = sse_thishart_state_ptr();

	spin_lock(&state->enabled_event_lock);
	sbi_list_for_each_entry(tmp, &state->enabled_event_list, গ্ৰন্থি) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		যদি (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, out);
			ভঙ্গ;
		}
	}
	spin_unlock(&state->enabled_event_lock);

	ঘূৰাই ret;
}

পূর্ণাঙ্ক sbi_sse_enable(uint32_t event_id)
{
	পূর্ণাঙ্ক ret;
	সংরচনা sbi_sse_event *e;
	সংরচনা sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	যদি (ret)
		ঘূৰাই ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_lock);
	sse_event_put(e);

	ঘূৰাই ret;
}

পূর্ণাঙ্ক sbi_sse_disable(uint32_t event_id)
{
	পূর্ণাঙ্ক ret;
	সংরচনা sbi_sse_event *e;
	সংরচনা sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	যদি (ret)
		ঘূৰাই ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_lock);

	sse_event_put(e);

	ঘূৰাই ret;
}

পূর্ণাঙ্ক sbi_sse_hart_mask(শূন্য)
{
	সংরচনা sse_hart_state *state = sse_thishart_state_ptr();

	যদি (!state)
		ঘূৰাই SBI_EFAIL;

	যদি (state->masked)
		ঘূৰাই SBI_EALREADY_STOPPED;

	state->masked = true;

	ঘূৰাই SBI_SUCCESS;
}

পূর্ণাঙ্ক sbi_sse_hart_unmask(শূন্য)
{
	সংরচনা sse_hart_state *state = sse_thishart_state_ptr();

	যদি (!state)
		ঘূৰাই SBI_EFAIL;

	যদি (!state->masked)
		ঘূৰাই SBI_EALREADY_STARTED;

	state->masked = false;

	ঘূৰাই SBI_SUCCESS;
}

পূর্ণাঙ্ক sbi_sse_inject_from_ecall(uint32_t event_id, unsigned long hartid,
			      সংরচনা sbi_ecall_return *out)
{
	যদি (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_to_hartindex(hartid)))
		ঘূৰাই SBI_EINVAL;

	ঘূৰাই sse_inject_event(event_id, hartid);
}

পূর্ণাঙ্ক sbi_sse_inject_event(uint32_t event_id)
{
	ঘূৰাই sse_inject_event(event_id, current_hartid());
}

পূর্ণাঙ্ক sbi_sse_add_event(uint32_t event_id, ধ্রুৱ সংরচনা sbi_sse_cb_ops *cb_ops)
{
	সংরচনা sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	যদি (info)
		ঘূৰাই SBI_EALREADY;

	যদি (cb_ops && cb_ops->set_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		ঘূৰাই SBI_EINVAL;

	info = sbi_zalloc(পরিমাণ(*info));
	যদি (!info)
		ঘূৰাই SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ADD(info, supported_events);

	ঘূৰাই SBI_OK;
}

পূর্ণাঙ্ক sbi_sse_attr_check(uint32_t base_attr_id, uint32_t attr_গুনতি,
		       unsigned long phys_lo, unsigned long phys_hi,
		       unsigned long access)
{
	ধ্রুৱ unsigned align = __riscv_xlen >> 3;
	uint64_t end_id = (uint64_t)base_attr_id + (attr_গুনতি - 1);

	যদি (attr_গুনতি == 0)
		ঘূৰাই SBI_ERR_INVALID_PARAM;

	যদি (end_id >= SBI_SSE_ATTR_MAX)
		ঘূৰাই SBI_EBAD_RANGE;

	যদি (phys_lo & (align - 1))
		ঘূৰাই SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	যদি (phys_hi)
		ঘূৰাই SBI_EINVALID_ADDR;

	যদি (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 পরিমাণ(unsigned long) * attr_গুনতি,
					 PRV_S, access))
		ঘূৰাই SBI_EINVALID_ADDR;

	ঘূৰাই SBI_OK;
}

স্থির শূন্য copy_attrs(unsigned long *out, ধ্রুৱ unsigned long *in,
		       unsigned পূর্ণাঙ্ক long_গুনতি)
{
	পূর্ণাঙ্ক i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	বাবে (i = 0; i < long_গুনতি; i++)
		out[i] = in[i];
}

পূর্ণাঙ্ক sbi_sse_read_attrs(uint32_t event_id, uint32_t base_attr_id,
		       uint32_t attr_গুনতি, unsigned long output_phys_lo,
		       unsigned long output_phys_hi)
{
	পূর্ণাঙ্ক ret;
	unsigned long *e_attrs;
	সংরচনা sbi_sse_event *e;
	unsigned long *attrs;

	ret = sbi_sse_attr_check(base_attr_id, attr_গুনতি, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	যদি (ret)
		ঘূৰাই ret;

	ret = sse_event_get(event_id, &e);
	যদি (ret)
		ঘূৰাই ret;

	sbi_hart_protection_temp_map_range(output_phys_lo, পরিমাণ(unsigned long) * attr_গুনতি);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (unsigned long *)&e->attrs;
	attrs = (unsigned long *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[base_attr_id], attr_গুনতি);

	sbi_hart_protection_temp_unmap_range(output_phys_lo, পরিমাণ(unsigned long) * attr_গুনতি);

	sse_event_put(e);

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক sse_write_attrs(সংরচনা sbi_sse_event *e, uint32_t base_attr_id,
			   uint32_t attr_গুনতি, unsigned long প্ৰবেশ_phys)
{
	পূর্ণাঙ্ক ret = 0;
	unsigned long attr = 0, val;
	uint32_t id, end_id = base_attr_id + attr_গুনতি;
	unsigned long *attrs = (unsigned long *)প্ৰবেশ_phys;

	sbi_hart_protection_temp_map_range(প্ৰবেশ_phys, পরিমাণ(unsigned long) * attr_গুনতি);

	বাবে (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		ret = sse_event_set_attr_check(e, id, val);
		যদি (ret)
			গচ্ছ out;
	}

	attr = 0;
	বাবে (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		sse_event_set_attr(e, id, val);
	}

out:
	sbi_hart_protection_temp_unmap_range(প্ৰবেশ_phys, পরিমাণ(unsigned long) * attr_গুনতি);

	ঘূৰাই ret;
}

পূর্ণাঙ্ক sbi_sse_write_attrs(uint32_t event_id, uint32_t base_attr_id,
			uint32_t attr_গুনতি, unsigned long প্ৰবেশ_phys_lo,
			unsigned long প্ৰবেশ_phys_hi)
{
	পূর্ণাঙ্ক ret = 0;
	সংরচনা sbi_sse_event *e;

	ret = sbi_sse_attr_check(base_attr_id, attr_গুনতি, প্ৰবেশ_phys_lo,
				 প্ৰবেশ_phys_hi, SBI_DOMAIN_READ);
	যদি (ret)
		ঘূৰাই ret;

	ret = sse_event_get(event_id, &e);
	যদি (ret)
		ঘূৰাই ret;

	ret = sse_write_attrs(e, base_attr_id, attr_গুনতি, প্ৰবেশ_phys_lo);
	sse_event_put(e);

	ঘূৰাই ret;
}

পূর্ণাঙ্ক sbi_sse_register(uint32_t event_id, unsigned long handler_entry_pc,
		     unsigned long handler_entry_arg)
{
	পূর্ণাঙ্ক ret;
	সংরচনা sbi_sse_event *e;

	যদি (handler_entry_pc & 0x1)
		ঘূৰাই SBI_EINVAL;

	যদি (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_entry_pc,
					 পরিমাণ(unsigned long), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		ঘূৰাই SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	যদি (ret)
		ঘূৰাই ret;

	ret = sse_event_register(e, handler_entry_pc, handler_entry_arg);
	sse_event_put(e);

	ঘূৰাই ret;
}

পূর্ণাঙ্ক sbi_sse_unregister(uint32_t event_id)
{
	পূর্ণাঙ্ক ret;
	সংরচনা sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	যদি (ret)
		ঘূৰাই ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	ঘূৰাই ret;
}

স্থির শূন্য sse_event_init(সংরচনা sbi_sse_event *e, সংরচনা sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_INJECT_OFFSET);
}

স্থির শূন্য sse_event_গুনতি_init()
{
	সংরচনা sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		যদি (EVENT_IS_GLOBAL(info->event_id))
			global_event_গুনতি++;
		অন্যথা
			local_event_গুনতি++;
	}
}

স্থির পূর্ণাঙ্ক sse_global_init()
{
	সংরচনা sbi_sse_event *e;
	unsigned পূর্ণাঙ্ক ev = 0;
	সংরচনা sse_event_info *info;

	global_events = sbi_zalloc(পরিমাণ(*global_events) * global_event_গুনতি);
	যদি (!global_events)
		ঘূৰাই SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		যদি (!EVENT_IS_GLOBAL(info->event_id))
			অনুৱর্ত;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_LOCK_INIT(global_events[ev].lock);

		ev++;
	}

	ঘূৰাই 0;
}

স্থির শূন্য sse_local_init(সংরচনা sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	unsigned পূর্ণাঙ্ক ev = 0;
	সংরচনা sse_event_info *info;

	SBI_INIT_LIST_HEAD(&shs->enabled_event_list);
	SPIN_LOCK_INIT(shs->enabled_event_lock);

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		যদি (EVENT_IS_GLOBAL(info->event_id))
			অনুৱর্ত;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

পূর্ণাঙ্ক sbi_sse_init(সংরচনা sbi_scratch *scratch, bool cold_boot)
{
	পূর্ণাঙ্ক ret;
	শূন্য *sse_inject_mem;
	সংরচনা sse_hart_state *shs;
	সংরচনা sbi_fifo *sse_inject_q;

	যদি (cold_boot) {
		sse_event_গুনতি_init();

		ret = sse_global_init();
		যদি (ret)
			ঘূৰাই ret;

		shs_ptr_off = sbi_scratch_alloc_offset(পরিমাণ(শূন্য *));
		যদি (!shs_ptr_off)
			ঘূৰাই SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_offset(পরিমাণ(*sse_inject_q));
		যদি (!sse_inject_fifo_off) {
			sbi_scratch_free_offset(shs_ptr_off);
			ঘূৰাই SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_offset(
			(global_event_গুনতি + local_event_গুনতি) *
			পরিমাণ(সংরচনা sse_ipi_inject_data));
		যদি (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_offset(sse_inject_fifo_off);
			sbi_scratch_free_offset(shs_ptr_off);
			ঘূৰাই SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		যদি (ret < 0) {
			sbi_scratch_free_offset(shs_ptr_off);
			ঘূৰাই ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	যদি (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(পরিমাণ(*shs) + পরিমাণ(সংরচনা sbi_sse_event) *
							local_event_গুনতি);
		যদি (!shs)
			ঘূৰাই SBI_ENOMEM;

		shs->local_events = (সংরচনা sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_set_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_গুনতি + local_event_গুনতি),
		      পরিমাণ(সংরচনা sse_ipi_inject_data));

	ঘূৰাই 0;
}

শূন্য sbi_sse_exit(সংরচনা sbi_scratch *scratch)
{
	সংরচনা sbi_sse_event *e;
	সংরচনা sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		যদি (sse_event_get(info->event_id, &e))
			অনুৱর্ত;

		যদি (e->attrs.hartid != current_hartid())
			গচ্ছ skip;

		যদি (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

skip:
		sse_event_put(e);
	}
}
