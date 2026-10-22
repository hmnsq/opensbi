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
		החזר SBI_OK;
	حالة RPMI_ERR_FAILED:
		החזר SBI_EFAIL;
	حالة RPMI_ERR_NOTSUPP:
		החזר SBI_ENOTSUPP;
	حالة RPMI_ERR_INVALID_PARAM:
		החזר SBI_EINVAL;
	حالة RPMI_ERR_DENIED:
		החזר SBI_EDENIED;
	حالة RPMI_ERR_INVALID_ADDR:
		החזר SBI_EINVALID_ADDR;
	حالة RPMI_ERR_ALREADY:
		החזר SBI_EALREADY;
	حالة RPMI_ERR_EXTENSION:
		החזר SBI_EFAIL;
	حالة RPMI_ERR_HW_FAULT:
		החזר SBI_EIO;
	حالة RPMI_ERR_BUSY:
		החזר SBI_EFAIL;
	حالة RPMI_ERR_INVALID_STATE:
		החזר SBI_EINVALID_STATE;
	حالة RPMI_ERR_BAD_RANGE:
		החזר SBI_EBAD_RANGE;
	حالة RPMI_ERR_TIMEOUT:
		החזר SBI_ETIMEDOUT;
	حالة RPMI_ERR_IO:
		החזר SBI_EIO;
	حالة RPMI_ERR_NO_DATA:
		החזר SBI_EFAIL;
	افتراضي:
		החזר SBI_EUNKNOWN;
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
	אם (ret)
		החזר ret;

	החזר rpmi_xlate_error(((u32 *)resp)[0]);
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

	החזר mbox_chan_xfer(chan, &xfer);
}
