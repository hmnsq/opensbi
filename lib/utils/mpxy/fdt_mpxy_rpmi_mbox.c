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
ರಚನೆ mpxy_rpmi_mbox {
	ರಚನೆ mbox_chan *chan;
	ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_mbox_ದತ್ತಾಂಶ *mbox_ದತ್ತಾಂಶ;
	ರಚನೆ mpxy_rpmi_channel_attrs msgprot_attrs;
	ರಚನೆ sbi_mpxy_channel channel;
	ಶೂನ್ಯ *ಗುಂಪು_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_service_ದತ್ತಾಂಶ *mpxy_ಕಂಡುಹಿಡಿ_rpmi_srvid(u32 ಸಂದೇಶ_id_2,
					ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_mbox_ದತ್ತಾಂಶ *mbox_ದತ್ತಾಂಶ)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_service_ದತ್ತಾಂಶ *srv = mbox_ದತ್ತಾಂಶ->service_ದತ್ತಾಂಶ;
	ಪೂರ್ಣಾಂಕ mid = 0;

	ಗಾಗಿ (mid = 0; srv[mid].id < mbox_ದತ್ತಾಂಶ->ಅಂಕೆ_services; mid++) {
		ಆದರೆ (srv[mid].id == (u8)ಸಂದೇಶ_id_2)
			ಹಿಂತಿರುಗಿ &srv[mid];
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/** Copy attributes word size */
ಸ್ಥಿರ ಶೂನ್ಯ mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 ಎಣಿಕೆ_2)
{
	u32 idx;
	ಗಾಗಿ (idx = 0; idx < ಎಣಿಕೆ_2; idx++)
		outmem[idx] = cpu_ಗಮ್ಯ_le32(inmem[idx]);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mpxy_mbox_read_ಗುಣಲಕ್ಷಣಗಳು(ರಚನೆ sbi_mpxy_channel *channel,
				     u32 *outmem, u32 ತಳ_attr_id,
				     u32 attr_ಎಣಿಕೆ)
{
	ರಚನೆ mpxy_rpmi_mbox *rmb =
		container_ದ(channel, ರಚನೆ mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 ಕೊನೆ_id = ತಳ_attr_id + attr_ಎಣಿಕೆ - 1;

	ಆದರೆ (ಕೊನೆ_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ಹಿಂತಿರುಗಿ SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(ತಳ_attr_id)],
			attr_ಎಣಿಕೆ);

	ಹಿಂತಿರುಗಿ SBI_OK;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	ಪೂರ್ಣಾಂಕ ret = SBI_OK;

	ಆಯ್ಕೆ(attr_id) {
	/** All RO access attributes falls under default */
	ಪೂರ್ವನಿಯೋಜಿತ:
		ret = SBI_EBAD_RANGE;
	};

	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಶೂನ್ಯ mpxy_write_attr(ರಚನೆ mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mpxy_mbox_write_ಗುಣಲಕ್ಷಣಗಳು(ರಚನೆ sbi_mpxy_channel *channel,
				     u32 *outmem, u32 ತಳ_attr_id,
				     u32 attr_ಎಣಿಕೆ)
{
	ರಚನೆ mpxy_rpmi_mbox *rmb =
		container_ದ(channel, ರಚನೆ mpxy_rpmi_mbox, channel);
	u32 ಕೊನೆ_id = ತಳ_attr_id + attr_ಎಣಿಕೆ - 1;
	u32 attr_val, idx;
	ಪೂರ್ಣಾಂಕ ret, mem_idx;

	ಆದರೆ (ಕೊನೆ_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ಹಿಂತಿರುಗಿ SBI_EBAD_RANGE;

	mem_idx = 0;
	ಗಾಗಿ (idx = ತಳ_attr_id; idx <= ಕೊನೆ_id; idx++) {
		attr_val = le32_ಗಮ್ಯ_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	mem_idx = 0;
	ಗಾಗಿ (idx = ತಳ_attr_id; idx <= ಕೊನೆ_id; idx++) {
		attr_val = le32_ಗಮ್ಯ_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ __mpxy_mbox_send_message(ರಚನೆ sbi_mpxy_channel *channel,
				  u32 ಸಂದೇಶ_id_2, ಶೂನ್ಯ *tx, u32 tx_len,
				  ಶೂನ್ಯ *rx, u32 rx_max_len,
				  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ack_len)
{
	ರಚನೆ mpxy_rpmi_mbox *rmb =
		container_ದ(channel, ರಚನೆ mpxy_rpmi_mbox, channel);
	ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_mbox_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ = rmb->mbox_ದತ್ತಾಂಶ;
	ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_service_ದತ್ತಾಂಶ *srv =
		mpxy_ಕಂಡುಹಿಡಿ_rpmi_srvid(ಸಂದೇಶ_id_2, ದತ್ತಾಂಶ);
	ರಚನೆ rpmi_ಸಂದೇಶ_ವಾದಗಳು ವಾದಗಳು = {0};
	ರಚನೆ mbox_xfer xfer;
	u32 rx_len = 0;
	ಪೂರ್ಣಾಂಕ ret;

	ಆದರೆ (!srv)
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	ಆದರೆ (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	ಆದರೆ (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		ಆದರೆ (rx_max_len < srv->min_rx_len)
			ಹಿಂತಿರುಗಿ SBI_EFAIL;

		/* Max rx data size it can expect */
		ಆದರೆ (srv->max_rx_len < channel->attrs.ಸಂದೇಶ_ದತ್ತಾಂಶ_maxlen)
			rx_len = srv->max_rx_len;
		ಇಲ್ಲದಿದ್ದರೆ
			rx_len = channel->attrs.ಸಂದೇಶ_ದತ್ತಾಂಶ_maxlen;

		ವಾದಗಳು.type = RPMI_ಸಂದೇಶ_ಸಾಮಾನ್ಯ_REQUEST;
		ವಾದಗಳು.flags = (rx) ? 0 : RPMI_ಸಂದೇಶ_FLAGS_NO_RX;
		ವಾದಗಳು.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &ವಾದಗಳು,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	ಇಲ್ಲದಿದ್ದರೆ {
		ವಾದಗಳು.type = RPMI_ಸಂದೇಶ_POSTED_REQUEST;
		ವಾದಗಳು.flags = RPMI_ಸಂದೇಶ_FLAGS_NO_RX;
		ವಾದಗಳು.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &ವಾದಗಳು,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	ಆದರೆ (ದತ್ತಾಂಶ->xfer_ಗುಂಪು)
		ret = ದತ್ತಾಂಶ->xfer_ಗುಂಪು(rmb->ಗುಂಪು_context, rmb->chan, &xfer);
	ಇಲ್ಲದಿದ್ದರೆ
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ಆದರೆ (ack_len)
		*ack_len = ವಾದಗಳು.rx_ದತ್ತಾಂಶ_len;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mpxy_mbox_send_ಸಂದೇಶ_withresp(ರಚನೆ sbi_mpxy_channel *channel,
				  u32 ಸಂದೇಶ_id_2, ಶೂನ್ಯ *tx, u32 tx_len,
				  ಶೂನ್ಯ *rx, u32 rx_max_len,
				  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ack_len)
{
	ಹಿಂತಿರುಗಿ __mpxy_mbox_send_message(channel, ಸಂದೇಶ_id_2, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mpxy_mbox_send_ಸಂದೇಶ_withoutresp(ರಚನೆ sbi_mpxy_channel *channel,
				  u32 ಸಂದೇಶ_id_2, ಶೂನ್ಯ *tx, u32 tx_len)
{
	ಹಿಂತಿರುಗಿ __mpxy_mbox_send_message(channel, ಸಂದೇಶ_id_2, tx, tx_len,
				 ಖಾಲಿ, 0, ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ mpxy_rpmi_mbox_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_ದತ್ತಾಂಶ_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_mbox_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ = ಹೊಂದಿಕೆ->ದತ್ತಾಂಶ;
	ರಚನೆ mpxy_rpmi_mbox *rmb;
	ರಚನೆ mbox_chan *chan;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	ಪೂರ್ಣಾಂಕ rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(ಗಾತ್ರ(*rmb));
	ಆದರೆ (!rmb)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ಆದರೆ (rc) {
		rc = SBI_ENODEV;
		ಹೋಗು fail_free_client;
	}

	/* Match channel service group id */
	ಆದರೆ (ದತ್ತಾಂಶ->servicegrp_id != chan->chan_ವಾದಗಳು[0]) {
		rc = SBI_EINVAL;
		ಹೋಗು fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_ಆವೃತ್ತಿ,
				     &pro_ver);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_ದತ್ತಾಂಶ_LEN,
				     &max_ದತ್ತಾಂಶ_len);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_ಆವೃತ್ತಿ,
				     &servicegrp_ver);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ಆವೃತ್ತಿ,
				     &impl_ver);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	ಆದರೆ (len > 0 && val) {
		channel_id = fdt32_ಗಮ್ಯ_cpu(*val);
	} ಇಲ್ಲದಿದ್ದರೆ {
		rc = SBI_ENODEV;
		ಹೋಗು fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &ಬೇರು;
	/* Callback for read RPMI attributes */
	rmb->channel.read_ಗುಣಲಕ್ಷಣಗಳು = mpxy_mbox_read_ಗುಣಲಕ್ಷಣಗಳು;
	/* Callback for write RPMI attributes */
	rmb->channel.write_ಗುಣಲಕ್ಷಣಗಳು = mpxy_mbox_write_ಗುಣಲಕ್ಷಣಗಳು;
	/* Callback for sending RPMI message */
	rmb->channel.send_ಸಂದೇಶ_with_response =
					mpxy_mbox_send_ಸಂದೇಶ_withresp;
	rmb->channel.send_ಸಂದೇಶ_without_response =
					mpxy_mbox_send_ಸಂದೇಶ_withoutresp;

	/* RPMI Message Protocol ID */
	rmb->channel.attrs.ಸಂದೇಶ_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.ಸಂದೇಶ_proto_ಆವೃತ್ತಿ = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.ಸಂದೇಶ_ದತ್ತಾಂಶ_maxlen = max_ದತ್ತಾಂಶ_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.ಸಂದೇಶ_send_timeout = tx_tout;
	rmb->channel.attrs.ಸಂದೇಶ_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = ದತ್ತಾಂಶ->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	ಆದರೆ (ದತ್ತಾಂಶ->ಸಿದ್ಧತೆ_ಗುಂಪು) {
		rc = ದತ್ತಾಂಶ->ಸಿದ್ಧತೆ_ಗುಂಪು(&rmb->ಗುಂಪು_context, chan, ದತ್ತಾಂಶ);
		ಆದರೆ (rc)
			ಹೋಗು fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_ರಿಜಿಸ್ಟರ್_channel(&rmb->channel);
	ಆದರೆ (rc) {
		ಆದರೆ (ದತ್ತಾಂಶ->cleanup_ಗುಂಪು)
			ದತ್ತಾಂಶ->cleanup_ಗುಂಪು(rmb->ಗುಂಪು_context);
		ಹೋಗು fail_free_chan;
	}

	ಹಿಂತಿರುಗಿ SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	ಹಿಂತಿರುಗಿ rc;
}
