/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#ضمن <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
بنية mpxy_rpmi_mbox {
	بنية mbox_chan *chan;
	ثابت بنية mpxy_rpmi_mbox_data *mbox_data;
	بنية mpxy_rpmi_channel_attrs msgprot_attrs;
	بنية sbi_mpxy_channel channel;
	فراغ *group_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
ساكن ثابت بنية mpxy_rpmi_service_data *mpxy_find_rpmi_srvid(u32 message_id,
					ثابت بنية mpxy_rpmi_mbox_data *mbox_data)
{
	ثابت بنية mpxy_rpmi_service_data *srv = mbox_data->service_data;
	صحيح mid = 0;

	لكل (mid = 0; srv[mid].id < mbox_data->num_services; mid++) {
		ܐܢ (srv[mid].id == (u8)message_id)
			ܦܢܐ &srv[mid];
	}

	ܦܢܐ عدم;
}

/** Copy attributes word size */
ساكن فراغ mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 count)
{
	u32 idx;
	لكل (idx = 0; idx < count; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

ساكن صحيح mpxy_mbox_read_attributes(بنية sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_count)
{
	بنية mpxy_rpmi_mbox *rmb =
		container_of(channel, بنية mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 end_id = base_attr_id + attr_count - 1;

	ܐܢ (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ܦܢܐ SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(base_attr_id)],
			attr_count);

	ܦܢܐ SBI_OK;
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
ساكن صحيح mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	صحيح ret = SBI_OK;

	بدل(attr_id) {
	/** All RO access attributes falls under default */
	افتراضي:
		ret = SBI_EBAD_RANGE;
	};

	ܦܢܐ ret;
}

ساكن فراغ mpxy_write_attr(بنية mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

ساكن صحيح mpxy_mbox_write_attributes(بنية sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_count)
{
	بنية mpxy_rpmi_mbox *rmb =
		container_of(channel, بنية mpxy_rpmi_mbox, channel);
	u32 end_id = base_attr_id + attr_count - 1;
	u32 attr_val, idx;
	صحيح ret, mem_idx;

	ܐܢ (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ܦܢܐ SBI_EBAD_RANGE;

	mem_idx = 0;
	لكل (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		ܐܢ (ret)
			ܦܢܐ ret;
	}

	mem_idx = 0;
	لكل (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	ܦܢܐ SBI_OK;
}

ساكن صحيح __mpxy_mbox_send_message(بنية sbi_mpxy_channel *channel,
				  u32 message_id, فراغ *tx, u32 tx_len,
				  فراغ *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	بنية mpxy_rpmi_mbox *rmb =
		container_of(channel, بنية mpxy_rpmi_mbox, channel);
	ثابت بنية mpxy_rpmi_mbox_data *data = rmb->mbox_data;
	ثابت بنية mpxy_rpmi_service_data *srv =
		mpxy_find_rpmi_srvid(message_id, data);
	بنية rpmi_message_args args = {0};
	بنية mbox_xfer xfer;
	u32 rx_len = 0;
	صحيح ret;

	ܐܢ (!srv)
		ܦܢܐ SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	ܐܢ (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		ܦܢܐ SBI_EFAIL;

	ܐܢ (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		ܐܢ (rx_max_len < srv->min_rx_len)
			ܦܢܐ SBI_EFAIL;

		/* Max rx data size it can expect */
		ܐܢ (srv->max_rx_len < channel->attrs.msg_data_maxlen)
			rx_len = srv->max_rx_len;
		ܐܚܪܢܐ
			rx_len = channel->attrs.msg_data_maxlen;

		args.type = RPMI_MSG_NORMAL_REQUEST;
		args.flags = (rx) ? 0 : RPMI_MSG_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &args,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	ܐܚܪܢܐ {
		args.type = RPMI_MSG_POSTED_REQUEST;
		args.flags = RPMI_MSG_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &args,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	ܐܢ (data->xfer_group)
		ret = data->xfer_group(rmb->group_context, rmb->chan, &xfer);
	ܐܚܪܢܐ
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	ܐܢ (ret)
		ܦܢܐ ret;

	ܐܢ (ack_len)
		*ack_len = args.rx_data_len;

	ܦܢܐ SBI_OK;
}

ساكن صحيح mpxy_mbox_send_message_withresp(بنية sbi_mpxy_channel *channel,
				  u32 message_id, فراغ *tx, u32 tx_len,
				  فراغ *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	ܦܢܐ __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

ساكن صحيح mpxy_mbox_send_message_withoutresp(بنية sbi_mpxy_channel *channel,
				  u32 message_id, فراغ *tx, u32 tx_len)
{
	ܦܢܐ __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 عدم, 0, عدم);
}

صحيح mpxy_rpmi_mbox_init(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_match *match)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_data_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	ثابت بنية mpxy_rpmi_mbox_data *data = match->data;
	بنية mpxy_rpmi_mbox *rmb;
	بنية mbox_chan *chan;
	ثابت fdt32_t *val;
	صحيح rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(حجم(*rmb));
	ܐܢ (!rmb)
		ܦܢܐ SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ܐܢ (rc) {
		rc = SBI_ENODEV;
		اذهب fail_free_client;
	}

	/* Match channel service group id */
	ܐܢ (data->servicegrp_id != chan->chan_args[0]) {
		rc = SBI_EINVAL;
		اذهب fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_VERSION,
				     &pro_ver);
	ܐܢ (rc)
		اذهب fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_DATA_LEN,
				     &max_data_len);
	ܐܢ (rc)
		اذهب fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	ܐܢ (rc)
		اذهب fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	ܐܢ (rc)
		اذهب fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION,
				     &servicegrp_ver);
	ܐܢ (rc)
		اذهب fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	ܐܢ (rc)
		اذهب fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_VERSION,
				     &impl_ver);
	ܐܢ (rc)
		اذهب fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	ܐܢ (len > 0 && val) {
		channel_id = fdt32_to_cpu(*val);
	} ܐܚܪܢܐ {
		rc = SBI_ENODEV;
		اذهب fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &root;
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
	rmb->channel.attrs.msg_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.msg_proto_version = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.msg_data_maxlen = max_data_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.msg_send_timeout = tx_tout;
	rmb->channel.attrs.msg_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = data->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_data = data;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	ܐܢ (data->setup_group) {
		rc = data->setup_group(&rmb->group_context, chan, data);
		ܐܢ (rc)
			اذهب fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_register_channel(&rmb->channel);
	ܐܢ (rc) {
		ܐܢ (data->cleanup_group)
			data->cleanup_group(rmb->group_context);
		اذهب fail_free_chan;
	}

	ܦܢܐ SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	ܦܢܐ rc;
}
