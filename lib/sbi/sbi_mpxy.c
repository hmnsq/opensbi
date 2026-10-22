/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_hart_protection.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_irqchip.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_mpxy.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_byteorder.h>

/** Shared memory size across all harts */
நிலையான unsigned long mpxy_shmem_size = PAGE_SIZE;

/** List of MPXY proxy channels */
நிலையான SBI_LIST_HEAD(mpxy_channel_list);

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
#define CAP_SEND_செய்தி_WITH_RESP_POS	3
#define CAP_SEND_செய்தி_WITH_RESP_MASK	(1U << CAP_SEND_செய்தி_WITH_RESP_POS)

/** Channel Capability - Send Message Without Response function support */
#define CAP_SEND_செய்தி_WITHOUT_RESP_POS	4
#define CAP_SEND_செய்தி_WITHOUT_RESP_MASK	(1U << CAP_SEND_செய்தி_WITHOUT_RESP_POS)

/** Channel Capability - Get Notification function support */
#define CAP_GET_NOTIFICATIONS_POS	5
#define CAP_GET_NOTIFICATIONS_MASK	(1U << CAP_GET_NOTIFICATIONS_POS)

/** Helpers to enable/disable channel capability bits
 * _c: capability variable
 * _m: capability mask
 */
#define CAP_ENABLE(_c, _m)	செருகு_FIELD(_c, _m, 1)
#define CAP_DISABLE(_c, _m)	செருகு_FIELD(_c, _m, 0)
#define CAP_GET(_c, _m)		EXTRACT_FIELD(_c, _m)

#define SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
கட்டமைப்பு mpxy_shmem {
	unsigned long shmem_addr_lo;
	unsigned long shmem_addr_hi;
};

கட்டமைப்பு mpxy_state {
	/* MSI support in MPXY */
	bool msi_avail;
	/* SSE support in MPXY */
	bool sse_avail;
	/* MPXY Shared memory details */
	கட்டமைப்பு mpxy_shmem shmem;
};

நிலையான கட்டமைப்பு mpxy_state *sbi_domain_get_mpxy_state(கட்டமைப்பு sbi_domain *dom,
						    u32 hartindex);

/** Macro to obtain the current hart's MPXY state pointer in current domain */
#define sbi_domain_mpxy_state_thishart_ptr()			\
	sbi_domain_get_mpxy_state(sbi_domain_thishart_ptr(),	\
				  current_hartindex())

/** Disable hart shared memory */
நிலையான உள்வரி வெறுமை sbi_mpxy_shmem_disable(கட்டமைப்பு mpxy_state *ms)
{
	ms->shmem.shmem_addr_lo = INVALID_ADDR;
	ms->shmem.shmem_addr_hi = INVALID_ADDR;
}

/** Check if shared memory is already setup on hart */
நிலையான உள்வரி bool mpxy_shmem_enabled(கட்டமைப்பு mpxy_state *ms)
{
	திரும்பு (ms->shmem.shmem_addr_lo == INVALID_ADDR
		&& ms->shmem.shmem_addr_hi == INVALID_ADDR) ?
		false : true;
}

/** Get hart shared memory base address */
நிலையான உள்வரி வெறுமை *hart_shmem_base(கட்டமைப்பு mpxy_state *ms)
{
	திரும்பு (வெறுமை *)(unsigned long)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)				\
	_Static_assert(							\
		((offsetof(கட்டமைப்பு sbi_mpxy_channel_attrs, field)) /	\
		 அளவு(u32)) == attr_offset,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_field_offset(செய்தி_proto_id, SBI_MPXY_ATTR_செய்தி_PROT_ID);
assert_field_offset(செய்தி_proto_version, SBI_MPXY_ATTR_செய்தி_PROT_VER);
assert_field_offset(செய்தி_data_maxlen, SBI_MPXY_ATTR_செய்தி_MAX_LEN);
assert_field_offset(செய்தி_send_timeout, SBI_MPXY_ATTR_செய்தி_SEND_TIMEOUT);
assert_field_offset(செய்தி_completion_timeout, SBI_MPXY_ATTR_செய்தி_COMPLETION_TIMEOUT);
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
நிலையான உள்வரி bool mpxy_is_std_attr(u32 attr_id)
{
	திரும்பு (attr_id >> 31) ? false : true;
}

நிலையான உள்வரி bool mpxy_channel_visible(கட்டமைப்பு sbi_mpxy_channel *channel,
					கட்டமைப்பு sbi_domain *dom)
{
	திரும்பு channel->owner_domain == dom;
}

/** Find channel_id in registered channels list for this domain */
நிலையான கட்டமைப்பு sbi_mpxy_channel *mpxy_find_channel(u32 channel_id,
					கட்டமைப்பு sbi_domain *dom)
{
	கட்டமைப்பு sbi_mpxy_channel *channel;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		எனில் (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			திரும்பு channel;
	}

	திரும்பு வெற்று;
}

/** Find channel_id in registered channels list */
கட்டமைப்பு sbi_mpxy_channel *sbi_mpxy_find_channel_any(u32 channel_id)
{
	கட்டமைப்பு sbi_mpxy_channel *channel;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		எனில் (channel->channel_id == channel_id)
			திரும்பு channel;
	}

	திரும்பு வெற்று;
}

/** Copy attributes word size */
நிலையான வெறுமை mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 மொத்தம்)
{
	முழு idx;
	க்கு (idx = 0; idx < மொத்தம்; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_available(வெறுமை)
{
	திரும்பு sbi_list_empty(&mpxy_channel_list) ? false : true;
}

நிலையான வெறுமை mpxy_std_attrs_init(கட்டமைப்பு sbi_mpxy_channel *channel)
{
	கட்டமைப்பு mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_data = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	எனில் (channel->send_message_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_செய்தி_WITH_RESP_MASK);

	எனில் (channel->send_message_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_செய்தி_WITHOUT_RESP_MASK);

	எனில் (channel->get_notification_events) {
		capability = CAP_ENABLE(capability, CAP_GET_NOTIFICATIONS_MASK);
		/**
		 * Check if MSI or SSE available for notification interrrupt.
		 * Priority given to MSI if both MSI and SSE are avaialble.
		 */
		எனில் (ms->msi_avail)
			capability = CAP_ENABLE(capability, CAP_MSI_MASK);
		இல்லையெனில் எனில் (ms->sse_avail) {
			capability = CAP_ENABLE(capability, CAP_SSE_MASK);
			/* TODO: Assign SSE EVENT_ID for the channel */
		}

		/**
		 * switch_eventstate callback support means support for events
		 * state reporting supoprt. Enable events state reporting in
		 * channel capability.
		 */
		எனில் (channel->switch_eventsstate)
			capability = CAP_ENABLE(capability, CAP_EVENTSSTATE_MASK);
	}

	channel->attrs.capability = capability;
}

/**
 * Register a channel with MPXY framework.
 * Called by message protocol drivers
 */
முழு sbi_mpxy_register_channel(கட்டமைப்பு sbi_mpxy_channel *channel)
{
	எனில் (!channel)
		திரும்பு SBI_EINVAL;

	எனில் (!channel->owner_domain)
		திரும்பு SBI_EINVAL;

	எனில் (sbi_mpxy_find_channel_any(channel->channel_id))
		திரும்பு SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	எனில் (mpxy_shmem_size < channel->attrs.செய்தி_data_maxlen) {
		mpxy_shmem_size = ROUNDUP(channel->attrs.செய்தி_data_maxlen,
					  PAGE_SIZE);
	}

	sbi_list_add_tail(&channel->head, &mpxy_channel_list);

	திரும்பு SBI_OK;
}

/** Setup per domain MPXY state data */
நிலையான முழு domain_mpxy_state_setup(கட்டமைப்பு sbi_domain *dom,
					கட்டமைப்பு sbi_domain_state *state,
					வெறுமை *state_ptr)
{
	கட்டமைப்பு mpxy_state **dom_hartindex_to_mpxy_state_table = state_ptr;
	கட்டமைப்பு mpxy_state *ms;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ms = sbi_zalloc(அளவு(*ms));
		எனில் (!ms)
			திரும்பு SBI_ENOMEM;

		/*
		 * TODO: Proper support for checking sse support from
		 * platform. Currently disable sse and use polling
		 */
		ms->msi_avail = !!sbi_irqchip_find_device_by_caps(SBI_IRQCHIP_CAPS_MSI, வெற்று);
		ms->sse_avail = false;

		sbi_mpxy_shmem_disable(ms);

		dom_hartindex_to_mpxy_state_table[i] = ms;
	}

	திரும்பு 0;
}

/** Cleanup per domain MPXY state data */
நிலையான வெறுமை domain_mpxy_state_cleanup(கட்டமைப்பு sbi_domain *dom,
					   கட்டமைப்பு sbi_domain_state *state,
					   வெறுமை *state_ptr)
{
	கட்டமைப்பு mpxy_state **dom_hartindex_to_mpxy_state_table = state_ptr;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts)
		sbi_free(dom_hartindex_to_mpxy_state_table[i]);
}

நிலையான கட்டமைப்பு sbi_domain_state dmstate = {
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
நிலையான கட்டமைப்பு mpxy_state *sbi_domain_get_mpxy_state(கட்டமைப்பு sbi_domain *dom,
						    u32 hartindex)
{
	கட்டமைப்பு mpxy_state **dom_hartindex_to_mpxy_state_table;

	dom_hartindex_to_mpxy_state_table = sbi_domain_state_ptr(dom, &dmstate);
	எனில் (!dom_hartindex_to_mpxy_state_table ||
	    !sbi_hartindex_valid(hartindex))
		திரும்பு வெற்று;

	திரும்பு dom_hartindex_to_mpxy_state_table[hartindex];
}

முழு sbi_mpxy_init(கட்டமைப்பு sbi_scratch *scratch)
{
	முழு ret;

	/**
	 * Allocate per-domain and per-hart MPXY state data.
	 * The data type is "struct mpxy_state **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one() and
	 * domain_mpxy_state_setup(). Calculate needed size of memory space
	 * here.
	 */
	dmstate.state_size = அளவு(கட்டமைப்பு mpxy_state *) * sbi_hart_மொத்தம்();
	ret = sbi_domain_register_state(&dmstate);
	எனில் (ret)
		திரும்பு ret;

	திரும்பு sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

unsigned long sbi_mpxy_get_shmem_size(வெறுமை)
{
	திரும்பு mpxy_shmem_size;
}

முழு sbi_mpxy_set_shmem(unsigned long shmem_phys_lo,
		       unsigned long shmem_phys_hi,
		       unsigned long flags)
{
	கட்டமைப்பு mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	unsigned long *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	எனில் (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		திரும்பு SBI_SUCCESS;
	}

	எனில் (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		திரும்பு SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	எனில் (shmem_phys_lo & ~PAGE_MASK)
		திரும்பு SBI_ERR_INVALID_PARAM;

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
	எனில் (shmem_phys_hi)
		திரும்பு SBI_ERR_INVALID_ADDRESS;

	எனில் (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_size, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		திரும்பு SBI_ERR_INVALID_ADDRESS;

	/** Save the current shmem details in new shmem region */
	எனில் (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
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

	திரும்பு SBI_SUCCESS;
}

முழு sbi_mpxy_get_channel_ids(u32 start_index)
{
	கட்டமைப்பு mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 remaining, returned, max_channelids;
	u32 முனை_index = 0, முனை_ret = 0;
	கட்டமைப்பு sbi_mpxy_channel *channel;
	u32 channels_மொத்தம் = 0;
	u32 *shmem_base;
	கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();

	எனில் (!mpxy_shmem_enabled(ms))
		திரும்பு SBI_ERR_NO_SHMEM;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		எனில் (mpxy_channel_visible(channel, dom))
			channels_மொத்தம் += 1;
	}

	எனில் (start_index > channels_மொத்தம்)
		திரும்பு SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_temp_map_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_size);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_size / அளவு(u32)) - 2;
	/* total remaining from the start index */
	remaining = channels_மொத்தம் - start_index;
	/* how many can be returned */
	returned = (remaining > max_channelids)? max_channelids : remaining;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		எனில் (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			தொடர்;

		எனில் (முனை_index >= start_index &&
			முனை_index < (start_index + returned)) {
			shmem_base[2 + முனை_ret] = cpu_to_le32(channel->channel_id);
			முனை_ret += 1;
		}

		முனை_index += 1;
	}

	/* final remaininig channel ids */
	remaining = channels_மொத்தம் - (start_index + returned);

	shmem_base[0] = cpu_to_le32(remaining);
	shmem_base[1] = cpu_to_le32(returned);

	sbi_hart_protection_temp_unmap_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_size);

	திரும்பு SBI_SUCCESS;
}

முழு sbi_mpxy_read_attrs(u32 channel_id, u32 base_attr_id, u32 attr_மொத்தம்)
{
	கட்டமைப்பு mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	முழு ret = SBI_SUCCESS;
	u32 *attr_ptr, end_id;
	வெறுமை *shmem_base;
	கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();

	எனில் (!mpxy_shmem_enabled(ms))
		திரும்பு SBI_ERR_NO_SHMEM;

	கட்டமைப்பு sbi_mpxy_channel *channel = mpxy_find_channel(channel_id, dom);
	எனில் (!channel)
		திரும்பு SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	எனில் (base_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		base_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		திரும்பு SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	எனில் (!attr_மொத்தம் || (attr_மொத்தம் > (mpxy_shmem_size / ATTR_SIZE)))
		திரும்பு SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	end_id = base_attr_id + attr_மொத்தம் - 1;

	sbi_hart_protection_temp_map_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_size);

	/* Standard attributes range check */
	எனில் (mpxy_is_std_attr(base_attr_id)) {
		எனில் (end_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			செல் out;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_base, &attr_ptr[base_attr_id],
				    attr_மொத்தம்);
	} இல்லையெனில் {
		/**
		 * Even if the message protocol driver does not provide
		 * read attribute callback, return bad range error instead
		 * of not supported to let client distinguish it from channel
		 * id not supported.
		 * Check the complate range supported for message protocol
		 * attributes. Actual supported attributes will be checked
		 * by the message protocol driver.
		 */
		எனில் (!channel->read_attributes ||
				end_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_END) {
			ret = SBI_ERR_BAD_RANGE;
			செல் out;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_attributes(channel,
					       (u32 *)shmem_base,
					       base_attr_id, attr_மொத்தம்);
	}
out:
	sbi_hart_protection_temp_unmap_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_size);
	திரும்பு ret;
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
நிலையான முழு mpxy_check_write_std_attr(கட்டமைப்பு sbi_mpxy_channel *channel,
				     u32 attr_id, u32 attr_val)
{
	கட்டமைப்பு sbi_mpxy_channel_attrs *attrs = &channel->attrs;
	முழு ret = SBI_SUCCESS;

	தேர்வு(attr_id) {
	நிலை SBI_MPXY_ATTR_MSI_CONTROL:
		எனில் (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		எனில் (attr_val == 1 &&
		    (attrs->msi_info.msi_addr_lo == (u32)INVALID_ADDR) &&
		    (attrs->msi_info.msi_addr_hi == (u32)INVALID_ADDR))
			ret = SBI_ERR_DENIED;
		நிறுத்து;
	நிலை SBI_MPXY_ATTR_MSI_ADDR_LO:
	நிலை SBI_MPXY_ATTR_MSI_ADDR_HI:
	நிலை SBI_MPXY_ATTR_MSI_DATA:
		ret = SBI_SUCCESS;
		நிறுத்து;
	நிலை SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		எனில் (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		நிறுத்து;
	இயல்பு:
		/** All RO access attributes falls under default */
		ret = SBI_ERR_BAD_RANGE;
	};

	திரும்பு ret;
}

/**
 * Write the attribute value
 */
நிலையான வெறுமை mpxy_write_std_attr(கட்டமைப்பு sbi_mpxy_channel *channel, u32 attr_id,
				u32 attr_val)
{
	கட்டமைப்பு mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	கட்டமைப்பு sbi_mpxy_channel_attrs *attrs = &channel->attrs;

	தேர்வு(attr_id) {
	நிலை SBI_MPXY_ATTR_MSI_CONTROL:
		எனில் (ms->msi_avail && attr_val <= 1)
			attrs->msi_control = attr_val;
		நிறுத்து;
	நிலை SBI_MPXY_ATTR_MSI_ADDR_LO:
		எனில் (ms->msi_avail)
			attrs->msi_info.msi_addr_lo = attr_val;
		நிறுத்து;
	நிலை SBI_MPXY_ATTR_MSI_ADDR_HI:
		எனில் (ms->msi_avail)
			attrs->msi_info.msi_addr_hi = attr_val;
		நிறுத்து;
	நிலை SBI_MPXY_ATTR_MSI_DATA:
		எனில் (ms->msi_avail)
			attrs->msi_info.msi_data = attr_val;
		நிறுத்து;
	நிலை SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		எனில் (channel->switch_eventsstate && attr_val <= 1) {
			attrs->eventsstate_ctrl = attr_val;
			 /* message protocol callback to enable/disable
			 * events state reporting. */
			channel->switch_eventsstate(attr_val);
		}

		நிறுத்து;
	};
}

முழு sbi_mpxy_write_attrs(u32 channel_id, u32 base_attr_id, u32 attr_மொத்தம்)
{
	கட்டமைப்பு mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, end_id, attr_val;
	கட்டமைப்பு sbi_mpxy_channel *channel;
	முழு ret, mem_idx;
	வெறுமை *shmem_base;
	கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();

	எனில் (!mpxy_shmem_enabled(ms))
		திரும்பு SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	எனில் (!channel)
		திரும்பு SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	எனில் (base_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		base_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		திரும்பு SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	எனில் (!attr_மொத்தம் || (attr_மொத்தம் > (mpxy_shmem_size / ATTR_SIZE)))
		திரும்பு SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	end_id = base_attr_id + attr_மொத்தம் - 1;

	sbi_hart_protection_temp_map_range((unsigned long)shmem_base, mpxy_shmem_size);

	mem_ptr = (u32 *)shmem_base;

	எனில் (mpxy_is_std_attr(base_attr_id)) {
		எனில் (end_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			செல் out;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		க்கு (attr_id = base_attr_id; attr_id <= end_id; attr_id++) {
			attr_val = le32_to_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			எனில் (ret)
				செல் out;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		க்கு (attr_id = base_attr_id; attr_id <= end_id; attr_id++) {
			attr_val = le32_to_cpu(mem_ptr[mem_idx++]);
			mpxy_write_std_attr(channel, attr_id, attr_val);
		}
	} இல்லையெனில் {/**
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
		எனில் (!channel->write_attributes ||
				end_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_END) {
			ret = SBI_ERR_BAD_RANGE;
			செல் out;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_attributes(channel,
					       (u32 *)shmem_base,
					       base_attr_id, attr_மொத்தம்);
	}
out:
	sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base, mpxy_shmem_size);
	திரும்பு ret;
}

முழு sbi_mpxy_send_message(u32 channel_id, u8 செய்தி_id,
			  unsigned long செய்தி_data_len,
			  unsigned long *resp_data_len)
{
	கட்டமைப்பு mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	கட்டமைப்பு sbi_mpxy_channel *channel;
	வெறுமை *shmem_base, *resp_buf;
	u32 resp_bufsize;
	கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();
	முழு ret;

	எனில் (!mpxy_shmem_enabled(ms))
		திரும்பு SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	எனில் (!channel)
		திரும்பு SBI_ERR_NOT_SUPPORTED;

	எனில் (resp_data_len && !channel->send_message_with_response)
		திரும்பு SBI_ERR_NOT_SUPPORTED;

	எனில் (!resp_data_len && !channel->send_message_without_response)
		திரும்பு SBI_ERR_NOT_SUPPORTED;

	எனில் (செய்தி_data_len > mpxy_shmem_size ||
		செய்தி_data_len > channel->attrs.செய்தி_data_maxlen)
		திரும்பு SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base, mpxy_shmem_size);

	எனில் (resp_data_len) {
		resp_buf = shmem_base;
		resp_bufsize = mpxy_shmem_size;
		ret = channel->send_message_with_response(channel, செய்தி_id,
							  shmem_base,
							  செய்தி_data_len,
							  resp_buf,
							  resp_bufsize,
							  resp_data_len);
	} இல்லையெனில் {
		ret = channel->send_message_without_response(channel, செய்தி_id,
							     shmem_base,
							     செய்தி_data_len);
	}

	sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base, mpxy_shmem_size);

	எனில் (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		திரும்பு ret;
	இல்லையெனில் எனில் (ret)
		திரும்பு SBI_ERR_FAILED;

	எனில் (resp_data_len &&
	    (*resp_data_len > mpxy_shmem_size ||
	     *resp_data_len > channel->attrs.செய்தி_data_maxlen))
		திரும்பு SBI_ERR_FAILED;

	திரும்பு SBI_SUCCESS;
}

முழு sbi_mpxy_get_notification_events(u32 channel_id, unsigned long *events_len)
{
	கட்டமைப்பு mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	கட்டமைப்பு sbi_mpxy_channel *channel;
	வெறுமை *eventsbuf, *shmem_base;
	கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();
	முழு ret;

	எனில் (!mpxy_shmem_enabled(ms))
		திரும்பு SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	எனில் (!channel || !channel->get_notification_events)
		திரும்பு SBI_ERR_NOT_SUPPORTED;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base, mpxy_shmem_size);
	eventsbuf = shmem_base;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_size,
					       events_len);
	sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base, mpxy_shmem_size);

	எனில் (ret)
		திரும்பு ret;

	எனில் (*events_len > (mpxy_shmem_size - 16))
		திரும்பு SBI_ERR_FAILED;

	திரும்பு SBI_SUCCESS;
}
