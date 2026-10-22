/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_hart_protection.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_irqchip.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_mpxy.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_byteorder.h>

/** Shared memory size across all harts */
స్థిర గుర్తులేని దీర్ఘ mpxy_shmem_కొలత = పుట_కొలత;

/** List of MPXY proxy channels */
స్థిర SBI_LIST_తల(mpxy_channel_list);

/** Invalid Physical Address(all bits 1) */
#నిర్వచించు INVALID_ADDR		(-1UL)

/** MPXY Attribute size in bytes */
#నిర్వచించు ATTR_కొలత		(4)

/** Channel Capability - MSI */
#నిర్వచించు CAP_MSI_POS		0
#నిర్వచించు CAP_MSI_ముసుగు		(1U << CAP_MSI_POS)

/** Channel Capability - SSE */
#నిర్వచించు CAP_SSE_POS		1
#నిర్వచించు CAP_SSE_ముసుగు		(1U << CAP_SSE_POS)

/** Channel Capability - Events State */
#నిర్వచించు CAP_EVENTSSTATE_POS	2
#నిర్వచించు CAP_EVENTSSTATE_ముసుగు	(1U << CAP_EVENTSSTATE_POS)

/** Channel Capability - Send Message With Response function support */
#నిర్వచించు CAP_SEND_సందేశం_WITH_RESP_POS	3
#నిర్వచించు CAP_SEND_సందేశం_WITH_RESP_ముసుగు	(1U << CAP_SEND_సందేశం_WITH_RESP_POS)

/** Channel Capability - Send Message Without Response function support */
#నిర్వచించు CAP_SEND_సందేశం_WITHOUT_RESP_POS	4
#నిర్వచించు CAP_SEND_సందేశం_WITHOUT_RESP_ముసుగు	(1U << CAP_SEND_సందేశం_WITHOUT_RESP_POS)

/** Channel Capability - Get Notification function support */
#నిర్వచించు CAP_GET_NOTIFICATIONS_POS	5
#నిర్వచించు CAP_GET_NOTIFICATIONS_ముసుగు	(1U << CAP_GET_NOTIFICATIONS_POS)

/** Helpers to enable/disable channel capability bits
 * _c: capability variable
 * _m: capability mask
 */
#నిర్వచించు CAP_ENABLE(_c, _m)	చొప్పించు_క్షేత్రం(_c, _m, 1)
#నిర్వచించు CAP_DISABLE(_c, _m)	చొప్పించు_క్షేత్రం(_c, _m, 0)
#నిర్వచించు CAP_GET(_c, _m)		వెలికితీయి_క్షేత్రం(_c, _m)

#నిర్వచించు SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
నిర్మాణం mpxy_shmem {
	గుర్తులేని దీర్ఘ shmem_addr_lo;
	గుర్తులేని దీర్ఘ shmem_addr_hi;
};

నిర్మాణం mpxy_state {
	/* MSI support in MPXY */
	bool msi_avail;
	/* SSE support in MPXY */
	bool sse_avail;
	/* MPXY Shared memory details */
	నిర్మాణం mpxy_shmem shmem;
};

స్థిర నిర్మాణం mpxy_state *sbi_domain_get_mpxy_state(నిర్మాణం sbi_domain *dom,
						    u32 hartindex);

/** Macro to obtain the current hart's MPXY state pointer in current domain */
#నిర్వచించు sbi_domain_mpxy_state_thishart_ptr()			\
	sbi_domain_get_mpxy_state(sbi_domain_thishart_ptr(),	\
				  current_hartindex())

/** Disable hart shared memory */
స్థిర అంతఃవరుస శూన్య sbi_mpxy_shmem_disable(నిర్మాణం mpxy_state *ms)
{
	ms->shmem.shmem_addr_lo = INVALID_ADDR;
	ms->shmem.shmem_addr_hi = INVALID_ADDR;
}

/** Check if shared memory is already setup on hart */
స్థిర అంతఃవరుస bool mpxy_shmem_enabled(నిర్మాణం mpxy_state *ms)
{
	తిరిగి (ms->shmem.shmem_addr_lo == INVALID_ADDR
		&& ms->shmem.shmem_addr_hi == INVALID_ADDR) ?
		false : true;
}

/** Get hart shared memory base address */
స్థిర అంతఃవరుస శూన్య *hart_shmem_పునాది(నిర్మాణం mpxy_state *ms)
{
	తిరిగి (శూన్య *)(గుర్తులేని దీర్ఘ)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#నిర్వచించు assert_క్షేత్రం_స్థానభ్రంశం(క్షేత్రం, attr_స్థానభ్రంశం)				\
	_Static_assert(							\
		((offsetof(నిర్మాణం sbi_mpxy_channel_attrs, క్షేత్రం)) /	\
		 పరిమాణం(u32)) == attr_స్థానభ్రంశం,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_క్షేత్రం_స్థానభ్రంశం(సందేశం_proto_id, SBI_MPXY_ATTR_సందేశం_PROT_ID);
assert_క్షేత్రం_స్థానభ్రంశం(సందేశం_proto_సంస్కరణ, SBI_MPXY_ATTR_సందేశం_PROT_VER);
assert_క్షేత్రం_స్థానభ్రంశం(సందేశం_దత్తాంశం_maxlen, SBI_MPXY_ATTR_సందేశం_MAX_LEN);
assert_క్షేత్రం_స్థానభ్రంశం(సందేశం_send_timeout, SBI_MPXY_ATTR_సందేశం_SEND_TIMEOUT);
assert_క్షేత్రం_స్థానభ్రంశం(సందేశం_completion_timeout, SBI_MPXY_ATTR_సందేశం_COMPLETION_TIMEOUT);
assert_క్షేత్రం_స్థానభ్రంశం(capability, SBI_MPXY_ATTR_CHANNEL_CAPABILITY);
assert_క్షేత్రం_స్థానభ్రంశం(sse_event_id, SBI_MPXY_ATTR_SSE_EVENT_ID);
assert_క్షేత్రం_స్థానభ్రంశం(msi_control, SBI_MPXY_ATTR_MSI_CONTROL);
assert_క్షేత్రం_స్థానభ్రంశం(msi_info.msi_addr_lo, SBI_MPXY_ATTR_MSI_ADDR_LO);
assert_క్షేత్రం_స్థానభ్రంశం(msi_info.msi_addr_hi, SBI_MPXY_ATTR_MSI_ADDR_HI);
assert_క్షేత్రం_స్థానభ్రంశం(msi_info.msi_దత్తాంశం, SBI_MPXY_ATTR_MSI_దత్తాంశం);
assert_క్షేత్రం_స్థానభ్రంశం(eventsstate_ctrl, SBI_MPXY_ATTR_EVENTS_STATE_CONTROL);

/**
 * Check if the attribute is a standard attribute or
 * a message protocol specific attribute
 * attr_id[31] = 0 for standard
 * attr_id[31] = 1 for message protocol specific
 */
స్థిర అంతఃవరుస bool mpxy_is_std_attr(u32 attr_id)
{
	తిరిగి (attr_id >> 31) ? false : true;
}

స్థిర అంతఃవరుస bool mpxy_channel_visible(నిర్మాణం sbi_mpxy_channel *channel,
					నిర్మాణం sbi_domain *dom)
{
	తిరిగి channel->owner_domain == dom;
}

/** Find channel_id in registered channels list for this domain */
స్థిర నిర్మాణం sbi_mpxy_channel *mpxy_కనుగొను_channel(u32 channel_id,
					నిర్మాణం sbi_domain *dom)
{
	నిర్మాణం sbi_mpxy_channel *channel;

	sbi_list_for_each_నమోదు(channel, &mpxy_channel_list, తల) {
		ఒకవేళ (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			తిరిగి channel;
	}

	తిరిగి ఖాళీ;
}

/** Find channel_id in registered channels list */
నిర్మాణం sbi_mpxy_channel *sbi_mpxy_కనుగొను_channel_any(u32 channel_id)
{
	నిర్మాణం sbi_mpxy_channel *channel;

	sbi_list_for_each_నమోదు(channel, &mpxy_channel_list, తల) {
		ఒకవేళ (channel->channel_id == channel_id)
			తిరిగి channel;
	}

	తిరిగి ఖాళీ;
}

/** Copy attributes word size */
స్థిర శూన్య mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 లెక్క)
{
	పూర్ణాంకం idx;
	కొరకు (idx = 0; idx < లెక్క; idx++)
		outmem[idx] = cpu_గమ్యం_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_అందుబాటు(శూన్య)
{
	తిరిగి sbi_list_ఖాళీలు(&mpxy_channel_list) ? false : true;
}

స్థిర శూన్య mpxy_std_attrs_init(నిర్మాణం sbi_mpxy_channel *channel)
{
	నిర్మాణం mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_దత్తాంశం = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	ఒకవేళ (channel->send_సందేశం_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_సందేశం_WITH_RESP_ముసుగు);

	ఒకవేళ (channel->send_సందేశం_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_సందేశం_WITHOUT_RESP_ముసుగు);

	ఒకవేళ (channel->get_notification_events) {
		capability = CAP_ENABLE(capability, CAP_GET_NOTIFICATIONS_ముసుగు);
		/**
		 * Check if MSI or SSE available for notification interrrupt.
		 * Priority given to MSI if both MSI and SSE are avaialble.
		 */
		ఒకవేళ (ms->msi_avail)
			capability = CAP_ENABLE(capability, CAP_MSI_ముసుగు);
		లేకపోతే ఒకవేళ (ms->sse_avail) {
			capability = CAP_ENABLE(capability, CAP_SSE_ముసుగు);
			/* TODO: Assign SSE EVENT_ID for the channel */
		}

		/**
		 * switch_eventstate callback support means support for events
		 * state reporting supoprt. Enable events state reporting in
		 * channel capability.
		 */
		ఒకవేళ (channel->switch_eventsstate)
			capability = CAP_ENABLE(capability, CAP_EVENTSSTATE_ముసుగు);
	}

	channel->attrs.capability = capability;
}

/**
 * Register a channel with MPXY framework.
 * Called by message protocol drivers
 */
పూర్ణాంకం sbi_mpxy_రిజిస్టరు_channel(నిర్మాణం sbi_mpxy_channel *channel)
{
	ఒకవేళ (!channel)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (!channel->owner_domain)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (sbi_mpxy_కనుగొను_channel_any(channel->channel_id))
		తిరిగి SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	ఒకవేళ (mpxy_shmem_కొలత < channel->attrs.సందేశం_దత్తాంశం_maxlen) {
		mpxy_shmem_కొలత = ROUNDUP(channel->attrs.సందేశం_దత్తాంశం_maxlen,
					  పుట_కొలత);
	}

	sbi_list_కూడు_తోక(&channel->తల, &mpxy_channel_list);

	తిరిగి SBI_OK;
}

/** Setup per domain MPXY state data */
స్థిర పూర్ణాంకం domain_mpxy_state_అమరిక(నిర్మాణం sbi_domain *dom,
					నిర్మాణం sbi_domain_state *state,
					శూన్య *state_ptr)
{
	నిర్మాణం mpxy_state **dom_hartindex_గమ్యం_mpxy_state_పట్టిక = state_ptr;
	నిర్మాణం mpxy_state *ms;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ms = sbi_zalloc(పరిమాణం(*ms));
		ఒకవేళ (!ms)
			తిరిగి SBI_ENOMEM;

		/*
		 * TODO: Proper support for checking sse support from
		 * platform. Currently disable sse and use polling
		 */
		ms->msi_avail = !!sbi_irqchip_కనుగొను_device_by_caps(SBI_IRQCHIP_CAPS_MSI, ఖాళీ);
		ms->sse_avail = false;

		sbi_mpxy_shmem_disable(ms);

		dom_hartindex_గమ్యం_mpxy_state_పట్టిక[i] = ms;
	}

	తిరిగి 0;
}

/** Cleanup per domain MPXY state data */
స్థిర శూన్య domain_mpxy_state_cleanup(నిర్మాణం sbi_domain *dom,
					   నిర్మాణం sbi_domain_state *state,
					   శూన్య *state_ptr)
{
	నిర్మాణం mpxy_state **dom_hartindex_గమ్యం_mpxy_state_పట్టిక = state_ptr;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts)
		sbi_free(dom_hartindex_గమ్యం_mpxy_state_పట్టిక[i]);
}

స్థిర నిర్మాణం sbi_domain_state dmstate = {
	.state_అమరిక = domain_mpxy_state_అమరిక,
	.state_cleanup = domain_mpxy_state_cleanup,
};

/**
 * Get per-domain MPXY state pointer for a given domain and HART index
 * @param dom pointer to domain
 * @param hartindex the HART index
 *
 * @return per-domain MPXY state pointer for given HART index
 */
స్థిర నిర్మాణం mpxy_state *sbi_domain_get_mpxy_state(నిర్మాణం sbi_domain *dom,
						    u32 hartindex)
{
	నిర్మాణం mpxy_state **dom_hartindex_గమ్యం_mpxy_state_పట్టిక;

	dom_hartindex_గమ్యం_mpxy_state_పట్టిక = sbi_domain_state_ptr(dom, &dmstate);
	ఒకవేళ (!dom_hartindex_గమ్యం_mpxy_state_పట్టిక ||
	    !sbi_hartindex_valid(hartindex))
		తిరిగి ఖాళీ;

	తిరిగి dom_hartindex_గమ్యం_mpxy_state_పట్టిక[hartindex];
}

పూర్ణాంకం sbi_mpxy_init(నిర్మాణం sbi_scratch *scratch)
{
	పూర్ణాంకం ret;

	/**
	 * Allocate per-domain and per-hart MPXY state data.
	 * The data type is "struct mpxy_state **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one() and
	 * domain_mpxy_state_setup(). Calculate needed size of memory space
	 * here.
	 */
	dmstate.state_కొలత = పరిమాణం(నిర్మాణం mpxy_state *) * sbi_hart_లెక్క();
	ret = sbi_domain_రిజిస్టరు_state(&dmstate);
	ఒకవేళ (ret)
		తిరిగి ret;

	తిరిగి sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

గుర్తులేని దీర్ఘ sbi_mpxy_get_shmem_కొలత(శూన్య)
{
	తిరిగి mpxy_shmem_కొలత;
}

పూర్ణాంకం sbi_mpxy_సమితి_shmem(గుర్తులేని దీర్ఘ shmem_phys_lo,
		       గుర్తులేని దీర్ఘ shmem_phys_hi,
		       గుర్తులేని దీర్ఘ flags)
{
	నిర్మాణం mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	గుర్తులేని దీర్ఘ *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	ఒకవేళ (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		తిరిగి SBI_SUCCESS;
	}

	ఒకవేళ (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		తిరిగి SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	ఒకవేళ (shmem_phys_lo & ~పుట_ముసుగు)
		తిరిగి SBI_ERR_INVALID_PARAM;

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
	ఒకవేళ (shmem_phys_hi)
		తిరిగి SBI_ERR_INVALID_చిరునామా;

	ఒకవేళ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_కొలత, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		తిరిగి SBI_ERR_INVALID_చిరునామా;

	/** Save the current shmem details in new shmem region */
	ఒకవేళ (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
		ret_buf = (గుర్తులేని దీర్ఘ *)(ulong)SHMEM_PHYS_ADDR(shmem_phys_hi,
								  shmem_phys_lo);
		sbi_hart_protection_తాత్కాలిక_map_range((గుర్తులేని దీర్ఘ)ret_buf, mpxy_shmem_కొలత);
		ret_buf[0] = cpu_గమ్యం_lle(ms->shmem.shmem_addr_lo);
		ret_buf[1] = cpu_గమ్యం_lle(ms->shmem.shmem_addr_hi);
		sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)ret_buf, mpxy_shmem_కొలత);
	}

	/** Setup the new shared memory */
	ms->shmem.shmem_addr_lo = shmem_phys_lo;
	ms->shmem.shmem_addr_hi = shmem_phys_hi;

	తిరిగి SBI_SUCCESS;
}

పూర్ణాంకం sbi_mpxy_get_channel_ids(u32 ప్రారంభం_సూచీ)
{
	నిర్మాణం mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 మిగిలిన, returned, max_channelids;
	u32 కణుపు_సూచీ = 0, కణుపు_ret = 0;
	నిర్మాణం sbi_mpxy_channel *channel;
	u32 channels_లెక్క = 0;
	u32 *shmem_పునాది;
	నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();

	ఒకవేళ (!mpxy_shmem_enabled(ms))
		తిరిగి SBI_ERR_NO_SHMEM;

	sbi_list_for_each_నమోదు(channel, &mpxy_channel_list, తల) {
		ఒకవేళ (mpxy_channel_visible(channel, dom))
			channels_లెక్క += 1;
	}

	ఒకవేళ (ప్రారంభం_సూచీ > channels_లెక్క)
		తిరిగి SBI_ERR_INVALID_PARAM;

	shmem_పునాది = hart_shmem_పునాది(ms);
	sbi_hart_protection_తాత్కాలిక_map_range((గుర్తులేని దీర్ఘ)hart_shmem_పునాది(ms), mpxy_shmem_కొలత);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_కొలత / పరిమాణం(u32)) - 2;
	/* total remaining from the start index */
	మిగిలిన = channels_లెక్క - ప్రారంభం_సూచీ;
	/* how many can be returned */
	returned = (మిగిలిన > max_channelids)? max_channelids : మిగిలిన;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_నమోదు(channel, &mpxy_channel_list, తల) {
		ఒకవేళ (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			కొనసాగు;

		ఒకవేళ (కణుపు_సూచీ >= ప్రారంభం_సూచీ &&
			కణుపు_సూచీ < (ప్రారంభం_సూచీ + returned)) {
			shmem_పునాది[2 + కణుపు_ret] = cpu_గమ్యం_le32(channel->channel_id);
			కణుపు_ret += 1;
		}

		కణుపు_సూచీ += 1;
	}

	/* final remaininig channel ids */
	మిగిలిన = channels_లెక్క - (ప్రారంభం_సూచీ + returned);

	shmem_పునాది[0] = cpu_గమ్యం_le32(మిగిలిన);
	shmem_పునాది[1] = cpu_గమ్యం_le32(returned);

	sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)hart_shmem_పునాది(ms), mpxy_shmem_కొలత);

	తిరిగి SBI_SUCCESS;
}

పూర్ణాంకం sbi_mpxy_read_attrs(u32 channel_id, u32 పునాది_attr_id, u32 attr_లెక్క)
{
	నిర్మాణం mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	పూర్ణాంకం ret = SBI_SUCCESS;
	u32 *attr_ptr, చివర_id;
	శూన్య *shmem_పునాది;
	నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();

	ఒకవేళ (!mpxy_shmem_enabled(ms))
		తిరిగి SBI_ERR_NO_SHMEM;

	నిర్మాణం sbi_mpxy_channel *channel = mpxy_కనుగొను_channel(channel_id, dom);
	ఒకవేళ (!channel)
		తిరిగి SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	ఒకవేళ (పునాది_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		పునాది_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_ప్రారంభం)
		తిరిగి SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	ఒకవేళ (!attr_లెక్క || (attr_లెక్క > (mpxy_shmem_కొలత / ATTR_కొలత)))
		తిరిగి SBI_ERR_INVALID_PARAM;

	shmem_పునాది = hart_shmem_పునాది(ms);
	చివర_id = పునాది_attr_id + attr_లెక్క - 1;

	sbi_hart_protection_తాత్కాలిక_map_range((గుర్తులేని దీర్ఘ)hart_shmem_పునాది(ms), mpxy_shmem_కొలత);

	/* Standard attributes range check */
	ఒకవేళ (mpxy_is_std_attr(పునాది_attr_id)) {
		ఒకవేళ (చివర_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			వెళ్ళు బహిర్గతం;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_పునాది, &attr_ptr[పునాది_attr_id],
				    attr_లెక్క);
	} లేకపోతే {
		/**
		 * Even if the message protocol driver does not provide
		 * read attribute callback, return bad range error instead
		 * of not supported to let client distinguish it from channel
		 * id not supported.
		 * Check the complate range supported for message protocol
		 * attributes. Actual supported attributes will be checked
		 * by the message protocol driver.
		 */
		ఒకవేళ (!channel->read_లక్షణాలు ||
				చివర_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_చివర) {
			ret = SBI_ERR_BAD_RANGE;
			వెళ్ళు బహిర్గతం;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_లక్షణాలు(channel,
					       (u32 *)shmem_పునాది,
					       పునాది_attr_id, attr_లెక్క);
	}
బహిర్గతం:
	sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)hart_shmem_పునాది(ms), mpxy_shmem_కొలత);
	తిరిగి ret;
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
స్థిర పూర్ణాంకం mpxy_check_write_std_attr(నిర్మాణం sbi_mpxy_channel *channel,
				     u32 attr_id, u32 attr_val)
{
	నిర్మాణం sbi_mpxy_channel_attrs *attrs = &channel->attrs;
	పూర్ణాంకం ret = SBI_SUCCESS;

	ఎంపిక(attr_id) {
	స్థితి SBI_MPXY_ATTR_MSI_CONTROL:
		ఒకవేళ (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		ఒకవేళ (attr_val == 1 &&
		    (attrs->msi_info.msi_addr_lo == (u32)INVALID_ADDR) &&
		    (attrs->msi_info.msi_addr_hi == (u32)INVALID_ADDR))
			ret = SBI_ERR_DENIED;
		ఆపు;
	స్థితి SBI_MPXY_ATTR_MSI_ADDR_LO:
	స్థితి SBI_MPXY_ATTR_MSI_ADDR_HI:
	స్థితి SBI_MPXY_ATTR_MSI_దత్తాంశం:
		ret = SBI_SUCCESS;
		ఆపు;
	స్థితి SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		ఒకవేళ (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		ఆపు;
	అప్రమేయ:
		/** All RO access attributes falls under default */
		ret = SBI_ERR_BAD_RANGE;
	};

	తిరిగి ret;
}

/**
 * Write the attribute value
 */
స్థిర శూన్య mpxy_write_std_attr(నిర్మాణం sbi_mpxy_channel *channel, u32 attr_id,
				u32 attr_val)
{
	నిర్మాణం mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	నిర్మాణం sbi_mpxy_channel_attrs *attrs = &channel->attrs;

	ఎంపిక(attr_id) {
	స్థితి SBI_MPXY_ATTR_MSI_CONTROL:
		ఒకవేళ (ms->msi_avail && attr_val <= 1)
			attrs->msi_control = attr_val;
		ఆపు;
	స్థితి SBI_MPXY_ATTR_MSI_ADDR_LO:
		ఒకవేళ (ms->msi_avail)
			attrs->msi_info.msi_addr_lo = attr_val;
		ఆపు;
	స్థితి SBI_MPXY_ATTR_MSI_ADDR_HI:
		ఒకవేళ (ms->msi_avail)
			attrs->msi_info.msi_addr_hi = attr_val;
		ఆపు;
	స్థితి SBI_MPXY_ATTR_MSI_దత్తాంశం:
		ఒకవేళ (ms->msi_avail)
			attrs->msi_info.msi_దత్తాంశం = attr_val;
		ఆపు;
	స్థితి SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		ఒకవేళ (channel->switch_eventsstate && attr_val <= 1) {
			attrs->eventsstate_ctrl = attr_val;
			 /* message protocol callback to enable/disable
			 * events state reporting. */
			channel->switch_eventsstate(attr_val);
		}

		ఆపు;
	};
}

పూర్ణాంకం sbi_mpxy_write_attrs(u32 channel_id, u32 పునాది_attr_id, u32 attr_లెక్క)
{
	నిర్మాణం mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, చివర_id, attr_val;
	నిర్మాణం sbi_mpxy_channel *channel;
	పూర్ణాంకం ret, mem_idx;
	శూన్య *shmem_పునాది;
	నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();

	ఒకవేళ (!mpxy_shmem_enabled(ms))
		తిరిగి SBI_ERR_NO_SHMEM;

	channel = mpxy_కనుగొను_channel(channel_id, dom);
	ఒకవేళ (!channel)
		తిరిగి SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	ఒకవేళ (పునాది_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		పునాది_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_ప్రారంభం)
		తిరిగి SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	ఒకవేళ (!attr_లెక్క || (attr_లెక్క > (mpxy_shmem_కొలత / ATTR_కొలత)))
		తిరిగి SBI_ERR_INVALID_PARAM;

	shmem_పునాది = hart_shmem_పునాది(ms);
	చివర_id = పునాది_attr_id + attr_లెక్క - 1;

	sbi_hart_protection_తాత్కాలిక_map_range((గుర్తులేని దీర్ఘ)shmem_పునాది, mpxy_shmem_కొలత);

	mem_ptr = (u32 *)shmem_పునాది;

	ఒకవేళ (mpxy_is_std_attr(పునాది_attr_id)) {
		ఒకవేళ (చివర_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			వెళ్ళు బహిర్గతం;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		కొరకు (attr_id = పునాది_attr_id; attr_id <= చివర_id; attr_id++) {
			attr_val = le32_గమ్యం_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			ఒకవేళ (ret)
				వెళ్ళు బహిర్గతం;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		కొరకు (attr_id = పునాది_attr_id; attr_id <= చివర_id; attr_id++) {
			attr_val = le32_గమ్యం_cpu(mem_ptr[mem_idx++]);
			mpxy_write_std_attr(channel, attr_id, attr_val);
		}
	} లేకపోతే {/**
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
		ఒకవేళ (!channel->write_లక్షణాలు ||
				చివర_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_చివర) {
			ret = SBI_ERR_BAD_RANGE;
			వెళ్ళు బహిర్గతం;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_లక్షణాలు(channel,
					       (u32 *)shmem_పునాది,
					       పునాది_attr_id, attr_లెక్క);
	}
బహిర్గతం:
	sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)shmem_పునాది, mpxy_shmem_కొలత);
	తిరిగి ret;
}

పూర్ణాంకం sbi_mpxy_send_సందేశం(u32 channel_id, u8 సందేశం_id,
			  గుర్తులేని దీర్ఘ సందేశం_దత్తాంశం_len,
			  గుర్తులేని దీర్ఘ *resp_దత్తాంశం_len)
{
	నిర్మాణం mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	నిర్మాణం sbi_mpxy_channel *channel;
	శూన్య *shmem_పునాది, *resp_buf;
	u32 resp_bufsize;
	నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();
	పూర్ణాంకం ret;

	ఒకవేళ (!mpxy_shmem_enabled(ms))
		తిరిగి SBI_ERR_NO_SHMEM;

	channel = mpxy_కనుగొను_channel(channel_id, dom);
	ఒకవేళ (!channel)
		తిరిగి SBI_ERR_NOT_SUPPORTED;

	ఒకవేళ (resp_దత్తాంశం_len && !channel->send_సందేశం_with_response)
		తిరిగి SBI_ERR_NOT_SUPPORTED;

	ఒకవేళ (!resp_దత్తాంశం_len && !channel->send_సందేశం_without_response)
		తిరిగి SBI_ERR_NOT_SUPPORTED;

	ఒకవేళ (సందేశం_దత్తాంశం_len > mpxy_shmem_కొలత ||
		సందేశం_దత్తాంశం_len > channel->attrs.సందేశం_దత్తాంశం_maxlen)
		తిరిగి SBI_ERR_INVALID_PARAM;

	shmem_పునాది = hart_shmem_పునాది(ms);
	sbi_hart_protection_తాత్కాలిక_map_range((గుర్తులేని దీర్ఘ)shmem_పునాది, mpxy_shmem_కొలత);

	ఒకవేళ (resp_దత్తాంశం_len) {
		resp_buf = shmem_పునాది;
		resp_bufsize = mpxy_shmem_కొలత;
		ret = channel->send_సందేశం_with_response(channel, సందేశం_id,
							  shmem_పునాది,
							  సందేశం_దత్తాంశం_len,
							  resp_buf,
							  resp_bufsize,
							  resp_దత్తాంశం_len);
	} లేకపోతే {
		ret = channel->send_సందేశం_without_response(channel, సందేశం_id,
							     shmem_పునాది,
							     సందేశం_దత్తాంశం_len);
	}

	sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)shmem_పునాది, mpxy_shmem_కొలత);

	ఒకవేళ (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		తిరిగి ret;
	లేకపోతే ఒకవేళ (ret)
		తిరిగి SBI_ERR_FAILED;

	ఒకవేళ (resp_దత్తాంశం_len &&
	    (*resp_దత్తాంశం_len > mpxy_shmem_కొలత ||
	     *resp_దత్తాంశం_len > channel->attrs.సందేశం_దత్తాంశం_maxlen))
		తిరిగి SBI_ERR_FAILED;

	తిరిగి SBI_SUCCESS;
}

పూర్ణాంకం sbi_mpxy_get_notification_events(u32 channel_id, గుర్తులేని దీర్ఘ *events_len)
{
	నిర్మాణం mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	నిర్మాణం sbi_mpxy_channel *channel;
	శూన్య *eventsbuf, *shmem_పునాది;
	నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();
	పూర్ణాంకం ret;

	ఒకవేళ (!mpxy_shmem_enabled(ms))
		తిరిగి SBI_ERR_NO_SHMEM;

	channel = mpxy_కనుగొను_channel(channel_id, dom);
	ఒకవేళ (!channel || !channel->get_notification_events)
		తిరిగి SBI_ERR_NOT_SUPPORTED;

	shmem_పునాది = hart_shmem_పునాది(ms);
	sbi_hart_protection_తాత్కాలిక_map_range((గుర్తులేని దీర్ఘ)shmem_పునాది, mpxy_shmem_కొలత);
	eventsbuf = shmem_పునాది;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_కొలత,
					       events_len);
	sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)shmem_పునాది, mpxy_shmem_కొలత);

	ఒకవేళ (ret)
		తిరిగి ret;

	ఒకవేళ (*events_len > (mpxy_shmem_కొలత - 16))
		తిరిగి SBI_ERR_FAILED;

	తిరిగి SBI_SUCCESS;
}
