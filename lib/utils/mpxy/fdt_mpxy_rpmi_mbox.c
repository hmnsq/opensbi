/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#અન્તર્ભાવ <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
સંરચના mpxy_rpmi_mbox {
	સંરચના mbox_chan *chan;
	અચલ સંરચના mpxy_rpmi_mbox_data *mbox_data;
	સંરચના mpxy_rpmi_channel_attrs msgprot_attrs;
	સંરચના sbi_mpxy_channel channel;
	શૂન્ય *group_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
સ્થિર અચલ સંરચના mpxy_rpmi_service_data *mpxy_find_rpmi_srvid(u32 message_id,
					અચલ સંરચના mpxy_rpmi_mbox_data *mbox_data)
{
	અચલ સંરચના mpxy_rpmi_service_data *srv = mbox_data->service_data;
	પૂર્ણાંક mid = 0;

	માટે (mid = 0; srv[mid].id < mbox_data->અંક_services; mid++) {
		જો (srv[mid].id == (u8)message_id)
			પાછા &srv[mid];
	}

	પાછા ખાલી;
}

/** Copy attributes word size */
સ્થિર શૂન્ય mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 ગણતરી)
{
	u32 idx;
	માટે (idx = 0; idx < ગણતરી; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

સ્થિર પૂર્ણાંક mpxy_mbox_read_attributes(સંરચના sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_ગણતરી)
{
	સંરચના mpxy_rpmi_mbox *rmb =
		container_of(channel, સંરચના mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 end_id = base_attr_id + attr_ગણતરી - 1;

	જો (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		પાછા SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(base_attr_id)],
			attr_ગણતરી);

	પાછા SBI_OK;
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
સ્થિર પૂર્ણાંક mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	પૂર્ણાંક ret = SBI_OK;

	પસંદગી(attr_id) {
	/** All RO access attributes falls under default */
	મૂળભૂત:
		ret = SBI_EBAD_RANGE;
	};

	પાછા ret;
}

સ્થિર શૂન્ય mpxy_write_attr(સંરચના mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

સ્થિર પૂર્ણાંક mpxy_mbox_write_attributes(સંરચના sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_ગણતરી)
{
	સંરચના mpxy_rpmi_mbox *rmb =
		container_of(channel, સંરચના mpxy_rpmi_mbox, channel);
	u32 end_id = base_attr_id + attr_ગણતરી - 1;
	u32 attr_val, idx;
	પૂર્ણાંક ret, mem_idx;

	જો (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		પાછા SBI_EBAD_RANGE;

	mem_idx = 0;
	માટે (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		જો (ret)
			પાછા ret;
	}

	mem_idx = 0;
	માટે (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક __mpxy_mbox_send_message(સંરચના sbi_mpxy_channel *channel,
				  u32 message_id, શૂન્ય *tx, u32 tx_len,
				  શૂન્ય *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	સંરચના mpxy_rpmi_mbox *rmb =
		container_of(channel, સંરચના mpxy_rpmi_mbox, channel);
	અચલ સંરચના mpxy_rpmi_mbox_data *data = rmb->mbox_data;
	અચલ સંરચના mpxy_rpmi_service_data *srv =
		mpxy_find_rpmi_srvid(message_id, data);
	સંરચના rpmi_message_args args = {0};
	સંરચના mbox_xfer xfer;
	u32 rx_len = 0;
	પૂર્ણાંક ret;

	જો (!srv)
		પાછા SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	જો (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		પાછા SBI_EFAIL;

	જો (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		જો (rx_max_len < srv->min_rx_len)
			પાછા SBI_EFAIL;

		/* Max rx data size it can expect */
		જો (srv->max_rx_len < channel->attrs.સંદેશ_data_maxlen)
			rx_len = srv->max_rx_len;
		નહીંતર
			rx_len = channel->attrs.સંદેશ_data_maxlen;

		args.type = RPMI_સંદેશ_NORMAL_REQUEST;
		args.flags = (rx) ? 0 : RPMI_સંદેશ_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &args,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	નહીંતર {
		args.type = RPMI_સંદેશ_POSTED_REQUEST;
		args.flags = RPMI_સંદેશ_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &args,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	જો (data->xfer_group)
		ret = data->xfer_group(rmb->group_context, rmb->chan, &xfer);
	નહીંતર
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	જો (ret)
		પાછા ret;

	જો (ack_len)
		*ack_len = args.rx_data_len;

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક mpxy_mbox_send_message_withresp(સંરચના sbi_mpxy_channel *channel,
				  u32 message_id, શૂન્ય *tx, u32 tx_len,
				  શૂન્ય *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	પાછા __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

સ્થિર પૂર્ણાંક mpxy_mbox_send_message_withoutresp(સંરચના sbi_mpxy_channel *channel,
				  u32 message_id, શૂન્ય *tx, u32 tx_len)
{
	પાછા __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 ખાલી, 0, ખાલી);
}

પૂર્ણાંક mpxy_rpmi_mbox_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, અચલ સંરચના fdt_match *match)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_data_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	અચલ સંરચના mpxy_rpmi_mbox_data *data = match->data;
	સંરચના mpxy_rpmi_mbox *rmb;
	સંરચના mbox_chan *chan;
	અચલ fdt32_t *val;
	પૂર્ણાંક rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(કદ(*rmb));
	જો (!rmb)
		પાછા SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	જો (rc) {
		rc = SBI_ENODEV;
		જાઓ fail_free_client;
	}

	/* Match channel service group id */
	જો (data->servicegrp_id != chan->chan_args[0]) {
		rc = SBI_EINVAL;
		જાઓ fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_VERSION,
				     &pro_ver);
	જો (rc)
		જાઓ fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_DATA_LEN,
				     &max_data_len);
	જો (rc)
		જાઓ fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	જો (rc)
		જાઓ fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	જો (rc)
		જાઓ fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION,
				     &servicegrp_ver);
	જો (rc)
		જાઓ fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	જો (rc)
		જાઓ fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_VERSION,
				     &impl_ver);
	જો (rc)
		જાઓ fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	જો (len > 0 && val) {
		channel_id = fdt32_to_cpu(*val);
	} નહીંતર {
		rc = SBI_ENODEV;
		જાઓ fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &મૂળ;
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
	rmb->channel.attrs.સંદેશ_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.સંદેશ_proto_version = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.સંદેશ_data_maxlen = max_data_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.સંદેશ_send_timeout = tx_tout;
	rmb->channel.attrs.સંદેશ_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = data->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_data = data;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	જો (data->setup_group) {
		rc = data->setup_group(&rmb->group_context, chan, data);
		જો (rc)
			જાઓ fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_register_channel(&rmb->channel);
	જો (rc) {
		જો (data->cleanup_group)
			data->cleanup_group(rmb->group_context);
		જાઓ fail_free_chan;
	}

	પાછા SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	પાછા rc;
}
