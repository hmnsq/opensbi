/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#அந்தர்பாவ <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
கட்டமைப்பு mpxy_rpmi_mbox {
	கட்டமைப்பு mbox_chan *chan;
	மாறிலி கட்டமைப்பு mpxy_rpmi_mbox_data *mbox_data;
	கட்டமைப்பு mpxy_rpmi_channel_attrs msgprot_attrs;
	கட்டமைப்பு sbi_mpxy_channel channel;
	வெறுமை *group_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
நிலையான மாறிலி கட்டமைப்பு mpxy_rpmi_service_data *mpxy_find_rpmi_srvid(u32 message_id,
					மாறிலி கட்டமைப்பு mpxy_rpmi_mbox_data *mbox_data)
{
	மாறிலி கட்டமைப்பு mpxy_rpmi_service_data *srv = mbox_data->service_data;
	முழு mid = 0;

	க்கு (mid = 0; srv[mid].id < mbox_data->இலக்கம்_services; mid++) {
		எனில் (srv[mid].id == (u8)message_id)
			திரும்பு &srv[mid];
	}

	திரும்பு வெற்று;
}

/** Copy attributes word size */
நிலையான வெறுமை mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 மொத்தம்)
{
	u32 idx;
	க்கு (idx = 0; idx < மொத்தம்; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

நிலையான முழு mpxy_mbox_read_attributes(கட்டமைப்பு sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_மொத்தம்)
{
	கட்டமைப்பு mpxy_rpmi_mbox *rmb =
		container_of(channel, கட்டமைப்பு mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 end_id = base_attr_id + attr_மொத்தம் - 1;

	எனில் (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		திரும்பு SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(base_attr_id)],
			attr_மொத்தம்);

	திரும்பு SBI_OK;
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
நிலையான முழு mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	முழு ret = SBI_OK;

	தேர்வு(attr_id) {
	/** All RO access attributes falls under default */
	இயல்பு:
		ret = SBI_EBAD_RANGE;
	};

	திரும்பு ret;
}

நிலையான வெறுமை mpxy_write_attr(கட்டமைப்பு mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

நிலையான முழு mpxy_mbox_write_attributes(கட்டமைப்பு sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_மொத்தம்)
{
	கட்டமைப்பு mpxy_rpmi_mbox *rmb =
		container_of(channel, கட்டமைப்பு mpxy_rpmi_mbox, channel);
	u32 end_id = base_attr_id + attr_மொத்தம் - 1;
	u32 attr_val, idx;
	முழு ret, mem_idx;

	எனில் (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		திரும்பு SBI_EBAD_RANGE;

	mem_idx = 0;
	க்கு (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		எனில் (ret)
			திரும்பு ret;
	}

	mem_idx = 0;
	க்கு (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	திரும்பு SBI_OK;
}

நிலையான முழு __mpxy_mbox_send_message(கட்டமைப்பு sbi_mpxy_channel *channel,
				  u32 message_id, வெறுமை *tx, u32 tx_len,
				  வெறுமை *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	கட்டமைப்பு mpxy_rpmi_mbox *rmb =
		container_of(channel, கட்டமைப்பு mpxy_rpmi_mbox, channel);
	மாறிலி கட்டமைப்பு mpxy_rpmi_mbox_data *data = rmb->mbox_data;
	மாறிலி கட்டமைப்பு mpxy_rpmi_service_data *srv =
		mpxy_find_rpmi_srvid(message_id, data);
	கட்டமைப்பு rpmi_message_args args = {0};
	கட்டமைப்பு mbox_xfer xfer;
	u32 rx_len = 0;
	முழு ret;

	எனில் (!srv)
		திரும்பு SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	எனில் (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		திரும்பு SBI_EFAIL;

	எனில் (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		எனில் (rx_max_len < srv->min_rx_len)
			திரும்பு SBI_EFAIL;

		/* Max rx data size it can expect */
		எனில் (srv->max_rx_len < channel->attrs.செய்தி_data_maxlen)
			rx_len = srv->max_rx_len;
		இல்லையெனில்
			rx_len = channel->attrs.செய்தி_data_maxlen;

		args.type = RPMI_செய்தி_NORMAL_REQUEST;
		args.flags = (rx) ? 0 : RPMI_செய்தி_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &args,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	இல்லையெனில் {
		args.type = RPMI_செய்தி_POSTED_REQUEST;
		args.flags = RPMI_செய்தி_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &args,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	எனில் (data->xfer_group)
		ret = data->xfer_group(rmb->group_context, rmb->chan, &xfer);
	இல்லையெனில்
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	எனில் (ret)
		திரும்பு ret;

	எனில் (ack_len)
		*ack_len = args.rx_data_len;

	திரும்பு SBI_OK;
}

நிலையான முழு mpxy_mbox_send_message_withresp(கட்டமைப்பு sbi_mpxy_channel *channel,
				  u32 message_id, வெறுமை *tx, u32 tx_len,
				  வெறுமை *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	திரும்பு __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

நிலையான முழு mpxy_mbox_send_message_withoutresp(கட்டமைப்பு sbi_mpxy_channel *channel,
				  u32 message_id, வெறுமை *tx, u32 tx_len)
{
	திரும்பு __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 வெற்று, 0, வெற்று);
}

முழு mpxy_rpmi_mbox_init(மாறிலி வெறுமை *fdt, முழு nodeoff, மாறிலி கட்டமைப்பு fdt_match *match)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_data_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	மாறிலி கட்டமைப்பு mpxy_rpmi_mbox_data *data = match->data;
	கட்டமைப்பு mpxy_rpmi_mbox *rmb;
	கட்டமைப்பு mbox_chan *chan;
	மாறிலி fdt32_t *val;
	முழு rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(அளவு(*rmb));
	எனில் (!rmb)
		திரும்பு SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	எனில் (rc) {
		rc = SBI_ENODEV;
		செல் fail_free_client;
	}

	/* Match channel service group id */
	எனில் (data->servicegrp_id != chan->chan_args[0]) {
		rc = SBI_EINVAL;
		செல் fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_VERSION,
				     &pro_ver);
	எனில் (rc)
		செல் fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_DATA_LEN,
				     &max_data_len);
	எனில் (rc)
		செல் fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	எனில் (rc)
		செல் fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	எனில் (rc)
		செல் fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION,
				     &servicegrp_ver);
	எனில் (rc)
		செல் fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	எனில் (rc)
		செல் fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_VERSION,
				     &impl_ver);
	எனில் (rc)
		செல் fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	எனில் (len > 0 && val) {
		channel_id = fdt32_to_cpu(*val);
	} இல்லையெனில் {
		rc = SBI_ENODEV;
		செல் fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &வேர்;
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
	rmb->channel.attrs.செய்தி_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.செய்தி_proto_version = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.செய்தி_data_maxlen = max_data_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.செய்தி_send_timeout = tx_tout;
	rmb->channel.attrs.செய்தி_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = data->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_data = data;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	எனில் (data->setup_group) {
		rc = data->setup_group(&rmb->group_context, chan, data);
		எனில் (rc)
			செல் fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_register_channel(&rmb->channel);
	எனில் (rc) {
		எனில் (data->cleanup_group)
			data->cleanup_group(rmb->group_context);
		செல் fail_free_chan;
	}

	திரும்பு SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	திரும்பு rc;
}
