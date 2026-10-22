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
	ثابت بنية mpxy_rpmi_mbox_انکڑے *mbox_انکڑے;
	بنية mpxy_rpmi_channel_attrs msgprot_attrs;
	بنية sbi_mpxy_channel channel;
	فراغ *گروہ_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
ساكن ثابت بنية mpxy_rpmi_service_انکڑے *mpxy_لبھو_rpmi_srvid(u32 سنیہا_id,
					ثابت بنية mpxy_rpmi_mbox_انکڑے *mbox_انکڑے)
{
	ثابت بنية mpxy_rpmi_service_انکڑے *srv = mbox_انکڑے->service_انکڑے;
	صحيح mid = 0;

	لكل (mid = 0; srv[mid].id < mbox_انکڑے->عدد_services; mid++) {
		إذا (srv[mid].id == (u8)سنیہا_id)
			أرجع &srv[mid];
	}

	أرجع عدم;
}

/** Copy attributes word size */
ساكن فراغ mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 گنتی)
{
	u32 idx;
	لكل (idx = 0; idx < گنتی; idx++)
		outmem[idx] = cpu_منزل_le32(inmem[idx]);
}

ساكن صحيح mpxy_mbox_read_گن(بنية sbi_mpxy_channel *channel,
				     u32 *outmem, u32 ادھار_attr_id,
				     u32 attr_گنتی)
{
	بنية mpxy_rpmi_mbox *rmb =
		container_دا(channel, بنية mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 سرا_id = ادھار_attr_id + attr_گنتی - 1;

	إذا (سرا_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		أرجع SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(ادھار_attr_id)],
			attr_گنتی);

	أرجع SBI_OK;
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

	أرجع ret;
}

ساكن فراغ mpxy_write_attr(بنية mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

ساكن صحيح mpxy_mbox_write_گن(بنية sbi_mpxy_channel *channel,
				     u32 *outmem, u32 ادھار_attr_id,
				     u32 attr_گنتی)
{
	بنية mpxy_rpmi_mbox *rmb =
		container_دا(channel, بنية mpxy_rpmi_mbox, channel);
	u32 سرا_id = ادھار_attr_id + attr_گنتی - 1;
	u32 attr_val, idx;
	صحيح ret, mem_idx;

	إذا (سرا_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		أرجع SBI_EBAD_RANGE;

	mem_idx = 0;
	لكل (idx = ادھار_attr_id; idx <= سرا_id; idx++) {
		attr_val = le32_منزل_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		إذا (ret)
			أرجع ret;
	}

	mem_idx = 0;
	لكل (idx = ادھار_attr_id; idx <= سرا_id; idx++) {
		attr_val = le32_منزل_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	أرجع SBI_OK;
}

ساكن صحيح __mpxy_mbox_send_message(بنية sbi_mpxy_channel *channel,
				  u32 سنیہا_id, فراغ *tx, u32 tx_len,
				  فراغ *rx, u32 rx_max_len,
				  بنانشانی لما *ack_len)
{
	بنية mpxy_rpmi_mbox *rmb =
		container_دا(channel, بنية mpxy_rpmi_mbox, channel);
	ثابت بنية mpxy_rpmi_mbox_انکڑے *انکڑے = rmb->mbox_انکڑے;
	ثابت بنية mpxy_rpmi_service_انکڑے *srv =
		mpxy_لبھو_rpmi_srvid(سنیہا_id, انکڑے);
	بنية rpmi_سنیہا_دلیلاں دلیلاں = {0};
	بنية mbox_xfer xfer;
	u32 rx_len = 0;
	صحيح ret;

	إذا (!srv)
		أرجع SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	إذا (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		أرجع SBI_EFAIL;

	إذا (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		إذا (rx_max_len < srv->min_rx_len)
			أرجع SBI_EFAIL;

		/* Max rx data size it can expect */
		إذا (srv->max_rx_len < channel->attrs.پیغام_انکڑے_maxlen)
			rx_len = srv->max_rx_len;
		وإلا
			rx_len = channel->attrs.پیغام_انکڑے_maxlen;

		دلیلاں.type = RPMI_پیغام_عام_REQUEST;
		دلیلاں.flags = (rx) ? 0 : RPMI_پیغام_FLAGS_NO_RX;
		دلیلاں.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &دلیلاں,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	وإلا {
		دلیلاں.type = RPMI_پیغام_POSTED_REQUEST;
		دلیلاں.flags = RPMI_پیغام_FLAGS_NO_RX;
		دلیلاں.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &دلیلاں,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	إذا (انکڑے->xfer_گروہ)
		ret = انکڑے->xfer_گروہ(rmb->گروہ_context, rmb->chan, &xfer);
	وإلا
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	إذا (ret)
		أرجع ret;

	إذا (ack_len)
		*ack_len = دلیلاں.rx_انکڑے_len;

	أرجع SBI_OK;
}

ساكن صحيح mpxy_mbox_send_سنیہا_withresp(بنية sbi_mpxy_channel *channel,
				  u32 سنیہا_id, فراغ *tx, u32 tx_len,
				  فراغ *rx, u32 rx_max_len,
				  بنانشانی لما *ack_len)
{
	أرجع __mpxy_mbox_send_message(channel, سنیہا_id, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

ساكن صحيح mpxy_mbox_send_سنیہا_withoutresp(بنية sbi_mpxy_channel *channel,
				  u32 سنیہا_id, فراغ *tx, u32 tx_len)
{
	أرجع __mpxy_mbox_send_message(channel, سنیہا_id, tx, tx_len,
				 عدم, 0, عدم);
}

صحيح mpxy_rpmi_mbox_init(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_میل *میل)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_انکڑے_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	ثابت بنية mpxy_rpmi_mbox_انکڑے *انکڑے = میل->انکڑے;
	بنية mpxy_rpmi_mbox *rmb;
	بنية mbox_chan *chan;
	ثابت fdt32_t *val;
	صحيح rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(حجم(*rmb));
	إذا (!rmb)
		أرجع SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	إذا (rc) {
		rc = SBI_ENODEV;
		اذهب fail_free_client;
	}

	/* Match channel service group id */
	إذا (انکڑے->servicegrp_id != chan->chan_دلیلاں[0]) {
		rc = SBI_EINVAL;
		اذهب fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_نسخہ,
				     &pro_ver);
	إذا (rc)
		اذهب fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_انکڑے_LEN,
				     &max_انکڑے_len);
	إذا (rc)
		اذهب fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	إذا (rc)
		اذهب fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	إذا (rc)
		اذهب fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_نسخہ,
				     &servicegrp_ver);
	إذا (rc)
		اذهب fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	إذا (rc)
		اذهب fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_نسخہ,
				     &impl_ver);
	إذا (rc)
		اذهب fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	إذا (len > 0 && val) {
		channel_id = fdt32_منزل_cpu(*val);
	} وإلا {
		rc = SBI_ENODEV;
		اذهب fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &جڑ;
	/* Callback for read RPMI attributes */
	rmb->channel.read_گن = mpxy_mbox_read_گن;
	/* Callback for write RPMI attributes */
	rmb->channel.write_گن = mpxy_mbox_write_گن;
	/* Callback for sending RPMI message */
	rmb->channel.send_سنیہا_with_response =
					mpxy_mbox_send_سنیہا_withresp;
	rmb->channel.send_سنیہا_without_response =
					mpxy_mbox_send_سنیہا_withoutresp;

	/* RPMI Message Protocol ID */
	rmb->channel.attrs.پیغام_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.پیغام_proto_نسخہ = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.پیغام_انکڑے_maxlen = max_انکڑے_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.پیغام_send_timeout = tx_tout;
	rmb->channel.attrs.پیغام_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = انکڑے->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_انکڑے = انکڑے;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	إذا (انکڑے->تیاری_گروہ) {
		rc = انکڑے->تیاری_گروہ(&rmb->گروہ_context, chan, انکڑے);
		إذا (rc)
			اذهب fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_رجسٹر_channel(&rmb->channel);
	إذا (rc) {
		إذا (انکڑے->cleanup_گروہ)
			انکڑے->cleanup_گروہ(rmb->گروہ_context);
		اذهب fail_free_chan;
	}

	أرجع SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	أرجع rc;
}
