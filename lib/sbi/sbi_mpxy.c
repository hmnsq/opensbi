/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_hart_protection.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_irqchip.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_mpxy.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_byteorder.h>

/** Shared memory size across all harts */
ساكن بنانشان ڊگهو mpxy_shmem_سائيز = صفحو_سائيز;

/** List of MPXY proxy channels */
ساكن SBI_LIST_سر(mpxy_channel_list);

/** Invalid Physical Address(all bits 1) */
#وصف_ڪريو INVALID_ADDR		(-1UL)

/** MPXY Attribute size in bytes */
#وصف_ڪريو ATTR_سائيز		(4)

/** Channel Capability - MSI */
#وصف_ڪريو CAP_MSI_POS		0
#وصف_ڪريو CAP_MSI_نقاب		(1U << CAP_MSI_POS)

/** Channel Capability - SSE */
#وصف_ڪريو CAP_SSE_POS		1
#وصف_ڪريو CAP_SSE_نقاب		(1U << CAP_SSE_POS)

/** Channel Capability - Events State */
#وصف_ڪريو CAP_EVENTSSTATE_POS	2
#وصف_ڪريو CAP_EVENTSSTATE_نقاب	(1U << CAP_EVENTSSTATE_POS)

/** Channel Capability - Send Message With Response function support */
#وصف_ڪريو CAP_SEND_نياپو_WITH_RESP_POS	3
#وصف_ڪريو CAP_SEND_نياپو_WITH_RESP_نقاب	(1U << CAP_SEND_نياپو_WITH_RESP_POS)

/** Channel Capability - Send Message Without Response function support */
#وصف_ڪريو CAP_SEND_نياپو_WITHOUT_RESP_POS	4
#وصف_ڪريو CAP_SEND_نياپو_WITHOUT_RESP_نقاب	(1U << CAP_SEND_نياپو_WITHOUT_RESP_POS)

/** Channel Capability - Get Notification function support */
#وصف_ڪريو CAP_GET_NOTIFICATIONS_POS	5
#وصف_ڪريو CAP_GET_NOTIFICATIONS_نقاب	(1U << CAP_GET_NOTIFICATIONS_POS)

/** Helpers to enable/disable channel capability bits
 * _c: capability variable
 * _m: capability mask
 */
#وصف_ڪريو CAP_ENABLE(_c, _m)	وجهو_ميدان(_c, _m, 1)
#وصف_ڪريو CAP_DISABLE(_c, _m)	وجهو_ميدان(_c, _m, 0)
#وصف_ڪريو CAP_GET(_c, _m)		ڪڍو_ميدان(_c, _m)

#وصف_ڪريو SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
بنية mpxy_shmem {
	بنانشان ڊگهو shmem_addr_lo;
	بنانشان ڊگهو shmem_addr_hi;
};

بنية mpxy_state {
	/* MSI support in MPXY */
	bool msi_avail;
	/* SSE support in MPXY */
	bool sse_avail;
	/* MPXY Shared memory details */
	بنية mpxy_shmem shmem;
};

ساكن بنية mpxy_state *sbi_domain_get_mpxy_state(بنية sbi_domain *dom,
						    u32 hartindex);

/** Macro to obtain the current hart's MPXY state pointer in current domain */
#وصف_ڪريو sbi_domain_mpxy_state_thishart_ptr()			\
	sbi_domain_get_mpxy_state(sbi_domain_thishart_ptr(),	\
				  current_hartindex())

/** Disable hart shared memory */
ساكن مضمن فراغ sbi_mpxy_shmem_disable(بنية mpxy_state *ms)
{
	ms->shmem.shmem_addr_lo = INVALID_ADDR;
	ms->shmem.shmem_addr_hi = INVALID_ADDR;
}

/** Check if shared memory is already setup on hart */
ساكن مضمن bool mpxy_shmem_enabled(بنية mpxy_state *ms)
{
	أرجع (ms->shmem.shmem_addr_lo == INVALID_ADDR
		&& ms->shmem.shmem_addr_hi == INVALID_ADDR) ?
		false : true;
}

/** Get hart shared memory base address */
ساكن مضمن فراغ *hart_shmem_بنياد(بنية mpxy_state *ms)
{
	أرجع (فراغ *)(بنانشان ڊگهو)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#وصف_ڪريو assert_ميدان_وٿي(ميدان, attr_وٿي)				\
	_Static_assert(							\
		((offsetof(بنية sbi_mpxy_channel_attrs, ميدان)) /	\
		 حجم(u32)) == attr_وٿي,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_ميدان_وٿي(نياپو_proto_id, SBI_MPXY_ATTR_نياپو_PROT_ID);
assert_ميدان_وٿي(نياپو_proto_نسخو, SBI_MPXY_ATTR_نياپو_PROT_VER);
assert_ميدان_وٿي(نياپو_معلومات_maxlen, SBI_MPXY_ATTR_نياپو_MAX_LEN);
assert_ميدان_وٿي(نياپو_send_timeout, SBI_MPXY_ATTR_نياپو_SEND_TIMEOUT);
assert_ميدان_وٿي(نياپو_completion_timeout, SBI_MPXY_ATTR_نياپو_COMPLETION_TIMEOUT);
assert_ميدان_وٿي(capability, SBI_MPXY_ATTR_CHANNEL_CAPABILITY);
assert_ميدان_وٿي(sse_event_id, SBI_MPXY_ATTR_SSE_EVENT_ID);
assert_ميدان_وٿي(msi_control, SBI_MPXY_ATTR_MSI_CONTROL);
assert_ميدان_وٿي(msi_info.msi_addr_lo, SBI_MPXY_ATTR_MSI_ADDR_LO);
assert_ميدان_وٿي(msi_info.msi_addr_hi, SBI_MPXY_ATTR_MSI_ADDR_HI);
assert_ميدان_وٿي(msi_info.msi_معلومات, SBI_MPXY_ATTR_MSI_معلومات);
assert_ميدان_وٿي(eventsstate_ctrl, SBI_MPXY_ATTR_EVENTS_STATE_CONTROL);

/**
 * Check if the attribute is a standard attribute or
 * a message protocol specific attribute
 * attr_id[31] = 0 for standard
 * attr_id[31] = 1 for message protocol specific
 */
ساكن مضمن bool mpxy_is_std_attr(u32 attr_id)
{
	أرجع (attr_id >> 31) ? false : true;
}

ساكن مضمن bool mpxy_channel_visible(بنية sbi_mpxy_channel *channel,
					بنية sbi_domain *dom)
{
	أرجع channel->owner_domain == dom;
}

/** Find channel_id in registered channels list for this domain */
ساكن بنية sbi_mpxy_channel *mpxy_ڳوليو_channel(u32 channel_id,
					بنية sbi_domain *dom)
{
	بنية sbi_mpxy_channel *channel;

	sbi_list_for_each_داخلا(channel, &mpxy_channel_list, سر) {
		إذا (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			أرجع channel;
	}

	أرجع عدم;
}

/** Find channel_id in registered channels list */
بنية sbi_mpxy_channel *sbi_mpxy_ڳوليو_channel_any(u32 channel_id)
{
	بنية sbi_mpxy_channel *channel;

	sbi_list_for_each_داخلا(channel, &mpxy_channel_list, سر) {
		إذا (channel->channel_id == channel_id)
			أرجع channel;
	}

	أرجع عدم;
}

/** Copy attributes word size */
ساكن فراغ mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 ڳڻپ)
{
	صحيح idx;
	لكل (idx = 0; idx < ڳڻپ; idx++)
		outmem[idx] = cpu_منزل_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_موجود(فراغ)
{
	أرجع sbi_list_خالي_جايون(&mpxy_channel_list) ? false : true;
}

ساكن فراغ mpxy_std_attrs_init(بنية sbi_mpxy_channel *channel)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_معلومات = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	إذا (channel->send_پيغام_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_نياپو_WITH_RESP_نقاب);

	إذا (channel->send_پيغام_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_نياپو_WITHOUT_RESP_نقاب);

	إذا (channel->get_notification_events) {
		capability = CAP_ENABLE(capability, CAP_GET_NOTIFICATIONS_نقاب);
		/**
		 * Check if MSI or SSE available for notification interrrupt.
		 * Priority given to MSI if both MSI and SSE are avaialble.
		 */
		إذا (ms->msi_avail)
			capability = CAP_ENABLE(capability, CAP_MSI_نقاب);
		وإلا إذا (ms->sse_avail) {
			capability = CAP_ENABLE(capability, CAP_SSE_نقاب);
			/* TODO: Assign SSE EVENT_ID for the channel */
		}

		/**
		 * switch_eventstate callback support means support for events
		 * state reporting supoprt. Enable events state reporting in
		 * channel capability.
		 */
		إذا (channel->switch_eventsstate)
			capability = CAP_ENABLE(capability, CAP_EVENTSSTATE_نقاب);
	}

	channel->attrs.capability = capability;
}

/**
 * Register a channel with MPXY framework.
 * Called by message protocol drivers
 */
صحيح sbi_mpxy_رجسٽر_channel(بنية sbi_mpxy_channel *channel)
{
	إذا (!channel)
		أرجع SBI_EINVAL;

	إذا (!channel->owner_domain)
		أرجع SBI_EINVAL;

	إذا (sbi_mpxy_ڳوليو_channel_any(channel->channel_id))
		أرجع SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	إذا (mpxy_shmem_سائيز < channel->attrs.نياپو_معلومات_maxlen) {
		mpxy_shmem_سائيز = ROUNDUP(channel->attrs.نياپو_معلومات_maxlen,
					  صفحو_سائيز);
	}

	sbi_list_جوڙيو_پڇ(&channel->سر, &mpxy_channel_list);

	أرجع SBI_OK;
}

/** Setup per domain MPXY state data */
ساكن صحيح domain_mpxy_state_تياري(بنية sbi_domain *dom,
					بنية sbi_domain_state *state,
					فراغ *state_ptr)
{
	بنية mpxy_state **dom_hartindex_منزل_mpxy_state_جدول = state_ptr;
	بنية mpxy_state *ms;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ms = sbi_zalloc(حجم(*ms));
		إذا (!ms)
			أرجع SBI_ENOMEM;

		/*
		 * TODO: Proper support for checking sse support from
		 * platform. Currently disable sse and use polling
		 */
		ms->msi_avail = !!sbi_irqchip_ڳوليو_device_by_caps(SBI_IRQCHIP_CAPS_MSI, عدم);
		ms->sse_avail = false;

		sbi_mpxy_shmem_disable(ms);

		dom_hartindex_منزل_mpxy_state_جدول[i] = ms;
	}

	أرجع 0;
}

/** Cleanup per domain MPXY state data */
ساكن فراغ domain_mpxy_state_cleanup(بنية sbi_domain *dom,
					   بنية sbi_domain_state *state,
					   فراغ *state_ptr)
{
	بنية mpxy_state **dom_hartindex_منزل_mpxy_state_جدول = state_ptr;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts)
		sbi_free(dom_hartindex_منزل_mpxy_state_جدول[i]);
}

ساكن بنية sbi_domain_state dmstate = {
	.state_تياري = domain_mpxy_state_تياري,
	.state_cleanup = domain_mpxy_state_cleanup,
};

/**
 * Get per-domain MPXY state pointer for a given domain and HART index
 * @param dom pointer to domain
 * @param hartindex the HART index
 *
 * @return per-domain MPXY state pointer for given HART index
 */
ساكن بنية mpxy_state *sbi_domain_get_mpxy_state(بنية sbi_domain *dom,
						    u32 hartindex)
{
	بنية mpxy_state **dom_hartindex_منزل_mpxy_state_جدول;

	dom_hartindex_منزل_mpxy_state_جدول = sbi_domain_state_ptr(dom, &dmstate);
	إذا (!dom_hartindex_منزل_mpxy_state_جدول ||
	    !sbi_hartindex_valid(hartindex))
		أرجع عدم;

	أرجع dom_hartindex_منزل_mpxy_state_جدول[hartindex];
}

صحيح sbi_mpxy_init(بنية sbi_scratch *scratch)
{
	صحيح ret;

	/**
	 * Allocate per-domain and per-hart MPXY state data.
	 * The data type is "struct mpxy_state **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one() and
	 * domain_mpxy_state_setup(). Calculate needed size of memory space
	 * here.
	 */
	dmstate.state_سائيز = حجم(بنية mpxy_state *) * sbi_hart_ڳڻپ();
	ret = sbi_domain_رجسٽر_state(&dmstate);
	إذا (ret)
		أرجع ret;

	أرجع sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

بنانشان ڊگهو sbi_mpxy_get_shmem_سائيز(فراغ)
{
	أرجع mpxy_shmem_سائيز;
}

صحيح sbi_mpxy_سيٽ_shmem(بنانشان ڊگهو shmem_phys_lo,
		       بنانشان ڊگهو shmem_phys_hi,
		       بنانشان ڊگهو flags)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	بنانشان ڊگهو *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	إذا (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		أرجع SBI_SUCCESS;
	}

	إذا (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		أرجع SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	إذا (shmem_phys_lo & ~صفحو_نقاب)
		أرجع SBI_ERR_INVALID_PARAM;

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
	إذا (shmem_phys_hi)
		أرجع SBI_ERR_INVALID_پتو;

	إذا (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_سائيز, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		أرجع SBI_ERR_INVALID_پتو;

	/** Save the current shmem details in new shmem region */
	إذا (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
		ret_buf = (بنانشان ڊگهو *)(ulong)SHMEM_PHYS_ADDR(shmem_phys_hi,
								  shmem_phys_lo);
		sbi_hart_protection_عارضي_map_range((بنانشان ڊگهو)ret_buf, mpxy_shmem_سائيز);
		ret_buf[0] = cpu_منزل_lle(ms->shmem.shmem_addr_lo);
		ret_buf[1] = cpu_منزل_lle(ms->shmem.shmem_addr_hi);
		sbi_hart_protection_عارضي_unmap_range((بنانشان ڊگهو)ret_buf, mpxy_shmem_سائيز);
	}

	/** Setup the new shared memory */
	ms->shmem.shmem_addr_lo = shmem_phys_lo;
	ms->shmem.shmem_addr_hi = shmem_phys_hi;

	أرجع SBI_SUCCESS;
}

صحيح sbi_mpxy_get_channel_ids(u32 شروعات_اشاريو)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 بچيل, returned, max_channelids;
	u32 ڳنڍ_اشاريو = 0, ڳنڍ_ret = 0;
	بنية sbi_mpxy_channel *channel;
	u32 channels_ڳڻپ = 0;
	u32 *shmem_بنياد;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	sbi_list_for_each_داخلا(channel, &mpxy_channel_list, سر) {
		إذا (mpxy_channel_visible(channel, dom))
			channels_ڳڻپ += 1;
	}

	إذا (شروعات_اشاريو > channels_ڳڻپ)
		أرجع SBI_ERR_INVALID_PARAM;

	shmem_بنياد = hart_shmem_بنياد(ms);
	sbi_hart_protection_عارضي_map_range((بنانشان ڊگهو)hart_shmem_بنياد(ms), mpxy_shmem_سائيز);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_سائيز / حجم(u32)) - 2;
	/* total remaining from the start index */
	بچيل = channels_ڳڻپ - شروعات_اشاريو;
	/* how many can be returned */
	returned = (بچيل > max_channelids)? max_channelids : بچيل;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_داخلا(channel, &mpxy_channel_list, سر) {
		إذا (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			استمر;

		إذا (ڳنڍ_اشاريو >= شروعات_اشاريو &&
			ڳنڍ_اشاريو < (شروعات_اشاريو + returned)) {
			shmem_بنياد[2 + ڳنڍ_ret] = cpu_منزل_le32(channel->channel_id);
			ڳنڍ_ret += 1;
		}

		ڳنڍ_اشاريو += 1;
	}

	/* final remaininig channel ids */
	بچيل = channels_ڳڻپ - (شروعات_اشاريو + returned);

	shmem_بنياد[0] = cpu_منزل_le32(بچيل);
	shmem_بنياد[1] = cpu_منزل_le32(returned);

	sbi_hart_protection_عارضي_unmap_range((بنانشان ڊگهو)hart_shmem_بنياد(ms), mpxy_shmem_سائيز);

	أرجع SBI_SUCCESS;
}

صحيح sbi_mpxy_read_attrs(u32 channel_id, u32 بنياد_attr_id, u32 attr_ڳڻپ)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	صحيح ret = SBI_SUCCESS;
	u32 *attr_ptr, پڇاڙي_id;
	فراغ *shmem_بنياد;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	بنية sbi_mpxy_channel *channel = mpxy_ڳوليو_channel(channel_id, dom);
	إذا (!channel)
		أرجع SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	إذا (بنياد_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		بنياد_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_شروعات)
		أرجع SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	إذا (!attr_ڳڻپ || (attr_ڳڻپ > (mpxy_shmem_سائيز / ATTR_سائيز)))
		أرجع SBI_ERR_INVALID_PARAM;

	shmem_بنياد = hart_shmem_بنياد(ms);
	پڇاڙي_id = بنياد_attr_id + attr_ڳڻپ - 1;

	sbi_hart_protection_عارضي_map_range((بنانشان ڊگهو)hart_shmem_بنياد(ms), mpxy_shmem_سائيز);

	/* Standard attributes range check */
	إذا (mpxy_is_std_attr(بنياد_attr_id)) {
		إذا (پڇاڙي_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			اذهب ٻاهريون;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_بنياد, &attr_ptr[بنياد_attr_id],
				    attr_ڳڻپ);
	} وإلا {
		/**
		 * Even if the message protocol driver does not provide
		 * read attribute callback, return bad range error instead
		 * of not supported to let client distinguish it from channel
		 * id not supported.
		 * Check the complate range supported for message protocol
		 * attributes. Actual supported attributes will be checked
		 * by the message protocol driver.
		 */
		إذا (!channel->read_خاصيتون ||
				پڇاڙي_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_پڇاڙي) {
			ret = SBI_ERR_BAD_RANGE;
			اذهب ٻاهريون;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_خاصيتون(channel,
					       (u32 *)shmem_بنياد,
					       بنياد_attr_id, attr_ڳڻپ);
	}
ٻاهريون:
	sbi_hart_protection_عارضي_unmap_range((بنانشان ڊگهو)hart_shmem_بنياد(ms), mpxy_shmem_سائيز);
	أرجع ret;
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
ساكن صحيح mpxy_check_write_std_attr(بنية sbi_mpxy_channel *channel,
				     u32 attr_id, u32 attr_val)
{
	بنية sbi_mpxy_channel_attrs *attrs = &channel->attrs;
	صحيح ret = SBI_SUCCESS;

	بدل(attr_id) {
	حالة SBI_MPXY_ATTR_MSI_CONTROL:
		إذا (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		إذا (attr_val == 1 &&
		    (attrs->msi_info.msi_addr_lo == (u32)INVALID_ADDR) &&
		    (attrs->msi_info.msi_addr_hi == (u32)INVALID_ADDR))
			ret = SBI_ERR_DENIED;
		اكسر;
	حالة SBI_MPXY_ATTR_MSI_ADDR_LO:
	حالة SBI_MPXY_ATTR_MSI_ADDR_HI:
	حالة SBI_MPXY_ATTR_MSI_معلومات:
		ret = SBI_SUCCESS;
		اكسر;
	حالة SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		إذا (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		اكسر;
	افتراضي:
		/** All RO access attributes falls under default */
		ret = SBI_ERR_BAD_RANGE;
	};

	أرجع ret;
}

/**
 * Write the attribute value
 */
ساكن فراغ mpxy_write_std_attr(بنية sbi_mpxy_channel *channel, u32 attr_id,
				u32 attr_val)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	بنية sbi_mpxy_channel_attrs *attrs = &channel->attrs;

	بدل(attr_id) {
	حالة SBI_MPXY_ATTR_MSI_CONTROL:
		إذا (ms->msi_avail && attr_val <= 1)
			attrs->msi_control = attr_val;
		اكسر;
	حالة SBI_MPXY_ATTR_MSI_ADDR_LO:
		إذا (ms->msi_avail)
			attrs->msi_info.msi_addr_lo = attr_val;
		اكسر;
	حالة SBI_MPXY_ATTR_MSI_ADDR_HI:
		إذا (ms->msi_avail)
			attrs->msi_info.msi_addr_hi = attr_val;
		اكسر;
	حالة SBI_MPXY_ATTR_MSI_معلومات:
		إذا (ms->msi_avail)
			attrs->msi_info.msi_معلومات = attr_val;
		اكسر;
	حالة SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		إذا (channel->switch_eventsstate && attr_val <= 1) {
			attrs->eventsstate_ctrl = attr_val;
			 /* message protocol callback to enable/disable
			 * events state reporting. */
			channel->switch_eventsstate(attr_val);
		}

		اكسر;
	};
}

صحيح sbi_mpxy_write_attrs(u32 channel_id, u32 بنياد_attr_id, u32 attr_ڳڻپ)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, پڇاڙي_id, attr_val;
	بنية sbi_mpxy_channel *channel;
	صحيح ret, mem_idx;
	فراغ *shmem_بنياد;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	channel = mpxy_ڳوليو_channel(channel_id, dom);
	إذا (!channel)
		أرجع SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	إذا (بنياد_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		بنياد_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_شروعات)
		أرجع SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	إذا (!attr_ڳڻپ || (attr_ڳڻپ > (mpxy_shmem_سائيز / ATTR_سائيز)))
		أرجع SBI_ERR_INVALID_PARAM;

	shmem_بنياد = hart_shmem_بنياد(ms);
	پڇاڙي_id = بنياد_attr_id + attr_ڳڻپ - 1;

	sbi_hart_protection_عارضي_map_range((بنانشان ڊگهو)shmem_بنياد, mpxy_shmem_سائيز);

	mem_ptr = (u32 *)shmem_بنياد;

	إذا (mpxy_is_std_attr(بنياد_attr_id)) {
		إذا (پڇاڙي_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			اذهب ٻاهريون;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		لكل (attr_id = بنياد_attr_id; attr_id <= پڇاڙي_id; attr_id++) {
			attr_val = le32_منزل_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			إذا (ret)
				اذهب ٻاهريون;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		لكل (attr_id = بنياد_attr_id; attr_id <= پڇاڙي_id; attr_id++) {
			attr_val = le32_منزل_cpu(mem_ptr[mem_idx++]);
			mpxy_write_std_attr(channel, attr_id, attr_val);
		}
	} وإلا {/**
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
		إذا (!channel->write_خاصيتون ||
				پڇاڙي_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_پڇاڙي) {
			ret = SBI_ERR_BAD_RANGE;
			اذهب ٻاهريون;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_خاصيتون(channel,
					       (u32 *)shmem_بنياد,
					       بنياد_attr_id, attr_ڳڻپ);
	}
ٻاهريون:
	sbi_hart_protection_عارضي_unmap_range((بنانشان ڊگهو)shmem_بنياد, mpxy_shmem_سائيز);
	أرجع ret;
}

صحيح sbi_mpxy_send_پيغام(u32 channel_id, u8 نياپو_id,
			  بنانشان ڊگهو نياپو_معلومات_len,
			  بنانشان ڊگهو *resp_معلومات_len)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	بنية sbi_mpxy_channel *channel;
	فراغ *shmem_بنياد, *resp_buf;
	u32 resp_bufsize;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	صحيح ret;

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	channel = mpxy_ڳوليو_channel(channel_id, dom);
	إذا (!channel)
		أرجع SBI_ERR_NOT_SUPPORTED;

	إذا (resp_معلومات_len && !channel->send_پيغام_with_response)
		أرجع SBI_ERR_NOT_SUPPORTED;

	إذا (!resp_معلومات_len && !channel->send_پيغام_without_response)
		أرجع SBI_ERR_NOT_SUPPORTED;

	إذا (نياپو_معلومات_len > mpxy_shmem_سائيز ||
		نياپو_معلومات_len > channel->attrs.نياپو_معلومات_maxlen)
		أرجع SBI_ERR_INVALID_PARAM;

	shmem_بنياد = hart_shmem_بنياد(ms);
	sbi_hart_protection_عارضي_map_range((بنانشان ڊگهو)shmem_بنياد, mpxy_shmem_سائيز);

	إذا (resp_معلومات_len) {
		resp_buf = shmem_بنياد;
		resp_bufsize = mpxy_shmem_سائيز;
		ret = channel->send_پيغام_with_response(channel, نياپو_id,
							  shmem_بنياد,
							  نياپو_معلومات_len,
							  resp_buf,
							  resp_bufsize,
							  resp_معلومات_len);
	} وإلا {
		ret = channel->send_پيغام_without_response(channel, نياپو_id,
							     shmem_بنياد,
							     نياپو_معلومات_len);
	}

	sbi_hart_protection_عارضي_unmap_range((بنانشان ڊگهو)shmem_بنياد, mpxy_shmem_سائيز);

	إذا (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		أرجع ret;
	وإلا إذا (ret)
		أرجع SBI_ERR_FAILED;

	إذا (resp_معلومات_len &&
	    (*resp_معلومات_len > mpxy_shmem_سائيز ||
	     *resp_معلومات_len > channel->attrs.نياپو_معلومات_maxlen))
		أرجع SBI_ERR_FAILED;

	أرجع SBI_SUCCESS;
}

صحيح sbi_mpxy_get_notification_events(u32 channel_id, بنانشان ڊگهو *events_len)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	بنية sbi_mpxy_channel *channel;
	فراغ *eventsbuf, *shmem_بنياد;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	صحيح ret;

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	channel = mpxy_ڳوليو_channel(channel_id, dom);
	إذا (!channel || !channel->get_notification_events)
		أرجع SBI_ERR_NOT_SUPPORTED;

	shmem_بنياد = hart_shmem_بنياد(ms);
	sbi_hart_protection_عارضي_map_range((بنانشان ڊگهو)shmem_بنياد, mpxy_shmem_سائيز);
	eventsbuf = shmem_بنياد;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_سائيز,
					       events_len);
	sbi_hart_protection_عارضي_unmap_range((بنانشان ڊگهو)shmem_بنياد, mpxy_shmem_سائيز);

	إذا (ret)
		أرجع ret;

	إذا (*events_len > (mpxy_shmem_سائيز - 16))
		أرجع SBI_ERR_FAILED;

	أرجع SBI_SUCCESS;
}
