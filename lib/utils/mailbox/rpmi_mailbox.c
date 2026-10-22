/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/mailbox/mailbox.h>
#અન્તર્ભાવ <sbi_utils/mailbox/rpmi_mailbox.h>

પૂર્ણાંક rpmi_xlate_error(ગણના rpmi_error error)
{
	પસંદગી(error) {
	સ્થિતિ RPMI_SUCCESS:
		પાછા SBI_OK;
	સ્થિતિ RPMI_ERR_FAILED:
		પાછા SBI_EFAIL;
	સ્થિતિ RPMI_ERR_NOTSUPP:
		પાછા SBI_ENOTSUPP;
	સ્થિતિ RPMI_ERR_INVALID_PARAM:
		પાછા SBI_EINVAL;
	સ્થિતિ RPMI_ERR_DENIED:
		પાછા SBI_EDENIED;
	સ્થિતિ RPMI_ERR_INVALID_ADDR:
		પાછા SBI_EINVALID_ADDR;
	સ્થિતિ RPMI_ERR_ALREADY:
		પાછા SBI_EALREADY;
	સ્થિતિ RPMI_ERR_EXTENSION:
		પાછા SBI_EFAIL;
	સ્થિતિ RPMI_ERR_HW_FAULT:
		પાછા SBI_EIO;
	સ્થિતિ RPMI_ERR_BUSY:
		પાછા SBI_EFAIL;
	સ્થિતિ RPMI_ERR_INVALID_STATE:
		પાછા SBI_EINVALID_STATE;
	સ્થિતિ RPMI_ERR_BAD_RANGE:
		પાછા SBI_EBAD_RANGE;
	સ્થિતિ RPMI_ERR_TIMEOUT:
		પાછા SBI_ETIMEDOUT;
	સ્થિતિ RPMI_ERR_IO:
		પાછા SBI_EIO;
	સ્થિતિ RPMI_ERR_NO_DATA:
		પાછા SBI_EFAIL;
	મૂળભૂત:
		પાછા SBI_EUNKNOWN;
	}
}

પૂર્ણાંક rpmi_normal_request_with_status(
			સંરચના mbox_chan *chan, u32 service_id,
			શૂન્ય *req, u32 req_શબ્દો, u32 req_endian_શબ્દો,
			શૂન્ય *resp, u32 resp_શબ્દો, u32 resp_endian_શબ્દો)
{
	પૂર્ણાંક ret;
	સંરચના mbox_xfer xfer;
	સંરચના rpmi_message_args args = { 0 };

	args.type = RPMI_સંદેશ_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_શબ્દો = req_endian_શબ્દો;
	args.rx_endian_શબ્દો = resp_endian_શબ્દો;
	mbox_xfer_init_txrx(&xfer, &args,
			req, કદ(u32) * req_શબ્દો, RPMI_DEF_TX_TIMEOUT,
			resp, કદ(u32) * resp_શબ્દો, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	જો (ret)
		પાછા ret;

	પાછા rpmi_xlate_error(((u32 *)resp)[0]);
}

પૂર્ણાંક rpmi_posted_request(
		સંરચના mbox_chan *chan, u32 service_id,
		શૂન્ય *req, u32 req_શબ્દો, u32 req_endian_શબ્દો)
{
	સંરચના mbox_xfer xfer;
	સંરચના rpmi_message_args args = { 0 };

	args.type = RPMI_સંદેશ_POSTED_REQUEST;
	args.flags = RPMI_સંદેશ_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_શબ્દો = req_endian_શબ્દો;
	mbox_xfer_init_tx(&xfer, &args,
			  req, કદ(u32) * req_શબ્દો, RPMI_DEF_TX_TIMEOUT);

	પાછા mbox_chan_xfer(chan, &xfer);
}
