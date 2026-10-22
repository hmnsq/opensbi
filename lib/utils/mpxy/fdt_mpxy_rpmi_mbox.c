/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#समावेश <sbi/sbi_console.h>

/**
 * MPXY mbox instance per MPXY channel. This ties
 * an MPXY channel with an RPMI Service group.
 */
काष्ठा mpxy_rpmi_mbox {
	काष्ठा mbox_chan *chan;
	स्थिर काष्ठा mpxy_rpmi_mbox_तथ्य *mbox_तथ्य;
	काष्ठा mpxy_rpmi_channel_attrs msgprot_attrs;
	काष्ठा sbi_mpxy_channel channel;
	व्योम *group_context;
};

/**
 * Discover the RPMI service data using message_id
 * MPXY message_id == RPMI service_id
 */
अटल स्थिर काष्ठा mpxy_rpmi_service_तथ्य *mpxy_find_rpmi_srvid(u32 message_id,
					स्थिर काष्ठा mpxy_rpmi_mbox_तथ्य *mbox_तथ्य)
{
	स्थिर काष्ठा mpxy_rpmi_service_तथ्य *srv = mbox_तथ्य->service_तथ्य;
	पूर्णांक mid = 0;

	क्रम (mid = 0; srv[mid].id < mbox_तथ्य->अंक_services; mid++) {
		अगर (srv[mid].id == (u8)message_id)
			वापस &srv[mid];
	}

	वापस शून्य;
}

/** Copy attributes word size */
अटल व्योम mpxy_copy_attrs(u32 *outmem, u32 *inmem, u32 गिनती)
{
	u32 idx;
	क्रम (idx = 0; idx < गिनती; idx++)
		outmem[idx] = cpu_to_le32(inmem[idx]);
}

अटल पूर्णांक mpxy_mbox_read_attributes(काष्ठा sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_गिनती)
{
	काष्ठा mpxy_rpmi_mbox *rmb =
		container_of(channel, काष्ठा mpxy_rpmi_mbox, channel);
	u32 *attr_array = (u32 *)&rmb->msgprot_attrs;
	u32 end_id = base_attr_id + attr_गिनती - 1;

	अगर (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		वापस SBI_EBAD_RANGE;

	mpxy_copy_attrs(outmem, &attr_array[attr_id2index(base_attr_id)],
			attr_गिनती);

	वापस SBI_OK;
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
अटल पूर्णांक mpxy_check_write_attr(u32 attr_id, u32 attr_val)
{
	पूर्णांक ret = SBI_OK;

	चयन(attr_id) {
	/** All RO access attributes falls under default */
	शेष:
		ret = SBI_EBAD_RANGE;
	};

	वापस ret;
}

अटल व्योम mpxy_write_attr(काष्ठा mpxy_rpmi_channel_attrs *attrs,
			   u32 attr_id,
			   u32 attr_val)
{
	/* No writable attributes in RPMI */
}

अटल पूर्णांक mpxy_mbox_write_attributes(काष्ठा sbi_mpxy_channel *channel,
				     u32 *outmem, u32 base_attr_id,
				     u32 attr_गिनती)
{
	काष्ठा mpxy_rpmi_mbox *rmb =
		container_of(channel, काष्ठा mpxy_rpmi_mbox, channel);
	u32 end_id = base_attr_id + attr_गिनती - 1;
	u32 attr_val, idx;
	पूर्णांक ret, mem_idx;

	अगर (end_id >= MPXY_MSGPROT_RPMI_ATTR_MAX_ID)
		वापस SBI_EBAD_RANGE;

	mem_idx = 0;
	क्रम (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		ret = mpxy_check_write_attr(idx, attr_val);
		अगर (ret)
			वापस ret;
	}

	mem_idx = 0;
	क्रम (idx = base_attr_id; idx <= end_id; idx++) {
		attr_val = le32_to_cpu(outmem[mem_idx++]);
		mpxy_write_attr(&rmb->msgprot_attrs, idx, attr_val);
	}

	वापस SBI_OK;
}

अटल पूर्णांक __mpxy_mbox_send_message(काष्ठा sbi_mpxy_channel *channel,
				  u32 message_id, व्योम *tx, u32 tx_len,
				  व्योम *rx, u32 rx_max_len,
				  अचिन्हित दीर्घ *ack_len)
{
	काष्ठा mpxy_rpmi_mbox *rmb =
		container_of(channel, काष्ठा mpxy_rpmi_mbox, channel);
	स्थिर काष्ठा mpxy_rpmi_mbox_तथ्य *तथ्य = rmb->mbox_तथ्य;
	स्थिर काष्ठा mpxy_rpmi_service_तथ्य *srv =
		mpxy_find_rpmi_srvid(message_id, तथ्य);
	काष्ठा rpmi_message_args args = {0};
	काष्ठा mbox_xfer xfer;
	u32 rx_len = 0;
	पूर्णांक ret;

	अगर (!srv)
		वापस SBI_ENOTSUPP;

	/** message data size to be sent is in the
	 * supported service data size range */
	अगर (tx_len < srv->min_tx_len || tx_len > srv->max_tx_len)
		वापस SBI_EFAIL;

	अगर (ack_len) {
		/** MPXY buffer cannnot hold service min response data */
		अगर (rx_max_len < srv->min_rx_len)
			वापस SBI_EFAIL;

		/* Max rx data size it can expect */
		अगर (srv->max_rx_len < channel->attrs.संदेश_तथ्य_maxlen)
			rx_len = srv->max_rx_len;
		अन्यथा
			rx_len = channel->attrs.संदेश_तथ्य_maxlen;

		args.type = RPMI_संदेश_NORMAL_REQUEST;
		args.flags = (rx) ? 0 : RPMI_संदेश_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_txrx(&xfer, &args,
				    tx, tx_len, RPMI_DEF_TX_TIMEOUT,
				    rx, rx_len, RPMI_DEF_RX_TIMEOUT);
	}
	अन्यथा {
		args.type = RPMI_संदेश_POSTED_REQUEST;
		args.flags = RPMI_संदेश_FLAGS_NO_RX;
		args.service_id = srv->id;
		mbox_xfer_init_tx(&xfer, &args,
				  tx, tx_len, RPMI_DEF_TX_TIMEOUT);
	}

	अगर (तथ्य->xfer_group)
		ret = तथ्य->xfer_group(rmb->group_context, rmb->chan, &xfer);
	अन्यथा
		ret = mbox_chan_xfer(rmb->chan, &xfer);
	अगर (ret)
		वापस ret;

	अगर (ack_len)
		*ack_len = args.rx_तथ्य_len;

	वापस SBI_OK;
}

अटल पूर्णांक mpxy_mbox_send_message_withresp(काष्ठा sbi_mpxy_channel *channel,
				  u32 message_id, व्योम *tx, u32 tx_len,
				  व्योम *rx, u32 rx_max_len,
				  अचिन्हित दीर्घ *ack_len)
{
	वापस __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 rx, rx_max_len, ack_len);
}

अटल पूर्णांक mpxy_mbox_send_message_withoutresp(काष्ठा sbi_mpxy_channel *channel,
				  u32 message_id, व्योम *tx, u32 tx_len)
{
	वापस __mpxy_mbox_send_message(channel, message_id, tx, tx_len,
				 शून्य, 0, शून्य);
}

पूर्णांक mpxy_rpmi_mbox_init(स्थिर व्योम *fdt, पूर्णांक nodeoff, स्थिर काष्ठा fdt_match *match)
{
	u32 channel_id, servicegrp_ver, pro_ver, max_तथ्य_len, tx_tout, rx_tout;
	u32 impl_id, impl_ver;
	स्थिर काष्ठा mpxy_rpmi_mbox_तथ्य *तथ्य = match->तथ्य;
	काष्ठा mpxy_rpmi_mbox *rmb;
	काष्ठा mbox_chan *chan;
	स्थिर fdt32_t *val;
	पूर्णांक rc, len;

	/* Allocate context for MPXY mbox client */
	rmb = sbi_zalloc(माप(*rmb));
	अगर (!rmb)
		वापस SBI_ENOMEM;

	/*
	 * If channel request failed then other end does not support
	 * service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	अगर (rc) {
		rc = SBI_ENODEV;
		जाओ fail_free_client;
	}

	/* Match channel service group id */
	अगर (तथ्य->servicegrp_id != chan->chan_args[0]) {
		rc = SBI_EINVAL;
		जाओ fail_free_chan;
	}

	/* Get channel protocol version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_PROTOCOL_VERSION,
				     &pro_ver);
	अगर (rc)
		जाओ fail_free_chan;

	/* Get channel maximum data length */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_MAX_तथ्य_LEN,
				     &max_तथ्य_len);
	अगर (rc)
		जाओ fail_free_chan;

	/* Get channel Tx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_TX_TIMEOUT,
				     &tx_tout);
	अगर (rc)
		जाओ fail_free_chan;

	/* Get channel Rx timeout */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_RX_TIMEOUT,
				     &rx_tout);
	अगर (rc)
		जाओ fail_free_chan;

	/* Get channel service group version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION,
				     &servicegrp_ver);
	अगर (rc)
		जाओ fail_free_chan;

	/* Get channel implementation id */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_ID,
				     &impl_id);
	अगर (rc)
		जाओ fail_free_chan;

	/* Get channel implementation version */
	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_IMPL_VERSION,
				     &impl_ver);
	अगर (rc)
		जाओ fail_free_chan;

	/*
	 * The "riscv,sbi-mpxy-channel-id" DT property is mandatory
	 * for MPXY RPMI mailbox client driver so if this is not
	 * present then try other drivers.
	 */
	val = fdt_getprop(fdt, nodeoff, "riscv,sbi-mpxy-channel-id", &len);
	अगर (len > 0 && val) {
		channel_id = fdt32_to_cpu(*val);
	} अन्यथा {
		rc = SBI_ENODEV;
		जाओ fail_free_chan;
	}

	/* Setup MPXY mbox client */
	/* Channel ID*/
	rmb->channel.channel_id = channel_id;
	/* Set the owner domain */
	rmb->channel.owner_domain = &मूल;
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
	rmb->channel.attrs.संदेश_proto_id = SBI_MPXY_MSGPROTO_RPMI_ID;
	/* RPMI Message Protocol Version */
	rmb->channel.attrs.संदेश_proto_version = pro_ver;

	/*
	 * RPMI supported max message data length(bytes), same for
	 * all service groups
	 */
	rmb->channel.attrs.संदेश_तथ्य_maxlen = max_तथ्य_len;
	/*
	 * RPMI message send timeout(milliseconds)
	 * same for all service groups
	 */
	rmb->channel.attrs.संदेश_send_timeout = tx_tout;
	rmb->channel.attrs.संदेश_completion_timeout = tx_tout + rx_tout;

	/* RPMI service group attributes */
	rmb->msgprot_attrs.servicegrp_id = तथ्य->servicegrp_id;
	rmb->msgprot_attrs.servicegrp_ver = servicegrp_ver;
	rmb->msgprot_attrs.impl_id = impl_id;
	rmb->msgprot_attrs.impl_ver = impl_ver;

	rmb->mbox_तथ्य = तथ्य;
	rmb->chan = chan;

	/* Setup RPMI service group context */
	अगर (तथ्य->setup_group) {
		rc = तथ्य->setup_group(&rmb->group_context, chan, तथ्य);
		अगर (rc)
			जाओ fail_free_chan;
	}

	/* Register RPMI service group */
	rc = sbi_mpxy_register_channel(&rmb->channel);
	अगर (rc) {
		अगर (तथ्य->cleanup_group)
			तथ्य->cleanup_group(rmb->group_context);
		जाओ fail_free_chan;
	}

	वापस SBI_OK;

fail_free_chan:
	mbox_controller_free_chan(chan);
fail_free_client:
	sbi_free(rmb);
	वापस rc;
}
