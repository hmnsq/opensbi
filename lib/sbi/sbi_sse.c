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

#تعریف_کرو sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), خالی *, shs_ptr_off)

#تعریف_کرو sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#تعریف_کرو sse_سیٹ_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), خالی *, shs_ptr_off, (__sse_state))

#تعریف_کرو EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_ثنائی_ہندسہ)

#تعریف_کرو sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		ثابت ساخت sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		اگر (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

ساخت sse_اندراج_state {
	/** entry pc */
	غیرعلامتی طویل pc;
	/** a6 register state */
	غیرعلامتی طویل arg;
};

ساخت sse_interrupted_state {
	/** sepc register state */
	غیرعلامتی طویل sepc;
	/** flags register state */
	غیرعلامتی طویل flags;
	/** a6 register state */
	غیرعلامتی طویل a6;
	/** a7 register state */
	غیرعلامتی طویل a7;
};

ساخت sse_ipi_inject_ڈیٹا {
	uint32_t event_id;
};

ساخت sbi_sse_event_attrs {
	غیرعلامتی طویل حالت_2;
	غیرعلامتی طویل prio;
	غیرعلامتی طویل config;
	غیرعلامتی طویل hartid;
	ساخت sse_اندراج_state اندراج_3;
	ساخت sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#تعریف_کرو assert_میدان_انحراف(میدان, attr_انحراف)                  \
	_Static_assert(                                          \
		((offsetof(ساخت sbi_sse_event_attrs, میدان)) / \
		 حجم(غیرعلامتی طویل)) == attr_انحراف,          \
		"field " #field                                  \
		" from struct sbi_sse_event_attrs invalid offset, expected " #attr_offset)

assert_میدان_انحراف(حالت_2, SBI_SSE_ATTR_حالت);
assert_میدان_انحراف(prio, SBI_SSE_ATTR_PRIO);
assert_میدان_انحراف(config, SBI_SSE_ATTR_CONFIG);
assert_میدان_انحراف(hartid, SBI_SSE_ATTR_PREFERRED_HART);
assert_میدان_انحراف(اندراج_3.pc, SBI_SSE_ATTR_اندراج_PC);
assert_میدان_انحراف(اندراج_3.arg, SBI_SSE_ATTR_اندراج_ARG);
assert_میدان_انحراف(interrupted.sepc, SBI_SSE_ATTR_INTERRUPTED_SEPC);
assert_میدان_انحراف(interrupted.flags, SBI_SSE_ATTR_INTERRUPTED_FLAGS);
assert_میدان_انحراف(interrupted.a6, SBI_SSE_ATTR_INTERRUPTED_A6);
assert_میدان_انحراف(interrupted.a7, SBI_SSE_ATTR_INTERRUPTED_A7);

ساخت sbi_sse_event {
	ساخت sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	ساخت sse_event_info *info;
	ساخت sbi_dlist گرہ;
};

/** Per-hart state */
ساخت sse_hart_state {
	/**
	 * Lock that protects enabled_event_list
	 */
	spinlock_t enabled_event_تالا;

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
	ساخت sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_تالا);

	/**
	 * List of local events allocated at boot time.
	 */
	ساخت sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
ساخت sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t تالا_2;

	/**
	 * global event struct
	 */
	ساخت sbi_sse_event event GUARDED_BY(&تالا_2);
};

ساخت sse_event_info {
	uint32_t event_id;
	ثابت ساخت sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_گرہ(sse_event_info);
};

ساکن غیرعلامتی صحیح local_event_گنتی;
ساکن غیرعلامتی صحیح global_event_گنتی;
ساکن ساخت sse_global_event *global_events;

ساکن غیرعلامتی طویل sse_inject_fifo_off;
ساکن غیرعلامتی طویل sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
ساکن غیرعلامتی طویل shs_ptr_off;

ساکن u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

ساکن صحیح sse_ipi_inject_send(غیرعلامتی طویل hartid, uint32_t event_id);

ساخت sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_گرہ_INIT(تہی),
};

ساخت sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_گرہ_INIT(&global_software_event),
};

ساکن SBI_SLIST_سر(supported_events, sse_event_info) =
				SBI_SLIST_سر_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
ساکن ثابت uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_بلند_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_بلند_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_حد_سے_تجاوز,
	SBI_SSE_EVENT_LOCAL_کم_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_کم_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

ساکن bool sse_is_standard_event(uint32_t event_id)
{
	صحیح i;

	برائے (i = 0; i < array_سائز(standard_events); i++) {
		اگر (event_id == standard_events[i])
			واپس true;
	}

	واپس false;
}

ساکن ساخت sse_event_info *sse_event_info_get(uint32_t event_id)
{
	ساخت sse_event_info *info;

	SBI_SLIST_FOR_EACH_اندراج(info, supported_events) {
		اگر (info->event_id == event_id)
			واپس info;
	}

	واپس تہی;
}

ساکن غیرعلامتی طویل sse_event_state(ساخت sbi_sse_event *e)
{
	واپس e->attrs.حالت_2 & SBI_SSE_ATTR_حالت_STATE_نقاب;
}

ساکن غیرعلامتی طویل sse_event_pending(ساخت sbi_sse_event *e)
{
	واپس !!(e->attrs.حالت_2 & ثنائی_ہندسہ(SBI_SSE_ATTR_حالت_PENDING_انحراف));
}

ساکن bool sse_event_is_global(ساخت sbi_sse_event *e)
{
	واپس EVENT_IS_GLOBAL(e->event_id);
}

ساکن bool sse_event_is_local(ساخت sbi_sse_event *e)
{
	واپس !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
ساکن ساخت sse_hart_state *sse_get_hart_state(ساخت sbi_sse_event *e)
{
	ساخت sbi_scratch *s = sbi_hartindex_مقصد_scratch(e->hartindex);

	واپس sse_get_hart_state_ptr(s);
}

ساکن ساخت sse_global_event *sse_get_global_event(ساخت sbi_sse_event *e)
{
	واپس container_کا(e, ساخت sse_global_event, event);
}

ساکن خالی sse_event_سیٹ_state(ساخت sbi_sse_event *e,
				غیرعلامتی طویل new_state)
{
	e->attrs.حالت_2 &= ~SBI_SSE_ATTR_حالت_STATE_نقاب;
	e->attrs.حالت_2 |= new_state;
}

ساکن صحیح sse_event_get(uint32_t event_id,
			 ساخت sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	غیرعلامتی صحیح i;
	ساخت sbi_sse_event *e;
	ساخت sse_hart_state *shs;

	اگر (!eret)
		واپس SBI_EINVAL;

	اگر (EVENT_IS_GLOBAL(event_id)) {
		برائے (i = 0; i < global_event_گنتی; i++) {
			e = &global_events[i].event;
			اگر (e->event_id == event_id) {
				spin_تالا(&global_events[i].تالا_2);
				*eret = e;
				واپس SBI_SUCCESS;
			}
		}
	} ورنہ {
		shs = sse_thishart_state_ptr();
		برائے (i = 0; i < local_event_گنتی; i++) {
			e = &shs->local_events[i];
			اگر (e->event_id == event_id) {
				*eret = e;
				واپس SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	اگر (sse_is_standard_event(event_id))
		واپس SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	واپس SBI_EINVAL;
}

ساکن خالی sse_event_put(ساخت sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	ساخت sse_global_event *ge;

	اگر (sse_event_is_local(e))
		واپس;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->تالا_2);
}

ساکن خالی sse_event_remove_from_list(ساخت sbi_sse_event *e)
{
	sbi_list_del(&e->گرہ);
}

/**
 * Must be called under owner hart lock
 */
ساکن خالی sse_event_جوڑو_مقصد_list(ساخت sse_hart_state *state,
				   ساخت sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_تالا)
{
	ساخت sbi_sse_event *tmp;

	sbi_list_for_each_اندراج(tmp, &state->enabled_event_list, گرہ) {
		اگر (e->attrs.prio < tmp->attrs.prio)
			توڑو;
		اگر (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			توڑو;
	}
	sbi_list_جوڑو_دم(&e->گرہ, &tmp->گرہ);
}

/**
 * Must be called under owner hart lock
 */
ساکن صحیح sse_event_disable(ساخت sse_hart_state *shs,
			      ساخت sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_تالا)
{
	اگر (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		واپس SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_سیٹ_state(e, SBI_SSE_STATE_REGISTERED);

	واپس SBI_OK;
}

ساکن صحیح sse_event_سیٹ_hart_id_check(ساخت sbi_sse_event *e,
				       غیرعلامتی طویل new_hartid)
{
	صحیح hstate;
	غیرعلامتی صحیح hartid = (uint32_t)new_hartid;
	ساخت sbi_domain *hd = sbi_domain_thishart_ptr();

	اگر (!sse_event_is_global(e))
		واپس SBI_EDENIED;

	اگر (!sbi_domain_is_assigned_hart(hd, sbi_hartid_مقصد_hartindex(hartid)))
		واپس SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	اگر (hstate != SBI_HSM_STATE_STARTED)
		واپس SBI_EINVAL;

	واپس SBI_OK;
}

ساکن صحیح sse_event_سیٹ_attr_check(ساخت sbi_sse_event *e, uint32_t attr_id,
				    غیرعلامتی طویل val)
{
	انتخاب (attr_id) {
	حالت SBI_SSE_ATTR_CONFIG:
		اگر (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			واپس SBI_EINVALID_STATE;

		اگر (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			واپس SBI_EINVAL;

		واپس SBI_OK;
	حالت SBI_SSE_ATTR_PRIO:
		اگر (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			واپس SBI_EINVALID_STATE;

#اگر __riscv_xlen > 32
		اگر (val != (uint32_t)val)
			واپس SBI_EINVAL;
#اگر_ختم
		واپس SBI_OK;
	حالت SBI_SSE_ATTR_PREFERRED_HART:
		اگر (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			واپس SBI_EINVALID_STATE;

		واپس sse_event_سیٹ_hart_id_check(e, val);
	حالت SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		اگر (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			واپس SBI_EINVAL;
		__attribute__((__fallthrough__));
	حالت SBI_SSE_ATTR_INTERRUPTED_SEPC:
	حالت SBI_SSE_ATTR_INTERRUPTED_A6:
	حالت SBI_SSE_ATTR_INTERRUPTED_A7:
		اگر (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			واپس SBI_EINVALID_STATE;

		اگر (current_hartid() != e->attrs.hartid)
			واپس SBI_EINVAL;

		واپس SBI_OK;
	طےشدہ:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		واپس SBI_EDENIED;
	}
}

ساکن خالی sse_event_سیٹ_attr(ساخت sbi_sse_event *e, uint32_t attr_id,
			       غیرعلامتی طویل val)
{
	انتخاب (attr_id) {
	حالت SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		توڑو;
	حالت SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		توڑو;
	حالت SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_مقصد_hartindex(val);
		sse_event_invoke_cb(e, سیٹ_hartid_cb, val);
		توڑو;

	حالت SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		توڑو;
	حالت SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		توڑو;
	حالت SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		توڑو;
	حالت SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		توڑو;
	}
}

ساکن صحیح sse_event_رجسٹر(ساخت sbi_sse_event *e,
			      غیرعلامتی طویل handler_اندراج_pc,
			      غیرعلامتی طویل handler_اندراج_arg)
{
	اگر (sse_event_state(e) != SBI_SSE_STATE_غیر_مستعمل)
		واپس SBI_EINVALID_STATE;

	e->attrs.اندراج_3.pc = handler_اندراج_pc;
	e->attrs.اندراج_3.arg = handler_اندراج_arg;

	sse_event_سیٹ_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, رجسٹر_cb);

	واپس 0;
}

ساکن صحیح sse_event_unregister(ساخت sbi_sse_event *e)
{
	اگر (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		واپس SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_سیٹ_state(e, SBI_SSE_STATE_غیر_مستعمل);

	واپس 0;
}

ساکن غیرعلامتی طویل sse_interrupted_flags(غیرعلامتی طویل mstatus)
{
	غیرعلامتی طویل hstatus, flags = 0;

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

	واپس flags;
}

ساکن خالی sse_event_inject(ساخت sbi_sse_event *e,
			     ساخت sbi_trap_regs *regs)
{
	ساخت sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_سیٹ_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.حالت_2 &= ~ثنائی_ہندسہ(SBI_SSE_ATTR_حالت_PENDING_انحراف);

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
		غیرعلامتی طویل hstatus = csr_read(CSR_HSTATUS);
		اگر (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		ورنہ
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		اگر (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}
	csr_write(CSR_SEPC, regs->mepc);

	/* Setup entry context */
	regs->a6 = current_hartid();
	regs->a7 = e->attrs.اندراج_3.arg;
	regs->mepc = e->attrs.اندراج_3.pc;

	/*
	 * Return to S-mode with virtualization disabled, not expected landing
	 * pad, supervisor trap disabled.
	 */
	regs->mstatus &= ~(MSTATUS_MPP | MSTATUS_SIE | MSTATUS_SPELP);
	regs->mstatus |= (PRV_S << MSTATUS_MPP_سرکاؤ);
	regs->mstatus |= MSTATUS_SDT;

#اگر __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#وگرنہ_اگر __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#ورنہ
#غلطی "Unexpected __riscv_xlen"
#اگر_ختم

}

ساکن خالی sse_event_resume(ساخت sbi_sse_event *e,
			     ساخت sbi_trap_regs *regs)
{
	ساخت sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	اگر (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_سرکاؤ);

	اگر (misa_extension('H')) {
		غیرعلامتی طویل hstatus = csr_read(CSR_HSTATUS);
#اگر __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		اگر (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#وگرنہ_اگر __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		اگر (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#ورنہ
#غلطی "Unexpected __riscv_xlen"
#اگر_ختم
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

ساکن bool sse_event_is_ready(ساخت sbi_sse_event *e)
{
	اگر (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		واپس false;
	}

	واپس true;
}

ساکن bool sse_event_check_inject(ساخت sbi_sse_event *e,
				   ساخت sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	اگر (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		واپس true;
	}

	اگر (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		واپس true;
	}

	واپس false;
}

/* Return true if an event has been injected, false otherwise */
خالی sbi_sse_process_pending_events(ساخت sbi_trap_regs *regs)
{
	bool ret;
	ساخت sbi_sse_event *e;
	ساخت sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	اگر (state->masked)
		واپس;

	spin_تالا(&state->enabled_event_تالا);

	sbi_list_for_each_اندراج(e, &state->enabled_event_list, گرہ) {
		ret = sse_event_check_inject(e, regs);
		اگر (ret)
			جاؤ برآمد;
	}

برآمد:
	spin_unlock(&state->enabled_event_تالا);
}

ساکن صحیح sse_event_سیٹ_pending(ساخت sbi_sse_event *e)
{
	اگر (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		واپس SBI_EINVALID_STATE;

	e->attrs.حالت_2 |= ثنائی_ہندسہ(SBI_SSE_ATTR_حالت_PENDING_انحراف);

	واپس SBI_OK;
}

ساکن خالی sse_ipi_inject_process(ساخت sbi_scratch *scratch)
{
	ساخت sbi_sse_event *e;
	ساخت sse_ipi_inject_ڈیٹا evt;
	ساخت sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_انحراف_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	جب (!sbi_fifo_قطار_سے_نکالو(sse_inject_fifo_r, &evt)) {
		اگر (sse_event_get(evt.event_id, &e))
			جاری;

		sse_event_سیٹ_pending(e);
		sse_event_put(e);
	}
}

ساکن ساخت sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

ساکن صحیح sse_ipi_inject_send(غیرعلامتی طویل hartid, uint32_t event_id)
{
	صحیح ret;
	ساخت sbi_scratch *remote_scratch = تہی;
	ساخت sse_ipi_inject_ڈیٹا evt = {event_id};
	ساخت sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_مقصد_scratch(hartid);
	اگر (!remote_scratch)
		واپس SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_انحراف_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_قطار_میں_ڈالو(sse_inject_fifo_r, &evt, false);
	اگر (ret)
		واپس SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, تہی);
	اگر (ret)
		واپس SBI_EFAIL;

	واپس SBI_OK;
}

ساکن صحیح sse_inject_event(uint32_t event_id, غیرعلامتی طویل hartid)
{
	صحیح ret;
	ساخت sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		واپس ret;

	/* In case of global event, provided hart_id is ignored */
	اگر (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	اگر (hartid != current_hartid()) {
		sse_event_put(e);
		واپس sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_سیٹ_pending(e);
	sse_event_put(e);
	اگر (ret)
		واپس ret;

	واپس SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
ساکن صحیح sse_event_enable(ساخت sse_hart_state *shs,
			     ساخت sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_تالا)
{
	اگر (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		واپس SBI_EINVALID_STATE;

	sse_event_سیٹ_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_جوڑو_مقصد_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	اگر (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  تہی);

	واپس SBI_OK;
}

ساکن صحیح sse_event_complete(ساخت sse_hart_state *shs,
			      ساخت sbi_sse_event *e,
			      ساخت sbi_trap_regs *regs,
			      ساخت sbi_ecall_return *برآمد)
	MUST_HOLD(&shs->enabled_event_تالا)
{
	اگر (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		واپس SBI_EINVALID_STATE;

	اگر (e->attrs.hartid != current_hartid())
		واپس SBI_EINVAL;

	sse_event_سیٹ_state(e, SBI_SSE_STATE_ENABLED);
	اگر (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	برآمد->skip_regs_update = true;

	واپس SBI_OK;
}

صحیح sbi_sse_complete(ساخت sbi_trap_regs *regs, ساخت sbi_ecall_return *برآمد)
{
	صحیح ret = SBI_OK;
	ساخت sbi_sse_event *tmp;
	ساخت sse_hart_state *state = sse_thishart_state_ptr();

	spin_تالا(&state->enabled_event_تالا);
	sbi_list_for_each_اندراج(tmp, &state->enabled_event_list, گرہ) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		اگر (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, برآمد);
			توڑو;
		}
	}
	spin_unlock(&state->enabled_event_تالا);

	واپس ret;
}

صحیح sbi_sse_enable(uint32_t event_id)
{
	صحیح ret;
	ساخت sbi_sse_event *e;
	ساخت sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		واپس ret;

	shs = sse_get_hart_state(e);
	spin_تالا(&shs->enabled_event_تالا);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_تالا);
	sse_event_put(e);

	واپس ret;
}

صحیح sbi_sse_disable(uint32_t event_id)
{
	صحیح ret;
	ساخت sbi_sse_event *e;
	ساخت sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		واپس ret;

	shs = sse_get_hart_state(e);
	spin_تالا(&shs->enabled_event_تالا);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_تالا);

	sse_event_put(e);

	واپس ret;
}

صحیح sbi_sse_hart_نقاب(خالی)
{
	ساخت sse_hart_state *state = sse_thishart_state_ptr();

	اگر (!state)
		واپس SBI_EFAIL;

	اگر (state->masked)
		واپس SBI_EALREADY_STOPPED;

	state->masked = true;

	واپس SBI_SUCCESS;
}

صحیح sbi_sse_hart_unmask(خالی)
{
	ساخت sse_hart_state *state = sse_thishart_state_ptr();

	اگر (!state)
		واپس SBI_EFAIL;

	اگر (!state->masked)
		واپس SBI_EALREADY_STARTED;

	state->masked = false;

	واپس SBI_SUCCESS;
}

صحیح sbi_sse_inject_from_ecall(uint32_t event_id, غیرعلامتی طویل hartid,
			      ساخت sbi_ecall_return *برآمد)
{
	اگر (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_مقصد_hartindex(hartid)))
		واپس SBI_EINVAL;

	واپس sse_inject_event(event_id, hartid);
}

صحیح sbi_sse_inject_event(uint32_t event_id)
{
	واپس sse_inject_event(event_id, current_hartid());
}

صحیح sbi_sse_جوڑو_event(uint32_t event_id, ثابت ساخت sbi_sse_cb_ops *cb_ops)
{
	ساخت sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	اگر (info)
		واپس SBI_EALREADY;

	اگر (cb_ops && cb_ops->سیٹ_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		واپس SBI_EINVAL;

	info = sbi_zalloc(حجم(*info));
	اگر (!info)
		واپس SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_جوڑو(info, supported_events);

	واپس SBI_OK;
}

صحیح sbi_sse_attr_check(uint32_t بنیاد_attr_id, uint32_t attr_گنتی,
		       غیرعلامتی طویل phys_lo, غیرعلامتی طویل phys_hi,
		       غیرعلامتی طویل access)
{
	ثابت غیرعلامتی align = __riscv_xlen >> 3;
	uint64_t انتہا_id = (uint64_t)بنیاد_attr_id + (attr_گنتی - 1);

	اگر (attr_گنتی == 0)
		واپس SBI_ERR_INVALID_PARAM;

	اگر (انتہا_id >= SBI_SSE_ATTR_MAX)
		واپس SBI_EBAD_RANGE;

	اگر (phys_lo & (align - 1))
		واپس SBI_EINVALID_ADDR;

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
		واپس SBI_EINVALID_ADDR;

	اگر (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 حجم(غیرعلامتی طویل) * attr_گنتی,
					 PRV_S, access))
		واپس SBI_EINVALID_ADDR;

	واپس SBI_OK;
}

ساکن خالی copy_attrs(غیرعلامتی طویل *برآمد, ثابت غیرعلامتی طویل *اندر,
		       غیرعلامتی صحیح long_گنتی)
{
	صحیح i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	برائے (i = 0; i < long_گنتی; i++)
		برآمد[i] = اندر[i];
}

صحیح sbi_sse_read_attrs(uint32_t event_id, uint32_t بنیاد_attr_id,
		       uint32_t attr_گنتی, غیرعلامتی طویل output_phys_lo,
		       غیرعلامتی طویل output_phys_hi)
{
	صحیح ret;
	غیرعلامتی طویل *e_attrs;
	ساخت sbi_sse_event *e;
	غیرعلامتی طویل *attrs;

	ret = sbi_sse_attr_check(بنیاد_attr_id, attr_گنتی, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	اگر (ret)
		واپس ret;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		واپس ret;

	sbi_hart_protection_عارضی_map_range(output_phys_lo, حجم(غیرعلامتی طویل) * attr_گنتی);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (غیرعلامتی طویل *)&e->attrs;
	attrs = (غیرعلامتی طویل *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[بنیاد_attr_id], attr_گنتی);

	sbi_hart_protection_عارضی_unmap_range(output_phys_lo, حجم(غیرعلامتی طویل) * attr_گنتی);

	sse_event_put(e);

	واپس SBI_OK;
}

ساکن صحیح sse_write_attrs(ساخت sbi_sse_event *e, uint32_t بنیاد_attr_id,
			   uint32_t attr_گنتی, غیرعلامتی طویل داخلہ_phys)
{
	صحیح ret = 0;
	غیرعلامتی طویل attr = 0, val;
	uint32_t id, انتہا_id = بنیاد_attr_id + attr_گنتی;
	غیرعلامتی طویل *attrs = (غیرعلامتی طویل *)داخلہ_phys;

	sbi_hart_protection_عارضی_map_range(داخلہ_phys, حجم(غیرعلامتی طویل) * attr_گنتی);

	برائے (id = بنیاد_attr_id; id < انتہا_id; id++) {
		val = attrs[attr++];
		ret = sse_event_سیٹ_attr_check(e, id, val);
		اگر (ret)
			جاؤ برآمد;
	}

	attr = 0;
	برائے (id = بنیاد_attr_id; id < انتہا_id; id++) {
		val = attrs[attr++];
		sse_event_سیٹ_attr(e, id, val);
	}

برآمد:
	sbi_hart_protection_عارضی_unmap_range(داخلہ_phys, حجم(غیرعلامتی طویل) * attr_گنتی);

	واپس ret;
}

صحیح sbi_sse_write_attrs(uint32_t event_id, uint32_t بنیاد_attr_id,
			uint32_t attr_گنتی, غیرعلامتی طویل داخلہ_phys_lo,
			غیرعلامتی طویل داخلہ_phys_hi)
{
	صحیح ret = 0;
	ساخت sbi_sse_event *e;

	ret = sbi_sse_attr_check(بنیاد_attr_id, attr_گنتی, داخلہ_phys_lo,
				 داخلہ_phys_hi, SBI_DOMAIN_READ);
	اگر (ret)
		واپس ret;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		واپس ret;

	ret = sse_write_attrs(e, بنیاد_attr_id, attr_گنتی, داخلہ_phys_lo);
	sse_event_put(e);

	واپس ret;
}

صحیح sbi_sse_رجسٹر(uint32_t event_id, غیرعلامتی طویل handler_اندراج_pc,
		     غیرعلامتی طویل handler_اندراج_arg)
{
	صحیح ret;
	ساخت sbi_sse_event *e;

	اگر (handler_اندراج_pc & 0x1)
		واپس SBI_EINVAL;

	اگر (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_اندراج_pc,
					 حجم(غیرعلامتی طویل), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		واپس SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		واپس ret;

	ret = sse_event_رجسٹر(e, handler_اندراج_pc, handler_اندراج_arg);
	sse_event_put(e);

	واپس ret;
}

صحیح sbi_sse_unregister(uint32_t event_id)
{
	صحیح ret;
	ساخت sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	اگر (ret)
		واپس ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	واپس ret;
}

ساکن خالی sse_event_init(ساخت sbi_sse_event *e, ساخت sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.حالت_2 |= ثنائی_ہندسہ(SBI_SSE_ATTR_حالت_INJECT_انحراف);
}

ساکن خالی sse_event_گنتی_init()
{
	ساخت sse_event_info *info;

	SBI_SLIST_FOR_EACH_اندراج(info, supported_events) {
		اگر (EVENT_IS_GLOBAL(info->event_id))
			global_event_گنتی++;
		ورنہ
			local_event_گنتی++;
	}
}

ساکن صحیح sse_global_init()
{
	ساخت sbi_sse_event *e;
	غیرعلامتی صحیح ev = 0;
	ساخت sse_event_info *info;

	global_events = sbi_zalloc(حجم(*global_events) * global_event_گنتی);
	اگر (!global_events)
		واپس SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_اندراج(info, supported_events) {
		اگر (!EVENT_IS_GLOBAL(info->event_id))
			جاری;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_تالا_INIT(global_events[ev].تالا_2);

		ev++;
	}

	واپس 0;
}

ساکن خالی sse_local_init(ساخت sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	غیرعلامتی صحیح ev = 0;
	ساخت sse_event_info *info;

	SBI_INIT_LIST_سر(&shs->enabled_event_list);
	SPIN_تالا_INIT(shs->enabled_event_تالا);

	SBI_SLIST_FOR_EACH_اندراج(info, supported_events) {
		اگر (EVENT_IS_GLOBAL(info->event_id))
			جاری;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

صحیح sbi_sse_init(ساخت sbi_scratch *scratch, bool cold_boot)
{
	صحیح ret;
	خالی *sse_inject_mem;
	ساخت sse_hart_state *shs;
	ساخت sbi_fifo *sse_inject_q;

	اگر (cold_boot) {
		sse_event_گنتی_init();

		ret = sse_global_init();
		اگر (ret)
			واپس ret;

		shs_ptr_off = sbi_scratch_alloc_انحراف(حجم(خالی *));
		اگر (!shs_ptr_off)
			واپس SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_انحراف(حجم(*sse_inject_q));
		اگر (!sse_inject_fifo_off) {
			sbi_scratch_free_انحراف(shs_ptr_off);
			واپس SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_انحراف(
			(global_event_گنتی + local_event_گنتی) *
			حجم(ساخت sse_ipi_inject_ڈیٹا));
		اگر (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_انحراف(sse_inject_fifo_off);
			sbi_scratch_free_انحراف(shs_ptr_off);
			واپس SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		اگر (ret < 0) {
			sbi_scratch_free_انحراف(shs_ptr_off);
			واپس ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	اگر (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(حجم(*shs) + حجم(ساخت sbi_sse_event) *
							local_event_گنتی);
		اگر (!shs)
			واپس SBI_ENOMEM;

		shs->local_events = (ساخت sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_سیٹ_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_انحراف_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_انحراف_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_گنتی + local_event_گنتی),
		      حجم(ساخت sse_ipi_inject_ڈیٹا));

	واپس 0;
}

خالی sbi_sse_exit(ساخت sbi_scratch *scratch)
{
	ساخت sbi_sse_event *e;
	ساخت sse_event_info *info;

	SBI_SLIST_FOR_EACH_اندراج(info, supported_events) {
		اگر (sse_event_get(info->event_id, &e))
			جاری;

		اگر (e->attrs.hartid != current_hartid())
			جاؤ skip;

		اگر (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_سیٹ_state(e, SBI_SSE_STATE_غیر_مستعمل);

skip:
		sse_event_put(e);
	}
}
