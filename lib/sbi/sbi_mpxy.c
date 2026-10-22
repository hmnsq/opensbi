/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart_protection.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_irqchip.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_mpxy.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_byteorder.h>

/** Shared memory size across all harts */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mpxy_shmem_ਮਾਪ = ਪੰਨਾ_ਮਾਪ;

/** List of MPXY proxy channels */
ਸਥਿਰ SBI_LIST_ਸਿਰ(mpxy_channel_list);

/** Invalid Physical Address(all bits 1) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVALID_ADDR		(-1UL)

/** MPXY Attribute size in bytes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATTR_ਮਾਪ		(4)

/** Channel Capability - MSI */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_MSI_POS		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_MSI_ਨਕਾਬ		(1U << CAP_MSI_POS)

/** Channel Capability - SSE */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_SSE_POS		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_SSE_ਨਕਾਬ		(1U << CAP_SSE_POS)

/** Channel Capability - Events State */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_EVENTSSTATE_POS	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_EVENTSSTATE_ਨਕਾਬ	(1U << CAP_EVENTSSTATE_POS)

/** Channel Capability - Send Message With Response function support */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_SEND_ਸੁਨੇਹਾ_WITH_RESP_POS	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_SEND_ਸੁਨੇਹਾ_WITH_RESP_ਨਕਾਬ	(1U << CAP_SEND_ਸੁਨੇਹਾ_WITH_RESP_POS)

/** Channel Capability - Send Message Without Response function support */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_SEND_ਸੁਨੇਹਾ_WITHOUT_RESP_POS	4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_SEND_ਸੁਨੇਹਾ_WITHOUT_RESP_ਨਕਾਬ	(1U << CAP_SEND_ਸੁਨੇਹਾ_WITHOUT_RESP_POS)

/** Channel Capability - Get Notification function support */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_GET_NOTIFICATIONS_POS	5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_GET_NOTIFICATIONS_ਨਕਾਬ	(1U << CAP_GET_NOTIFICATIONS_POS)

/** Helpers to enable/disable channel capability bits
 * _c: capability variable
 * _m: capability mask
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_ENABLE(_c, _m)	ਪਾਓ_ਖੇਤਰ(_c, _m, 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_DISABLE(_c, _m)	ਪਾਓ_ਖੇਤਰ(_c, _m, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_GET(_c, _m)		ਕੱਢੋ_ਖੇਤਰ(_c, _m)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
ਬਣਤਰ mpxy_shmem {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ shmem_addr_lo;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ shmem_addr_hi;
};

ਬਣਤਰ mpxy_state {
	/* MSI support in MPXY */
	bool msi_avail;
	/* SSE support in MPXY */
	bool sse_avail;
	/* MPXY Shared memory details */
	ਬਣਤਰ mpxy_shmem shmem;
};

ਸਥਿਰ ਬਣਤਰ mpxy_state *sbi_domain_get_mpxy_state(ਬਣਤਰ sbi_domain *dom,
						    u32 hartindex);

/** Macro to obtain the current hart's MPXY state pointer in current domain */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sbi_domain_mpxy_state_thishart_ptr()			\
	sbi_domain_get_mpxy_state(sbi_domain_thishart_ptr(),	\
				  current_hartindex())

/** Disable hart shared memory */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ sbi_mpxy_shmem_disable(ਬਣਤਰ mpxy_state *ms)
{
	ms->shmem.shmem_addr_lo = INVALID_ADDR;
	ms->shmem.shmem_addr_hi = INVALID_ADDR;
}

/** Check if shared memory is already setup on hart */
ਸਥਿਰ ਸਤਰਅੰਦਰ bool mpxy_shmem_enabled(ਬਣਤਰ mpxy_state *ms)
{
	ਵਾਪਸ (ms->shmem.shmem_addr_lo == INVALID_ADDR
		&& ms->shmem.shmem_addr_hi == INVALID_ADDR) ?
		false : true;
}

/** Get hart shared memory base address */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *hart_shmem_ਅਧਾਰ(ਬਣਤਰ mpxy_state *ms)
{
	ਵਾਪਸ (ਖਾਲੀ *)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ assert_ਖੇਤਰ_ਵਿੱਥ(ਖੇਤਰ, attr_ਵਿੱਥ)				\
	_Static_assert(							\
		((offsetof(ਬਣਤਰ sbi_mpxy_channel_attrs, ਖੇਤਰ)) /	\
		 ਆਕਾਰ(u32)) == attr_ਵਿੱਥ,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_ਖੇਤਰ_ਵਿੱਥ(ਸੁਨੇਹਾ_proto_id, SBI_MPXY_ATTR_ਸੁਨੇਹਾ_PROT_ID);
assert_ਖੇਤਰ_ਵਿੱਥ(ਸੁਨੇਹਾ_proto_ਸੰਸਕਰਣ, SBI_MPXY_ATTR_ਸੁਨੇਹਾ_PROT_VER);
assert_ਖੇਤਰ_ਵਿੱਥ(ਸੁਨੇਹਾ_ਅੰਕੜੇ_maxlen, SBI_MPXY_ATTR_ਸੁਨੇਹਾ_MAX_LEN);
assert_ਖੇਤਰ_ਵਿੱਥ(ਸੁਨੇਹਾ_send_timeout, SBI_MPXY_ATTR_ਸੁਨੇਹਾ_SEND_TIMEOUT);
assert_ਖੇਤਰ_ਵਿੱਥ(ਸੁਨੇਹਾ_completion_timeout, SBI_MPXY_ATTR_ਸੁਨੇਹਾ_COMPLETION_TIMEOUT);
assert_ਖੇਤਰ_ਵਿੱਥ(capability, SBI_MPXY_ATTR_CHANNEL_CAPABILITY);
assert_ਖੇਤਰ_ਵਿੱਥ(sse_event_id, SBI_MPXY_ATTR_SSE_EVENT_ID);
assert_ਖੇਤਰ_ਵਿੱਥ(msi_control, SBI_MPXY_ATTR_MSI_CONTROL);
assert_ਖੇਤਰ_ਵਿੱਥ(msi_info.msi_addr_lo, SBI_MPXY_ATTR_MSI_ADDR_LO);
assert_ਖੇਤਰ_ਵਿੱਥ(msi_info.msi_addr_hi, SBI_MPXY_ATTR_MSI_ADDR_HI);
assert_ਖੇਤਰ_ਵਿੱਥ(msi_info.msi_ਅੰਕੜੇ, SBI_MPXY_ATTR_MSI_ਅੰਕੜੇ);
assert_ਖੇਤਰ_ਵਿੱਥ(eventsstate_ctrl, SBI_MPXY_ATTR_EVENTS_STATE_CONTROL);

/**
 * Check if the attribute is a standard attribute or
 * a message protocol specific attribute
 * attr_id[31] = 0 for standard
 * attr_id[31] = 1 for message protocol specific
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ bool mpxy_is_std_attr(u32 attr_id)
{
	ਵਾਪਸ (attr_id >> 31) ? false : true;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool mpxy_channel_visible(ਬਣਤਰ sbi_mpxy_channel *channel,
					ਬਣਤਰ sbi_domain *dom)
{
	ਵਾਪਸ channel->owner_domain == dom;
}

/** Find channel_id in registered channels list for this domain */
ਸਥਿਰ ਬਣਤਰ sbi_mpxy_channel *mpxy_ਲੱਭੋ_channel(u32 channel_id,
					ਬਣਤਰ sbi_domain *dom)
{
	ਬਣਤਰ sbi_mpxy_channel *channel;

	sbi_list_for_each_ਇੰਦਰਾਜ(channel, &mpxy_channel_list, ਸਿਰ) {
		ਜੇ (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			ਵਾਪਸ channel;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

/** Find channel_id in registered channels list */
ਬਣਤਰ sbi_mpxy_channel *sbi_mpxy_ਲੱਭੋ_channel_any(u32 channel_id)
{
	ਬਣਤਰ sbi_mpxy_channel *channel;

	sbi_list_for_each_ਇੰਦਰਾਜ(channel, &mpxy_channel_list, ਸਿਰ) {
		ਜੇ (channel->channel_id == channel_id)
			ਵਾਪਸ channel;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

/** Copy attributes word size */
ਸਥਿਰ ਖਾਲੀ mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 ਗਿਣਤੀ_2)
{
	ਪੂਰਨ idx;
	ਲਈ (idx = 0; idx < ਗਿਣਤੀ_2; idx++)
		outmem[idx] = cpu_ਮੰਜ਼ਿਲ_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_ਉਪਲਬਧ(ਖਾਲੀ)
{
	ਵਾਪਸ sbi_list_ਖਾਲੀ_ਥਾਵਾਂ(&mpxy_channel_list) ? false : true;
}

ਸਥਿਰ ਖਾਲੀ mpxy_std_attrs_init(ਬਣਤਰ sbi_mpxy_channel *channel)
{
	ਬਣਤਰ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_ਅੰਕੜੇ = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	ਜੇ (channel->send_ਸੁਨੇਹਾ_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_ਸੁਨੇਹਾ_WITH_RESP_ਨਕਾਬ);

	ਜੇ (channel->send_ਸੁਨੇਹਾ_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_ਸੁਨੇਹਾ_WITHOUT_RESP_ਨਕਾਬ);

	ਜੇ (channel->get_notification_events) {
		capability = CAP_ENABLE(capability, CAP_GET_NOTIFICATIONS_ਨਕਾਬ);
		/**
		 * Check if MSI or SSE available for notification interrrupt.
		 * Priority given to MSI if both MSI and SSE are avaialble.
		 */
		ਜੇ (ms->msi_avail)
			capability = CAP_ENABLE(capability, CAP_MSI_ਨਕਾਬ);
		ਨਹੀਂਤਾਂ ਜੇ (ms->sse_avail) {
			capability = CAP_ENABLE(capability, CAP_SSE_ਨਕਾਬ);
			/* TODO: Assign SSE EVENT_ID for the channel */
		}

		/**
		 * switch_eventstate callback support means support for events
		 * state reporting supoprt. Enable events state reporting in
		 * channel capability.
		 */
		ਜੇ (channel->switch_eventsstate)
			capability = CAP_ENABLE(capability, CAP_EVENTSSTATE_ਨਕਾਬ);
	}

	channel->attrs.capability = capability;
}

/**
 * Register a channel with MPXY framework.
 * Called by message protocol drivers
 */
ਪੂਰਨ sbi_mpxy_ਰਜਿਸਟਰ_channel(ਬਣਤਰ sbi_mpxy_channel *channel)
{
	ਜੇ (!channel)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (!channel->owner_domain)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (sbi_mpxy_ਲੱਭੋ_channel_any(channel->channel_id))
		ਵਾਪਸ SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	ਜੇ (mpxy_shmem_ਮਾਪ < channel->attrs.ਸੁਨੇਹਾ_ਅੰਕੜੇ_maxlen) {
		mpxy_shmem_ਮਾਪ = ROUNDUP(channel->attrs.ਸੁਨੇਹਾ_ਅੰਕੜੇ_maxlen,
					  ਪੰਨਾ_ਮਾਪ);
	}

	sbi_list_ਜੋੜੋ_ਪੂਛ(&channel->ਸਿਰ, &mpxy_channel_list);

	ਵਾਪਸ SBI_OK;
}

/** Setup per domain MPXY state data */
ਸਥਿਰ ਪੂਰਨ domain_mpxy_state_ਤਿਆਰੀ(ਬਣਤਰ sbi_domain *dom,
					ਬਣਤਰ sbi_domain_state *state,
					ਖਾਲੀ *state_ptr)
{
	ਬਣਤਰ mpxy_state **dom_hartindex_ਮੰਜ਼ਿਲ_mpxy_state_ਸਾਰਣੀ = state_ptr;
	ਬਣਤਰ mpxy_state *ms;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ms = sbi_zalloc(ਆਕਾਰ(*ms));
		ਜੇ (!ms)
			ਵਾਪਸ SBI_ENOMEM;

		/*
		 * TODO: Proper support for checking sse support from
		 * platform. Currently disable sse and use polling
		 */
		ms->msi_avail = !!sbi_irqchip_ਲੱਭੋ_device_by_caps(SBI_IRQCHIP_CAPS_MSI, ਨਿਰਾ);
		ms->sse_avail = false;

		sbi_mpxy_shmem_disable(ms);

		dom_hartindex_ਮੰਜ਼ਿਲ_mpxy_state_ਸਾਰਣੀ[i] = ms;
	}

	ਵਾਪਸ 0;
}

/** Cleanup per domain MPXY state data */
ਸਥਿਰ ਖਾਲੀ domain_mpxy_state_cleanup(ਬਣਤਰ sbi_domain *dom,
					   ਬਣਤਰ sbi_domain_state *state,
					   ਖਾਲੀ *state_ptr)
{
	ਬਣਤਰ mpxy_state **dom_hartindex_ਮੰਜ਼ਿਲ_mpxy_state_ਸਾਰਣੀ = state_ptr;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts)
		sbi_free(dom_hartindex_ਮੰਜ਼ਿਲ_mpxy_state_ਸਾਰਣੀ[i]);
}

ਸਥਿਰ ਬਣਤਰ sbi_domain_state dmstate = {
	.state_ਤਿਆਰੀ = domain_mpxy_state_ਤਿਆਰੀ,
	.state_cleanup = domain_mpxy_state_cleanup,
};

/**
 * Get per-domain MPXY state pointer for a given domain and HART index
 * @param dom pointer to domain
 * @param hartindex the HART index
 *
 * @return per-domain MPXY state pointer for given HART index
 */
ਸਥਿਰ ਬਣਤਰ mpxy_state *sbi_domain_get_mpxy_state(ਬਣਤਰ sbi_domain *dom,
						    u32 hartindex)
{
	ਬਣਤਰ mpxy_state **dom_hartindex_ਮੰਜ਼ਿਲ_mpxy_state_ਸਾਰਣੀ;

	dom_hartindex_ਮੰਜ਼ਿਲ_mpxy_state_ਸਾਰਣੀ = sbi_domain_state_ptr(dom, &dmstate);
	ਜੇ (!dom_hartindex_ਮੰਜ਼ਿਲ_mpxy_state_ਸਾਰਣੀ ||
	    !sbi_hartindex_valid(hartindex))
		ਵਾਪਸ ਨਿਰਾ;

	ਵਾਪਸ dom_hartindex_ਮੰਜ਼ਿਲ_mpxy_state_ਸਾਰਣੀ[hartindex];
}

ਪੂਰਨ sbi_mpxy_init(ਬਣਤਰ sbi_scratch *scratch)
{
	ਪੂਰਨ ret;

	/**
	 * Allocate per-domain and per-hart MPXY state data.
	 * The data type is "struct mpxy_state **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one() and
	 * domain_mpxy_state_setup(). Calculate needed size of memory space
	 * here.
	 */
	dmstate.state_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ mpxy_state *) * sbi_hart_ਗਿਣਤੀ();
	ret = sbi_domain_ਰਜਿਸਟਰ_state(&dmstate);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ਵਾਪਸ sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_mpxy_get_shmem_ਮਾਪ(ਖਾਲੀ)
{
	ਵਾਪਸ mpxy_shmem_ਮਾਪ;
}

ਪੂਰਨ sbi_mpxy_ਸੈੱਟ_shmem(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ shmem_phys_lo,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ shmem_phys_hi,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags)
{
	ਬਣਤਰ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	ਜੇ (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		ਵਾਪਸ SBI_SUCCESS;
	}

	ਜੇ (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	ਜੇ (shmem_phys_lo & ~ਪੰਨਾ_ਨਕਾਬ)
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	/*
	* On RV32, the M-mode can only access the first 4GB of
	* the physical address space because M-mode does not have
	* MMU to access full 34-bit physical address space.
	* So fail if the upper 32 bits of the physical address
	* is non-zero on RV32.
	*
	* On RV64, kernel sets upper 64bit address part to zero.
	* So fail if the upper 64bit of the physical address
	* is non-zero on RV64.
	*/
	ਜੇ (shmem_phys_hi)
		ਵਾਪਸ SBI_ERR_INVALID_ਪਤਾ;

	ਜੇ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_ਮਾਪ, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ਵਾਪਸ SBI_ERR_INVALID_ਪਤਾ;

	/** Save the current shmem details in new shmem region */
	ਜੇ (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
		ret_buf = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *)(ulong)SHMEM_PHYS_ADDR(shmem_phys_hi,
								  shmem_phys_lo);
		sbi_hart_protection_ਆਰਜ਼ੀ_map_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ret_buf, mpxy_shmem_ਮਾਪ);
		ret_buf[0] = cpu_ਮੰਜ਼ਿਲ_lle(ms->shmem.shmem_addr_lo);
		ret_buf[1] = cpu_ਮੰਜ਼ਿਲ_lle(ms->shmem.shmem_addr_hi);
		sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ret_buf, mpxy_shmem_ਮਾਪ);
	}

	/** Setup the new shared memory */
	ms->shmem.shmem_addr_lo = shmem_phys_lo;
	ms->shmem.shmem_addr_hi = shmem_phys_hi;

	ਵਾਪਸ SBI_SUCCESS;
}

ਪੂਰਨ sbi_mpxy_get_channel_ids(u32 ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ)
{
	ਬਣਤਰ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 ਬਚਿਆ, returned, max_channelids;
	u32 ਗੰਢ_ਸੂਚਕਾਂਕ = 0, ਗੰਢ_ret = 0;
	ਬਣਤਰ sbi_mpxy_channel *channel;
	u32 channels_ਗਿਣਤੀ = 0;
	u32 *shmem_ਅਧਾਰ;
	ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();

	ਜੇ (!mpxy_shmem_enabled(ms))
		ਵਾਪਸ SBI_ERR_NO_SHMEM;

	sbi_list_for_each_ਇੰਦਰਾਜ(channel, &mpxy_channel_list, ਸਿਰ) {
		ਜੇ (mpxy_channel_visible(channel, dom))
			channels_ਗਿਣਤੀ += 1;
	}

	ਜੇ (ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ > channels_ਗਿਣਤੀ)
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	shmem_ਅਧਾਰ = hart_shmem_ਅਧਾਰ(ms);
	sbi_hart_protection_ਆਰਜ਼ੀ_map_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hart_shmem_ਅਧਾਰ(ms), mpxy_shmem_ਮਾਪ);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_ਮਾਪ / ਆਕਾਰ(u32)) - 2;
	/* total remaining from the start index */
	ਬਚਿਆ = channels_ਗਿਣਤੀ - ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ;
	/* how many can be returned */
	returned = (ਬਚਿਆ > max_channelids)? max_channelids : ਬਚਿਆ;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_ਇੰਦਰਾਜ(channel, &mpxy_channel_list, ਸਿਰ) {
		ਜੇ (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			ਜਾਰੀ;

		ਜੇ (ਗੰਢ_ਸੂਚਕਾਂਕ >= ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ &&
			ਗੰਢ_ਸੂਚਕਾਂਕ < (ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + returned)) {
			shmem_ਅਧਾਰ[2 + ਗੰਢ_ret] = cpu_ਮੰਜ਼ਿਲ_le32(channel->channel_id);
			ਗੰਢ_ret += 1;
		}

		ਗੰਢ_ਸੂਚਕਾਂਕ += 1;
	}

	/* final remaininig channel ids */
	ਬਚਿਆ = channels_ਗਿਣਤੀ - (ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + returned);

	shmem_ਅਧਾਰ[0] = cpu_ਮੰਜ਼ਿਲ_le32(ਬਚਿਆ);
	shmem_ਅਧਾਰ[1] = cpu_ਮੰਜ਼ਿਲ_le32(returned);

	sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hart_shmem_ਅਧਾਰ(ms), mpxy_shmem_ਮਾਪ);

	ਵਾਪਸ SBI_SUCCESS;
}

ਪੂਰਨ sbi_mpxy_read_attrs(u32 channel_id, u32 ਅਧਾਰ_attr_id, u32 attr_ਗਿਣਤੀ)
{
	ਬਣਤਰ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ਪੂਰਨ ret = SBI_SUCCESS;
	u32 *attr_ptr, ਸਿਰਾ_id;
	ਖਾਲੀ *shmem_ਅਧਾਰ;
	ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();

	ਜੇ (!mpxy_shmem_enabled(ms))
		ਵਾਪਸ SBI_ERR_NO_SHMEM;

	ਬਣਤਰ sbi_mpxy_channel *channel = mpxy_ਲੱਭੋ_channel(channel_id, dom);
	ਜੇ (!channel)
		ਵਾਪਸ SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	ਜੇ (ਅਧਾਰ_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		ਅਧਾਰ_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_ਸ਼ੁਰੂ)
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	ਜੇ (!attr_ਗਿਣਤੀ || (attr_ਗਿਣਤੀ > (mpxy_shmem_ਮਾਪ / ATTR_ਮਾਪ)))
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	shmem_ਅਧਾਰ = hart_shmem_ਅਧਾਰ(ms);
	ਸਿਰਾ_id = ਅਧਾਰ_attr_id + attr_ਗਿਣਤੀ - 1;

	sbi_hart_protection_ਆਰਜ਼ੀ_map_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hart_shmem_ਅਧਾਰ(ms), mpxy_shmem_ਮਾਪ);

	/* Standard attributes range check */
	ਜੇ (mpxy_is_std_attr(ਅਧਾਰ_attr_id)) {
		ਜੇ (ਸਿਰਾ_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			ਜਾਓ ਬਾਹਰਲਾ;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_ਅਧਾਰ, &attr_ptr[ਅਧਾਰ_attr_id],
				    attr_ਗਿਣਤੀ);
	} ਨਹੀਂਤਾਂ {
		/**
		 * Even if the message protocol driver does not provide
		 * read attribute callback, return bad range error instead
		 * of not supported to let client distinguish it from channel
		 * id not supported.
		 * Check the complate range supported for message protocol
		 * attributes. Actual supported attributes will be checked
		 * by the message protocol driver.
		 */
		ਜੇ (!channel->read_ਗੁਣ ||
				ਸਿਰਾ_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_ਸਿਰਾ) {
			ret = SBI_ERR_BAD_RANGE;
			ਜਾਓ ਬਾਹਰਲਾ;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_ਗੁਣ(channel,
					       (u32 *)shmem_ਅਧਾਰ,
					       ਅਧਾਰ_attr_id, attr_ਗਿਣਤੀ);
	}
ਬਾਹਰਲਾ:
	sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hart_shmem_ਅਧਾਰ(ms), mpxy_shmem_ਮਾਪ);
	ਵਾਪਸ ret;
}

/**
 * Verify the channel standard attribute wrt to write permission
 * and the value to be set if valid or not.
 * Only attributes needs to be checked which are defined Read/Write
 * permission. Other with Readonly permission will result in error.
 *
 * Attributes values to be written must also be checked because
 * before writing a range of attributes, we need to make sure that
 * either complete range of attributes is written successfully or not
 * at all.
 */
ਸਥਿਰ ਪੂਰਨ mpxy_check_write_std_attr(ਬਣਤਰ sbi_mpxy_channel *channel,
				     u32 attr_id, u32 attr_val)
{
	ਬਣਤਰ sbi_mpxy_channel_attrs *attrs = &channel->attrs;
	ਪੂਰਨ ret = SBI_SUCCESS;

	ਚੋਣ(attr_id) {
	ਹਾਲਤ SBI_MPXY_ATTR_MSI_CONTROL:
		ਜੇ (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		ਜੇ (attr_val == 1 &&
		    (attrs->msi_info.msi_addr_lo == (u32)INVALID_ADDR) &&
		    (attrs->msi_info.msi_addr_hi == (u32)INVALID_ADDR))
			ret = SBI_ERR_DENIED;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_MPXY_ATTR_MSI_ADDR_LO:
	ਹਾਲਤ SBI_MPXY_ATTR_MSI_ADDR_HI:
	ਹਾਲਤ SBI_MPXY_ATTR_MSI_ਅੰਕੜੇ:
		ret = SBI_SUCCESS;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		ਜੇ (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		ਰੋਕੋ;
	ਮੂਲ:
		/** All RO access attributes falls under default */
		ret = SBI_ERR_BAD_RANGE;
	};

	ਵਾਪਸ ret;
}

/**
 * Write the attribute value
 */
ਸਥਿਰ ਖਾਲੀ mpxy_write_std_attr(ਬਣਤਰ sbi_mpxy_channel *channel, u32 attr_id,
				u32 attr_val)
{
	ਬਣਤਰ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ਬਣਤਰ sbi_mpxy_channel_attrs *attrs = &channel->attrs;

	ਚੋਣ(attr_id) {
	ਹਾਲਤ SBI_MPXY_ATTR_MSI_CONTROL:
		ਜੇ (ms->msi_avail && attr_val <= 1)
			attrs->msi_control = attr_val;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_MPXY_ATTR_MSI_ADDR_LO:
		ਜੇ (ms->msi_avail)
			attrs->msi_info.msi_addr_lo = attr_val;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_MPXY_ATTR_MSI_ADDR_HI:
		ਜੇ (ms->msi_avail)
			attrs->msi_info.msi_addr_hi = attr_val;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_MPXY_ATTR_MSI_ਅੰਕੜੇ:
		ਜੇ (ms->msi_avail)
			attrs->msi_info.msi_ਅੰਕੜੇ = attr_val;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		ਜੇ (channel->switch_eventsstate && attr_val <= 1) {
			attrs->eventsstate_ctrl = attr_val;
			 /* message protocol callback to enable/disable
			 * events state reporting. */
			channel->switch_eventsstate(attr_val);
		}

		ਰੋਕੋ;
	};
}

ਪੂਰਨ sbi_mpxy_write_attrs(u32 channel_id, u32 ਅਧਾਰ_attr_id, u32 attr_ਗਿਣਤੀ)
{
	ਬਣਤਰ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, ਸਿਰਾ_id, attr_val;
	ਬਣਤਰ sbi_mpxy_channel *channel;
	ਪੂਰਨ ret, mem_idx;
	ਖਾਲੀ *shmem_ਅਧਾਰ;
	ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();

	ਜੇ (!mpxy_shmem_enabled(ms))
		ਵਾਪਸ SBI_ERR_NO_SHMEM;

	channel = mpxy_ਲੱਭੋ_channel(channel_id, dom);
	ਜੇ (!channel)
		ਵਾਪਸ SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	ਜੇ (ਅਧਾਰ_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		ਅਧਾਰ_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_ਸ਼ੁਰੂ)
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	ਜੇ (!attr_ਗਿਣਤੀ || (attr_ਗਿਣਤੀ > (mpxy_shmem_ਮਾਪ / ATTR_ਮਾਪ)))
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	shmem_ਅਧਾਰ = hart_shmem_ਅਧਾਰ(ms);
	ਸਿਰਾ_id = ਅਧਾਰ_attr_id + attr_ਗਿਣਤੀ - 1;

	sbi_hart_protection_ਆਰਜ਼ੀ_map_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ, mpxy_shmem_ਮਾਪ);

	mem_ptr = (u32 *)shmem_ਅਧਾਰ;

	ਜੇ (mpxy_is_std_attr(ਅਧਾਰ_attr_id)) {
		ਜੇ (ਸਿਰਾ_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			ਜਾਓ ਬਾਹਰਲਾ;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		ਲਈ (attr_id = ਅਧਾਰ_attr_id; attr_id <= ਸਿਰਾ_id; attr_id++) {
			attr_val = le32_ਮੰਜ਼ਿਲ_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			ਜੇ (ret)
				ਜਾਓ ਬਾਹਰਲਾ;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		ਲਈ (attr_id = ਅਧਾਰ_attr_id; attr_id <= ਸਿਰਾ_id; attr_id++) {
			attr_val = le32_ਮੰਜ਼ਿਲ_cpu(mem_ptr[mem_idx++]);
			mpxy_write_std_attr(channel, attr_id, attr_val);
		}
	} ਨਹੀਂਤਾਂ {/**
		 * Message protocol specific attributes:
		 * If attributes belong to message protocol, they
		 * are simply passed to the message protocol driver
		 * callback after checking the valid range.
		 * Attributes contiguous range & permission & other checks
		 * are done by the mpxy and message protocol glue layer.
		 */
		/**
		 * Even if the message protocol driver does not provide
		 * write attribute callback, return bad range error instead
		 * of not supported to let client distinguish it from channel
		 * id not supported.
		 */
		ਜੇ (!channel->write_ਗੁਣ ||
				ਸਿਰਾ_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_ਸਿਰਾ) {
			ret = SBI_ERR_BAD_RANGE;
			ਜਾਓ ਬਾਹਰਲਾ;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_ਗੁਣ(channel,
					       (u32 *)shmem_ਅਧਾਰ,
					       ਅਧਾਰ_attr_id, attr_ਗਿਣਤੀ);
	}
ਬਾਹਰਲਾ:
	sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ, mpxy_shmem_ਮਾਪ);
	ਵਾਪਸ ret;
}

ਪੂਰਨ sbi_mpxy_send_ਸੁਨੇਹਾ(u32 channel_id, u8 ਸੁਨੇਹਾ_id,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸੁਨੇਹਾ_ਅੰਕੜੇ_len,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *resp_ਅੰਕੜੇ_len)
{
	ਬਣਤਰ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ਬਣਤਰ sbi_mpxy_channel *channel;
	ਖਾਲੀ *shmem_ਅਧਾਰ, *resp_buf;
	u32 resp_bufsize;
	ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();
	ਪੂਰਨ ret;

	ਜੇ (!mpxy_shmem_enabled(ms))
		ਵਾਪਸ SBI_ERR_NO_SHMEM;

	channel = mpxy_ਲੱਭੋ_channel(channel_id, dom);
	ਜੇ (!channel)
		ਵਾਪਸ SBI_ERR_NOT_SUPPORTED;

	ਜੇ (resp_ਅੰਕੜੇ_len && !channel->send_ਸੁਨੇਹਾ_with_response)
		ਵਾਪਸ SBI_ERR_NOT_SUPPORTED;

	ਜੇ (!resp_ਅੰਕੜੇ_len && !channel->send_ਸੁਨੇਹਾ_without_response)
		ਵਾਪਸ SBI_ERR_NOT_SUPPORTED;

	ਜੇ (ਸੁਨੇਹਾ_ਅੰਕੜੇ_len > mpxy_shmem_ਮਾਪ ||
		ਸੁਨੇਹਾ_ਅੰਕੜੇ_len > channel->attrs.ਸੁਨੇਹਾ_ਅੰਕੜੇ_maxlen)
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	shmem_ਅਧਾਰ = hart_shmem_ਅਧਾਰ(ms);
	sbi_hart_protection_ਆਰਜ਼ੀ_map_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ, mpxy_shmem_ਮਾਪ);

	ਜੇ (resp_ਅੰਕੜੇ_len) {
		resp_buf = shmem_ਅਧਾਰ;
		resp_bufsize = mpxy_shmem_ਮਾਪ;
		ret = channel->send_ਸੁਨੇਹਾ_with_response(channel, ਸੁਨੇਹਾ_id,
							  shmem_ਅਧਾਰ,
							  ਸੁਨੇਹਾ_ਅੰਕੜੇ_len,
							  resp_buf,
							  resp_bufsize,
							  resp_ਅੰਕੜੇ_len);
	} ਨਹੀਂਤਾਂ {
		ret = channel->send_ਸੁਨੇਹਾ_without_response(channel, ਸੁਨੇਹਾ_id,
							     shmem_ਅਧਾਰ,
							     ਸੁਨੇਹਾ_ਅੰਕੜੇ_len);
	}

	sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ, mpxy_shmem_ਮਾਪ);

	ਜੇ (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		ਵਾਪਸ ret;
	ਨਹੀਂਤਾਂ ਜੇ (ret)
		ਵਾਪਸ SBI_ERR_FAILED;

	ਜੇ (resp_ਅੰਕੜੇ_len &&
	    (*resp_ਅੰਕੜੇ_len > mpxy_shmem_ਮਾਪ ||
	     *resp_ਅੰਕੜੇ_len > channel->attrs.ਸੁਨੇਹਾ_ਅੰਕੜੇ_maxlen))
		ਵਾਪਸ SBI_ERR_FAILED;

	ਵਾਪਸ SBI_SUCCESS;
}

ਪੂਰਨ sbi_mpxy_get_notification_events(u32 channel_id, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *events_len)
{
	ਬਣਤਰ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ਬਣਤਰ sbi_mpxy_channel *channel;
	ਖਾਲੀ *eventsbuf, *shmem_ਅਧਾਰ;
	ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();
	ਪੂਰਨ ret;

	ਜੇ (!mpxy_shmem_enabled(ms))
		ਵਾਪਸ SBI_ERR_NO_SHMEM;

	channel = mpxy_ਲੱਭੋ_channel(channel_id, dom);
	ਜੇ (!channel || !channel->get_notification_events)
		ਵਾਪਸ SBI_ERR_NOT_SUPPORTED;

	shmem_ਅਧਾਰ = hart_shmem_ਅਧਾਰ(ms);
	sbi_hart_protection_ਆਰਜ਼ੀ_map_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ, mpxy_shmem_ਮਾਪ);
	eventsbuf = shmem_ਅਧਾਰ;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_ਮਾਪ,
					       events_len);
	sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ, mpxy_shmem_ਮਾਪ);

	ਜੇ (ret)
		ਵਾਪਸ ret;

	ਜੇ (*events_len > (mpxy_shmem_ਮਾਪ - 16))
		ਵਾਪਸ SBI_ERR_FAILED;

	ਵਾਪਸ SBI_SUCCESS;
}
