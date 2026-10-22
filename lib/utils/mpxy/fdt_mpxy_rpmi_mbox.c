/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#అన్తర్భావ <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
నిర్మాణం mpxy_rpmi_mbox {
	నిర్మాణం mbox_chan *chan;
	స్థిరరాశి నిర్మాణం mpxy_rpmi_mbox_దత్తాంశం *mbox_దత్తాంశం;
	నిర్మాణం mpxy_rpmi_channel_attrs msgprot_attrs;
	నిర్మాణం sbi_mpxy_channel channel;
	శూన్య *సమూహం_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
స్థిర స్థిరరాశి నిర్మాణం mpxy_rpmi_service_దత్తాంశం *mpxy_కనుగొను_rpmi_srvid(u32 సందేశం_id_2,
					స్థిరరాశి నిర్మాణం mpxy_rpmi_mbox_దత్తాంశం *mbox_దత్తాంశం)
{
	స్థిరరాశి నిర్మాణం mpxy_rpmi_service_దత్తాంశం *srv = mbox_దత్తాంశం->service_దత్తాంశం;
	పూర్ణాంకం mid = 0;

	కొరకు (mid = 0; srv[mid].id < mbox_దత్తాంశం->అంకె_services; mid++) {
		ఒకవేళ (srv[mid].id == (u8)సందేశం_id_2)
			తిరిగి &srv[mid];
	}

	తిరిగి ఖాళీ;
}

/** Copy attributes word size */
స్థిర శూన్య mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 లెక్క)
{
	u32 idx;
	కొరకు (idx = 0; idx < లెక్క; idx++)
		outmem[idx] = cpu_గమ్యం_le32(inmem[idx]);
}

స్థిర పూర్ణాంకం mpxy_mbox_read_లక్షణాలు(నిర్మాణం sbi_mpxy_channel *channel,
				     u32 *outmem, u32 పునాది_attr_id,
				     u32 attr_లెక్క)
{
	నిర్మాణం mpxy_rpmi_mbox *rmb =
		container_యొక్క(channel, నిర్మాణం mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 చివర_id = పునాది_attr_id + attr_లెక్క - 1;

	ఒకవేళ (చివర_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		తిరిగి SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(పునాది_attr_id)],
			attr_లెక్క);

	తిరిగి SBI_OK;
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
స్థిర పూర్ణాంకం mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	పూర్ణాంకం ret = SBI_OK;

	ఎంపిక(attr_id) {
	/** All RO access attributes falls under default */
	అప్రమేయ:
		ret = SBI_EBAD_RANGE;
	};

	తిరిగి ret;
}

స్థిర శూన్య mpxy_write_attr(నిర్మాణం mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

స్థిర పూర్ణాంకం mpxy_mbox_write_లక్షణాలు(నిర్మాణం sbi_mpxy_channel *channel,
				     u32 *outmem, u32 పునాది_attr_id,
				     u32 attr_లెక్క)
{
	నిర్మాణం mpxy_rpmi_mbox *rmb =
		container_యొక్క(channel, నిర్మాణం mpxy_rpmi_mbox, channel);
	u32 చివర_id = పునాది_attr_id + attr_లెక్క - 1;
	u32 attr_val, idx;
	పూర్ణాంకం ret, mem_idx;

	ఒకవేళ (చివర_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		తిరిగి SBI_EBAD_RANGE;

	mem_idx = 0;
	కొరకు (idx = పునాది_attr_id; idx <= చివర_id; idx++) {
		attr_val = le32_గమ్యం_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	mem_idx = 0;
	కొరకు (idx = పునాది_attr_id; idx <= చివర_id; idx++) {
		attr_val = le32_గమ్యం_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం __mpxy_mbox_send_message(నిర్మాణం sbi_mpxy_channel *channel,
				  u32 సందేశం_id_2, శూన్య *tx, u32 tx_len,
				  శూన్య *rx, u32 rx_max_len,
				  గుర్తులేని దీర్ఘ *ack_len)
{
	నిర్మాణం mpxy_rpmi_mbox *rmb =
		container_యొక్క(channel, నిర్మాణం mpxy_rpmi_mbox, channel);
	స్థిరరాశి నిర్మాణం mpxy_rpmi_mbox_దత్తాంశం *దత్తాంశం = rmb->mbox_దత్తాంశం;
	స్థిరరాశి నిర్మాణం mpxy_rpmi_service_దత్తాంశం *srv =
		mpxy_కనుగొను_rpmi_srvid(సందేశం_id_2, దత్తాంశం);
	నిర్మాణం rpmi_సందేశం_వాదనలు వాదనలు = {0};
	నిర్మాణం mbox_xfer xfer;
	u32 rx_len = 0;
	పూర్ణాంకం ret;

	ఒకవేళ (!srv)
		తిరిగి SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	ఒకవేళ (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		తిరిగి SBI_EFAIL;

	ఒకవేళ (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		ఒకవేళ (rx_max_len < srv->min_rx_len)
			తిరిగి SBI_EFAIL;

		/* Max rx data size it can expect */
		ఒకవేళ (srv->max_rx_len < channel->attrs.సందేశం_దత్తాంశం_maxlen)
			rx_len = srv->max_rx_len;
		లేకపోతే
			rx_len = channel->attrs.సందేశం_దత్తాంశం_maxlen;

		వాదనలు.type = RPMI_సందేశం_సాధారణ_REQUEST;
		వాదనలు.flags = (rx) ? 0 : RPMI_సందేశం_FLAGS_NO_RX;
		వాదనలు.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &వాదనలు,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	లేకపోతే {
		వాదనలు.type = RPMI_సందేశం_POSTED_REQUEST;
		వాదనలు.flags = RPMI_సందేశం_FLAGS_NO_RX;
		వాదనలు.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &వాదనలు,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	ఒకవేళ (దత్తాంశం->xfer_సమూహం)
		ret = దత్తాంశం->xfer_సమూహం(rmb->సమూహం_context, rmb->chan, &xfer);
	లేకపోతే
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	ఒకవేళ (ret)
		తిరిగి ret;

	ఒకవేళ (ack_len)
		*ack_len = వాదనలు.rx_దత్తాంశం_len;

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం mpxy_mbox_send_సందేశం_withresp(నిర్మాణం sbi_mpxy_channel *channel,
				  u32 సందేశం_id_2, శూన్య *tx, u32 tx_len,
				  శూన్య *rx, u32 rx_max_len,
				  గుర్తులేని దీర్ఘ *ack_len)
{
	తిరిగి __mpxy_mbox_send_message(channel, సందేశం_id_2, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

స్థిర పూర్ణాంకం mpxy_mbox_send_సందేశం_withoutresp(నిర్మాణం sbi_mpxy_channel *channel,
				  u32 సందేశం_id_2, శూన్య *tx, u32 tx_len)
{
	తిరిగి __mpxy_mbox_send_message(channel, సందేశం_id_2, tx, tx_len,
				 ఖాళీ, 0, ఖాళీ);
}

పూర్ణాంకం mpxy_rpmi_mbox_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_దత్తాంశం_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	స్థిరరాశి నిర్మాణం mpxy_rpmi_mbox_దత్తాంశం *దత్తాంశం = సరిపోలిక->దత్తాంశం;
	నిర్మాణం mpxy_rpmi_mbox *rmb;
	నిర్మాణం mbox_chan *chan;
	స్థిరరాశి fdt32_t *val;
	పూర్ణాంకం rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(పరిమాణం(*rmb));
	ఒకవేళ (!rmb)
		తిరిగి SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ఒకవేళ (rc) {
		rc = SBI_ENODEV;
		వెళ్ళు fail_free_client;
	}

	/* Match channel service group id */
	ఒకవేళ (దత్తాంశం->servicegrp_id != chan->chan_వాదనలు[0]) {
		rc = SBI_EINVAL;
		వెళ్ళు fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_సంస్కరణ,
				     &pro_ver);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_దత్తాంశం_LEN,
				     &max_దత్తాంశం_len);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_సంస్కరణ,
				     &servicegrp_ver);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_సంస్కరణ,
				     &impl_ver);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	ఒకవేళ (len > 0 && val) {
		channel_id = fdt32_గమ్యం_cpu(*val);
	} లేకపోతే {
		rc = SBI_ENODEV;
		వెళ్ళు fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &మూలం;
	/* Callback for read RPMI attributes */
	rmb->channel.read_లక్షణాలు = mpxy_mbox_read_లక్షణాలు;
	/* Callback for write RPMI attributes */
	rmb->channel.write_లక్షణాలు = mpxy_mbox_write_లక్షణాలు;
	/* Callback for sending RPMI message */
	rmb->channel.send_సందేశం_with_response =
					mpxy_mbox_send_సందేశం_withresp;
	rmb->channel.send_సందేశం_without_response =
					mpxy_mbox_send_సందేశం_withoutresp;

	/* RPMI Message Protocol ID */
	rmb->channel.attrs.సందేశం_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.సందేశం_proto_సంస్కరణ = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.సందేశం_దత్తాంశం_maxlen = max_దత్తాంశం_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.సందేశం_send_timeout = tx_tout;
	rmb->channel.attrs.సందేశం_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = దత్తాంశం->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_దత్తాంశం = దత్తాంశం;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	ఒకవేళ (దత్తాంశం->అమరిక_సమూహం) {
		rc = దత్తాంశం->అమరిక_సమూహం(&rmb->సమూహం_context, chan, దత్తాంశం);
		ఒకవేళ (rc)
			వెళ్ళు fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_రిజిస్టరు_channel(&rmb->channel);
	ఒకవేళ (rc) {
		ఒకవేళ (దత్తాంశం->cleanup_సమూహం)
			దత్తాంశం->cleanup_సమూహం(rmb->సమూహం_context);
		వెళ్ళు fail_free_chan;
	}

	తిరిగి SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	తిరిగి rc;
}
