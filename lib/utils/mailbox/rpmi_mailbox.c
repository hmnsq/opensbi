/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/mailbox/mailbox.h>
#অন্তর্ভাব <sbi_utils/mailbox/rpmi_mailbox.h>

পূর্ণাঙ্ক rpmi_xlate_error(গণনা rpmi_error error)
{
	বিকল্প(error) {
	পক্ষ RPMI_SUCCESS:
		প্রত্যাগম SBI_OK;
	পক্ষ RPMI_ERR_FAILED:
		প্রত্যাগম SBI_EFAIL;
	পক্ষ RPMI_ERR_NOTSUPP:
		প্রত্যাগম SBI_ENOTSUPP;
	পক্ষ RPMI_ERR_INVALID_PARAM:
		প্রত্যাগম SBI_EINVAL;
	পক্ষ RPMI_ERR_DENIED:
		প্রত্যাগম SBI_EDENIED;
	পক্ষ RPMI_ERR_INVALID_ADDR:
		প্রত্যাগম SBI_EINVALID_ADDR;
	পক্ষ RPMI_ERR_ALREADY:
		প্রত্যাগম SBI_EALREADY;
	পক্ষ RPMI_ERR_EXTENSION:
		প্রত্যাগম SBI_EFAIL;
	পক্ষ RPMI_ERR_HW_FAULT:
		প্রত্যাগম SBI_EIO;
	পক্ষ RPMI_ERR_BUSY:
		প্রত্যাগম SBI_EFAIL;
	পক্ষ RPMI_ERR_INVALID_STATE:
		প্রত্যাগম SBI_EINVALID_STATE;
	পক্ষ RPMI_ERR_BAD_RANGE:
		প্রত্যাগম SBI_EBAD_RANGE;
	পক্ষ RPMI_ERR_TIMEOUT:
		প্রত্যাগম SBI_ETIMEDOUT;
	পক্ষ RPMI_ERR_IO:
		প্রত্যাগম SBI_EIO;
	পক্ষ RPMI_ERR_NO_DATA:
		প্রত্যাগম SBI_EFAIL;
	স্বভাব:
		প্রত্যাগম SBI_EUNKNOWN;
	}
}

পূর্ণাঙ্ক rpmi_normal_request_with_status(
			সংরচনা mbox_chan *chan, u32 service_id,
			শূন্য *req, u32 req_শব্দগুলি, u32 req_endian_শব্দগুলি,
			শূন্য *resp, u32 resp_শব্দগুলি, u32 resp_endian_শব্দগুলি)
{
	পূর্ণাঙ্ক ret;
	সংরচনা mbox_xfer xfer;
	সংরচনা rpmi_message_args args = { 0 };

	args.type = RPMI_বার্তা_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_শব্দগুলি = req_endian_শব্দগুলি;
	args.rx_endian_শব্দগুলি = resp_endian_শব্দগুলি;
	mbox_xfer_init_txrx(&xfer, &args,
			req, পরিমাণ(u32) * req_শব্দগুলি, RPMI_DEF_TX_TIMEOUT,
			resp, পরিমাণ(u32) * resp_শব্দগুলি, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	যদি (ret)
		প্রত্যাগম ret;

	প্রত্যাগম rpmi_xlate_error(((u32 *)resp)[0]);
}

পূর্ণাঙ্ক rpmi_posted_request(
		সংরচনা mbox_chan *chan, u32 service_id,
		শূন্য *req, u32 req_শব্দগুলি, u32 req_endian_শব্দগুলি)
{
	সংরচনা mbox_xfer xfer;
	সংরচনা rpmi_message_args args = { 0 };

	args.type = RPMI_বার্তা_POSTED_REQUEST;
	args.flags = RPMI_বার্তা_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_শব্দগুলি = req_endian_শব্দগুলি;
	mbox_xfer_init_tx(&xfer, &args,
			  req, পরিমাণ(u32) * req_শব্দগুলি, RPMI_DEF_TX_TIMEOUT);

	প্রত্যাগম mbox_chan_xfer(chan, &xfer);
}
