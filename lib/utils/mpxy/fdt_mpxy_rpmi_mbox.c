/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
সংরচনা mpxy_rpmi_mbox {
	সংরচনা mbox_chan *chan;
	ধ্রুৱ সংরচনা mpxy_rpmi_mbox_data *mbox_data;
	সংরচনা mpxy_rpmi_channel_attrs msgprot_attrs;
	সংরচনা sbi_mpxy_channel channel;
	শূন্য *group_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
স্থির ধ্রুৱ সংরচনা mpxy_rpmi_service_data *mpxy_find_rpmi_srvid(u32 message_id,
					ধ্রুৱ সংরচনা mpxy_rpmi_mbox_data *mbox_data)
{
	ধ্রুৱ সংরচনা mpxy_rpmi_service_data *srv = mbox_data->service_data;
	পূর্ণাঙ্ক mid = 0;

	বাবে (mid = 0; srv[mid].id < mbox_data->অঙ্ক_services; mid++) {
		যদি (srv[mid].id == (u8)message_id)
			ঘূৰাই &srv[mid];
	}

	ঘূৰাই রিক্ত;
}

/** Copy attributes word size */
স্থির শূন্য mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 গুনতি)
{
	u32 idx;
	বাবে (idx = 0; idx < গুনতি; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

স্থির পূর্ণাঙ্ক mpxy_mbox_read_attributes(সংরচনা sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_গুনতি)
{
	সংরচনা mpxy_rpmi_mbox *rmb =
		container_of(channel, সংরচনা mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 end_id = base_attr_id + attr_গুনতি - 1;

	যদি (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ঘূৰাই SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(base_attr_id)],
			attr_গুনতি);

	ঘূৰাই SBI_OK;
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
স্থির পূর্ণাঙ্ক mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	পূর্ণাঙ্ক ret = SBI_OK;

	ৱিকল্প(attr_id) {
	/** All RO access attributes falls under default */
	স্ৱভাৱ:
		ret = SBI_EBAD_RANGE;
	};

	ঘূৰাই ret;
}

স্থির শূন্য mpxy_write_attr(সংরচনা mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

স্থির পূর্ণাঙ্ক mpxy_mbox_write_attributes(সংরচনা sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_গুনতি)
{
	সংরচনা mpxy_rpmi_mbox *rmb =
		container_of(channel, সংরচনা mpxy_rpmi_mbox, channel);
	u32 end_id = base_attr_id + attr_গুনতি - 1;
	u32 attr_val, idx;
	পূর্ণাঙ্ক ret, mem_idx;

	যদি (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ঘূৰাই SBI_EBAD_RANGE;

	mem_idx = 0;
	বাবে (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		যদি (ret)
			ঘূৰাই ret;
	}

	mem_idx = 0;
	বাবে (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক __mpxy_mbox_send_message(সংরচনা sbi_mpxy_channel *channel,
				  u32 message_id, শূন্য *tx, u32 tx_len,
				  শূন্য *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	সংরচনা mpxy_rpmi_mbox *rmb =
		container_of(channel, সংরচনা mpxy_rpmi_mbox, channel);
	ধ্রুৱ সংরচনা mpxy_rpmi_mbox_data *data = rmb->mbox_data;
	ধ্রুৱ সংরচনা mpxy_rpmi_service_data *srv =
		mpxy_find_rpmi_srvid(message_id, data);
	সংরচনা rpmi_message_args args = {0};
	সংরচনা mbox_xfer xfer;
	u32 rx_len = 0;
	পূর্ণাঙ্ক ret;

	যদি (!srv)
		ঘূৰাই SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	যদি (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		ঘূৰাই SBI_EFAIL;

	যদি (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		যদি (rx_max_len < srv->min_rx_len)
			ঘূৰাই SBI_EFAIL;

		/* Max rx data size it can expect */
		যদি (srv->max_rx_len < channel->attrs.বাৰ্তা_data_maxlen)
			rx_len = srv->max_rx_len;
		অন্যথা
			rx_len = channel->attrs.বাৰ্তা_data_maxlen;

		args.type = RPMI_বাৰ্তা_NORMAL_REQUEST;
		args.flags = (rx) ? 0 : RPMI_বাৰ্তা_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &args,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	অন্যথা {
		args.type = RPMI_বাৰ্তা_POSTED_REQUEST;
		args.flags = RPMI_বাৰ্তা_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &args,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	যদি (data->xfer_group)
		ret = data->xfer_group(rmb->group_context, rmb->chan, &xfer);
	অন্যথা
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	যদি (ret)
		ঘূৰাই ret;

	যদি (ack_len)
		*ack_len = args.rx_data_len;

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক mpxy_mbox_send_message_withresp(সংরচনা sbi_mpxy_channel *channel,
				  u32 message_id, শূন্য *tx, u32 tx_len,
				  শূন্য *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	ঘূৰাই __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

স্থির পূর্ণাঙ্ক mpxy_mbox_send_message_withoutresp(সংরচনা sbi_mpxy_channel *channel,
				  u32 message_id, শূন্য *tx, u32 tx_len)
{
	ঘূৰাই __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 রিক্ত, 0, রিক্ত);
}

পূর্ণাঙ্ক mpxy_rpmi_mbox_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff, ধ্রুৱ সংরচনা fdt_match *match)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_data_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	ধ্রুৱ সংরচনা mpxy_rpmi_mbox_data *data = match->data;
	সংরচনা mpxy_rpmi_mbox *rmb;
	সংরচনা mbox_chan *chan;
	ধ্রুৱ fdt32_t *val;
	পূর্ণাঙ্ক rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(পরিমাণ(*rmb));
	যদি (!rmb)
		ঘূৰাই SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	যদি (rc) {
		rc = SBI_ENODEV;
		গচ্ছ fail_free_client;
	}

	/* Match channel service group id */
	যদি (data->servicegrp_id != chan->chan_args[0]) {
		rc = SBI_EINVAL;
		গচ্ছ fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_VERSION,
				     &pro_ver);
	যদি (rc)
		গচ্ছ fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_DATA_LEN,
				     &max_data_len);
	যদি (rc)
		গচ্ছ fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	যদি (rc)
		গচ্ছ fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	যদি (rc)
		গচ্ছ fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION,
				     &servicegrp_ver);
	যদি (rc)
		গচ্ছ fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	যদি (rc)
		গচ্ছ fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_VERSION,
				     &impl_ver);
	যদি (rc)
		গচ্ছ fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	যদি (len > 0 && val) {
		channel_id = fdt32_to_cpu(*val);
	} অন্যথা {
		rc = SBI_ENODEV;
		গচ্ছ fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &মূল;
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
	rmb->channel.attrs.বাৰ্তা_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.বাৰ্তা_proto_version = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.বাৰ্তা_data_maxlen = max_data_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.বাৰ্তা_send_timeout = tx_tout;
	rmb->channel.attrs.বাৰ্তা_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = data->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_data = data;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	যদি (data->setup_group) {
		rc = data->setup_group(&rmb->group_context, chan, data);
		যদি (rc)
			গচ্ছ fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_register_channel(&rmb->channel);
	যদি (rc) {
		যদি (data->cleanup_group)
			data->cleanup_group(rmb->group_context);
		গচ্ছ fail_free_chan;
	}

	ঘূৰাই SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	ঘূৰাই rc;
}
