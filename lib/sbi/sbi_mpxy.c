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
ಸ್ಥಿರ unsigned long mpxy_shmem_size = PAGE_SIZE;

/** List of MPXY proxy channels */
ಸ್ಥಿರ SBI_LIST_HEAD(mpxy_channel_list);

/** Invalid Physical Address(all bits 1) */
#define INVALID_ADDR		(-1UL)

/** MPXY Attribute size in bytes */
#define ATTR_SIZE		(4)

/** Channel Capability - MSI */
#define CAP_MSI_POS		0
#define CAP_MSI_MASK		(1U << CAP_MSI_POS)

/** Channel Capability - SSE */
#define CAP_SSE_POS		1
#define CAP_SSE_MASK		(1U << CAP_SSE_POS)

/** Channel Capability - Events State */
#define CAP_EVENTSSTATE_POS	2
#define CAP_EVENTSSTATE_MASK	(1U << CAP_EVENTSSTATE_POS)

/** Channel Capability - Send Message With Response function support */
#define CAP_SEND_ಸಂದೇಶ_WITH_RESP_POS	3
#define CAP_SEND_ಸಂದೇಶ_WITH_RESP_MASK	(1U << CAP_SEND_ಸಂದೇಶ_WITH_RESP_POS)

/** Channel Capability - Send Message Without Response function support */
#define CAP_SEND_ಸಂದೇಶ_WITHOUT_RESP_POS	4
#define CAP_SEND_ಸಂದೇಶ_WITHOUT_RESP_MASK	(1U << CAP_SEND_ಸಂದೇಶ_WITHOUT_RESP_POS)

/** Channel Capability - Get Notification function support */
#define CAP_GET_NOTIFICATIONS_POS	5
#define CAP_GET_NOTIFICATIONS_MASK	(1U << CAP_GET_NOTIFICATIONS_POS)

/** Helpers to enable/disable channel capability bits
 * _c: capability variable
 * _m: capability mask
 */
#define CAP_ENABLE(_c, _m)	ಸೇರಿಸು_FIELD(_c, _m, 1)
#define CAP_DISABLE(_c, _m)	ಸೇರಿಸು_FIELD(_c, _m, 0)
#define CAP_GET(_c, _m)		EXTRACT_FIELD(_c, _m)

#define SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
ಸಂರಚನಾ mpxy_shmem {
	unsigned long shmem_addr_lo;
	unsigned long shmem_addr_hi;
};

ಸಂರಚನಾ mpxy_state {
	/* MSI support in MPXY */
	bool msi_avail;
	/* SSE support in MPXY */
	bool sse_avail;
	/* MPXY Shared memory details */
	ಸಂರಚನಾ mpxy_shmem shmem;
};

ಸ್ಥಿರ ಸಂರಚನಾ mpxy_state *sbi_domain_get_mpxy_state(ಸಂರಚನಾ sbi_domain *dom,
						    u32 hartindex);

/** Macro to obtain the current hart's MPXY state pointer in current domain */
#define sbi_domain_mpxy_state_thishart_ptr()			\
	sbi_domain_get_mpxy_state(sbi_domain_thishart_ptr(),	\
				  current_hartindex())

/** Disable hart shared memory */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ sbi_mpxy_shmem_disable(ಸಂರಚನಾ mpxy_state *ms)
{
	ms->shmem.shmem_addr_lo = INVALID_ADDR;
	ms->shmem.shmem_addr_hi = INVALID_ADDR;
}

/** Check if shared memory is already setup on hart */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool mpxy_shmem_enabled(ಸಂರಚನಾ mpxy_state *ms)
{
	ಪ್ರತ್ಯಾಗಮ (ms->shmem.shmem_addr_lo == INVALID_ADDR
		&& ms->shmem.shmem_addr_hi == INVALID_ADDR) ?
		false : true;
}

/** Get hart shared memory base address */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *hart_shmem_base(ಸಂರಚನಾ mpxy_state *ms)
{
	ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)(unsigned long)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)				\
	_Static_assert(							\
		((offsetof(ಸಂರಚನಾ sbi_mpxy_channel_attrs, field)) /	\
		 ಪರಿಮಾಣ(u32)) == attr_offset,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_field_offset(ಸಂದೇಶ_proto_id, SBI_MPXY_ATTR_ಸಂದೇಶ_PROT_ID);
assert_field_offset(ಸಂದೇಶ_proto_version, SBI_MPXY_ATTR_ಸಂದೇಶ_PROT_VER);
assert_field_offset(ಸಂದೇಶ_data_maxlen, SBI_MPXY_ATTR_ಸಂದೇಶ_MAX_LEN);
assert_field_offset(ಸಂದೇಶ_send_timeout, SBI_MPXY_ATTR_ಸಂದೇಶ_SEND_TIMEOUT);
assert_field_offset(ಸಂದೇಶ_completion_timeout, SBI_MPXY_ATTR_ಸಂದೇಶ_COMPLETION_TIMEOUT);
assert_field_offset(capability, SBI_MPXY_ATTR_CHANNEL_CAPABILITY);
assert_field_offset(sse_event_id, SBI_MPXY_ATTR_SSE_EVENT_ID);
assert_field_offset(msi_control, SBI_MPXY_ATTR_MSI_CONTROL);
assert_field_offset(msi_info.msi_addr_lo, SBI_MPXY_ATTR_MSI_ADDR_LO);
assert_field_offset(msi_info.msi_addr_hi, SBI_MPXY_ATTR_MSI_ADDR_HI);
assert_field_offset(msi_info.msi_data, SBI_MPXY_ATTR_MSI_DATA);
assert_field_offset(eventsstate_ctrl, SBI_MPXY_ATTR_EVENTS_STATE_CONTROL);

/**
 * Check if the attribute is a standard attribute or
 * a message protocol specific attribute
 * attr_id[31] = 0 for standard
 * attr_id[31] = 1 for message protocol specific
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool mpxy_is_std_attr(u32 attr_id)
{
	ಪ್ರತ್ಯಾಗಮ (attr_id >> 31) ? false : true;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool mpxy_channel_visible(ಸಂರಚನಾ sbi_mpxy_channel *channel,
					ಸಂರಚನಾ sbi_domain *dom)
{
	ಪ್ರತ್ಯಾಗಮ channel->owner_domain == dom;
}

/** Find channel_id in registered channels list for this domain */
ಸ್ಥಿರ ಸಂರಚನಾ sbi_mpxy_channel *mpxy_find_channel(u32 channel_id,
					ಸಂರಚನಾ sbi_domain *dom)
{
	ಸಂರಚನಾ sbi_mpxy_channel *channel;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		ಯದಿ (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			ಪ್ರತ್ಯಾಗಮ channel;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/** Find channel_id in registered channels list */
ಸಂರಚನಾ sbi_mpxy_channel *sbi_mpxy_find_channel_any(u32 channel_id)
{
	ಸಂರಚನಾ sbi_mpxy_channel *channel;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		ಯದಿ (channel->channel_id == channel_id)
			ಪ್ರತ್ಯಾಗಮ channel;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/** Copy attributes word size */
ಸ್ಥಿರ ಶೂನ್ಯ mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 ಎಣಿಕೆ)
{
	ಪೂರ್ಣಾಙ್ಕ idx;
	ಪ್ರತಿ (idx = 0; idx < ಎಣಿಕೆ; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_available(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ sbi_list_empty(&mpxy_channel_list) ? false : true;
}

ಸ್ಥಿರ ಶೂನ್ಯ mpxy_std_attrs_init(ಸಂರಚನಾ sbi_mpxy_channel *channel)
{
	ಸಂರಚನಾ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_data = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	ಯದಿ (channel->send_message_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_ಸಂದೇಶ_WITH_RESP_MASK);

	ಯದಿ (channel->send_message_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_ಸಂದೇಶ_WITHOUT_RESP_MASK);

	ಯದಿ (channel->get_notification_events) {
		capability = CAP_ENABLE(capability, CAP_GET_NOTIFICATIONS_MASK);
		/**
		 * Check if MSI or SSE available for notification interrrupt.
		 * Priority given to MSI if both MSI and SSE are avaialble.
		 */
		ಯದಿ (ms->msi_avail)
			capability = CAP_ENABLE(capability, CAP_MSI_MASK);
		ಅನ್ಯಥಾ ಯದಿ (ms->sse_avail) {
			capability = CAP_ENABLE(capability, CAP_SSE_MASK);
			/* TODO: Assign SSE EVENT_ID for the channel */
		}

		/**
		 * switch_eventstate callback support means support for events
		 * state reporting supoprt. Enable events state reporting in
		 * channel capability.
		 */
		ಯದಿ (channel->switch_eventsstate)
			capability = CAP_ENABLE(capability, CAP_EVENTSSTATE_MASK);
	}

	channel->attrs.capability = capability;
}

/**
 * Register a channel with MPXY framework.
 * Called by message protocol drivers
 */
ಪೂರ್ಣಾಙ್ಕ sbi_mpxy_register_channel(ಸಂರಚನಾ sbi_mpxy_channel *channel)
{
	ಯದಿ (!channel)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (!channel->owner_domain)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (sbi_mpxy_find_channel_any(channel->channel_id))
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	ಯದಿ (mpxy_shmem_size < channel->attrs.ಸಂದೇಶ_data_maxlen) {
		mpxy_shmem_size = ROUNDUP(channel->attrs.ಸಂದೇಶ_data_maxlen,
					  PAGE_SIZE);
	}

	sbi_list_add_tail(&channel->head, &mpxy_channel_list);

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

/** Setup per domain MPXY state data */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ domain_mpxy_state_setup(ಸಂರಚನಾ sbi_domain *dom,
					ಸಂರಚನಾ sbi_domain_state *state,
					ಶೂನ್ಯ *state_ptr)
{
	ಸಂರಚನಾ mpxy_state **dom_hartindex_to_mpxy_state_table = state_ptr;
	ಸಂರಚನಾ mpxy_state *ms;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ms = sbi_zalloc(ಪರಿಮಾಣ(*ms));
		ಯದಿ (!ms)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

		/*
		 * TODO: Proper support for checking sse support from
		 * platform. Currently disable sse and use polling
		 */
		ms->msi_avail = !!sbi_irqchip_find_device_by_caps(SBI_IRQCHIP_CAPS_MSI, ರಿಕ್ತ);
		ms->sse_avail = false;

		sbi_mpxy_shmem_disable(ms);

		dom_hartindex_to_mpxy_state_table[i] = ms;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

/** Cleanup per domain MPXY state data */
ಸ್ಥಿರ ಶೂನ್ಯ domain_mpxy_state_cleanup(ಸಂರಚನಾ sbi_domain *dom,
					   ಸಂರಚನಾ sbi_domain_state *state,
					   ಶೂನ್ಯ *state_ptr)
{
	ಸಂರಚನಾ mpxy_state **dom_hartindex_to_mpxy_state_table = state_ptr;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts)
		sbi_free(dom_hartindex_to_mpxy_state_table[i]);
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_domain_state dmstate = {
	.state_setup = domain_mpxy_state_setup,
	.state_cleanup = domain_mpxy_state_cleanup,
};

/**
 * Get per-domain MPXY state pointer for a given domain and HART index
 * @param dom pointer to domain
 * @param hartindex the HART index
 *
 * @return per-domain MPXY state pointer for given HART index
 */
ಸ್ಥಿರ ಸಂರಚನಾ mpxy_state *sbi_domain_get_mpxy_state(ಸಂರಚನಾ sbi_domain *dom,
						    u32 hartindex)
{
	ಸಂರಚನಾ mpxy_state **dom_hartindex_to_mpxy_state_table;

	dom_hartindex_to_mpxy_state_table = sbi_domain_state_ptr(dom, &dmstate);
	ಯದಿ (!dom_hartindex_to_mpxy_state_table ||
	    !sbi_hartindex_valid(hartindex))
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಪ್ರತ್ಯಾಗಮ dom_hartindex_to_mpxy_state_table[hartindex];
}

ಪೂರ್ಣಾಙ್ಕ sbi_mpxy_init(ಸಂರಚನಾ sbi_scratch *scratch)
{
	ಪೂರ್ಣಾಙ್ಕ ret;

	/**
	 * Allocate per-domain and per-hart MPXY state data.
	 * The data type is "struct mpxy_state **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one() and
	 * domain_mpxy_state_setup(). Calculate needed size of memory space
	 * here.
	 */
	dmstate.state_size = ಪರಿಮಾಣ(ಸಂರಚನಾ mpxy_state *) * sbi_hart_ಎಣಿಕೆ();
	ret = sbi_domain_register_state(&dmstate);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ಪ್ರತ್ಯಾಗಮ sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

unsigned long sbi_mpxy_get_shmem_size(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ mpxy_shmem_size;
}

ಪೂರ್ಣಾಙ್ಕ sbi_mpxy_set_shmem(unsigned long shmem_phys_lo,
		       unsigned long shmem_phys_hi,
		       unsigned long flags)
{
	ಸಂರಚನಾ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	unsigned long *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	ಯದಿ (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
	}

	ಯದಿ (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	ಯದಿ (shmem_phys_lo & ~PAGE_MASK)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;

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
	ಯದಿ (shmem_phys_hi)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_ADDRESS;

	ಯದಿ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_size, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_ADDRESS;

	/** Save the current shmem details in new shmem region */
	ಯದಿ (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
		ret_buf = (unsigned long *)(ulong)SHMEM_PHYS_ADDR(shmem_phys_hi,
								  shmem_phys_lo);
		sbi_hart_protection_temp_map_range((unsigned long)ret_buf, mpxy_shmem_size);
		ret_buf[0] = cpu_to_lle(ms->shmem.shmem_addr_lo);
		ret_buf[1] = cpu_to_lle(ms->shmem.shmem_addr_hi);
		sbi_hart_protection_temp_unmap_range((unsigned long)ret_buf, mpxy_shmem_size);
	}

	/** Setup the new shared memory */
	ms->shmem.shmem_addr_lo = shmem_phys_lo;
	ms->shmem.shmem_addr_hi = shmem_phys_hi;

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಪೂರ್ಣಾಙ್ಕ sbi_mpxy_get_channel_ids(u32 start_index)
{
	ಸಂರಚನಾ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 remaining, returned, max_channelids;
	u32 ಗಂಟು_index = 0, ಗಂಟು_ret = 0;
	ಸಂರಚನಾ sbi_mpxy_channel *channel;
	u32 channels_ಎಣಿಕೆ = 0;
	u32 *shmem_base;
	ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();

	ಯದಿ (!mpxy_shmem_enabled(ms))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NO_SHMEM;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		ಯದಿ (mpxy_channel_visible(channel, dom))
			channels_ಎಣಿಕೆ += 1;
	}

	ಯದಿ (start_index > channels_ಎಣಿಕೆ)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_temp_map_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_size);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_size / ಪರಿಮಾಣ(u32)) - 2;
	/* total remaining from the start index */
	remaining = channels_ಎಣಿಕೆ - start_index;
	/* how many can be returned */
	returned = (remaining > max_channelids)? max_channelids : remaining;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		ಯದಿ (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			ಅನುವರ್ತ;

		ಯದಿ (ಗಂಟು_index >= start_index &&
			ಗಂಟು_index < (start_index + returned)) {
			shmem_base[2 + ಗಂಟು_ret] = cpu_to_le32(channel->channel_id);
			ಗಂಟು_ret += 1;
		}

		ಗಂಟು_index += 1;
	}

	/* final remaininig channel ids */
	remaining = channels_ಎಣಿಕೆ - (start_index + returned);

	shmem_base[0] = cpu_to_le32(remaining);
	shmem_base[1] = cpu_to_le32(returned);

	sbi_hart_protection_temp_unmap_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_size);

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಪೂರ್ಣಾಙ್ಕ sbi_mpxy_read_attrs(u32 channel_id, u32 base_attr_id, u32 attr_ಎಣಿಕೆ)
{
	ಸಂರಚನಾ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ಪೂರ್ಣಾಙ್ಕ ret = SBI_SUCCESS;
	u32 *attr_ptr, end_id;
	ಶೂನ್ಯ *shmem_base;
	ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();

	ಯದಿ (!mpxy_shmem_enabled(ms))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NO_SHMEM;

	ಸಂರಚನಾ sbi_mpxy_channel *channel = mpxy_find_channel(channel_id, dom);
	ಯದಿ (!channel)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	ಯದಿ (base_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		base_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	ಯದಿ (!attr_ಎಣಿಕೆ || (attr_ಎಣಿಕೆ > (mpxy_shmem_size / ATTR_SIZE)))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	end_id = base_attr_id + attr_ಎಣಿಕೆ - 1;

	sbi_hart_protection_temp_map_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_size);

	/* Standard attributes range check */
	ಯದಿ (mpxy_is_std_attr(base_attr_id)) {
		ಯದಿ (end_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			ಗಚ್ಛ out;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_base, &attr_ptr[base_attr_id],
				    attr_ಎಣಿಕೆ);
	} ಅನ್ಯಥಾ {
		/**
		 * Even if the message protocol driver does not provide
		 * read attribute callback, return bad range error instead
		 * of not supported to let client distinguish it from channel
		 * id not supported.
		 * Check the complate range supported for message protocol
		 * attributes. Actual supported attributes will be checked
		 * by the message protocol driver.
		 */
		ಯದಿ (!channel->read_attributes ||
				end_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_END) {
			ret = SBI_ERR_BAD_RANGE;
			ಗಚ್ಛ out;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_attributes(channel,
					       (u32 *)shmem_base,
					       base_attr_id, attr_ಎಣಿಕೆ);
	}
out:
	sbi_hart_protection_temp_unmap_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_size);
	ಪ್ರತ್ಯಾಗಮ ret;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mpxy_check_write_std_attr(ಸಂರಚನಾ sbi_mpxy_channel *channel,
				     u32 attr_id, u32 attr_val)
{
	ಸಂರಚನಾ sbi_mpxy_channel_attrs *attrs = &channel->attrs;
	ಪೂರ್ಣಾಙ್ಕ ret = SBI_SUCCESS;

	ವಿಕಲ್ಪ(attr_id) {
	ಪಕ್ಷ SBI_MPXY_ATTR_MSI_CONTROL:
		ಯದಿ (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		ಯದಿ (attr_val == 1 &&
		    (attrs->msi_info.msi_addr_lo == (u32)INVALID_ADDR) &&
		    (attrs->msi_info.msi_addr_hi == (u32)INVALID_ADDR))
			ret = SBI_ERR_DENIED;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_MPXY_ATTR_MSI_ADDR_LO:
	ಪಕ್ಷ SBI_MPXY_ATTR_MSI_ADDR_HI:
	ಪಕ್ಷ SBI_MPXY_ATTR_MSI_DATA:
		ret = SBI_SUCCESS;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		ಯದಿ (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		/** All RO access attributes falls under default */
		ret = SBI_ERR_BAD_RANGE;
	};

	ಪ್ರತ್ಯಾಗಮ ret;
}

/**
 * Write the attribute value
 */
ಸ್ಥಿರ ಶೂನ್ಯ mpxy_write_std_attr(ಸಂರಚನಾ sbi_mpxy_channel *channel, u32 attr_id,
				u32 attr_val)
{
	ಸಂರಚನಾ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ಸಂರಚನಾ sbi_mpxy_channel_attrs *attrs = &channel->attrs;

	ವಿಕಲ್ಪ(attr_id) {
	ಪಕ್ಷ SBI_MPXY_ATTR_MSI_CONTROL:
		ಯದಿ (ms->msi_avail && attr_val <= 1)
			attrs->msi_control = attr_val;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_MPXY_ATTR_MSI_ADDR_LO:
		ಯದಿ (ms->msi_avail)
			attrs->msi_info.msi_addr_lo = attr_val;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_MPXY_ATTR_MSI_ADDR_HI:
		ಯದಿ (ms->msi_avail)
			attrs->msi_info.msi_addr_hi = attr_val;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_MPXY_ATTR_MSI_DATA:
		ಯದಿ (ms->msi_avail)
			attrs->msi_info.msi_data = attr_val;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		ಯದಿ (channel->switch_eventsstate && attr_val <= 1) {
			attrs->eventsstate_ctrl = attr_val;
			 /* message protocol callback to enable/disable
			 * events state reporting. */
			channel->switch_eventsstate(attr_val);
		}

		ಭಙ್ಗ;
	};
}

ಪೂರ್ಣಾಙ್ಕ sbi_mpxy_write_attrs(u32 channel_id, u32 base_attr_id, u32 attr_ಎಣಿಕೆ)
{
	ಸಂರಚನಾ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, end_id, attr_val;
	ಸಂರಚನಾ sbi_mpxy_channel *channel;
	ಪೂರ್ಣಾಙ್ಕ ret, mem_idx;
	ಶೂನ್ಯ *shmem_base;
	ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();

	ಯದಿ (!mpxy_shmem_enabled(ms))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	ಯದಿ (!channel)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	ಯದಿ (base_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		base_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	ಯದಿ (!attr_ಎಣಿಕೆ || (attr_ಎಣಿಕೆ > (mpxy_shmem_size / ATTR_SIZE)))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	end_id = base_attr_id + attr_ಎಣಿಕೆ - 1;

	sbi_hart_protection_temp_map_range((unsigned long)shmem_base, mpxy_shmem_size);

	mem_ptr = (u32 *)shmem_base;

	ಯದಿ (mpxy_is_std_attr(base_attr_id)) {
		ಯದಿ (end_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			ಗಚ್ಛ out;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		ಪ್ರತಿ (attr_id = base_attr_id; attr_id <= end_id; attr_id++) {
			attr_val = le32_to_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			ಯದಿ (ret)
				ಗಚ್ಛ out;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		ಪ್ರತಿ (attr_id = base_attr_id; attr_id <= end_id; attr_id++) {
			attr_val = le32_to_cpu(mem_ptr[mem_idx++]);
			mpxy_write_std_attr(channel, attr_id, attr_val);
		}
	} ಅನ್ಯಥಾ {/**
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
		ಯದಿ (!channel->write_attributes ||
				end_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_END) {
			ret = SBI_ERR_BAD_RANGE;
			ಗಚ್ಛ out;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_attributes(channel,
					       (u32 *)shmem_base,
					       base_attr_id, attr_ಎಣಿಕೆ);
	}
out:
	sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base, mpxy_shmem_size);
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ sbi_mpxy_send_message(u32 channel_id, u8 ಸಂದೇಶ_id,
			  unsigned long ಸಂದೇಶ_data_len,
			  unsigned long *resp_data_len)
{
	ಸಂರಚನಾ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ಸಂರಚನಾ sbi_mpxy_channel *channel;
	ಶೂನ್ಯ *shmem_base, *resp_buf;
	u32 resp_bufsize;
	ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಪೂರ್ಣಾಙ್ಕ ret;

	ಯದಿ (!mpxy_shmem_enabled(ms))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	ಯದಿ (!channel)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NOT_SUPPORTED;

	ಯದಿ (resp_data_len && !channel->send_message_with_response)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NOT_SUPPORTED;

	ಯದಿ (!resp_data_len && !channel->send_message_without_response)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NOT_SUPPORTED;

	ಯದಿ (ಸಂದೇಶ_data_len > mpxy_shmem_size ||
		ಸಂದೇಶ_data_len > channel->attrs.ಸಂದೇಶ_data_maxlen)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base, mpxy_shmem_size);

	ಯದಿ (resp_data_len) {
		resp_buf = shmem_base;
		resp_bufsize = mpxy_shmem_size;
		ret = channel->send_message_with_response(channel, ಸಂದೇಶ_id,
							  shmem_base,
							  ಸಂದೇಶ_data_len,
							  resp_buf,
							  resp_bufsize,
							  resp_data_len);
	} ಅನ್ಯಥಾ {
		ret = channel->send_message_without_response(channel, ಸಂದೇಶ_id,
							     shmem_base,
							     ಸಂದೇಶ_data_len);
	}

	sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base, mpxy_shmem_size);

	ಯದಿ (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		ಪ್ರತ್ಯಾಗಮ ret;
	ಅನ್ಯಥಾ ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;

	ಯದಿ (resp_data_len &&
	    (*resp_data_len > mpxy_shmem_size ||
	     *resp_data_len > channel->attrs.ಸಂದೇಶ_data_maxlen))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಪೂರ್ಣಾಙ್ಕ sbi_mpxy_get_notification_events(u32 channel_id, unsigned long *events_len)
{
	ಸಂರಚನಾ mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ಸಂರಚನಾ sbi_mpxy_channel *channel;
	ಶೂನ್ಯ *eventsbuf, *shmem_base;
	ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಪೂರ್ಣಾಙ್ಕ ret;

	ಯದಿ (!mpxy_shmem_enabled(ms))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	ಯದಿ (!channel || !channel->get_notification_events)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NOT_SUPPORTED;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base, mpxy_shmem_size);
	eventsbuf = shmem_base;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_size,
					       events_len);
	sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base, mpxy_shmem_size);

	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ಯದಿ (*events_len > (mpxy_shmem_size - 16))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}
