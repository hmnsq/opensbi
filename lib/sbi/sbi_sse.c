/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Rivos Systems Inc.
 *
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_barrier.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_locks.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_fifo.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart_protection.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_list.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmu.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_sse.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_slist.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>

#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>

#define sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, shs_ptr_off)

#define sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define sse_set_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, shs_ptr_off, (__sse_state))

#define EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_BIT)

#define sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		ಧ್ರುವ ಸಂರಚನಾ sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		ಯದಿ (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

ಸಂರಚನಾ sse_entry_state {
	/** entry pc */
	unsigned long pc;
	/** a6 register state */
	unsigned long arg;
};

ಸಂರಚನಾ sse_interrupted_state {
	/** sepc register state */
	unsigned long sepc;
	/** flags register state */
	unsigned long flags;
	/** a6 register state */
	unsigned long a6;
	/** a7 register state */
	unsigned long a7;
};

ಸಂರಚನಾ sse_ipi_inject_data {
	uint32_t event_id;
};

ಸಂರಚನಾ sbi_sse_event_attrs {
	unsigned long status;
	unsigned long prio;
	unsigned long config;
	unsigned long hartid;
	ಸಂರಚನಾ sse_entry_state entry;
	ಸಂರಚನಾ sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)                  \
	_Static_assert(                                          \
		((offsetof(ಸಂರಚನಾ sbi_sse_event_attrs, field)) / \
		 ಪರಿಮಾಣ(unsigned long)) == attr_offset,          \
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

ಸಂರಚನಾ sbi_sse_event {
	ಸಂರಚನಾ sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	ಸಂರಚನಾ sse_event_info *info;
	ಸಂರಚನಾ sbi_dlist ಗಂಟು;
};

/** Per-hart state */
ಸಂರಚನಾ sse_hart_state {
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
	ಸಂರಚನಾ sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_lock);

	/**
	 * List of local events allocated at boot time.
	 */
	ಸಂರಚನಾ sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
ಸಂರಚನಾ sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t lock;

	/**
	 * global event struct
	 */
	ಸಂರಚನಾ sbi_sse_event event GUARDED_BY(&lock);
};

ಸಂರಚನಾ sse_event_info {
	uint32_t event_id;
	ಧ್ರುವ ಸಂರಚನಾ sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_ಗಂಟು(sse_event_info);
};

ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ local_event_ಎಣಿಕೆ;
ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ global_event_ಎಣಿಕೆ;
ಸ್ಥಿರ ಸಂರಚನಾ sse_global_event *global_events;

ಸ್ಥಿರ unsigned long sse_inject_fifo_off;
ಸ್ಥಿರ unsigned long sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
ಸ್ಥಿರ unsigned long shs_ptr_off;

ಸ್ಥಿರ u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_ipi_inject_send(unsigned long hartid, uint32_t event_id);

ಸಂರಚನಾ sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_ಗಂಟು_INIT(ರಿಕ್ತ),
};

ಸಂರಚನಾ sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_ಗಂಟು_INIT(&global_software_event),
};

ಸ್ಥಿರ SBI_SLIST_HEAD(supported_events, sse_event_info) =
				SBI_SLIST_HEAD_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
ಸ್ಥಿರ ಧ್ರುವ uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_ಮೇಲಿನ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_ಮೇಲಿನ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW,
	SBI_SSE_EVENT_LOCAL_ಕೆಳಗಿನ_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_ಕೆಳಗಿನ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

ಸ್ಥಿರ bool sse_is_standard_event(uint32_t event_id)
{
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿ (i = 0; i < array_size(standard_events); i++) {
		ಯದಿ (event_id == standard_events[i])
			ಪ್ರತ್ಯಾಗಮ true;
	}

	ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ ಸಂರಚನಾ sse_event_info *sse_event_info_get(uint32_t event_id)
{
	ಸಂರಚನಾ sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ಯದಿ (info->event_id == event_id)
			ಪ್ರತ್ಯಾಗಮ info;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ unsigned long sse_event_state(ಸಂರಚನಾ sbi_sse_event *e)
{
	ಪ್ರತ್ಯಾಗಮ e->attrs.status & SBI_SSE_ATTR_STATUS_STATE_MASK;
}

ಸ್ಥಿರ unsigned long sse_event_pending(ಸಂರಚನಾ sbi_sse_event *e)
{
	ಪ್ರತ್ಯಾಗಮ !!(e->attrs.status & BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET));
}

ಸ್ಥಿರ bool sse_event_is_global(ಸಂರಚನಾ sbi_sse_event *e)
{
	ಪ್ರತ್ಯಾಗಮ EVENT_IS_GLOBAL(e->event_id);
}

ಸ್ಥಿರ bool sse_event_is_local(ಸಂರಚನಾ sbi_sse_event *e)
{
	ಪ್ರತ್ಯಾಗಮ !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
ಸ್ಥಿರ ಸಂರಚನಾ sse_hart_state *sse_get_hart_state(ಸಂರಚನಾ sbi_sse_event *e)
{
	ಸಂರಚನಾ sbi_scratch *s = sbi_hartindex_to_scratch(e->hartindex);

	ಪ್ರತ್ಯಾಗಮ sse_get_hart_state_ptr(s);
}

ಸ್ಥಿರ ಸಂರಚನಾ sse_global_event *sse_get_global_event(ಸಂರಚನಾ sbi_sse_event *e)
{
	ಪ್ರತ್ಯಾಗಮ container_of(e, ಸಂರಚನಾ sse_global_event, event);
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_set_state(ಸಂರಚನಾ sbi_sse_event *e,
				unsigned long new_state)
{
	e->attrs.status &= ~SBI_SSE_ATTR_STATUS_STATE_MASK;
	e->attrs.status |= new_state;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_event_get(uint32_t event_id,
			 ಸಂರಚನಾ sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	unsigned ಪೂರ್ಣಾಙ್ಕ i;
	ಸಂರಚನಾ sbi_sse_event *e;
	ಸಂರಚನಾ sse_hart_state *shs;

	ಯದಿ (!eret)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (EVENT_IS_GLOBAL(event_id)) {
		ಪ್ರತಿ (i = 0; i < global_event_ಎಣಿಕೆ; i++) {
			e = &global_events[i].event;
			ಯದಿ (e->event_id == event_id) {
				spin_lock(&global_events[i].lock);
				*eret = e;
				ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
			}
		}
	} ಅನ್ಯಥಾ {
		shs = sse_thishart_state_ptr();
		ಪ್ರತಿ (i = 0; i < local_event_ಎಣಿಕೆ; i++) {
			e = &shs->local_events[i];
			ಯದಿ (e->event_id == event_id) {
				*eret = e;
				ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	ಯದಿ (sse_is_standard_event(event_id))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_put(ಸಂರಚನಾ sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	ಸಂರಚನಾ sse_global_event *ge;

	ಯದಿ (sse_event_is_local(e))
		ಪ್ರತ್ಯಾಗಮ;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->lock);
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_remove_from_list(ಸಂರಚನಾ sbi_sse_event *e)
{
	sbi_list_del(&e->ಗಂಟು);
}

/**
 * Must be called under owner hart lock
 */
ಸ್ಥಿರ ಶೂನ್ಯ sse_event_add_to_list(ಸಂರಚನಾ sse_hart_state *state,
				   ಸಂರಚನಾ sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_lock)
{
	ಸಂರಚನಾ sbi_sse_event *tmp;

	sbi_list_for_each_entry(tmp, &state->enabled_event_list, ಗಂಟು) {
		ಯದಿ (e->attrs.prio < tmp->attrs.prio)
			ಭಙ್ಗ;
		ಯದಿ (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			ಭಙ್ಗ;
	}
	sbi_list_add_tail(&e->ಗಂಟು, &tmp->ಗಂಟು);
}

/**
 * Must be called under owner hart lock
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_event_disable(ಸಂರಚನಾ sse_hart_state *shs,
			      ಸಂರಚನಾ sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	ಯದಿ (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_event_set_hart_id_check(ಸಂರಚನಾ sbi_sse_event *e,
				       unsigned long new_hartid)
{
	ಪೂರ್ಣಾಙ್ಕ hstate;
	unsigned ಪೂರ್ಣಾಙ್ಕ hartid = (uint32_t)new_hartid;
	ಸಂರಚನಾ sbi_domain *hd = sbi_domain_thishart_ptr();

	ಯದಿ (!sse_event_is_global(e))
		ಪ್ರತ್ಯಾಗಮ SBI_EDENIED;

	ಯದಿ (!sbi_domain_is_assigned_hart(hd, sbi_hartid_to_hartindex(hartid)))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	ಯದಿ (hstate != SBI_HSM_STATE_STARTED)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_event_set_attr_check(ಸಂರಚನಾ sbi_sse_event *e, uint32_t attr_id,
				    unsigned long val)
{
	ವಿಕಲ್ಪ (attr_id) {
	ಪಕ್ಷ SBI_SSE_ATTR_CONFIG:
		ಯದಿ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_STATE;

		ಯದಿ (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

		ಪ್ರತ್ಯಾಗಮ SBI_OK;
	ಪಕ್ಷ SBI_SSE_ATTR_PRIO:
		ಯದಿ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_STATE;

#ಯದಿ __riscv_xlen > 32
		ಯದಿ (val != (uint32_t)val)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
#endif
		ಪ್ರತ್ಯಾಗಮ SBI_OK;
	ಪಕ್ಷ SBI_SSE_ATTR_PREFERRED_HART:
		ಯದಿ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_STATE;

		ಪ್ರತ್ಯಾಗಮ sse_event_set_hart_id_check(e, val);
	ಪಕ್ಷ SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		ಯದಿ (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
		__attribute__((__fallthrough__));
	ಪಕ್ಷ SBI_SSE_ATTR_INTERRUPTED_SEPC:
	ಪಕ್ಷ SBI_SSE_ATTR_INTERRUPTED_A6:
	ಪಕ್ಷ SBI_SSE_ATTR_INTERRUPTED_A7:
		ಯದಿ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_STATE;

		ಯದಿ (current_hartid() != e->attrs.hartid)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

		ಪ್ರತ್ಯಾಗಮ SBI_OK;
	ಸ್ವಭಾವ:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		ಪ್ರತ್ಯಾಗಮ SBI_EDENIED;
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_set_attr(ಸಂರಚನಾ sbi_sse_event *e, uint32_t attr_id,
			       unsigned long val)
{
	ವಿಕಲ್ಪ (attr_id) {
	ಪಕ್ಷ SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_to_hartindex(val);
		sse_event_invoke_cb(e, set_hartid_cb, val);
		ಭಙ್ಗ;

	ಪಕ್ಷ SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		ಭಙ್ಗ;
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_event_register(ಸಂರಚನಾ sbi_sse_event *e,
			      unsigned long handler_entry_pc,
			      unsigned long handler_entry_arg)
{
	ಯದಿ (sse_event_state(e) != SBI_SSE_STATE_UNUSED)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_STATE;

	e->attrs.entry.pc = handler_entry_pc;
	e->attrs.entry.arg = handler_entry_arg;

	sse_event_set_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, register_cb);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_event_unregister(ಸಂರಚನಾ sbi_sse_event *e)
{
	ಯದಿ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ unsigned long sse_interrupted_flags(unsigned long mstatus)
{
	unsigned long hstatus, flags = 0;

	ಯದಿ (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	ಯದಿ (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	ಯದಿ (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	ಯದಿ (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	ಯದಿ (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		ಯದಿ (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		ಯದಿ (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	ಪ್ರತ್ಯಾಗಮ flags;
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_inject(ಸಂರಚನಾ sbi_sse_event *e,
			     ಸಂರಚನಾ sbi_trap_regs *regs)
{
	ಸಂರಚನಾ sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_set_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.status &= ~BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	ಯದಿ (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	ಯದಿ (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	ಯದಿ (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
		ಯದಿ (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		ಅನ್ಯಥಾ
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		ಯದಿ (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
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

#ಯದಿ __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#ನೋಚೇದ್ಯದಿ __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#ಅನ್ಯಥಾ
#error "Unexpected __riscv_xlen"
#endif

}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_resume(ಸಂರಚನಾ sbi_sse_event *e,
			     ಸಂರಚನಾ sbi_trap_regs *regs)
{
	ಸಂರಚನಾ sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	ಯದಿ (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

	ಯದಿ (misa_extension('H')) {
		unsigned long hstatus = csr_read(CSR_HSTATUS);
#ಯದಿ __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		ಯದಿ (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#ನೋಚೇದ್ಯದಿ __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		ಯದಿ (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#ಅನ್ಯಥಾ
#error "Unexpected __riscv_xlen"
#endif
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		ಯದಿ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		ಯದಿ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	ಯದಿ (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	ಯದಿ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	ಯದಿ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	ಯದಿ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	ಯದಿ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

ಸ್ಥಿರ bool sse_event_is_ready(ಸಂರಚನಾ sbi_sse_event *e)
{
	ಯದಿ (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		ಪ್ರತ್ಯಾಗಮ false;
	}

	ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ bool sse_event_check_inject(ಸಂರಚನಾ sbi_sse_event *e,
				   ಸಂರಚನಾ sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	ಯದಿ (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		ಪ್ರತ್ಯಾಗಮ true;
	}

	ಯದಿ (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		ಪ್ರತ್ಯಾಗಮ true;
	}

	ಪ್ರತ್ಯಾಗಮ false;
}

/* Return true if an event has been injected, false otherwise */
ಶೂನ್ಯ sbi_sse_process_pending_events(ಸಂರಚನಾ sbi_trap_regs *regs)
{
	bool ret;
	ಸಂರಚನಾ sbi_sse_event *e;
	ಸಂರಚನಾ sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	ಯದಿ (state->masked)
		ಪ್ರತ್ಯಾಗಮ;

	spin_lock(&state->enabled_event_lock);

	sbi_list_for_each_entry(e, &state->enabled_event_list, ಗಂಟು) {
		ret = sse_event_check_inject(e, regs);
		ಯದಿ (ret)
			ಗಚ್ಛ out;
	}

out:
	spin_unlock(&state->enabled_event_lock);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_event_set_pending(ಸಂರಚನಾ sbi_sse_event *e)
{
	ಯದಿ (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_STATE;

	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_PENDING_OFFSET);

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_ipi_inject_process(ಸಂರಚನಾ sbi_scratch *scratch)
{
	ಸಂರಚನಾ sbi_sse_event *e;
	ಸಂರಚನಾ sse_ipi_inject_data evt;
	ಸಂರಚನಾ sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	ಯಾವತ್ (!sbi_fifo_dequeue(sse_inject_fifo_r, &evt)) {
		ಯದಿ (sse_event_get(evt.event_id, &e))
			ಅನುವರ್ತ;

		sse_event_set_pending(e);
		sse_event_put(e);
	}
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_ipi_inject_send(unsigned long hartid, uint32_t event_id)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ sbi_scratch *remote_scratch = ರಿಕ್ತ;
	ಸಂರಚನಾ sse_ipi_inject_data evt = {event_id};
	ಸಂರಚನಾ sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_to_scratch(hartid);
	ಯದಿ (!remote_scratch)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_offset_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_enqueue(sse_inject_fifo_r, &evt, false);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, ರಿಕ್ತ);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_inject_event(uint32_t event_id, unsigned long hartid)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	/* In case of global event, provided hart_id is ignored */
	ಯದಿ (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	ಯದಿ (hartid != current_hartid()) {
		sse_event_put(e);
		ಪ್ರತ್ಯಾಗಮ sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_set_pending(e);
	sse_event_put(e);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_event_enable(ಸಂರಚನಾ sse_hart_state *shs,
			     ಸಂರಚನಾ sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_lock)
{
	ಯದಿ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_STATE;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_add_to_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	ಯದಿ (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  ರಿಕ್ತ);

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_event_complete(ಸಂರಚನಾ sse_hart_state *shs,
			      ಸಂರಚನಾ sbi_sse_event *e,
			      ಸಂರಚನಾ sbi_trap_regs *regs,
			      ಸಂರಚನಾ sbi_ecall_return *out)
	MUST_HOLD(&shs->enabled_event_lock)
{
	ಯದಿ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_STATE;

	ಯದಿ (e->attrs.hartid != current_hartid())
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	sse_event_set_state(e, SBI_SSE_STATE_ENABLED);
	ಯದಿ (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	out->skip_regs_update = true;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_complete(ಸಂರಚನಾ sbi_trap_regs *regs, ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಪೂರ್ಣಾಙ್ಕ ret = SBI_OK;
	ಸಂರಚನಾ sbi_sse_event *tmp;
	ಸಂರಚನಾ sse_hart_state *state = sse_thishart_state_ptr();

	spin_lock(&state->enabled_event_lock);
	sbi_list_for_each_entry(tmp, &state->enabled_event_list, ಗಂಟು) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		ಯದಿ (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, out);
			ಭಙ್ಗ;
		}
	}
	spin_unlock(&state->enabled_event_lock);

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_enable(uint32_t event_id)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ sbi_sse_event *e;
	ಸಂರಚನಾ sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_lock);
	sse_event_put(e);

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_disable(uint32_t event_id)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ sbi_sse_event *e;
	ಸಂರಚನಾ sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	shs = sse_get_hart_state(e);
	spin_lock(&shs->enabled_event_lock);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_lock);

	sse_event_put(e);

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_hart_mask(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sse_hart_state *state = sse_thishart_state_ptr();

	ಯದಿ (!state)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	ಯದಿ (state->masked)
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY_STOPPED;

	state->masked = true;

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_hart_unmask(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sse_hart_state *state = sse_thishart_state_ptr();

	ಯದಿ (!state)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	ಯದಿ (!state->masked)
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY_STARTED;

	state->masked = false;

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_inject_from_ecall(uint32_t event_id, unsigned long hartid,
			      ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಯದಿ (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_to_hartindex(hartid)))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಪ್ರತ್ಯಾಗಮ sse_inject_event(event_id, hartid);
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_inject_event(uint32_t event_id)
{
	ಪ್ರತ್ಯಾಗಮ sse_inject_event(event_id, current_hartid());
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_add_event(uint32_t event_id, ಧ್ರುವ ಸಂರಚನಾ sbi_sse_cb_ops *cb_ops)
{
	ಸಂರಚನಾ sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	ಯದಿ (info)
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;

	ಯದಿ (cb_ops && cb_ops->set_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	info = sbi_zalloc(ಪರಿಮಾಣ(*info));
	ಯದಿ (!info)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ADD(info, supported_events);

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_attr_check(uint32_t base_attr_id, uint32_t attr_ಎಣಿಕೆ,
		       unsigned long phys_lo, unsigned long phys_hi,
		       unsigned long access)
{
	ಧ್ರುವ unsigned align = __riscv_xlen >> 3;
	uint64_t end_id = (uint64_t)base_attr_id + (attr_ಎಣಿಕೆ - 1);

	ಯದಿ (attr_ಎಣಿಕೆ == 0)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;

	ಯದಿ (end_id >= SBI_SSE_ATTR_MAX)
		ಪ್ರತ್ಯಾಗಮ SBI_EBAD_RANGE;

	ಯದಿ (phys_lo & (align - 1))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	ಯದಿ (phys_hi)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_ADDR;

	ಯದಿ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 ಪರಿಮಾಣ(unsigned long) * attr_ಎಣಿಕೆ,
					 PRV_S, access))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_ADDR;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ copy_attrs(unsigned long *out, ಧ್ರುವ unsigned long *in,
		       unsigned ಪೂರ್ಣಾಙ್ಕ long_ಎಣಿಕೆ)
{
	ಪೂರ್ಣಾಙ್ಕ i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	ಪ್ರತಿ (i = 0; i < long_ಎಣಿಕೆ; i++)
		out[i] = in[i];
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_read_attrs(uint32_t event_id, uint32_t base_attr_id,
		       uint32_t attr_ಎಣಿಕೆ, unsigned long output_phys_lo,
		       unsigned long output_phys_hi)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	unsigned long *e_attrs;
	ಸಂರಚನಾ sbi_sse_event *e;
	unsigned long *attrs;

	ret = sbi_sse_attr_check(base_attr_id, attr_ಎಣಿಕೆ, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ret = sse_event_get(event_id, &e);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	sbi_hart_protection_temp_map_range(output_phys_lo, ಪರಿಮಾಣ(unsigned long) * attr_ಎಣಿಕೆ);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (unsigned long *)&e->attrs;
	attrs = (unsigned long *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[base_attr_id], attr_ಎಣಿಕೆ);

	sbi_hart_protection_temp_unmap_range(output_phys_lo, ಪರಿಮಾಣ(unsigned long) * attr_ಎಣಿಕೆ);

	sse_event_put(e);

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_write_attrs(ಸಂರಚನಾ sbi_sse_event *e, uint32_t base_attr_id,
			   uint32_t attr_ಎಣಿಕೆ, unsigned long ಒಳಹರಿವು_phys)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;
	unsigned long attr = 0, val;
	uint32_t id, end_id = base_attr_id + attr_ಎಣಿಕೆ;
	unsigned long *attrs = (unsigned long *)ಒಳಹರಿವು_phys;

	sbi_hart_protection_temp_map_range(ಒಳಹರಿವು_phys, ಪರಿಮಾಣ(unsigned long) * attr_ಎಣಿಕೆ);

	ಪ್ರತಿ (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		ret = sse_event_set_attr_check(e, id, val);
		ಯದಿ (ret)
			ಗಚ್ಛ out;
	}

	attr = 0;
	ಪ್ರತಿ (id = base_attr_id; id < end_id; id++) {
		val = attrs[attr++];
		sse_event_set_attr(e, id, val);
	}

out:
	sbi_hart_protection_temp_unmap_range(ಒಳಹರಿವು_phys, ಪರಿಮಾಣ(unsigned long) * attr_ಎಣಿಕೆ);

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_write_attrs(uint32_t event_id, uint32_t base_attr_id,
			uint32_t attr_ಎಣಿಕೆ, unsigned long ಒಳಹರಿವು_phys_lo,
			unsigned long ಒಳಹರಿವು_phys_hi)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;
	ಸಂರಚನಾ sbi_sse_event *e;

	ret = sbi_sse_attr_check(base_attr_id, attr_ಎಣಿಕೆ, ಒಳಹರಿವು_phys_lo,
				 ಒಳಹರಿವು_phys_hi, SBI_DOMAIN_READ);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ret = sse_event_get(event_id, &e);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ret = sse_write_attrs(e, base_attr_id, attr_ಎಣಿಕೆ, ಒಳಹರಿವು_phys_lo);
	sse_event_put(e);

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_register(uint32_t event_id, unsigned long handler_entry_pc,
		     unsigned long handler_entry_arg)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ sbi_sse_event *e;

	ಯದಿ (handler_entry_pc & 0x1)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_entry_pc,
					 ಪರಿಮಾಣ(unsigned long), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ret = sse_event_register(e, handler_entry_pc, handler_entry_arg);
	sse_event_put(e);

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_unregister(uint32_t event_id)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_init(ಸಂರಚನಾ sbi_sse_event *e, ಸಂರಚನಾ sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.status |= BIT(SBI_SSE_ATTR_STATUS_INJECT_OFFSET);
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_ಎಣಿಕೆ_init()
{
	ಸಂರಚನಾ sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ಯದಿ (EVENT_IS_GLOBAL(info->event_id))
			global_event_ಎಣಿಕೆ++;
		ಅನ್ಯಥಾ
			local_event_ಎಣಿಕೆ++;
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sse_global_init()
{
	ಸಂರಚನಾ sbi_sse_event *e;
	unsigned ಪೂರ್ಣಾಙ್ಕ ev = 0;
	ಸಂರಚನಾ sse_event_info *info;

	global_events = sbi_zalloc(ಪರಿಮಾಣ(*global_events) * global_event_ಎಣಿಕೆ);
	ಯದಿ (!global_events)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ಯದಿ (!EVENT_IS_GLOBAL(info->event_id))
			ಅನುವರ್ತ;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_LOCK_INIT(global_events[ev].lock);

		ev++;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_local_init(ಸಂರಚನಾ sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	unsigned ಪೂರ್ಣಾಙ್ಕ ev = 0;
	ಸಂರಚನಾ sse_event_info *info;

	SBI_INIT_LIST_HEAD(&shs->enabled_event_list);
	SPIN_LOCK_INIT(shs->enabled_event_lock);

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ಯದಿ (EVENT_IS_GLOBAL(info->event_id))
			ಅನುವರ್ತ;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

ಪೂರ್ಣಾಙ್ಕ sbi_sse_init(ಸಂರಚನಾ sbi_scratch *scratch, bool cold_boot)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಶೂನ್ಯ *sse_inject_mem;
	ಸಂರಚನಾ sse_hart_state *shs;
	ಸಂರಚನಾ sbi_fifo *sse_inject_q;

	ಯದಿ (cold_boot) {
		sse_event_ಎಣಿಕೆ_init();

		ret = sse_global_init();
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;

		shs_ptr_off = sbi_scratch_alloc_offset(ಪರಿಮಾಣ(ಶೂನ್ಯ *));
		ಯದಿ (!shs_ptr_off)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_offset(ಪರಿಮಾಣ(*sse_inject_q));
		ಯದಿ (!sse_inject_fifo_off) {
			sbi_scratch_free_offset(shs_ptr_off);
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_offset(
			(global_event_ಎಣಿಕೆ + local_event_ಎಣಿಕೆ) *
			ಪರಿಮಾಣ(ಸಂರಚನಾ sse_ipi_inject_data));
		ಯದಿ (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_offset(sse_inject_fifo_off);
			sbi_scratch_free_offset(shs_ptr_off);
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		ಯದಿ (ret < 0) {
			sbi_scratch_free_offset(shs_ptr_off);
			ಪ್ರತ್ಯಾಗಮ ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	ಯದಿ (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(ಪರಿಮಾಣ(*shs) + ಪರಿಮಾಣ(ಸಂರಚನಾ sbi_sse_event) *
							local_event_ಎಣಿಕೆ);
		ಯದಿ (!shs)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

		shs->local_events = (ಸಂರಚನಾ sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_set_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_offset_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_offset_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_ಎಣಿಕೆ + local_event_ಎಣಿಕೆ),
		      ಪರಿಮಾಣ(ಸಂರಚನಾ sse_ipi_inject_data));

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ sbi_sse_exit(ಸಂರಚನಾ sbi_scratch *scratch)
{
	ಸಂರಚನಾ sbi_sse_event *e;
	ಸಂರಚನಾ sse_event_info *info;

	SBI_SLIST_FOR_EACH_ENTRY(info, supported_events) {
		ಯದಿ (sse_event_get(info->event_id, &e))
			ಅನುವರ್ತ;

		ಯದಿ (e->attrs.hartid != current_hartid())
			ಗಚ್ಛ skip;

		ಯದಿ (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_set_state(e, SBI_SSE_STATE_UNUSED);

skip:
		sse_event_put(e);
	}
}
