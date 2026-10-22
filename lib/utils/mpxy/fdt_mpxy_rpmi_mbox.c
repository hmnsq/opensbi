/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#অন্তর্ভাব <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
কাঠামো mpxy_rpmi_mbox {
	কাঠামো mbox_chan *chan;
	ধ্রুবক কাঠামো mpxy_rpmi_mbox_তথ্য *mbox_তথ্য;
	কাঠামো mpxy_rpmi_channel_attrs msgprot_attrs;
	কাঠামো sbi_mpxy_channel channel;
	শূন্য *group_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
স্থির ধ্রুবক কাঠামো mpxy_rpmi_service_তথ্য *mpxy_find_rpmi_srvid(u32 message_id,
					ধ্রুবক কাঠামো mpxy_rpmi_mbox_তথ্য *mbox_তথ্য)
{
	ধ্রুবক কাঠামো mpxy_rpmi_service_তথ্য *srv = mbox_তথ্য->service_তথ্য;
	পূর্ণসংখ্যা mid = 0;

	জন্য (mid = 0; srv[mid].id < mbox_তথ্য->অঙ্ক_services; mid++) {
		যদি (srv[mid].id == (u8)message_id)
			ফেরত &srv[mid];
	}

	ফেরত ফাঁকা;
}

/** Copy attributes word size */
স্থির শূন্য mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 গুনতি)
{
	u32 idx;
	জন্য (idx = 0; idx < গুনতি; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

স্থির পূর্ণসংখ্যা mpxy_mbox_read_attributes(কাঠামো sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_গুনতি)
{
	কাঠামো mpxy_rpmi_mbox *rmb =
		container_of(channel, কাঠামো mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 end_id = base_attr_id + attr_গুনতি - 1;

	যদি (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ফেরত SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(base_attr_id)],
			attr_গুনতি);

	ফেরত SBI_OK;
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
স্থির পূর্ণসংখ্যা mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	পূর্ণসংখ্যা ret = SBI_OK;

	নির্বাচন(attr_id) {
	/** All RO access attributes falls under default */
	পূর্বনির্ধারিত:
		ret = SBI_EBAD_RANGE;
	};

	ফেরত ret;
}

স্থির শূন্য mpxy_write_attr(কাঠামো mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

স্থির পূর্ণসংখ্যা mpxy_mbox_write_attributes(কাঠামো sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_গুনতি)
{
	কাঠামো mpxy_rpmi_mbox *rmb =
		container_of(channel, কাঠামো mpxy_rpmi_mbox, channel);
	u32 end_id = base_attr_id + attr_গুনতি - 1;
	u32 attr_val, idx;
	পূর্ণসংখ্যা ret, mem_idx;

	যদি (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		ফেরত SBI_EBAD_RANGE;

	mem_idx = 0;
	জন্য (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		যদি (ret)
			ফেরত ret;
	}

	mem_idx = 0;
	জন্য (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা __mpxy_mbox_send_message(কাঠামো sbi_mpxy_channel *channel,
				  u32 message_id, শূন্য *tx, u32 tx_len,
				  শূন্য *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	কাঠামো mpxy_rpmi_mbox *rmb =
		container_of(channel, কাঠামো mpxy_rpmi_mbox, channel);
	ধ্রুবক কাঠামো mpxy_rpmi_mbox_তথ্য *তথ্য = rmb->mbox_তথ্য;
	ধ্রুবক কাঠামো mpxy_rpmi_service_তথ্য *srv =
		mpxy_find_rpmi_srvid(message_id, তথ্য);
	কাঠামো rpmi_message_args args = {0};
	কাঠামো mbox_xfer xfer;
	u32 rx_len = 0;
	পূর্ণসংখ্যা ret;

	যদি (!srv)
		ফেরত SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	যদি (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		ফেরত SBI_EFAIL;

	যদি (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		যদি (rx_max_len < srv->min_rx_len)
			ফেরত SBI_EFAIL;

		/* Max rx data size it can expect */
		যদি (srv->max_rx_len < channel->attrs.বার্তা_তথ্য_maxlen)
			rx_len = srv->max_rx_len;
		অন্যথা
			rx_len = channel->attrs.বার্তা_তথ্য_maxlen;

		args.type = RPMI_বার্তা_NORMAL_REQUEST;
		args.flags = (rx) ? 0 : RPMI_বার্তা_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &args,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	অন্যথা {
		args.type = RPMI_বার্তা_POSTED_REQUEST;
		args.flags = RPMI_বার্তা_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &args,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	যদি (তথ্য->xfer_group)
		ret = তথ্য->xfer_group(rmb->group_context, rmb->chan, &xfer);
	অন্যথা
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	যদি (ret)
		ফেরত ret;

	যদি (ack_len)
		*ack_len = args.rx_তথ্য_len;

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা mpxy_mbox_send_message_withresp(কাঠামো sbi_mpxy_channel *channel,
				  u32 message_id, শূন্য *tx, u32 tx_len,
				  শূন্য *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	ফেরত __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

স্থির পূর্ণসংখ্যা mpxy_mbox_send_message_withoutresp(কাঠামো sbi_mpxy_channel *channel,
				  u32 message_id, শূন্য *tx, u32 tx_len)
{
	ফেরত __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 ফাঁকা, 0, ফাঁকা);
}

পূর্ণসংখ্যা mpxy_rpmi_mbox_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff, ধ্রুবক কাঠামো fdt_match *match)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_তথ্য_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	ধ্রুবক কাঠামো mpxy_rpmi_mbox_তথ্য *তথ্য = match->তথ্য;
	কাঠামো mpxy_rpmi_mbox *rmb;
	কাঠামো mbox_chan *chan;
	ধ্রুবক fdt32_t *val;
	পূর্ণসংখ্যা rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(আকার(*rmb));
	যদি (!rmb)
		ফেরত SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	যদি (rc) {
		rc = SBI_ENODEV;
		যাও fail_free_client;
	}

	/* Match channel service group id */
	যদি (তথ্য->servicegrp_id != chan->chan_args[0]) {
		rc = SBI_EINVAL;
		যাও fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_VERSION,
				     &pro_ver);
	যদি (rc)
		যাও fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_তথ্য_LEN,
				     &max_তথ্য_len);
	যদি (rc)
		যাও fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	যদি (rc)
		যাও fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	যদি (rc)
		যাও fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION,
				     &servicegrp_ver);
	যদি (rc)
		যাও fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	যদি (rc)
		যাও fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_VERSION,
				     &impl_ver);
	যদি (rc)
		যাও fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	যদি (len > 0 && val) {
		channel_id = fdt32_to_cpu(*val);
	} অন্যথা {
		rc = SBI_ENODEV;
		যাও fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &মূল;
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
	rmb->channel.attrs.বার্তা_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.বার্তা_proto_version = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.বার্তা_তথ্য_maxlen = max_তথ্য_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.বার্তা_send_timeout = tx_tout;
	rmb->channel.attrs.বার্তা_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = তথ্য->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_তথ্য = তথ্য;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	যদি (তথ্য->setup_group) {
		rc = তথ্য->setup_group(&rmb->group_context, chan, তথ্য);
		যদি (rc)
			যাও fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_register_channel(&rmb->channel);
	যদি (rc) {
		যদি (তথ্য->cleanup_group)
			তথ্য->cleanup_group(rmb->group_context);
		যাও fail_free_chan;
	}

	ফেরত SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	ফেরত rc;
}
