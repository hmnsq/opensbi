/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/mailbox/mailbox.h>
#ضمن <sbi_utils/mailbox/rpmi_mailbox.h>

صحيح rpmi_xlate_error(تعداد rpmi_error error)
{
	بدل(error) {
	حالة RPMI_SUCCESS:
		ܦܢܐ SBI_OK;
	حالة RPMI_ERR_FAILED:
		ܦܢܐ SBI_EFAIL;
	حالة RPMI_ERR_NOTSUPP:
		ܦܢܐ SBI_ENOTSUPP;
	حالة RPMI_ERR_INVALID_PARAM:
		ܦܢܐ SBI_EINVAL;
	حالة RPMI_ERR_DENIED:
		ܦܢܐ SBI_EDENIED;
	حالة RPMI_ERR_INVALID_ADDR:
		ܦܢܐ SBI_EINVALID_ADDR;
	حالة RPMI_ERR_ALREADY:
		ܦܢܐ SBI_EALREADY;
	حالة RPMI_ERR_EXTENSION:
		ܦܢܐ SBI_EFAIL;
	حالة RPMI_ERR_HW_FAULT:
		ܦܢܐ SBI_EIO;
	حالة RPMI_ERR_BUSY:
		ܦܢܐ SBI_EFAIL;
	حالة RPMI_ERR_INVALID_STATE:
		ܦܢܐ SBI_EINVALID_STATE;
	حالة RPMI_ERR_BAD_RANGE:
		ܦܢܐ SBI_EBAD_RANGE;
	حالة RPMI_ERR_TIMEOUT:
		ܦܢܐ SBI_ETIMEDOUT;
	حالة RPMI_ERR_IO:
		ܦܢܐ SBI_EIO;
	حالة RPMI_ERR_NO_DATA:
		ܦܢܐ SBI_EFAIL;
	افتراضي:
		ܦܢܐ SBI_EUNKNOWN;
	}
}

صحيح rpmi_normal_request_with_status(
			بنية mbox_chan *chan, u32 service_id,
			فراغ *req, u32 req_words, u32 req_endian_words,
			فراغ *resp, u32 resp_words, u32 resp_endian_words)
{
	صحيح ret;
	بنية mbox_xfer xfer;
	بنية rpmi_message_args args = { 0 };

	args.type = RPMI_MSG_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_words = req_endian_words;
	args.rx_endian_words = resp_endian_words;
	mbox_xfer_init_txrx(&xfer, &args,
			req, حجم(u32) * req_words, RPMI_DEF_TX_TIMEOUT,
			resp, حجم(u32) * resp_words, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	ܐܢ (ret)
		ܦܢܐ ret;

	ܦܢܐ rpmi_xlate_error(((u32 *)resp)[0]);
}

صحيح rpmi_posted_request(
		بنية mbox_chan *chan, u32 service_id,
		فراغ *req, u32 req_words, u32 req_endian_words)
{
	بنية mbox_xfer xfer;
	بنية rpmi_message_args args = { 0 };

	args.type = RPMI_MSG_POSTED_REQUEST;
	args.flags = RPMI_MSG_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_words = req_endian_words;
	mbox_xfer_init_tx(&xfer, &args,
			  req, حجم(u32) * req_words, RPMI_DEF_TX_TIMEOUT);

	ܦܢܐ mbox_chan_xfer(chan, &xfer);
}
