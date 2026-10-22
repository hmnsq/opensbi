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

ཧྲིལ rpmi_xlate_error(ga_nanaa rpmi_error error)
{
	switch(error) {
	pak_sa RPMI_SUCCESS:
		ལོག SBI_OK;
	pak_sa RPMI_ERR_FAILED:
		ལོག SBI_EFAIL;
	pak_sa RPMI_ERR_NOTSUPP:
		ལོག SBI_ENOTSUPP;
	pak_sa RPMI_ERR_INVALID_PARAM:
		ལོག SBI_EINVAL;
	pak_sa RPMI_ERR_DENIED:
		ལོག SBI_EDENIED;
	pak_sa RPMI_ERR_INVALID_ADDR:
		ལོག SBI_EINVALID_ADDR;
	pak_sa RPMI_ERR_ALREADY:
		ལོག SBI_EALREADY;
	pak_sa RPMI_ERR_EXTENSION:
		ལོག SBI_EFAIL;
	pak_sa RPMI_ERR_HW_FAULT:
		ལོག SBI_EIO;
	pak_sa RPMI_ERR_BUSY:
		ལོག SBI_EFAIL;
	pak_sa RPMI_ERR_INVALID_STATE:
		ལོག SBI_EINVALID_STATE;
	pak_sa RPMI_ERR_BAD_RANGE:
		ལོག SBI_EBAD_RANGE;
	pak_sa RPMI_ERR_TIMEOUT:
		ལོག SBI_ETIMEDOUT;
	pak_sa RPMI_ERR_IO:
		ལོག SBI_EIO;
	pak_sa RPMI_ERR_NO_DATA:
		ལོག SBI_EFAIL;
	default:
		ལོག SBI_EUNKNOWN;
	}
}

ཧྲིལ rpmi_normal_request_with_status(
			sa_nracanaa mbox_chan *chan, u32 service_id,
			void *req, u32 req_words, u32 req_endian_words,
			void *resp, u32 resp_words, u32 resp_endian_words)
{
	ཧྲིལ ret;
	sa_nracanaa mbox_xfer xfer;
	sa_nracanaa rpmi_message_args args = { 0 };

	args.type = RPMI_MSG_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_words = req_endian_words;
	args.rx_endian_words = resp_endian_words;
	mbox_xfer_init_txrx(&xfer, &args,
			req, parimaa_na(u32) * req_words, RPMI_DEF_TX_TIMEOUT,
			resp, parimaa_na(u32) * resp_words, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	གལ་སྲིད (ret)
		ལོག ret;

	ལོག rpmi_xlate_error(((u32 *)resp)[0]);
}

ཧྲིལ rpmi_posted_request(
		sa_nracanaa mbox_chan *chan, u32 service_id,
		void *req, u32 req_words, u32 req_endian_words)
{
	sa_nracanaa mbox_xfer xfer;
	sa_nracanaa rpmi_message_args args = { 0 };

	args.type = RPMI_MSG_POSTED_REQUEST;
	args.flags = RPMI_MSG_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_words = req_endian_words;
	mbox_xfer_init_tx(&xfer, &args,
			  req, parimaa_na(u32) * req_words, RPMI_DEF_TX_TIMEOUT);

	ལོག mbox_chan_xfer(chan, &xfer);
}
