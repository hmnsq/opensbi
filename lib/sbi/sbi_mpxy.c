/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hart_protection.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_irqchip.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_mpxy.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_byteorder.h>

/** Shared memory size across all harts */
ساکن غیرعلامتی طویل mpxy_shmem_سائز = صفحہ_سائز;

/** List of MPXY proxy channels */
ساکن SBI_LIST_سر(mpxy_channel_list);

/** Invalid Physical Address(all bits 1) */
#تعریف_کرو INVALID_ADDR		(-1UL)

/** MPXY Attribute size in bytes */
#تعریف_کرو ATTR_سائز		(4)

/** Channel Capability - MSI */
#تعریف_کرو CAP_MSI_POS		0
#تعریف_کرو CAP_MSI_نقاب		(1U << CAP_MSI_POS)

/** Channel Capability - SSE */
#تعریف_کرو CAP_SSE_POS		1
#تعریف_کرو CAP_SSE_نقاب		(1U << CAP_SSE_POS)

/** Channel Capability - Events State */
#تعریف_کرو CAP_EVENTSSTATE_POS	2
#تعریف_کرو CAP_EVENTSSTATE_نقاب	(1U << CAP_EVENTSSTATE_POS)

/** Channel Capability - Send Message With Response function support */
#تعریف_کرو CAP_SEND_پیغام_WITH_RESP_POS	3
#تعریف_کرو CAP_SEND_پیغام_WITH_RESP_نقاب	(1U << CAP_SEND_پیغام_WITH_RESP_POS)

/** Channel Capability - Send Message Without Response function support */
#تعریف_کرو CAP_SEND_پیغام_WITHOUT_RESP_POS	4
#تعریف_کرو CAP_SEND_پیغام_WITHOUT_RESP_نقاب	(1U << CAP_SEND_پیغام_WITHOUT_RESP_POS)

/** Channel Capability - Get Notification function support */
#تعریف_کرو CAP_GET_NOTIFICATIONS_POS	5
#تعریف_کرو CAP_GET_NOTIFICATIONS_نقاب	(1U << CAP_GET_NOTIFICATIONS_POS)

/** Helpers to enable/disable channel capability bits
 * _c: capability variable
 * _m: capability mask
 */
#تعریف_کرو CAP_ENABLE(_c, _m)	داخل_کرو_میدان(_c, _m, 1)
#تعریف_کرو CAP_DISABLE(_c, _m)	داخل_کرو_میدان(_c, _m, 0)
#تعریف_کرو CAP_GET(_c, _m)		نکالو_میدان(_c, _m)

#تعریف_کرو SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
ساخت mpxy_shmem {
	غیرعلامتی طویل shmem_addr_lo;
	غیرعلامتی طویل shmem_addr_hi;
};

ساخت mpxy_state {
	/* MSI support in MPXY */
	bool msi_avail;
	/* SSE support in MPXY */
	bool sse_avail;
	/* MPXY Shared memory details */
	ساخت mpxy_shmem shmem;
};

ساکن ساخت mpxy_state *sbi_domain_get_mpxy_state(ساخت sbi_domain *dom,
						    u32 hartindex);

/** Macro to obtain the current hart's MPXY state pointer in current domain */
#تعریف_کرو sbi_domain_mpxy_state_thishart_ptr()			\
	sbi_domain_get_mpxy_state(sbi_domain_thishart_ptr(),	\
				  current_hartindex())

/** Disable hart shared memory */
ساکن اندرونی خالی sbi_mpxy_shmem_disable(ساخت mpxy_state *ms)
{
	ms->shmem.shmem_addr_lo = INVALID_ADDR;
	ms->shmem.shmem_addr_hi = INVALID_ADDR;
}

/** Check if shared memory is already setup on hart */
ساکن اندرونی bool mpxy_shmem_enabled(ساخت mpxy_state *ms)
{
	واپس (ms->shmem.shmem_addr_lo == INVALID_ADDR
		&& ms->shmem.shmem_addr_hi == INVALID_ADDR) ?
		false : true;
}

/** Get hart shared memory base address */
ساکن اندرونی خالی *hart_shmem_بنیاد(ساخت mpxy_state *ms)
{
	واپس (خالی *)(غیرعلامتی طویل)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#تعریف_کرو assert_میدان_انحراف(میدان, attr_انحراف)				\
	_Static_assert(							\
		((offsetof(ساخت sbi_mpxy_channel_attrs, میدان)) /	\
		 حجم(u32)) == attr_انحراف,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_میدان_انحراف(پیغام_proto_id, SBI_MPXY_ATTR_پیغام_PROT_ID);
assert_میدان_انحراف(پیغام_proto_نسخہ, SBI_MPXY_ATTR_پیغام_PROT_VER);
assert_میدان_انحراف(پیغام_ڈیٹا_maxlen, SBI_MPXY_ATTR_پیغام_MAX_LEN);
assert_میدان_انحراف(پیغام_send_timeout, SBI_MPXY_ATTR_پیغام_SEND_TIMEOUT);
assert_میدان_انحراف(پیغام_completion_timeout, SBI_MPXY_ATTR_پیغام_COMPLETION_TIMEOUT);
assert_میدان_انحراف(capability, SBI_MPXY_ATTR_CHANNEL_CAPABILITY);
assert_میدان_انحراف(sse_event_id, SBI_MPXY_ATTR_SSE_EVENT_ID);
assert_میدان_انحراف(msi_control, SBI_MPXY_ATTR_MSI_CONTROL);
assert_میدان_انحراف(msi_info.msi_addr_lo, SBI_MPXY_ATTR_MSI_ADDR_LO);
assert_میدان_انحراف(msi_info.msi_addr_hi, SBI_MPXY_ATTR_MSI_ADDR_HI);
assert_میدان_انحراف(msi_info.msi_ڈیٹا, SBI_MPXY_ATTR_MSI_ڈیٹا);
assert_میدان_انحراف(eventsstate_ctrl, SBI_MPXY_ATTR_EVENTS_STATE_CONTROL);

/**
 * Check if the attribute is a standard attribute or
 * a message protocol specific attribute
 * attr_id[31] = 0 for standard
 * attr_id[31] = 1 for message protocol specific
 */
ساکن اندرونی bool mpxy_is_std_attr(u32 attr_id)
{
	واپس (attr_id >> 31) ? false : true;
}

ساکن اندرونی bool mpxy_channel_visible(ساخت sbi_mpxy_channel *channel,
					ساخت sbi_domain *dom)
{
	واپس channel->owner_domain == dom;
}

/** Find channel_id in registered channels list for this domain */
ساکن ساخت sbi_mpxy_channel *mpxy_ڈھونڈو_channel(u32 channel_id,
					ساخت sbi_domain *dom)
{
	ساخت sbi_mpxy_channel *channel;

	sbi_list_for_each_اندراج(channel, &mpxy_channel_list, سر) {
		اگر (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			واپس channel;
	}

	واپس تہی;
}

/** Find channel_id in registered channels list */
ساخت sbi_mpxy_channel *sbi_mpxy_ڈھونڈو_channel_any(u32 channel_id)
{
	ساخت sbi_mpxy_channel *channel;

	sbi_list_for_each_اندراج(channel, &mpxy_channel_list, سر) {
		اگر (channel->channel_id == channel_id)
			واپس channel;
	}

	واپس تہی;
}

/** Copy attributes word size */
ساکن خالی mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 گنتی)
{
	صحیح idx;
	برائے (idx = 0; idx < گنتی; idx++)
		outmem[idx] = cpu_مقصد_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_دستیاب(خالی)
{
	واپس sbi_list_خالی_خانے(&mpxy_channel_list) ? false : true;
}

ساکن خالی mpxy_std_attrs_init(ساخت sbi_mpxy_channel *channel)
{
	ساخت mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_ڈیٹا = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	اگر (channel->send_پیغام_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_پیغام_WITH_RESP_نقاب);

	اگر (channel->send_پیغام_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_پیغام_WITHOUT_RESP_نقاب);

	اگر (channel->get_notification_events) {
		capability = CAP_ENABLE(capability, CAP_GET_NOTIFICATIONS_نقاب);
		/**
		 * Check if MSI or SSE available for notification interrrupt.
		 * Priority given to MSI if both MSI and SSE are avaialble.
		 */
		اگر (ms->msi_avail)
			capability = CAP_ENABLE(capability, CAP_MSI_نقاب);
		ورنہ اگر (ms->sse_avail) {
			capability = CAP_ENABLE(capability, CAP_SSE_نقاب);
			/* TODO: Assign SSE EVENT_ID for the channel */
		}

		/**
		 * switch_eventstate callback support means support for events
		 * state reporting supoprt. Enable events state reporting in
		 * channel capability.
		 */
		اگر (channel->switch_eventsstate)
			capability = CAP_ENABLE(capability, CAP_EVENTSSTATE_نقاب);
	}

	channel->attrs.capability = capability;
}

/**
 * Register a channel with MPXY framework.
 * Called by message protocol drivers
 */
صحیح sbi_mpxy_رجسٹر_channel(ساخت sbi_mpxy_channel *channel)
{
	اگر (!channel)
		واپس SBI_EINVAL;

	اگر (!channel->owner_domain)
		واپس SBI_EINVAL;

	اگر (sbi_mpxy_ڈھونڈو_channel_any(channel->channel_id))
		واپس SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	اگر (mpxy_shmem_سائز < channel->attrs.پیغام_ڈیٹا_maxlen) {
		mpxy_shmem_سائز = ROUNDUP(channel->attrs.پیغام_ڈیٹا_maxlen,
					  صفحہ_سائز);
	}

	sbi_list_جوڑو_دم(&channel->سر, &mpxy_channel_list);

	واپس SBI_OK;
}

/** Setup per domain MPXY state data */
ساکن صحیح domain_mpxy_state_تیاری(ساخت sbi_domain *dom,
					ساخت sbi_domain_state *state,
					خالی *state_ptr)
{
	ساخت mpxy_state **dom_hartindex_مقصد_mpxy_state_جدول = state_ptr;
	ساخت mpxy_state *ms;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ms = sbi_zalloc(حجم(*ms));
		اگر (!ms)
			واپس SBI_ENOMEM;

		/*
		 * TODO: Proper support for checking sse support from
		 * platform. Currently disable sse and use polling
		 */
		ms->msi_avail = !!sbi_irqchip_ڈھونڈو_device_by_caps(SBI_IRQCHIP_CAPS_MSI, تہی);
		ms->sse_avail = false;

		sbi_mpxy_shmem_disable(ms);

		dom_hartindex_مقصد_mpxy_state_جدول[i] = ms;
	}

	واپس 0;
}

/** Cleanup per domain MPXY state data */
ساکن خالی domain_mpxy_state_cleanup(ساخت sbi_domain *dom,
					   ساخت sbi_domain_state *state,
					   خالی *state_ptr)
{
	ساخت mpxy_state **dom_hartindex_مقصد_mpxy_state_جدول = state_ptr;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts)
		sbi_free(dom_hartindex_مقصد_mpxy_state_جدول[i]);
}

ساکن ساخت sbi_domain_state dmstate = {
	.state_تیاری = domain_mpxy_state_تیاری,
	.state_cleanup = domain_mpxy_state_cleanup,
};

/**
 * Get per-domain MPXY state pointer for a given domain and HART index
 * @param dom pointer to domain
 * @param hartindex the HART index
 *
 * @return per-domain MPXY state pointer for given HART index
 */
ساکن ساخت mpxy_state *sbi_domain_get_mpxy_state(ساخت sbi_domain *dom,
						    u32 hartindex)
{
	ساخت mpxy_state **dom_hartindex_مقصد_mpxy_state_جدول;

	dom_hartindex_مقصد_mpxy_state_جدول = sbi_domain_state_ptr(dom, &dmstate);
	اگر (!dom_hartindex_مقصد_mpxy_state_جدول ||
	    !sbi_hartindex_valid(hartindex))
		واپس تہی;

	واپس dom_hartindex_مقصد_mpxy_state_جدول[hartindex];
}

صحیح sbi_mpxy_init(ساخت sbi_scratch *scratch)
{
	صحیح ret;

	/**
	 * Allocate per-domain and per-hart MPXY state data.
	 * The data type is "struct mpxy_state **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one() and
	 * domain_mpxy_state_setup(). Calculate needed size of memory space
	 * here.
	 */
	dmstate.state_سائز = حجم(ساخت mpxy_state *) * sbi_hart_گنتی();
	ret = sbi_domain_رجسٹر_state(&dmstate);
	اگر (ret)
		واپس ret;

	واپس sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

غیرعلامتی طویل sbi_mpxy_get_shmem_سائز(خالی)
{
	واپس mpxy_shmem_سائز;
}

صحیح sbi_mpxy_سیٹ_shmem(غیرعلامتی طویل shmem_phys_lo,
		       غیرعلامتی طویل shmem_phys_hi,
		       غیرعلامتی طویل flags)
{
	ساخت mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	غیرعلامتی طویل *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	اگر (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		واپس SBI_SUCCESS;
	}

	اگر (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		واپس SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	اگر (shmem_phys_lo & ~صفحہ_نقاب)
		واپس SBI_ERR_INVALID_PARAM;

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
	اگر (shmem_phys_hi)
		واپس SBI_ERR_INVALID_پتہ;

	اگر (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_سائز, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		واپس SBI_ERR_INVALID_پتہ;

	/** Save the current shmem details in new shmem region */
	اگر (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
		ret_buf = (غیرعلامتی طویل *)(ulong)SHMEM_PHYS_ADDR(shmem_phys_hi,
								  shmem_phys_lo);
		sbi_hart_protection_عارضی_map_range((غیرعلامتی طویل)ret_buf, mpxy_shmem_سائز);
		ret_buf[0] = cpu_مقصد_lle(ms->shmem.shmem_addr_lo);
		ret_buf[1] = cpu_مقصد_lle(ms->shmem.shmem_addr_hi);
		sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)ret_buf, mpxy_shmem_سائز);
	}

	/** Setup the new shared memory */
	ms->shmem.shmem_addr_lo = shmem_phys_lo;
	ms->shmem.shmem_addr_hi = shmem_phys_hi;

	واپس SBI_SUCCESS;
}

صحیح sbi_mpxy_get_channel_ids(u32 آغاز_اشاریہ)
{
	ساخت mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 بقایا, returned, max_channelids;
	u32 گرہ_اشاریہ = 0, گرہ_ret = 0;
	ساخت sbi_mpxy_channel *channel;
	u32 channels_گنتی = 0;
	u32 *shmem_بنیاد;
	ساخت sbi_domain *dom = sbi_domain_thishart_ptr();

	اگر (!mpxy_shmem_enabled(ms))
		واپس SBI_ERR_NO_SHMEM;

	sbi_list_for_each_اندراج(channel, &mpxy_channel_list, سر) {
		اگر (mpxy_channel_visible(channel, dom))
			channels_گنتی += 1;
	}

	اگر (آغاز_اشاریہ > channels_گنتی)
		واپس SBI_ERR_INVALID_PARAM;

	shmem_بنیاد = hart_shmem_بنیاد(ms);
	sbi_hart_protection_عارضی_map_range((غیرعلامتی طویل)hart_shmem_بنیاد(ms), mpxy_shmem_سائز);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_سائز / حجم(u32)) - 2;
	/* total remaining from the start index */
	بقایا = channels_گنتی - آغاز_اشاریہ;
	/* how many can be returned */
	returned = (بقایا > max_channelids)? max_channelids : بقایا;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_اندراج(channel, &mpxy_channel_list, سر) {
		اگر (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			جاری;

		اگر (گرہ_اشاریہ >= آغاز_اشاریہ &&
			گرہ_اشاریہ < (آغاز_اشاریہ + returned)) {
			shmem_بنیاد[2 + گرہ_ret] = cpu_مقصد_le32(channel->channel_id);
			گرہ_ret += 1;
		}

		گرہ_اشاریہ += 1;
	}

	/* final remaininig channel ids */
	بقایا = channels_گنتی - (آغاز_اشاریہ + returned);

	shmem_بنیاد[0] = cpu_مقصد_le32(بقایا);
	shmem_بنیاد[1] = cpu_مقصد_le32(returned);

	sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)hart_shmem_بنیاد(ms), mpxy_shmem_سائز);

	واپس SBI_SUCCESS;
}

صحیح sbi_mpxy_read_attrs(u32 channel_id, u32 بنیاد_attr_id, u32 attr_گنتی)
{
	ساخت mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	صحیح ret = SBI_SUCCESS;
	u32 *attr_ptr, انتہا_id;
	خالی *shmem_بنیاد;
	ساخت sbi_domain *dom = sbi_domain_thishart_ptr();

	اگر (!mpxy_shmem_enabled(ms))
		واپس SBI_ERR_NO_SHMEM;

	ساخت sbi_mpxy_channel *channel = mpxy_ڈھونڈو_channel(channel_id, dom);
	اگر (!channel)
		واپس SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	اگر (بنیاد_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		بنیاد_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_آغاز)
		واپس SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	اگر (!attr_گنتی || (attr_گنتی > (mpxy_shmem_سائز / ATTR_سائز)))
		واپس SBI_ERR_INVALID_PARAM;

	shmem_بنیاد = hart_shmem_بنیاد(ms);
	انتہا_id = بنیاد_attr_id + attr_گنتی - 1;

	sbi_hart_protection_عارضی_map_range((غیرعلامتی طویل)hart_shmem_بنیاد(ms), mpxy_shmem_سائز);

	/* Standard attributes range check */
	اگر (mpxy_is_std_attr(بنیاد_attr_id)) {
		اگر (انتہا_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			جاؤ برآمد;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_بنیاد, &attr_ptr[بنیاد_attr_id],
				    attr_گنتی);
	} ورنہ {
		/**
		 * Even if the message protocol driver does not provide
		 * read attribute callback, return bad range error instead
		 * of not supported to let client distinguish it from channel
		 * id not supported.
		 * Check the complate range supported for message protocol
		 * attributes. Actual supported attributes will be checked
		 * by the message protocol driver.
		 */
		اگر (!channel->read_اوصاف ||
				انتہا_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_انتہا) {
			ret = SBI_ERR_BAD_RANGE;
			جاؤ برآمد;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_اوصاف(channel,
					       (u32 *)shmem_بنیاد,
					       بنیاد_attr_id, attr_گنتی);
	}
برآمد:
	sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)hart_shmem_بنیاد(ms), mpxy_shmem_سائز);
	واپس ret;
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
ساکن صحیح mpxy_check_write_std_attr(ساخت sbi_mpxy_channel *channel,
				     u32 attr_id, u32 attr_val)
{
	ساخت sbi_mpxy_channel_attrs *attrs = &channel->attrs;
	صحیح ret = SBI_SUCCESS;

	انتخاب(attr_id) {
	حالت SBI_MPXY_ATTR_MSI_CONTROL:
		اگر (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		اگر (attr_val == 1 &&
		    (attrs->msi_info.msi_addr_lo == (u32)INVALID_ADDR) &&
		    (attrs->msi_info.msi_addr_hi == (u32)INVALID_ADDR))
			ret = SBI_ERR_DENIED;
		توڑو;
	حالت SBI_MPXY_ATTR_MSI_ADDR_LO:
	حالت SBI_MPXY_ATTR_MSI_ADDR_HI:
	حالت SBI_MPXY_ATTR_MSI_ڈیٹا:
		ret = SBI_SUCCESS;
		توڑو;
	حالت SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		اگر (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		توڑو;
	طےشدہ:
		/** All RO access attributes falls under default */
		ret = SBI_ERR_BAD_RANGE;
	};

	واپس ret;
}

/**
 * Write the attribute value
 */
ساکن خالی mpxy_write_std_attr(ساخت sbi_mpxy_channel *channel, u32 attr_id,
				u32 attr_val)
{
	ساخت mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ساخت sbi_mpxy_channel_attrs *attrs = &channel->attrs;

	انتخاب(attr_id) {
	حالت SBI_MPXY_ATTR_MSI_CONTROL:
		اگر (ms->msi_avail && attr_val <= 1)
			attrs->msi_control = attr_val;
		توڑو;
	حالت SBI_MPXY_ATTR_MSI_ADDR_LO:
		اگر (ms->msi_avail)
			attrs->msi_info.msi_addr_lo = attr_val;
		توڑو;
	حالت SBI_MPXY_ATTR_MSI_ADDR_HI:
		اگر (ms->msi_avail)
			attrs->msi_info.msi_addr_hi = attr_val;
		توڑو;
	حالت SBI_MPXY_ATTR_MSI_ڈیٹا:
		اگر (ms->msi_avail)
			attrs->msi_info.msi_ڈیٹا = attr_val;
		توڑو;
	حالت SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		اگر (channel->switch_eventsstate && attr_val <= 1) {
			attrs->eventsstate_ctrl = attr_val;
			 /* message protocol callback to enable/disable
			 * events state reporting. */
			channel->switch_eventsstate(attr_val);
		}

		توڑو;
	};
}

صحیح sbi_mpxy_write_attrs(u32 channel_id, u32 بنیاد_attr_id, u32 attr_گنتی)
{
	ساخت mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, انتہا_id, attr_val;
	ساخت sbi_mpxy_channel *channel;
	صحیح ret, mem_idx;
	خالی *shmem_بنیاد;
	ساخت sbi_domain *dom = sbi_domain_thishart_ptr();

	اگر (!mpxy_shmem_enabled(ms))
		واپس SBI_ERR_NO_SHMEM;

	channel = mpxy_ڈھونڈو_channel(channel_id, dom);
	اگر (!channel)
		واپس SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	اگر (بنیاد_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		بنیاد_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_آغاز)
		واپس SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	اگر (!attr_گنتی || (attr_گنتی > (mpxy_shmem_سائز / ATTR_سائز)))
		واپس SBI_ERR_INVALID_PARAM;

	shmem_بنیاد = hart_shmem_بنیاد(ms);
	انتہا_id = بنیاد_attr_id + attr_گنتی - 1;

	sbi_hart_protection_عارضی_map_range((غیرعلامتی طویل)shmem_بنیاد, mpxy_shmem_سائز);

	mem_ptr = (u32 *)shmem_بنیاد;

	اگر (mpxy_is_std_attr(بنیاد_attr_id)) {
		اگر (انتہا_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			جاؤ برآمد;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		برائے (attr_id = بنیاد_attr_id; attr_id <= انتہا_id; attr_id++) {
			attr_val = le32_مقصد_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			اگر (ret)
				جاؤ برآمد;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		برائے (attr_id = بنیاد_attr_id; attr_id <= انتہا_id; attr_id++) {
			attr_val = le32_مقصد_cpu(mem_ptr[mem_idx++]);
			mpxy_write_std_attr(channel, attr_id, attr_val);
		}
	} ورنہ {/**
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
		اگر (!channel->write_اوصاف ||
				انتہا_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_انتہا) {
			ret = SBI_ERR_BAD_RANGE;
			جاؤ برآمد;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_اوصاف(channel,
					       (u32 *)shmem_بنیاد,
					       بنیاد_attr_id, attr_گنتی);
	}
برآمد:
	sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)shmem_بنیاد, mpxy_shmem_سائز);
	واپس ret;
}

صحیح sbi_mpxy_send_پیغام(u32 channel_id, u8 پیغام_id,
			  غیرعلامتی طویل پیغام_ڈیٹا_len,
			  غیرعلامتی طویل *resp_ڈیٹا_len)
{
	ساخت mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ساخت sbi_mpxy_channel *channel;
	خالی *shmem_بنیاد, *resp_buf;
	u32 resp_bufsize;
	ساخت sbi_domain *dom = sbi_domain_thishart_ptr();
	صحیح ret;

	اگر (!mpxy_shmem_enabled(ms))
		واپس SBI_ERR_NO_SHMEM;

	channel = mpxy_ڈھونڈو_channel(channel_id, dom);
	اگر (!channel)
		واپس SBI_ERR_NOT_SUPPORTED;

	اگر (resp_ڈیٹا_len && !channel->send_پیغام_with_response)
		واپس SBI_ERR_NOT_SUPPORTED;

	اگر (!resp_ڈیٹا_len && !channel->send_پیغام_without_response)
		واپس SBI_ERR_NOT_SUPPORTED;

	اگر (پیغام_ڈیٹا_len > mpxy_shmem_سائز ||
		پیغام_ڈیٹا_len > channel->attrs.پیغام_ڈیٹا_maxlen)
		واپس SBI_ERR_INVALID_PARAM;

	shmem_بنیاد = hart_shmem_بنیاد(ms);
	sbi_hart_protection_عارضی_map_range((غیرعلامتی طویل)shmem_بنیاد, mpxy_shmem_سائز);

	اگر (resp_ڈیٹا_len) {
		resp_buf = shmem_بنیاد;
		resp_bufsize = mpxy_shmem_سائز;
		ret = channel->send_پیغام_with_response(channel, پیغام_id,
							  shmem_بنیاد,
							  پیغام_ڈیٹا_len,
							  resp_buf,
							  resp_bufsize,
							  resp_ڈیٹا_len);
	} ورنہ {
		ret = channel->send_پیغام_without_response(channel, پیغام_id,
							     shmem_بنیاد,
							     پیغام_ڈیٹا_len);
	}

	sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)shmem_بنیاد, mpxy_shmem_سائز);

	اگر (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		واپس ret;
	ورنہ اگر (ret)
		واپس SBI_ERR_FAILED;

	اگر (resp_ڈیٹا_len &&
	    (*resp_ڈیٹا_len > mpxy_shmem_سائز ||
	     *resp_ڈیٹا_len > channel->attrs.پیغام_ڈیٹا_maxlen))
		واپس SBI_ERR_FAILED;

	واپس SBI_SUCCESS;
}

صحیح sbi_mpxy_get_notification_events(u32 channel_id, غیرعلامتی طویل *events_len)
{
	ساخت mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	ساخت sbi_mpxy_channel *channel;
	خالی *eventsbuf, *shmem_بنیاد;
	ساخت sbi_domain *dom = sbi_domain_thishart_ptr();
	صحیح ret;

	اگر (!mpxy_shmem_enabled(ms))
		واپس SBI_ERR_NO_SHMEM;

	channel = mpxy_ڈھونڈو_channel(channel_id, dom);
	اگر (!channel || !channel->get_notification_events)
		واپس SBI_ERR_NOT_SUPPORTED;

	shmem_بنیاد = hart_shmem_بنیاد(ms);
	sbi_hart_protection_عارضی_map_range((غیرعلامتی طویل)shmem_بنیاد, mpxy_shmem_سائز);
	eventsbuf = shmem_بنیاد;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_سائز,
					       events_len);
	sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)shmem_بنیاد, mpxy_shmem_سائز);

	اگر (ret)
		واپس ret;

	اگر (*events_len > (mpxy_shmem_سائز - 16))
		واپس SBI_ERR_FAILED;

	واپس SBI_SUCCESS;
}
