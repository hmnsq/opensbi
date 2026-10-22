/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi_utils/mailbox/mailbox.h>
#অন্তর্ভাৱ <sbi_utils/mailbox/rpmi_mailbox.h>

পূর্ণাঙ্ক rpmi_xlate_error(গণনা rpmi_error error)
{
	ৱিকল্প(error) {
	পক্ষ RPMI_SUCCESS:
		ঘূৰাই SBI_OK;
	পক্ষ RPMI_ERR_FAILED:
		ঘূৰাই SBI_EFAIL;
	পক্ষ RPMI_ERR_NOTSUPP:
		ঘূৰাই SBI_ENOTSUPP;
	পক্ষ RPMI_ERR_INVALID_PARAM:
		ঘূৰাই SBI_EINVAL;
	পক্ষ RPMI_ERR_DENIED:
		ঘূৰাই SBI_EDENIED;
	পক্ষ RPMI_ERR_INVALID_ADDR:
		ঘূৰাই SBI_EINVALID_ADDR;
	পক্ষ RPMI_ERR_ALREADY:
		ঘূৰাই SBI_EALREADY;
	পক্ষ RPMI_ERR_EXTENSION:
		ঘূৰাই SBI_EFAIL;
	পক্ষ RPMI_ERR_HW_FAULT:
		ঘূৰাই SBI_EIO;
	পক্ষ RPMI_ERR_BUSY:
		ঘূৰাই SBI_EFAIL;
	পক্ষ RPMI_ERR_INVALID_STATE:
		ঘূৰাই SBI_EINVALID_STATE;
	পক্ষ RPMI_ERR_BAD_RANGE:
		ঘূৰাই SBI_EBAD_RANGE;
	পক্ষ RPMI_ERR_TIMEOUT:
		ঘূৰাই SBI_ETIMEDOUT;
	পক্ষ RPMI_ERR_IO:
		ঘূৰাই SBI_EIO;
	পক্ষ RPMI_ERR_NO_DATA:
		ঘূৰাই SBI_EFAIL;
	স্ৱভাৱ:
		ঘূৰাই SBI_EUNKNOWN;
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

	args.type = RPMI_বাৰ্তা_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_শব্দগুলি = req_endian_শব্দগুলি;
	args.rx_endian_শব্দগুলি = resp_endian_শব্দগুলি;
	mbox_xfer_init_txrx(&xfer, &args,
			req, পরিমাণ(u32) * req_শব্দগুলি, RPMI_DEF_TX_TIMEOUT,
			resp, পরিমাণ(u32) * resp_শব্দগুলি, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	যদি (ret)
		ঘূৰাই ret;

	ঘূৰাই rpmi_xlate_error(((u32 *)resp)[0]);
}

পূর্ণাঙ্ক rpmi_posted_request(
		সংরচনা mbox_chan *chan, u32 service_id,
		শূন্য *req, u32 req_শব্দগুলি, u32 req_endian_শব্দগুলি)
{
	সংরচনা mbox_xfer xfer;
	সংরচনা rpmi_message_args args = { 0 };

	args.type = RPMI_বাৰ্তা_POSTED_REQUEST;
	args.flags = RPMI_বাৰ্তা_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_শব্দগুলি = req_endian_শব্দগুলি;
	mbox_xfer_init_tx(&xfer, &args,
			  req, পরিমাণ(u32) * req_শব্দগুলি, RPMI_DEF_TX_TIMEOUT);

	ঘূৰাই mbox_chan_xfer(chan, &xfer);
}
