/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <sbi/sbi_error.h>
#include <sbi_utils/mailbox/mailbox.h>
#include <sbi_utils/mailbox/rpmi_mailbox.h>

整数 rpmi_xlate_error(enum rpmi_error error)
{
	switch(error) {
	case RPMI_SUCCESS:
		戻る SBI_OK;
	case RPMI_ERR_FAILED:
		戻る SBI_EFAIL;
	case RPMI_ERR_NOTSUPP:
		戻る SBI_ENOTSUPP;
	case RPMI_ERR_INVALID_PARAM:
		戻る SBI_EINVAL;
	case RPMI_ERR_DENIED:
		戻る SBI_EDENIED;
	case RPMI_ERR_INVALID_ADDR:
		戻る SBI_EINVALID_ADDR;
	case RPMI_ERR_ALREADY:
		戻る SBI_EALREADY;
	case RPMI_ERR_EXTENSION:
		戻る SBI_EFAIL;
	case RPMI_ERR_HW_FAULT:
		戻る SBI_EIO;
	case RPMI_ERR_BUSY:
		戻る SBI_EFAIL;
	case RPMI_ERR_INVALID_STATE:
		戻る SBI_EINVALID_STATE;
	case RPMI_ERR_BAD_RANGE:
		戻る SBI_EBAD_RANGE;
	case RPMI_ERR_TIMEOUT:
		戻る SBI_ETIMEDOUT;
	case RPMI_ERR_IO:
		戻る SBI_EIO;
	case RPMI_ERR_NO_DATA:
		戻る SBI_EFAIL;
	default:
		戻る SBI_EUNKNOWN;
	}
}

整数 rpmi_normal_request_with_status(
			struct mbox_chan *chan, u32 service_id,
			void *req, u32 req_words, u32 req_endian_words,
			void *resp, u32 resp_words, u32 resp_endian_words)
{
	整数 ret;
	struct mbox_xfer xfer;
	struct rpmi_message_args args = { 0 };

	args.type = RPMI_MSG_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_words = req_endian_words;
	args.rx_endian_words = resp_endian_words;
	mbox_xfer_init_txrx(&xfer, &args,
			req, sizeof(u32) * req_words, RPMI_DEF_TX_TIMEOUT,
			resp, sizeof(u32) * resp_words, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	もし (ret)
		戻る ret;

	戻る rpmi_xlate_error(((u32 *)resp)[0]);
}

整数 rpmi_posted_request(
		struct mbox_chan *chan, u32 service_id,
		void *req, u32 req_words, u32 req_endian_words)
{
	struct mbox_xfer xfer;
	struct rpmi_message_args args = { 0 };

	args.type = RPMI_MSG_POSTED_REQUEST;
	args.flags = RPMI_MSG_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_words = req_endian_words;
	mbox_xfer_init_tx(&xfer, &args,
			  req, sizeof(u32) * req_words, RPMI_DEF_TX_TIMEOUT);

	戻る mbox_chan_xfer(chan, &xfer);
}
