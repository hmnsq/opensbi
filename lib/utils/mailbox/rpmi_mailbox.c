/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/mailbox/mailbox.h>
#شامل <sbi_utils/mailbox/rpmi_mailbox.h>

صحیح rpmi_xlate_error(شمار rpmi_error error)
{
	انتخاب(error) {
	حالت RPMI_SUCCESS:
		واپس SBI_OK;
	حالت RPMI_ERR_FAILED:
		واپس SBI_EFAIL;
	حالت RPMI_ERR_NOTSUPP:
		واپس SBI_ENOTSUPP;
	حالت RPMI_ERR_INVALID_PARAM:
		واپس SBI_EINVAL;
	حالت RPMI_ERR_DENIED:
		واپس SBI_EDENIED;
	حالت RPMI_ERR_INVALID_ADDR:
		واپس SBI_EINVALID_ADDR;
	حالت RPMI_ERR_ALREADY:
		واپس SBI_EALREADY;
	حالت RPMI_ERR_EXTENSION:
		واپس SBI_EFAIL;
	حالت RPMI_ERR_HW_FAULT:
		واپس SBI_EIO;
	حالت RPMI_ERR_BUSY:
		واپس SBI_EFAIL;
	حالت RPMI_ERR_INVALID_STATE:
		واپس SBI_EINVALID_STATE;
	حالت RPMI_ERR_BAD_RANGE:
		واپس SBI_EBAD_RANGE;
	حالت RPMI_ERR_TIMEOUT:
		واپس SBI_ETIMEDOUT;
	حالت RPMI_ERR_IO:
		واپس SBI_EIO;
	حالت RPMI_ERR_NO_ڈیٹا:
		واپس SBI_EFAIL;
	طےشدہ:
		واپس SBI_EUNKNOWN;
	}
}

صحیح rpmi_معمول_request_with_حالت(
			ساخت mbox_chan *chan, u32 service_id,
			خالی *req, u32 req_الفاظ, u32 req_endian_الفاظ,
			خالی *resp, u32 resp_الفاظ, u32 resp_endian_الفاظ)
{
	صحیح ret;
	ساخت mbox_xfer xfer;
	ساخت rpmi_پیغام_دلائل دلائل = { 0 };

	دلائل.type = RPMI_پیغام_معمول_REQUEST;
	دلائل.service_id = service_id;
	دلائل.tx_endian_الفاظ = req_endian_الفاظ;
	دلائل.rx_endian_الفاظ = resp_endian_الفاظ;
	mbox_xfer_init_txrx(&xfer, &دلائل,
			req, حجم(u32) * req_الفاظ, RPMI_DEF_TX_TIMEOUT,
			resp, حجم(u32) * resp_الفاظ, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	اگر (ret)
		واپس ret;

	واپس rpmi_xlate_error(((u32 *)resp)[0]);
}

صحیح rpmi_posted_request(
		ساخت mbox_chan *chan, u32 service_id,
		خالی *req, u32 req_الفاظ, u32 req_endian_الفاظ)
{
	ساخت mbox_xfer xfer;
	ساخت rpmi_پیغام_دلائل دلائل = { 0 };

	دلائل.type = RPMI_پیغام_POSTED_REQUEST;
	دلائل.flags = RPMI_پیغام_FLAGS_NO_RX;
	دلائل.service_id = service_id;
	دلائل.tx_endian_الفاظ = req_endian_الفاظ;
	mbox_xfer_init_tx(&xfer, &دلائل,
			  req, حجم(u32) * req_الفاظ, RPMI_DEF_TX_TIMEOUT);

	واپس mbox_chan_xfer(chan, &xfer);
}
