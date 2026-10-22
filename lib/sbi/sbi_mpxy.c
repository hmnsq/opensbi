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
ساكن بیعلامت دراز mpxy_shmem_اندازه = صفحه_اندازه;

/** List of MPXY proxy channels */
ساكن SBI_LIST_سر(mpxy_channel_list);

/** Invalid Physical Address(all bits 1) */
#تعریف_کن INVALID_ADDR		(-1UL)

/** MPXY Attribute size in bytes */
#تعریف_کن ATTR_اندازه		(4)

/** Channel Capability - MSI */
#تعریف_کن CAP_MSI_POS		0
#تعریف_کن CAP_MSI_ماسک		(1U << CAP_MSI_POS)

/** Channel Capability - SSE */
#تعریف_کن CAP_SSE_POS		1
#تعریف_کن CAP_SSE_ماسک		(1U << CAP_SSE_POS)

/** Channel Capability - Events State */
#تعریف_کن CAP_EVENTSSTATE_POS	2
#تعریف_کن CAP_EVENTSSTATE_ماسک	(1U << CAP_EVENTSSTATE_POS)

/** Channel Capability - Send Message With Response function support */
#تعریف_کن CAP_SEND_پیام_WITH_RESP_POS	3
#تعریف_کن CAP_SEND_پیام_WITH_RESP_ماسک	(1U << CAP_SEND_پیام_WITH_RESP_POS)

/** Channel Capability - Send Message Without Response function support */
#تعریف_کن CAP_SEND_پیام_WITHOUT_RESP_POS	4
#تعریف_کن CAP_SEND_پیام_WITHOUT_RESP_ماسک	(1U << CAP_SEND_پیام_WITHOUT_RESP_POS)

/** Channel Capability - Get Notification function support */
#تعریف_کن CAP_GET_NOTIFICATIONS_POS	5
#تعریف_کن CAP_GET_NOTIFICATIONS_ماسک	(1U << CAP_GET_NOTIFICATIONS_POS)

/** Helpers to enable/disable channel capability bits
 * _c: capability variable
 * _m: capability mask
 */
#تعریف_کن CAP_ENABLE(_c, _m)	درج_فیلد(_c, _m, 1)
#تعریف_کن CAP_DISABLE(_c, _m)	درج_فیلد(_c, _m, 0)
#تعریف_کن CAP_GET(_c, _m)		استخراج_فیلد(_c, _m)

#تعریف_کن SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
بنية mpxy_shmem {
	بیعلامت دراز shmem_addr_lo;
	بیعلامت دراز shmem_addr_hi;
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
#تعریف_کن sbi_domain_mpxy_state_thishart_ptr()			\
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
ساكن مضمن فراغ *hart_shmem_پایه(بنية mpxy_state *ms)
{
	أرجع (فراغ *)(بیعلامت دراز)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#تعریف_کن assert_فیلد_جابه_جایی(فیلد, attr_جابه_جایی)				\
	_Static_assert(							\
		((offsetof(بنية sbi_mpxy_channel_attrs, فیلد)) /	\
		 حجم(u32)) == attr_جابه_جایی,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_فیلد_جابه_جایی(پیام_proto_id, SBI_MPXY_ATTR_پیام_PROT_ID);
assert_فیلد_جابه_جایی(پیام_proto_نسخه, SBI_MPXY_ATTR_پیام_PROT_VER);
assert_فیلد_جابه_جایی(پیام_داده_maxlen, SBI_MPXY_ATTR_پیام_MAX_LEN);
assert_فیلد_جابه_جایی(پیام_send_timeout, SBI_MPXY_ATTR_پیام_SEND_TIMEOUT);
assert_فیلد_جابه_جایی(پیام_completion_timeout, SBI_MPXY_ATTR_پیام_COMPLETION_TIMEOUT);
assert_فیلد_جابه_جایی(capability, SBI_MPXY_ATTR_CHANNEL_CAPABILITY);
assert_فیلد_جابه_جایی(sse_event_id, SBI_MPXY_ATTR_SSE_EVENT_ID);
assert_فیلد_جابه_جایی(msi_control, SBI_MPXY_ATTR_MSI_CONTROL);
assert_فیلد_جابه_جایی(msi_info.msi_addr_lo, SBI_MPXY_ATTR_MSI_ADDR_LO);
assert_فیلد_جابه_جایی(msi_info.msi_addr_hi, SBI_MPXY_ATTR_MSI_ADDR_HI);
assert_فیلد_جابه_جایی(msi_info.msi_داده, SBI_MPXY_ATTR_MSI_داده);
assert_فیلد_جابه_جایی(eventsstate_ctrl, SBI_MPXY_ATTR_EVENTS_STATE_CONTROL);

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
ساكن بنية sbi_mpxy_channel *mpxy_پیدا_کن_channel(u32 channel_id,
					بنية sbi_domain *dom)
{
	بنية sbi_mpxy_channel *channel;

	sbi_list_for_each_مدخل(channel, &mpxy_channel_list, سر) {
		إذا (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			أرجع channel;
	}

	أرجع عدم;
}

/** Find channel_id in registered channels list */
بنية sbi_mpxy_channel *sbi_mpxy_پیدا_کن_channel_any(u32 channel_id)
{
	بنية sbi_mpxy_channel *channel;

	sbi_list_for_each_مدخل(channel, &mpxy_channel_list, سر) {
		إذا (channel->channel_id == channel_id)
			أرجع channel;
	}

	أرجع عدم;
}

/** Copy attributes word size */
ساكن فراغ mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 شمار)
{
	صحيح idx;
	لكل (idx = 0; idx < شمار; idx++)
		outmem[idx] = cpu_مقصد_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_موجود(فراغ)
{
	أرجع sbi_list_خالی(&mpxy_channel_list) ? false : true;
}

ساكن فراغ mpxy_std_attrs_init(بنية sbi_mpxy_channel *channel)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_داده = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	إذا (channel->send_پیام_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_پیام_WITH_RESP_ماسک);

	إذا (channel->send_پیام_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_پیام_WITHOUT_RESP_ماسک);

	إذا (channel->get_notification_events) {
		capability = CAP_ENABLE(capability, CAP_GET_NOTIFICATIONS_ماسک);
		/**
		 * Check if MSI or SSE available for notification interrrupt.
		 * Priority given to MSI if both MSI and SSE are avaialble.
		 */
		إذا (ms->msi_avail)
			capability = CAP_ENABLE(capability, CAP_MSI_ماسک);
		وإلا إذا (ms->sse_avail) {
			capability = CAP_ENABLE(capability, CAP_SSE_ماسک);
			/* TODO: Assign SSE EVENT_ID for the channel */
		}

		/**
		 * switch_eventstate callback support means support for events
		 * state reporting supoprt. Enable events state reporting in
		 * channel capability.
		 */
		إذا (channel->switch_eventsstate)
			capability = CAP_ENABLE(capability, CAP_EVENTSSTATE_ماسک);
	}

	channel->attrs.capability = capability;
}

/**
 * Register a channel with MPXY framework.
 * Called by message protocol drivers
 */
صحيح sbi_mpxy_ثبات_channel(بنية sbi_mpxy_channel *channel)
{
	إذا (!channel)
		أرجع SBI_EINVAL;

	إذا (!channel->owner_domain)
		أرجع SBI_EINVAL;

	إذا (sbi_mpxy_پیدا_کن_channel_any(channel->channel_id))
		أرجع SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	إذا (mpxy_shmem_اندازه < channel->attrs.پیام_داده_maxlen) {
		mpxy_shmem_اندازه = ROUNDUP(channel->attrs.پیام_داده_maxlen,
					  صفحه_اندازه);
	}

	sbi_list_جمع_کن_دم(&channel->سر, &mpxy_channel_list);

	أرجع SBI_OK;
}

/** Setup per domain MPXY state data */
ساكن صحيح domain_mpxy_state_راهاندازی(بنية sbi_domain *dom,
					بنية sbi_domain_state *state,
					فراغ *state_ptr)
{
	بنية mpxy_state **dom_hartindex_مقصد_mpxy_state_جدول = state_ptr;
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
		ms->msi_avail = !!sbi_irqchip_پیدا_کن_device_by_caps(SBI_IRQCHIP_CAPS_MSI, عدم);
		ms->sse_avail = false;

		sbi_mpxy_shmem_disable(ms);

		dom_hartindex_مقصد_mpxy_state_جدول[i] = ms;
	}

	أرجع 0;
}

/** Cleanup per domain MPXY state data */
ساكن فراغ domain_mpxy_state_cleanup(بنية sbi_domain *dom,
					   بنية sbi_domain_state *state,
					   فراغ *state_ptr)
{
	بنية mpxy_state **dom_hartindex_مقصد_mpxy_state_جدول = state_ptr;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts)
		sbi_free(dom_hartindex_مقصد_mpxy_state_جدول[i]);
}

ساكن بنية sbi_domain_state dmstate = {
	.state_راهاندازی = domain_mpxy_state_راهاندازی,
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
	بنية mpxy_state **dom_hartindex_مقصد_mpxy_state_جدول;

	dom_hartindex_مقصد_mpxy_state_جدول = sbi_domain_state_ptr(dom, &dmstate);
	إذا (!dom_hartindex_مقصد_mpxy_state_جدول ||
	    !sbi_hartindex_valid(hartindex))
		أرجع عدم;

	أرجع dom_hartindex_مقصد_mpxy_state_جدول[hartindex];
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
	dmstate.state_اندازه = حجم(بنية mpxy_state *) * sbi_hart_شمار();
	ret = sbi_domain_ثبات_state(&dmstate);
	إذا (ret)
		أرجع ret;

	أرجع sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

بیعلامت دراز sbi_mpxy_get_shmem_اندازه(فراغ)
{
	أرجع mpxy_shmem_اندازه;
}

صحيح sbi_mpxy_مجموعه_shmem(بیعلامت دراز shmem_phys_lo,
		       بیعلامت دراز shmem_phys_hi,
		       بیعلامت دراز flags)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	بیعلامت دراز *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	إذا (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		أرجع SBI_SUCCESS;
	}

	إذا (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		أرجع SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	إذا (shmem_phys_lo & ~صفحه_ماسک)
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
		أرجع SBI_ERR_INVALID_آدرس;

	إذا (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_اندازه, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		أرجع SBI_ERR_INVALID_آدرس;

	/** Save the current shmem details in new shmem region */
	إذا (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
		ret_buf = (بیعلامت دراز *)(ulong)SHMEM_PHYS_ADDR(shmem_phys_hi,
								  shmem_phys_lo);
		sbi_hart_protection_موقت_map_range((بیعلامت دراز)ret_buf, mpxy_shmem_اندازه);
		ret_buf[0] = cpu_مقصد_lle(ms->shmem.shmem_addr_lo);
		ret_buf[1] = cpu_مقصد_lle(ms->shmem.shmem_addr_hi);
		sbi_hart_protection_موقت_unmap_range((بیعلامت دراز)ret_buf, mpxy_shmem_اندازه);
	}

	/** Setup the new shared memory */
	ms->shmem.shmem_addr_lo = shmem_phys_lo;
	ms->shmem.shmem_addr_hi = shmem_phys_hi;

	أرجع SBI_SUCCESS;
}

صحيح sbi_mpxy_get_channel_ids(u32 شروع_شاخص)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 مانده, returned, max_channelids;
	u32 گره_شاخص = 0, گره_ret = 0;
	بنية sbi_mpxy_channel *channel;
	u32 channels_شمار = 0;
	u32 *shmem_پایه;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	sbi_list_for_each_مدخل(channel, &mpxy_channel_list, سر) {
		إذا (mpxy_channel_visible(channel, dom))
			channels_شمار += 1;
	}

	إذا (شروع_شاخص > channels_شمار)
		أرجع SBI_ERR_INVALID_PARAM;

	shmem_پایه = hart_shmem_پایه(ms);
	sbi_hart_protection_موقت_map_range((بیعلامت دراز)hart_shmem_پایه(ms), mpxy_shmem_اندازه);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_اندازه / حجم(u32)) - 2;
	/* total remaining from the start index */
	مانده = channels_شمار - شروع_شاخص;
	/* how many can be returned */
	returned = (مانده > max_channelids)? max_channelids : مانده;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_مدخل(channel, &mpxy_channel_list, سر) {
		إذا (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			استمر;

		إذا (گره_شاخص >= شروع_شاخص &&
			گره_شاخص < (شروع_شاخص + returned)) {
			shmem_پایه[2 + گره_ret] = cpu_مقصد_le32(channel->channel_id);
			گره_ret += 1;
		}

		گره_شاخص += 1;
	}

	/* final remaininig channel ids */
	مانده = channels_شمار - (شروع_شاخص + returned);

	shmem_پایه[0] = cpu_مقصد_le32(مانده);
	shmem_پایه[1] = cpu_مقصد_le32(returned);

	sbi_hart_protection_موقت_unmap_range((بیعلامت دراز)hart_shmem_پایه(ms), mpxy_shmem_اندازه);

	أرجع SBI_SUCCESS;
}

صحيح sbi_mpxy_read_attrs(u32 channel_id, u32 پایه_attr_id, u32 attr_شمار)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	صحيح ret = SBI_SUCCESS;
	u32 *attr_ptr, انتها_id;
	فراغ *shmem_پایه;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	بنية sbi_mpxy_channel *channel = mpxy_پیدا_کن_channel(channel_id, dom);
	إذا (!channel)
		أرجع SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	إذا (پایه_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		پایه_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_شروع)
		أرجع SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	إذا (!attr_شمار || (attr_شمار > (mpxy_shmem_اندازه / ATTR_اندازه)))
		أرجع SBI_ERR_INVALID_PARAM;

	shmem_پایه = hart_shmem_پایه(ms);
	انتها_id = پایه_attr_id + attr_شمار - 1;

	sbi_hart_protection_موقت_map_range((بیعلامت دراز)hart_shmem_پایه(ms), mpxy_shmem_اندازه);

	/* Standard attributes range check */
	إذا (mpxy_is_std_attr(پایه_attr_id)) {
		إذا (انتها_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			اذهب بیرون;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_پایه, &attr_ptr[پایه_attr_id],
				    attr_شمار);
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
		إذا (!channel->read_صفات ||
				انتها_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_انتها) {
			ret = SBI_ERR_BAD_RANGE;
			اذهب بیرون;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_صفات(channel,
					       (u32 *)shmem_پایه,
					       پایه_attr_id, attr_شمار);
	}
بیرون:
	sbi_hart_protection_موقت_unmap_range((بیعلامت دراز)hart_shmem_پایه(ms), mpxy_shmem_اندازه);
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
	حالة SBI_MPXY_ATTR_MSI_داده:
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
	حالة SBI_MPXY_ATTR_MSI_داده:
		إذا (ms->msi_avail)
			attrs->msi_info.msi_داده = attr_val;
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

صحيح sbi_mpxy_write_attrs(u32 channel_id, u32 پایه_attr_id, u32 attr_شمار)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, انتها_id, attr_val;
	بنية sbi_mpxy_channel *channel;
	صحيح ret, mem_idx;
	فراغ *shmem_پایه;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	channel = mpxy_پیدا_کن_channel(channel_id, dom);
	إذا (!channel)
		أرجع SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	إذا (پایه_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		پایه_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_شروع)
		أرجع SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	إذا (!attr_شمار || (attr_شمار > (mpxy_shmem_اندازه / ATTR_اندازه)))
		أرجع SBI_ERR_INVALID_PARAM;

	shmem_پایه = hart_shmem_پایه(ms);
	انتها_id = پایه_attr_id + attr_شمار - 1;

	sbi_hart_protection_موقت_map_range((بیعلامت دراز)shmem_پایه, mpxy_shmem_اندازه);

	mem_ptr = (u32 *)shmem_پایه;

	إذا (mpxy_is_std_attr(پایه_attr_id)) {
		إذا (انتها_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			اذهب بیرون;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		لكل (attr_id = پایه_attr_id; attr_id <= انتها_id; attr_id++) {
			attr_val = le32_مقصد_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			إذا (ret)
				اذهب بیرون;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		لكل (attr_id = پایه_attr_id; attr_id <= انتها_id; attr_id++) {
			attr_val = le32_مقصد_cpu(mem_ptr[mem_idx++]);
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
		إذا (!channel->write_صفات ||
				انتها_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_انتها) {
			ret = SBI_ERR_BAD_RANGE;
			اذهب بیرون;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_صفات(channel,
					       (u32 *)shmem_پایه,
					       پایه_attr_id, attr_شمار);
	}
بیرون:
	sbi_hart_protection_موقت_unmap_range((بیعلامت دراز)shmem_پایه, mpxy_shmem_اندازه);
	أرجع ret;
}

صحيح sbi_mpxy_send_پیام(u32 channel_id, u8 پیام_id,
			  بیعلامت دراز پیام_داده_len,
			  بیعلامت دراز *resp_داده_len)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	بنية sbi_mpxy_channel *channel;
	فراغ *shmem_پایه, *resp_buf;
	u32 resp_bufsize;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	صحيح ret;

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	channel = mpxy_پیدا_کن_channel(channel_id, dom);
	إذا (!channel)
		أرجع SBI_ERR_NOT_SUPPORTED;

	إذا (resp_داده_len && !channel->send_پیام_with_response)
		أرجع SBI_ERR_NOT_SUPPORTED;

	إذا (!resp_داده_len && !channel->send_پیام_without_response)
		أرجع SBI_ERR_NOT_SUPPORTED;

	إذا (پیام_داده_len > mpxy_shmem_اندازه ||
		پیام_داده_len > channel->attrs.پیام_داده_maxlen)
		أرجع SBI_ERR_INVALID_PARAM;

	shmem_پایه = hart_shmem_پایه(ms);
	sbi_hart_protection_موقت_map_range((بیعلامت دراز)shmem_پایه, mpxy_shmem_اندازه);

	إذا (resp_داده_len) {
		resp_buf = shmem_پایه;
		resp_bufsize = mpxy_shmem_اندازه;
		ret = channel->send_پیام_with_response(channel, پیام_id,
							  shmem_پایه,
							  پیام_داده_len,
							  resp_buf,
							  resp_bufsize,
							  resp_داده_len);
	} وإلا {
		ret = channel->send_پیام_without_response(channel, پیام_id,
							     shmem_پایه,
							     پیام_داده_len);
	}

	sbi_hart_protection_موقت_unmap_range((بیعلامت دراز)shmem_پایه, mpxy_shmem_اندازه);

	إذا (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		أرجع ret;
	وإلا إذا (ret)
		أرجع SBI_ERR_FAILED;

	إذا (resp_داده_len &&
	    (*resp_داده_len > mpxy_shmem_اندازه ||
	     *resp_داده_len > channel->attrs.پیام_داده_maxlen))
		أرجع SBI_ERR_FAILED;

	أرجع SBI_SUCCESS;
}

صحيح sbi_mpxy_get_notification_events(u32 channel_id, بیعلامت دراز *events_len)
{
	بنية mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	بنية sbi_mpxy_channel *channel;
	فراغ *eventsbuf, *shmem_پایه;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	صحيح ret;

	إذا (!mpxy_shmem_enabled(ms))
		أرجع SBI_ERR_NO_SHMEM;

	channel = mpxy_پیدا_کن_channel(channel_id, dom);
	إذا (!channel || !channel->get_notification_events)
		أرجع SBI_ERR_NOT_SUPPORTED;

	shmem_پایه = hart_shmem_پایه(ms);
	sbi_hart_protection_موقت_map_range((بیعلامت دراز)shmem_پایه, mpxy_shmem_اندازه);
	eventsbuf = shmem_پایه;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_اندازه,
					       events_len);
	sbi_hart_protection_موقت_unmap_range((بیعلامت دراز)shmem_پایه, mpxy_shmem_اندازه);

	إذا (ret)
		أرجع ret;

	إذا (*events_len > (mpxy_shmem_اندازه - 16))
		أرجع SBI_ERR_FAILED;

	أرجع SBI_SUCCESS;
}
