/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Rivos Systems Inc.
 *
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_barrier.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/riscv_locks.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_fifo.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_hart_protection.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <sbi/sbi_ipi.h>
#అన్తర్భావ <sbi/sbi_list.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_pmu.h>
#అన్తర్భావ <sbi/sbi_sse.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_slist.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_trap.h>

#అన్తర్భావ <sbi/sbi_console.h>

#నిర్వచించు sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), శూన్య *, shs_ptr_off)

#నిర్వచించు sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#నిర్వచించు sse_సమితి_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), శూన్య *, shs_ptr_off, (__sse_state))

#నిర్వచించు EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_బిట్టు)

#నిర్వచించు sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		స్థిరరాశి నిర్మాణం sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		ఒకవేళ (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

నిర్మాణం sse_నమోదు_state {
	/** entry pc */
	గుర్తులేని దీర్ఘ pc;
	/** a6 register state */
	గుర్తులేని దీర్ఘ arg;
};

నిర్మాణం sse_interrupted_state {
	/** sepc register state */
	గుర్తులేని దీర్ఘ sepc;
	/** flags register state */
	గుర్తులేని దీర్ఘ flags;
	/** a6 register state */
	గుర్తులేని దీర్ఘ a6;
	/** a7 register state */
	గుర్తులేని దీర్ఘ a7;
};

నిర్మాణం sse_ipi_inject_దత్తాంశం {
	uint32_t event_id;
};

నిర్మాణం sbi_sse_event_attrs {
	గుర్తులేని దీర్ఘ స్థితి_2;
	గుర్తులేని దీర్ఘ prio;
	గుర్తులేని దీర్ఘ config;
	గుర్తులేని దీర్ఘ hartid;
	నిర్మాణం sse_నమోదు_state నమోదు_3;
	నిర్మాణం sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#నిర్వచించు assert_క్షేత్రం_స్థానభ్రంశం(క్షేత్రం, attr_స్థానభ్రంశం)                  \
	_Static_assert(                                          \
		((offsetof(నిర్మాణం sbi_sse_event_attrs, క్షేత్రం)) / \
		 పరిమాణం(గుర్తులేని దీర్ఘ)) == attr_స్థానభ్రంశం,          \
		"field " #field                                  \
		" from struct sbi_sse_event_attrs invalid offset, expected " #attr_offset)

assert_క్షేత్రం_స్థానభ్రంశం(స్థితి_2, SBI_SSE_ATTR_స్థితి);
assert_క్షేత్రం_స్థానభ్రంశం(prio, SBI_SSE_ATTR_PRIO);
assert_క్షేత్రం_స్థానభ్రంశం(config, SBI_SSE_ATTR_CONFIG);
assert_క్షేత్రం_స్థానభ్రంశం(hartid, SBI_SSE_ATTR_PREFERRED_HART);
assert_క్షేత్రం_స్థానభ్రంశం(నమోదు_3.pc, SBI_SSE_ATTR_నమోదు_PC);
assert_క్షేత్రం_స్థానభ్రంశం(నమోదు_3.arg, SBI_SSE_ATTR_నమోదు_ARG);
assert_క్షేత్రం_స్థానభ్రంశం(interrupted.sepc, SBI_SSE_ATTR_INTERRUPTED_SEPC);
assert_క్షేత్రం_స్థానభ్రంశం(interrupted.flags, SBI_SSE_ATTR_INTERRUPTED_FLAGS);
assert_క్షేత్రం_స్థానభ్రంశం(interrupted.a6, SBI_SSE_ATTR_INTERRUPTED_A6);
assert_క్షేత్రం_స్థానభ్రంశం(interrupted.a7, SBI_SSE_ATTR_INTERRUPTED_A7);

నిర్మాణం sbi_sse_event {
	నిర్మాణం sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	నిర్మాణం sse_event_info *info;
	నిర్మాణం sbi_dlist కణుపు;
};

/** Per-hart state */
నిర్మాణం sse_hart_state {
	/**
	 * Lock that protects enabled_event_list
	 */
	spinlock_t enabled_event_తాళం;

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
	నిర్మాణం sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_తాళం);

	/**
	 * List of local events allocated at boot time.
	 */
	నిర్మాణం sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
నిర్మాణం sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t తాళం_2;

	/**
	 * global event struct
	 */
	నిర్మాణం sbi_sse_event event GUARDED_BY(&తాళం_2);
};

నిర్మాణం sse_event_info {
	uint32_t event_id;
	స్థిరరాశి నిర్మాణం sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_కణుపు(sse_event_info);
};

స్థిర గుర్తులేని పూర్ణాంకం local_event_లెక్క;
స్థిర గుర్తులేని పూర్ణాంకం global_event_లెక్క;
స్థిర నిర్మాణం sse_global_event *global_events;

స్థిర గుర్తులేని దీర్ఘ sse_inject_fifo_off;
స్థిర గుర్తులేని దీర్ఘ sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
స్థిర గుర్తులేని దీర్ఘ shs_ptr_off;

స్థిర u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

స్థిర పూర్ణాంకం sse_ipi_inject_send(గుర్తులేని దీర్ఘ hartid, uint32_t event_id);

నిర్మాణం sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_కణుపు_INIT(ఖాళీ),
};

నిర్మాణం sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_కణుపు_INIT(&global_software_event),
};

స్థిర SBI_SLIST_తల(supported_events, sse_event_info) =
				SBI_SLIST_తల_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
స్థిర స్థిరరాశి uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_ఎగువ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_ఎగువ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_అధికప్రవాహం,
	SBI_SSE_EVENT_LOCAL_దిగువ_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_దిగువ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

స్థిర bool sse_is_standard_event(uint32_t event_id)
{
	పూర్ణాంకం i;

	కొరకు (i = 0; i < array_కొలత(standard_events); i++) {
		ఒకవేళ (event_id == standard_events[i])
			తిరిగి true;
	}

	తిరిగి false;
}

స్థిర నిర్మాణం sse_event_info *sse_event_info_get(uint32_t event_id)
{
	నిర్మాణం sse_event_info *info;

	SBI_SLIST_FOR_EACH_నమోదు(info, supported_events) {
		ఒకవేళ (info->event_id == event_id)
			తిరిగి info;
	}

	తిరిగి ఖాళీ;
}

స్థిర గుర్తులేని దీర్ఘ sse_event_state(నిర్మాణం sbi_sse_event *e)
{
	తిరిగి e->attrs.స్థితి_2 & SBI_SSE_ATTR_స్థితి_STATE_ముసుగు;
}

స్థిర గుర్తులేని దీర్ఘ sse_event_pending(నిర్మాణం sbi_sse_event *e)
{
	తిరిగి !!(e->attrs.స్థితి_2 & బిట్టు(SBI_SSE_ATTR_స్థితి_PENDING_స్థానభ్రంశం));
}

స్థిర bool sse_event_is_global(నిర్మాణం sbi_sse_event *e)
{
	తిరిగి EVENT_IS_GLOBAL(e->event_id);
}

స్థిర bool sse_event_is_local(నిర్మాణం sbi_sse_event *e)
{
	తిరిగి !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
స్థిర నిర్మాణం sse_hart_state *sse_get_hart_state(నిర్మాణం sbi_sse_event *e)
{
	నిర్మాణం sbi_scratch *s = sbi_hartindex_గమ్యం_scratch(e->hartindex);

	తిరిగి sse_get_hart_state_ptr(s);
}

స్థిర నిర్మాణం sse_global_event *sse_get_global_event(నిర్మాణం sbi_sse_event *e)
{
	తిరిగి container_యొక్క(e, నిర్మాణం sse_global_event, event);
}

స్థిర శూన్య sse_event_సమితి_state(నిర్మాణం sbi_sse_event *e,
				గుర్తులేని దీర్ఘ new_state)
{
	e->attrs.స్థితి_2 &= ~SBI_SSE_ATTR_స్థితి_STATE_ముసుగు;
	e->attrs.స్థితి_2 |= new_state;
}

స్థిర పూర్ణాంకం sse_event_get(uint32_t event_id,
			 నిర్మాణం sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	గుర్తులేని పూర్ణాంకం i;
	నిర్మాణం sbi_sse_event *e;
	నిర్మాణం sse_hart_state *shs;

	ఒకవేళ (!eret)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (EVENT_IS_GLOBAL(event_id)) {
		కొరకు (i = 0; i < global_event_లెక్క; i++) {
			e = &global_events[i].event;
			ఒకవేళ (e->event_id == event_id) {
				spin_తాళం(&global_events[i].తాళం_2);
				*eret = e;
				తిరిగి SBI_SUCCESS;
			}
		}
	} లేకపోతే {
		shs = sse_thishart_state_ptr();
		కొరకు (i = 0; i < local_event_లెక్క; i++) {
			e = &shs->local_events[i];
			ఒకవేళ (e->event_id == event_id) {
				*eret = e;
				తిరిగి SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	ఒకవేళ (sse_is_standard_event(event_id))
		తిరిగి SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	తిరిగి SBI_EINVAL;
}

స్థిర శూన్య sse_event_put(నిర్మాణం sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	నిర్మాణం sse_global_event *ge;

	ఒకవేళ (sse_event_is_local(e))
		తిరిగి;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->తాళం_2);
}

స్థిర శూన్య sse_event_remove_from_list(నిర్మాణం sbi_sse_event *e)
{
	sbi_list_del(&e->కణుపు);
}

/**
 * Must be called under owner hart lock
 */
స్థిర శూన్య sse_event_కూడు_గమ్యం_list(నిర్మాణం sse_hart_state *state,
				   నిర్మాణం sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_తాళం)
{
	నిర్మాణం sbi_sse_event *tmp;

	sbi_list_for_each_నమోదు(tmp, &state->enabled_event_list, కణుపు) {
		ఒకవేళ (e->attrs.prio < tmp->attrs.prio)
			ఆపు;
		ఒకవేళ (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			ఆపు;
	}
	sbi_list_కూడు_తోక(&e->కణుపు, &tmp->కణుపు);
}

/**
 * Must be called under owner hart lock
 */
స్థిర పూర్ణాంకం sse_event_disable(నిర్మాణం sse_hart_state *shs,
			      నిర్మాణం sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_తాళం)
{
	ఒకవేళ (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		తిరిగి SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_సమితి_state(e, SBI_SSE_STATE_REGISTERED);

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం sse_event_సమితి_hart_id_check(నిర్మాణం sbi_sse_event *e,
				       గుర్తులేని దీర్ఘ new_hartid)
{
	పూర్ణాంకం hstate;
	గుర్తులేని పూర్ణాంకం hartid = (uint32_t)new_hartid;
	నిర్మాణం sbi_domain *hd = sbi_domain_thishart_ptr();

	ఒకవేళ (!sse_event_is_global(e))
		తిరిగి SBI_EDENIED;

	ఒకవేళ (!sbi_domain_is_assigned_hart(hd, sbi_hartid_గమ్యం_hartindex(hartid)))
		తిరిగి SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	ఒకవేళ (hstate != SBI_HSM_STATE_STARTED)
		తిరిగి SBI_EINVAL;

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం sse_event_సమితి_attr_check(నిర్మాణం sbi_sse_event *e, uint32_t attr_id,
				    గుర్తులేని దీర్ఘ val)
{
	ఎంపిక (attr_id) {
	స్థితి SBI_SSE_ATTR_CONFIG:
		ఒకవేళ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			తిరిగి SBI_EINVALID_STATE;

		ఒకవేళ (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			తిరిగి SBI_EINVAL;

		తిరిగి SBI_OK;
	స్థితి SBI_SSE_ATTR_PRIO:
		ఒకవేళ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			తిరిగి SBI_EINVALID_STATE;

#ఒకవేళ __riscv_xlen > 32
		ఒకవేళ (val != (uint32_t)val)
			తిరిగి SBI_EINVAL;
#అయితే_ముగింపు
		తిరిగి SBI_OK;
	స్థితి SBI_SSE_ATTR_PREFERRED_HART:
		ఒకవేళ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			తిరిగి SBI_EINVALID_STATE;

		తిరిగి sse_event_సమితి_hart_id_check(e, val);
	స్థితి SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		ఒకవేళ (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			తిరిగి SBI_EINVAL;
		__attribute__((__fallthrough__));
	స్థితి SBI_SSE_ATTR_INTERRUPTED_SEPC:
	స్థితి SBI_SSE_ATTR_INTERRUPTED_A6:
	స్థితి SBI_SSE_ATTR_INTERRUPTED_A7:
		ఒకవేళ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			తిరిగి SBI_EINVALID_STATE;

		ఒకవేళ (current_hartid() != e->attrs.hartid)
			తిరిగి SBI_EINVAL;

		తిరిగి SBI_OK;
	అప్రమేయ:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		తిరిగి SBI_EDENIED;
	}
}

స్థిర శూన్య sse_event_సమితి_attr(నిర్మాణం sbi_sse_event *e, uint32_t attr_id,
			       గుర్తులేని దీర్ఘ val)
{
	ఎంపిక (attr_id) {
	స్థితి SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		ఆపు;
	స్థితి SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		ఆపు;
	స్థితి SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_గమ్యం_hartindex(val);
		sse_event_invoke_cb(e, సమితి_hartid_cb, val);
		ఆపు;

	స్థితి SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		ఆపు;
	స్థితి SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		ఆపు;
	స్థితి SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		ఆపు;
	స్థితి SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		ఆపు;
	}
}

స్థిర పూర్ణాంకం sse_event_రిజిస్టరు(నిర్మాణం sbi_sse_event *e,
			      గుర్తులేని దీర్ఘ handler_నమోదు_pc,
			      గుర్తులేని దీర్ఘ handler_నమోదు_arg)
{
	ఒకవేళ (sse_event_state(e) != SBI_SSE_STATE_వాడని)
		తిరిగి SBI_EINVALID_STATE;

	e->attrs.నమోదు_3.pc = handler_నమోదు_pc;
	e->attrs.నమోదు_3.arg = handler_నమోదు_arg;

	sse_event_సమితి_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, రిజిస్టరు_cb);

	తిరిగి 0;
}

స్థిర పూర్ణాంకం sse_event_unregister(నిర్మాణం sbi_sse_event *e)
{
	ఒకవేళ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		తిరిగి SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_సమితి_state(e, SBI_SSE_STATE_వాడని);

	తిరిగి 0;
}

స్థిర గుర్తులేని దీర్ఘ sse_interrupted_flags(గుర్తులేని దీర్ఘ mstatus)
{
	గుర్తులేని దీర్ఘ hstatus, flags = 0;

	ఒకవేళ (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	ఒకవేళ (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	ఒకవేళ (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	ఒకవేళ (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	ఒకవేళ (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		ఒకవేళ (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		ఒకవేళ (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	తిరిగి flags;
}

స్థిర శూన్య sse_event_inject(నిర్మాణం sbi_sse_event *e,
			     నిర్మాణం sbi_trap_regs *regs)
{
	నిర్మాణం sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_సమితి_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.స్థితి_2 &= ~బిట్టు(SBI_SSE_ATTR_స్థితి_PENDING_స్థానభ్రంశం);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	ఒకవేళ (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	ఒకవేళ (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	ఒకవేళ (misa_extension('H')) {
		గుర్తులేని దీర్ఘ hstatus = csr_read(CSR_HSTATUS);
		ఒకవేళ (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		లేకపోతే
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		ఒకవేళ (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}
	csr_write(CSR_SEPC, regs->mepc);

	/* Setup entry context */
	regs->a6 = current_hartid();
	regs->a7 = e->attrs.నమోదు_3.arg;
	regs->mepc = e->attrs.నమోదు_3.pc;

	/*
	 * Return to S-mode with virtualization disabled, not expected landing
	 * pad, supervisor trap disabled.
	 */
	regs->mstatus &= ~(MSTATUS_MPP | MSTATUS_SIE | MSTATUS_SPELP);
	regs->mstatus |= (PRV_S << MSTATUS_MPP_జరుపు);
	regs->mstatus |= MSTATUS_SDT;

#ఒకవేళ __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#లేదావేళ __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#లేకపోతే
#దోషం "Unexpected __riscv_xlen"
#అయితే_ముగింపు

}

స్థిర శూన్య sse_event_resume(నిర్మాణం sbi_sse_event *e,
			     నిర్మాణం sbi_trap_regs *regs)
{
	నిర్మాణం sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	ఒకవేళ (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_జరుపు);

	ఒకవేళ (misa_extension('H')) {
		గుర్తులేని దీర్ఘ hstatus = csr_read(CSR_HSTATUS);
#ఒకవేళ __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		ఒకవేళ (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#లేదావేళ __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		ఒకవేళ (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#లేకపోతే
#దోషం "Unexpected __riscv_xlen"
#అయితే_ముగింపు
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		ఒకవేళ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		ఒకవేళ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	ఒకవేళ (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	ఒకవేళ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	ఒకవేళ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	ఒకవేళ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	ఒకవేళ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

స్థిర bool sse_event_is_ready(నిర్మాణం sbi_sse_event *e)
{
	ఒకవేళ (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		తిరిగి false;
	}

	తిరిగి true;
}

స్థిర bool sse_event_check_inject(నిర్మాణం sbi_sse_event *e,
				   నిర్మాణం sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	ఒకవేళ (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		తిరిగి true;
	}

	ఒకవేళ (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		తిరిగి true;
	}

	తిరిగి false;
}

/* Return true if an event has been injected, false otherwise */
శూన్య sbi_sse_process_pending_events(నిర్మాణం sbi_trap_regs *regs)
{
	bool ret;
	నిర్మాణం sbi_sse_event *e;
	నిర్మాణం sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	ఒకవేళ (state->masked)
		తిరిగి;

	spin_తాళం(&state->enabled_event_తాళం);

	sbi_list_for_each_నమోదు(e, &state->enabled_event_list, కణుపు) {
		ret = sse_event_check_inject(e, regs);
		ఒకవేళ (ret)
			వెళ్ళు బహిర్గతం;
	}

బహిర్గతం:
	spin_unlock(&state->enabled_event_తాళం);
}

స్థిర పూర్ణాంకం sse_event_సమితి_pending(నిర్మాణం sbi_sse_event *e)
{
	ఒకవేళ (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		తిరిగి SBI_EINVALID_STATE;

	e->attrs.స్థితి_2 |= బిట్టు(SBI_SSE_ATTR_స్థితి_PENDING_స్థానభ్రంశం);

	తిరిగి SBI_OK;
}

స్థిర శూన్య sse_ipi_inject_process(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_sse_event *e;
	నిర్మాణం sse_ipi_inject_దత్తాంశం evt;
	నిర్మాణం sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_స్థానభ్రంశం_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	అయితే (!sbi_fifo_వరుస_నుండి_తీయి(sse_inject_fifo_r, &evt)) {
		ఒకవేళ (sse_event_get(evt.event_id, &e))
			కొనసాగు;

		sse_event_సమితి_pending(e);
		sse_event_put(e);
	}
}

స్థిర నిర్మాణం sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

స్థిర పూర్ణాంకం sse_ipi_inject_send(గుర్తులేని దీర్ఘ hartid, uint32_t event_id)
{
	పూర్ణాంకం ret;
	నిర్మాణం sbi_scratch *remote_scratch = ఖాళీ;
	నిర్మాణం sse_ipi_inject_దత్తాంశం evt = {event_id};
	నిర్మాణం sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_గమ్యం_scratch(hartid);
	ఒకవేళ (!remote_scratch)
		తిరిగి SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_స్థానభ్రంశం_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_వరుసలో_చేర్చు(sse_inject_fifo_r, &evt, false);
	ఒకవేళ (ret)
		తిరిగి SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, ఖాళీ);
	ఒకవేళ (ret)
		తిరిగి SBI_EFAIL;

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం sse_inject_event(uint32_t event_id, గుర్తులేని దీర్ఘ hartid)
{
	పూర్ణాంకం ret;
	నిర్మాణం sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	ఒకవేళ (ret)
		తిరిగి ret;

	/* In case of global event, provided hart_id is ignored */
	ఒకవేళ (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	ఒకవేళ (hartid != current_hartid()) {
		sse_event_put(e);
		తిరిగి sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_సమితి_pending(e);
	sse_event_put(e);
	ఒకవేళ (ret)
		తిరిగి ret;

	తిరిగి SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
స్థిర పూర్ణాంకం sse_event_enable(నిర్మాణం sse_hart_state *shs,
			     నిర్మాణం sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_తాళం)
{
	ఒకవేళ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		తిరిగి SBI_EINVALID_STATE;

	sse_event_సమితి_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_కూడు_గమ్యం_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	ఒకవేళ (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  ఖాళీ);

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం sse_event_complete(నిర్మాణం sse_hart_state *shs,
			      నిర్మాణం sbi_sse_event *e,
			      నిర్మాణం sbi_trap_regs *regs,
			      నిర్మాణం sbi_ecall_return *బహిర్గతం)
	MUST_HOLD(&shs->enabled_event_తాళం)
{
	ఒకవేళ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		తిరిగి SBI_EINVALID_STATE;

	ఒకవేళ (e->attrs.hartid != current_hartid())
		తిరిగి SBI_EINVAL;

	sse_event_సమితి_state(e, SBI_SSE_STATE_ENABLED);
	ఒకవేళ (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	బహిర్గతం->skip_regs_update = true;

	తిరిగి SBI_OK;
}

పూర్ణాంకం sbi_sse_complete(నిర్మాణం sbi_trap_regs *regs, నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	పూర్ణాంకం ret = SBI_OK;
	నిర్మాణం sbi_sse_event *tmp;
	నిర్మాణం sse_hart_state *state = sse_thishart_state_ptr();

	spin_తాళం(&state->enabled_event_తాళం);
	sbi_list_for_each_నమోదు(tmp, &state->enabled_event_list, కణుపు) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		ఒకవేళ (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, బహిర్గతం);
			ఆపు;
		}
	}
	spin_unlock(&state->enabled_event_తాళం);

	తిరిగి ret;
}

పూర్ణాంకం sbi_sse_enable(uint32_t event_id)
{
	పూర్ణాంకం ret;
	నిర్మాణం sbi_sse_event *e;
	నిర్మాణం sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	ఒకవేళ (ret)
		తిరిగి ret;

	shs = sse_get_hart_state(e);
	spin_తాళం(&shs->enabled_event_తాళం);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_తాళం);
	sse_event_put(e);

	తిరిగి ret;
}

పూర్ణాంకం sbi_sse_disable(uint32_t event_id)
{
	పూర్ణాంకం ret;
	నిర్మాణం sbi_sse_event *e;
	నిర్మాణం sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	ఒకవేళ (ret)
		తిరిగి ret;

	shs = sse_get_hart_state(e);
	spin_తాళం(&shs->enabled_event_తాళం);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_తాళం);

	sse_event_put(e);

	తిరిగి ret;
}

పూర్ణాంకం sbi_sse_hart_ముసుగు(శూన్య)
{
	నిర్మాణం sse_hart_state *state = sse_thishart_state_ptr();

	ఒకవేళ (!state)
		తిరిగి SBI_EFAIL;

	ఒకవేళ (state->masked)
		తిరిగి SBI_EALREADY_STOPPED;

	state->masked = true;

	తిరిగి SBI_SUCCESS;
}

పూర్ణాంకం sbi_sse_hart_unmask(శూన్య)
{
	నిర్మాణం sse_hart_state *state = sse_thishart_state_ptr();

	ఒకవేళ (!state)
		తిరిగి SBI_EFAIL;

	ఒకవేళ (!state->masked)
		తిరిగి SBI_EALREADY_STARTED;

	state->masked = false;

	తిరిగి SBI_SUCCESS;
}

పూర్ణాంకం sbi_sse_inject_from_ecall(uint32_t event_id, గుర్తులేని దీర్ఘ hartid,
			      నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	ఒకవేళ (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_గమ్యం_hartindex(hartid)))
		తిరిగి SBI_EINVAL;

	తిరిగి sse_inject_event(event_id, hartid);
}

పూర్ణాంకం sbi_sse_inject_event(uint32_t event_id)
{
	తిరిగి sse_inject_event(event_id, current_hartid());
}

పూర్ణాంకం sbi_sse_కూడు_event(uint32_t event_id, స్థిరరాశి నిర్మాణం sbi_sse_cb_ops *cb_ops)
{
	నిర్మాణం sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	ఒకవేళ (info)
		తిరిగి SBI_EALREADY;

	ఒకవేళ (cb_ops && cb_ops->సమితి_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		తిరిగి SBI_EINVAL;

	info = sbi_zalloc(పరిమాణం(*info));
	ఒకవేళ (!info)
		తిరిగి SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_కూడు(info, supported_events);

	తిరిగి SBI_OK;
}

పూర్ణాంకం sbi_sse_attr_check(uint32_t పునాది_attr_id, uint32_t attr_లెక్క,
		       గుర్తులేని దీర్ఘ phys_lo, గుర్తులేని దీర్ఘ phys_hi,
		       గుర్తులేని దీర్ఘ access)
{
	స్థిరరాశి గుర్తులేని align = __riscv_xlen >> 3;
	uint64_t చివర_id = (uint64_t)పునాది_attr_id + (attr_లెక్క - 1);

	ఒకవేళ (attr_లెక్క == 0)
		తిరిగి SBI_ERR_INVALID_PARAM;

	ఒకవేళ (చివర_id >= SBI_SSE_ATTR_MAX)
		తిరిగి SBI_EBAD_RANGE;

	ఒకవేళ (phys_lo & (align - 1))
		తిరిగి SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	ఒకవేళ (phys_hi)
		తిరిగి SBI_EINVALID_ADDR;

	ఒకవేళ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 పరిమాణం(గుర్తులేని దీర్ఘ) * attr_లెక్క,
					 PRV_S, access))
		తిరిగి SBI_EINVALID_ADDR;

	తిరిగి SBI_OK;
}

స్థిర శూన్య copy_attrs(గుర్తులేని దీర్ఘ *బహిర్గతం, స్థిరరాశి గుర్తులేని దీర్ఘ *లోపల,
		       గుర్తులేని పూర్ణాంకం long_లెక్క)
{
	పూర్ణాంకం i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	కొరకు (i = 0; i < long_లెక్క; i++)
		బహిర్గతం[i] = లోపల[i];
}

పూర్ణాంకం sbi_sse_read_attrs(uint32_t event_id, uint32_t పునాది_attr_id,
		       uint32_t attr_లెక్క, గుర్తులేని దీర్ఘ output_phys_lo,
		       గుర్తులేని దీర్ఘ output_phys_hi)
{
	పూర్ణాంకం ret;
	గుర్తులేని దీర్ఘ *e_attrs;
	నిర్మాణం sbi_sse_event *e;
	గుర్తులేని దీర్ఘ *attrs;

	ret = sbi_sse_attr_check(పునాది_attr_id, attr_లెక్క, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	ఒకవేళ (ret)
		తిరిగి ret;

	ret = sse_event_get(event_id, &e);
	ఒకవేళ (ret)
		తిరిగి ret;

	sbi_hart_protection_తాత్కాలిక_map_range(output_phys_lo, పరిమాణం(గుర్తులేని దీర్ఘ) * attr_లెక్క);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (గుర్తులేని దీర్ఘ *)&e->attrs;
	attrs = (గుర్తులేని దీర్ఘ *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[పునాది_attr_id], attr_లెక్క);

	sbi_hart_protection_తాత్కాలిక_unmap_range(output_phys_lo, పరిమాణం(గుర్తులేని దీర్ఘ) * attr_లెక్క);

	sse_event_put(e);

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం sse_write_attrs(నిర్మాణం sbi_sse_event *e, uint32_t పునాది_attr_id,
			   uint32_t attr_లెక్క, గుర్తులేని దీర్ఘ ఆదానం_phys)
{
	పూర్ణాంకం ret = 0;
	గుర్తులేని దీర్ఘ attr = 0, val;
	uint32_t id, చివర_id = పునాది_attr_id + attr_లెక్క;
	గుర్తులేని దీర్ఘ *attrs = (గుర్తులేని దీర్ఘ *)ఆదానం_phys;

	sbi_hart_protection_తాత్కాలిక_map_range(ఆదానం_phys, పరిమాణం(గుర్తులేని దీర్ఘ) * attr_లెక్క);

	కొరకు (id = పునాది_attr_id; id < చివర_id; id++) {
		val = attrs[attr++];
		ret = sse_event_సమితి_attr_check(e, id, val);
		ఒకవేళ (ret)
			వెళ్ళు బహిర్గతం;
	}

	attr = 0;
	కొరకు (id = పునాది_attr_id; id < చివర_id; id++) {
		val = attrs[attr++];
		sse_event_సమితి_attr(e, id, val);
	}

బహిర్గతం:
	sbi_hart_protection_తాత్కాలిక_unmap_range(ఆదానం_phys, పరిమాణం(గుర్తులేని దీర్ఘ) * attr_లెక్క);

	తిరిగి ret;
}

పూర్ణాంకం sbi_sse_write_attrs(uint32_t event_id, uint32_t పునాది_attr_id,
			uint32_t attr_లెక్క, గుర్తులేని దీర్ఘ ఆదానం_phys_lo,
			గుర్తులేని దీర్ఘ ఆదానం_phys_hi)
{
	పూర్ణాంకం ret = 0;
	నిర్మాణం sbi_sse_event *e;

	ret = sbi_sse_attr_check(పునాది_attr_id, attr_లెక్క, ఆదానం_phys_lo,
				 ఆదానం_phys_hi, SBI_DOMAIN_READ);
	ఒకవేళ (ret)
		తిరిగి ret;

	ret = sse_event_get(event_id, &e);
	ఒకవేళ (ret)
		తిరిగి ret;

	ret = sse_write_attrs(e, పునాది_attr_id, attr_లెక్క, ఆదానం_phys_lo);
	sse_event_put(e);

	తిరిగి ret;
}

పూర్ణాంకం sbi_sse_రిజిస్టరు(uint32_t event_id, గుర్తులేని దీర్ఘ handler_నమోదు_pc,
		     గుర్తులేని దీర్ఘ handler_నమోదు_arg)
{
	పూర్ణాంకం ret;
	నిర్మాణం sbi_sse_event *e;

	ఒకవేళ (handler_నమోదు_pc & 0x1)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_నమోదు_pc,
					 పరిమాణం(గుర్తులేని దీర్ఘ), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		తిరిగి SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	ఒకవేళ (ret)
		తిరిగి ret;

	ret = sse_event_రిజిస్టరు(e, handler_నమోదు_pc, handler_నమోదు_arg);
	sse_event_put(e);

	తిరిగి ret;
}

పూర్ణాంకం sbi_sse_unregister(uint32_t event_id)
{
	పూర్ణాంకం ret;
	నిర్మాణం sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	ఒకవేళ (ret)
		తిరిగి ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	తిరిగి ret;
}

స్థిర శూన్య sse_event_init(నిర్మాణం sbi_sse_event *e, నిర్మాణం sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.స్థితి_2 |= బిట్టు(SBI_SSE_ATTR_స్థితి_INJECT_స్థానభ్రంశం);
}

స్థిర శూన్య sse_event_లెక్క_init()
{
	నిర్మాణం sse_event_info *info;

	SBI_SLIST_FOR_EACH_నమోదు(info, supported_events) {
		ఒకవేళ (EVENT_IS_GLOBAL(info->event_id))
			global_event_లెక్క++;
		లేకపోతే
			local_event_లెక్క++;
	}
}

స్థిర పూర్ణాంకం sse_global_init()
{
	నిర్మాణం sbi_sse_event *e;
	గుర్తులేని పూర్ణాంకం ev = 0;
	నిర్మాణం sse_event_info *info;

	global_events = sbi_zalloc(పరిమాణం(*global_events) * global_event_లెక్క);
	ఒకవేళ (!global_events)
		తిరిగి SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_నమోదు(info, supported_events) {
		ఒకవేళ (!EVENT_IS_GLOBAL(info->event_id))
			కొనసాగు;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_తాళం_INIT(global_events[ev].తాళం_2);

		ev++;
	}

	తిరిగి 0;
}

స్థిర శూన్య sse_local_init(నిర్మాణం sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	గుర్తులేని పూర్ణాంకం ev = 0;
	నిర్మాణం sse_event_info *info;

	SBI_INIT_LIST_తల(&shs->enabled_event_list);
	SPIN_తాళం_INIT(shs->enabled_event_తాళం);

	SBI_SLIST_FOR_EACH_నమోదు(info, supported_events) {
		ఒకవేళ (EVENT_IS_GLOBAL(info->event_id))
			కొనసాగు;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

పూర్ణాంకం sbi_sse_init(నిర్మాణం sbi_scratch *scratch, bool cold_boot)
{
	పూర్ణాంకం ret;
	శూన్య *sse_inject_mem;
	నిర్మాణం sse_hart_state *shs;
	నిర్మాణం sbi_fifo *sse_inject_q;

	ఒకవేళ (cold_boot) {
		sse_event_లెక్క_init();

		ret = sse_global_init();
		ఒకవేళ (ret)
			తిరిగి ret;

		shs_ptr_off = sbi_scratch_alloc_స్థానభ్రంశం(పరిమాణం(శూన్య *));
		ఒకవేళ (!shs_ptr_off)
			తిరిగి SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_స్థానభ్రంశం(పరిమాణం(*sse_inject_q));
		ఒకవేళ (!sse_inject_fifo_off) {
			sbi_scratch_free_స్థానభ్రంశం(shs_ptr_off);
			తిరిగి SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_స్థానభ్రంశం(
			(global_event_లెక్క + local_event_లెక్క) *
			పరిమాణం(నిర్మాణం sse_ipi_inject_దత్తాంశం));
		ఒకవేళ (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_స్థానభ్రంశం(sse_inject_fifo_off);
			sbi_scratch_free_స్థానభ్రంశం(shs_ptr_off);
			తిరిగి SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		ఒకవేళ (ret < 0) {
			sbi_scratch_free_స్థానభ్రంశం(shs_ptr_off);
			తిరిగి ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	ఒకవేళ (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(పరిమాణం(*shs) + పరిమాణం(నిర్మాణం sbi_sse_event) *
							local_event_లెక్క);
		ఒకవేళ (!shs)
			తిరిగి SBI_ENOMEM;

		shs->local_events = (నిర్మాణం sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_సమితి_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_స్థానభ్రంశం_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_స్థానభ్రంశం_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_లెక్క + local_event_లెక్క),
		      పరిమాణం(నిర్మాణం sse_ipi_inject_దత్తాంశం));

	తిరిగి 0;
}

శూన్య sbi_sse_exit(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_sse_event *e;
	నిర్మాణం sse_event_info *info;

	SBI_SLIST_FOR_EACH_నమోదు(info, supported_events) {
		ఒకవేళ (sse_event_get(info->event_id, &e))
			కొనసాగు;

		ఒకవేళ (e->attrs.hartid != current_hartid())
			వెళ్ళు skip;

		ఒకవేళ (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_సమితి_state(e, SBI_SSE_STATE_వాడని);

skip:
		sse_event_put(e);
	}
}
