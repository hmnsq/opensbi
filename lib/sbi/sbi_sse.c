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

#وصف_ڪريو sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), فراغ *, shs_ptr_off)

#وصف_ڪريو sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#وصف_ڪريو sse_سيٽ_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), فراغ *, shs_ptr_off, (__sse_state))

#وصف_ڪريو EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_ٻٽو_انگ)

#وصف_ڪريو sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		ثابت بنية sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		إذا (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

بنية sse_داخلا_state {
	/** entry pc */
	بنانشان ڊگهو pc;
	/** a6 register state */
	بنانشان ڊگهو arg;
};

بنية sse_interrupted_state {
	/** sepc register state */
	بنانشان ڊگهو sepc;
	/** flags register state */
	بنانشان ڊگهو flags;
	/** a6 register state */
	بنانشان ڊگهو a6;
	/** a7 register state */
	بنانشان ڊگهو a7;
};

بنية sse_ipi_inject_معلومات {
	uint32_t event_id;
};

بنية sbi_sse_event_attrs {
	بنانشان ڊگهو حالت;
	بنانشان ڊگهو prio;
	بنانشان ڊگهو config;
	بنانشان ڊگهو hartid;
	بنية sse_داخلا_state داخلا_2;
	بنية sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#وصف_ڪريو assert_ميدان_وٿي(ميدان, attr_وٿي)                  \
	_Static_assert(                                          \
		((offsetof(بنية sbi_sse_event_attrs, ميدان)) / \
		 حجم(بنانشان ڊگهو)) == attr_وٿي,          \
		"field " #field                                  \
		" from struct sbi_sse_event_attrs invalid offset, expected " #attr_offset)

assert_ميدان_وٿي(حالت, SBI_SSE_ATTR_حالت);
assert_ميدان_وٿي(prio, SBI_SSE_ATTR_PRIO);
assert_ميدان_وٿي(config, SBI_SSE_ATTR_CONFIG);
assert_ميدان_وٿي(hartid, SBI_SSE_ATTR_PREFERRED_HART);
assert_ميدان_وٿي(داخلا_2.pc, SBI_SSE_ATTR_داخلا_PC);
assert_ميدان_وٿي(داخلا_2.arg, SBI_SSE_ATTR_داخلا_ARG);
assert_ميدان_وٿي(interrupted.sepc, SBI_SSE_ATTR_INTERRUPTED_SEPC);
assert_ميدان_وٿي(interrupted.flags, SBI_SSE_ATTR_INTERRUPTED_FLAGS);
assert_ميدان_وٿي(interrupted.a6, SBI_SSE_ATTR_INTERRUPTED_A6);
assert_ميدان_وٿي(interrupted.a7, SBI_SSE_ATTR_INTERRUPTED_A7);

بنية sbi_sse_event {
	بنية sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	بنية sse_event_info *info;
	بنية sbi_dlist ڳنڍ;
};

/** Per-hart state */
بنية sse_hart_state {
	/**
	 * Lock that protects enabled_event_list
	 */
	spinlock_t enabled_event_تالو;

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
	بنية sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_تالو);

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
	spinlock_t تالو;

	/**
	 * global event struct
	 */
	بنية sbi_sse_event event GUARDED_BY(&تالو);
};

بنية sse_event_info {
	uint32_t event_id;
	ثابت بنية sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_ڳنڍ(sse_event_info);
};

ساكن بنانشان صحيح local_event_ڳڻپ;
ساكن بنانشان صحيح global_event_ڳڻپ;
ساكن بنية sse_global_event *global_events;

ساكن بنانشان ڊگهو sse_inject_fifo_off;
ساكن بنانشان ڊگهو sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
ساكن بنانشان ڊگهو shs_ptr_off;

ساكن u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

ساكن صحيح sse_ipi_inject_send(بنانشان ڊگهو hartid, uint32_t event_id);

بنية sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_ڳنڍ_INIT(عدم),
};

بنية sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_ڳنڍ_INIT(&global_software_event),
};

ساكن SBI_SLIST_سر(supported_events, sse_event_info) =
				SBI_SLIST_سر_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
ساكن ثابت uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_مٿاهون_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_مٿاهون_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_حد_کان_مٿي,
	SBI_SSE_EVENT_LOCAL_هيٺيون_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_هيٺيون_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

ساكن bool sse_is_standard_event(uint32_t event_id)
{
	صحيح i;

	لكل (i = 0; i < array_سائيز(standard_events); i++) {
		إذا (event_id == standard_events[i])
			أرجع true;
	}

	أرجع false;
}

ساكن بنية sse_event_info *sse_event_info_get(uint32_t event_id)
{
	بنية sse_event_info *info;

	SBI_SLIST_FOR_EACH_داخلا(info, supported_events) {
		إذا (info->event_id == event_id)
			أرجع info;
	}

	أرجع عدم;
}

ساكن بنانشان ڊگهو sse_event_state(بنية sbi_sse_event *e)
{
	أرجع e->attrs.حالت & SBI_SSE_ATTR_حالت_STATE_نقاب;
}

ساكن بنانشان ڊگهو sse_event_pending(بنية sbi_sse_event *e)
{
	أرجع !!(e->attrs.حالت & ٻٽو_انگ(SBI_SSE_ATTR_حالت_PENDING_وٿي));
}

ساكن bool sse_event_is_global(بنية sbi_sse_event *e)
{
	أرجع EVENT_IS_GLOBAL(e->event_id);
}

ساكن bool sse_event_is_local(بنية sbi_sse_event *e)
{
	أرجع !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
ساكن بنية sse_hart_state *sse_get_hart_state(بنية sbi_sse_event *e)
{
	بنية sbi_scratch *s = sbi_hartindex_منزل_scratch(e->hartindex);

	أرجع sse_get_hart_state_ptr(s);
}

ساكن بنية sse_global_event *sse_get_global_event(بنية sbi_sse_event *e)
{
	أرجع container_جو(e, بنية sse_global_event, event);
}

ساكن فراغ sse_event_سيٽ_state(بنية sbi_sse_event *e,
				بنانشان ڊگهو new_state)
{
	e->attrs.حالت &= ~SBI_SSE_ATTR_حالت_STATE_نقاب;
	e->attrs.حالت |= new_state;
}

ساكن صحيح sse_event_get(uint32_t event_id,
			 بنية sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	بنانشان صحيح i;
	بنية sbi_sse_event *e;
	بنية sse_hart_state *shs;

	إذا (!eret)
		أرجع SBI_EINVAL;

	إذا (EVENT_IS_GLOBAL(event_id)) {
		لكل (i = 0; i < global_event_ڳڻپ; i++) {
			e = &global_events[i].event;
			إذا (e->event_id == event_id) {
				spin_تالو(&global_events[i].تالو);
				*eret = e;
				أرجع SBI_SUCCESS;
			}
		}
	} وإلا {
		shs = sse_thishart_state_ptr();
		لكل (i = 0; i < local_event_ڳڻپ; i++) {
			e = &shs->local_events[i];
			إذا (e->event_id == event_id) {
				*eret = e;
				أرجع SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	إذا (sse_is_standard_event(event_id))
		أرجع SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	أرجع SBI_EINVAL;
}

ساكن فراغ sse_event_put(بنية sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	بنية sse_global_event *ge;

	إذا (sse_event_is_local(e))
		أرجع;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->تالو);
}

ساكن فراغ sse_event_remove_from_list(بنية sbi_sse_event *e)
{
	sbi_list_del(&e->ڳنڍ);
}

/**
 * Must be called under owner hart lock
 */
ساكن فراغ sse_event_جوڙيو_منزل_list(بنية sse_hart_state *state,
				   بنية sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_تالو)
{
	بنية sbi_sse_event *tmp;

	sbi_list_for_each_داخلا(tmp, &state->enabled_event_list, ڳنڍ) {
		إذا (e->attrs.prio < tmp->attrs.prio)
			اكسر;
		إذا (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			اكسر;
	}
	sbi_list_جوڙيو_پڇ(&e->ڳنڍ, &tmp->ڳنڍ);
}

/**
 * Must be called under owner hart lock
 */
ساكن صحيح sse_event_disable(بنية sse_hart_state *shs,
			      بنية sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_تالو)
{
	إذا (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		أرجع SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_سيٽ_state(e, SBI_SSE_STATE_REGISTERED);

	أرجع SBI_OK;
}

ساكن صحيح sse_event_سيٽ_hart_id_check(بنية sbi_sse_event *e,
				       بنانشان ڊگهو new_hartid)
{
	صحيح hstate;
	بنانشان صحيح hartid = (uint32_t)new_hartid;
	بنية sbi_domain *hd = sbi_domain_thishart_ptr();

	إذا (!sse_event_is_global(e))
		أرجع SBI_EDENIED;

	إذا (!sbi_domain_is_assigned_hart(hd, sbi_hartid_منزل_hartindex(hartid)))
		أرجع SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	إذا (hstate != SBI_HSM_STATE_STARTED)
		أرجع SBI_EINVAL;

	أرجع SBI_OK;
}

ساكن صحيح sse_event_سيٽ_attr_check(بنية sbi_sse_event *e, uint32_t attr_id,
				    بنانشان ڊگهو val)
{
	بدل (attr_id) {
	حالة SBI_SSE_ATTR_CONFIG:
		إذا (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			أرجع SBI_EINVALID_STATE;

		إذا (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			أرجع SBI_EINVAL;

		أرجع SBI_OK;
	حالة SBI_SSE_ATTR_PRIO:
		إذا (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			أرجع SBI_EINVALID_STATE;

#إذا __riscv_xlen > 32
		إذا (val != (uint32_t)val)
			أرجع SBI_EINVAL;
#جيڪڏهن_پورو
		أرجع SBI_OK;
	حالة SBI_SSE_ATTR_PREFERRED_HART:
		إذا (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			أرجع SBI_EINVALID_STATE;

		أرجع sse_event_سيٽ_hart_id_check(e, val);
	حالة SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		إذا (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			أرجع SBI_EINVAL;
		__attribute__((__fallthrough__));
	حالة SBI_SSE_ATTR_INTERRUPTED_SEPC:
	حالة SBI_SSE_ATTR_INTERRUPTED_A6:
	حالة SBI_SSE_ATTR_INTERRUPTED_A7:
		إذا (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			أرجع SBI_EINVALID_STATE;

		إذا (current_hartid() != e->attrs.hartid)
			أرجع SBI_EINVAL;

		أرجع SBI_OK;
	افتراضي:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		أرجع SBI_EDENIED;
	}
}

ساكن فراغ sse_event_سيٽ_attr(بنية sbi_sse_event *e, uint32_t attr_id,
			       بنانشان ڊگهو val)
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
		e->hartindex = sbi_hartid_منزل_hartindex(val);
		sse_event_invoke_cb(e, سيٽ_hartid_cb, val);
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

ساكن صحيح sse_event_رجسٽر(بنية sbi_sse_event *e,
			      بنانشان ڊگهو handler_داخلا_pc,
			      بنانشان ڊگهو handler_داخلا_arg)
{
	إذا (sse_event_state(e) != SBI_SSE_STATE_اڻ_استعمال_ٿيل)
		أرجع SBI_EINVALID_STATE;

	e->attrs.داخلا_2.pc = handler_داخلا_pc;
	e->attrs.داخلا_2.arg = handler_داخلا_arg;

	sse_event_سيٽ_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, رجسٽر_cb);

	أرجع 0;
}

ساكن صحيح sse_event_unregister(بنية sbi_sse_event *e)
{
	إذا (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		أرجع SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_سيٽ_state(e, SBI_SSE_STATE_اڻ_استعمال_ٿيل);

	أرجع 0;
}

ساكن بنانشان ڊگهو sse_interrupted_flags(بنانشان ڊگهو mstatus)
{
	بنانشان ڊگهو hstatus, flags = 0;

	إذا (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	إذا (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	إذا (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	إذا (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	إذا (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		إذا (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		إذا (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	أرجع flags;
}

ساكن فراغ sse_event_inject(بنية sbi_sse_event *e,
			     بنية sbi_trap_regs *regs)
{
	بنية sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_سيٽ_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.حالت &= ~ٻٽو_انگ(SBI_SSE_ATTR_حالت_PENDING_وٿي);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	إذا (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	إذا (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	إذا (misa_extension('H')) {
		بنانشان ڊگهو hstatus = csr_read(CSR_HSTATUS);
		إذا (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		وإلا
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		إذا (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}
	csr_write(CSR_SEPC, regs->mepc);

	/* Setup entry context */
	regs->a6 = current_hartid();
	regs->a7 = e->attrs.داخلا_2.arg;
	regs->mepc = e->attrs.داخلا_2.pc;

	/*
	 * Return to S-mode with virtualization disabled, not expected landing
	 * pad, supervisor trap disabled.
	 */
	regs->mstatus &= ~(MSTATUS_MPP | MSTATUS_SIE | MSTATUS_SPELP);
	regs->mstatus |= (PRV_S << MSTATUS_MPP_سرڪايو);
	regs->mstatus |= MSTATUS_SDT;

#إذا __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#وإلاإذا __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#وإلا
#غلطي "Unexpected __riscv_xlen"
#جيڪڏهن_پورو

}

ساكن فراغ sse_event_resume(بنية sbi_sse_event *e,
			     بنية sbi_trap_regs *regs)
{
	بنية sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	إذا (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_سرڪايو);

	إذا (misa_extension('H')) {
		بنانشان ڊگهو hstatus = csr_read(CSR_HSTATUS);
#إذا __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		إذا (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#وإلاإذا __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		إذا (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#وإلا
#غلطي "Unexpected __riscv_xlen"
#جيڪڏهن_پورو
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		إذا (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		إذا (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	إذا (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	إذا (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	إذا (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	إذا (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	إذا (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

ساكن bool sse_event_is_ready(بنية sbi_sse_event *e)
{
	إذا (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		أرجع false;
	}

	أرجع true;
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
	إذا (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		أرجع true;
	}

	إذا (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		أرجع true;
	}

	أرجع false;
}

/* Return true if an event has been injected, false otherwise */
فراغ sbi_sse_process_pending_events(بنية sbi_trap_regs *regs)
{
	bool ret;
	بنية sbi_sse_event *e;
	بنية sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	إذا (state->masked)
		أرجع;

	spin_تالو(&state->enabled_event_تالو);

	sbi_list_for_each_داخلا(e, &state->enabled_event_list, ڳنڍ) {
		ret = sse_event_check_inject(e, regs);
		إذا (ret)
			اذهب ٻاهريون;
	}

ٻاهريون:
	spin_unlock(&state->enabled_event_تالو);
}

ساكن صحيح sse_event_سيٽ_pending(بنية sbi_sse_event *e)
{
	إذا (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		أرجع SBI_EINVALID_STATE;

	e->attrs.حالت |= ٻٽو_انگ(SBI_SSE_ATTR_حالت_PENDING_وٿي);

	أرجع SBI_OK;
}

ساكن فراغ sse_ipi_inject_process(بنية sbi_scratch *scratch)
{
	بنية sbi_sse_event *e;
	بنية sse_ipi_inject_معلومات evt;
	بنية sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_وٿي_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	طالما (!sbi_fifo_قطار_مان_ڪڍو(sse_inject_fifo_r, &evt)) {
		إذا (sse_event_get(evt.event_id, &e))
			استمر;

		sse_event_سيٽ_pending(e);
		sse_event_put(e);
	}
}

ساكن بنية sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

ساكن صحيح sse_ipi_inject_send(بنانشان ڊگهو hartid, uint32_t event_id)
{
	صحيح ret;
	بنية sbi_scratch *remote_scratch = عدم;
	بنية sse_ipi_inject_معلومات evt = {event_id};
	بنية sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_منزل_scratch(hartid);
	إذا (!remote_scratch)
		أرجع SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_وٿي_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_قطار_اندر_وجهو(sse_inject_fifo_r, &evt, false);
	إذا (ret)
		أرجع SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, عدم);
	إذا (ret)
		أرجع SBI_EFAIL;

	أرجع SBI_OK;
}

ساكن صحيح sse_inject_event(uint32_t event_id, بنانشان ڊگهو hartid)
{
	صحيح ret;
	بنية sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	إذا (ret)
		أرجع ret;

	/* In case of global event, provided hart_id is ignored */
	إذا (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	إذا (hartid != current_hartid()) {
		sse_event_put(e);
		أرجع sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_سيٽ_pending(e);
	sse_event_put(e);
	إذا (ret)
		أرجع ret;

	أرجع SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
ساكن صحيح sse_event_enable(بنية sse_hart_state *shs,
			     بنية sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_تالو)
{
	إذا (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		أرجع SBI_EINVALID_STATE;

	sse_event_سيٽ_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_جوڙيو_منزل_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	إذا (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  عدم);

	أرجع SBI_OK;
}

ساكن صحيح sse_event_complete(بنية sse_hart_state *shs,
			      بنية sbi_sse_event *e,
			      بنية sbi_trap_regs *regs,
			      بنية sbi_ecall_return *ٻاهريون)
	MUST_HOLD(&shs->enabled_event_تالو)
{
	إذا (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		أرجع SBI_EINVALID_STATE;

	إذا (e->attrs.hartid != current_hartid())
		أرجع SBI_EINVAL;

	sse_event_سيٽ_state(e, SBI_SSE_STATE_ENABLED);
	إذا (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	ٻاهريون->skip_regs_update = true;

	أرجع SBI_OK;
}

صحيح sbi_sse_complete(بنية sbi_trap_regs *regs, بنية sbi_ecall_return *ٻاهريون)
{
	صحيح ret = SBI_OK;
	بنية sbi_sse_event *tmp;
	بنية sse_hart_state *state = sse_thishart_state_ptr();

	spin_تالو(&state->enabled_event_تالو);
	sbi_list_for_each_داخلا(tmp, &state->enabled_event_list, ڳنڍ) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		إذا (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, ٻاهريون);
			اكسر;
		}
	}
	spin_unlock(&state->enabled_event_تالو);

	أرجع ret;
}

صحيح sbi_sse_enable(uint32_t event_id)
{
	صحيح ret;
	بنية sbi_sse_event *e;
	بنية sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	إذا (ret)
		أرجع ret;

	shs = sse_get_hart_state(e);
	spin_تالو(&shs->enabled_event_تالو);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_تالو);
	sse_event_put(e);

	أرجع ret;
}

صحيح sbi_sse_disable(uint32_t event_id)
{
	صحيح ret;
	بنية sbi_sse_event *e;
	بنية sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	إذا (ret)
		أرجع ret;

	shs = sse_get_hart_state(e);
	spin_تالو(&shs->enabled_event_تالو);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_تالو);

	sse_event_put(e);

	أرجع ret;
}

صحيح sbi_sse_hart_نقاب(فراغ)
{
	بنية sse_hart_state *state = sse_thishart_state_ptr();

	إذا (!state)
		أرجع SBI_EFAIL;

	إذا (state->masked)
		أرجع SBI_EALREADY_STOPPED;

	state->masked = true;

	أرجع SBI_SUCCESS;
}

صحيح sbi_sse_hart_unmask(فراغ)
{
	بنية sse_hart_state *state = sse_thishart_state_ptr();

	إذا (!state)
		أرجع SBI_EFAIL;

	إذا (!state->masked)
		أرجع SBI_EALREADY_STARTED;

	state->masked = false;

	أرجع SBI_SUCCESS;
}

صحيح sbi_sse_inject_from_ecall(uint32_t event_id, بنانشان ڊگهو hartid,
			      بنية sbi_ecall_return *ٻاهريون)
{
	إذا (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_منزل_hartindex(hartid)))
		أرجع SBI_EINVAL;

	أرجع sse_inject_event(event_id, hartid);
}

صحيح sbi_sse_inject_event(uint32_t event_id)
{
	أرجع sse_inject_event(event_id, current_hartid());
}

صحيح sbi_sse_جوڙيو_event(uint32_t event_id, ثابت بنية sbi_sse_cb_ops *cb_ops)
{
	بنية sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	إذا (info)
		أرجع SBI_EALREADY;

	إذا (cb_ops && cb_ops->سيٽ_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		أرجع SBI_EINVAL;

	info = sbi_zalloc(حجم(*info));
	إذا (!info)
		أرجع SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_جوڙيو(info, supported_events);

	أرجع SBI_OK;
}

صحيح sbi_sse_attr_check(uint32_t بنياد_attr_id, uint32_t attr_ڳڻپ,
		       بنانشان ڊگهو phys_lo, بنانشان ڊگهو phys_hi,
		       بنانشان ڊگهو access)
{
	ثابت بنانشان align = __riscv_xlen >> 3;
	uint64_t پڇاڙي_id = (uint64_t)بنياد_attr_id + (attr_ڳڻپ - 1);

	إذا (attr_ڳڻپ == 0)
		أرجع SBI_ERR_INVALID_PARAM;

	إذا (پڇاڙي_id >= SBI_SSE_ATTR_MAX)
		أرجع SBI_EBAD_RANGE;

	إذا (phys_lo & (align - 1))
		أرجع SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	إذا (phys_hi)
		أرجع SBI_EINVALID_ADDR;

	إذا (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 حجم(بنانشان ڊگهو) * attr_ڳڻپ,
					 PRV_S, access))
		أرجع SBI_EINVALID_ADDR;

	أرجع SBI_OK;
}

ساكن فراغ copy_attrs(بنانشان ڊگهو *ٻاهريون, ثابت بنانشان ڊگهو *اندريون,
		       بنانشان صحيح long_ڳڻپ)
{
	صحيح i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	لكل (i = 0; i < long_ڳڻپ; i++)
		ٻاهريون[i] = اندريون[i];
}

صحيح sbi_sse_read_attrs(uint32_t event_id, uint32_t بنياد_attr_id,
		       uint32_t attr_ڳڻپ, بنانشان ڊگهو output_phys_lo,
		       بنانشان ڊگهو output_phys_hi)
{
	صحيح ret;
	بنانشان ڊگهو *e_attrs;
	بنية sbi_sse_event *e;
	بنانشان ڊگهو *attrs;

	ret = sbi_sse_attr_check(بنياد_attr_id, attr_ڳڻپ, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	إذا (ret)
		أرجع ret;

	ret = sse_event_get(event_id, &e);
	إذا (ret)
		أرجع ret;

	sbi_hart_protection_عارضي_map_range(output_phys_lo, حجم(بنانشان ڊگهو) * attr_ڳڻپ);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (بنانشان ڊگهو *)&e->attrs;
	attrs = (بنانشان ڊگهو *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[بنياد_attr_id], attr_ڳڻپ);

	sbi_hart_protection_عارضي_unmap_range(output_phys_lo, حجم(بنانشان ڊگهو) * attr_ڳڻپ);

	sse_event_put(e);

	أرجع SBI_OK;
}

ساكن صحيح sse_write_attrs(بنية sbi_sse_event *e, uint32_t بنياد_attr_id,
			   uint32_t attr_ڳڻپ, بنانشان ڊگهو داخلا_phys)
{
	صحيح ret = 0;
	بنانشان ڊگهو attr = 0, val;
	uint32_t id, پڇاڙي_id = بنياد_attr_id + attr_ڳڻپ;
	بنانشان ڊگهو *attrs = (بنانشان ڊگهو *)داخلا_phys;

	sbi_hart_protection_عارضي_map_range(داخلا_phys, حجم(بنانشان ڊگهو) * attr_ڳڻپ);

	لكل (id = بنياد_attr_id; id < پڇاڙي_id; id++) {
		val = attrs[attr++];
		ret = sse_event_سيٽ_attr_check(e, id, val);
		إذا (ret)
			اذهب ٻاهريون;
	}

	attr = 0;
	لكل (id = بنياد_attr_id; id < پڇاڙي_id; id++) {
		val = attrs[attr++];
		sse_event_سيٽ_attr(e, id, val);
	}

ٻاهريون:
	sbi_hart_protection_عارضي_unmap_range(داخلا_phys, حجم(بنانشان ڊگهو) * attr_ڳڻپ);

	أرجع ret;
}

صحيح sbi_sse_write_attrs(uint32_t event_id, uint32_t بنياد_attr_id,
			uint32_t attr_ڳڻپ, بنانشان ڊگهو داخلا_phys_lo,
			بنانشان ڊگهو داخلا_phys_hi)
{
	صحيح ret = 0;
	بنية sbi_sse_event *e;

	ret = sbi_sse_attr_check(بنياد_attr_id, attr_ڳڻپ, داخلا_phys_lo,
				 داخلا_phys_hi, SBI_DOMAIN_READ);
	إذا (ret)
		أرجع ret;

	ret = sse_event_get(event_id, &e);
	إذا (ret)
		أرجع ret;

	ret = sse_write_attrs(e, بنياد_attr_id, attr_ڳڻپ, داخلا_phys_lo);
	sse_event_put(e);

	أرجع ret;
}

صحيح sbi_sse_رجسٽر(uint32_t event_id, بنانشان ڊگهو handler_داخلا_pc,
		     بنانشان ڊگهو handler_داخلا_arg)
{
	صحيح ret;
	بنية sbi_sse_event *e;

	إذا (handler_داخلا_pc & 0x1)
		أرجع SBI_EINVAL;

	إذا (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_داخلا_pc,
					 حجم(بنانشان ڊگهو), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		أرجع SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	إذا (ret)
		أرجع ret;

	ret = sse_event_رجسٽر(e, handler_داخلا_pc, handler_داخلا_arg);
	sse_event_put(e);

	أرجع ret;
}

صحيح sbi_sse_unregister(uint32_t event_id)
{
	صحيح ret;
	بنية sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	إذا (ret)
		أرجع ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	أرجع ret;
}

ساكن فراغ sse_event_init(بنية sbi_sse_event *e, بنية sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.حالت |= ٻٽو_انگ(SBI_SSE_ATTR_حالت_INJECT_وٿي);
}

ساكن فراغ sse_event_ڳڻپ_init()
{
	بنية sse_event_info *info;

	SBI_SLIST_FOR_EACH_داخلا(info, supported_events) {
		إذا (EVENT_IS_GLOBAL(info->event_id))
			global_event_ڳڻپ++;
		وإلا
			local_event_ڳڻپ++;
	}
}

ساكن صحيح sse_global_init()
{
	بنية sbi_sse_event *e;
	بنانشان صحيح ev = 0;
	بنية sse_event_info *info;

	global_events = sbi_zalloc(حجم(*global_events) * global_event_ڳڻپ);
	إذا (!global_events)
		أرجع SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_داخلا(info, supported_events) {
		إذا (!EVENT_IS_GLOBAL(info->event_id))
			استمر;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_تالو_INIT(global_events[ev].تالو);

		ev++;
	}

	أرجع 0;
}

ساكن فراغ sse_local_init(بنية sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	بنانشان صحيح ev = 0;
	بنية sse_event_info *info;

	SBI_INIT_LIST_سر(&shs->enabled_event_list);
	SPIN_تالو_INIT(shs->enabled_event_تالو);

	SBI_SLIST_FOR_EACH_داخلا(info, supported_events) {
		إذا (EVENT_IS_GLOBAL(info->event_id))
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

	إذا (cold_boot) {
		sse_event_ڳڻپ_init();

		ret = sse_global_init();
		إذا (ret)
			أرجع ret;

		shs_ptr_off = sbi_scratch_alloc_وٿي(حجم(فراغ *));
		إذا (!shs_ptr_off)
			أرجع SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_وٿي(حجم(*sse_inject_q));
		إذا (!sse_inject_fifo_off) {
			sbi_scratch_free_وٿي(shs_ptr_off);
			أرجع SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_وٿي(
			(global_event_ڳڻپ + local_event_ڳڻپ) *
			حجم(بنية sse_ipi_inject_معلومات));
		إذا (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_وٿي(sse_inject_fifo_off);
			sbi_scratch_free_وٿي(shs_ptr_off);
			أرجع SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		إذا (ret < 0) {
			sbi_scratch_free_وٿي(shs_ptr_off);
			أرجع ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	إذا (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(حجم(*shs) + حجم(بنية sbi_sse_event) *
							local_event_ڳڻپ);
		إذا (!shs)
			أرجع SBI_ENOMEM;

		shs->local_events = (بنية sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_سيٽ_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_وٿي_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_وٿي_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_ڳڻپ + local_event_ڳڻپ),
		      حجم(بنية sse_ipi_inject_معلومات));

	أرجع 0;
}

فراغ sbi_sse_exit(بنية sbi_scratch *scratch)
{
	بنية sbi_sse_event *e;
	بنية sse_event_info *info;

	SBI_SLIST_FOR_EACH_داخلا(info, supported_events) {
		إذا (sse_event_get(info->event_id, &e))
			استمر;

		إذا (e->attrs.hartid != current_hartid())
			اذهب skip;

		إذا (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_سيٽ_state(e, SBI_SSE_STATE_اڻ_استعمال_ٿيل);

skip:
		sse_event_put(e);
	}
}
