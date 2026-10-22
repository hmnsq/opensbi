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
ساكن بنانشانی لما mpxy_shmem_ناپ = ورقہ_ناپ;

/** List of MPXY proxy channels */
ساكن SBI_LIST_سر(mpxy_channel_list);

/** Invalid Physical Address(all bits 1) */
#تعریف_کرو INVALID_ADDR		(-1UL)

/** MPXY Attribute size in bytes */
#تعریف_کرو ATTR_ناپ		(4)

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
#تعریف_کرو CAP_GET(_c, _m)		کڈھو_میدان(_c, _m)

#تعریف_کرو SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
بنية mpxy_shmem {
	بنانشانی لما shmem_addr_lo;
	بنانشانی لما shmem_addr_hi;
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
#تعریف_کرو sbi_domain_mpxy_state_thishart_ptr()			\
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
ساكن مضمن فراغ *hart_shmem_ادھار(بنية mpxy_state *ms)
{
	أرجع (فراغ *)(بنانشانی لما)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#تعریف_کرو assert_میدان_وتھ(میدان, attr_وتھ)				\
	_Static_assert(							\
		((offsetof(بنية sbi_mpxy_channel_attrs, میدان)) /	\
		 حجم(u32)) == attr_وتھ,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_میدان_وتھ(پیغام_proto_id, SBI_MPXY_ATTR_پیغام_PROT_ID);
assert_میدان_وتھ(پیغام_proto_نسخہ, SBI_MPXY_ATTR_پیغام_PROT_VER);
assert_میدان_وتھ(پیغام_انکڑے_maxlen, SBI_MPXY_ATTR_پیغام_MAX_LEN);
assert_میدان_وتھ(پیغام_send_timeout, SBI_MPXY_ATTR_پیغام_SEND_TIMEOUT);
assert_میدان_وتھ(پیغام_completion_timeout, SBI_MPXY_ATTR_پیغام_COMPLETION_TIMEOUT);
assert_میدان_وتھ(capability, SBI_MPXY_ATTR_CHANNEL_CAPABILITY);
assert_میدان_وتھ(sse_event_id, SBI_MPXY_ATTR_SSE_EVENT_ID);
assert_میدان_وتھ(msi_control, SBI_MPXY_ATTR_MSI_CONTROL);
assert_میدان_وتھ(msi_info.msi_addr_lo, SBI_MPXY_ATTR_MSI_ADDR_LO);
assert_میدان_وتھ(msi_info.msi_addr_hi, SBI_MPXY_ATTR_MSI_ADDR_HI);
assert_میدان_وتھ(msi_info.msi_انکڑے, SBI_MPXY_ATTR_MSI_انکڑے);
assert_میدان_وتھ(eventsstate_ctrl, SBI_MPXY_ATTR_EVENTS_STATE_CONTROL);

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
ساكن بنية sbi_mpxy_channel *mpxy_لبھو_channel(u32 channel_id,
					بنية sbi_domain *dom)
{
	بنية sbi_mpxy_channel *channel;

	sbi_list_for_each_اندراج(channel, &mpxy_channel_list, سر) {
		إذا (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			أرجع channel;
	}

	أرجع عدم;
}

/** Find channel_id in registered channels list */
بنية sbi_mpxy_channel *sbi_mpxy_لبھو_channel_any(u32 channel_id)
{
	بنية sbi_mpxy_channel *channel;

	sbi_list_for_each_اندراج(channel, &mpxy_channel_list, سر) {
		إذا (channel->channel_id == channel_id)
			أرجع channel;
	}

	أرجع عدم;
}

/** Copy attributes word size */
ساكن فراغ mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 گنتی)
{
	صحيح idx;
	لكل (idx = 0; idx < گنتی; idx++)
		outmem[idx] = cpu_منزل_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_دستیاب(فراغ)
{
	أرجع sbi_list_خالی_تھاواں(&mpxy_channel_list) ? false : true;
}

ساكن فراغ mpxy_std_attrs_init(بنية sbi_mpxy_channel *channel)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_انکڑے = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	إذا (channel->send_سنیہا_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_پیغام_WITH_RESP_نقاب);

	إذا (channel->send_سنیہا_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_پیغام_WITHOUT_RESP_نقاب);

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
صحيح sbi_mpxy_رجسٹر_channel(بنية sbi_mpxy_channel *channel)
{
	إذا (!channel)
		أرجع SBI_EINVAL;

	إذا (!channel->owner_domain)
		أرجع SBI_EINVAL;

	إذا (sbi_mpxy_لبھو_channel_any(channel->channel_id))
		أرجع SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	إذا (mpxy_shmem_ناپ < channel->attrs.پیغام_انکڑے_maxlen) {
		mpxy_shmem_ناپ = ROUNDUP(channel->attrs.پیغام_انکڑے_maxlen,
					  ورقہ_ناپ);
	}

	sbi_list_جوڑو_پوچھ(&channel->سر, &mpxy_channel_list);

	أرجع SBI_OK;
}

/** Setup per domain MPXY state data */
ساكن صحيح domain_mpxy_state_تیاری(بنية sbi_domain *dom,
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
		ms->msi_avail = !!sbi_irqchip_لبھو_device_by_caps(SBI_IRQCHIP_CAPS_MSI, عدم);
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
	dmstate.state_ناپ = حجم(بنية mpxy_state *) * sbi_hart_گنتی();
	ret = sbi_domain_رجسٹر_state(&dmstate);
	إذا (ret)
		أرجع ret;

	أرجع sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

بنانشانی لما sbi_mpxy_get_shmem_ناپ(فراغ)
{
	أرجع mpxy_shmem_ناپ;
}

صحيح sbi_mpxy_سیٹ_shmem(بنانشانی لما shmem_phys_lo,
		       بنانشانی لما shmem_phys_hi,
		       بنانشانی لما flags)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	بنانشانی لما *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	إذا (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		أرجع SBI_SUCCESS;
	}

	إذا (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		أرجع SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	إذا (shmem_phys_lo & ~ورقہ_نقاب)
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
		أرجع SBI_ERR_INVALID_پتہ;

	إذا (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_ناپ, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		أرجع SBI_ERR_INVALID_پتہ;

	/** Save the current shmem details in new shmem region */
	إذا (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
		ret_buf = (بنانشانی لما *)(ulong)SHMEM_PHYS_ADDR(shmem_phys_hi,
								  shmem_phys_lo);
		sbi_hart_protection_عارضی_map_range((بنانشانی لما)ret_buf, mpxy_shmem_ناپ);
		ret_buf[0] = cpu_منزل_lle(ms->shmem.shmem_addr_lo);
		ret_buf[1] = cpu_منزل_lle(ms->shmem.shmem_addr_hi);
		sbi_hart_protection_عارضی_unmap_range((بنانشانی لما)ret_buf, mpxy_shmem_ناپ);
	}

	/** Setup the new shared memory */
	ms->shmem.shmem_addr_lo = shmem_phys_lo;
	ms->shmem.shmem_addr_hi = shmem_phys_hi;

	أرجع SBI_SUCCESS;
}

صحيح sbi_mpxy_get_channel_ids(u32 شروع_اشاریہ)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 بچیا, returned, max_channelids;
	u32 گرہ_اشاریہ = 0, گرہ_ret = 0;
	بنية sbi_mpxy_channel *channel;
	u32 channels_گنتی = 0;
	u32 *shmem_ادھار;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	sbi_list_for_each_اندراج(channel, &mpxy_channel_list, سر) {
		إذا (mpxy_channel_visible(channel, dom))
			channels_گنتی += 1;
	}

	إذا (شروع_اشاریہ > channels_گنتی)
		أرجع SBI_ERR_INVALID_PARAM;

	shmem_ادھار = hart_shmem_ادھار(ms);
	sbi_hart_protection_عارضی_map_range((بنانشانی لما)hart_shmem_ادھار(ms), mpxy_shmem_ناپ);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_ناپ / حجم(u32)) - 2;
	/* total remaining from the start index */
	بچیا = channels_گنتی - شروع_اشاریہ;
	/* how many can be returned */
	returned = (بچیا > max_channelids)? max_channelids : بچیا;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_اندراج(channel, &mpxy_channel_list, سر) {
		إذا (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			استمر;

		إذا (گرہ_اشاریہ >= شروع_اشاریہ &&
			گرہ_اشاریہ < (شروع_اشاریہ + returned)) {
			shmem_ادھار[2 + گرہ_ret] = cpu_منزل_le32(channel->channel_id);
			گرہ_ret += 1;
		}

		گرہ_اشاریہ += 1;
	}

	/* final remaininig channel ids */
	بچیا = channels_گنتی - (شروع_اشاریہ + returned);

	shmem_ادھار[0] = cpu_منزل_le32(بچیا);
	shmem_ادھار[1] = cpu_منزل_le32(returned);

	sbi_hart_protection_عارضی_unmap_range((بنانشانی لما)hart_shmem_ادھار(ms), mpxy_shmem_ناپ);

	أرجع SBI_SUCCESS;
}

صحيح sbi_mpxy_read_attrs(u32 channel_id, u32 ادھار_attr_id, u32 attr_گنتی)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	صحيح ret = SBI_SUCCESS;
	u32 *attr_ptr, سرا_id;
	فراغ *shmem_ادھار;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	بنية sbi_mpxy_channel *channel = mpxy_لبھو_channel(channel_id, dom);
	إذا (!channel)
		أرجع SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	إذا (ادھار_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		ادھار_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_شروع)
		أرجع SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	إذا (!attr_گنتی || (attr_گنتی > (mpxy_shmem_ناپ / ATTR_ناپ)))
		أرجع SBI_ERR_INVALID_PARAM;

	shmem_ادھار = hart_shmem_ادھار(ms);
	سرا_id = ادھار_attr_id + attr_گنتی - 1;

	sbi_hart_protection_عارضی_map_range((بنانشانی لما)hart_shmem_ادھار(ms), mpxy_shmem_ناپ);

	/* Standard attributes range check */
	إذا (mpxy_is_std_attr(ادھار_attr_id)) {
		إذا (سرا_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			اذهب باہرلا;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_ادھار, &attr_ptr[ادھار_attr_id],
				    attr_گنتی);
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
		إذا (!channel->read_گن ||
				سرا_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_سرا) {
			ret = SBI_ERR_BAD_RANGE;
			اذهب باہرلا;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_گن(channel,
					       (u32 *)shmem_ادھار,
					       ادھار_attr_id, attr_گنتی);
	}
باہرلا:
	sbi_hart_protection_عارضی_unmap_range((بنانشانی لما)hart_shmem_ادھار(ms), mpxy_shmem_ناپ);
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
	حالة SBI_MPXY_ATTR_MSI_انکڑے:
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
	حالة SBI_MPXY_ATTR_MSI_انکڑے:
		إذا (ms->msi_avail)
			attrs->msi_info.msi_انکڑے = attr_val;
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

صحيح sbi_mpxy_write_attrs(u32 channel_id, u32 ادھار_attr_id, u32 attr_گنتی)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, سرا_id, attr_val;
	بنية sbi_mpxy_channel *channel;
	صحيح ret, mem_idx;
	فراغ *shmem_ادھار;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	channel = mpxy_لبھو_channel(channel_id, dom);
	إذا (!channel)
		أرجع SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	إذا (ادھار_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		ادھار_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_شروع)
		أرجع SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	إذا (!attr_گنتی || (attr_گنتی > (mpxy_shmem_ناپ / ATTR_ناپ)))
		أرجع SBI_ERR_INVALID_PARAM;

	shmem_ادھار = hart_shmem_ادھار(ms);
	سرا_id = ادھار_attr_id + attr_گنتی - 1;

	sbi_hart_protection_عارضی_map_range((بنانشانی لما)shmem_ادھار, mpxy_shmem_ناپ);

	mem_ptr = (u32 *)shmem_ادھار;

	إذا (mpxy_is_std_attr(ادھار_attr_id)) {
		إذا (سرا_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			اذهب باہرلا;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		لكل (attr_id = ادھار_attr_id; attr_id <= سرا_id; attr_id++) {
			attr_val = le32_منزل_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			إذا (ret)
				اذهب باہرلا;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		لكل (attr_id = ادھار_attr_id; attr_id <= سرا_id; attr_id++) {
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
		إذا (!channel->write_گن ||
				سرا_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_سرا) {
			ret = SBI_ERR_BAD_RANGE;
			اذهب باہرلا;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_گن(channel,
					       (u32 *)shmem_ادھار,
					       ادھار_attr_id, attr_گنتی);
	}
باہرلا:
	sbi_hart_protection_عارضی_unmap_range((بنانشانی لما)shmem_ادھار, mpxy_shmem_ناپ);
	أرجع ret;
}

صحيح sbi_mpxy_send_سنیہا(u32 channel_id, u8 پیغام_id,
			  بنانشانی لما پیغام_انکڑے_len,
			  بنانشانی لما *resp_انکڑے_len)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	بنية sbi_mpxy_channel *channel;
	فراغ *shmem_ادھار, *resp_buf;
	u32 resp_bufsize;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	صحيح ret;

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	channel = mpxy_لبھو_channel(channel_id, dom);
	إذا (!channel)
		أرجع SBI_ERR_NOT_SUPPORTED;

	إذا (resp_انکڑے_len && !channel->send_سنیہا_with_response)
		أرجع SBI_ERR_NOT_SUPPORTED;

	إذا (!resp_انکڑے_len && !channel->send_سنیہا_without_response)
		أرجع SBI_ERR_NOT_SUPPORTED;

	إذا (پیغام_انکڑے_len > mpxy_shmem_ناپ ||
		پیغام_انکڑے_len > channel->attrs.پیغام_انکڑے_maxlen)
		أرجع SBI_ERR_INVALID_PARAM;

	shmem_ادھار = hart_shmem_ادھار(ms);
	sbi_hart_protection_عارضی_map_range((بنانشانی لما)shmem_ادھار, mpxy_shmem_ناپ);

	إذا (resp_انکڑے_len) {
		resp_buf = shmem_ادھار;
		resp_bufsize = mpxy_shmem_ناپ;
		ret = channel->send_سنیہا_with_response(channel, پیغام_id,
							  shmem_ادھار,
							  پیغام_انکڑے_len,
							  resp_buf,
							  resp_bufsize,
							  resp_انکڑے_len);
	} وإلا {
		ret = channel->send_سنیہا_without_response(channel, پیغام_id,
							     shmem_ادھار,
							     پیغام_انکڑے_len);
	}

	sbi_hart_protection_عارضی_unmap_range((بنانشانی لما)shmem_ادھار, mpxy_shmem_ناپ);

	إذا (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		أرجع ret;
	وإلا إذا (ret)
		أرجع SBI_ERR_FAILED;

	إذا (resp_انکڑے_len &&
	    (*resp_انکڑے_len > mpxy_shmem_ناپ ||
	     *resp_انکڑے_len > channel->attrs.پیغام_انکڑے_maxlen))
		أرجع SBI_ERR_FAILED;

	أرجع SBI_SUCCESS;
}

صحيح sbi_mpxy_get_notification_events(u32 channel_id, بنانشانی لما *events_len)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	بنية sbi_mpxy_channel *channel;
	فراغ *eventsbuf, *shmem_ادھار;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	صحيح ret;

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	channel = mpxy_لبھو_channel(channel_id, dom);
	إذا (!channel || !channel->get_notification_events)
		أرجع SBI_ERR_NOT_SUPPORTED;

	shmem_ادھار = hart_shmem_ادھار(ms);
	sbi_hart_protection_عارضی_map_range((بنانشانی لما)shmem_ادھار, mpxy_shmem_ناپ);
	eventsbuf = shmem_ادھار;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_ناپ,
					       events_len);
	sbi_hart_protection_عارضی_unmap_range((بنانشانی لما)shmem_ادھار, mpxy_shmem_ناپ);

	إذا (ret)
		أرجع ret;

	إذا (*events_len > (mpxy_shmem_ناپ - 16))
		أرجع SBI_ERR_FAILED;

	أرجع SBI_SUCCESS;
}
