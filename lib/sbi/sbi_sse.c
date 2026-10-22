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

#ವ್ಯಾಖ್ಯಾನಿಸು sse_get_hart_state_ptr(__scratch) \
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, shs_ptr_off)

#ವ್ಯಾಖ್ಯಾನಿಸು sse_thishart_state_ptr() \
	sse_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#ವ್ಯಾಖ್ಯಾನಿಸು sse_ಗಣ_hart_state_ptr(__scratch, __sse_state) \
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, shs_ptr_off, (__sse_state))

#ವ್ಯಾಖ್ಯಾನಿಸು EVENT_IS_GLOBAL(__event_id) ((__event_id) & SBI_SSE_EVENT_GLOBAL_ಬಿಟ್)

#ವ್ಯಾಖ್ಯಾನಿಸು sse_event_invoke_cb(_event, _cb, ...)                                 \
	{                                                                     \
		ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_sse_cb_ops *__ops = _event->info->cb_ops;    \
		ಆದರೆ (__ops && __ops->_cb)                                      \
			__ops->_cb(_event->event_id, ##__VA_ARGS__);          \
	}

ರಚನೆ sse_ನಮೂದು_state {
	/** entry pc */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ pc;
	/** a6 register state */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg;
};

ರಚನೆ sse_interrupted_state {
	/** sepc register state */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sepc;
	/** flags register state */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags;
	/** a6 register state */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a6;
	/** a7 register state */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a7;
};

ರಚನೆ sse_ipi_inject_ದತ್ತಾಂಶ {
	uint32_t event_id;
};

ರಚನೆ sbi_sse_event_attrs {
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸ್ಥಿತಿ_2;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ prio;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ config;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hartid;
	ರಚನೆ sse_ನಮೂದು_state ನಮೂದು_2;
	ರಚನೆ sse_interrupted_state interrupted;
};

/* Make sure all attributes are packed for direct memcpy in ATTR_READ */
#ವ್ಯಾಖ್ಯಾನಿಸು assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(ಕ್ಷೇತ್ರ, attr_ಸ್ಥಾನಾಂತರ)                  \
	_Static_assert(                                          \
		((offsetof(ರಚನೆ sbi_sse_event_attrs, ಕ್ಷೇತ್ರ)) / \
		 ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) == attr_ಸ್ಥಾನಾಂತರ,          \
		"field " #field                                  \
		" from struct sbi_sse_event_attrs invalid offset, expected " #attr_offset)

assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿತಿ_2, SBI_SSE_ATTR_ಸ್ಥಿತಿ);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(prio, SBI_SSE_ATTR_PRIO);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(config, SBI_SSE_ATTR_CONFIG);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(hartid, SBI_SSE_ATTR_PREFERRED_HART);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(ನಮೂದು_2.pc, SBI_SSE_ATTR_ನಮೂದು_PC);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(ನಮೂದು_2.arg, SBI_SSE_ATTR_ನಮೂದು_ARG);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(interrupted.sepc, SBI_SSE_ATTR_INTERRUPTED_SEPC);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(interrupted.flags, SBI_SSE_ATTR_INTERRUPTED_FLAGS);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(interrupted.a6, SBI_SSE_ATTR_INTERRUPTED_A6);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(interrupted.a7, SBI_SSE_ATTR_INTERRUPTED_A7);

ರಚನೆ sbi_sse_event {
	ರಚನೆ sbi_sse_event_attrs attrs;
	uint32_t event_id;
	u32 hartindex;
	ರಚನೆ sse_event_info *info;
	ರಚನೆ sbi_dlist ಗಂಟು;
};

/** Per-hart state */
ರಚನೆ sse_hart_state {
	/**
	 * Lock that protects enabled_event_list
	 */
	spinlock_t enabled_event_ಬೀಗ;

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
	ರಚನೆ sbi_dlist enabled_event_list GUARDED_BY(&enabled_event_ಬೀಗ);

	/**
	 * List of local events allocated at boot time.
	 */
	ರಚನೆ sbi_sse_event *local_events;

	/**
	 * State to track if the hart is ready to take sse events.
	 * One hart cannot modify this state of another hart.
	 */
	bool masked;
};

/**
 * Global events are accessible by all harts
 */
ರಚನೆ sse_global_event {
	/**
	 * Global event lock protecting access from multiple harts from ecall to
	 * the event.
	 */
	spinlock_t ಬೀಗ_2;

	/**
	 * global event struct
	 */
	ರಚನೆ sbi_sse_event event GUARDED_BY(&ಬೀಗ_2);
};

ರಚನೆ sse_event_info {
	uint32_t event_id;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_sse_cb_ops *cb_ops;
	SBI_SLIST_ಗಂಟು(sse_event_info);
};

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ local_event_ಎಣಿಕೆ;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ global_event_ಎಣಿಕೆ;
ಸ್ಥಿರ ರಚನೆ sse_global_event *global_events;

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sse_inject_fifo_off;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sse_inject_fifo_mem_off;
/* Offset of pointer to SSE HART state in scratch space */
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ shs_ptr_off;

ಸ್ಥಿರ u32 sse_ipi_inject_event = SBI_IPI_EVENT_MAX;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_ipi_inject_send(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hartid, uint32_t event_id);

ರಚನೆ sse_event_info global_software_event = {
	.event_id = SBI_SSE_EVENT_GLOBAL_SOFTWARE,
	SBI_SLIST_ಗಂಟು_INIT(ಖಾಲಿ),
};

ರಚನೆ sse_event_info local_software_event = {
	.event_id = SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SLIST_ಗಂಟು_INIT(&global_software_event),
};

ಸ್ಥಿರ SBI_SLIST_ತಲೆ(supported_events, sse_event_info) =
				SBI_SLIST_ತಲೆ_INIT(&local_software_event);

/*
 * This array is used to distinguish between standard event and platform
 * events in order to return SBI_ERR_NOT_SUPPORTED for them.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint32_t standard_events[] = {
	SBI_SSE_EVENT_LOCAL_ಮೇಲಿನ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_DOUBLE_TRAP,
	SBI_SSE_EVENT_GLOBAL_ಮೇಲಿನ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_PMU_ಅತಿಪ್ರವಾಹ,
	SBI_SSE_EVENT_LOCAL_ಕೆಳಗಿನ_PRIO_RAS,
	SBI_SSE_EVENT_GLOBAL_ಕೆಳಗಿನ_PRIO_RAS,
	SBI_SSE_EVENT_LOCAL_SOFTWARE,
	SBI_SSE_EVENT_GLOBAL_SOFTWARE,
};

ಸ್ಥಿರ bool sse_is_standard_event(uint32_t event_id)
{
	ಪೂರ್ಣಾಂಕ i;

	ಗಾಗಿ (i = 0; i < array_ಗಾತ್ರ(standard_events); i++) {
		ಆದರೆ (event_id == standard_events[i])
			ಹಿಂತಿರುಗಿ true;
	}

	ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ರಚನೆ sse_event_info *sse_event_info_get(uint32_t event_id)
{
	ರಚನೆ sse_event_info *info;

	SBI_SLIST_FOR_EACH_ನಮೂದು(info, supported_events) {
		ಆದರೆ (info->event_id == event_id)
			ಹಿಂತಿರುಗಿ info;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sse_event_state(ರಚನೆ sbi_sse_event *e)
{
	ಹಿಂತಿರುಗಿ e->attrs.ಸ್ಥಿತಿ_2 & SBI_SSE_ATTR_ಸ್ಥಿತಿ_STATE_ಮುಖವಾಡ;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sse_event_pending(ರಚನೆ sbi_sse_event *e)
{
	ಹಿಂತಿರುಗಿ !!(e->attrs.ಸ್ಥಿತಿ_2 & ಬಿಟ್(SBI_SSE_ATTR_ಸ್ಥಿತಿ_PENDING_ಸ್ಥಾನಾಂತರ));
}

ಸ್ಥಿರ bool sse_event_is_global(ರಚನೆ sbi_sse_event *e)
{
	ಹಿಂತಿರುಗಿ EVENT_IS_GLOBAL(e->event_id);
}

ಸ್ಥಿರ bool sse_event_is_local(ರಚನೆ sbi_sse_event *e)
{
	ಹಿಂತಿರುಗಿ !sse_event_is_global(e);
}

/**
 * If event is global, must be called under global event lock
 */
ಸ್ಥಿರ ರಚನೆ sse_hart_state *sse_get_hart_state(ರಚನೆ sbi_sse_event *e)
{
	ರಚನೆ sbi_scratch *s = sbi_hartindex_ಗಮ್ಯ_scratch(e->hartindex);

	ಹಿಂತಿರುಗಿ sse_get_hart_state_ptr(s);
}

ಸ್ಥಿರ ರಚನೆ sse_global_event *sse_get_global_event(ರಚನೆ sbi_sse_event *e)
{
	ಹಿಂತಿರುಗಿ container_ದ(e, ರಚನೆ sse_global_event, event);
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_ಗಣ_state(ರಚನೆ sbi_sse_event *e,
				ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ new_state)
{
	e->attrs.ಸ್ಥಿತಿ_2 &= ~SBI_SSE_ATTR_ಸ್ಥಿತಿ_STATE_ಮುಖವಾಡ;
	e->attrs.ಸ್ಥಿತಿ_2 |= new_state;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_event_get(uint32_t event_id,
			 ರಚನೆ sbi_sse_event **eret) NO_THREAD_SAFETY_ANALYSIS
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
	ರಚನೆ sbi_sse_event *e;
	ರಚನೆ sse_hart_state *shs;

	ಆದರೆ (!eret)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (EVENT_IS_GLOBAL(event_id)) {
		ಗಾಗಿ (i = 0; i < global_event_ಎಣಿಕೆ; i++) {
			e = &global_events[i].event;
			ಆದರೆ (e->event_id == event_id) {
				spin_ಬೀಗ(&global_events[i].ಬೀಗ_2);
				*eret = e;
				ಹಿಂತಿರುಗಿ SBI_SUCCESS;
			}
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		shs = sse_thishart_state_ptr();
		ಗಾಗಿ (i = 0; i < local_event_ಎಣಿಕೆ; i++) {
			e = &shs->local_events[i];
			ಆದರೆ (e->event_id == event_id) {
				*eret = e;
				ಹಿಂತಿರುಗಿ SBI_SUCCESS;
			}
		}
	}

	/* Check if the event is a standard one but not supported */
	ಆದರೆ (sse_is_standard_event(event_id))
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	/* If not supported nor a standard event, it is invalid */
	ಹಿಂತಿರುಗಿ SBI_EINVAL;
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_put(ರಚನೆ sbi_sse_event *e) NO_THREAD_SAFETY_ANALYSIS
{
	ರಚನೆ sse_global_event *ge;

	ಆದರೆ (sse_event_is_local(e))
		ಹಿಂತಿರುಗಿ;

	ge = sse_get_global_event(e);
	spin_unlock(&ge->ಬೀಗ_2);
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_remove_from_list(ರಚನೆ sbi_sse_event *e)
{
	sbi_list_del(&e->ಗಂಟು);
}

/**
 * Must be called under owner hart lock
 */
ಸ್ಥಿರ ಶೂನ್ಯ sse_event_ಕೂಡಿಸು_ಗಮ್ಯ_list(ರಚನೆ sse_hart_state *state,
				   ರಚನೆ sbi_sse_event *e)
	MUST_HOLD(&state->enabled_event_ಬೀಗ)
{
	ರಚನೆ sbi_sse_event *tmp;

	sbi_list_for_each_ನಮೂದು(tmp, &state->enabled_event_list, ಗಂಟು) {
		ಆದರೆ (e->attrs.prio < tmp->attrs.prio)
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (e->attrs.prio == tmp->attrs.prio &&
		    e->event_id < tmp->event_id)
			ನಿಲ್ಲಿಸು;
	}
	sbi_list_ಕೂಡಿಸು_ಬಾಲ(&e->ಗಂಟು, &tmp->ಗಂಟು);
}

/**
 * Must be called under owner hart lock
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_event_disable(ರಚನೆ sse_hart_state *shs,
			      ರಚನೆ sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_ಬೀಗ)
{
	ಆದರೆ (sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ಹಿಂತಿರುಗಿ SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, disable_cb);

	sse_event_remove_from_list(e);
	sse_event_ಗಣ_state(e, SBI_SSE_STATE_REGISTERED);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_event_ಗಣ_hart_id_check(ರಚನೆ sbi_sse_event *e,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ new_hartid)
{
	ಪೂರ್ಣಾಂಕ hstate;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hartid = (uint32_t)new_hartid;
	ರಚನೆ sbi_domain *hd = sbi_domain_thishart_ptr();

	ಆದರೆ (!sse_event_is_global(e))
		ಹಿಂತಿರುಗಿ SBI_EDENIED;

	ಆದರೆ (!sbi_domain_is_assigned_hart(hd, sbi_hartid_ಗಮ್ಯ_hartindex(hartid)))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	hstate = sbi_hsm_hart_get_state(hd, hartid);
	ಆದರೆ (hstate != SBI_HSM_STATE_STARTED)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_event_ಗಣ_attr_check(ರಚನೆ sbi_sse_event *e, uint32_t attr_id,
				    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val)
{
	ಆಯ್ಕೆ (attr_id) {
	ಸ್ಥಿತಿ SBI_SSE_ATTR_CONFIG:
		ಆದರೆ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ಹಿಂತಿರುಗಿ SBI_EINVALID_STATE;

		ಆದರೆ (val & ~SBI_SSE_ATTR_CONFIG_ONESHOT)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;

		ಹಿಂತಿರುಗಿ SBI_OK;
	ಸ್ಥಿತಿ SBI_SSE_ATTR_PRIO:
		ಆದರೆ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ಹಿಂತಿರುಗಿ SBI_EINVALID_STATE;

#ಆದರೆ __riscv_xlen > 32
		ಆದರೆ (val != (uint32_t)val)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ SBI_OK;
	ಸ್ಥಿತಿ SBI_SSE_ATTR_PREFERRED_HART:
		ಆದರೆ (sse_event_state(e) >= SBI_SSE_STATE_ENABLED)
			ಹಿಂತಿರುಗಿ SBI_EINVALID_STATE;

		ಹಿಂತಿರುಗಿ sse_event_ಗಣ_hart_id_check(e, val);
	ಸ್ಥಿತಿ SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		ಆದರೆ (val & ~(SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP |
			    SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT))
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		__attribute__((__fallthrough__));
	ಸ್ಥಿತಿ SBI_SSE_ATTR_INTERRUPTED_SEPC:
	ಸ್ಥಿತಿ SBI_SSE_ATTR_INTERRUPTED_A6:
	ಸ್ಥಿತಿ SBI_SSE_ATTR_INTERRUPTED_A7:
		ಆದರೆ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
			ಹಿಂತಿರುಗಿ SBI_EINVALID_STATE;

		ಆದರೆ (current_hartid() != e->attrs.hartid)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;

		ಹಿಂತಿರುಗಿ SBI_OK;
	ಪೂರ್ವನಿಯೋಜಿತ:
		/*
		 * Attribute range validity was already checked by
		 * sbi_sse_attr_check(). If we end up here, attribute was not
		 * handled by the above 'case' statements and thus it is
		 * read-only.
		 */
		ಹಿಂತಿರುಗಿ SBI_EDENIED;
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_ಗಣ_attr(ರಚನೆ sbi_sse_event *e, uint32_t attr_id,
			       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val)
{
	ಆಯ್ಕೆ (attr_id) {
	ಸ್ಥಿತಿ SBI_SSE_ATTR_CONFIG:
		e->attrs.config = val;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_SSE_ATTR_PRIO:
		e->attrs.prio = (uint32_t)val;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_SSE_ATTR_PREFERRED_HART:
		e->attrs.hartid = val;
		e->hartindex = sbi_hartid_ಗಮ್ಯ_hartindex(val);
		sse_event_invoke_cb(e, ಗಣ_hartid_cb, val);
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SBI_SSE_ATTR_INTERRUPTED_SEPC:
		e->attrs.interrupted.sepc = val;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_SSE_ATTR_INTERRUPTED_FLAGS:
		e->attrs.interrupted.flags = val;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_SSE_ATTR_INTERRUPTED_A6:
		e->attrs.interrupted.a6 = val;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_SSE_ATTR_INTERRUPTED_A7:
		e->attrs.interrupted.a7 = val;
		ನಿಲ್ಲಿಸು;
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_event_ರಿಜಿಸ್ಟರ್(ರಚನೆ sbi_sse_event *e,
			      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ handler_ನಮೂದು_pc,
			      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ handler_ನಮೂದು_arg)
{
	ಆದರೆ (sse_event_state(e) != SBI_SSE_STATE_ಬಳಸದ)
		ಹಿಂತಿರುಗಿ SBI_EINVALID_STATE;

	e->attrs.ನಮೂದು_2.pc = handler_ನಮೂದು_pc;
	e->attrs.ನಮೂದು_2.arg = handler_ನಮೂದು_arg;

	sse_event_ಗಣ_state(e, SBI_SSE_STATE_REGISTERED);

	sse_event_invoke_cb(e, ರಿಜಿಸ್ಟರ್_cb);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_event_unregister(ರಚನೆ sbi_sse_event *e)
{
	ಆದರೆ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ಹಿಂತಿರುಗಿ SBI_EINVALID_STATE;

	sse_event_invoke_cb(e, unregister_cb);

	sse_event_ಗಣ_state(e, SBI_SSE_STATE_ಬಳಸದ);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sse_interrupted_flags(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mstatus)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hstatus, flags = 0;

	ಆದರೆ (mstatus & MSTATUS_SPIE)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE;
	ಆದರೆ (mstatus & MSTATUS_SPP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP;
	ಆದರೆ (mstatus & MSTATUS_SPELP)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP;
	ಆದರೆ (mstatus & MSTATUS_SDT)
		flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT;

	ಆದರೆ (misa_extension('H')) {
		hstatus = csr_read(CSR_HSTATUS);
		ಆದರೆ (hstatus & HSTATUS_SPV)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV;
		ಆದರೆ (hstatus & HSTATUS_SPVP)
			flags |= SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP;
	}

	ಹಿಂತಿರುಗಿ flags;
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_inject(ರಚನೆ sbi_sse_event *e,
			     ರಚನೆ sbi_trap_regs *regs)
{
	ರಚನೆ sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	sse_event_ಗಣ_state(e, SBI_SSE_STATE_RUNNING);

	e->attrs.ಸ್ಥಿತಿ_2 &= ~ಬಿಟ್(SBI_SSE_ATTR_ಸ್ಥಿತಿ_PENDING_ಸ್ಥಾನಾಂತರ);

	i_ctx->a6 = regs->a6;
	i_ctx->a7 = regs->a7;
	i_ctx->flags = sse_interrupted_flags(regs->mstatus);
	i_ctx->sepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~(MSTATUS_SPP | SSTATUS_SPIE);
	ಆದರೆ (regs->mstatus & MSTATUS_MPP)
		regs->mstatus |= MSTATUS_SPP;
	ಆದರೆ (regs->mstatus & MSTATUS_SIE)
		regs->mstatus |= MSTATUS_SPIE;

	ಆದರೆ (misa_extension('H')) {
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hstatus = csr_read(CSR_HSTATUS);
		ಆದರೆ (sbi_regs_from_virt(regs))
			hstatus |= HSTATUS_SPV;
		ಇಲ್ಲದಿದ್ದರೆ
			hstatus &= ~HSTATUS_SPV;

		hstatus &= ~HSTATUS_SPVP;
		ಆದರೆ (hstatus & HSTATUS_SPV && regs->mstatus & SSTATUS_SPP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}
	csr_write(CSR_SEPC, regs->mepc);

	/* Setup entry context */
	regs->a6 = current_hartid();
	regs->a7 = e->attrs.ನಮೂದು_2.arg;
	regs->mepc = e->attrs.ನಮೂದು_2.pc;

	/*
	 * Return to S-mode with virtualization disabled, not expected landing
	 * pad, supervisor trap disabled.
	 */
	regs->mstatus &= ~(MSTATUS_MPP | MSTATUS_SIE | MSTATUS_SPELP);
	regs->mstatus |= (PRV_S << MSTATUS_MPP_ಸರಿಸು);
	regs->mstatus |= MSTATUS_SDT;

#ಆದರೆ __riscv_xlen == 64
	regs->mstatus &= ~MSTATUS_MPV;
#ಅಥವಾಆದರೆ __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ "Unexpected __riscv_xlen"
#ಆದರೆ_ಅಂತ್ಯ

}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_resume(ರಚನೆ sbi_sse_event *e,
			     ರಚನೆ sbi_trap_regs *regs)
{
	ರಚನೆ sse_interrupted_state *i_ctx = &e->attrs.interrupted;

	regs->mepc = csr_read(CSR_SEPC);

	regs->mstatus &= ~MSTATUS_MPP;
	ಆದರೆ (regs->mstatus & MSTATUS_SPP)
		regs->mstatus |= (PRV_S << MSTATUS_MPP_ಸರಿಸು);

	ಆದರೆ (misa_extension('H')) {
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hstatus = csr_read(CSR_HSTATUS);
#ಆದರೆ __riscv_xlen == 64
		regs->mstatus &= ~MSTATUS_MPV;
		ಆದರೆ (hstatus & HSTATUS_SPV)
			regs->mstatus |= MSTATUS_MPV;
#ಅಥವಾಆದರೆ __riscv_xlen == 32
		regs->mstatusH &= ~MSTATUSH_MPV;
		ಆದರೆ (hstatus & HSTATUS_SPV)
			regs->mstatusH |= MSTATUSH_MPV;
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ "Unexpected __riscv_xlen"
#ಆದರೆ_ಅಂತ್ಯ
		hstatus &= ~(HSTATUS_SPV | HSTATUS_SPVP);
		ಆದರೆ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV)
			hstatus |= HSTATUS_SPV;

		ಆದರೆ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP)
			hstatus |= HSTATUS_SPVP;

		csr_write(CSR_HSTATUS, hstatus);
	}

	regs->mstatus &= ~MSTATUS_SIE;
	ಆದರೆ (regs->mstatus & MSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SIE;

	regs->mstatus &= ~MSTATUS_SPIE;
	ಆದರೆ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPIE)
		regs->mstatus |= MSTATUS_SPIE;

	regs->mstatus &= ~MSTATUS_SPP;
	ಆದರೆ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPP)
		regs->mstatus |= MSTATUS_SPP;

	regs->mstatus &= ~MSTATUS_SPELP;
	ಆದರೆ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SPELP)
		regs->mstatus |= MSTATUS_SPELP;

	regs->mstatus &= ~MSTATUS_SDT;
	ಆದರೆ (i_ctx->flags & SBI_SSE_ATTR_INTERRUPTED_FLAGS_SSTATUS_SDT)
		regs->mstatus |= MSTATUS_SDT;

	regs->a7 = i_ctx->a7;
	regs->a6 = i_ctx->a6;
	csr_write(CSR_SEPC, i_ctx->sepc);
}

ಸ್ಥಿರ bool sse_event_is_ready(ರಚನೆ sbi_sse_event *e)
{
	ಆದರೆ (!sse_event_pending(e) ||
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED ||
	    e->attrs.hartid != current_hartid()) {
		ಹಿಂತಿರುಗಿ false;
	}

	ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ bool sse_event_check_inject(ರಚನೆ sbi_sse_event *e,
				   ರಚನೆ sbi_trap_regs *regs)
{
	/*
	 * List of event is ordered by priority, stop at first running
	 * event since all other events after this one are of lower
	 * priority. This means an event of higher priority is already
	 * running.
	 */
	ಆದರೆ (sse_event_state(e) == SBI_SSE_STATE_RUNNING) {
		ಹಿಂತಿರುಗಿ true;
	}

	ಆದರೆ (sse_event_is_ready(e)) {
		sse_event_inject(e, regs);
		ಹಿಂತಿರುಗಿ true;
	}

	ಹಿಂತಿರುಗಿ false;
}

/* Return true if an event has been injected, false otherwise */
ಶೂನ್ಯ sbi_sse_process_pending_events(ರಚನೆ sbi_trap_regs *regs)
{
	bool ret;
	ರಚನೆ sbi_sse_event *e;
	ರಚನೆ sse_hart_state *state = sse_thishart_state_ptr();

	/* if sse is masked on this hart, do nothing */
	ಆದರೆ (state->masked)
		ಹಿಂತಿರುಗಿ;

	spin_ಬೀಗ(&state->enabled_event_ಬೀಗ);

	sbi_list_for_each_ನಮೂದು(e, &state->enabled_event_list, ಗಂಟು) {
		ret = sse_event_check_inject(e, regs);
		ಆದರೆ (ret)
			ಹೋಗು ಹೊರಹೋಗು;
	}

ಹೊರಹೋಗು:
	spin_unlock(&state->enabled_event_ಬೀಗ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_event_ಗಣ_pending(ರಚನೆ sbi_sse_event *e)
{
	ಆದರೆ (sse_event_state(e) != SBI_SSE_STATE_RUNNING &&
	    sse_event_state(e) != SBI_SSE_STATE_ENABLED)
		ಹಿಂತಿರುಗಿ SBI_EINVALID_STATE;

	e->attrs.ಸ್ಥಿತಿ_2 |= ಬಿಟ್(SBI_SSE_ATTR_ಸ್ಥಿತಿ_PENDING_ಸ್ಥಾನಾಂತರ);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_ipi_inject_process(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_sse_event *e;
	ರಚನೆ sse_ipi_inject_ದತ್ತಾಂಶ evt;
	ರಚನೆ sbi_fifo *sse_inject_fifo_r =
		sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, sse_inject_fifo_off);

	/* Mark all queued events as pending */
	ಆಗ (!sbi_fifo_ಸರತಿಯಿಂದ_ತೆಗೆ(sse_inject_fifo_r, &evt)) {
		ಆದರೆ (sse_event_get(evt.event_id, &e))
			ಮುಂದುವರಿ;

		sse_event_ಗಣ_pending(e);
		sse_event_put(e);
	}
}

ಸ್ಥಿರ ರಚನೆ sbi_ipi_event_ops sse_ipi_inject_ops = {
	.name	 = "IPI_SSE_INJECT",
	.process = sse_ipi_inject_process,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_ipi_inject_send(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hartid, uint32_t event_id)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ sbi_scratch *remote_scratch = ಖಾಲಿ;
	ರಚನೆ sse_ipi_inject_ದತ್ತಾಂಶ evt = {event_id};
	ರಚನೆ sbi_fifo *sse_inject_fifo_r;

	remote_scratch = sbi_hartid_ಗಮ್ಯ_scratch(hartid);
	ಆದರೆ (!remote_scratch)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	sse_inject_fifo_r =
		sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(remote_scratch, sse_inject_fifo_off);

	ret = sbi_fifo_ಸರತಿಗೆ_ಸೇರಿಸು(sse_inject_fifo_r, &evt, false);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	ret = sbi_ipi_send_many(1, hartid, sse_ipi_inject_event, ಖಾಲಿ);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_inject_event(uint32_t event_id, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hartid)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	/* In case of global event, provided hart_id is ignored */
	ಆದರೆ (sse_event_is_global(e))
		hartid = e->attrs.hartid;

	/* Event is for another hart, send it through IPI */
	ಆದರೆ (hartid != current_hartid()) {
		sse_event_put(e);
		ಹಿಂತಿರುಗಿ sse_ipi_inject_send(hartid, event_id);
	}

	ret = sse_event_ಗಣ_pending(e);
	sse_event_put(e);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

/**
 * Must be called under owner hart lock
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_event_enable(ರಚನೆ sse_hart_state *shs,
			     ರಚನೆ sbi_sse_event *e)
	MUST_HOLD(&shs->enabled_event_ಬೀಗ)
{
	ಆದರೆ (sse_event_state(e) != SBI_SSE_STATE_REGISTERED)
		ಹಿಂತಿರುಗಿ SBI_EINVALID_STATE;

	sse_event_ಗಣ_state(e, SBI_SSE_STATE_ENABLED);
	sse_event_ಕೂಡಿಸು_ಗಮ್ಯ_list(shs, e);

	sse_event_invoke_cb(e, enable_cb);

	ಆದರೆ (sse_event_is_global(e) && sse_event_pending(e))
		sbi_ipi_send_many(1, e->attrs.hartid, sse_ipi_inject_event,
				  ಖಾಲಿ);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_event_complete(ರಚನೆ sse_hart_state *shs,
			      ರಚನೆ sbi_sse_event *e,
			      ರಚನೆ sbi_trap_regs *regs,
			      ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
	MUST_HOLD(&shs->enabled_event_ಬೀಗ)
{
	ಆದರೆ (sse_event_state(e) != SBI_SSE_STATE_RUNNING)
		ಹಿಂತಿರುಗಿ SBI_EINVALID_STATE;

	ಆದರೆ (e->attrs.hartid != current_hartid())
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	sse_event_ಗಣ_state(e, SBI_SSE_STATE_ENABLED);
	ಆದರೆ (e->attrs.config & SBI_SSE_ATTR_CONFIG_ONESHOT)
		sse_event_disable(shs, e);

	sse_event_invoke_cb(e, complete_cb);

	sse_event_resume(e, regs);
	ಹೊರಹೋಗು->skip_regs_update = true;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಪೂರ್ಣಾಂಕ sbi_sse_complete(ರಚನೆ sbi_trap_regs *regs, ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಪೂರ್ಣಾಂಕ ret = SBI_OK;
	ರಚನೆ sbi_sse_event *tmp;
	ರಚನೆ sse_hart_state *state = sse_thishart_state_ptr();

	spin_ಬೀಗ(&state->enabled_event_ಬೀಗ);
	sbi_list_for_each_ನಮೂದು(tmp, &state->enabled_event_list, ಗಂಟು) {
		/*
		 * List of event is ordered by priority, first one running is
		 * the one that needs to be completed
		 */
		ಆದರೆ (sse_event_state(tmp) == SBI_SSE_STATE_RUNNING) {
			ret = sse_event_complete(state, tmp, regs, ಹೊರಹೋಗು);
			ನಿಲ್ಲಿಸು;
		}
	}
	spin_unlock(&state->enabled_event_ಬೀಗ);

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ sbi_sse_enable(uint32_t event_id)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ sbi_sse_event *e;
	ರಚನೆ sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	shs = sse_get_hart_state(e);
	spin_ಬೀಗ(&shs->enabled_event_ಬೀಗ);
	ret = sse_event_enable(shs, e);
	spin_unlock(&shs->enabled_event_ಬೀಗ);
	sse_event_put(e);

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ sbi_sse_disable(uint32_t event_id)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ sbi_sse_event *e;
	ರಚನೆ sse_hart_state *shs;

	ret = sse_event_get(event_id, &e);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	shs = sse_get_hart_state(e);
	spin_ಬೀಗ(&shs->enabled_event_ಬೀಗ);
	ret = sse_event_disable(shs, e);
	spin_unlock(&shs->enabled_event_ಬೀಗ);

	sse_event_put(e);

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ sbi_sse_hart_ಮುಖವಾಡ(ಶೂನ್ಯ)
{
	ರಚನೆ sse_hart_state *state = sse_thishart_state_ptr();

	ಆದರೆ (!state)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	ಆದರೆ (state->masked)
		ಹಿಂತಿರುಗಿ SBI_EALREADY_STOPPED;

	state->masked = true;

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಪೂರ್ಣಾಂಕ sbi_sse_hart_unmask(ಶೂನ್ಯ)
{
	ರಚನೆ sse_hart_state *state = sse_thishart_state_ptr();

	ಆದರೆ (!state)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	ಆದರೆ (!state->masked)
		ಹಿಂತಿರುಗಿ SBI_EALREADY_STARTED;

	state->masked = false;

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಪೂರ್ಣಾಂಕ sbi_sse_inject_from_ecall(uint32_t event_id, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hartid,
			      ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಆದರೆ (!sbi_domain_is_assigned_hart(sbi_domain_thishart_ptr(),
					 sbi_hartid_ಗಮ್ಯ_hartindex(hartid)))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಹಿಂತಿರುಗಿ sse_inject_event(event_id, hartid);
}

ಪೂರ್ಣಾಂಕ sbi_sse_inject_event(uint32_t event_id)
{
	ಹಿಂತಿರುಗಿ sse_inject_event(event_id, current_hartid());
}

ಪೂರ್ಣಾಂಕ sbi_sse_ಕೂಡಿಸು_event(uint32_t event_id, ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_sse_cb_ops *cb_ops)
{
	ರಚನೆ sse_event_info *info;

	/* Do not allow adding an event twice */
	info = sse_event_info_get(event_id);
	ಆದರೆ (info)
		ಹಿಂತಿರುಗಿ SBI_EALREADY;

	ಆದರೆ (cb_ops && cb_ops->ಗಣ_hartid_cb && !EVENT_IS_GLOBAL(event_id))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	info = sbi_zalloc(ಗಾತ್ರ(*info));
	ಆದರೆ (!info)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	info->cb_ops = cb_ops;
	info->event_id = event_id;

	SBI_SLIST_ಕೂಡಿಸು(info, supported_events);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಪೂರ್ಣಾಂಕ sbi_sse_attr_check(uint32_t ತಳ_attr_id, uint32_t attr_ಎಣಿಕೆ,
		       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ phys_lo, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ phys_hi,
		       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ access)
{
	ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ align = __riscv_xlen >> 3;
	uint64_t ಕೊನೆ_id = (uint64_t)ತಳ_attr_id + (attr_ಎಣಿಕೆ - 1);

	ಆದರೆ (attr_ಎಣಿಕೆ == 0)
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	ಆದರೆ (ಕೊನೆ_id >= SBI_SSE_ATTR_MAX)
		ಹಿಂತಿರುಗಿ SBI_EBAD_RANGE;

	ಆದರೆ (phys_lo & (align - 1))
		ಹಿಂತಿರುಗಿ SBI_EINVALID_ADDR;

	/*
	 * On RV32, the M-mode can only access the first 4GB of
	 * the physical address space because M-mode does not have
	 * MMU to access full 34-bit physical address space.
	 *
	 * Based on above, we simply fail if the upper 32bits of
	 * the physical address (i.e. a2 register) is non-zero on
	 * RV32.
	 */
	ಆದರೆ (phys_hi)
		ಹಿಂತಿರುಗಿ SBI_EINVALID_ADDR;

	ಆದರೆ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(), phys_lo,
					 ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) * attr_ಎಣಿಕೆ,
					 PRV_S, access))
		ಹಿಂತಿರುಗಿ SBI_EINVALID_ADDR;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ copy_attrs(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಒಳ,
		       ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ long_ಎಣಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ i = 0;

	/*
	 * sbi_memcpy() does byte-per-byte copy, using this yields long-per-long
	 * copy
	 */
	ಗಾಗಿ (i = 0; i < long_ಎಣಿಕೆ; i++)
		ಹೊರಹೋಗು[i] = ಒಳ[i];
}

ಪೂರ್ಣಾಂಕ sbi_sse_read_attrs(uint32_t event_id, uint32_t ತಳ_attr_id,
		       uint32_t attr_ಎಣಿಕೆ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ output_phys_lo,
		       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ output_phys_hi)
{
	ಪೂರ್ಣಾಂಕ ret;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *e_attrs;
	ರಚನೆ sbi_sse_event *e;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *attrs;

	ret = sbi_sse_attr_check(ತಳ_attr_id, attr_ಎಣಿಕೆ, output_phys_lo,
				 output_phys_hi, SBI_DOMAIN_WRITE);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ret = sse_event_get(event_id, &e);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range(output_phys_lo, ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) * attr_ಎಣಿಕೆ);

	/*
	 * Copy all attributes at once since struct sse_event_attrs is matching
	 * the SBI_SSE_ATTR_* attributes. READ_ATTR is used in SSE handling path
	 * to retrieve the value of registers when interrupted. Rather than
	 * doing multiple SBI calls a single one is done allowing to retrieve
	 * them all at once.
	 */
	e_attrs = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *)&e->attrs;
	attrs = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *)output_phys_lo;
	copy_attrs(attrs, &e_attrs[ತಳ_attr_id], attr_ಎಣಿಕೆ);

	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range(output_phys_lo, ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) * attr_ಎಣಿಕೆ);

	sse_event_put(e);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_write_attrs(ರಚನೆ sbi_sse_event *e, uint32_t ತಳ_attr_id,
			   uint32_t attr_ಎಣಿಕೆ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಒಳಹರಿವು_phys)
{
	ಪೂರ್ಣಾಂಕ ret = 0;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ attr = 0, val;
	uint32_t id, ಕೊನೆ_id = ತಳ_attr_id + attr_ಎಣಿಕೆ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *attrs = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *)ಒಳಹರಿವು_phys;

	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range(ಒಳಹರಿವು_phys, ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) * attr_ಎಣಿಕೆ);

	ಗಾಗಿ (id = ತಳ_attr_id; id < ಕೊನೆ_id; id++) {
		val = attrs[attr++];
		ret = sse_event_ಗಣ_attr_check(e, id, val);
		ಆದರೆ (ret)
			ಹೋಗು ಹೊರಹೋಗು;
	}

	attr = 0;
	ಗಾಗಿ (id = ತಳ_attr_id; id < ಕೊನೆ_id; id++) {
		val = attrs[attr++];
		sse_event_ಗಣ_attr(e, id, val);
	}

ಹೊರಹೋಗು:
	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range(ಒಳಹರಿವು_phys, ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) * attr_ಎಣಿಕೆ);

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ sbi_sse_write_attrs(uint32_t event_id, uint32_t ತಳ_attr_id,
			uint32_t attr_ಎಣಿಕೆ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಒಳಹರಿವು_phys_lo,
			ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಒಳಹರಿವು_phys_hi)
{
	ಪೂರ್ಣಾಂಕ ret = 0;
	ರಚನೆ sbi_sse_event *e;

	ret = sbi_sse_attr_check(ತಳ_attr_id, attr_ಎಣಿಕೆ, ಒಳಹರಿವು_phys_lo,
				 ಒಳಹರಿವು_phys_hi, SBI_DOMAIN_READ);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ret = sse_event_get(event_id, &e);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ret = sse_write_attrs(e, ತಳ_attr_id, attr_ಎಣಿಕೆ, ಒಳಹರಿವು_phys_lo);
	sse_event_put(e);

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ sbi_sse_ರಿಜಿಸ್ಟರ್(uint32_t event_id, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ handler_ನಮೂದು_pc,
		     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ handler_ನಮೂದು_arg)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ sbi_sse_event *e;

	ಆದರೆ (handler_ನಮೂದು_pc & 0x1)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					 handler_ನಮೂದು_pc,
					 ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ), PRV_S,
					 SBI_DOMAIN_EXECUTE))
		ಹಿಂತಿರುಗಿ SBI_EINVALID_ADDR;

	ret = sse_event_get(event_id, &e);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ret = sse_event_ರಿಜಿಸ್ಟರ್(e, handler_ನಮೂದು_pc, handler_ನಮೂದು_arg);
	sse_event_put(e);

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ sbi_sse_unregister(uint32_t event_id)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ sbi_sse_event *e;

	ret = sse_event_get(event_id, &e);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ret = sse_event_unregister(e);
	sse_event_put(e);

	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_init(ರಚನೆ sbi_sse_event *e, ರಚನೆ sse_event_info *info)
{
	e->event_id = info->event_id;
	e->info = info;
	e->hartindex = current_hartindex();
	e->attrs.hartid = current_hartid();
	/* Declare all events as injectable */
	e->attrs.ಸ್ಥಿತಿ_2 |= ಬಿಟ್(SBI_SSE_ATTR_ಸ್ಥಿತಿ_INJECT_ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_event_ಎಣಿಕೆ_init()
{
	ರಚನೆ sse_event_info *info;

	SBI_SLIST_FOR_EACH_ನಮೂದು(info, supported_events) {
		ಆದರೆ (EVENT_IS_GLOBAL(info->event_id))
			global_event_ಎಣಿಕೆ++;
		ಇಲ್ಲದಿದ್ದರೆ
			local_event_ಎಣಿಕೆ++;
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sse_global_init()
{
	ರಚನೆ sbi_sse_event *e;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ev = 0;
	ರಚನೆ sse_event_info *info;

	global_events = sbi_zalloc(ಗಾತ್ರ(*global_events) * global_event_ಎಣಿಕೆ);
	ಆದರೆ (!global_events)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	SBI_SLIST_FOR_EACH_ನಮೂದು(info, supported_events) {
		ಆದರೆ (!EVENT_IS_GLOBAL(info->event_id))
			ಮುಂದುವರಿ;

		e = &global_events[ev].event;
		sse_event_init(e, info);
		SPIN_ಬೀಗ_INIT(global_events[ev].ಬೀಗ_2);

		ev++;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ sse_local_init(ರಚನೆ sse_hart_state *shs)
	NO_THREAD_SAFETY_ANALYSIS
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ev = 0;
	ರಚನೆ sse_event_info *info;

	SBI_INIT_LIST_ತಲೆ(&shs->enabled_event_list);
	SPIN_ಬೀಗ_INIT(shs->enabled_event_ಬೀಗ);

	SBI_SLIST_FOR_EACH_ನಮೂದು(info, supported_events) {
		ಆದರೆ (EVENT_IS_GLOBAL(info->event_id))
			ಮುಂದುವರಿ;
		sse_event_init(&shs->local_events[ev++], info);
	}
}

ಪೂರ್ಣಾಂಕ sbi_sse_init(ರಚನೆ sbi_scratch *scratch, bool cold_boot)
{
	ಪೂರ್ಣಾಂಕ ret;
	ಶೂನ್ಯ *sse_inject_mem;
	ರಚನೆ sse_hart_state *shs;
	ರಚನೆ sbi_fifo *sse_inject_q;

	ಆದರೆ (cold_boot) {
		sse_event_ಎಣಿಕೆ_init();

		ret = sse_global_init();
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;

		shs_ptr_off = sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(ಗಾತ್ರ(ಶೂನ್ಯ *));
		ಆದರೆ (!shs_ptr_off)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;

		sse_inject_fifo_off =
			sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(ಗಾತ್ರ(*sse_inject_q));
		ಆದರೆ (!sse_inject_fifo_off) {
			sbi_scratch_free_ಸ್ಥಾನಾಂತರ(shs_ptr_off);
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
		}

		sse_inject_fifo_mem_off = sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(
			(global_event_ಎಣಿಕೆ + local_event_ಎಣಿಕೆ) *
			ಗಾತ್ರ(ರಚನೆ sse_ipi_inject_ದತ್ತಾಂಶ));
		ಆದರೆ (!sse_inject_fifo_mem_off) {
			sbi_scratch_free_ಸ್ಥಾನಾಂತರ(sse_inject_fifo_off);
			sbi_scratch_free_ಸ್ಥಾನಾಂತರ(shs_ptr_off);
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
		}

		ret = sbi_ipi_event_create(&sse_ipi_inject_ops);
		ಆದರೆ (ret < 0) {
			sbi_scratch_free_ಸ್ಥಾನಾಂತರ(shs_ptr_off);
			ಹಿಂತಿರುಗಿ ret;
		}
		sse_ipi_inject_event = ret;
	}

	shs = sse_get_hart_state_ptr(scratch);
	ಆದರೆ (!shs) {
		/* Allocate per hart state and local events at once */
		shs = sbi_zalloc(ಗಾತ್ರ(*shs) + ಗಾತ್ರ(ರಚನೆ sbi_sse_event) *
							local_event_ಎಣಿಕೆ);
		ಆದರೆ (!shs)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;

		shs->local_events = (ರಚನೆ sbi_sse_event *)(shs + 1);

		/* SSE events are masked until hart unmasks them */
		shs->masked = true;

		sse_ಗಣ_hart_state_ptr(scratch, shs);
	}

	sse_local_init(shs);

	sse_inject_q = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, sse_inject_fifo_off);
	sse_inject_mem =
		sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, sse_inject_fifo_mem_off);

	sbi_fifo_init(sse_inject_q, sse_inject_mem,
		      (global_event_ಎಣಿಕೆ + local_event_ಎಣಿಕೆ),
		      ಗಾತ್ರ(ರಚನೆ sse_ipi_inject_ದತ್ತಾಂಶ));

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ sbi_sse_exit(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_sse_event *e;
	ರಚನೆ sse_event_info *info;

	SBI_SLIST_FOR_EACH_ನಮೂದು(info, supported_events) {
		ಆದರೆ (sse_event_get(info->event_id, &e))
			ಮುಂದುವರಿ;

		ಆದರೆ (e->attrs.hartid != current_hartid())
			ಹೋಗು skip;

		ಆದರೆ (sse_event_state(e) > SBI_SSE_STATE_REGISTERED)
			sse_event_ಗಣ_state(e, SBI_SSE_STATE_ಬಳಸದ);

skip:
		sse_event_put(e);
	}
}
