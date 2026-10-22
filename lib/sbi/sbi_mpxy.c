/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_hart_protection.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_irqchip.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_mpxy.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi/sbi_bitops.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_byteorder.h>

/** Shared memory size across all harts */
স্থির unsigned long mpxy_shmem_আকার = PAGE_আকার;

/** List of MPXY proxy channels */
স্থির SBI_LIST_শীর্ষ(mpxy_channel_list);

/** Invalid Physical Address(all bits 1) */
#define INVALID_ADDR		(-1UL)

/** MPXY Attribute size in bytes */
#define ATTR_আকার		(4)

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
#define CAP_SEND_বার্তা_WITH_RESP_POS	3
#define CAP_SEND_বার্তা_WITH_RESP_MASK	(1U << CAP_SEND_বার্তা_WITH_RESP_POS)

/** Channel Capability - Send Message Without Response function support */
#define CAP_SEND_বার্তা_WITHOUT_RESP_POS	4
#define CAP_SEND_বার্তা_WITHOUT_RESP_MASK	(1U << CAP_SEND_বার্তা_WITHOUT_RESP_POS)

/** Channel Capability - Get Notification function support */
#define CAP_GET_NOTIFICATIONS_POS	5
#define CAP_GET_NOTIFICATIONS_MASK	(1U << CAP_GET_NOTIFICATIONS_POS)

/** Helpers to enable/disable channel capability bits
 * _c: capability variable
 * _m: capability mask
 */
#define CAP_ENABLE(_c, _m)	ঢোকাও_FIELD(_c, _m, 1)
#define CAP_DISABLE(_c, _m)	ঢোকাও_FIELD(_c, _m, 0)
#define CAP_GET(_c, _m)		EXTRACT_FIELD(_c, _m)

#define SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
কাঠামো mpxy_shmem {
	unsigned long shmem_addr_lo;
	unsigned long shmem_addr_hi;
};

কাঠামো mpxy_state {
	/* MSI support in MPXY */
	bool msi_avail;
	/* SSE support in MPXY */
	bool sse_avail;
	/* MPXY Shared memory details */
	কাঠামো mpxy_shmem shmem;
};

স্থির কাঠামো mpxy_state *sbi_domain_get_mpxy_state(কাঠামো sbi_domain *dom,
						    u32 hartindex);

/** Macro to obtain the current hart's MPXY state pointer in current domain */
#define sbi_domain_mpxy_state_thishart_ptr()			\
	sbi_domain_get_mpxy_state(sbi_domain_thishart_ptr(),	\
				  current_hartindex())

/** Disable hart shared memory */
স্থির অন্তঃরেখ শূন্য sbi_mpxy_shmem_disable(কাঠামো mpxy_state *ms)
{
	ms->shmem.shmem_addr_lo = INVALID_ADDR;
	ms->shmem.shmem_addr_hi = INVALID_ADDR;
}

/** Check if shared memory is already setup on hart */
স্থির অন্তঃরেখ bool mpxy_shmem_enabled(কাঠামো mpxy_state *ms)
{
	ফেরত (ms->shmem.shmem_addr_lo == INVALID_ADDR
		&& ms->shmem.shmem_addr_hi == INVALID_ADDR) ?
		false : true;
}

/** Get hart shared memory base address */
স্থির অন্তঃরেখ শূন্য *hart_shmem_base(কাঠামো mpxy_state *ms)
{
	ফেরত (শূন্য *)(unsigned long)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)				\
	_Static_assert(							\
		((offsetof(কাঠামো sbi_mpxy_channel_attrs, field)) /	\
		 আকার(u32)) == attr_offset,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_field_offset(বার্তা_proto_id, SBI_MPXY_ATTR_বার্তা_PROT_ID);
assert_field_offset(বার্তা_proto_version, SBI_MPXY_ATTR_বার্তা_PROT_VER);
assert_field_offset(বার্তা_তথ্য_maxlen, SBI_MPXY_ATTR_বার্তা_MAX_LEN);
assert_field_offset(বার্তা_send_timeout, SBI_MPXY_ATTR_বার্তা_SEND_TIMEOUT);
assert_field_offset(বার্তা_completion_timeout, SBI_MPXY_ATTR_বার্তা_COMPLETION_TIMEOUT);
assert_field_offset(capability, SBI_MPXY_ATTR_CHANNEL_CAPABILITY);
assert_field_offset(sse_event_id, SBI_MPXY_ATTR_SSE_EVENT_ID);
assert_field_offset(msi_control, SBI_MPXY_ATTR_MSI_CONTROL);
assert_field_offset(msi_info.msi_addr_lo, SBI_MPXY_ATTR_MSI_ADDR_LO);
assert_field_offset(msi_info.msi_addr_hi, SBI_MPXY_ATTR_MSI_ADDR_HI);
assert_field_offset(msi_info.msi_তথ্য, SBI_MPXY_ATTR_MSI_তথ্য);
assert_field_offset(eventsstate_ctrl, SBI_MPXY_ATTR_EVENTS_STATE_CONTROL);

/**
 * Check if the attribute is a standard attribute or
 * a message protocol specific attribute
 * attr_id[31] = 0 for standard
 * attr_id[31] = 1 for message protocol specific
 */
স্থির অন্তঃরেখ bool mpxy_is_std_attr(u32 attr_id)
{
	ফেরত (attr_id >> 31) ? false : true;
}

স্থির অন্তঃরেখ bool mpxy_channel_visible(কাঠামো sbi_mpxy_channel *channel,
					কাঠামো sbi_domain *dom)
{
	ফেরত channel->owner_domain == dom;
}

/** Find channel_id in registered channels list for this domain */
স্থির কাঠামো sbi_mpxy_channel *mpxy_find_channel(u32 channel_id,
					কাঠামো sbi_domain *dom)
{
	কাঠামো sbi_mpxy_channel *channel;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, শীর্ষ) {
		যদি (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			ফেরত channel;
	}

	ফেরত ফাঁকা;
}

/** Find channel_id in registered channels list */
কাঠামো sbi_mpxy_channel *sbi_mpxy_find_channel_any(u32 channel_id)
{
	কাঠামো sbi_mpxy_channel *channel;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, শীর্ষ) {
		যদি (channel->channel_id == channel_id)
			ফেরত channel;
	}

	ফেরত ফাঁকা;
}

/** Copy attributes word size */
স্থির শূন্য mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 গুনতি)
{
	পূর্ণসংখ্যা idx;
	জন্য (idx = 0; idx < গুনতি; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_available(শূন্য)
{
	ফেরত sbi_list_empty(&mpxy_channel_list) ? false : true;
}

স্থির শূন্য mpxy_std_attrs_init(কাঠামো sbi_mpxy_channel *channel)
{
	কাঠামো mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_তথ্য = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	যদি (channel->send_message_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_বার্তা_WITH_RESP_MASK);

	যদি (channel->send_message_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_বার্তা_WITHOUT_RESP_MASK);

	যদি (channel->get_notification_events) {
		capability = CAP_ENABLE(capability, CAP_GET_NOTIFICATIONS_MASK);
		/**
		 * Check if MSI or SSE available for notification interrrupt.
		 * Priority given to MSI if both MSI and SSE are avaialble.
		 */
		যদি (ms->msi_avail)
			capability = CAP_ENABLE(capability, CAP_MSI_MASK);
		অন্যথা যদি (ms->sse_avail) {
			capability = CAP_ENABLE(capability, CAP_SSE_MASK);
			/* TODO: Assign SSE EVENT_ID for the channel */
		}

		/**
		 * switch_eventstate callback support means support for events
		 * state reporting supoprt. Enable events state reporting in
		 * channel capability.
		 */
		যদি (channel->switch_eventsstate)
			capability = CAP_ENABLE(capability, CAP_EVENTSSTATE_MASK);
	}

	channel->attrs.capability = capability;
}

/**
 * Register a channel with MPXY framework.
 * Called by message protocol drivers
 */
পূর্ণসংখ্যা sbi_mpxy_register_channel(কাঠামো sbi_mpxy_channel *channel)
{
	যদি (!channel)
		ফেরত SBI_EINVAL;

	যদি (!channel->owner_domain)
		ফেরত SBI_EINVAL;

	যদি (sbi_mpxy_find_channel_any(channel->channel_id))
		ফেরত SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	যদি (mpxy_shmem_আকার < channel->attrs.বার্তা_তথ্য_maxlen) {
		mpxy_shmem_আকার = ROUNDUP(channel->attrs.বার্তা_তথ্য_maxlen,
					  PAGE_আকার);
	}

	sbi_list_add_লেজ(&channel->শীর্ষ, &mpxy_channel_list);

	ফেরত SBI_OK;
}

/** Setup per domain MPXY state data */
স্থির পূর্ণসংখ্যা domain_mpxy_state_setup(কাঠামো sbi_domain *dom,
					কাঠামো sbi_domain_state *state,
					শূন্য *state_ptr)
{
	কাঠামো mpxy_state **dom_hartindex_to_mpxy_state_table = state_ptr;
	কাঠামো mpxy_state *ms;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ms = sbi_zalloc(আকার(*ms));
		যদি (!ms)
			ফেরত SBI_ENOMEM;

		/*
		 * TODO: Proper support for checking sse support from
		 * platform. Currently disable sse and use polling
		 */
		ms->msi_avail = !!sbi_irqchip_find_device_by_caps(SBI_IRQCHIP_CAPS_MSI, ফাঁকা);
		ms->sse_avail = false;

		sbi_mpxy_shmem_disable(ms);

		dom_hartindex_to_mpxy_state_table[i] = ms;
	}

	ফেরত 0;
}

/** Cleanup per domain MPXY state data */
স্থির শূন্য domain_mpxy_state_cleanup(কাঠামো sbi_domain *dom,
					   কাঠামো sbi_domain_state *state,
					   শূন্য *state_ptr)
{
	কাঠামো mpxy_state **dom_hartindex_to_mpxy_state_table = state_ptr;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts)
		sbi_free(dom_hartindex_to_mpxy_state_table[i]);
}

স্থির কাঠামো sbi_domain_state dmstate = {
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
স্থির কাঠামো mpxy_state *sbi_domain_get_mpxy_state(কাঠামো sbi_domain *dom,
						    u32 hartindex)
{
	কাঠামো mpxy_state **dom_hartindex_to_mpxy_state_table;

	dom_hartindex_to_mpxy_state_table = sbi_domain_state_ptr(dom, &dmstate);
	যদি (!dom_hartindex_to_mpxy_state_table ||
	    !sbi_hartindex_valid(hartindex))
		ফেরত ফাঁকা;

	ফেরত dom_hartindex_to_mpxy_state_table[hartindex];
}

পূর্ণসংখ্যা sbi_mpxy_init(কাঠামো sbi_scratch *scratch)
{
	পূর্ণসংখ্যা ret;

	/**
	 * Allocate per-domain and per-hart MPXY state data.
	 * The data type is "struct mpxy_state **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one() and
	 * domain_mpxy_state_setup(). Calculate needed size of memory space
	 * here.
	 */
	dmstate.state_আকার = আকার(কাঠামো mpxy_state *) * sbi_hart_গুনতি();
	ret = sbi_domain_register_state(&dmstate);
	যদি (ret)
		ফেরত ret;

	ফেরত sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

unsigned long sbi_mpxy_get_shmem_আকার(শূন্য)
{
	ফেরত mpxy_shmem_আকার;
}

পূর্ণসংখ্যা sbi_mpxy_set_shmem(unsigned long shmem_phys_lo,
		       unsigned long shmem_phys_hi,
		       unsigned long flags)
{
	কাঠামো mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	unsigned long *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	যদি (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		ফেরত SBI_SUCCESS;
	}

	যদি (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		ফেরত SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	যদি (shmem_phys_lo & ~PAGE_MASK)
		ফেরত SBI_ERR_INVALID_PARAM;

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
	যদি (shmem_phys_hi)
		ফেরত SBI_ERR_INVALID_ADDRESS;

	যদি (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_আকার, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ফেরত SBI_ERR_INVALID_ADDRESS;

	/** Save the current shmem details in new shmem region */
	যদি (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
		ret_buf = (unsigned long *)(ulong)SHMEM_PHYS_ADDR(shmem_phys_hi,
								  shmem_phys_lo);
		sbi_hart_protection_অস্থায়ী_map_range((unsigned long)ret_buf, mpxy_shmem_আকার);
		ret_buf[0] = cpu_to_lle(ms->shmem.shmem_addr_lo);
		ret_buf[1] = cpu_to_lle(ms->shmem.shmem_addr_hi);
		sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)ret_buf, mpxy_shmem_আকার);
	}

	/** Setup the new shared memory */
	ms->shmem.shmem_addr_lo = shmem_phys_lo;
	ms->shmem.shmem_addr_hi = shmem_phys_hi;

	ফেরত SBI_SUCCESS;
}

পূর্ণসংখ্যা sbi_mpxy_get_channel_ids(u32 start_সূচক)
{
	কাঠামো mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 remaining, returned, max_channelids;
	u32 গ্রন্থি_সূচক = 0, গ্রন্থি_ret = 0;
	কাঠামো sbi_mpxy_channel *channel;
	u32 channels_গুনতি = 0;
	u32 *shmem_base;
	কাঠামো sbi_domain *dom = sbi_domain_thishart_ptr();

	যদি (!mpxy_shmem_enabled(ms))
		ফেরত SBI_ERR_NO_SHMEM;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, শীর্ষ) {
		যদি (mpxy_channel_visible(channel, dom))
			channels_গুনতি += 1;
	}

	যদি (start_সূচক > channels_গুনতি)
		ফেরত SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_অস্থায়ী_map_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_আকার);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_আকার / আকার(u32)) - 2;
	/* total remaining from the start index */
	remaining = channels_গুনতি - start_সূচক;
	/* how many can be returned */
	returned = (remaining > max_channelids)? max_channelids : remaining;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_entry(channel, &mpxy_channel_list, শীর্ষ) {
		যদি (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			চালু;

		যদি (গ্রন্থি_সূচক >= start_সূচক &&
			গ্রন্থি_সূচক < (start_সূচক + returned)) {
			shmem_base[2 + গ্রন্থি_ret] = cpu_to_le32(channel->channel_id);
			গ্রন্থি_ret += 1;
		}

		গ্রন্থি_সূচক += 1;
	}

	/* final remaininig channel ids */
	remaining = channels_গুনতি - (start_সূচক + returned);

	shmem_base[0] = cpu_to_le32(remaining);
	shmem_base[1] = cpu_to_le32(returned);

	sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_আকার);

	ফেরত SBI_SUCCESS;
}

পূর্ণসংখ্যা sbi_mpxy_read_attrs(u32 channel_id, u32 base_attr_id, u32 attr_গুনতি)
{
	কাঠামো mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	পূর্ণসংখ্যা ret = SBI_SUCCESS;
	u32 *attr_ptr, end_id;
	শূন্য *shmem_base;
	কাঠামো sbi_domain *dom = sbi_domain_thishart_ptr();

	যদি (!mpxy_shmem_enabled(ms))
		ফেরত SBI_ERR_NO_SHMEM;

	কাঠামো sbi_mpxy_channel *channel = mpxy_find_channel(channel_id, dom);
	যদি (!channel)
		ফেরত SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	যদি (base_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		base_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		ফেরত SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	যদি (!attr_গুনতি || (attr_গুনতি > (mpxy_shmem_আকার / ATTR_আকার)))
		ফেরত SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	end_id = base_attr_id + attr_গুনতি - 1;

	sbi_hart_protection_অস্থায়ী_map_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_আকার);

	/* Standard attributes range check */
	যদি (mpxy_is_std_attr(base_attr_id)) {
		যদি (end_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			যাও out;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_base, &attr_ptr[base_attr_id],
				    attr_গুনতি);
	} অন্যথা {
		/**
		 * Even if the message protocol driver does not provide
		 * read attribute callback, return bad range error instead
		 * of not supported to let client distinguish it from channel
		 * id not supported.
		 * Check the complate range supported for message protocol
		 * attributes. Actual supported attributes will be checked
		 * by the message protocol driver.
		 */
		যদি (!channel->read_attributes ||
				end_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_END) {
			ret = SBI_ERR_BAD_RANGE;
			যাও out;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_attributes(channel,
					       (u32 *)shmem_base,
					       base_attr_id, attr_গুনতি);
	}
out:
	sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)hart_shmem_base(ms), mpxy_shmem_আকার);
	ফেরত ret;
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
স্থির পূর্ণসংখ্যা mpxy_check_write_std_attr(কাঠামো sbi_mpxy_channel *channel,
				     u32 attr_id, u32 attr_val)
{
	কাঠামো sbi_mpxy_channel_attrs *attrs = &channel->attrs;
	পূর্ণসংখ্যা ret = SBI_SUCCESS;

	নির্বাচন(attr_id) {
	ক্ষেত্র SBI_MPXY_ATTR_MSI_CONTROL:
		যদি (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		যদি (attr_val == 1 &&
		    (attrs->msi_info.msi_addr_lo == (u32)INVALID_ADDR) &&
		    (attrs->msi_info.msi_addr_hi == (u32)INVALID_ADDR))
			ret = SBI_ERR_DENIED;
		বিরতি;
	ক্ষেত্র SBI_MPXY_ATTR_MSI_ADDR_LO:
	ক্ষেত্র SBI_MPXY_ATTR_MSI_ADDR_HI:
	ক্ষেত্র SBI_MPXY_ATTR_MSI_তথ্য:
		ret = SBI_SUCCESS;
		বিরতি;
	ক্ষেত্র SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		যদি (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		বিরতি;
	পূর্বনির্ধারিত:
		/** All RO access attributes falls under default */
		ret = SBI_ERR_BAD_RANGE;
	};

	ফেরত ret;
}

/**
 * Write the attribute value
 */
স্থির শূন্য mpxy_write_std_attr(কাঠামো sbi_mpxy_channel *channel, u32 attr_id,
				u32 attr_val)
{
	কাঠামো mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	কাঠামো sbi_mpxy_channel_attrs *attrs = &channel->attrs;

	নির্বাচন(attr_id) {
	ক্ষেত্র SBI_MPXY_ATTR_MSI_CONTROL:
		যদি (ms->msi_avail && attr_val <= 1)
			attrs->msi_control = attr_val;
		বিরতি;
	ক্ষেত্র SBI_MPXY_ATTR_MSI_ADDR_LO:
		যদি (ms->msi_avail)
			attrs->msi_info.msi_addr_lo = attr_val;
		বিরতি;
	ক্ষেত্র SBI_MPXY_ATTR_MSI_ADDR_HI:
		যদি (ms->msi_avail)
			attrs->msi_info.msi_addr_hi = attr_val;
		বিরতি;
	ক্ষেত্র SBI_MPXY_ATTR_MSI_তথ্য:
		যদি (ms->msi_avail)
			attrs->msi_info.msi_তথ্য = attr_val;
		বিরতি;
	ক্ষেত্র SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		যদি (channel->switch_eventsstate && attr_val <= 1) {
			attrs->eventsstate_ctrl = attr_val;
			 /* message protocol callback to enable/disable
			 * events state reporting. */
			channel->switch_eventsstate(attr_val);
		}

		বিরতি;
	};
}

পূর্ণসংখ্যা sbi_mpxy_write_attrs(u32 channel_id, u32 base_attr_id, u32 attr_গুনতি)
{
	কাঠামো mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, end_id, attr_val;
	কাঠামো sbi_mpxy_channel *channel;
	পূর্ণসংখ্যা ret, mem_idx;
	শূন্য *shmem_base;
	কাঠামো sbi_domain *dom = sbi_domain_thishart_ptr();

	যদি (!mpxy_shmem_enabled(ms))
		ফেরত SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	যদি (!channel)
		ফেরত SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	যদি (base_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		base_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		ফেরত SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	যদি (!attr_গুনতি || (attr_গুনতি > (mpxy_shmem_আকার / ATTR_আকার)))
		ফেরত SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	end_id = base_attr_id + attr_গুনতি - 1;

	sbi_hart_protection_অস্থায়ী_map_range((unsigned long)shmem_base, mpxy_shmem_আকার);

	mem_ptr = (u32 *)shmem_base;

	যদি (mpxy_is_std_attr(base_attr_id)) {
		যদি (end_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			যাও out;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		জন্য (attr_id = base_attr_id; attr_id <= end_id; attr_id++) {
			attr_val = le32_to_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			যদি (ret)
				যাও out;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		জন্য (attr_id = base_attr_id; attr_id <= end_id; attr_id++) {
			attr_val = le32_to_cpu(mem_ptr[mem_idx++]);
			mpxy_write_std_attr(channel, attr_id, attr_val);
		}
	} অন্যথা {/**
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
		যদি (!channel->write_attributes ||
				end_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_END) {
			ret = SBI_ERR_BAD_RANGE;
			যাও out;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_attributes(channel,
					       (u32 *)shmem_base,
					       base_attr_id, attr_গুনতি);
	}
out:
	sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)shmem_base, mpxy_shmem_আকার);
	ফেরত ret;
}

পূর্ণসংখ্যা sbi_mpxy_send_message(u32 channel_id, u8 বার্তা_id,
			  unsigned long বার্তা_তথ্য_len,
			  unsigned long *resp_তথ্য_len)
{
	কাঠামো mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	কাঠামো sbi_mpxy_channel *channel;
	শূন্য *shmem_base, *resp_buf;
	u32 resp_bufsize;
	কাঠামো sbi_domain *dom = sbi_domain_thishart_ptr();
	পূর্ণসংখ্যা ret;

	যদি (!mpxy_shmem_enabled(ms))
		ফেরত SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	যদি (!channel)
		ফেরত SBI_ERR_NOT_SUPPORTED;

	যদি (resp_তথ্য_len && !channel->send_message_with_response)
		ফেরত SBI_ERR_NOT_SUPPORTED;

	যদি (!resp_তথ্য_len && !channel->send_message_without_response)
		ফেরত SBI_ERR_NOT_SUPPORTED;

	যদি (বার্তা_তথ্য_len > mpxy_shmem_আকার ||
		বার্তা_তথ্য_len > channel->attrs.বার্তা_তথ্য_maxlen)
		ফেরত SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_অস্থায়ী_map_range((unsigned long)shmem_base, mpxy_shmem_আকার);

	যদি (resp_তথ্য_len) {
		resp_buf = shmem_base;
		resp_bufsize = mpxy_shmem_আকার;
		ret = channel->send_message_with_response(channel, বার্তা_id,
							  shmem_base,
							  বার্তা_তথ্য_len,
							  resp_buf,
							  resp_bufsize,
							  resp_তথ্য_len);
	} অন্যথা {
		ret = channel->send_message_without_response(channel, বার্তা_id,
							     shmem_base,
							     বার্তা_তথ্য_len);
	}

	sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)shmem_base, mpxy_shmem_আকার);

	যদি (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		ফেরত ret;
	অন্যথা যদি (ret)
		ফেরত SBI_ERR_FAILED;

	যদি (resp_তথ্য_len &&
	    (*resp_তথ্য_len > mpxy_shmem_আকার ||
	     *resp_তথ্য_len > channel->attrs.বার্তা_তথ্য_maxlen))
		ফেরত SBI_ERR_FAILED;

	ফেরত SBI_SUCCESS;
}

পূর্ণসংখ্যা sbi_mpxy_get_notification_events(u32 channel_id, unsigned long *events_len)
{
	কাঠামো mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	কাঠামো sbi_mpxy_channel *channel;
	শূন্য *eventsbuf, *shmem_base;
	কাঠামো sbi_domain *dom = sbi_domain_thishart_ptr();
	পূর্ণসংখ্যা ret;

	যদি (!mpxy_shmem_enabled(ms))
		ফেরত SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	যদি (!channel || !channel->get_notification_events)
		ফেরত SBI_ERR_NOT_SUPPORTED;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_অস্থায়ী_map_range((unsigned long)shmem_base, mpxy_shmem_আকার);
	eventsbuf = shmem_base;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_আকার,
					       events_len);
	sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)shmem_base, mpxy_shmem_আকার);

	যদি (ret)
		ফেরত ret;

	যদি (*events_len > (mpxy_shmem_আকার - 16))
		ফেরত SBI_ERR_FAILED;

	ফেরত SBI_SUCCESS;
}
