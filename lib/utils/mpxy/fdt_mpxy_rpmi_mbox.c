/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#अन्तर्भाव <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
संरचना mpxy_rpmi_mbox {
	संरचना mbox_chan *chan;
	ध्रुव संरचना mpxy_rpmi_mbox_data *mbox_data;
	संरचना mpxy_rpmi_channel_attrs msgprot_attrs;
	संरचना sbi_mpxy_channel channel;
	शून्य *group_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
स्थिर ध्रुव संरचना mpxy_rpmi_service_data *mpxy_find_rpmi_srvid(u32 message_id,
					ध्रुव संरचना mpxy_rpmi_mbox_data *mbox_data)
{
	ध्रुव संरचना mpxy_rpmi_service_data *srv = mbox_data->service_data;
	पूर्णाङ्क mid = 0;

	अत्थाय (mid = 0; srv[mid].id < mbox_data->अङ्कः_services; mid++) {
		सचे (srv[mid].id == (u8)message_id)
			पटिनिवत्त &srv[mid];
	}

	पटिनिवत्त रिक्त;
}

/** Copy attributes word size */
स्थिर शून्य mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 गणनम्)
{
	u32 idx;
	अत्थाय (idx = 0; idx < गणनम्; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

स्थिर पूर्णाङ्क mpxy_mbox_read_attributes(संरचना sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_गणनम्)
{
	संरचना mpxy_rpmi_mbox *rmb =
		container_of(channel, संरचना mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 end_id = base_attr_id + attr_गणनम् - 1;

	सचे (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		पटिनिवत्त SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(base_attr_id)],
			attr_गणनम्);

	पटिनिवत्त SBI_OK;
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
स्थिर पूर्णाङ्क mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	पूर्णाङ्क ret = SBI_OK;

	विकल्प(attr_id) {
	/** All RO access attributes falls under default */
	स्वभाव:
		ret = SBI_EBAD_RANGE;
	};

	पटिनिवत्त ret;
}

स्थिर शून्य mpxy_write_attr(संरचना mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

स्थिर पूर्णाङ्क mpxy_mbox_write_attributes(संरचना sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_गणनम्)
{
	संरचना mpxy_rpmi_mbox *rmb =
		container_of(channel, संरचना mpxy_rpmi_mbox, channel);
	u32 end_id = base_attr_id + attr_गणनम् - 1;
	u32 attr_val, idx;
	पूर्णाङ्क ret, mem_idx;

	सचे (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		पटिनिवत्त SBI_EBAD_RANGE;

	mem_idx = 0;
	अत्थाय (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		सचे (ret)
			पटिनिवत्त ret;
	}

	mem_idx = 0;
	अत्थाय (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क __mpxy_mbox_send_message(संरचना sbi_mpxy_channel *channel,
				  u32 message_id, शून्य *tx, u32 tx_len,
				  शून्य *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	संरचना mpxy_rpmi_mbox *rmb =
		container_of(channel, संरचना mpxy_rpmi_mbox, channel);
	ध्रुव संरचना mpxy_rpmi_mbox_data *data = rmb->mbox_data;
	ध्रुव संरचना mpxy_rpmi_service_data *srv =
		mpxy_find_rpmi_srvid(message_id, data);
	संरचना rpmi_message_args args = {0};
	संरचना mbox_xfer xfer;
	u32 rx_len = 0;
	पूर्णाङ्क ret;

	सचे (!srv)
		पटिनिवत्त SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	सचे (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		पटिनिवत्त SBI_EFAIL;

	सचे (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		सचे (rx_max_len < srv->min_rx_len)
			पटिनिवत्त SBI_EFAIL;

		/* Max rx data size it can expect */
		सचे (srv->max_rx_len < channel->attrs.सन्देशः_data_maxlen)
			rx_len = srv->max_rx_len;
		अञ्ञथा
			rx_len = channel->attrs.सन्देशः_data_maxlen;

		args.type = RPMI_सन्देशः_NORMAL_REQUEST;
		args.flags = (rx) ? 0 : RPMI_सन्देशः_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &args,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	अञ्ञथा {
		args.type = RPMI_सन्देशः_POSTED_REQUEST;
		args.flags = RPMI_सन्देशः_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &args,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	सचे (data->xfer_group)
		ret = data->xfer_group(rmb->group_context, rmb->chan, &xfer);
	अञ्ञथा
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	सचे (ret)
		पटिनिवत्त ret;

	सचे (ack_len)
		*ack_len = args.rx_data_len;

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क mpxy_mbox_send_message_withresp(संरचना sbi_mpxy_channel *channel,
				  u32 message_id, शून्य *tx, u32 tx_len,
				  शून्य *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	पटिनिवत्त __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

स्थिर पूर्णाङ्क mpxy_mbox_send_message_withoutresp(संरचना sbi_mpxy_channel *channel,
				  u32 message_id, शून्य *tx, u32 tx_len)
{
	पटिनिवत्त __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 रिक्त, 0, रिक्त);
}

पूर्णाङ्क mpxy_rpmi_mbox_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, ध्रुव संरचना fdt_match *match)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_data_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	ध्रुव संरचना mpxy_rpmi_mbox_data *data = match->data;
	संरचना mpxy_rpmi_mbox *rmb;
	संरचना mbox_chan *chan;
	ध्रुव fdt32_t *val;
	पूर्णाङ्क rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(परिमाण(*rmb));
	सचे (!rmb)
		पटिनिवत्त SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	सचे (rc) {
		rc = SBI_ENODEV;
		गच्छ fail_free_client;
	}

	/* Match channel service group id */
	सचे (data->servicegrp_id != chan->chan_args[0]) {
		rc = SBI_EINVAL;
		गच्छ fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_VERSION,
				     &pro_ver);
	सचे (rc)
		गच्छ fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_DATA_LEN,
				     &max_data_len);
	सचे (rc)
		गच्छ fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	सचे (rc)
		गच्छ fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	सचे (rc)
		गच्छ fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION,
				     &servicegrp_ver);
	सचे (rc)
		गच्छ fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	सचे (rc)
		गच्छ fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_VERSION,
				     &impl_ver);
	सचे (rc)
		गच्छ fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	सचे (len > 0 && val) {
		channel_id = fdt32_to_cpu(*val);
	} अञ्ञथा {
		rc = SBI_ENODEV;
		गच्छ fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &मूलम्;
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
	rmb->channel.attrs.सन्देशः_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.सन्देशः_proto_version = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.सन्देशः_data_maxlen = max_data_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.सन्देशः_send_timeout = tx_tout;
	rmb->channel.attrs.सन्देशः_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = data->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_data = data;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	सचे (data->setup_group) {
		rc = data->setup_group(&rmb->group_context, chan, data);
		सचे (rc)
			गच्छ fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_register_channel(&rmb->channel);
	सचे (rc) {
		सचे (data->cleanup_group)
			data->cleanup_group(rmb->group_context);
		गच्छ fail_free_chan;
	}

	पटिनिवत्त SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	पटिनिवत्त rc;
}
