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

정수 rpmi_xlate_error(enum rpmi_error error)
{
	switch(error) {
	case RPMI_SUCCESS:
		반환 SBI_OK;
	case RPMI_ERR_FAILED:
		반환 SBI_EFAIL;
	case RPMI_ERR_NOTSUPP:
		반환 SBI_ENOTSUPP;
	case RPMI_ERR_INVALID_PARAM:
		반환 SBI_EINVAL;
	case RPMI_ERR_DENIED:
		반환 SBI_EDENIED;
	case RPMI_ERR_INVALID_ADDR:
		반환 SBI_EINVALID_ADDR;
	case RPMI_ERR_ALREADY:
		반환 SBI_EALREADY;
	case RPMI_ERR_EXTENSION:
		반환 SBI_EFAIL;
	case RPMI_ERR_HW_FAULT:
		반환 SBI_EIO;
	case RPMI_ERR_BUSY:
		반환 SBI_EFAIL;
	case RPMI_ERR_INVALID_STATE:
		반환 SBI_EINVALID_STATE;
	case RPMI_ERR_BAD_RANGE:
		반환 SBI_EBAD_RANGE;
	case RPMI_ERR_TIMEOUT:
		반환 SBI_ETIMEDOUT;
	case RPMI_ERR_IO:
		반환 SBI_EIO;
	case RPMI_ERR_NO_DATA:
		반환 SBI_EFAIL;
	default:
		반환 SBI_EUNKNOWN;
	}
}

정수 rpmi_normal_request_with_status(
			struct mbox_chan *chan, u32 service_id,
			void *req, u32 req_words, u32 req_endian_words,
			void *resp, u32 resp_words, u32 resp_endian_words)
{
	정수 ret;
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
	만약 (ret)
		반환 ret;

	반환 rpmi_xlate_error(((u32 *)resp)[0]);
}

정수 rpmi_posted_request(
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

	반환 mbox_chan_xfer(chan, &xfer);
}
