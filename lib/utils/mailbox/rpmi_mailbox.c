/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/mailbox.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/rpmi_mailbox.h>

ಪೂರ್ಣಾಙ್ಕ rpmi_xlate_error(ಗಣನಾ rpmi_error error)
{
	ವಿಕಲ್ಪ(error) {
	ಪಕ್ಷ RPMI_SUCCESS:
		ಪ್ರತ್ಯಾಗಮ SBI_OK;
	ಪಕ್ಷ RPMI_ERR_FAILED:
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;
	ಪಕ್ಷ RPMI_ERR_NOTSUPP:
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
	ಪಕ್ಷ RPMI_ERR_INVALID_PARAM:
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಪಕ್ಷ RPMI_ERR_DENIED:
		ಪ್ರತ್ಯಾಗಮ SBI_EDENIED;
	ಪಕ್ಷ RPMI_ERR_INVALID_ADDR:
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_ADDR;
	ಪಕ್ಷ RPMI_ERR_ALREADY:
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;
	ಪಕ್ಷ RPMI_ERR_EXTENSION:
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;
	ಪಕ್ಷ RPMI_ERR_HW_FAULT:
		ಪ್ರತ್ಯಾಗಮ SBI_EIO;
	ಪಕ್ಷ RPMI_ERR_BUSY:
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;
	ಪಕ್ಷ RPMI_ERR_INVALID_STATE:
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_STATE;
	ಪಕ್ಷ RPMI_ERR_BAD_RANGE:
		ಪ್ರತ್ಯಾಗಮ SBI_EBAD_RANGE;
	ಪಕ್ಷ RPMI_ERR_TIMEOUT:
		ಪ್ರತ್ಯಾಗಮ SBI_ETIMEDOUT;
	ಪಕ್ಷ RPMI_ERR_IO:
		ಪ್ರತ್ಯಾಗಮ SBI_EIO;
	ಪಕ್ಷ RPMI_ERR_NO_DATA:
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ SBI_EUNKNOWN;
	}
}

ಪೂರ್ಣಾಙ್ಕ rpmi_normal_request_with_status(
			ಸಂರಚನಾ mbox_chan *chan, u32 service_id,
			ಶೂನ್ಯ *req, u32 req_ಪದಗಳು, u32 req_endian_ಪದಗಳು,
			ಶೂನ್ಯ *resp, u32 resp_ಪದಗಳು, u32 resp_endian_ಪದಗಳು)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ mbox_xfer xfer;
	ಸಂರಚನಾ rpmi_message_args args = { 0 };

	args.type = RPMI_ಸಂದೇಶ_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_ಪದಗಳು = req_endian_ಪದಗಳು;
	args.rx_endian_ಪದಗಳು = resp_endian_ಪದಗಳು;
	mbox_xfer_init_txrx(&xfer, &args,
			req, ಪರಿಮಾಣ(u32) * req_ಪದಗಳು, RPMI_DEF_TX_TIMEOUT,
			resp, ಪರಿಮಾಣ(u32) * resp_ಪದಗಳು, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ಪ್ರತ್ಯಾಗಮ rpmi_xlate_error(((u32 *)resp)[0]);
}

ಪೂರ್ಣಾಙ್ಕ rpmi_posted_request(
		ಸಂರಚನಾ mbox_chan *chan, u32 service_id,
		ಶೂನ್ಯ *req, u32 req_ಪದಗಳು, u32 req_endian_ಪದಗಳು)
{
	ಸಂರಚನಾ mbox_xfer xfer;
	ಸಂರಚನಾ rpmi_message_args args = { 0 };

	args.type = RPMI_ಸಂದೇಶ_POSTED_REQUEST;
	args.flags = RPMI_ಸಂದೇಶ_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_ಪದಗಳು = req_endian_ಪದಗಳು;
	mbox_xfer_init_tx(&xfer, &args,
			  req, ಪರಿಮಾಣ(u32) * req_ಪದಗಳು, RPMI_DEF_TX_TIMEOUT);

	ಪ್ರತ್ಯಾಗಮ mbox_chan_xfer(chan, &xfer);
}
