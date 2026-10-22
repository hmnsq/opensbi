/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart_protection.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_irqchip.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_mpxy.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_byteorder.h>

/** Shared memory size across all harts */
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mpxy_shmem_ಗಾತ್ರ = ಪುಟ_ಗಾತ್ರ;

/** List of MPXY proxy channels */
ಸ್ಥಿರ SBI_LIST_ತಲೆ(mpxy_channel_list);

/** Invalid Physical Address(all bits 1) */
#ವ್ಯಾಖ್ಯಾನಿಸು INVALID_ADDR		(-1UL)

/** MPXY Attribute size in bytes */
#ವ್ಯಾಖ್ಯಾನಿಸು ATTR_ಗಾತ್ರ		(4)

/** Channel Capability - MSI */
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_MSI_POS		0
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_MSI_ಮುಖವಾಡ		(1U << CAP_MSI_POS)

/** Channel Capability - SSE */
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_SSE_POS		1
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_SSE_ಮುಖವಾಡ		(1U << CAP_SSE_POS)

/** Channel Capability - Events State */
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_EVENTSSTATE_POS	2
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_EVENTSSTATE_ಮುಖವಾಡ	(1U << CAP_EVENTSSTATE_POS)

/** Channel Capability - Send Message With Response function support */
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_SEND_ಸಂದೇಶ_WITH_RESP_POS	3
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_SEND_ಸಂದೇಶ_WITH_RESP_ಮುಖವಾಡ	(1U << CAP_SEND_ಸಂದೇಶ_WITH_RESP_POS)

/** Channel Capability - Send Message Without Response function support */
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_SEND_ಸಂದೇಶ_WITHOUT_RESP_POS	4
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_SEND_ಸಂದೇಶ_WITHOUT_RESP_ಮುಖವಾಡ	(1U << CAP_SEND_ಸಂದೇಶ_WITHOUT_RESP_POS)

/** Channel Capability - Get Notification function support */
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_GET_NOTIFICATIONS_POS	5
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_GET_NOTIFICATIONS_ಮುಖವಾಡ	(1U << CAP_GET_NOTIFICATIONS_POS)

/** Helpers to enable/disable channel capability bits
 * _c: capability variable
 * _m: capability mask
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_ENABLE(_c, _m)	ಸೇರಿಸು_ಕ್ಷೇತ್ರ(_c, _m, 1)
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_DISABLE(_c, _m)	ಸೇರಿಸು_ಕ್ಷೇತ್ರ(_c, _m, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_GET(_c, _m)		ಹೊರತೆಗೆ_ಕ್ಷೇತ್ರ(_c, _m)

#ವ್ಯಾಖ್ಯಾನಿಸು SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
ರಚನೆ mpxy_shmem {
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ shmem_addr_lo;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ shmem_addr_hi;
};

ರಚನೆ mpxy_state {
	/* MSI support in MPXY */
	bool msi_avail;
	/* SSE support in MPXY */
	bool sse_avail;
	/* MPXY Shared memory details */
	ರಚನೆ mpxy_shmem shmem;
};

ಸ್ಥಿರ ರಚನೆ mpxy_state *sbi_domain_get_mpxy_state(ರಚನೆ sbi_domain *dom,
						    u32 hartindex);

/** Macro to obtain the current hart's MPXY state pointer in current domain */
#ವ್ಯಾಖ್ಯಾನಿಸು sbi_domain_mpxy_state_thishart_ptr()			\
	sbi_domain_get_mpxy_state(sbi_domain_thishart_ptr(),	\
				  current_hartindex())

/** Disable hart shared memory */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ sbi_mpxy_shmem_disable(ರಚನೆ mpxy_state *ms)
{
	ms->shmem.shmem_addr_lo = INVALID_ADDR;
	ms->shmem.shmem_addr_hi = INVALID_ADDR;
}

/** Check if shared memory is already setup on hart */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool mpxy_shmem_enabled(ರಚನೆ mpxy_state *ms)
{
	ಹಿಂತಿರುಗಿ (ms->shmem.shmem_addr_lo == INVALID_ADDR
		&& ms->shmem.shmem_addr_hi == INVALID_ADDR) ?
		false : true;
}

/** Get hart shared memory base address */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *hart_shmem_ತಳ(ರಚನೆ mpxy_state *ms)
{
	ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#ವ್ಯಾಖ್ಯಾನಿಸು assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(ಕ್ಷೇತ್ರ, attr_ಸ್ಥಾನಾಂತರ)				\
	_Static_assert(							\
		((offsetof(ರಚನೆ sbi_mpxy_channel_attrs, ಕ್ಷೇತ್ರ)) /	\
		 ಗಾತ್ರ(u32)) == attr_ಸ್ಥಾನಾಂತರ,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(ಸಂದೇಶ_proto_id, SBI_MPXY_ATTR_ಸಂದೇಶ_PROT_ID);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(ಸಂದೇಶ_proto_ಆವೃತ್ತಿ, SBI_MPXY_ATTR_ಸಂದೇಶ_PROT_VER);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(ಸಂದೇಶ_ದತ್ತಾಂಶ_maxlen, SBI_MPXY_ATTR_ಸಂದೇಶ_MAX_LEN);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(ಸಂದೇಶ_send_timeout, SBI_MPXY_ATTR_ಸಂದೇಶ_SEND_TIMEOUT);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(ಸಂದೇಶ_completion_timeout, SBI_MPXY_ATTR_ಸಂದೇಶ_COMPLETION_TIMEOUT);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(capability, SBI_MPXY_ATTR_CHANNEL_CAPABILITY);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(sse_event_id, SBI_MPXY_ATTR_SSE_EVENT_ID);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(msi_control, SBI_MPXY_ATTR_MSI_CONTROL);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(msi_info.msi_addr_lo, SBI_MPXY_ATTR_MSI_ADDR_LO);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(msi_info.msi_addr_hi, SBI_MPXY_ATTR_MSI_ADDR_HI);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(msi_info.msi_ದತ್ತಾಂಶ, SBI_MPXY_ATTR_MSI_ದತ್ತಾಂಶ);
assert_ಕ್ಷೇತ್ರ_ಸ್ಥಾನಾಂತರ(eventsstate_ctrl, SBI_MPXY_ATTR_EVENTS_STATE_CONTROL);

/**
 * Check if the attribute is a standard attribute or
 * a message protocol specific attribute
 * attr_id[31] = 0 for standard
 * attr_id[31] = 1 for message protocol specific
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool mpxy_is_std_attr(u32 attr_id)
{
	ಹಿಂತಿರುಗಿ (attr_id >> 31) ? false : true;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool mpxy_channel_visible(ರಚನೆ sbi_mpxy_channel *channel,
					ರಚನೆ sbi_domain *dom)
{
	ಹಿಂತಿರುಗಿ channel->owner_domain == dom;
}

/** Find channel_id in registered channels list for this domain */
ಸ್ಥಿರ ರಚನೆ sbi_mpxy_channel *mpxy_ಕಂಡುಹಿಡಿ_channel(u32 channel_id,
					ರಚನೆ sbi_domain *dom)
{
	ರಚನೆ sbi_mpxy_channel *channel;

	sbi_list_for_each_ನಮೂದು(channel, &mpxy_channel_list, ತಲೆ) {
		ಆದರೆ (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			ಹಿಂತಿರುಗಿ channel;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/** Find channel_id in registered channels list */
ರಚನೆ sbi_mpxy_channel *sbi_mpxy_ಕಂಡುಹಿಡಿ_channel_any(u32 channel_id)
{
	ರಚನೆ sbi_mpxy_channel *channel;

	sbi_list_for_each_ನಮೂದು(channel, &mpxy_channel_list, ತಲೆ) {
		ಆದರೆ (channel->channel_id == channel_id)
			ಹಿಂತಿರುಗಿ channel;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/** Copy attributes word size */
ಸ್ಥಿರ ಶೂನ್ಯ mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 ಎಣಿಕೆ_2)
{
	ಪೂರ್ಣಾಂಕ idx;
	ಗಾಗಿ (idx = 0; idx < ಎಣಿಕೆ_2; idx++)
		outmem[idx] = cpu_ಗಮ್ಯ_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_ಲಭ್ಯ(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ sbi_list_ಖಾಲಿಗಳು(&mpxy_channel_list) ? false : true;
}

ಸ್ಥಿರ ಶೂನ್ಯ mpxy_std_attrs_init(ರಚನೆ sbi_mpxy_channel *channel)
{
	ರಚನೆ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_ದತ್ತಾಂಶ = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	ಆದರೆ (channel->send_ಸಂದೇಶ_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_ಸಂದೇಶ_WITH_RESP_ಮುಖವಾಡ);

	ಆದರೆ (channel->send_ಸಂದೇಶ_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_ಸಂದೇಶ_WITHOUT_RESP_ಮುಖವಾಡ);

	ಆದರೆ (channel->get_notification_events) {
		capability = CAP_ENABLE(capability, CAP_GET_NOTIFICATIONS_ಮುಖವಾಡ);
		/**
		 * Check if MSI or SSE available for notification interrrupt.
		 * Priority given to MSI if both MSI and SSE are avaialble.
		 */
		ಆದರೆ (ms->msi_avail)
			capability = CAP_ENABLE(capability, CAP_MSI_ಮುಖವಾಡ);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ms->sse_avail) {
			capability = CAP_ENABLE(capability, CAP_SSE_ಮುಖವಾಡ);
			/* TODO: Assign SSE EVENT_ID for the channel */
		}

		/**
		 * switch_eventstate callback support means support for events
		 * state reporting supoprt. Enable events state reporting in
		 * channel capability.
		 */
		ಆದರೆ (channel->switch_eventsstate)
			capability = CAP_ENABLE(capability, CAP_EVENTSSTATE_ಮುಖವಾಡ);
	}

	channel->attrs.capability = capability;
}

/**
 * Register a channel with MPXY framework.
 * Called by message protocol drivers
 */
ಪೂರ್ಣಾಂಕ sbi_mpxy_ರಿಜಿಸ್ಟರ್_channel(ರಚನೆ sbi_mpxy_channel *channel)
{
	ಆದರೆ (!channel)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (!channel->owner_domain)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (sbi_mpxy_ಕಂಡುಹಿಡಿ_channel_any(channel->channel_id))
		ಹಿಂತಿರುಗಿ SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	ಆದರೆ (mpxy_shmem_ಗಾತ್ರ < channel->attrs.ಸಂದೇಶ_ದತ್ತಾಂಶ_maxlen) {
		mpxy_shmem_ಗಾತ್ರ = ROUNDUP(channel->attrs.ಸಂದೇಶ_ದತ್ತಾಂಶ_maxlen,
					  ಪುಟ_ಗಾತ್ರ);
	}

	sbi_list_ಕೂಡಿಸು_ಬಾಲ(&channel->ತಲೆ, &mpxy_channel_list);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

/** Setup per domain MPXY state data */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ domain_mpxy_state_ಸಿದ್ಧತೆ(ರಚನೆ sbi_domain *dom,
					ರಚನೆ sbi_domain_state *state,
					ಶೂನ್ಯ *state_ptr)
{
	ರಚನೆ mpxy_state **dom_hartindex_ಗಮ್ಯ_mpxy_state_ಕೋಷ್ಟಕ = state_ptr;
	ರಚನೆ mpxy_state *ms;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ms = sbi_zalloc(ಗಾತ್ರ(*ms));
		ಆದರೆ (!ms)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;

		/*
		 * TODO: Proper support for checking sse support from
		 * platform. Currently disable sse and use polling
		 */
		ms->msi_avail = !!sbi_irqchip_ಕಂಡುಹಿಡಿ_device_by_caps(SBI_IRQCHIP_CAPS_MSI, ಖಾಲಿ);
		ms->sse_avail = false;

		sbi_mpxy_shmem_disable(ms);

		dom_hartindex_ಗಮ್ಯ_mpxy_state_ಕೋಷ್ಟಕ[i] = ms;
	}

	ಹಿಂತಿರುಗಿ 0;
}

/** Cleanup per domain MPXY state data */
ಸ್ಥಿರ ಶೂನ್ಯ domain_mpxy_state_cleanup(ರಚನೆ sbi_domain *dom,
					   ರಚನೆ sbi_domain_state *state,
					   ಶೂನ್ಯ *state_ptr)
{
	ರಚನೆ mpxy_state **dom_hartindex_ಗಮ್ಯ_mpxy_state_ಕೋಷ್ಟಕ = state_ptr;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts)
		sbi_free(dom_hartindex_ಗಮ್ಯ_mpxy_state_ಕೋಷ್ಟಕ[i]);
}

ಸ್ಥಿರ ರಚನೆ sbi_domain_state dmstate = {
	.state_ಸಿದ್ಧತೆ = domain_mpxy_state_ಸಿದ್ಧತೆ,
	.state_cleanup = domain_mpxy_state_cleanup,
};

/**
 * Get per-domain MPXY state pointer for a given domain and HART index
 * @param dom pointer to domain
 * @param hartindex the HART index
 *
 * @return per-domain MPXY state pointer for given HART index
 */
ಸ್ಥಿರ ರಚನೆ mpxy_state *sbi_domain_get_mpxy_state(ರಚನೆ sbi_domain *dom,
						    u32 hartindex)
{
	ರಚನೆ mpxy_state **dom_hartindex_ಗಮ್ಯ_mpxy_state_ಕೋಷ್ಟಕ;

	dom_hartindex_ಗಮ್ಯ_mpxy_state_ಕೋಷ್ಟಕ = sbi_domain_state_ptr(dom, &dmstate);
	ಆದರೆ (!dom_hartindex_ಗಮ್ಯ_mpxy_state_ಕೋಷ್ಟಕ ||
	    !sbi_hartindex_valid(hartindex))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ dom_hartindex_ಗಮ್ಯ_mpxy_state_ಕೋಷ್ಟಕ[hartindex];
}

ಪೂರ್ಣಾಂಕ sbi_mpxy_init(ರಚನೆ sbi_scratch *scratch)
{
	ಪೂರ್ಣಾಂಕ ret;

	/**
	 * Allocate per-domain and per-hart MPXY state data.
	 * The data type is "struct mpxy_state **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one() and
	 * domain_mpxy_state_setup(). Calculate needed size of memory space
	 * here.
	 */
	dmstate.state_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ mpxy_state *) * sbi_hart_ಎಣಿಕೆ();
	ret = sbi_domain_ರಿಜಿಸ್ಟರ್_state(&dmstate);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ಹಿಂತಿರುಗಿ sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_mpxy_get_shmem_ಗಾತ್ರ(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ mpxy_shmem_ಗಾತ್ರ;
}

ಪೂರ್ಣಾಂಕ sbi_mpxy_ಗಣ_shmem(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ shmem_phys_lo,
		       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ shmem_phys_hi,
		       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags)
{
	ರಚನೆ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	ಆದರೆ (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		ಹಿಂತಿರುಗಿ SBI_SUCCESS;
	}

	ಆದರೆ (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	ಆದರೆ (shmem_phys_lo & ~ಪುಟ_ಮುಖವಾಡ)
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

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
	ಆದರೆ (shmem_phys_hi)
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_ವಿಳಾಸ;

	ಆದರೆ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_ಗಾತ್ರ, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_ವಿಳಾಸ;

	/** Save the current shmem details in new shmem region */
	ಆದರೆ (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
		ret_buf = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *)(ulong)SHMEM_PHYS_ADDR(shmem_phys_hi,
								  shmem_phys_lo);
		sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ret_buf, mpxy_shmem_ಗಾತ್ರ);
		ret_buf[0] = cpu_ಗಮ್ಯ_lle(ms->shmem.shmem_addr_lo);
		ret_buf[1] = cpu_ಗಮ್ಯ_lle(ms->shmem.shmem_addr_hi);
		sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ret_buf, mpxy_shmem_ಗಾತ್ರ);
	}

	/** Setup the new shared memory */
	ms->shmem.shmem_addr_lo = shmem_phys_lo;
	ms->shmem.shmem_addr_hi = shmem_phys_hi;

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಪೂರ್ಣಾಂಕ sbi_mpxy_get_channel_ids(u32 ಆರಂಭ_ಸೂಚ್ಯಂಕ)
{
	ರಚನೆ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 ಉಳಿದ, returned, max_channelids;
	u32 ಗಂಟು_ಸೂಚ್ಯಂಕ = 0, ಗಂಟು_ret = 0;
	ರಚನೆ sbi_mpxy_channel *channel;
	u32 channels_ಎಣಿಕೆ = 0;
	u32 *shmem_ತಳ;
	ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();

	ಆದರೆ (!mpxy_shmem_enabled(ms))
		ಹಿಂತಿರುಗಿ SBI_ERR_NO_SHMEM;

	sbi_list_for_each_ನಮೂದು(channel, &mpxy_channel_list, ತಲೆ) {
		ಆದರೆ (mpxy_channel_visible(channel, dom))
			channels_ಎಣಿಕೆ += 1;
	}

	ಆದರೆ (ಆರಂಭ_ಸೂಚ್ಯಂಕ > channels_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	shmem_ತಳ = hart_shmem_ತಳ(ms);
	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hart_shmem_ತಳ(ms), mpxy_shmem_ಗಾತ್ರ);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_ಗಾತ್ರ / ಗಾತ್ರ(u32)) - 2;
	/* total remaining from the start index */
	ಉಳಿದ = channels_ಎಣಿಕೆ - ಆರಂಭ_ಸೂಚ್ಯಂಕ;
	/* how many can be returned */
	returned = (ಉಳಿದ > max_channelids)? max_channelids : ಉಳಿದ;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_ನಮೂದು(channel, &mpxy_channel_list, ತಲೆ) {
		ಆದರೆ (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			ಮುಂದುವರಿ;

		ಆದರೆ (ಗಂಟು_ಸೂಚ್ಯಂಕ >= ಆರಂಭ_ಸೂಚ್ಯಂಕ &&
			ಗಂಟು_ಸೂಚ್ಯಂಕ < (ಆರಂಭ_ಸೂಚ್ಯಂಕ + returned)) {
			shmem_ತಳ[2 + ಗಂಟು_ret] = cpu_ಗಮ್ಯ_le32(channel->channel_id);
			ಗಂಟು_ret += 1;
		}

		ಗಂಟು_ಸೂಚ್ಯಂಕ += 1;
	}

	/* final remaininig channel ids */
	ಉಳಿದ = channels_ಎಣಿಕೆ - (ಆರಂಭ_ಸೂಚ್ಯಂಕ + returned);

	shmem_ತಳ[0] = cpu_ಗಮ್ಯ_le32(ಉಳಿದ);
	shmem_ತಳ[1] = cpu_ಗಮ್ಯ_le32(returned);

	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hart_shmem_ತಳ(ms), mpxy_shmem_ಗಾತ್ರ);

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಪೂರ್ಣಾಂಕ sbi_mpxy_read_attrs(u32 channel_id, u32 ತಳ_attr_id, u32 attr_ಎಣಿಕೆ)
{
	ರಚನೆ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ಪೂರ್ಣಾಂಕ ret = SBI_SUCCESS;
	u32 *attr_ptr, ಕೊನೆ_id;
	ಶೂನ್ಯ *shmem_ತಳ;
	ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();

	ಆದರೆ (!mpxy_shmem_enabled(ms))
		ಹಿಂತಿರುಗಿ SBI_ERR_NO_SHMEM;

	ರಚನೆ sbi_mpxy_channel *channel = mpxy_ಕಂಡುಹಿಡಿ_channel(channel_id, dom);
	ಆದರೆ (!channel)
		ಹಿಂತಿರುಗಿ SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	ಆದರೆ (ತಳ_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		ತಳ_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_ಆರಂಭ)
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	ಆದರೆ (!attr_ಎಣಿಕೆ || (attr_ಎಣಿಕೆ > (mpxy_shmem_ಗಾತ್ರ / ATTR_ಗಾತ್ರ)))
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	shmem_ತಳ = hart_shmem_ತಳ(ms);
	ಕೊನೆ_id = ತಳ_attr_id + attr_ಎಣಿಕೆ - 1;

	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hart_shmem_ತಳ(ms), mpxy_shmem_ಗಾತ್ರ);

	/* Standard attributes range check */
	ಆದರೆ (mpxy_is_std_attr(ತಳ_attr_id)) {
		ಆದರೆ (ಕೊನೆ_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			ಹೋಗು ಹೊರಹೋಗು;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_ತಳ, &attr_ptr[ತಳ_attr_id],
				    attr_ಎಣಿಕೆ);
	} ಇಲ್ಲದಿದ್ದರೆ {
		/**
		 * Even if the message protocol driver does not provide
		 * read attribute callback, return bad range error instead
		 * of not supported to let client distinguish it from channel
		 * id not supported.
		 * Check the complate range supported for message protocol
		 * attributes. Actual supported attributes will be checked
		 * by the message protocol driver.
		 */
		ಆದರೆ (!channel->read_ಗುಣಲಕ್ಷಣಗಳು ||
				ಕೊನೆ_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_ಕೊನೆ) {
			ret = SBI_ERR_BAD_RANGE;
			ಹೋಗು ಹೊರಹೋಗು;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_ಗುಣಲಕ್ಷಣಗಳು(channel,
					       (u32 *)shmem_ತಳ,
					       ತಳ_attr_id, attr_ಎಣಿಕೆ);
	}
ಹೊರಹೋಗು:
	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hart_shmem_ತಳ(ms), mpxy_shmem_ಗಾತ್ರ);
	ಹಿಂತಿರುಗಿ ret;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mpxy_check_write_std_attr(ರಚನೆ sbi_mpxy_channel *channel,
				     u32 attr_id, u32 attr_val)
{
	ರಚನೆ sbi_mpxy_channel_attrs *attrs = &channel->attrs;
	ಪೂರ್ಣಾಂಕ ret = SBI_SUCCESS;

	ಆಯ್ಕೆ(attr_id) {
	ಸ್ಥಿತಿ SBI_MPXY_ATTR_MSI_CONTROL:
		ಆದರೆ (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		ಆದರೆ (attr_val == 1 &&
		    (attrs->msi_info.msi_addr_lo == (u32)INVALID_ADDR) &&
		    (attrs->msi_info.msi_addr_hi == (u32)INVALID_ADDR))
			ret = SBI_ERR_DENIED;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_MPXY_ATTR_MSI_ADDR_LO:
	ಸ್ಥಿತಿ SBI_MPXY_ATTR_MSI_ADDR_HI:
	ಸ್ಥಿತಿ SBI_MPXY_ATTR_MSI_ದತ್ತಾಂಶ:
		ret = SBI_SUCCESS;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		ಆದರೆ (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		/** All RO access attributes falls under default */
		ret = SBI_ERR_BAD_RANGE;
	};

	ಹಿಂತಿರುಗಿ ret;
}

/**
 * Write the attribute value
 */
ಸ್ಥಿರ ಶೂನ್ಯ mpxy_write_std_attr(ರಚನೆ sbi_mpxy_channel *channel, u32 attr_id,
				u32 attr_val)
{
	ರಚನೆ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ರಚನೆ sbi_mpxy_channel_attrs *attrs = &channel->attrs;

	ಆಯ್ಕೆ(attr_id) {
	ಸ್ಥಿತಿ SBI_MPXY_ATTR_MSI_CONTROL:
		ಆದರೆ (ms->msi_avail && attr_val <= 1)
			attrs->msi_control = attr_val;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_MPXY_ATTR_MSI_ADDR_LO:
		ಆದರೆ (ms->msi_avail)
			attrs->msi_info.msi_addr_lo = attr_val;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_MPXY_ATTR_MSI_ADDR_HI:
		ಆದರೆ (ms->msi_avail)
			attrs->msi_info.msi_addr_hi = attr_val;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_MPXY_ATTR_MSI_ದತ್ತಾಂಶ:
		ಆದರೆ (ms->msi_avail)
			attrs->msi_info.msi_ದತ್ತಾಂಶ = attr_val;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		ಆದರೆ (channel->switch_eventsstate && attr_val <= 1) {
			attrs->eventsstate_ctrl = attr_val;
			 /* message protocol callback to enable/disable
			 * events state reporting. */
			channel->switch_eventsstate(attr_val);
		}

		ನಿಲ್ಲಿಸು;
	};
}

ಪೂರ್ಣಾಂಕ sbi_mpxy_write_attrs(u32 channel_id, u32 ತಳ_attr_id, u32 attr_ಎಣಿಕೆ)
{
	ರಚನೆ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, ಕೊನೆ_id, attr_val;
	ರಚನೆ sbi_mpxy_channel *channel;
	ಪೂರ್ಣಾಂಕ ret, mem_idx;
	ಶೂನ್ಯ *shmem_ತಳ;
	ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();

	ಆದರೆ (!mpxy_shmem_enabled(ms))
		ಹಿಂತಿರುಗಿ SBI_ERR_NO_SHMEM;

	channel = mpxy_ಕಂಡುಹಿಡಿ_channel(channel_id, dom);
	ಆದರೆ (!channel)
		ಹಿಂತಿರುಗಿ SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	ಆದರೆ (ತಳ_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		ತಳ_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_ಆರಂಭ)
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	ಆದರೆ (!attr_ಎಣಿಕೆ || (attr_ಎಣಿಕೆ > (mpxy_shmem_ಗಾತ್ರ / ATTR_ಗಾತ್ರ)))
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	shmem_ತಳ = hart_shmem_ತಳ(ms);
	ಕೊನೆ_id = ತಳ_attr_id + attr_ಎಣಿಕೆ - 1;

	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ, mpxy_shmem_ಗಾತ್ರ);

	mem_ptr = (u32 *)shmem_ತಳ;

	ಆದರೆ (mpxy_is_std_attr(ತಳ_attr_id)) {
		ಆದರೆ (ಕೊನೆ_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			ಹೋಗು ಹೊರಹೋಗು;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		ಗಾಗಿ (attr_id = ತಳ_attr_id; attr_id <= ಕೊನೆ_id; attr_id++) {
			attr_val = le32_ಗಮ್ಯ_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			ಆದರೆ (ret)
				ಹೋಗು ಹೊರಹೋಗು;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		ಗಾಗಿ (attr_id = ತಳ_attr_id; attr_id <= ಕೊನೆ_id; attr_id++) {
			attr_val = le32_ಗಮ್ಯ_cpu(mem_ptr[mem_idx++]);
			mpxy_write_std_attr(channel, attr_id, attr_val);
		}
	} ಇಲ್ಲದಿದ್ದರೆ {/**
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
		ಆದರೆ (!channel->write_ಗುಣಲಕ್ಷಣಗಳು ||
				ಕೊನೆ_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_ಕೊನೆ) {
			ret = SBI_ERR_BAD_RANGE;
			ಹೋಗು ಹೊರಹೋಗು;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_ಗುಣಲಕ್ಷಣಗಳು(channel,
					       (u32 *)shmem_ತಳ,
					       ತಳ_attr_id, attr_ಎಣಿಕೆ);
	}
ಹೊರಹೋಗು:
	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ, mpxy_shmem_ಗಾತ್ರ);
	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ sbi_mpxy_send_ಸಂದೇಶ(u32 channel_id, u8 ಸಂದೇಶ_id,
			  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸಂದೇಶ_ದತ್ತಾಂಶ_len,
			  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *resp_ದತ್ತಾಂಶ_len)
{
	ರಚನೆ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ರಚನೆ sbi_mpxy_channel *channel;
	ಶೂನ್ಯ *shmem_ತಳ, *resp_buf;
	u32 resp_bufsize;
	ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಪೂರ್ಣಾಂಕ ret;

	ಆದರೆ (!mpxy_shmem_enabled(ms))
		ಹಿಂತಿರುಗಿ SBI_ERR_NO_SHMEM;

	channel = mpxy_ಕಂಡುಹಿಡಿ_channel(channel_id, dom);
	ಆದರೆ (!channel)
		ಹಿಂತಿರುಗಿ SBI_ERR_NOT_SUPPORTED;

	ಆದರೆ (resp_ದತ್ತಾಂಶ_len && !channel->send_ಸಂದೇಶ_with_response)
		ಹಿಂತಿರುಗಿ SBI_ERR_NOT_SUPPORTED;

	ಆದರೆ (!resp_ದತ್ತಾಂಶ_len && !channel->send_ಸಂದೇಶ_without_response)
		ಹಿಂತಿರುಗಿ SBI_ERR_NOT_SUPPORTED;

	ಆದರೆ (ಸಂದೇಶ_ದತ್ತಾಂಶ_len > mpxy_shmem_ಗಾತ್ರ ||
		ಸಂದೇಶ_ದತ್ತಾಂಶ_len > channel->attrs.ಸಂದೇಶ_ದತ್ತಾಂಶ_maxlen)
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	shmem_ತಳ = hart_shmem_ತಳ(ms);
	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ, mpxy_shmem_ಗಾತ್ರ);

	ಆದರೆ (resp_ದತ್ತಾಂಶ_len) {
		resp_buf = shmem_ತಳ;
		resp_bufsize = mpxy_shmem_ಗಾತ್ರ;
		ret = channel->send_ಸಂದೇಶ_with_response(channel, ಸಂದೇಶ_id,
							  shmem_ತಳ,
							  ಸಂದೇಶ_ದತ್ತಾಂಶ_len,
							  resp_buf,
							  resp_bufsize,
							  resp_ದತ್ತಾಂಶ_len);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ret = channel->send_ಸಂದೇಶ_without_response(channel, ಸಂದೇಶ_id,
							     shmem_ತಳ,
							     ಸಂದೇಶ_ದತ್ತಾಂಶ_len);
	}

	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ, mpxy_shmem_ಗಾತ್ರ);

	ಆದರೆ (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		ಹಿಂತಿರುಗಿ ret;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;

	ಆದರೆ (resp_ದತ್ತಾಂಶ_len &&
	    (*resp_ದತ್ತಾಂಶ_len > mpxy_shmem_ಗಾತ್ರ ||
	     *resp_ದತ್ತಾಂಶ_len > channel->attrs.ಸಂದೇಶ_ದತ್ತಾಂಶ_maxlen))
		ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಪೂರ್ಣಾಂಕ sbi_mpxy_get_notification_events(u32 channel_id, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *events_len)
{
	ರಚನೆ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ರಚನೆ sbi_mpxy_channel *channel;
	ಶೂನ್ಯ *eventsbuf, *shmem_ತಳ;
	ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಪೂರ್ಣಾಂಕ ret;

	ಆದರೆ (!mpxy_shmem_enabled(ms))
		ಹಿಂತಿರುಗಿ SBI_ERR_NO_SHMEM;

	channel = mpxy_ಕಂಡುಹಿಡಿ_channel(channel_id, dom);
	ಆದರೆ (!channel || !channel->get_notification_events)
		ಹಿಂತಿರುಗಿ SBI_ERR_NOT_SUPPORTED;

	shmem_ತಳ = hart_shmem_ತಳ(ms);
	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ, mpxy_shmem_ಗಾತ್ರ);
	eventsbuf = shmem_ತಳ;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_ಗಾತ್ರ,
					       events_len);
	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ, mpxy_shmem_ಗಾತ್ರ);

	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ಆದರೆ (*events_len > (mpxy_shmem_ಗಾತ್ರ - 16))
		ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}
