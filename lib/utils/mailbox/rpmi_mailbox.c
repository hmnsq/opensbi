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

পূর্ণসংখ্যা rpmi_xlate_error(গণনা rpmi_error error)
{
	নির্বাচন(error) {
	ক্ষেত্র RPMI_SUCCESS:
		ফেরত SBI_OK;
	ক্ষেত্র RPMI_ERR_FAILED:
		ফেরত SBI_EFAIL;
	ক্ষেত্র RPMI_ERR_NOTSUPP:
		ফেরত SBI_ENOTSUPP;
	ক্ষেত্র RPMI_ERR_INVALID_PARAM:
		ফেরত SBI_EINVAL;
	ক্ষেত্র RPMI_ERR_DENIED:
		ফেরত SBI_EDENIED;
	ক্ষেত্র RPMI_ERR_INVALID_ADDR:
		ফেরত SBI_EINVALID_ADDR;
	ক্ষেত্র RPMI_ERR_ALREADY:
		ফেরত SBI_EALREADY;
	ক্ষেত্র RPMI_ERR_EXTENSION:
		ফেরত SBI_EFAIL;
	ক্ষেত্র RPMI_ERR_HW_FAULT:
		ফেরত SBI_EIO;
	ক্ষেত্র RPMI_ERR_BUSY:
		ফেরত SBI_EFAIL;
	ক্ষেত্র RPMI_ERR_INVALID_STATE:
		ফেরত SBI_EINVALID_STATE;
	ক্ষেত্র RPMI_ERR_BAD_RANGE:
		ফেরত SBI_EBAD_RANGE;
	ক্ষেত্র RPMI_ERR_TIMEOUT:
		ফেরত SBI_ETIMEDOUT;
	ক্ষেত্র RPMI_ERR_IO:
		ফেরত SBI_EIO;
	ক্ষেত্র RPMI_ERR_NO_তথ্য:
		ফেরত SBI_EFAIL;
	পূর্বনির্ধারিত:
		ফেরত SBI_EUNKNOWN;
	}
}

পূর্ণসংখ্যা rpmi_normal_request_with_status(
			কাঠামো mbox_chan *chan, u32 service_id,
			শূন্য *req, u32 req_শব্দগুলি, u32 req_endian_শব্দগুলি,
			শূন্য *resp, u32 resp_শব্দগুলি, u32 resp_endian_শব্দগুলি)
{
	পূর্ণসংখ্যা ret;
	কাঠামো mbox_xfer xfer;
	কাঠামো rpmi_message_args args = { 0 };

	args.type = RPMI_বার্তা_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_শব্দগুলি = req_endian_শব্দগুলি;
	args.rx_endian_শব্দগুলি = resp_endian_শব্দগুলি;
	mbox_xfer_init_txrx(&xfer, &args,
			req, আকার(u32) * req_শব্দগুলি, RPMI_DEF_TX_TIMEOUT,
			resp, আকার(u32) * resp_শব্দগুলি, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	যদি (ret)
		ফেরত ret;

	ফেরত rpmi_xlate_error(((u32 *)resp)[0]);
}

পূর্ণসংখ্যা rpmi_posted_request(
		কাঠামো mbox_chan *chan, u32 service_id,
		শূন্য *req, u32 req_শব্দগুলি, u32 req_endian_শব্দগুলি)
{
	কাঠামো mbox_xfer xfer;
	কাঠামো rpmi_message_args args = { 0 };

	args.type = RPMI_বার্তা_POSTED_REQUEST;
	args.flags = RPMI_বার্তা_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_শব্দগুলি = req_endian_শব্দগুলি;
	mbox_xfer_init_tx(&xfer, &args,
			  req, আকার(u32) * req_শব্দগুলি, RPMI_DEF_TX_TIMEOUT);

	ফেরত mbox_chan_xfer(chan, &xfer);
}
