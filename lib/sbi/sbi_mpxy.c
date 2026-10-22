/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_hart_protection.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_irqchip.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_mpxy.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_byteorder.h>

/** Shared memory size across all harts */
अटल अचिन्हित दीर्घ mpxy_shmem_आकार = PAGE_आकार;

/** List of MPXY proxy channels */
अटल SBI_LIST_शीर्ष(mpxy_channel_list);

/** Invalid Physical Address(all bits 1) */
#घोषणा INVALID_ADDR		(-1UL)

/** MPXY Attribute size in bytes */
#घोषणा ATTR_आकार		(4)

/** Channel Capability - MSI */
#घोषणा CAP_MSI_POS		0
#घोषणा CAP_MSI_MASK		(1U << CAP_MSI_POS)

/** Channel Capability - SSE */
#घोषणा CAP_SSE_POS		1
#घोषणा CAP_SSE_MASK		(1U << CAP_SSE_POS)

/** Channel Capability - Events State */
#घोषणा CAP_EVENTSSTATE_POS	2
#घोषणा CAP_EVENTSSTATE_MASK	(1U << CAP_EVENTSSTATE_POS)

/** Channel Capability - Send Message With Response function support */
#घोषणा CAP_SEND_संदेश_WITH_RESP_POS	3
#घोषणा CAP_SEND_संदेश_WITH_RESP_MASK	(1U << CAP_SEND_संदेश_WITH_RESP_POS)

/** Channel Capability - Send Message Without Response function support */
#घोषणा CAP_SEND_संदेश_WITHOUT_RESP_POS	4
#घोषणा CAP_SEND_संदेश_WITHOUT_RESP_MASK	(1U << CAP_SEND_संदेश_WITHOUT_RESP_POS)

/** Channel Capability - Get Notification function support */
#घोषणा CAP_GET_NOTIFICATIONS_POS	5
#घोषणा CAP_GET_NOTIFICATIONS_MASK	(1U << CAP_GET_NOTIFICATIONS_POS)

/** Helpers to enable/disable channel capability bits
 * _c: capability variable
 * _m: capability mask
 */
#घोषणा CAP_ENABLE(_c, _m)	जोड़ो_FIELD(_c, _m, 1)
#घोषणा CAP_DISABLE(_c, _m)	जोड़ो_FIELD(_c, _m, 0)
#घोषणा CAP_GET(_c, _m)		EXTRACT_FIELD(_c, _m)

#घोषणा SHMEM_PHYS_ADDR(_hi, _lo) (_lo)

/** Per hart shared memory */
काष्ठा mpxy_shmem {
	अचिन्हित दीर्घ shmem_addr_lo;
	अचिन्हित दीर्घ shmem_addr_hi;
};

काष्ठा mpxy_state {
	/* MSI support in MPXY */
	bool msi_avail;
	/* SSE support in MPXY */
	bool sse_avail;
	/* MPXY Shared memory details */
	काष्ठा mpxy_shmem shmem;
};

अटल काष्ठा mpxy_state *sbi_domain_get_mpxy_state(काष्ठा sbi_domain *dom,
						    u32 hartindex);

/** Macro to obtain the current hart's MPXY state pointer in current domain */
#घोषणा sbi_domain_mpxy_state_thishart_ptr()			\
	sbi_domain_get_mpxy_state(sbi_domain_thishart_ptr(),	\
				  current_hartindex())

/** Disable hart shared memory */
अटल अंतरभूत व्योम sbi_mpxy_shmem_disable(काष्ठा mpxy_state *ms)
{
	ms->shmem.shmem_addr_lo = INVALID_ADDR;
	ms->shmem.shmem_addr_hi = INVALID_ADDR;
}

/** Check if shared memory is already setup on hart */
अटल अंतरभूत bool mpxy_shmem_enabled(काष्ठा mpxy_state *ms)
{
	वापस (ms->shmem.shmem_addr_lo == INVALID_ADDR
		&& ms->shmem.shmem_addr_hi == INVALID_ADDR) ?
		false : true;
}

/** Get hart shared memory base address */
अटल अंतरभूत व्योम *hart_shmem_base(काष्ठा mpxy_state *ms)
{
	वापस (व्योम *)(अचिन्हित दीर्घ)SHMEM_PHYS_ADDR(ms->shmem.shmem_addr_hi,
						ms->shmem.shmem_addr_lo);
}

/** Make sure all attributes are packed for direct memcpy in ATTR_READ */
#घोषणा assert_field_offset(field, attr_offset)				\
	_Static_assert(							\
		((दुरत्व(काष्ठा sbi_mpxy_channel_attrs, field)) /	\
		 माप(u32)) == attr_offset,				\
		"field " #field						\
		" from struct sbi_mpxy_channel_attrs invalid offset, expected " #attr_offset)

assert_field_offset(संदेश_proto_id, SBI_MPXY_ATTR_संदेश_PROT_ID);
assert_field_offset(संदेश_proto_version, SBI_MPXY_ATTR_संदेश_PROT_VER);
assert_field_offset(संदेश_तथ्य_maxlen, SBI_MPXY_ATTR_संदेश_MAX_LEN);
assert_field_offset(संदेश_send_timeout, SBI_MPXY_ATTR_संदेश_SEND_TIMEOUT);
assert_field_offset(संदेश_completion_timeout, SBI_MPXY_ATTR_संदेश_COMPLETION_TIMEOUT);
assert_field_offset(capability, SBI_MPXY_ATTR_CHANNEL_CAPABILITY);
assert_field_offset(sse_event_id, SBI_MPXY_ATTR_SSE_EVENT_ID);
assert_field_offset(msi_control, SBI_MPXY_ATTR_MSI_CONTROL);
assert_field_offset(msi_info.msi_addr_lo, SBI_MPXY_ATTR_MSI_ADDR_LO);
assert_field_offset(msi_info.msi_addr_hi, SBI_MPXY_ATTR_MSI_ADDR_HI);
assert_field_offset(msi_info.msi_तथ्य, SBI_MPXY_ATTR_MSI_तथ्य);
assert_field_offset(eventsstate_ctrl, SBI_MPXY_ATTR_EVENTS_STATE_CONTROL);

/**
 * Check if the attribute is a standard attribute or
 * a message protocol specific attribute
 * attr_id[31] = 0 for standard
 * attr_id[31] = 1 for message protocol specific
 */
अटल अंतरभूत bool mpxy_is_std_attr(u32 attr_id)
{
	वापस (attr_id >> 31) ? false : true;
}

अटल अंतरभूत bool mpxy_channel_visible(काष्ठा sbi_mpxy_channel *channel,
					काष्ठा sbi_domain *dom)
{
	वापस channel->owner_domain == dom;
}

/** Find channel_id in registered channels list for this domain */
अटल काष्ठा sbi_mpxy_channel *mpxy_find_channel(u32 channel_id,
					काष्ठा sbi_domain *dom)
{
	काष्ठा sbi_mpxy_channel *channel;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, शीर्ष) {
		अगर (channel->channel_id == channel_id &&
		    mpxy_channel_visible(channel, dom))
			वापस channel;
	}

	वापस शून्य;
}

/** Find channel_id in registered channels list */
काष्ठा sbi_mpxy_channel *sbi_mpxy_find_channel_any(u32 channel_id)
{
	काष्ठा sbi_mpxy_channel *channel;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, शीर्ष) {
		अगर (channel->channel_id == channel_id)
			वापस channel;
	}

	वापस शून्य;
}

/** Copy attributes word size */
अटल व्योम mpxy_copy_std_attrs(u32 *outmem, u32 *inmem, u32 गिनती)
{
	पूर्णांक idx;
	क्रम (idx = 0; idx < गिनती; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

/** Check if any channel is registered with mpxy framework */
bool sbi_mpxy_channel_available(व्योम)
{
	वापस sbi_list_empty(&mpxy_channel_list) ? false : true;
}

अटल व्योम mpxy_std_attrs_init(काष्ठा sbi_mpxy_channel *channel)
{
	काष्ठा mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 capability = 0;

	/* Reset values */
	channel->attrs.msi_control = 0;
	channel->attrs.msi_info.msi_तथ्य = 0;
	channel->attrs.msi_info.msi_addr_lo = 0;
	channel->attrs.msi_info.msi_addr_hi = 0;
	channel->attrs.capability = 0;
	channel->attrs.eventsstate_ctrl = 0;

	अगर (channel->send_message_with_response)
		capability = CAP_ENABLE(capability, CAP_SEND_संदेश_WITH_RESP_MASK);

	अगर (channel->send_message_without_response)
		capability = CAP_ENABLE(capability, CAP_SEND_संदेश_WITHOUT_RESP_MASK);

	अगर (channel->get_notification_events) {
		capability = CAP_ENABLE(capability, CAP_GET_NOTIFICATIONS_MASK);
		/**
		 * Check if MSI or SSE available for notification interrrupt.
		 * Priority given to MSI if both MSI and SSE are avaialble.
		 */
		अगर (ms->msi_avail)
			capability = CAP_ENABLE(capability, CAP_MSI_MASK);
		अन्यथा अगर (ms->sse_avail) {
			capability = CAP_ENABLE(capability, CAP_SSE_MASK);
			/* TODO: Assign SSE EVENT_ID for the channel */
		}

		/**
		 * switch_eventstate callback support means support for events
		 * state reporting supoprt. Enable events state reporting in
		 * channel capability.
		 */
		अगर (channel->switch_eventsstate)
			capability = CAP_ENABLE(capability, CAP_EVENTSSTATE_MASK);
	}

	channel->attrs.capability = capability;
}

/**
 * Register a channel with MPXY framework.
 * Called by message protocol drivers
 */
पूर्णांक sbi_mpxy_register_channel(काष्ठा sbi_mpxy_channel *channel)
{
	अगर (!channel)
		वापस SBI_EINVAL;

	अगर (!channel->owner_domain)
		वापस SBI_EINVAL;

	अगर (sbi_mpxy_find_channel_any(channel->channel_id))
		वापस SBI_EALREADY;

	/* Initialize channel specific attributes */
	mpxy_std_attrs_init(channel);

	/* Update shared memory size if required */
	अगर (mpxy_shmem_आकार < channel->attrs.संदेश_तथ्य_maxlen) {
		mpxy_shmem_आकार = ROUNDUP(channel->attrs.संदेश_तथ्य_maxlen,
					  PAGE_आकार);
	}

	sbi_list_add_पुच्छ(&channel->शीर्ष, &mpxy_channel_list);

	वापस SBI_OK;
}

/** Setup per domain MPXY state data */
अटल पूर्णांक domain_mpxy_state_setup(काष्ठा sbi_domain *dom,
					काष्ठा sbi_domain_state *state,
					व्योम *state_ptr)
{
	काष्ठा mpxy_state **dom_hartindex_to_mpxy_state_table = state_ptr;
	काष्ठा mpxy_state *ms;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ms = sbi_zalloc(माप(*ms));
		अगर (!ms)
			वापस SBI_ENOMEM;

		/*
		 * TODO: Proper support for checking sse support from
		 * platform. Currently disable sse and use polling
		 */
		ms->msi_avail = !!sbi_irqchip_find_device_by_caps(SBI_IRQCHIP_CAPS_MSI, शून्य);
		ms->sse_avail = false;

		sbi_mpxy_shmem_disable(ms);

		dom_hartindex_to_mpxy_state_table[i] = ms;
	}

	वापस 0;
}

/** Cleanup per domain MPXY state data */
अटल व्योम domain_mpxy_state_cleanup(काष्ठा sbi_domain *dom,
					   काष्ठा sbi_domain_state *state,
					   व्योम *state_ptr)
{
	काष्ठा mpxy_state **dom_hartindex_to_mpxy_state_table = state_ptr;
	u32 i;

	sbi_hartmask_for_each_hartindex(i, dom->possible_harts)
		sbi_free(dom_hartindex_to_mpxy_state_table[i]);
}

अटल काष्ठा sbi_domain_state dmstate = {
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
अटल काष्ठा mpxy_state *sbi_domain_get_mpxy_state(काष्ठा sbi_domain *dom,
						    u32 hartindex)
{
	काष्ठा mpxy_state **dom_hartindex_to_mpxy_state_table;

	dom_hartindex_to_mpxy_state_table = sbi_domain_state_ptr(dom, &dmstate);
	अगर (!dom_hartindex_to_mpxy_state_table ||
	    !sbi_hartindex_valid(hartindex))
		वापस शून्य;

	वापस dom_hartindex_to_mpxy_state_table[hartindex];
}

पूर्णांक sbi_mpxy_init(काष्ठा sbi_scratch *scratch)
{
	पूर्णांक ret;

	/**
	 * Allocate per-domain and per-hart MPXY state data.
	 * The data type is "struct mpxy_state **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one() and
	 * domain_mpxy_state_setup(). Calculate needed size of memory space
	 * here.
	 */
	dmstate.state_आकार = माप(काष्ठा mpxy_state *) * sbi_hart_गिनती();
	ret = sbi_domain_register_state(&dmstate);
	अगर (ret)
		वापस ret;

	वापस sbi_platform_mpxy_init(sbi_platform_ptr(scratch));
}

अचिन्हित दीर्घ sbi_mpxy_get_shmem_आकार(व्योम)
{
	वापस mpxy_shmem_आकार;
}

पूर्णांक sbi_mpxy_set_shmem(अचिन्हित दीर्घ shmem_phys_lo,
		       अचिन्हित दीर्घ shmem_phys_hi,
		       अचिन्हित दीर्घ flags)
{
	काष्ठा mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	अचिन्हित दीर्घ *ret_buf;

	/** Disable shared memory if both hi and lo have all bit 1s */
	अगर (shmem_phys_lo == INVALID_ADDR &&
	    shmem_phys_hi == INVALID_ADDR) {
		sbi_mpxy_shmem_disable(ms);
		वापस SBI_SUCCESS;
	}

	अगर (flags >= SBI_EXT_MPXY_SHMEM_FLAG_MAX_IDX)
		वापस SBI_ERR_INVALID_PARAM;

	/** Check shared memory size and address aligned to 4K Page */
	अगर (shmem_phys_lo & ~PAGE_MASK)
		वापस SBI_ERR_INVALID_PARAM;

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
	अगर (shmem_phys_hi)
		वापस SBI_ERR_INVALID_ADDRESS;

	अगर (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
				SHMEM_PHYS_ADDR(shmem_phys_hi, shmem_phys_lo),
				mpxy_shmem_आकार, PRV_S,
				SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		वापस SBI_ERR_INVALID_ADDRESS;

	/** Save the current shmem details in new shmem region */
	अगर (flags == SBI_EXT_MPXY_SHMEM_FLAG_OVERWRITE_RETURN) {
		ret_buf = (अचिन्हित दीर्घ *)(ulong)SHMEM_PHYS_ADDR(shmem_phys_hi,
								  shmem_phys_lo);
		sbi_hart_protection_अस्थायी_map_range((अचिन्हित दीर्घ)ret_buf, mpxy_shmem_आकार);
		ret_buf[0] = cpu_to_lle(ms->shmem.shmem_addr_lo);
		ret_buf[1] = cpu_to_lle(ms->shmem.shmem_addr_hi);
		sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)ret_buf, mpxy_shmem_आकार);
	}

	/** Setup the new shared memory */
	ms->shmem.shmem_addr_lo = shmem_phys_lo;
	ms->shmem.shmem_addr_hi = shmem_phys_hi;

	वापस SBI_SUCCESS;
}

पूर्णांक sbi_mpxy_get_channel_ids(u32 start_सूचकांक)
{
	काष्ठा mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 remaining, returned, max_channelids;
	u32 गाँठ_सूचकांक = 0, गाँठ_ret = 0;
	काष्ठा sbi_mpxy_channel *channel;
	u32 channels_गिनती = 0;
	u32 *shmem_base;
	काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();

	अगर (!mpxy_shmem_enabled(ms))
		वापस SBI_ERR_NO_SHMEM;

	sbi_list_for_each_entry(channel, &mpxy_channel_list, शीर्ष) {
		अगर (mpxy_channel_visible(channel, dom))
			channels_गिनती += 1;
	}

	अगर (start_सूचकांक > channels_गिनती)
		वापस SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_अस्थायी_map_range((अचिन्हित दीर्घ)hart_shmem_base(ms), mpxy_shmem_आकार);

	/** number of channel ids which can be stored in shmem adjusting
	 * for remaining and returned fields */
	max_channelids = (mpxy_shmem_आकार / माप(u32)) - 2;
	/* total remaining from the start index */
	remaining = channels_गिनती - start_सूचकांक;
	/* how many can be returned */
	returned = (remaining > max_channelids)? max_channelids : remaining;

	// Iterate over the list of channels to get the channel ids.
	sbi_list_for_each_entry(channel, &mpxy_channel_list, शीर्ष) {
		अगर (!mpxy_channel_visible(channel, sbi_domain_thishart_ptr()))
			जारी;

		अगर (गाँठ_सूचकांक >= start_सूचकांक &&
			गाँठ_सूचकांक < (start_सूचकांक + returned)) {
			shmem_base[2 + गाँठ_ret] = cpu_to_le32(channel->channel_id);
			गाँठ_ret += 1;
		}

		गाँठ_सूचकांक += 1;
	}

	/* final remaininig channel ids */
	remaining = channels_गिनती - (start_सूचकांक + returned);

	shmem_base[0] = cpu_to_le32(remaining);
	shmem_base[1] = cpu_to_le32(returned);

	sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)hart_shmem_base(ms), mpxy_shmem_आकार);

	वापस SBI_SUCCESS;
}

पूर्णांक sbi_mpxy_read_attrs(u32 channel_id, u32 base_attr_id, u32 attr_गिनती)
{
	काष्ठा mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	पूर्णांक ret = SBI_SUCCESS;
	u32 *attr_ptr, end_id;
	व्योम *shmem_base;
	काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();

	अगर (!mpxy_shmem_enabled(ms))
		वापस SBI_ERR_NO_SHMEM;

	काष्ठा sbi_mpxy_channel *channel = mpxy_find_channel(channel_id, dom);
	अगर (!channel)
		वापस SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	अगर (base_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		base_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		वापस SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	अगर (!attr_गिनती || (attr_गिनती > (mpxy_shmem_आकार / ATTR_आकार)))
		वापस SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	end_id = base_attr_id + attr_गिनती - 1;

	sbi_hart_protection_अस्थायी_map_range((अचिन्हित दीर्घ)hart_shmem_base(ms), mpxy_shmem_आकार);

	/* Standard attributes range check */
	अगर (mpxy_is_std_attr(base_attr_id)) {
		अगर (end_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_EBAD_RANGE;
			जाओ out;
		}

		attr_ptr = (u32 *)&channel->attrs;
		mpxy_copy_std_attrs((u32 *)shmem_base, &attr_ptr[base_attr_id],
				    attr_गिनती);
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
		अगर (!channel->read_attributes ||
				end_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_END) {
			ret = SBI_ERR_BAD_RANGE;
			जाओ out;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->read_attributes(channel,
					       (u32 *)shmem_base,
					       base_attr_id, attr_गिनती);
	}
out:
	sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)hart_shmem_base(ms), mpxy_shmem_आकार);
	वापस ret;
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
अटल पूर्णांक mpxy_check_write_std_attr(काष्ठा sbi_mpxy_channel *channel,
				     u32 attr_id, u32 attr_val)
{
	काष्ठा sbi_mpxy_channel_attrs *attrs = &channel->attrs;
	पूर्णांक ret = SBI_SUCCESS;

	चयन(attr_id) {
	हाल SBI_MPXY_ATTR_MSI_CONTROL:
		अगर (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		अगर (attr_val == 1 &&
		    (attrs->msi_info.msi_addr_lo == (u32)INVALID_ADDR) &&
		    (attrs->msi_info.msi_addr_hi == (u32)INVALID_ADDR))
			ret = SBI_ERR_DENIED;
		अवरोध;
	हाल SBI_MPXY_ATTR_MSI_ADDR_LO:
	हाल SBI_MPXY_ATTR_MSI_ADDR_HI:
	हाल SBI_MPXY_ATTR_MSI_तथ्य:
		ret = SBI_SUCCESS;
		अवरोध;
	हाल SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		अगर (attr_val > 1)
			ret = SBI_ERR_INVALID_PARAM;
		अवरोध;
	शेष:
		/** All RO access attributes falls under default */
		ret = SBI_ERR_BAD_RANGE;
	};

	वापस ret;
}

/**
 * Write the attribute value
 */
अटल व्योम mpxy_write_std_attr(काष्ठा sbi_mpxy_channel *channel, u32 attr_id,
				u32 attr_val)
{
	काष्ठा mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	काष्ठा sbi_mpxy_channel_attrs *attrs = &channel->attrs;

	चयन(attr_id) {
	हाल SBI_MPXY_ATTR_MSI_CONTROL:
		अगर (ms->msi_avail && attr_val <= 1)
			attrs->msi_control = attr_val;
		अवरोध;
	हाल SBI_MPXY_ATTR_MSI_ADDR_LO:
		अगर (ms->msi_avail)
			attrs->msi_info.msi_addr_lo = attr_val;
		अवरोध;
	हाल SBI_MPXY_ATTR_MSI_ADDR_HI:
		अगर (ms->msi_avail)
			attrs->msi_info.msi_addr_hi = attr_val;
		अवरोध;
	हाल SBI_MPXY_ATTR_MSI_तथ्य:
		अगर (ms->msi_avail)
			attrs->msi_info.msi_तथ्य = attr_val;
		अवरोध;
	हाल SBI_MPXY_ATTR_EVENTS_STATE_CONTROL:
		अगर (channel->switch_eventsstate && attr_val <= 1) {
			attrs->eventsstate_ctrl = attr_val;
			 /* message protocol callback to enable/disable
			 * events state reporting. */
			channel->switch_eventsstate(attr_val);
		}

		अवरोध;
	};
}

पूर्णांक sbi_mpxy_write_attrs(u32 channel_id, u32 base_attr_id, u32 attr_गिनती)
{
	काष्ठा mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	u32 *mem_ptr, attr_id, end_id, attr_val;
	काष्ठा sbi_mpxy_channel *channel;
	पूर्णांक ret, mem_idx;
	व्योम *shmem_base;
	काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();

	अगर (!mpxy_shmem_enabled(ms))
		वापस SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	अगर (!channel)
		वापस SBI_ERR_NOT_SUPPORTED;

	/* base attribute id is not a defined std attribute or reserved */
	अगर (base_attr_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX &&
		base_attr_id < SBI_MPXY_ATTR_MSGPROTO_ATTR_START)
		वापस SBI_ERR_INVALID_PARAM;

	/* Sanity check for base_attr_id and attr_count */
	अगर (!attr_गिनती || (attr_गिनती > (mpxy_shmem_आकार / ATTR_आकार)))
		वापस SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	end_id = base_attr_id + attr_गिनती - 1;

	sbi_hart_protection_अस्थायी_map_range((अचिन्हित दीर्घ)shmem_base, mpxy_shmem_आकार);

	mem_ptr = (u32 *)shmem_base;

	अगर (mpxy_is_std_attr(base_attr_id)) {
		अगर (end_id >= SBI_MPXY_ATTR_STD_ATTR_MAX_IDX) {
			ret = SBI_ERR_BAD_RANGE;
			जाओ out;
		}

		/** Verify the attribute ids range and values */
		mem_idx = 0;
		क्रम (attr_id = base_attr_id; attr_id <= end_id; attr_id++) {
			attr_val = le32_to_cpu(mem_ptr[mem_idx++]);
			ret = mpxy_check_write_std_attr(channel,
							attr_id, attr_val);
			अगर (ret)
				जाओ out;
		}

		/* Write the attribute ids values */
		mem_idx = 0;
		क्रम (attr_id = base_attr_id; attr_id <= end_id; attr_id++) {
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
		अगर (!channel->write_attributes ||
				end_id > SBI_MPXY_ATTR_MSGPROTO_ATTR_END) {
			ret = SBI_ERR_BAD_RANGE;
			जाओ out;
		}

		/**
		 * Function expected to return the SBI supported errors
		 * At this point both base attribute id and only the mpxy
		 * supported range been verified. Platform callback must
		 * check if the range requested is supported by message
		 * protocol driver */
		ret = channel->write_attributes(channel,
					       (u32 *)shmem_base,
					       base_attr_id, attr_गिनती);
	}
out:
	sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)shmem_base, mpxy_shmem_आकार);
	वापस ret;
}

पूर्णांक sbi_mpxy_send_message(u32 channel_id, u8 संदेश_id,
			  अचिन्हित दीर्घ संदेश_तथ्य_len,
			  अचिन्हित दीर्घ *resp_तथ्य_len)
{
	काष्ठा mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	काष्ठा sbi_mpxy_channel *channel;
	व्योम *shmem_base, *resp_buf;
	u32 resp_bufsize;
	काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();
	पूर्णांक ret;

	अगर (!mpxy_shmem_enabled(ms))
		वापस SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	अगर (!channel)
		वापस SBI_ERR_NOT_SUPPORTED;

	अगर (resp_तथ्य_len && !channel->send_message_with_response)
		वापस SBI_ERR_NOT_SUPPORTED;

	अगर (!resp_तथ्य_len && !channel->send_message_without_response)
		वापस SBI_ERR_NOT_SUPPORTED;

	अगर (संदेश_तथ्य_len > mpxy_shmem_आकार ||
		संदेश_तथ्य_len > channel->attrs.संदेश_तथ्य_maxlen)
		वापस SBI_ERR_INVALID_PARAM;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_अस्थायी_map_range((अचिन्हित दीर्घ)shmem_base, mpxy_shmem_आकार);

	अगर (resp_तथ्य_len) {
		resp_buf = shmem_base;
		resp_bufsize = mpxy_shmem_आकार;
		ret = channel->send_message_with_response(channel, संदेश_id,
							  shmem_base,
							  संदेश_तथ्य_len,
							  resp_buf,
							  resp_bufsize,
							  resp_तथ्य_len);
	} अन्यथा {
		ret = channel->send_message_without_response(channel, संदेश_id,
							     shmem_base,
							     संदेश_तथ्य_len);
	}

	sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)shmem_base, mpxy_shmem_आकार);

	अगर (ret == SBI_ERR_TIMEOUT || ret == SBI_ERR_IO)
		वापस ret;
	अन्यथा अगर (ret)
		वापस SBI_ERR_FAILED;

	अगर (resp_तथ्य_len &&
	    (*resp_तथ्य_len > mpxy_shmem_आकार ||
	     *resp_तथ्य_len > channel->attrs.संदेश_तथ्य_maxlen))
		वापस SBI_ERR_FAILED;

	वापस SBI_SUCCESS;
}

पूर्णांक sbi_mpxy_get_notification_events(u32 channel_id, अचिन्हित दीर्घ *events_len)
{
	काष्ठा mpxy_state *ms = sbi_domain_mpxy_state_thishart_ptr();
	काष्ठा sbi_mpxy_channel *channel;
	व्योम *eventsbuf, *shmem_base;
	काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();
	पूर्णांक ret;

	अगर (!mpxy_shmem_enabled(ms))
		वापस SBI_ERR_NO_SHMEM;

	channel = mpxy_find_channel(channel_id, dom);
	अगर (!channel || !channel->get_notification_events)
		वापस SBI_ERR_NOT_SUPPORTED;

	shmem_base = hart_shmem_base(ms);
	sbi_hart_protection_अस्थायी_map_range((अचिन्हित दीर्घ)shmem_base, mpxy_shmem_आकार);
	eventsbuf = shmem_base;
	ret = channel->get_notification_events(channel, eventsbuf,
					       mpxy_shmem_आकार,
					       events_len);
	sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)shmem_base, mpxy_shmem_आकार);

	अगर (ret)
		वापस ret;

	अगर (*events_len > (mpxy_shmem_आकार - 16))
		वापस SBI_ERR_FAILED;

	वापस SBI_SUCCESS;
}
