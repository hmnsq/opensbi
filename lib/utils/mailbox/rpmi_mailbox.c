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
		إرجاع SBI_OK;
	حالة RPMI_ERR_FAILED:
		إرجاع SBI_EFAIL;
	حالة RPMI_ERR_NOTSUPP:
		إرجاع SBI_ENOTSUPP;
	حالة RPMI_ERR_INVALID_PARAM:
		إرجاع SBI_EINVAL;
	حالة RPMI_ERR_DENIED:
		إرجاع SBI_EDENIED;
	حالة RPMI_ERR_INVALID_ADDR:
		إرجاع SBI_EINVALID_ADDR;
	حالة RPMI_ERR_ALREADY:
		إرجاع SBI_EALREADY;
	حالة RPMI_ERR_EXTENSION:
		إرجاع SBI_EFAIL;
	حالة RPMI_ERR_HW_FAULT:
		إرجاع SBI_EIO;
	حالة RPMI_ERR_BUSY:
		إرجاع SBI_EFAIL;
	حالة RPMI_ERR_INVALID_STATE:
		إرجاع SBI_EINVALID_STATE;
	حالة RPMI_ERR_BAD_RANGE:
		إرجاع SBI_EBAD_RANGE;
	حالة RPMI_ERR_TIMEOUT:
		إرجاع SBI_ETIMEDOUT;
	حالة RPMI_ERR_IO:
		إرجاع SBI_EIO;
	حالة RPMI_ERR_NO_DATA:
		إرجاع SBI_EFAIL;
	افتراضي:
		إرجاع SBI_EUNKNOWN;
	}
}

صحيح rpmi_normal_request_with_status(
			بنية mbox_chan *chan, u32 service_id,
			فراغ *req, u32 req_كلمات, u32 req_endian_كلمات,
			فراغ *resp, u32 resp_كلمات, u32 resp_endian_كلمات)
{
	صحيح ret;
	بنية mbox_xfer xfer;
	بنية rpmi_message_args args = { 0 };

	args.type = RPMI_رسالة_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_كلمات = req_endian_كلمات;
	args.rx_endian_كلمات = resp_endian_كلمات;
	mbox_xfer_init_txrx(&xfer, &args,
			req, حجم(u32) * req_كلمات, RPMI_DEF_TX_TIMEOUT,
			resp, حجم(u32) * resp_كلمات, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	إذا (ret)
		إرجاع ret;

	إرجاع rpmi_xlate_error(((u32 *)resp)[0]);
}

صحيح rpmi_posted_request(
		بنية mbox_chan *chan, u32 service_id,
		فراغ *req, u32 req_كلمات, u32 req_endian_كلمات)
{
	بنية mbox_xfer xfer;
	بنية rpmi_message_args args = { 0 };

	args.type = RPMI_رسالة_POSTED_REQUEST;
	args.flags = RPMI_رسالة_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_كلمات = req_endian_كلمات;
	mbox_xfer_init_tx(&xfer, &args,
			  req, حجم(u32) * req_كلمات, RPMI_DEF_TX_TIMEOUT);

	إرجاع mbox_chan_xfer(chan, &xfer);
}
