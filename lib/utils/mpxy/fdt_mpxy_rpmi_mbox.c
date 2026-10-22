/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#شامل <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
ساخت mpxy_rpmi_mbox {
	ساخت mbox_chan *chan;
	ثابت ساخت mpxy_rpmi_mbox_ڈیٹا *mbox_ڈیٹا;
	ساخت mpxy_rpmi_channel_attrs msgprot_attrs;
	ساخت sbi_mpxy_channel channel;
	خالی *گروہ_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
ساکن ثابت ساخت mpxy_rpmi_service_ڈیٹا *mpxy_ڈھونڈو_rpmi_srvid(u32 پیغام_id_2,
					ثابت ساخت mpxy_rpmi_mbox_ڈیٹا *mbox_ڈیٹا)
{
	ثابت ساخت mpxy_rpmi_service_ڈیٹا *srv = mbox_ڈیٹا->service_ڈیٹا;
	صحیح mid = 0;

	برائے (mid = 0; srv[mid].id < mbox_ڈیٹا->عدد_services; mid++) {
		اگر (srv[mid].id == (u8)پیغام_id_2)
			واپس &srv[mid];
	}

	واپس تہی;
}

/** Copy attributes word size */
ساکن خالی mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 گنتی)
{
	u32 idx;
	برائے (idx = 0; idx < گنتی; idx++)
		outmem[idx] = cpu_مقصد_le32(inmem[idx]);
}

ساکن صحیح mpxy_mbox_read_اوصاف(ساخت sbi_mpxy_channel *channel,
				     u32 *outmem, u32 بنیاد_attr_id,
				     u32 attr_گنتی)
{
	ساخت mpxy_rpmi_mbox *rmb =
		container_کا(channel, ساخت mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 انتہا_id = بنیاد_attr_id + attr_گنتی - 1;

	اگر (انتہا_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		واپس SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(بنیاد_attr_id)],
			attr_گنتی);

	واپس SBI_OK;
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
ساکن صحیح mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	صحیح ret = SBI_OK;

	انتخاب(attr_id) {
	/** All RO access attributes falls under default */
	طےشدہ:
		ret = SBI_EBAD_RANGE;
	};

	واپس ret;
}

ساکن خالی mpxy_write_attr(ساخت mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

ساکن صحیح mpxy_mbox_write_اوصاف(ساخت sbi_mpxy_channel *channel,
				     u32 *outmem, u32 بنیاد_attr_id,
				     u32 attr_گنتی)
{
	ساخت mpxy_rpmi_mbox *rmb =
		container_کا(channel, ساخت mpxy_rpmi_mbox, channel);
	u32 انتہا_id = بنیاد_attr_id + attr_گنتی - 1;
	u32 attr_val, idx;
	صحیح ret, mem_idx;

	اگر (انتہا_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		واپس SBI_EBAD_RANGE;

	mem_idx = 0;
	برائے (idx = بنیاد_attr_id; idx <= انتہا_id; idx++) {
		attr_val = le32_مقصد_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		اگر (ret)
			واپس ret;
	}

	mem_idx = 0;
	برائے (idx = بنیاد_attr_id; idx <= انتہا_id; idx++) {
		attr_val = le32_مقصد_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	واپس SBI_OK;
}

ساکن صحیح __mpxy_mbox_send_message(ساخت sbi_mpxy_channel *channel,
				  u32 پیغام_id_2, خالی *tx, u32 tx_len,
				  خالی *rx, u32 rx_max_len,
				  غیرعلامتی طویل *ack_len)
{
	ساخت mpxy_rpmi_mbox *rmb =
		container_کا(channel, ساخت mpxy_rpmi_mbox, channel);
	ثابت ساخت mpxy_rpmi_mbox_ڈیٹا *ڈیٹا = rmb->mbox_ڈیٹا;
	ثابت ساخت mpxy_rpmi_service_ڈیٹا *srv =
		mpxy_ڈھونڈو_rpmi_srvid(پیغام_id_2, ڈیٹا);
	ساخت rpmi_پیغام_دلائل دلائل = {0};
	ساخت mbox_xfer xfer;
	u32 rx_len = 0;
	صحیح ret;

	اگر (!srv)
		واپس SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	اگر (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		واپس SBI_EFAIL;

	اگر (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		اگر (rx_max_len < srv->min_rx_len)
			واپس SBI_EFAIL;

		/* Max rx data size it can expect */
		اگر (srv->max_rx_len < channel->attrs.پیغام_ڈیٹا_maxlen)
			rx_len = srv->max_rx_len;
		ورنہ
			rx_len = channel->attrs.پیغام_ڈیٹا_maxlen;

		دلائل.type = RPMI_پیغام_معمول_REQUEST;
		دلائل.flags = (rx) ? 0 : RPMI_پیغام_FLAGS_NO_RX;
		دلائل.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &دلائل,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	ورنہ {
		دلائل.type = RPMI_پیغام_POSTED_REQUEST;
		دلائل.flags = RPMI_پیغام_FLAGS_NO_RX;
		دلائل.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &دلائل,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	اگر (ڈیٹا->xfer_گروہ)
		ret = ڈیٹا->xfer_گروہ(rmb->گروہ_context, rmb->chan, &xfer);
	ورنہ
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	اگر (ret)
		واپس ret;

	اگر (ack_len)
		*ack_len = دلائل.rx_ڈیٹا_len;

	واپس SBI_OK;
}

ساکن صحیح mpxy_mbox_send_پیغام_withresp(ساخت sbi_mpxy_channel *channel,
				  u32 پیغام_id_2, خالی *tx, u32 tx_len,
				  خالی *rx, u32 rx_max_len,
				  غیرعلامتی طویل *ack_len)
{
	واپس __mpxy_mbox_send_message(channel, پیغام_id_2, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

ساکن صحیح mpxy_mbox_send_پیغام_withoutresp(ساخت sbi_mpxy_channel *channel,
				  u32 پیغام_id_2, خالی *tx, u32 tx_len)
{
	واپس __mpxy_mbox_send_message(channel, پیغام_id_2, tx, tx_len,
				 تہی, 0, تہی);
}

صحیح mpxy_rpmi_mbox_init(ثابت خالی *fdt, صحیح nodeoff, ثابت ساخت fdt_مطابقت *مطابقت)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_ڈیٹا_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	ثابت ساخت mpxy_rpmi_mbox_ڈیٹا *ڈیٹا = مطابقت->ڈیٹا;
	ساخت mpxy_rpmi_mbox *rmb;
	ساخت mbox_chan *chan;
	ثابت fdt32_t *val;
	صحیح rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(حجم(*rmb));
	اگر (!rmb)
		واپس SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	اگر (rc) {
		rc = SBI_ENODEV;
		جاؤ fail_free_client;
	}

	/* Match channel service group id */
	اگر (ڈیٹا->servicegrp_id != chan->chan_دلائل[0]) {
		rc = SBI_EINVAL;
		جاؤ fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_نسخہ,
				     &pro_ver);
	اگر (rc)
		جاؤ fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_ڈیٹا_LEN,
				     &max_ڈیٹا_len);
	اگر (rc)
		جاؤ fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	اگر (rc)
		جاؤ fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	اگر (rc)
		جاؤ fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_نسخہ,
				     &servicegrp_ver);
	اگر (rc)
		جاؤ fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	اگر (rc)
		جاؤ fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_نسخہ,
				     &impl_ver);
	اگر (rc)
		جاؤ fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	اگر (len > 0 && val) {
		channel_id = fdt32_مقصد_cpu(*val);
	} ورنہ {
		rc = SBI_ENODEV;
		جاؤ fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &جڑ;
	/* Callback for read RPMI attributes */
	rmb->channel.read_اوصاف = mpxy_mbox_read_اوصاف;
	/* Callback for write RPMI attributes */
	rmb->channel.write_اوصاف = mpxy_mbox_write_اوصاف;
	/* Callback for sending RPMI message */
	rmb->channel.send_پیغام_with_response =
					mpxy_mbox_send_پیغام_withresp;
	rmb->channel.send_پیغام_without_response =
					mpxy_mbox_send_پیغام_withoutresp;

	/* RPMI Message Protocol ID */
	rmb->channel.attrs.پیغام_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.پیغام_proto_نسخہ = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.پیغام_ڈیٹا_maxlen = max_ڈیٹا_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.پیغام_send_timeout = tx_tout;
	rmb->channel.attrs.پیغام_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = ڈیٹا->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_ڈیٹا = ڈیٹا;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	اگر (ڈیٹا->تیاری_گروہ) {
		rc = ڈیٹا->تیاری_گروہ(&rmb->گروہ_context, chan, ڈیٹا);
		اگر (rc)
			جاؤ fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_رجسٹر_channel(&rmb->channel);
	اگر (rc) {
		اگر (ڈیٹا->cleanup_گروہ)
			ڈیٹا->cleanup_گروہ(rmb->گروہ_context);
		جاؤ fail_free_chan;
	}

	واپس SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	واپس rc;
}
