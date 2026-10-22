/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/mailbox/mailbox.h>
#அந்தர்பாவ <sbi_utils/mailbox/rpmi_mailbox.h>

முழு rpmi_xlate_error(எண்ணிக்கை rpmi_error error)
{
	தேர்வு(error) {
	நிலை RPMI_SUCCESS:
		திரும்பு SBI_OK;
	நிலை RPMI_ERR_FAILED:
		திரும்பு SBI_EFAIL;
	நிலை RPMI_ERR_NOTSUPP:
		திரும்பு SBI_ENOTSUPP;
	நிலை RPMI_ERR_INVALID_PARAM:
		திரும்பு SBI_EINVAL;
	நிலை RPMI_ERR_DENIED:
		திரும்பு SBI_EDENIED;
	நிலை RPMI_ERR_INVALID_ADDR:
		திரும்பு SBI_EINVALID_ADDR;
	நிலை RPMI_ERR_ALREADY:
		திரும்பு SBI_EALREADY;
	நிலை RPMI_ERR_EXTENSION:
		திரும்பு SBI_EFAIL;
	நிலை RPMI_ERR_HW_FAULT:
		திரும்பு SBI_EIO;
	நிலை RPMI_ERR_BUSY:
		திரும்பு SBI_EFAIL;
	நிலை RPMI_ERR_INVALID_STATE:
		திரும்பு SBI_EINVALID_STATE;
	நிலை RPMI_ERR_BAD_RANGE:
		திரும்பு SBI_EBAD_RANGE;
	நிலை RPMI_ERR_TIMEOUT:
		திரும்பு SBI_ETIMEDOUT;
	நிலை RPMI_ERR_IO:
		திரும்பு SBI_EIO;
	நிலை RPMI_ERR_NO_DATA:
		திரும்பு SBI_EFAIL;
	இயல்பு:
		திரும்பு SBI_EUNKNOWN;
	}
}

முழு rpmi_normal_request_with_status(
			கட்டமைப்பு mbox_chan *chan, u32 service_id,
			வெறுமை *req, u32 req_சொற்கள், u32 req_endian_சொற்கள்,
			வெறுமை *resp, u32 resp_சொற்கள், u32 resp_endian_சொற்கள்)
{
	முழு ret;
	கட்டமைப்பு mbox_xfer xfer;
	கட்டமைப்பு rpmi_message_args args = { 0 };

	args.type = RPMI_செய்தி_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_சொற்கள் = req_endian_சொற்கள்;
	args.rx_endian_சொற்கள் = resp_endian_சொற்கள்;
	mbox_xfer_init_txrx(&xfer, &args,
			req, அளவு(u32) * req_சொற்கள், RPMI_DEF_TX_TIMEOUT,
			resp, அளவு(u32) * resp_சொற்கள், RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	எனில் (ret)
		திரும்பு ret;

	திரும்பு rpmi_xlate_error(((u32 *)resp)[0]);
}

முழு rpmi_posted_request(
		கட்டமைப்பு mbox_chan *chan, u32 service_id,
		வெறுமை *req, u32 req_சொற்கள், u32 req_endian_சொற்கள்)
{
	கட்டமைப்பு mbox_xfer xfer;
	கட்டமைப்பு rpmi_message_args args = { 0 };

	args.type = RPMI_செய்தி_POSTED_REQUEST;
	args.flags = RPMI_செய்தி_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_சொற்கள் = req_endian_சொற்கள்;
	mbox_xfer_init_tx(&xfer, &args,
			  req, அளவு(u32) * req_சொற்கள், RPMI_DEF_TX_TIMEOUT);

	திரும்பு mbox_chan_xfer(chan, &xfer);
}
