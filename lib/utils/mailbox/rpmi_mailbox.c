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

puur_naa_nka rpmi_xlate_error(ga_nanaa rpmi_error error)
{
	switch(error) {
	pak_sa RPMI_SUCCESS:
		ᨀᨘᨒ SBI_OK;
	pak_sa RPMI_ERR_FAILED:
		ᨀᨘᨒ SBI_EFAIL;
	pak_sa RPMI_ERR_NOTSUPP:
		ᨀᨘᨒ SBI_ENOTSUPP;
	pak_sa RPMI_ERR_INVALID_PARAM:
		ᨀᨘᨒ SBI_EINVAL;
	pak_sa RPMI_ERR_DENIED:
		ᨀᨘᨒ SBI_EDENIED;
	pak_sa RPMI_ERR_INVALID_ADDR:
		ᨀᨘᨒ SBI_EINVALID_ADDR;
	pak_sa RPMI_ERR_ALREADY:
		ᨀᨘᨒ SBI_EALREADY;
	pak_sa RPMI_ERR_EXTENSION:
		ᨀᨘᨒ SBI_EFAIL;
	pak_sa RPMI_ERR_HW_FAULT:
		ᨀᨘᨒ SBI_EIO;
	pak_sa RPMI_ERR_BUSY:
		ᨀᨘᨒ SBI_EFAIL;
	pak_sa RPMI_ERR_INVALID_STATE:
		ᨀᨘᨒ SBI_EINVALID_STATE;
	pak_sa RPMI_ERR_BAD_RANGE:
		ᨀᨘᨒ SBI_EBAD_RANGE;
	pak_sa RPMI_ERR_TIMEOUT:
		ᨀᨘᨒ SBI_ETIMEDOUT;
	pak_sa RPMI_ERR_IO:
		ᨀᨘᨒ SBI_EIO;
	pak_sa RPMI_ERR_NO_DATA:
		ᨀᨘᨒ SBI_EFAIL;
	default:
		ᨀᨘᨒ SBI_EUNKNOWN;
	}
}

puur_naa_nka rpmi_normal_request_with_status(
			sa_nracanaa mbox_chan *chan, u32 service_id,
			void *req, u32 req_words, u32 req_endian_words,
			void *resp, u32 resp_words, u32 resp_endian_words)
{
	puur_naa_nka ret;
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
	ᨀᨘ (ret)
		ᨀᨘᨒ ret;

	ᨀᨘᨒ rpmi_xlate_error(((u32 *)resp)[0]);
}

puur_naa_nka rpmi_posted_request(
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

	ᨀᨘᨒ mbox_chan_xfer(chan, &xfer);
}
