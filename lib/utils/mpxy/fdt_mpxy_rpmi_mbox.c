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
	ثابت بنية mpxy_rpmi_mbox_معلومات *mbox_معلومات;
	بنية mpxy_rpmi_channel_attrs msgprot_attrs;
	بنية sbi_mpxy_channel channel;
	فراغ *ډله_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
ساكن ثابت بنية mpxy_rpmi_service_معلومات *mpxy_پیدا_کړه_rpmi_srvid(u32 پیغام_id_2,
					ثابت بنية mpxy_rpmi_mbox_معلومات *mbox_معلومات)
{
	ثابت بنية mpxy_rpmi_service_معلومات *srv = mbox_معلومات->service_معلومات;
	صحيح mid = 0;

	لكل (mid = 0; srv[mid].id < mbox_معلومات->عدد_services; mid++) {
		إذا (srv[mid].id == (u8)پیغام_id_2)
			أرجع &srv[mid];
	}

	أرجع عدم;
}

/** Copy attributes word size */
ساكن فراغ mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 شمېر)
{
	u32 idx;
	لكل (idx = 0; idx < شمېر; idx++)
		outmem[idx] = cpu_منزل_le32(inmem[idx]);
}

ساكن صحيح mpxy_mbox_read_ځانګړتیاوې(بنية sbi_mpxy_channel *channel,
				     u32 *outmem, u32 بنسټ_attr_id,
				     u32 attr_شمېر)
{
	بنية mpxy_rpmi_mbox *rmb =
		container_د(channel, بنية mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 پای_ټکی_id = بنسټ_attr_id + attr_شمېر - 1;

	إذا (پای_ټکی_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		أرجع SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(بنسټ_attr_id)],
			attr_شمېر);

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

ساكن صحيح mpxy_mbox_write_ځانګړتیاوې(بنية sbi_mpxy_channel *channel,
				     u32 *outmem, u32 بنسټ_attr_id,
				     u32 attr_شمېر)
{
	بنية mpxy_rpmi_mbox *rmb =
		container_د(channel, بنية mpxy_rpmi_mbox, channel);
	u32 پای_ټکی_id = بنسټ_attr_id + attr_شمېر - 1;
	u32 attr_val, idx;
	صحيح ret, mem_idx;

	إذا (پای_ټکی_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		أرجع SBI_EBAD_RANGE;

	mem_idx = 0;
	لكل (idx = بنسټ_attr_id; idx <= پای_ټکی_id; idx++) {
		attr_val = le32_منزل_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		إذا (ret)
			أرجع ret;
	}

	mem_idx = 0;
	لكل (idx = بنسټ_attr_id; idx <= پای_ټکی_id; idx++) {
		attr_val = le32_منزل_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	أرجع SBI_OK;
}

ساكن صحيح __mpxy_mbox_send_message(بنية sbi_mpxy_channel *channel,
				  u32 پیغام_id_2, فراغ *tx, u32 tx_len,
				  فراغ *rx, u32 rx_max_len,
				  بینښه اوږد *ack_len)
{
	بنية mpxy_rpmi_mbox *rmb =
		container_د(channel, بنية mpxy_rpmi_mbox, channel);
	ثابت بنية mpxy_rpmi_mbox_معلومات *معلومات = rmb->mbox_معلومات;
	ثابت بنية mpxy_rpmi_service_معلومات *srv =
		mpxy_پیدا_کړه_rpmi_srvid(پیغام_id_2, معلومات);
	بنية rpmi_پیغام_دلیلونه دلیلونه = {0};
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
		إذا (srv->max_rx_len < channel->attrs.پیغام_معلومات_maxlen)
			rx_len = srv->max_rx_len;
		وإلا
			rx_len = channel->attrs.پیغام_معلومات_maxlen;

		دلیلونه.type = RPMI_پیغام_نورمال_REQUEST;
		دلیلونه.flags = (rx) ? 0 : RPMI_پیغام_FLAGS_NO_RX;
		دلیلونه.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &دلیلونه,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	وإلا {
		دلیلونه.type = RPMI_پیغام_POSTED_REQUEST;
		دلیلونه.flags = RPMI_پیغام_FLAGS_NO_RX;
		دلیلونه.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &دلیلونه,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	إذا (معلومات->xfer_ډله)
		ret = معلومات->xfer_ډله(rmb->ډله_context, rmb->chan, &xfer);
	وإلا
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	إذا (ret)
		أرجع ret;

	إذا (ack_len)
		*ack_len = دلیلونه.rx_معلومات_len;

	أرجع SBI_OK;
}

ساكن صحيح mpxy_mbox_send_پیغام_withresp(بنية sbi_mpxy_channel *channel,
				  u32 پیغام_id_2, فراغ *tx, u32 tx_len,
				  فراغ *rx, u32 rx_max_len,
				  بینښه اوږد *ack_len)
{
	أرجع __mpxy_mbox_send_message(channel, پیغام_id_2, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

ساكن صحيح mpxy_mbox_send_پیغام_withoutresp(بنية sbi_mpxy_channel *channel,
				  u32 پیغام_id_2, فراغ *tx, u32 tx_len)
{
	أرجع __mpxy_mbox_send_message(channel, پیغام_id_2, tx, tx_len,
				 عدم, 0, عدم);
}

صحيح mpxy_rpmi_mbox_init(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_سمون *سمون)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_معلومات_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	ثابت بنية mpxy_rpmi_mbox_معلومات *معلومات = سمون->معلومات;
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
	إذا (معلومات->servicegrp_id != chan->chan_دلیلونه[0]) {
		rc = SBI_EINVAL;
		اذهب fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_نسخه,
				     &pro_ver);
	إذا (rc)
		اذهب fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_معلومات_LEN,
				     &max_معلومات_len);
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
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_نسخه,
				     &servicegrp_ver);
	إذا (rc)
		اذهب fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	إذا (rc)
		اذهب fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_نسخه,
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
	rmb->channel.owner_domain = &ریښه;
	/* Callback for read RPMI attributes */
	rmb->channel.read_ځانګړتیاوې = mpxy_mbox_read_ځانګړتیاوې;
	/* Callback for write RPMI attributes */
	rmb->channel.write_ځانګړتیاوې = mpxy_mbox_write_ځانګړتیاوې;
	/* Callback for sending RPMI message */
	rmb->channel.send_پیغام_with_response =
					mpxy_mbox_send_پیغام_withresp;
	rmb->channel.send_پیغام_without_response =
					mpxy_mbox_send_پیغام_withoutresp;

	/* RPMI Message Protocol ID */
	rmb->channel.attrs.پیغام_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.پیغام_proto_نسخه = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.پیغام_معلومات_maxlen = max_معلومات_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.پیغام_send_timeout = tx_tout;
	rmb->channel.attrs.پیغام_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = معلومات->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_معلومات = معلومات;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	إذا (معلومات->تیاری_ډله) {
		rc = معلومات->تیاری_ډله(&rmb->ډله_context, chan, معلومات);
		إذا (rc)
			اذهب fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_ثبتوونکی_channel(&rmb->channel);
	إذا (rc) {
		إذا (معلومات->cleanup_ډله)
			معلومات->cleanup_ډله(rmb->ډله_context);
		اذهب fail_free_chan;
	}

	أرجع SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	أرجع rc;
}
