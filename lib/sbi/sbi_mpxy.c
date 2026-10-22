/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_hart_protection.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_irqchip.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_mpxy.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_byteorder.h>

/** Shared memory size across all harts */
स्थिर अचिह्नित दीर्घ mpxy_shmem_आकार = PAGE_आकार;

/** List of MPXY proxy channels */
स्थिर SBI_LIST_शीर्ष(mpxy_channel_list);

/** Invalid Physical Address(all bits 1) */
#परिभाषय INVALID_ADDR		(-1UL)

/** MPXY Attribute size in bytes */
#परिभाषय ATTR_आकार		(4)

/** Channel Capability - MSI */
#परिभाषय CAP_MSI_POS		0
#परिभाषय CAP_MSI_आवरणम्		(1U << CAP_MSI_POS)

/** Channel Capability - SSE */
#परिभाषय CAP_SSE_POS		1
#परिभाषय CAP_SSE_आवरणम्		(1U << CAP_SSE_POS)

/** Channel Capability - Events State */
#परिभाषय CAP_EVENTSSTATE_POS	2
#परिभाषय CAP_EVENTSSTATE_आवरणम्	(1U << CAP_EVENTSSTATE_POS)

/** Channel Capability - Send Message With Response function support */
#परिभाषय CAP_SEND_सन्देशः_WITH_RESP_POS	3
#परिभाषय CAP_SEND_सन्देशः_WITH_RESP_आवरणम्	(1U << CAP_SEND_सन्देशः_WITH_RESP_POS)

/** Channel Capability - Send Message Without Response function support */
#परिभाषय CAP_SEND_सन्देशः_WITHOUT_RESP_POS	4
#परिभाषय CAP_SEND_सन्देशः_WITHOUT_RESP_आवरणम्	(1U << CAP_SEND_सन्देशः_WITHOUT_RESP_POS)

/** Channel Capability - Get Notification function support */
#परिभाषय CAP_GET_NOTIFICATIONS_POS	5
#परिभाषय CAP_GET_NOTIFICATIONS_आवरणम्	(1U << CAP_GET_NOTIFICATIONS_POS)

/** Helpers to enable/disable channel capability bits
 * _c: capability variable
 * _m: capability mask
 */
#परिभाषय CAP_ENABLE(_c, _m)	निवेशय_क्षेत्रम्(_c, _m, 1)
#परिभाषय CAP_DISABLE(_c, _m)	निवेशय_क्षेत्रम्(_c, _m, 0)
#परिभाषय CAP_GET(_c, _m)		EXTRACT_क्षेत्रम्(_c, _m)

#परिभाषय SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
संरचना mpxy_shmem {
	अचिह्नित दीर्घ shmem_addr_lo;
	अचिह्नित दीर्घ shmem_addr_hi;
};

संरचना mpxy_state {
	/* MSI support in MPXY */
	bool msi_avail;
	/* SSE support in MPXY */
	bool sse_avail;
	/* MPXY Shared memory details */
	संरचना mpxy_shmem shmem;
};

स्थिर संरचना mpxy_state *sbi_domain_get_mpxy_state(संरचना sbi_domain *dom,
						    u32 hartindex);

/** Macro to obtain the current hart's MPXY state pointer in current domain */
#परिभाषय sbi_domain_mpxy_state_thishart_ptr()			\
	sbi_domain_get_mpxy_state(sbi_domain_thishart_ptr(),	\
				  current_hartindex())

/** Disable hart shared memory */
स्थिर अन्तःस्थ शून्य sbi_mpxy_shmem_disable(संरचना mpxy_state *ms)
{
	ms->shmem.shmem_addr_lo = INVALID_ADDR;
	ms->shmem.shmem_addr_hi = INVALID_ADDR;
}

/** Check if shared memory is already setup on hart */
स्थिर अन्तःस्थ bool mpxy_shmem_enabled(संरचना mpxy_state *ms)
{
	प्रत्यागम (ms->shmem.shmem_addr_lo == INVALID_ADDR
		&& ms->shmem.shmem_addr_hi == INVALID_ADDR) ?
		false : true;
}

/** Get hart shared memory base address */
स्थिर अन्तःस्थ शून्य *hart_shmem_आधारः(संरचना mpxy_state *ms)
{
	प्रत्यागम (शून्य *)(अचिह्नित दीर्घ)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#परिभाषय assert_क्षेत्रम्_विस्थापनम्(क्षेत्रम्, attr_विस्थापनम्)				\
	_Static_assert(							\
		((offsetof(संरचना sbi_mpxy_channel_attrs, क्षेत्रम्)) /	\
		 परिमाण(u32)) == attr_विस्थापनम्,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_क्षेत्रम्_विस्थापनम्(सन्देशः_proto_id, SBI_MPXY_ATTR_सन्देशः_PROT_ID);
assert_क्षेत्रम्_विस्थापनम्(सन्देशः_proto_संस्करणम्, SBI_MPXY_ATTR_सन्देशः_PROT_VER);
assert_क्षेत्रम्_विस्थापनम्(सन्देशः_दत्त_maxlen, SBI_MPXY_ATTR_सन्देशः_MAX_LEN);
assert_क्षेत्रम्_विस्थापनम्(सन्देशः_send_timeout, SBI_MPXY_ATTR_सन्देशः_SEND_TIMEOUT);
assert_क्षेत्रम्_विस्थापनम्(सन्देशः_completion_timeout, SBI_MPXY_ATTR_सन्देशः_COMPLETION_TIMEOUT);
assert_क्षेत्रम्_विस्थापनम्(capability, SBI_MPXY_ATTR_CHANNEL_CAPABILITY);
assert_क्षेत्रम्_विस्थापनम्(sse_event_id, SBI_MPXY_ATTR_SSE_EVENT_ID);
assert_क्षेत्रम्_विस्थापनम्(msi_control, SBI_MPXY_ATTR_MSI_CONTROL);
assert_क्षेत्रम्_विस्थापनम्(msi_info.msi_addr_lo, SBI_MPXY_ATTR_MSI_ADDR_LO);
assert_क्षेत्रम्_विस्थापनम्(msi_info.msi_addr_hi, SBI_MPXY_ATTR_MSI_ADDR_HI);
assert_क्षेत्रम्_विस्थापनम्(msi_info.msi_दत्त, SBI_MPXY_ATTR_MSI_दत्त);
assert_क्षेत्रम्_विस्थापनम्(eventsstate_ctrl, SBI_MPXY_ATTR_EVENTS_STATE_CONTROL);

/**
 * Check if the attribute is a standard attribute or
 * a message protocol specific attribute
 * attr_id[31] = 0 for standard
 * attr_id[31] = 1 for message protocol specific
 */
स्थिर अन्तःस्थ bool mpxy_is_std_attr(u32 attr_id)
{
	प्रत्यागम (attr_id >> 31) ? false : true;
}

स्थिर अन्तःस्थ bool mpxy_channel_visible(संरचना sbi_mpxy_channel *channel,
					संरचना sbi_domain *dom)
{
	प्रत्यागम channel->owner_domain == dom;
}

/** Find channel_id in registered channels list for this domain */
स्थिर संरचना sbi_mpxy_channel *mpxy_find_channel(u32 channel_id,
					संरचना sbi_domain *dom)
{
	संरचना sbi_mpxy_channel *channel;

	sbi_list_for_each_प्रविष्टिः(channel, &mpxy_channel_list, शीर्ष) {
		यदि (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			प्रत्यागम channel;
	}

	प्रत्यागम रिक्त;
}

/** Find channel_id in registered channels list */
संरचना sbi_mpxy_channel *sbi_mpxy_find_channel_any(u32 channel_id)
{
	संरचना sbi_mpxy_channel *channel;

	sbi_list_for_each_प्रविष्टिः(channel, &mpxy_channel_list, शीर्ष) {
		यदि (channel->channel_id == channel_id)
			प्रत्यागम channel;
	}

	प्रत्यागम रिक्त;
}

/** Copy attributes word size */
स्थिर शून्य mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 गणनम्)
{
	पूर्णाङ्क idx;
	प्रति (idx = 0; idx < गणनम्; idx++)
		outmem[idx] = cpu_गन्तव्यम्_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_उपलब्धम्(शून्य)
{
	प्रत्यागम sbi_list_रिक्तानि(&mpxy_channel_list) ? false : true;
}

स्थिर शून्य mpxy_std_attrs_init(संरचना sbi_mpxy_channel *channel)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_दत्त = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	यदि (channel->send_सन्देशः_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_सन्देशः_WITH_RESP_आवरणम्);

	यदि (channel->send_सन्देशः_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_सन्देशः_WITHOUT_RESP_आवरणम्);

	यदि (channel->get_notification_events) {
		capability = CAP_ENABLE(capability, CAP_GET_NOTIFICATIONS_आवरणम्);
		/**
		 * Check if MSI or SSE available for notification interrrupt.
		 * Priority given to MSI if both MSI and SSE are avaialble.
		 */
		यदि (ms->msi_avail)
			capability = CAP_ENABLE(capability, CAP_MSI_आवरणम्);
		अन्यथा यदि (ms->sse_avail) {
			capability = CAP_ENABLE(capability, CAP_SSE_आवरणम्);
			/* TODO: Assign SSE EVENT_ID for the channel */
		}

		/**
		 * switch_eventstate callback support means support for events
		 * state reporting supoprt. Enable events state reporting in
		 * channel capability.
		 */
		यदि (channel->switch_eventsstate)
			capability = CAP_ENABLE(capability, CAP_EVENTSSTATE_आवरणम्);
	}

	channel->attrs.capability = capability;
}

/**
 * Register a channel with MPXY framework.
 * Called by message protocol drivers
 */
पूर्णाङ्क sbi_mpxy_पञ्जिकाकोशः_channel(संरचना sbi_mpxy_channel *channel)
{
	यदि (!channel)
		प्रत्यागम SBI_EINVAL;

	यदि (!channel->owner_domain)
		प्रत्यागम SBI_EINVAL;

	यदि (sbi_mpxy_find_channel_any(channel->channel_id))
		प्रत्यागम SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	यदि (mpxy_shmem_आकार < channel->attrs.सन्देशः_दत्त_maxlen) {
		mpxy_shmem_आकार = ROUNDUP(channel->attrs.सन्देशः_दत्त_maxlen,
					  PAGE_आकार);
	}

	sbi_list_सङ्कलय_पुच्छ(&channel->शीर्ष, &mpxy_channel_list);

	प्रत्यागम SBI_OK;
}

/** Setup per domain MPXY state data */
स्थिर पूर्णाङ्क domain_mpxy_state_सज्जता(संरचना sbi_domain *dom,
					संरचना sbi_domain_state *state,
					शून्य *state_ptr)
{
	संरचना mpxy_state **dom_hartindex_गन्तव्यम्_mpxy_state_सारणी = state_ptr;
	संरचना mpxy_state *ms;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ms = sbi_zalloc(परिमाण(*ms));
		यदि (!ms)
			प्रत्यागम SBI_ENOMEM;

		/*
		 * TODO: Proper support for checking sse support from
		 * platform. Currently disable sse and use polling
		 */
		ms->msi_avail = !!sbi_irqchip_find_device_by_caps(SBI_IRQCHIP_CAPS_MSI, रिक्त);
		ms->sse_avail = false;

		sbi_mpxy_shmem_disable(ms);

		dom_hartindex_गन्तव्यम्_mpxy_state_सारणी[i] = ms;
	}

	प्रत्यागम 0;
}

/** Cleanup per domain MPXY state data */
स्थिर शून्य domain_mpxy_state_cleanup(संरचना sbi_domain *dom,
					   संरचना sbi_domain_state *state,
					   शून्य *state_ptr)
{
	संरचना mpxy_state **dom_hartindex_गन्तव्यम्_mpxy_state_सारणी = state_ptr;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts)
		sbi_free(dom_hartindex_गन्तव्यम्_mpxy_state_सारणी[i]);
}

स्थिर संरचना sbi_domain_state dmstate = {
	.state_सज्जता = domain_mpxy_state_सज्जता,
	.state_cleanup = domain_mpxy_state_cleanup,
};

/**
 * Get per-domain MPXY state pointer for a given domain and HART index
 * @param dom pointer to domain
 * @param hartindex the HART index
 *
 * @return per-domain MPXY state pointer for given HART index
 */
स्थिर संरचना mpxy_state *sbi_domain_get_mpxy_state(संरचना sbi_domain *dom,
						    u32 hartindex)
{
	संरचना mpxy_state **dom_hartindex_गन्तव्यम्_mpxy_state_सारणी;

	dom_hartindex_गन्तव्यम्_mpxy_state_सारणी = sbi_domain_state_ptr(dom, &dmstate);
	यदि (!dom_hartindex_गन्तव्यम्_mpxy_state_सारणी ||
	    !sbi_hartindex_valid(hartindex))
		प्रत्यागम रिक्त;

	प्रत्यागम dom_hartindex_गन्तव्यम्_mpxy_state_सारणी[hartindex];
}

पूर्णाङ्क sbi_mpxy_init(संरचना sbi_scratch *scratch)
{
	पूर्णाङ्क ret;

	/**
	 * Allocate per-domain and per-hart MPXY state data.
	 * The data type is "struct mpxy_state **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one() and
	 * domain_mpxy_state_setup(). Calculate needed size of memory space
	 * here.
	 */
	dmstate.state_आकार = परिमाण(संरचना mpxy_state *) * sbi_hart_गणनम्();
	ret = sbi_domain_पञ्जिकाकोशः_state(&dmstate);
	यदि (ret)
		प्रत्यागम ret;

	प्रत्यागम sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

अचिह्नित दीर्घ sbi_mpxy_get_shmem_आकार(शून्य)
{
	प्रत्यागम mpxy_shmem_आकार;
}

पूर्णाङ्क sbi_mpxy_समुच्चयः_shmem(अचिह्नित दीर्घ shmem_phys_lo,
		       अचिह्नित दीर्घ shmem_phys_hi,
		       अचिह्नित दीर्घ flags)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	अचिह्नित दीर्घ *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	यदि (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		प्रत्यागम SBI_SUCCESS;
	}

	यदि (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	यदि (shmem_phys_lo & ~PAGE_आवरणम्)
		प्रत्यागम SBI_ERR_INVALID_PARAM;

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
	यदि (shmem_phys_hi)
		प्रत्यागम SBI_ERR_INVALID_स्थानाङ्कः;

	यदि (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_आकार, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		प्रत्यागम SBI_ERR_INVALID_स्थानाङ्कः;

	/** Save the current shmem details in new shmem region */
	यदि (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
		ret_buf = (अचिह्नित दीर्घ *)(ulong)SHMEM_PHYS_ADDR(shmem_phys_hi,
								  shmem_phys_lo);
		sbi_hart_protection_अस्थायी_map_range((अचिह्नित दीर्घ)ret_buf, mpxy_shmem_आकार);
		ret_buf[0] = cpu_गन्तव्यम्_lle(ms->shmem.shmem_addr_lo);
		ret_buf[1] = cpu_गन्तव्यम्_lle(ms->shmem.shmem_addr_hi);
		sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)ret_buf, mpxy_shmem_आकार);
	}

	/** Setup the new shared memory */
	ms->shmem.shmem_addr_lo = shmem_phys_lo;
	ms->shmem.shmem_addr_hi = shmem_phys_hi;

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_mpxy_get_channel_ids(u32 start_सूचकांक)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 remaining, returned, max_channelids;
	u32 ग्रन्थिः_सूचकांक = 0, ग्रन्थिः_ret = 0;
	संरचना sbi_mpxy_channel *channel;
	u32 channels_गणनम् = 0;
	u32 *shmem_आधारः;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();

	यदि (!mpxy_shmem_enabled(ms))
		प्रत्यागम SBI_ERR_NO_SHMEM;

	sbi_list_for_each_प्रविष्टिः(channel, &mpxy_channel_list, शीर्ष) {
		यदि (mpxy_channel_visible(channel, dom))
			channels_गणनम् += 1;
	}

	यदि (start_सूचकांक > channels_गणनम्)
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	shmem_आधारः = hart_shmem_आधारः(ms);
	sbi_hart_protection_अस्थायी_map_range((अचिह्नित दीर्घ)hart_shmem_आधारः(ms), mpxy_shmem_आकार);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_आकार / परिमाण(u32)) - 2;
	/* total remaining from the start index */
	remaining = channels_गणनम् - start_सूचकांक;
	/* how many can be returned */
	returned = (remaining > max_channelids)? max_channelids : remaining;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_प्रविष्टिः(channel, &mpxy_channel_list, शीर्ष) {
		यदि (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			अनुवर्त;

		यदि (ग्रन्थिः_सूचकांक >= start_सूचकांक &&
			ग्रन्थिः_सूचकांक < (start_सूचकांक + returned)) {
			shmem_आधारः[2 + ग्रन्थिः_ret] = cpu_गन्तव्यम्_le32(channel->channel_id);
			ग्रन्थिः_ret += 1;
		}

		ग्रन्थिः_सूचकांक += 1;
	}

	/* final remaininig channel ids */
	remaining = channels_गणनम् - (start_सूचकांक + returned);

	shmem_आधारः[0] = cpu_गन्तव्यम्_le32(remaining);
	shmem_आधारः[1] = cpu_गन्तव्यम्_le32(returned);

	sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)hart_shmem_आधारः(ms), mpxy_shmem_आकार);

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_mpxy_read_attrs(u32 channel_id, u32 आधारः_attr_id, u32 attr_गणनम्)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	पूर्णाङ्क ret = SBI_SUCCESS;
	u32 *attr_ptr, अन्तः_id;
	शून्य *shmem_आधारः;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();

	यदि (!mpxy_shmem_enabled(ms))
		प्रत्यागम SBI_ERR_NO_SHMEM;

	संरचना sbi_mpxy_channel *channel = mpxy_find_channel(channel_id, dom);
	यदि (!channel)
		प्रत्यागम SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	यदि (आधारः_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		आधारः_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	यदि (!attr_गणनम् || (attr_गणनम् > (mpxy_shmem_आकार / ATTR_आकार)))
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	shmem_आधारः = hart_shmem_आधारः(ms);
	अन्तः_id = आधारः_attr_id + attr_गणनम् - 1;

	sbi_hart_protection_अस्थायी_map_range((अचिह्नित दीर्घ)hart_shmem_आधारः(ms), mpxy_shmem_आकार);

	/* Standard attributes range check */
	यदि (mpxy_is_std_attr(आधारः_attr_id)) {
		यदि (अन्तः_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			गच्छ बहिर्गमनम्;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_आधारः, &attr_ptr[आधारः_attr_id],
				    attr_गणनम्);
	} अन्यथा {
		/**
		 * Even if the message protocol driver does not provide
		 * read attribute callback, return bad range error instead
		 * of not supported to let client distinguish it from channel
		 * id not supported.
		 * Check the complate range supported for message protocol
		 * attributes. Actual supported attributes will be checked
		 * by the message protocol driver.
		 */
		यदि (!channel->read_गुणाः ||
				अन्तः_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_अन्तः) {
			ret = SBI_ERR_BAD_RANGE;
			गच्छ बहिर्गमनम्;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_गुणाः(channel,
					       (u32 *)shmem_आधारः,
					       आधारः_attr_id, attr_गणनम्);
	}
बहिर्गमनम्:
	sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)hart_shmem_आधारः(ms), mpxy_shmem_आकार);
	प्रत्यागम ret;
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
स्थिर पूर्णाङ्क mpxy_check_write_std_attr(संरचना sbi_mpxy_channel *channel,
				     u32 attr_id, u32 attr_val)
{
	संरचना sbi_mpxy_channel_attrs *attrs = &channel->attrs;
	पूर्णाङ्क ret = SBI_SUCCESS;

	विकल्प(attr_id) {
	पक्ष SBI_MPXY_ATTR_MSI_CONTROL:
		यदि (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		यदि (attr_val == 1 &&
		    (attrs->msi_info.msi_addr_lo == (u32)INVALID_ADDR) &&
		    (attrs->msi_info.msi_addr_hi == (u32)INVALID_ADDR))
			ret = SBI_ERR_DENIED;
		भङ्ग;
	पक्ष SBI_MPXY_ATTR_MSI_ADDR_LO:
	पक्ष SBI_MPXY_ATTR_MSI_ADDR_HI:
	पक्ष SBI_MPXY_ATTR_MSI_दत्त:
		ret = SBI_SUCCESS;
		भङ्ग;
	पक्ष SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		यदि (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		भङ्ग;
	स्वभाव:
		/** All RO access attributes falls under default */
		ret = SBI_ERR_BAD_RANGE;
	};

	प्रत्यागम ret;
}

/**
 * Write the attribute value
 */
स्थिर शून्य mpxy_write_std_attr(संरचना sbi_mpxy_channel *channel, u32 attr_id,
				u32 attr_val)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	संरचना sbi_mpxy_channel_attrs *attrs = &channel->attrs;

	विकल्प(attr_id) {
	पक्ष SBI_MPXY_ATTR_MSI_CONTROL:
		यदि (ms->msi_avail && attr_val <= 1)
			attrs->msi_control = attr_val;
		भङ्ग;
	पक्ष SBI_MPXY_ATTR_MSI_ADDR_LO:
		यदि (ms->msi_avail)
			attrs->msi_info.msi_addr_lo = attr_val;
		भङ्ग;
	पक्ष SBI_MPXY_ATTR_MSI_ADDR_HI:
		यदि (ms->msi_avail)
			attrs->msi_info.msi_addr_hi = attr_val;
		भङ्ग;
	पक्ष SBI_MPXY_ATTR_MSI_दत्त:
		यदि (ms->msi_avail)
			attrs->msi_info.msi_दत्त = attr_val;
		भङ्ग;
	पक्ष SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		यदि (channel->switch_eventsstate && attr_val <= 1) {
			attrs->eventsstate_ctrl = attr_val;
			 /* message protocol callback to enable/disable
			 * events state reporting. */
			channel->switch_eventsstate(attr_val);
		}

		भङ्ग;
	};
}

पूर्णाङ्क sbi_mpxy_write_attrs(u32 channel_id, u32 आधारः_attr_id, u32 attr_गणनम्)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, अन्तः_id, attr_val;
	संरचना sbi_mpxy_channel *channel;
	पूर्णाङ्क ret, mem_idx;
	शून्य *shmem_आधारः;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();

	यदि (!mpxy_shmem_enabled(ms))
		प्रत्यागम SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	यदि (!channel)
		प्रत्यागम SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	यदि (आधारः_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		आधारः_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	यदि (!attr_गणनम् || (attr_गणनम् > (mpxy_shmem_आकार / ATTR_आकार)))
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	shmem_आधारः = hart_shmem_आधारः(ms);
	अन्तः_id = आधारः_attr_id + attr_गणनम् - 1;

	sbi_hart_protection_अस्थायी_map_range((अचिह्नित दीर्घ)shmem_आधारः, mpxy_shmem_आकार);

	mem_ptr = (u32 *)shmem_आधारः;

	यदि (mpxy_is_std_attr(आधारः_attr_id)) {
		यदि (अन्तः_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			गच्छ बहिर्गमनम्;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		प्रति (attr_id = आधारः_attr_id; attr_id <= अन्तः_id; attr_id++) {
			attr_val = le32_गन्तव्यम्_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			यदि (ret)
				गच्छ बहिर्गमनम्;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		प्रति (attr_id = आधारः_attr_id; attr_id <= अन्तः_id; attr_id++) {
			attr_val = le32_गन्तव्यम्_cpu(mem_ptr[mem_idx++]);
			mpxy_write_std_attr(channel, attr_id, attr_val);
		}
	} अन्यथा {/**
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
		यदि (!channel->write_गुणाः ||
				अन्तः_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_अन्तः) {
			ret = SBI_ERR_BAD_RANGE;
			गच्छ बहिर्गमनम्;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_गुणाः(channel,
					       (u32 *)shmem_आधारः,
					       आधारः_attr_id, attr_गणनम्);
	}
बहिर्गमनम्:
	sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)shmem_आधारः, mpxy_shmem_आकार);
	प्रत्यागम ret;
}

पूर्णाङ्क sbi_mpxy_send_सन्देशः(u32 channel_id, u8 सन्देशः_id,
			  अचिह्नित दीर्घ सन्देशः_दत्त_len,
			  अचिह्नित दीर्घ *resp_दत्त_len)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	संरचना sbi_mpxy_channel *channel;
	शून्य *shmem_आधारः, *resp_buf;
	u32 resp_bufsize;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	पूर्णाङ्क ret;

	यदि (!mpxy_shmem_enabled(ms))
		प्रत्यागम SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	यदि (!channel)
		प्रत्यागम SBI_ERR_NOT_SUPPORTED;

	यदि (resp_दत्त_len && !channel->send_सन्देशः_with_response)
		प्रत्यागम SBI_ERR_NOT_SUPPORTED;

	यदि (!resp_दत्त_len && !channel->send_सन्देशः_without_response)
		प्रत्यागम SBI_ERR_NOT_SUPPORTED;

	यदि (सन्देशः_दत्त_len > mpxy_shmem_आकार ||
		सन्देशः_दत्त_len > channel->attrs.सन्देशः_दत्त_maxlen)
		प्रत्यागम SBI_ERR_INVALID_PARAM;

	shmem_आधारः = hart_shmem_आधारः(ms);
	sbi_hart_protection_अस्थायी_map_range((अचिह्नित दीर्घ)shmem_आधारः, mpxy_shmem_आकार);

	यदि (resp_दत्त_len) {
		resp_buf = shmem_आधारः;
		resp_bufsize = mpxy_shmem_आकार;
		ret = channel->send_सन्देशः_with_response(channel, सन्देशः_id,
							  shmem_आधारः,
							  सन्देशः_दत्त_len,
							  resp_buf,
							  resp_bufsize,
							  resp_दत्त_len);
	} अन्यथा {
		ret = channel->send_सन्देशः_without_response(channel, सन्देशः_id,
							     shmem_आधारः,
							     सन्देशः_दत्त_len);
	}

	sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)shmem_आधारः, mpxy_shmem_आकार);

	यदि (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		प्रत्यागम ret;
	अन्यथा यदि (ret)
		प्रत्यागम SBI_ERR_FAILED;

	यदि (resp_दत्त_len &&
	    (*resp_दत्त_len > mpxy_shmem_आकार ||
	     *resp_दत्त_len > channel->attrs.सन्देशः_दत्त_maxlen))
		प्रत्यागम SBI_ERR_FAILED;

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_mpxy_get_notification_events(u32 channel_id, अचिह्नित दीर्घ *events_len)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	संरचना sbi_mpxy_channel *channel;
	शून्य *eventsbuf, *shmem_आधारः;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	पूर्णाङ्क ret;

	यदि (!mpxy_shmem_enabled(ms))
		प्रत्यागम SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	यदि (!channel || !channel->get_notification_events)
		प्रत्यागम SBI_ERR_NOT_SUPPORTED;

	shmem_आधारः = hart_shmem_आधारः(ms);
	sbi_hart_protection_अस्थायी_map_range((अचिह्नित दीर्घ)shmem_आधारः, mpxy_shmem_आकार);
	eventsbuf = shmem_आधारः;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_आकार,
					       events_len);
	sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)shmem_आधारः, mpxy_shmem_आकार);

	यदि (ret)
		प्रत्यागम ret;

	यदि (*events_len > (mpxy_shmem_आकार - 16))
		प्रत्यागम SBI_ERR_FAILED;

	प्रत्यागम SBI_SUCCESS;
}
