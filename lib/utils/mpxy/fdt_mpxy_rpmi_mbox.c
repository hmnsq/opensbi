/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
ਬਣਤਰ mpxy_rpmi_mbox {
	ਬਣਤਰ mbox_chan *chan;
	ਅਚਲ ਬਣਤਰ mpxy_rpmi_mbox_ਅੰਕੜੇ *mbox_ਅੰਕੜੇ;
	ਬਣਤਰ mpxy_rpmi_channel_attrs msgprot_attrs;
	ਬਣਤਰ sbi_mpxy_channel channel;
	ਖਾਲੀ *ਸਮੂਹ_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ mpxy_rpmi_service_ਅੰਕੜੇ *mpxy_ਲੱਭੋ_rpmi_srvid(u32 ਸੁਨੇਹਾ_id_2,
					ਅਚਲ ਬਣਤਰ mpxy_rpmi_mbox_ਅੰਕੜੇ *mbox_ਅੰਕੜੇ)
{
	ਅਚਲ ਬਣਤਰ mpxy_rpmi_service_ਅੰਕੜੇ *srv = mbox_ਅੰਕੜੇ->service_ਅੰਕੜੇ;
	ਪੂਰਨ mid = 0;

	ਲਈ (mid = 0; srv[mid].id < mbox_ਅੰਕੜੇ->ਅੰਕ_services; mid++) {
		ਜੇ (srv[mid].id == (u8)ਸੁਨੇਹਾ_id_2)
			ਵਾਪਸ &srv[mid];
	}

	ਵਾਪਸ ਨਿਰਾ;
}

/** Copy attributes word size */
ਸਥਿਰ ਖਾਲੀ mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 ਗਿਣਤੀ_2)
{
	u32 idx;
	ਲਈ (idx = 0; idx < ਗਿਣਤੀ_2; idx++)
		outmem[idx] = cpu_ਮੰਜ਼ਿਲ_le32(inmem[idx]);
}

ਸਥਿਰ ਪੂਰਨ mpxy_mbox_read_ਗੁਣ(ਬਣਤਰ sbi_mpxy_channel *channel,
				     u32 *outmem, u32 ਅਧਾਰ_attr_id,
				     u32 attr_ਗਿਣਤੀ)
{
	ਬਣਤਰ mpxy_rpmi_mbox *rmb =
		container_ਦਾ(channel, ਬਣਤਰ mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 ਸਿਰਾ_id = ਅਧਾਰ_attr_id + attr_ਗਿਣਤੀ - 1;

	ਜੇ (ਸਿਰਾ_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ਵਾਪਸ SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(ਅਧਾਰ_attr_id)],
			attr_ਗਿਣਤੀ);

	ਵਾਪਸ SBI_OK;
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
ਸਥਿਰ ਪੂਰਨ mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	ਪੂਰਨ ret = SBI_OK;

	ਚੋਣ(attr_id) {
	/** All RO access attributes falls under default */
	ਮੂਲ:
		ret = SBI_EBAD_RANGE;
	};

	ਵਾਪਸ ret;
}

ਸਥਿਰ ਖਾਲੀ mpxy_write_attr(ਬਣਤਰ mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

ਸਥਿਰ ਪੂਰਨ mpxy_mbox_write_ਗੁਣ(ਬਣਤਰ sbi_mpxy_channel *channel,
				     u32 *outmem, u32 ਅਧਾਰ_attr_id,
				     u32 attr_ਗਿਣਤੀ)
{
	ਬਣਤਰ mpxy_rpmi_mbox *rmb =
		container_ਦਾ(channel, ਬਣਤਰ mpxy_rpmi_mbox, channel);
	u32 ਸਿਰਾ_id = ਅਧਾਰ_attr_id + attr_ਗਿਣਤੀ - 1;
	u32 attr_val, idx;
	ਪੂਰਨ ret, mem_idx;

	ਜੇ (ਸਿਰਾ_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ਵਾਪਸ SBI_EBAD_RANGE;

	mem_idx = 0;
	ਲਈ (idx = ਅਧਾਰ_attr_id; idx <= ਸਿਰਾ_id; idx++) {
		attr_val = le32_ਮੰਜ਼ਿਲ_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	mem_idx = 0;
	ਲਈ (idx = ਅਧਾਰ_attr_id; idx <= ਸਿਰਾ_id; idx++) {
		attr_val = le32_ਮੰਜ਼ਿਲ_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ __mpxy_mbox_send_message(ਬਣਤਰ sbi_mpxy_channel *channel,
				  u32 ਸੁਨੇਹਾ_id_2, ਖਾਲੀ *tx, u32 tx_len,
				  ਖਾਲੀ *rx, u32 rx_max_len,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ack_len)
{
	ਬਣਤਰ mpxy_rpmi_mbox *rmb =
		container_ਦਾ(channel, ਬਣਤਰ mpxy_rpmi_mbox, channel);
	ਅਚਲ ਬਣਤਰ mpxy_rpmi_mbox_ਅੰਕੜੇ *ਅੰਕੜੇ = rmb->mbox_ਅੰਕੜੇ;
	ਅਚਲ ਬਣਤਰ mpxy_rpmi_service_ਅੰਕੜੇ *srv =
		mpxy_ਲੱਭੋ_rpmi_srvid(ਸੁਨੇਹਾ_id_2, ਅੰਕੜੇ);
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ_ਦਲੀਲਾਂ ਦਲੀਲਾਂ = {0};
	ਬਣਤਰ mbox_xfer xfer;
	u32 rx_len = 0;
	ਪੂਰਨ ret;

	ਜੇ (!srv)
		ਵਾਪਸ SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	ਜੇ (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		ਵਾਪਸ SBI_EFAIL;

	ਜੇ (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		ਜੇ (rx_max_len < srv->min_rx_len)
			ਵਾਪਸ SBI_EFAIL;

		/* Max rx data size it can expect */
		ਜੇ (srv->max_rx_len < channel->attrs.ਸੁਨੇਹਾ_ਅੰਕੜੇ_maxlen)
			rx_len = srv->max_rx_len;
		ਨਹੀਂਤਾਂ
			rx_len = channel->attrs.ਸੁਨੇਹਾ_ਅੰਕੜੇ_maxlen;

		ਦਲੀਲਾਂ.type = RPMI_ਸੁਨੇਹਾ_ਸਾਧਾਰਨ_REQUEST;
		ਦਲੀਲਾਂ.flags = (rx) ? 0 : RPMI_ਸੁਨੇਹਾ_FLAGS_NO_RX;
		ਦਲੀਲਾਂ.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &ਦਲੀਲਾਂ,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	ਨਹੀਂਤਾਂ {
		ਦਲੀਲਾਂ.type = RPMI_ਸੁਨੇਹਾ_POSTED_REQUEST;
		ਦਲੀਲਾਂ.flags = RPMI_ਸੁਨੇਹਾ_FLAGS_NO_RX;
		ਦਲੀਲਾਂ.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &ਦਲੀਲਾਂ,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	ਜੇ (ਅੰਕੜੇ->xfer_ਸਮੂਹ)
		ret = ਅੰਕੜੇ->xfer_ਸਮੂਹ(rmb->ਸਮੂਹ_context, rmb->chan, &xfer);
	ਨਹੀਂਤਾਂ
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ਜੇ (ack_len)
		*ack_len = ਦਲੀਲਾਂ.rx_ਅੰਕੜੇ_len;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ mpxy_mbox_send_ਸੁਨੇਹਾ_withresp(ਬਣਤਰ sbi_mpxy_channel *channel,
				  u32 ਸੁਨੇਹਾ_id_2, ਖਾਲੀ *tx, u32 tx_len,
				  ਖਾਲੀ *rx, u32 rx_max_len,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ack_len)
{
	ਵਾਪਸ __mpxy_mbox_send_message(channel, ਸੁਨੇਹਾ_id_2, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

ਸਥਿਰ ਪੂਰਨ mpxy_mbox_send_ਸੁਨੇਹਾ_withoutresp(ਬਣਤਰ sbi_mpxy_channel *channel,
				  u32 ਸੁਨੇਹਾ_id_2, ਖਾਲੀ *tx, u32 tx_len)
{
	ਵਾਪਸ __mpxy_mbox_send_message(channel, ਸੁਨੇਹਾ_id_2, tx, tx_len,
				 ਨਿਰਾ, 0, ਨਿਰਾ);
}

ਪੂਰਨ mpxy_rpmi_mbox_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_ਅੰਕੜੇ_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	ਅਚਲ ਬਣਤਰ mpxy_rpmi_mbox_ਅੰਕੜੇ *ਅੰਕੜੇ = ਮੇਲ->ਅੰਕੜੇ;
	ਬਣਤਰ mpxy_rpmi_mbox *rmb;
	ਬਣਤਰ mbox_chan *chan;
	ਅਚਲ fdt32_t *val;
	ਪੂਰਨ rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(ਆਕਾਰ(*rmb));
	ਜੇ (!rmb)
		ਵਾਪਸ SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ਜੇ (rc) {
		rc = SBI_ENODEV;
		ਜਾਓ fail_free_client;
	}

	/* Match channel service group id */
	ਜੇ (ਅੰਕੜੇ->servicegrp_id != chan->chan_ਦਲੀਲਾਂ[0]) {
		rc = SBI_EINVAL;
		ਜਾਓ fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_ਸੰਸਕਰਣ,
				     &pro_ver);
	ਜੇ (rc)
		ਜਾਓ fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_ਅੰਕੜੇ_LEN,
				     &max_ਅੰਕੜੇ_len);
	ਜੇ (rc)
		ਜਾਓ fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	ਜੇ (rc)
		ਜਾਓ fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	ਜੇ (rc)
		ਜਾਓ fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_ਸੰਸਕਰਣ,
				     &servicegrp_ver);
	ਜੇ (rc)
		ਜਾਓ fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	ਜੇ (rc)
		ਜਾਓ fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ਸੰਸਕਰਣ,
				     &impl_ver);
	ਜੇ (rc)
		ਜਾਓ fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	ਜੇ (len > 0 && val) {
		channel_id = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	} ਨਹੀਂਤਾਂ {
		rc = SBI_ENODEV;
		ਜਾਓ fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &ਜੜ੍ਹ;
	/* Callback for read RPMI attributes */
	rmb->channel.read_ਗੁਣ = mpxy_mbox_read_ਗੁਣ;
	/* Callback for write RPMI attributes */
	rmb->channel.write_ਗੁਣ = mpxy_mbox_write_ਗੁਣ;
	/* Callback for sending RPMI message */
	rmb->channel.send_ਸੁਨੇਹਾ_with_response =
					mpxy_mbox_send_ਸੁਨੇਹਾ_withresp;
	rmb->channel.send_ਸੁਨੇਹਾ_without_response =
					mpxy_mbox_send_ਸੁਨੇਹਾ_withoutresp;

	/* RPMI Message Protocol ID */
	rmb->channel.attrs.ਸੁਨੇਹਾ_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.ਸੁਨੇਹਾ_proto_ਸੰਸਕਰਣ = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.ਸੁਨੇਹਾ_ਅੰਕੜੇ_maxlen = max_ਅੰਕੜੇ_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.ਸੁਨੇਹਾ_send_timeout = tx_tout;
	rmb->channel.attrs.ਸੁਨੇਹਾ_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = ਅੰਕੜੇ->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_ਅੰਕੜੇ = ਅੰਕੜੇ;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	ਜੇ (ਅੰਕੜੇ->ਤਿਆਰੀ_ਸਮੂਹ) {
		rc = ਅੰਕੜੇ->ਤਿਆਰੀ_ਸਮੂਹ(&rmb->ਸਮੂਹ_context, chan, ਅੰਕੜੇ);
		ਜੇ (rc)
			ਜਾਓ fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_ਰਜਿਸਟਰ_channel(&rmb->channel);
	ਜੇ (rc) {
		ਜੇ (ਅੰਕੜੇ->cleanup_ਸਮੂਹ)
			ਅੰਕੜੇ->cleanup_ਸਮੂਹ(rmb->ਸਮੂਹ_context);
		ਜਾਓ fail_free_chan;
	}

	ਵਾਪਸ SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	ਵਾਪਸ rc;
}
