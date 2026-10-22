/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/mailbox.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/rpmi_mailbox.h>

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_xlate_error(ଗଣନା rpmi_error error)
{
	ବାଛ(error) {
	ସ୍ଥିତି RPMI_SUCCESS:
		ଫେରନ୍ତୁ SBI_OK;
	ସ୍ଥିତି RPMI_ERR_FAILED:
		ଫେରନ୍ତୁ SBI_EFAIL;
	ସ୍ଥିତି RPMI_ERR_NOTSUPP:
		ଫେରନ୍ତୁ SBI_ENOTSUPP;
	ସ୍ଥିତି RPMI_ERR_INVALID_PARAM:
		ଫେରନ୍ତୁ SBI_EINVAL;
	ସ୍ଥିତି RPMI_ERR_DENIED:
		ଫେରନ୍ତୁ SBI_EDENIED;
	ସ୍ଥିତି RPMI_ERR_INVALID_ADDR:
		ଫେରନ୍ତୁ SBI_EINVALID_ADDR;
	ସ୍ଥିତି RPMI_ERR_ALREADY:
		ଫେରନ୍ତୁ SBI_EALREADY;
	ସ୍ଥିତି RPMI_ERR_EXTENSION:
		ଫେରନ୍ତୁ SBI_EFAIL;
	ସ୍ଥିତି RPMI_ERR_HW_FAULT:
		ଫେରନ୍ତୁ SBI_EIO;
	ସ୍ଥିତି RPMI_ERR_BUSY:
		ଫେରନ୍ତୁ SBI_EFAIL;
	ସ୍ଥିତି RPMI_ERR_INVALID_STATE:
		ଫେରନ୍ତୁ SBI_EINVALID_STATE;
	ସ୍ଥିତି RPMI_ERR_BAD_RANGE:
		ଫେରନ୍ତୁ SBI_EBAD_RANGE;
	ସ୍ଥିତି RPMI_ERR_TIMEOUT:
		ଫେରନ୍ତୁ SBI_ETIMEDOUT;
	ସ୍ଥିତି RPMI_ERR_IO:
		ଫେରନ୍ତୁ SBI_EIO;
	ସ୍ଥିତି RPMI_ERR_NO_DATA:
		ଫେରନ୍ତୁ SBI_EFAIL;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ SBI_EUNKNOWN;
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_normal_request_with_status(
			ସଂରଚନା mbox_chan *chan, u32 service_id,
			ଶୂନ୍ୟ *req, u32 req_ଶବ୍ଦଗୁଡ଼ିକ, u32 req_endian_ଶବ୍ଦଗୁଡ଼ିକ,
			ଶୂନ୍ୟ *resp, u32 resp_ଶବ୍ଦଗୁଡ଼ିକ, u32 resp_endian_ଶବ୍ଦଗୁଡ଼ିକ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା mbox_xfer xfer;
	ସଂରଚନା rpmi_message_args args = { 0 };

	args.type = RPMI_ସନ୍ଦେଶ_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_ଶବ୍ଦଗୁଡ଼ିକ = req_endian_ଶବ୍ଦଗୁଡ଼ିକ;
	args.rx_endian_ଶବ୍ଦଗୁଡ଼ିକ = resp_endian_ଶବ୍ଦଗୁଡ଼ିକ;
	mbox_xfer_init_txrx(&xfer, &args,
			req, ଆକାର(u32) * req_ଶବ୍ଦଗୁଡ଼ିକ, RPMI_DEF_TX_TIMEOUT,
			resp, ଆକାର(u32) * resp_ଶବ୍ଦଗୁଡ଼ିକ, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	ଫେରନ୍ତୁ rpmi_xlate_error(((u32 *)resp)[0]);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_posted_request(
		ସଂରଚନା mbox_chan *chan, u32 service_id,
		ଶୂନ୍ୟ *req, u32 req_ଶବ୍ଦଗୁଡ଼ିକ, u32 req_endian_ଶବ୍ଦଗୁଡ଼ିକ)
{
	ସଂରଚନା mbox_xfer xfer;
	ସଂରଚନା rpmi_message_args args = { 0 };

	args.type = RPMI_ସନ୍ଦେଶ_POSTED_REQUEST;
	args.flags = RPMI_ସନ୍ଦେଶ_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_ଶବ୍ଦଗୁଡ଼ିକ = req_endian_ଶବ୍ଦଗୁଡ଼ିକ;
	mbox_xfer_init_tx(&xfer, &args,
			  req, ଆକାର(u32) * req_ଶବ୍ଦଗୁଡ଼ିକ, RPMI_DEF_TX_TIMEOUT);

	ଫେରନ୍ତୁ mbox_chan_xfer(chan, &xfer);
}
