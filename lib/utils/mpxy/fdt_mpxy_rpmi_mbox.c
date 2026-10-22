/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
ಸಂರಚನಾ mpxy_rpmi_mbox {
	ಸಂರಚನಾ mbox_chan *chan;
	ಧ್ರುವ ಸಂರಚನಾ mpxy_rpmi_mbox_data *mbox_data;
	ಸಂರಚನಾ mpxy_rpmi_channel_attrs msgprot_attrs;
	ಸಂರಚನಾ sbi_mpxy_channel channel;
	ಶೂನ್ಯ *group_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ mpxy_rpmi_service_data *mpxy_find_rpmi_srvid(u32 message_id,
					ಧ್ರುವ ಸಂರಚನಾ mpxy_rpmi_mbox_data *mbox_data)
{
	ಧ್ರುವ ಸಂರಚನಾ mpxy_rpmi_service_data *srv = mbox_data->service_data;
	ಪೂರ್ಣಾಙ್ಕ mid = 0;

	ಪ್ರತಿ (mid = 0; srv[mid].id < mbox_data->ಅಂಕೆ_services; mid++) {
		ಯದಿ (srv[mid].id == (u8)message_id)
			ಪ್ರತ್ಯಾಗಮ &srv[mid];
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/** Copy attributes word size */
ಸ್ಥಿರ ಶೂನ್ಯ mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 ಎಣಿಕೆ)
{
	u32 idx;
	ಪ್ರತಿ (idx = 0; idx < ಎಣಿಕೆ; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mpxy_mbox_read_attributes(ಸಂರಚನಾ sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_ಎಣಿಕೆ)
{
	ಸಂರಚನಾ mpxy_rpmi_mbox *rmb =
		container_of(channel, ಸಂರಚನಾ mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 end_id = base_attr_id + attr_ಎಣಿಕೆ - 1;

	ಯದಿ (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ಪ್ರತ್ಯಾಗಮ SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(base_attr_id)],
			attr_ಎಣಿಕೆ);

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	ಪೂರ್ಣಾಙ್ಕ ret = SBI_OK;

	ವಿಕಲ್ಪ(attr_id) {
	/** All RO access attributes falls under default */
	ಸ್ವಭಾವ:
		ret = SBI_EBAD_RANGE;
	};

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಶೂನ್ಯ mpxy_write_attr(ಸಂರಚನಾ mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mpxy_mbox_write_attributes(ಸಂರಚನಾ sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_ಎಣಿಕೆ)
{
	ಸಂರಚನಾ mpxy_rpmi_mbox *rmb =
		container_of(channel, ಸಂರಚನಾ mpxy_rpmi_mbox, channel);
	u32 end_id = base_attr_id + attr_ಎಣಿಕೆ - 1;
	u32 attr_val, idx;
	ಪೂರ್ಣಾಙ್ಕ ret, mem_idx;

	ಯದಿ (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ಪ್ರತ್ಯಾಗಮ SBI_EBAD_RANGE;

	mem_idx = 0;
	ಪ್ರತಿ (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	mem_idx = 0;
	ಪ್ರತಿ (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ __mpxy_mbox_send_message(ಸಂರಚನಾ sbi_mpxy_channel *channel,
				  u32 message_id, ಶೂನ್ಯ *tx, u32 tx_len,
				  ಶೂನ್ಯ *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	ಸಂರಚನಾ mpxy_rpmi_mbox *rmb =
		container_of(channel, ಸಂರಚನಾ mpxy_rpmi_mbox, channel);
	ಧ್ರುವ ಸಂರಚನಾ mpxy_rpmi_mbox_data *data = rmb->mbox_data;
	ಧ್ರುವ ಸಂರಚನಾ mpxy_rpmi_service_data *srv =
		mpxy_find_rpmi_srvid(message_id, data);
	ಸಂರಚನಾ rpmi_message_args args = {0};
	ಸಂರಚನಾ mbox_xfer xfer;
	u32 rx_len = 0;
	ಪೂರ್ಣಾಙ್ಕ ret;

	ಯದಿ (!srv)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	ಯದಿ (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	ಯದಿ (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		ಯದಿ (rx_max_len < srv->min_rx_len)
			ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

		/* Max rx data size it can expect */
		ಯದಿ (srv->max_rx_len < channel->attrs.ಸಂದೇಶ_data_maxlen)
			rx_len = srv->max_rx_len;
		ಅನ್ಯಥಾ
			rx_len = channel->attrs.ಸಂದೇಶ_data_maxlen;

		args.type = RPMI_ಸಂದೇಶ_NORMAL_REQUEST;
		args.flags = (rx) ? 0 : RPMI_ಸಂದೇಶ_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &args,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	ಅನ್ಯಥಾ {
		args.type = RPMI_ಸಂದೇಶ_POSTED_REQUEST;
		args.flags = RPMI_ಸಂದೇಶ_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &args,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	ಯದಿ (data->xfer_group)
		ret = data->xfer_group(rmb->group_context, rmb->chan, &xfer);
	ಅನ್ಯಥಾ
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ಯದಿ (ack_len)
		*ack_len = args.rx_data_len;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mpxy_mbox_send_message_withresp(ಸಂರಚನಾ sbi_mpxy_channel *channel,
				  u32 message_id, ಶೂನ್ಯ *tx, u32 tx_len,
				  ಶೂನ್ಯ *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	ಪ್ರತ್ಯಾಗಮ __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mpxy_mbox_send_message_withoutresp(ಸಂರಚನಾ sbi_mpxy_channel *channel,
				  u32 message_id, ಶೂನ್ಯ *tx, u32 tx_len)
{
	ಪ್ರತ್ಯಾಗಮ __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 ರಿಕ್ತ, 0, ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ mpxy_rpmi_mbox_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_data_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	ಧ್ರುವ ಸಂರಚನಾ mpxy_rpmi_mbox_data *data = match->data;
	ಸಂರಚನಾ mpxy_rpmi_mbox *rmb;
	ಸಂರಚನಾ mbox_chan *chan;
	ಧ್ರುವ fdt32_t *val;
	ಪೂರ್ಣಾಙ್ಕ rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(ಪರಿಮಾಣ(*rmb));
	ಯದಿ (!rmb)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ಯದಿ (rc) {
		rc = SBI_ENODEV;
		ಗಚ್ಛ fail_free_client;
	}

	/* Match channel service group id */
	ಯದಿ (data->servicegrp_id != chan->chan_args[0]) {
		rc = SBI_EINVAL;
		ಗಚ್ಛ fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_VERSION,
				     &pro_ver);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_DATA_LEN,
				     &max_data_len);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION,
				     &servicegrp_ver);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_VERSION,
				     &impl_ver);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	ಯದಿ (len > 0 && val) {
		channel_id = fdt32_to_cpu(*val);
	} ಅನ್ಯಥಾ {
		rc = SBI_ENODEV;
		ಗಚ್ಛ fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &ಬೇರು;
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
	rmb->channel.attrs.ಸಂದೇಶ_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.ಸಂದೇಶ_proto_version = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.ಸಂದೇಶ_data_maxlen = max_data_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.ಸಂದೇಶ_send_timeout = tx_tout;
	rmb->channel.attrs.ಸಂದೇಶ_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = data->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_data = data;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	ಯದಿ (data->setup_group) {
		rc = data->setup_group(&rmb->group_context, chan, data);
		ಯದಿ (rc)
			ಗಚ್ಛ fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_register_channel(&rmb->channel);
	ಯದಿ (rc) {
		ಯದಿ (data->cleanup_group)
			data->cleanup_group(rmb->group_context);
		ಗಚ್ಛ fail_free_chan;
	}

	ಪ್ರತ್ಯಾಗಮ SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	ಪ್ರತ್ಯಾಗಮ rc;
}
