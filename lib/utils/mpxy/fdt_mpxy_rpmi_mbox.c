/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <libfdt.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#include <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
struct mpxy_rpmi_mbox {
	struct mbox_chan *chan;
	const struct mpxy_rpmi_mbox_data *mbox_data;
	struct mpxy_rpmi_channel_attrs msgprot_attrs;
	struct sbi_mpxy_channel channel;
	void *group_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
static const struct mpxy_rpmi_service_data *mpxy_find_rpmi_srvid(u32 message_id,
					const struct mpxy_rpmi_mbox_data *mbox_data)
{
	const struct mpxy_rpmi_service_data *srv = mbox_data->service_data;
	số_nguyên mid = 0;

	cho (mid = 0; srv[mid].id < mbox_data->num_services; mid++) {
		nếu (srv[mid].id == (u8)message_id)
			trả_về &srv[mid];
	}

	trả_về NULL;
}

/** Copy attributes word size */
static void mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 count)
{
	u32 idx;
	cho (idx = 0; idx < count; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

static số_nguyên mpxy_mbox_read_attributes(struct sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_count)
{
	struct mpxy_rpmi_mbox *rmb =
		container_of(channel, struct mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 end_id = base_attr_id + attr_count - 1;

	nếu (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		trả_về SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(base_attr_id)],
			attr_count);

	trả_về SBI_OK;
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
static số_nguyên mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	số_nguyên ret = SBI_OK;

	switch(attr_id) {
	/** All RO access attributes falls under default */
	default:
		ret = SBI_EBAD_RANGE;
	};

	trả_về ret;
}

static void mpxy_write_attr(struct mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

static số_nguyên mpxy_mbox_write_attributes(struct sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_count)
{
	struct mpxy_rpmi_mbox *rmb =
		container_of(channel, struct mpxy_rpmi_mbox, channel);
	u32 end_id = base_attr_id + attr_count - 1;
	u32 attr_val, idx;
	số_nguyên ret, mem_idx;

	nếu (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		trả_về SBI_EBAD_RANGE;

	mem_idx = 0;
	cho (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		nếu (ret)
			trả_về ret;
	}

	mem_idx = 0;
	cho (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	trả_về SBI_OK;
}

static số_nguyên __mpxy_mbox_send_message(struct sbi_mpxy_channel *channel,
				  u32 message_id, void *tx, u32 tx_len,
				  void *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	struct mpxy_rpmi_mbox *rmb =
		container_of(channel, struct mpxy_rpmi_mbox, channel);
	const struct mpxy_rpmi_mbox_data *data = rmb->mbox_data;
	const struct mpxy_rpmi_service_data *srv =
		mpxy_find_rpmi_srvid(message_id, data);
	struct rpmi_message_args args = {0};
	struct mbox_xfer xfer;
	u32 rx_len = 0;
	số_nguyên ret;

	nếu (!srv)
		trả_về SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	nếu (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		trả_về SBI_EFAIL;

	nếu (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		nếu (rx_max_len < srv->min_rx_len)
			trả_về SBI_EFAIL;

		/* Max rx data size it can expect */
		nếu (srv->max_rx_len < channel->attrs.msg_data_maxlen)
			rx_len = srv->max_rx_len;
		không_thì
			rx_len = channel->attrs.msg_data_maxlen;

		args.type = RPMI_MSG_NORMAL_REQUEST;
		args.flags = (rx) ? 0 : RPMI_MSG_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &args,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	không_thì {
		args.type = RPMI_MSG_POSTED_REQUEST;
		args.flags = RPMI_MSG_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &args,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	nếu (data->xfer_group)
		ret = data->xfer_group(rmb->group_context, rmb->chan, &xfer);
	không_thì
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	nếu (ret)
		trả_về ret;

	nếu (ack_len)
		*ack_len = args.rx_data_len;

	trả_về SBI_OK;
}

static số_nguyên mpxy_mbox_send_message_withresp(struct sbi_mpxy_channel *channel,
				  u32 message_id, void *tx, u32 tx_len,
				  void *rx, u32 rx_max_len,
				  unsigned long *ack_len)
{
	trả_về __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

static số_nguyên mpxy_mbox_send_message_withoutresp(struct sbi_mpxy_channel *channel,
				  u32 message_id, void *tx, u32 tx_len)
{
	trả_về __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 NULL, 0, NULL);
}

số_nguyên mpxy_rpmi_mbox_init(const void *fdt, số_nguyên nodeoff, const struct fdt_match *match)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_data_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	const struct mpxy_rpmi_mbox_data *data = match->data;
	struct mpxy_rpmi_mbox *rmb;
	struct mbox_chan *chan;
	const fdt32_t *val;
	số_nguyên rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(sizeof(*rmb));
	nếu (!rmb)
		trả_về SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	nếu (rc) {
		rc = SBI_ENODEV;
		goto fail_free_client;
	}

	/* Match channel service group id */
	nếu (data->servicegrp_id != chan->chan_args[0]) {
		rc = SBI_EINVAL;
		goto fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_VERSION,
				     &pro_ver);
	nếu (rc)
		goto fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_DATA_LEN,
				     &max_data_len);
	nếu (rc)
		goto fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	nếu (rc)
		goto fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	nếu (rc)
		goto fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION,
				     &servicegrp_ver);
	nếu (rc)
		goto fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	nếu (rc)
		goto fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_VERSION,
				     &impl_ver);
	nếu (rc)
		goto fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	nếu (len > 0 && val) {
		channel_id = fdt32_to_cpu(*val);
	} không_thì {
		rc = SBI_ENODEV;
		goto fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &root;
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
	rmb->channel.attrs.msg_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.msg_proto_version = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.msg_data_maxlen = max_data_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.msg_send_timeout = tx_tout;
	rmb->channel.attrs.msg_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = data->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_data = data;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	nếu (data->setup_group) {
		rc = data->setup_group(&rmb->group_context, chan, data);
		nếu (rc)
			goto fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_register_channel(&rmb->channel);
	nếu (rc) {
		nếu (data->cleanup_group)
			data->cleanup_group(rmb->group_context);
		goto fail_free_chan;
	}

	trả_về SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	trả_về rc;
}
