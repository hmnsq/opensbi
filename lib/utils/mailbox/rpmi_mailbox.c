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

ціле rpmi_xlate_error(enum rpmi_error error)
{
	switch(error) {
	case RPMI_SUCCESS:
		повернути SBI_OK;
	case RPMI_ERR_FAILED:
		повернути SBI_EFAIL;
	case RPMI_ERR_NOTSUPP:
		повернути SBI_ENOTSUPP;
	case RPMI_ERR_INVALID_PARAM:
		повернути SBI_EINVAL;
	case RPMI_ERR_DENIED:
		повернути SBI_EDENIED;
	case RPMI_ERR_INVALID_ADDR:
		повернути SBI_EINVALID_ADDR;
	case RPMI_ERR_ALREADY:
		повернути SBI_EALREADY;
	case RPMI_ERR_EXTENSION:
		повернути SBI_EFAIL;
	case RPMI_ERR_HW_FAULT:
		повернути SBI_EIO;
	case RPMI_ERR_BUSY:
		повернути SBI_EFAIL;
	case RPMI_ERR_INVALID_STATE:
		повернути SBI_EINVALID_STATE;
	case RPMI_ERR_BAD_RANGE:
		повернути SBI_EBAD_RANGE;
	case RPMI_ERR_TIMEOUT:
		повернути SBI_ETIMEDOUT;
	case RPMI_ERR_IO:
		повернути SBI_EIO;
	case RPMI_ERR_NO_DATA:
		повернути SBI_EFAIL;
	default:
		повернути SBI_EUNKNOWN;
	}
}

ціле rpmi_normal_request_with_status(
			struct mbox_chan *chan, u32 service_id,
			void *req, u32 req_words, u32 req_endian_words,
			void *resp, u32 resp_words, u32 resp_endian_words)
{
	ціле ret;
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
	якщо (ret)
		повернути ret;

	повернути rpmi_xlate_error(((u32 *)resp)[0]);
}

ціле rpmi_posted_request(
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

	повернути mbox_chan_xfer(chan, &xfer);
}
