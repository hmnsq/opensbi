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

صحیح rpmi_xlate_error(شمارش rpmi_error error)
{
	انتخاب(error) {
	حالت RPMI_SUCCESS:
		بازگشت SBI_OK;
	حالت RPMI_ERR_FAILED:
		بازگشت SBI_EFAIL;
	حالت RPMI_ERR_NOTSUPP:
		بازگشت SBI_ENOTSUPP;
	حالت RPMI_ERR_INVALID_PARAM:
		بازگشت SBI_EINVAL;
	حالت RPMI_ERR_DENIED:
		بازگشت SBI_EDENIED;
	حالت RPMI_ERR_INVALID_ADDR:
		بازگشت SBI_EINVALID_ADDR;
	حالت RPMI_ERR_ALREADY:
		بازگشت SBI_EALREADY;
	حالت RPMI_ERR_EXTENSION:
		بازگشت SBI_EFAIL;
	حالت RPMI_ERR_HW_FAULT:
		بازگشت SBI_EIO;
	حالت RPMI_ERR_BUSY:
		بازگشت SBI_EFAIL;
	حالت RPMI_ERR_INVALID_STATE:
		بازگشت SBI_EINVALID_STATE;
	حالت RPMI_ERR_BAD_RANGE:
		بازگشت SBI_EBAD_RANGE;
	حالت RPMI_ERR_TIMEOUT:
		بازگشت SBI_ETIMEDOUT;
	حالت RPMI_ERR_IO:
		بازگشت SBI_EIO;
	حالت RPMI_ERR_NO_DATA:
		بازگشت SBI_EFAIL;
	پیشفرض:
		بازگشت SBI_EUNKNOWN;
	}
}

صحیح rpmi_normal_request_with_status(
			ساختار mbox_chan *chan, u32 service_id,
			تهی *req, u32 req_واژگان, u32 req_endian_واژگان,
			تهی *resp, u32 resp_واژگان, u32 resp_endian_واژگان)
{
	صحیح ret;
	ساختار mbox_xfer xfer;
	ساختار rpmi_message_args args = { 0 };

	args.type = RPMI_پیام_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_واژگان = req_endian_واژگان;
	args.rx_endian_واژگان = resp_endian_واژگان;
	mbox_xfer_init_txrx(&xfer, &args,
			req, اندازه(u32) * req_واژگان, RPMI_DEF_TX_TIMEOUT,
			resp, اندازه(u32) * resp_واژگان, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	اگر (ret)
		بازگشت ret;

	بازگشت rpmi_xlate_error(((u32 *)resp)[0]);
}

صحیح rpmi_posted_request(
		ساختار mbox_chan *chan, u32 service_id,
		تهی *req, u32 req_واژگان, u32 req_endian_واژگان)
{
	ساختار mbox_xfer xfer;
	ساختار rpmi_message_args args = { 0 };

	args.type = RPMI_پیام_POSTED_REQUEST;
	args.flags = RPMI_پیام_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_واژگان = req_endian_واژگان;
	mbox_xfer_init_tx(&xfer, &args,
			  req, اندازه(u32) * req_واژگان, RPMI_DEF_TX_TIMEOUT);

	بازگشت mbox_chan_xfer(chan, &xfer);
}
