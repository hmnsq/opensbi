/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
ସଂରଚନା mpxy_rpmi_mbox {
	ସଂରଚନା mbox_chan *chan;
	ଅଚଳ ସଂରଚନା mpxy_rpmi_mbox_data *mbox_data;
	ସଂରଚନା mpxy_rpmi_channel_attrs msgprot_attrs;
	ସଂରଚନା sbi_mpxy_channel channel;
	ଶୂନ୍ୟ *group_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
ସ୍ଥିର ଅଚଳ ସଂରଚନା mpxy_rpmi_service_data *mpxy_find_rpmi_srvid(u32 message_id,
					ଅଚଳ ସଂରଚନା mpxy_rpmi_mbox_data *mbox_data)
{
	ଅଚଳ ସଂରଚନା mpxy_rpmi_service_data *srv = mbox_data->service_data;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mid = 0;

	ପାଇଁ (mid = 0; srv[mid].id < mbox_data->ଅଙ୍କ_services; mid++) {
		ଯଦି (srv[mid].id == (u8)message_id)
			ଫେରନ୍ତୁ &srv[mid];
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

/** Copy attributes word size */
ସ୍ଥିର ଶୂନ୍ୟ mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 ଗଣନା_2)
{
	u32 idx;
	ପାଇଁ (idx = 0; idx < ଗଣନା_2; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mpxy_mbox_read_attributes(ସଂରଚନା sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_ଗଣନା)
{
	ସଂରଚନା mpxy_rpmi_mbox *rmb =
		container_of(channel, ସଂରଚନା mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 end_id = base_attr_id + attr_ଗଣନା - 1;

	ଯଦି (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ଫେରନ୍ତୁ SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(base_attr_id)],
			attr_ଗଣନା);

	ଫେରନ୍ତୁ SBI_OK;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = SBI_OK;

	ବାଛ(attr_id) {
	/** All RO access attributes falls under default */
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ret = SBI_EBAD_RANGE;
	};

	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ଶୂନ୍ୟ mpxy_write_attr(ସଂରଚନା mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mpxy_mbox_write_attributes(ସଂରଚନା sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_ଗଣନା)
{
	ସଂରଚନା mpxy_rpmi_mbox *rmb =
		container_of(channel, ସଂରଚନା mpxy_rpmi_mbox, channel);
	u32 end_id = base_attr_id + attr_ଗଣନା - 1;
	u32 attr_val, idx;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret, mem_idx;

	ଯଦି (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ଫେରନ୍ତୁ SBI_EBAD_RANGE;

	mem_idx = 0;
	ପାଇଁ (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	mem_idx = 0;
	ପାଇଁ (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା __mpxy_mbox_send_message(ସଂରଚନା sbi_mpxy_channel *channel,
				  u32 message_id, ଶୂନ୍ୟ *tx, u32 tx_len,
				  ଶୂନ୍ୟ *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	ସଂରଚନା mpxy_rpmi_mbox *rmb =
		container_of(channel, ସଂରଚନା mpxy_rpmi_mbox, channel);
	ଅଚଳ ସଂରଚନା mpxy_rpmi_mbox_data *data = rmb->mbox_data;
	ଅଚଳ ସଂରଚନା mpxy_rpmi_service_data *srv =
		mpxy_find_rpmi_srvid(message_id, data);
	ସଂରଚନା rpmi_message_args args = {0};
	ସଂରଚନା mbox_xfer xfer;
	u32 rx_len = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ଯଦି (!srv)
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	ଯଦି (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		ଫେରନ୍ତୁ SBI_EFAIL;

	ଯଦି (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		ଯଦି (rx_max_len < srv->min_rx_len)
			ଫେରନ୍ତୁ SBI_EFAIL;

		/* Max rx data size it can expect */
		ଯଦି (srv->max_rx_len < channel->attrs.ସନ୍ଦେଶ_data_maxlen)
			rx_len = srv->max_rx_len;
		ଅନ୍ୟଥା
			rx_len = channel->attrs.ସନ୍ଦେଶ_data_maxlen;

		args.type = RPMI_ସନ୍ଦେଶ_NORMAL_REQUEST;
		args.flags = (rx) ? 0 : RPMI_ସନ୍ଦେଶ_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &args,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	ଅନ୍ୟଥା {
		args.type = RPMI_ସନ୍ଦେଶ_POSTED_REQUEST;
		args.flags = RPMI_ସନ୍ଦେଶ_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &args,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	ଯଦି (data->xfer_group)
		ret = data->xfer_group(rmb->group_context, rmb->chan, &xfer);
	ଅନ୍ୟଥା
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	ଯଦି (ack_len)
		*ack_len = args.rx_data_len;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mpxy_mbox_send_message_withresp(ସଂରଚନା sbi_mpxy_channel *channel,
				  u32 message_id, ଶୂନ୍ୟ *tx, u32 tx_len,
				  ଶୂନ୍ୟ *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	ଫେରନ୍ତୁ __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mpxy_mbox_send_message_withoutresp(ସଂରଚନା sbi_mpxy_channel *channel,
				  u32 message_id, ଶୂନ୍ୟ *tx, u32 tx_len)
{
	ଫେରନ୍ତୁ __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 ଖାଲି, 0, ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mpxy_rpmi_mbox_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ଅଚଳ ସଂରଚନା fdt_match *match)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_data_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	ଅଚଳ ସଂରଚନା mpxy_rpmi_mbox_data *data = match->data;
	ସଂରଚନା mpxy_rpmi_mbox *rmb;
	ସଂରଚନା mbox_chan *chan;
	ଅଚଳ fdt32_t *val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(ଆକାର(*rmb));
	ଯଦି (!rmb)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ଯଦି (rc) {
		rc = SBI_ENODEV;
		ଯାଅ fail_free_client;
	}

	/* Match channel service group id */
	ଯଦି (data->servicegrp_id != chan->chan_args[0]) {
		rc = SBI_EINVAL;
		ଯାଅ fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_VERSION,
				     &pro_ver);
	ଯଦି (rc)
		ଯାଅ fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_DATA_LEN,
				     &max_data_len);
	ଯଦି (rc)
		ଯାଅ fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	ଯଦି (rc)
		ଯାଅ fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	ଯଦି (rc)
		ଯାଅ fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION,
				     &servicegrp_ver);
	ଯଦି (rc)
		ଯାଅ fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	ଯଦି (rc)
		ଯାଅ fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_VERSION,
				     &impl_ver);
	ଯଦି (rc)
		ଯାଅ fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	ଯଦି (len > 0 && val) {
		channel_id = fdt32_to_cpu(*val);
	} ଅନ୍ୟଥା {
		rc = SBI_ENODEV;
		ଯାଅ fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &ମୂଳ;
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
	rmb->channel.attrs.ସନ୍ଦେଶ_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.ସନ୍ଦେଶ_proto_version = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.ସନ୍ଦେଶ_data_maxlen = max_data_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.ସନ୍ଦେଶ_send_timeout = tx_tout;
	rmb->channel.attrs.ସନ୍ଦେଶ_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = data->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_data = data;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	ଯଦି (data->setup_group) {
		rc = data->setup_group(&rmb->group_context, chan, data);
		ଯଦି (rc)
			ଯାଅ fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_register_channel(&rmb->channel);
	ଯଦି (rc) {
		ଯଦି (data->cleanup_group)
			data->cleanup_group(rmb->group_context);
		ଯାଅ fail_free_chan;
	}

	ଫେରନ୍ତୁ SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	ଫେରନ୍ତୁ rc;
}
