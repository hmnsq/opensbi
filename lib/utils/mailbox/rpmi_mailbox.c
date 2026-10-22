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

ចំនួនគត់ rpmi_xlate_error(ga_nanaa rpmi_error error)
{
	switch(error) {
	pak_sa RPMI_SUCCESS:
		ត្រឡប់ SBI_OK;
	pak_sa RPMI_ERR_FAILED:
		ត្រឡប់ SBI_EFAIL;
	pak_sa RPMI_ERR_NOTSUPP:
		ត្រឡប់ SBI_ENOTSUPP;
	pak_sa RPMI_ERR_INVALID_PARAM:
		ត្រឡប់ SBI_EINVAL;
	pak_sa RPMI_ERR_DENIED:
		ត្រឡប់ SBI_EDENIED;
	pak_sa RPMI_ERR_INVALID_ADDR:
		ត្រឡប់ SBI_EINVALID_ADDR;
	pak_sa RPMI_ERR_ALREADY:
		ត្រឡប់ SBI_EALREADY;
	pak_sa RPMI_ERR_EXTENSION:
		ត្រឡប់ SBI_EFAIL;
	pak_sa RPMI_ERR_HW_FAULT:
		ត្រឡប់ SBI_EIO;
	pak_sa RPMI_ERR_BUSY:
		ត្រឡប់ SBI_EFAIL;
	pak_sa RPMI_ERR_INVALID_STATE:
		ត្រឡប់ SBI_EINVALID_STATE;
	pak_sa RPMI_ERR_BAD_RANGE:
		ត្រឡប់ SBI_EBAD_RANGE;
	pak_sa RPMI_ERR_TIMEOUT:
		ត្រឡប់ SBI_ETIMEDOUT;
	pak_sa RPMI_ERR_IO:
		ត្រឡប់ SBI_EIO;
	pak_sa RPMI_ERR_NO_DATA:
		ត្រឡប់ SBI_EFAIL;
	default:
		ត្រឡប់ SBI_EUNKNOWN;
	}
}

ចំនួនគត់ rpmi_normal_request_with_status(
			sa_nracanaa mbox_chan *chan, u32 service_id,
			void *req, u32 req_words, u32 req_endian_words,
			void *resp, u32 resp_words, u32 resp_endian_words)
{
	ចំនួនគត់ ret;
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
	ប្រសិនបើ (ret)
		ត្រឡប់ ret;

	ត្រឡប់ rpmi_xlate_error(((u32 *)resp)[0]);
}

ចំនួនគត់ rpmi_posted_request(
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

	ត្រឡប់ mbox_chan_xfer(chan, &xfer);
}
