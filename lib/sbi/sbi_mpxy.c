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
स्थिर अचिन्हित दीर्घ mpxy_shmem_size = PAGE_SIZE;

/** List of MPXY proxy channels */
स्थिर SBI_LIST_HEAD(mpxy_channel_list);

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
#define CAP_SEND_सन्देश_WITH_RESP_POS	3
#define CAP_SEND_सन्देश_WITH_RESP_MASK	(1U << CAP_SEND_सन्देश_WITH_RESP_POS)

/** Channel Capability - Send Message Without Response function support */
#define CAP_SEND_सन्देश_WITHOUT_RESP_POS	4
#define CAP_SEND_सन्देश_WITHOUT_RESP_MASK	(1U << CAP_SEND_सन्देश_WITHOUT_RESP_POS)

/** Channel Capability - Get Notification function support */
#define CAP_GET_NOTIFICATIONS_POS	5
#define CAP_GET_NOTIFICATIONS_MASK	(1U << CAP_GET_NOTIFICATIONS_POS)

/** Helpers to enable/disable channel capability bits
 * _c: capability variable
 * _m: capability mask
 */
#define CAP_ENABLE(_c, _m)	घुसाउ_FIELD(_c, _m, 1)
#define CAP_DISABLE(_c, _m)	घुसाउ_FIELD(_c, _m, 0)
#define CAP_GET(_c, _m)		EXTRACT_FIELD(_c, _m)

#define SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
संरचना mpxy_shmem {
	अचिन्हित दीर्घ shmem_addr_lo;
	अचिन्हित दीर्घ shmem_addr_hi;
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
#define sbi_domain_mpxy_state_thishart_ptr()			\
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
	फर्काऊ (ms->shmem.shmem_addr_lo == INVALID_ADDR
		&& ms->shmem.shmem_addr_hi == INVALID_ADDR) ?
		false : true;
}

/** Get hart shared memory base address */
स्थिर अन्तःस्थ शून्य *hart_shmem_base(संरचना mpxy_state *ms)
{
	फर्काऊ (शून्य *)(अचिन्हित दीर्घ)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#define assert_field_offset(field, attr_offset)				\
	_Static_assert(							\
		((दुरत्व(संरचना sbi_mpxy_channel_attrs, field)) /	\
		 परिमाण(u32)) == attr_offset,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_field_offset(सन्देश_proto_id, SBI_MPXY_ATTR_सन्देश_PROT_ID);
assert_field_offset(सन्देश_proto_version, SBI_MPXY_ATTR_सन्देश_PROT_VER);
assert_field_offset(सन्देश_data_maxlen, SBI_MPXY_ATTR_सन्देश_MAX_LEN);
assert_field_offset(सन्देश_send_timeout, SBI_MPXY_ATTR_सन्देश_SEND_TIMEOUT);
assert_field_offset(सन्देश_completion_timeout, SBI_MPXY_ATTR_सन्देश_COMPLETION_TIMEOUT);
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
स्थिर अन्तःस्थ bool mpxy_is_std_attr(u32 attr_id)
{
	फर्काऊ (attr_id >> 31) ? false : true;
}

स्थिर अन्तःस्थ bool mpxy_channel_visible(संरचना sbi_mpxy_channel *channel,
					संरचना sbi_domain *dom)
{
	फर्काऊ channel->owner_domain == dom;
}

/** Find channel_id in registered channels list for this domain */
स्थिर संरचना sbi_mpxy_channel *mpxy_find_channel(u32 channel_id,
					संरचना sbi_domain *dom)
{
	संरचना sbi_mpxy_channel *channel;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		यदि (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			फर्काऊ channel;
	}

	फर्काऊ रिक्त;
}

/** Find channel_id in registered channels list */
संरचना sbi_mpxy_channel *sbi_mpxy_find_channel_any(u32 channel_id)
{
	संरचना sbi_mpxy_channel *channel;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		यदि (channel->channel_id == channel_id)
			फर्काऊ channel;
	}

	फर्काऊ रिक्त;
}

/** Copy attributes word size */
स्थिर शून्य mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 गन्ती)
{
	पूर्णाङ्क idx;
	लागि (idx = 0; idx < गन्ती; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_available(शून्य)
{
	फर्काऊ sbi_list_empty(&mpxy_channel_list) ? false : true;
}

स्थिर शून्य mpxy_std_attrs_init(संरचना sbi_mpxy_channel *channel)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_data = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	यदि (channel->send_message_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_सन्देश_WITH_RESP_MASK);

	यदि (channel->send_message_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_सन्देश_WITHOUT_RESP_MASK);

	यदि (channel->get_notification_events) {
		capability = CAP_ENABLE(capability, CAP_GET_NOTIFICATIONS_MASK);
		/**
		 * Check if MSI or SSE available for notification interrrupt.
		 * Priority given to MSI if both MSI and SSE are avaialble.
		 */
		यदि (ms->msi_avail)
			capability = CAP_ENABLE(capability, CAP_MSI_MASK);
		अन्यथा यदि (ms->sse_avail) {
			capability = CAP_ENABLE(capability, CAP_SSE_MASK);
			/* TODO: Assign SSE EVENT_ID for the channel */
		}

		/**
		 * switch_eventstate callback support means support for events
		 * state reporting supoprt. Enable events state reporting in
		 * channel capability.
		 */
		यदि (channel->switch_eventsstate)
			capability = CAP_ENABLE(capability, CAP_EVENTSSTATE_MASK);
	}

	channel->attrs.capability = capability;
}

/**
 * Register a channel with MPXY framework.
 * Called by message protocol drivers
 */
पूर्णाङ्क sbi_mpxy_register_channel(संरचना sbi_mpxy_channel *channel)
{
	यदि (!channel)
		फर्काऊ SBI_EINVAL;

	यदि (!channel->owner_domain)
		फर्काऊ SBI_EINVAL;

	यदि (sbi_mpxy_find_channel_any(channel->channel_id))
		फर्काऊ SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	यदि (mpxy_shmem_size < channel->attrs.सन्देश_data_maxlen) {
		mpxy_shmem_size = ROUNDUP(channel->attrs.सन्देश_data_maxlen,
					  PAGE_SIZE);
	}

	sbi_list_add_tail(&channel->head, &mpxy_channel_list);

	फर्काऊ SBI_OK;
}

/** Setup per domain MPXY state data */
स्थिर पूर्णाङ्क domain_mpxy_state_setup(संरचना sbi_domain *dom,
					संरचना sbi_domain_state *state,
					शून्य *state_ptr)
{
	संरचना mpxy_state **dom_hartindex_to_mpxy_state_table = state_ptr;
	संरचना mpxy_state *ms;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ms = sbi_zalloc(परिमाण(*ms));
		यदि (!ms)
			फर्काऊ SBI_ENOMEM;

		/*
		 * TODO: Proper support for checking sse support from
		 * platform. Currently disable sse and use polling
		 */
		ms->msi_avail = !!sbi_irqchip_find_device_by_caps(SBI_IRQCHIP_CAPS_MSI, रिक्त);
		ms->sse_avail = false;

		sbi_mpxy_shmem_disable(ms);

		dom_hartindex_to_mpxy_state_table[i] = ms;
	}

	फर्काऊ 0;
}

/** Cleanup per domain MPXY state data */
स्थिर शून्य domain_mpxy_state_cleanup(संरचना sbi_domain *dom,
					   संरचना sbi_domain_state *state,
					   शून्य *state_ptr)
{
	संरचना mpxy_state **dom_hartindex_to_mpxy_state_table = state_ptr;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts)
		sbi_free(dom_hartindex_to_mpxy_state_table[i]);
}

स्थिर संरचना sbi_domain_state dmstate = {
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
स्थिर संरचना mpxy_state *sbi_domain_get_mpxy_state(संरचना sbi_domain *dom,
						    u32 hartindex)
{
	संरचना mpxy_state **dom_hartindex_to_mpxy_state_table;

	dom_hartindex_to_mpxy_state_table = sbi_domain_state_ptr(dom, &dmstate);
	यदि (!dom_hartindex_to_mpxy_state_table ||
	    !sbi_hartindex_valid(hartindex))
		फर्काऊ रिक्त;

	फर्काऊ dom_hartindex_to_mpxy_state_table[hartindex];
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
	dmstate.state_size = परिमाण(संरचना mpxy_state *) * sbi_hart_गन्ती();
	ret = sbi_domain_register_state(&dmstate);
	यदि (ret)
		फर्काऊ ret;

	फर्काऊ sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

अचिन्हित दीर्घ sbi_mpxy_get_shmem_size(शून्य)
{
	फर्काऊ mpxy_shmem_size;
}

पूर्णाङ्क sbi_mpxy_set_shmem(अचिन्हित दीर्घ shmem_phys_lo,
		       अचिन्हित दीर्घ shmem_phys_hi,
		       अचिन्हित दीर्घ flags)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	अचिन्हित दीर्घ *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	यदि (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		फर्काऊ SBI_SUCCESS;
	}

	यदि (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		फर्काऊ SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	यदि (shmem_phys_lo & ~PAGE_MASK)
		फर्काऊ SBI_ERR_INVALID_PARAM;

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
		फर्काऊ SBI_ERR_INVALID_ADDRESS;

	यदि (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_size, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		फर्काऊ SBI_ERR_INVALID_ADDRESS;

	/** Save the current shmem details in new shmem region */
	यदि (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
		ret_buf = (अचिन्हित दीर्घ *)(ulong)SHMEM_PHYS_ADDR(shmem_phys_hi,
								  shmem_phys_lo);
		sbi_hart_protection_temp_map_range((अचिन्हित दीर्घ)ret_buf, mpxy_shmem_size);
		ret_buf[0] = cpu_to_lle(ms->shmem.shmem_addr_lo);
		ret_buf[1] = cpu_to_lle(ms->shmem.shmem_addr_hi);
		sbi_hart_protection_temp_unmap_range((अचिन्हित दीर्घ)ret_buf, mpxy_shmem_size);
	}

	/** Setup the new shared memory */
	ms->shmem.shmem_addr_lo = shmem_phys_lo;
	ms->shmem.shmem_addr_hi = shmem_phys_hi;

	फर्काऊ SBI_SUCCESS;
}

पूर्णाङ्क sbi_mpxy_get_channel_ids(u32 start_index)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 remaining, returned, max_channelids;
	u32 गाँठो_index = 0, गाँठो_ret = 0;
	संरचना sbi_mpxy_channel *channel;
	u32 channels_गन्ती = 0;
	u32 *shmem_base;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();

	यदि (!mpxy_shmem_enabled(ms))
		फर्काऊ SBI_ERR_NO_SHMEM;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		यदि (mpxy_channel_visible(channel, dom))
			channels_गन्ती += 1;
	}

	यदि (start_index > channels_गन्ती)
		फर्काऊ SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_temp_map_range((अचिन्हित दीर्घ)hart_shmem_base(ms), mpxy_shmem_size);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_size / परिमाण(u32)) - 2;
	/* total remaining from the start index */
	remaining = channels_गन्ती - start_index;
	/* how many can be returned */
	returned = (remaining > max_channelids)? max_channelids : remaining;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_entry(channel, &mpxy_channel_list, head) {
		यदि (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			जारी;

		यदि (गाँठो_index >= start_index &&
			गाँठो_index < (start_index + returned)) {
			shmem_base[2 + गाँठो_ret] = cpu_to_le32(channel->channel_id);
			गाँठो_ret += 1;
		}

		गाँठो_index += 1;
	}

	/* final remaininig channel ids */
	remaining = channels_गन्ती - (start_index + returned);

	shmem_base[0] = cpu_to_le32(remaining);
	shmem_base[1] = cpu_to_le32(returned);

	sbi_hart_protection_temp_unmap_range((अचिन्हित दीर्घ)hart_shmem_base(ms), mpxy_shmem_size);

	फर्काऊ SBI_SUCCESS;
}

पूर्णाङ्क sbi_mpxy_read_attrs(u32 channel_id, u32 base_attr_id, u32 attr_गन्ती)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	पूर्णाङ्क ret = SBI_SUCCESS;
	u32 *attr_ptr, end_id;
	शून्य *shmem_base;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();

	यदि (!mpxy_shmem_enabled(ms))
		फर्काऊ SBI_ERR_NO_SHMEM;

	संरचना sbi_mpxy_channel *channel = mpxy_find_channel(channel_id, dom);
	यदि (!channel)
		फर्काऊ SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	यदि (base_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		base_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		फर्काऊ SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	यदि (!attr_गन्ती || (attr_गन्ती > (mpxy_shmem_size / ATTR_SIZE)))
		फर्काऊ SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	end_id = base_attr_id + attr_गन्ती - 1;

	sbi_hart_protection_temp_map_range((अचिन्हित दीर्घ)hart_shmem_base(ms), mpxy_shmem_size);

	/* Standard attributes range check */
	यदि (mpxy_is_std_attr(base_attr_id)) {
		यदि (end_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			गच्छ out;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_base, &attr_ptr[base_attr_id],
				    attr_गन्ती);
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
		यदि (!channel->read_attributes ||
				end_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_END) {
			ret = SBI_ERR_BAD_RANGE;
			गच्छ out;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_attributes(channel,
					       (u32 *)shmem_base,
					       base_attr_id, attr_गन्ती);
	}
out:
	sbi_hart_protection_temp_unmap_range((अचिन्हित दीर्घ)hart_shmem_base(ms), mpxy_shmem_size);
	फर्काऊ ret;
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
		रोक;
	पक्ष SBI_MPXY_ATTR_MSI_ADDR_LO:
	पक्ष SBI_MPXY_ATTR_MSI_ADDR_HI:
	पक्ष SBI_MPXY_ATTR_MSI_DATA:
		ret = SBI_SUCCESS;
		रोक;
	पक्ष SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		यदि (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		रोक;
	स्वभाव:
		/** All RO access attributes falls under default */
		ret = SBI_ERR_BAD_RANGE;
	};

	फर्काऊ ret;
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
		रोक;
	पक्ष SBI_MPXY_ATTR_MSI_ADDR_LO:
		यदि (ms->msi_avail)
			attrs->msi_info.msi_addr_lo = attr_val;
		रोक;
	पक्ष SBI_MPXY_ATTR_MSI_ADDR_HI:
		यदि (ms->msi_avail)
			attrs->msi_info.msi_addr_hi = attr_val;
		रोक;
	पक्ष SBI_MPXY_ATTR_MSI_DATA:
		यदि (ms->msi_avail)
			attrs->msi_info.msi_data = attr_val;
		रोक;
	पक्ष SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		यदि (channel->switch_eventsstate && attr_val <= 1) {
			attrs->eventsstate_ctrl = attr_val;
			 /* message protocol callback to enable/disable
			 * events state reporting. */
			channel->switch_eventsstate(attr_val);
		}

		रोक;
	};
}

पूर्णाङ्क sbi_mpxy_write_attrs(u32 channel_id, u32 base_attr_id, u32 attr_गन्ती)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, end_id, attr_val;
	संरचना sbi_mpxy_channel *channel;
	पूर्णाङ्क ret, mem_idx;
	शून्य *shmem_base;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();

	यदि (!mpxy_shmem_enabled(ms))
		फर्काऊ SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	यदि (!channel)
		फर्काऊ SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	यदि (base_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		base_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		फर्काऊ SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	यदि (!attr_गन्ती || (attr_गन्ती > (mpxy_shmem_size / ATTR_SIZE)))
		फर्काऊ SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	end_id = base_attr_id + attr_गन्ती - 1;

	sbi_hart_protection_temp_map_range((अचिन्हित दीर्घ)shmem_base, mpxy_shmem_size);

	mem_ptr = (u32 *)shmem_base;

	यदि (mpxy_is_std_attr(base_attr_id)) {
		यदि (end_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			गच्छ out;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		लागि (attr_id = base_attr_id; attr_id <= end_id; attr_id++) {
			attr_val = le32_to_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			यदि (ret)
				गच्छ out;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		लागि (attr_id = base_attr_id; attr_id <= end_id; attr_id++) {
			attr_val = le32_to_cpu(mem_ptr[mem_idx++]);
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
		यदि (!channel->write_attributes ||
				end_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_END) {
			ret = SBI_ERR_BAD_RANGE;
			गच्छ out;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_attributes(channel,
					       (u32 *)shmem_base,
					       base_attr_id, attr_गन्ती);
	}
out:
	sbi_hart_protection_temp_unmap_range((अचिन्हित दीर्घ)shmem_base, mpxy_shmem_size);
	फर्काऊ ret;
}

पूर्णाङ्क sbi_mpxy_send_message(u32 channel_id, u8 सन्देश_id,
			  अचिन्हित दीर्घ सन्देश_data_len,
			  अचिन्हित दीर्घ *resp_data_len)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	संरचना sbi_mpxy_channel *channel;
	शून्य *shmem_base, *resp_buf;
	u32 resp_bufsize;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	पूर्णाङ्क ret;

	यदि (!mpxy_shmem_enabled(ms))
		फर्काऊ SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	यदि (!channel)
		फर्काऊ SBI_ERR_NOT_SUPPORTED;

	यदि (resp_data_len && !channel->send_message_with_response)
		फर्काऊ SBI_ERR_NOT_SUPPORTED;

	यदि (!resp_data_len && !channel->send_message_without_response)
		फर्काऊ SBI_ERR_NOT_SUPPORTED;

	यदि (सन्देश_data_len > mpxy_shmem_size ||
		सन्देश_data_len > channel->attrs.सन्देश_data_maxlen)
		फर्काऊ SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_temp_map_range((अचिन्हित दीर्घ)shmem_base, mpxy_shmem_size);

	यदि (resp_data_len) {
		resp_buf = shmem_base;
		resp_bufsize = mpxy_shmem_size;
		ret = channel->send_message_with_response(channel, सन्देश_id,
							  shmem_base,
							  सन्देश_data_len,
							  resp_buf,
							  resp_bufsize,
							  resp_data_len);
	} अन्यथा {
		ret = channel->send_message_without_response(channel, सन्देश_id,
							     shmem_base,
							     सन्देश_data_len);
	}

	sbi_hart_protection_temp_unmap_range((अचिन्हित दीर्घ)shmem_base, mpxy_shmem_size);

	यदि (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		फर्काऊ ret;
	अन्यथा यदि (ret)
		फर्काऊ SBI_ERR_FAILED;

	यदि (resp_data_len &&
	    (*resp_data_len > mpxy_shmem_size ||
	     *resp_data_len > channel->attrs.सन्देश_data_maxlen))
		फर्काऊ SBI_ERR_FAILED;

	फर्काऊ SBI_SUCCESS;
}

पूर्णाङ्क sbi_mpxy_get_notification_events(u32 channel_id, अचिन्हित दीर्घ *events_len)
{
	संरचना mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	संरचना sbi_mpxy_channel *channel;
	शून्य *eventsbuf, *shmem_base;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	पूर्णाङ्क ret;

	यदि (!mpxy_shmem_enabled(ms))
		फर्काऊ SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	यदि (!channel || !channel->get_notification_events)
		फर्काऊ SBI_ERR_NOT_SUPPORTED;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_temp_map_range((अचिन्हित दीर्घ)shmem_base, mpxy_shmem_size);
	eventsbuf = shmem_base;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_size,
					       events_len);
	sbi_hart_protection_temp_unmap_range((अचिन्हित दीर्घ)shmem_base, mpxy_shmem_size);

	यदि (ret)
		फर्काऊ ret;

	यदि (*events_len > (mpxy_shmem_size - 16))
		फर्काऊ SBI_ERR_FAILED;

	फर्काऊ SBI_SUCCESS;
}
