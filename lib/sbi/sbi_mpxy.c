/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_hart_protection.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_irqchip.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_mpxy.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_byteorder.h>

/** Shared memory size across all harts */
സ്ഥിതം unsigned long mpxy_shmem_size = PAGE_SIZE;

/** List of MPXY proxy channels */
സ്ഥിതം SBI_LIST_HEAD(mpxy_channel_list);

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
#define CAP_SEND_സന്ദേശം_WITH_RESP_POS	3
#define CAP_SEND_സന്ദേശം_WITH_RESP_MASK	(1U << CAP_SEND_സന്ദേശം_WITH_RESP_POS)

/** Channel Capability - Send Message Without Response function support */
#define CAP_SEND_സന്ദേശം_WITHOUT_RESP_POS	4
#define CAP_SEND_സന്ദേശം_WITHOUT_RESP_MASK	(1U << CAP_SEND_സന്ദേശം_WITHOUT_RESP_POS)

/** Channel Capability - Get Notification function support */
#define CAP_GET_NOTIFICATIONS_POS	5
#define CAP_GET_NOTIFICATIONS_MASK	(1U << CAP_GET_NOTIFICATIONS_POS)

/** Helpers to enable/disable channel capability bits
 * _c: capability variable
 * _m: capability mask
 */
#define CAP_ENABLE(_c, _m)	തിരുകുക_FIELD(_c, _m, 1)
#define CAP_DISABLE(_c, _m)	തിരുകുക_FIELD(_c, _m, 0)
#define CAP_GET(_c, _m)		EXTRACT_FIELD(_c, _m)

#define SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
ഘടന mpxy_shmem {
	unsigned long shmem_addr_lo;
	unsigned long shmem_addr_hi;
};

ഘടന mpxy_state {
	/* MSI support in MPXY */
	bool msi_avail;
	/* SSE support in MPXY */
	bool sse_avail;
	/* MPXY Shared memory details */
	ഘടന mpxy_shmem shmem;
};

സ്ഥിതം ഘടന mpxy_state *sbi_domain_get_mpxy_state(ഘടന sbi_domain *dom,
						    u32 hartindex);

/** Macro to obtain the current hart's MPXY state pointer in current domain */
#define sbi_domain_mpxy_state_thishart_ptr()			\
	sbi_domain_get_mpxy_state(sbi_domain_thishart_ptr(),	\
				  current_hartindex())

/** Disable hart shared memory */
സ്ഥിതം വരിയിൽ ശൂന്യം sbi_mpxy_shmem_disable(ഘടന mpxy_state *ms)
{
	ms->shmem.shmem_addr_lo = INVALID_ADDR;
	ms->shmem.shmem_addr_hi = INVALID_ADDR;
}

/** Check if shared memory is already setup on hart */
സ്ഥിതം വരിയിൽ bool mpxy_shmem_enabled(ഘടന mpxy_state *ms)
{
	തിരികെ (ms->shmem.shmem_addr_lo == INVALID_ADDR
		&& ms->shmem.shmem_addr_hi == INVALID_ADDR) ?
		false : true;
}

/** Get hart shared memory base address */
സ്ഥിതം വരിയിൽ ശൂന്യം *hart_shmem_base(ഘടന mpxy_state *ms)
{
	തിരികെ (ശൂന്യം *)(unsigned long)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)				\
	_Static_assert(							\
		((offsetof(ഘടന sbi_mpxy_channel_attrs, field)) /	\
		 വലുപ്പം(u32)) == attr_offset,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_field_offset(സന്ദേശം_proto_id, SBI_MPXY_ATTR_സന്ദേശം_PROT_ID);
assert_field_offset(സന്ദേശം_proto_version, SBI_MPXY_ATTR_സന്ദേശം_PROT_VER);
assert_field_offset(സന്ദേശം_data_maxlen, SBI_MPXY_ATTR_സന്ദേശം_MAX_LEN);
assert_field_offset(സന്ദേശം_send_timeout, SBI_MPXY_ATTR_സന്ദേശം_SEND_TIMEOUT);
assert_field_offset(സന്ദേശം_completion_timeout, SBI_MPXY_ATTR_സന്ദേശം_COMPLETION_TIMEOUT);
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
സ്ഥിതം വരിയിൽ bool mpxy_is_std_attr(u32 attr_id)
{
	തിരികെ (attr_id >> 31) ? false : true;
}

സ്ഥിതം വരിയിൽ bool mpxy_channel_visible(ഘടന sbi_mpxy_channel *channel,
					ഘടന sbi_domain *dom)
{
	തിരികെ channel->owner_domain == dom;
}

/** Find channel_id in registered channels list for this domain */
സ്ഥിതം ഘടന sbi_mpxy_channel *mpxy_find_channel(u32 channel_id,
					ഘടന sbi_domain *dom)
{
	ഘടന sbi_mpxy_channel *channel;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		എങ്കിൽ (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			തിരികെ channel;
	}

	തിരികെ ശൂന്യ;
}

/** Find channel_id in registered channels list */
ഘടന sbi_mpxy_channel *sbi_mpxy_find_channel_any(u32 channel_id)
{
	ഘടന sbi_mpxy_channel *channel;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		എങ്കിൽ (channel->channel_id == channel_id)
			തിരികെ channel;
	}

	തിരികെ ശൂന്യ;
}

/** Copy attributes word size */
സ്ഥിതം ശൂന്യം mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 എണ്ണം)
{
	പൂർണ്ണസംഖ്യ idx;
	വേണ്ടി (idx = 0; idx < എണ്ണം; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_available(ശൂന്യം)
{
	തിരികെ sbi_list_empty(&mpxy_channel_list) ? false : true;
}

സ്ഥിതം ശൂന്യം mpxy_std_attrs_init(ഘടന sbi_mpxy_channel *channel)
{
	ഘടന mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_data = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	എങ്കിൽ (channel->send_message_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_സന്ദേശം_WITH_RESP_MASK);

	എങ്കിൽ (channel->send_message_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_സന്ദേശം_WITHOUT_RESP_MASK);

	എങ്കിൽ (channel->get_notification_events) {
		capability = CAP_ENABLE(capability, CAP_GET_NOTIFICATIONS_MASK);
		/**
		 * Check if MSI or SSE available for notification interrrupt.
		 * Priority given to MSI if both MSI and SSE are avaialble.
		 */
		എങ്കിൽ (ms->msi_avail)
			capability = CAP_ENABLE(capability, CAP_MSI_MASK);
		അല്ലെങ്കിൽ എങ്കിൽ (ms->sse_avail) {
			capability = CAP_ENABLE(capability, CAP_SSE_MASK);
			/* TODO: Assign SSE EVENT_ID for the channel */
		}

		/**
		 * switch_eventstate callback support means support for events
		 * state reporting supoprt. Enable events state reporting in
		 * channel capability.
		 */
		എങ്കിൽ (channel->switch_eventsstate)
			capability = CAP_ENABLE(capability, CAP_EVENTSSTATE_MASK);
	}

	channel->attrs.capability = capability;
}

/**
 * Register a channel with MPXY framework.
 * Called by message protocol drivers
 */
പൂർണ്ണസംഖ്യ sbi_mpxy_register_channel(ഘടന sbi_mpxy_channel *channel)
{
	എങ്കിൽ (!channel)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (!channel->owner_domain)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (sbi_mpxy_find_channel_any(channel->channel_id))
		തിരികെ SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	എങ്കിൽ (mpxy_shmem_size < channel->attrs.സന്ദേശം_data_maxlen) {
		mpxy_shmem_size = ROUNDUP(channel->attrs.സന്ദേശം_data_maxlen,
					  PAGE_SIZE);
	}

	sbi_list_add_tail(&channel->head, &mpxy_channel_list);

	തിരികെ SBI_OK;
}

/** Setup per domain MPXY state data */
സ്ഥിതം പൂർണ്ണസംഖ്യ domain_mpxy_state_setup(ഘടന sbi_domain *dom,
					ഘടന sbi_domain_state *state,
					ശൂന്യം *state_ptr)
{
	ഘടന mpxy_state **dom_hartindex_to_mpxy_state_table = state_ptr;
	ഘടന mpxy_state *ms;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ms = sbi_zalloc(വലുപ്പം(*ms));
		എങ്കിൽ (!ms)
			തിരികെ SBI_ENOMEM;

		/*
		 * TODO: Proper support for checking sse support from
		 * platform. Currently disable sse and use polling
		 */
		ms->msi_avail = !!sbi_irqchip_find_device_by_caps(SBI_IRQCHIP_CAPS_MSI, ശൂന്യ);
		ms->sse_avail = false;

		sbi_mpxy_shmem_disable(ms);

		dom_hartindex_to_mpxy_state_table[i] = ms;
	}

	തിരികെ 0;
}

/** Cleanup per domain MPXY state data */
സ്ഥിതം ശൂന്യം domain_mpxy_state_cleanup(ഘടന sbi_domain *dom,
					   ഘടന sbi_domain_state *state,
					   ശൂന്യം *state_ptr)
{
	ഘടന mpxy_state **dom_hartindex_to_mpxy_state_table = state_ptr;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts)
		sbi_free(dom_hartindex_to_mpxy_state_table[i]);
}

സ്ഥിതം ഘടന sbi_domain_state dmstate = {
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
സ്ഥിതം ഘടന mpxy_state *sbi_domain_get_mpxy_state(ഘടന sbi_domain *dom,
						    u32 hartindex)
{
	ഘടന mpxy_state **dom_hartindex_to_mpxy_state_table;

	dom_hartindex_to_mpxy_state_table = sbi_domain_state_ptr(dom, &dmstate);
	എങ്കിൽ (!dom_hartindex_to_mpxy_state_table ||
	    !sbi_hartindex_valid(hartindex))
		തിരികെ ശൂന്യ;

	തിരികെ dom_hartindex_to_mpxy_state_table[hartindex];
}

പൂർണ്ണസംഖ്യ sbi_mpxy_init(ഘടന sbi_scratch *scratch)
{
	പൂർണ്ണസംഖ്യ ret;

	/**
	 * Allocate per-domain and per-hart MPXY state data.
	 * The data type is "struct mpxy_state **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one() and
	 * domain_mpxy_state_setup(). Calculate needed size of memory space
	 * here.
	 */
	dmstate.state_size = വലുപ്പം(ഘടന mpxy_state *) * sbi_hart_എണ്ണം();
	ret = sbi_domain_register_state(&dmstate);
	എങ്കിൽ (ret)
		തിരികെ ret;

	തിരികെ sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

unsigned long sbi_mpxy_get_shmem_size(ശൂന്യം)
{
	തിരികെ mpxy_shmem_size;
}

പൂർണ്ണസംഖ്യ sbi_mpxy_set_shmem(unsigned long shmem_phys_lo,
		       unsigned long shmem_phys_hi,
		       unsigned long flags)
{
	ഘടന mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	unsigned long *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	എങ്കിൽ (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		തിരികെ SBI_SUCCESS;
	}

	എങ്കിൽ (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		തിരികെ SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	എങ്കിൽ (shmem_phys_lo & ~PAGE_MASK)
		തിരികെ SBI_ERR_INVALID_PARAM;

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
	എങ്കിൽ (shmem_phys_hi)
		തിരികെ SBI_ERR_INVALID_ADDRESS;

	എങ്കിൽ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_size, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		തിരികെ SBI_ERR_INVALID_ADDRESS;

	/** Save the current shmem details in new shmem region */
	എങ്കിൽ (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
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

	തിരികെ SBI_SUCCESS;
}

പൂർണ്ണസംഖ്യ sbi_mpxy_get_channel_ids(u32 start_index)
{
	ഘടന mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 remaining, returned, max_channelids;
	u32 കണ്ണി_index = 0, കണ്ണി_ret = 0;
	ഘടന sbi_mpxy_channel *channel;
	u32 channels_എണ്ണം = 0;
	u32 *shmem_base;
	ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();

	എങ്കിൽ (!mpxy_shmem_enabled(ms))
		തിരികെ SBI_ERR_NO_SHMEM;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		എങ്കിൽ (mpxy_channel_visible(channel, dom))
			channels_എണ്ണം += 1;
	}

	എങ്കിൽ (start_index > channels_എണ്ണം)
		തിരികെ SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_temp_map_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_size);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_size / വലുപ്പം(u32)) - 2;
	/* total remaining from the start index */
	remaining = channels_എണ്ണം - start_index;
	/* how many can be returned */
	returned = (remaining > max_channelids)? max_channelids : remaining;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		എങ്കിൽ (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			തുടരുക;

		എങ്കിൽ (കണ്ണി_index >= start_index &&
			കണ്ണി_index < (start_index + returned)) {
			shmem_base[2 + കണ്ണി_ret] = cpu_to_le32(channel->channel_id);
			കണ്ണി_ret += 1;
		}

		കണ്ണി_index += 1;
	}

	/* final remaininig channel ids */
	remaining = channels_എണ്ണം - (start_index + returned);

	shmem_base[0] = cpu_to_le32(remaining);
	shmem_base[1] = cpu_to_le32(returned);

	sbi_hart_protection_temp_unmap_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_size);

	തിരികെ SBI_SUCCESS;
}

പൂർണ്ണസംഖ്യ sbi_mpxy_read_attrs(u32 channel_id, u32 base_attr_id, u32 attr_എണ്ണം)
{
	ഘടന mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	പൂർണ്ണസംഖ്യ ret = SBI_SUCCESS;
	u32 *attr_ptr, end_id;
	ശൂന്യം *shmem_base;
	ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();

	എങ്കിൽ (!mpxy_shmem_enabled(ms))
		തിരികെ SBI_ERR_NO_SHMEM;

	ഘടന sbi_mpxy_channel *channel = mpxy_find_channel(channel_id, dom);
	എങ്കിൽ (!channel)
		തിരികെ SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	എങ്കിൽ (base_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		base_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		തിരികെ SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	എങ്കിൽ (!attr_എണ്ണം || (attr_എണ്ണം > (mpxy_shmem_size / ATTR_SIZE)))
		തിരികെ SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	end_id = base_attr_id + attr_എണ്ണം - 1;

	sbi_hart_protection_temp_map_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_size);

	/* Standard attributes range check */
	എങ്കിൽ (mpxy_is_std_attr(base_attr_id)) {
		എങ്കിൽ (end_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			പോകുക out;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_base, &attr_ptr[base_attr_id],
				    attr_എണ്ണം);
	} അല്ലെങ്കിൽ {
		/**
		 * Even if the message protocol driver does not provide
		 * read attribute callback, return bad range error instead
		 * of not supported to let client distinguish it from channel
		 * id not supported.
		 * Check the complate range supported for message protocol
		 * attributes. Actual supported attributes will be checked
		 * by the message protocol driver.
		 */
		എങ്കിൽ (!channel->read_attributes ||
				end_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_END) {
			ret = SBI_ERR_BAD_RANGE;
			പോകുക out;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_attributes(channel,
					       (u32 *)shmem_base,
					       base_attr_id, attr_എണ്ണം);
	}
out:
	sbi_hart_protection_temp_unmap_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_size);
	തിരികെ ret;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ mpxy_check_write_std_attr(ഘടന sbi_mpxy_channel *channel,
				     u32 attr_id, u32 attr_val)
{
	ഘടന sbi_mpxy_channel_attrs *attrs = &channel->attrs;
	പൂർണ്ണസംഖ്യ ret = SBI_SUCCESS;

	തിരഞ്ഞെടുപ്പ്(attr_id) {
	അവസ്ഥ SBI_MPXY_ATTR_MSI_CONTROL:
		എങ്കിൽ (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		എങ്കിൽ (attr_val == 1 &&
		    (attrs->msi_info.msi_addr_lo == (u32)INVALID_ADDR) &&
		    (attrs->msi_info.msi_addr_hi == (u32)INVALID_ADDR))
			ret = SBI_ERR_DENIED;
		നിർത്തുക;
	അവസ്ഥ SBI_MPXY_ATTR_MSI_ADDR_LO:
	അവസ്ഥ SBI_MPXY_ATTR_MSI_ADDR_HI:
	അവസ്ഥ SBI_MPXY_ATTR_MSI_DATA:
		ret = SBI_SUCCESS;
		നിർത്തുക;
	അവസ്ഥ SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		എങ്കിൽ (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		/** All RO access attributes falls under default */
		ret = SBI_ERR_BAD_RANGE;
	};

	തിരികെ ret;
}

/**
 * Write the attribute value
 */
സ്ഥിതം ശൂന്യം mpxy_write_std_attr(ഘടന sbi_mpxy_channel *channel, u32 attr_id,
				u32 attr_val)
{
	ഘടന mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ഘടന sbi_mpxy_channel_attrs *attrs = &channel->attrs;

	തിരഞ്ഞെടുപ്പ്(attr_id) {
	അവസ്ഥ SBI_MPXY_ATTR_MSI_CONTROL:
		എങ്കിൽ (ms->msi_avail && attr_val <= 1)
			attrs->msi_control = attr_val;
		നിർത്തുക;
	അവസ്ഥ SBI_MPXY_ATTR_MSI_ADDR_LO:
		എങ്കിൽ (ms->msi_avail)
			attrs->msi_info.msi_addr_lo = attr_val;
		നിർത്തുക;
	അവസ്ഥ SBI_MPXY_ATTR_MSI_ADDR_HI:
		എങ്കിൽ (ms->msi_avail)
			attrs->msi_info.msi_addr_hi = attr_val;
		നിർത്തുക;
	അവസ്ഥ SBI_MPXY_ATTR_MSI_DATA:
		എങ്കിൽ (ms->msi_avail)
			attrs->msi_info.msi_data = attr_val;
		നിർത്തുക;
	അവസ്ഥ SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		എങ്കിൽ (channel->switch_eventsstate && attr_val <= 1) {
			attrs->eventsstate_ctrl = attr_val;
			 /* message protocol callback to enable/disable
			 * events state reporting. */
			channel->switch_eventsstate(attr_val);
		}

		നിർത്തുക;
	};
}

പൂർണ്ണസംഖ്യ sbi_mpxy_write_attrs(u32 channel_id, u32 base_attr_id, u32 attr_എണ്ണം)
{
	ഘടന mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, end_id, attr_val;
	ഘടന sbi_mpxy_channel *channel;
	പൂർണ്ണസംഖ്യ ret, mem_idx;
	ശൂന്യം *shmem_base;
	ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();

	എങ്കിൽ (!mpxy_shmem_enabled(ms))
		തിരികെ SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	എങ്കിൽ (!channel)
		തിരികെ SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	എങ്കിൽ (base_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		base_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		തിരികെ SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	എങ്കിൽ (!attr_എണ്ണം || (attr_എണ്ണം > (mpxy_shmem_size / ATTR_SIZE)))
		തിരികെ SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	end_id = base_attr_id + attr_എണ്ണം - 1;

	sbi_hart_protection_temp_map_range((unsigned long)shmem_base, mpxy_shmem_size);

	mem_ptr = (u32 *)shmem_base;

	എങ്കിൽ (mpxy_is_std_attr(base_attr_id)) {
		എങ്കിൽ (end_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			പോകുക out;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		വേണ്ടി (attr_id = base_attr_id; attr_id <= end_id; attr_id++) {
			attr_val = le32_to_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			എങ്കിൽ (ret)
				പോകുക out;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		വേണ്ടി (attr_id = base_attr_id; attr_id <= end_id; attr_id++) {
			attr_val = le32_to_cpu(mem_ptr[mem_idx++]);
			mpxy_write_std_attr(channel, attr_id, attr_val);
		}
	} അല്ലെങ്കിൽ {/**
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
		എങ്കിൽ (!channel->write_attributes ||
				end_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_END) {
			ret = SBI_ERR_BAD_RANGE;
			പോകുക out;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_attributes(channel,
					       (u32 *)shmem_base,
					       base_attr_id, attr_എണ്ണം);
	}
out:
	sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base, mpxy_shmem_size);
	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ sbi_mpxy_send_message(u32 channel_id, u8 സന്ദേശം_id,
			  unsigned long സന്ദേശം_data_len,
			  unsigned long *resp_data_len)
{
	ഘടന mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ഘടന sbi_mpxy_channel *channel;
	ശൂന്യം *shmem_base, *resp_buf;
	u32 resp_bufsize;
	ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();
	പൂർണ്ണസംഖ്യ ret;

	എങ്കിൽ (!mpxy_shmem_enabled(ms))
		തിരികെ SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	എങ്കിൽ (!channel)
		തിരികെ SBI_ERR_NOT_SUPPORTED;

	എങ്കിൽ (resp_data_len && !channel->send_message_with_response)
		തിരികെ SBI_ERR_NOT_SUPPORTED;

	എങ്കിൽ (!resp_data_len && !channel->send_message_without_response)
		തിരികെ SBI_ERR_NOT_SUPPORTED;

	എങ്കിൽ (സന്ദേശം_data_len > mpxy_shmem_size ||
		സന്ദേശം_data_len > channel->attrs.സന്ദേശം_data_maxlen)
		തിരികെ SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base, mpxy_shmem_size);

	എങ്കിൽ (resp_data_len) {
		resp_buf = shmem_base;
		resp_bufsize = mpxy_shmem_size;
		ret = channel->send_message_with_response(channel, സന്ദേശം_id,
							  shmem_base,
							  സന്ദേശം_data_len,
							  resp_buf,
							  resp_bufsize,
							  resp_data_len);
	} അല്ലെങ്കിൽ {
		ret = channel->send_message_without_response(channel, സന്ദേശം_id,
							     shmem_base,
							     സന്ദേശം_data_len);
	}

	sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base, mpxy_shmem_size);

	എങ്കിൽ (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		തിരികെ ret;
	അല്ലെങ്കിൽ എങ്കിൽ (ret)
		തിരികെ SBI_ERR_FAILED;

	എങ്കിൽ (resp_data_len &&
	    (*resp_data_len > mpxy_shmem_size ||
	     *resp_data_len > channel->attrs.സന്ദേശം_data_maxlen))
		തിരികെ SBI_ERR_FAILED;

	തിരികെ SBI_SUCCESS;
}

പൂർണ്ണസംഖ്യ sbi_mpxy_get_notification_events(u32 channel_id, unsigned long *events_len)
{
	ഘടന mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ഘടന sbi_mpxy_channel *channel;
	ശൂന്യം *eventsbuf, *shmem_base;
	ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();
	പൂർണ്ണസംഖ്യ ret;

	എങ്കിൽ (!mpxy_shmem_enabled(ms))
		തിരികെ SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	എങ്കിൽ (!channel || !channel->get_notification_events)
		തിരികെ SBI_ERR_NOT_SUPPORTED;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base, mpxy_shmem_size);
	eventsbuf = shmem_base;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_size,
					       events_len);
	sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base, mpxy_shmem_size);

	എങ്കിൽ (ret)
		തിരികെ ret;

	എങ്കിൽ (*events_len > (mpxy_shmem_size - 16))
		തിരികെ SBI_ERR_FAILED;

	തിരികെ SBI_SUCCESS;
}
