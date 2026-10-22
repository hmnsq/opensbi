/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
ഘടന mpxy_rpmi_mbox {
	ഘടന mbox_chan *chan;
	സ്ഥിരം ഘടന mpxy_rpmi_mbox_data *mbox_data;
	ഘടന mpxy_rpmi_channel_attrs msgprot_attrs;
	ഘടന sbi_mpxy_channel channel;
	ശൂന്യം *group_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
സ്ഥിതം സ്ഥിരം ഘടന mpxy_rpmi_service_data *mpxy_find_rpmi_srvid(u32 message_id,
					സ്ഥിരം ഘടന mpxy_rpmi_mbox_data *mbox_data)
{
	സ്ഥിരം ഘടന mpxy_rpmi_service_data *srv = mbox_data->service_data;
	പൂർണ്ണസംഖ്യ mid = 0;

	വേണ്ടി (mid = 0; srv[mid].id < mbox_data->അക്കം_services; mid++) {
		എങ്കിൽ (srv[mid].id == (u8)message_id)
			തിരികെ &srv[mid];
	}

	തിരികെ ശൂന്യ;
}

/** Copy attributes word size */
സ്ഥിതം ശൂന്യം mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 എണ്ണം)
{
	u32 idx;
	വേണ്ടി (idx = 0; idx < എണ്ണം; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mpxy_mbox_read_attributes(ഘടന sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_എണ്ണം)
{
	ഘടന mpxy_rpmi_mbox *rmb =
		container_of(channel, ഘടന mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 end_id = base_attr_id + attr_എണ്ണം - 1;

	എങ്കിൽ (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		തിരികെ SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(base_attr_id)],
			attr_എണ്ണം);

	തിരികെ SBI_OK;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	പൂർണ്ണസംഖ്യ ret = SBI_OK;

	തിരഞ്ഞെടുപ്പ്(attr_id) {
	/** All RO access attributes falls under default */
	സ്ഥിരസ്ഥിതി:
		ret = SBI_EBAD_RANGE;
	};

	തിരികെ ret;
}

സ്ഥിതം ശൂന്യം mpxy_write_attr(ഘടന mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mpxy_mbox_write_attributes(ഘടന sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_എണ്ണം)
{
	ഘടന mpxy_rpmi_mbox *rmb =
		container_of(channel, ഘടന mpxy_rpmi_mbox, channel);
	u32 end_id = base_attr_id + attr_എണ്ണം - 1;
	u32 attr_val, idx;
	പൂർണ്ണസംഖ്യ ret, mem_idx;

	എങ്കിൽ (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		തിരികെ SBI_EBAD_RANGE;

	mem_idx = 0;
	വേണ്ടി (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	mem_idx = 0;
	വേണ്ടി (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ __mpxy_mbox_send_message(ഘടന sbi_mpxy_channel *channel,
				  u32 message_id, ശൂന്യം *tx, u32 tx_len,
				  ശൂന്യം *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	ഘടന mpxy_rpmi_mbox *rmb =
		container_of(channel, ഘടന mpxy_rpmi_mbox, channel);
	സ്ഥിരം ഘടന mpxy_rpmi_mbox_data *data = rmb->mbox_data;
	സ്ഥിരം ഘടന mpxy_rpmi_service_data *srv =
		mpxy_find_rpmi_srvid(message_id, data);
	ഘടന rpmi_message_args args = {0};
	ഘടന mbox_xfer xfer;
	u32 rx_len = 0;
	പൂർണ്ണസംഖ്യ ret;

	എങ്കിൽ (!srv)
		തിരികെ SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	എങ്കിൽ (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		തിരികെ SBI_EFAIL;

	എങ്കിൽ (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		എങ്കിൽ (rx_max_len < srv->min_rx_len)
			തിരികെ SBI_EFAIL;

		/* Max rx data size it can expect */
		എങ്കിൽ (srv->max_rx_len < channel->attrs.സന്ദേശം_data_maxlen)
			rx_len = srv->max_rx_len;
		അല്ലെങ്കിൽ
			rx_len = channel->attrs.സന്ദേശം_data_maxlen;

		args.type = RPMI_സന്ദേശം_NORMAL_REQUEST;
		args.flags = (rx) ? 0 : RPMI_സന്ദേശം_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &args,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	അല്ലെങ്കിൽ {
		args.type = RPMI_സന്ദേശം_POSTED_REQUEST;
		args.flags = RPMI_സന്ദേശം_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &args,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	എങ്കിൽ (data->xfer_group)
		ret = data->xfer_group(rmb->group_context, rmb->chan, &xfer);
	അല്ലെങ്കിൽ
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	എങ്കിൽ (ret)
		തിരികെ ret;

	എങ്കിൽ (ack_len)
		*ack_len = args.rx_data_len;

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mpxy_mbox_send_message_withresp(ഘടന sbi_mpxy_channel *channel,
				  u32 message_id, ശൂന്യം *tx, u32 tx_len,
				  ശൂന്യം *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	തിരികെ __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mpxy_mbox_send_message_withoutresp(ഘടന sbi_mpxy_channel *channel,
				  u32 message_id, ശൂന്യം *tx, u32 tx_len)
{
	തിരികെ __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 ശൂന്യ, 0, ശൂന്യ);
}

പൂർണ്ണസംഖ്യ mpxy_rpmi_mbox_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, സ്ഥിരം ഘടന fdt_match *match)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_data_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	സ്ഥിരം ഘടന mpxy_rpmi_mbox_data *data = match->data;
	ഘടന mpxy_rpmi_mbox *rmb;
	ഘടന mbox_chan *chan;
	സ്ഥിരം fdt32_t *val;
	പൂർണ്ണസംഖ്യ rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(വലുപ്പം(*rmb));
	എങ്കിൽ (!rmb)
		തിരികെ SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	എങ്കിൽ (rc) {
		rc = SBI_ENODEV;
		പോകുക fail_free_client;
	}

	/* Match channel service group id */
	എങ്കിൽ (data->servicegrp_id != chan->chan_args[0]) {
		rc = SBI_EINVAL;
		പോകുക fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_VERSION,
				     &pro_ver);
	എങ്കിൽ (rc)
		പോകുക fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_DATA_LEN,
				     &max_data_len);
	എങ്കിൽ (rc)
		പോകുക fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	എങ്കിൽ (rc)
		പോകുക fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	എങ്കിൽ (rc)
		പോകുക fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION,
				     &servicegrp_ver);
	എങ്കിൽ (rc)
		പോകുക fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	എങ്കിൽ (rc)
		പോകുക fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_VERSION,
				     &impl_ver);
	എങ്കിൽ (rc)
		പോകുക fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	എങ്കിൽ (len > 0 && val) {
		channel_id = fdt32_to_cpu(*val);
	} അല്ലെങ്കിൽ {
		rc = SBI_ENODEV;
		പോകുക fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &വേര്;
	/* Callback for read RPMI attributes */
	rmb->channel.read_attributes = mpxy_mbox_read_attributes;
	/* Callback for write RPMI attributes */
	rmb->channel.write_attributes = mpxy_mbox_write_attributes;
	/* Callback for sending RPMI message */
	rmb->channel.send_message_with_response =
					mpxy_mbox_send_message_withresp;
	rmb->channel.send_message_without_response =
					mpxy_mbox_send_message_withoutresp;

	/* RPMI Message Protocol ID */
	rmb->channel.attrs.സന്ദേശം_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.സന്ദേശം_proto_version = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.സന്ദേശം_data_maxlen = max_data_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.സന്ദേശം_send_timeout = tx_tout;
	rmb->channel.attrs.സന്ദേശം_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = data->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_data = data;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	എങ്കിൽ (data->setup_group) {
		rc = data->setup_group(&rmb->group_context, chan, data);
		എങ്കിൽ (rc)
			പോകുക fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_register_channel(&rmb->channel);
	എങ്കിൽ (rc) {
		എങ്കിൽ (data->cleanup_group)
			data->cleanup_group(rmb->group_context);
		പോകുക fail_free_chan;
	}

	തിരികെ SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	തിരികെ rc;
}
