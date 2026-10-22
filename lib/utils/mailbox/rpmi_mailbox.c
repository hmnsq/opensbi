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

ಪೂರ್ಣಾಂಕ rpmi_xlate_error(ಎಣಿಕೆ rpmi_error error)
{
	ಆಯ್ಕೆ(error) {
	ಸ್ಥಿತಿ RPMI_SUCCESS:
		ಹಿಂತಿರುಗಿ SBI_OK;
	ಸ್ಥಿತಿ RPMI_ERR_FAILED:
		ಹಿಂತಿರುಗಿ SBI_EFAIL;
	ಸ್ಥಿತಿ RPMI_ERR_NOTSUPP:
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
	ಸ್ಥಿತಿ RPMI_ERR_INVALID_PARAM:
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಸ್ಥಿತಿ RPMI_ERR_DENIED:
		ಹಿಂತಿರುಗಿ SBI_EDENIED;
	ಸ್ಥಿತಿ RPMI_ERR_INVALID_ADDR:
		ಹಿಂತಿರುಗಿ SBI_EINVALID_ADDR;
	ಸ್ಥಿತಿ RPMI_ERR_ALREADY:
		ಹಿಂತಿರುಗಿ SBI_EALREADY;
	ಸ್ಥಿತಿ RPMI_ERR_EXTENSION:
		ಹಿಂತಿರುಗಿ SBI_EFAIL;
	ಸ್ಥಿತಿ RPMI_ERR_HW_FAULT:
		ಹಿಂತಿರುಗಿ SBI_EIO;
	ಸ್ಥಿತಿ RPMI_ERR_BUSY:
		ಹಿಂತಿರುಗಿ SBI_EFAIL;
	ಸ್ಥಿತಿ RPMI_ERR_INVALID_STATE:
		ಹಿಂತಿರುಗಿ SBI_EINVALID_STATE;
	ಸ್ಥಿತಿ RPMI_ERR_BAD_RANGE:
		ಹಿಂತಿರುಗಿ SBI_EBAD_RANGE;
	ಸ್ಥಿತಿ RPMI_ERR_TIMEOUT:
		ಹಿಂತಿರುಗಿ SBI_ETIMEDOUT;
	ಸ್ಥಿತಿ RPMI_ERR_IO:
		ಹಿಂತಿರುಗಿ SBI_EIO;
	ಸ್ಥಿತಿ RPMI_ERR_NO_ದತ್ತಾಂಶ:
		ಹಿಂತಿರುಗಿ SBI_EFAIL;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ SBI_EUNKNOWN;
	}
}

ಪೂರ್ಣಾಂಕ rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			ರಚನೆ mbox_chan *chan, u32 service_id,
			ಶೂನ್ಯ *req, u32 req_ಪದಗಳು, u32 req_endian_ಪದಗಳು,
			ಶೂನ್ಯ *resp, u32 resp_ಪದಗಳು, u32 resp_endian_ಪದಗಳು)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ mbox_xfer xfer;
	ರಚನೆ rpmi_ಸಂದೇಶ_ವಾದಗಳು ವಾದಗಳು = { 0 };

	ವಾದಗಳು.type = RPMI_ಸಂದೇಶ_ಸಾಮಾನ್ಯ_REQUEST;
	ವಾದಗಳು.service_id = service_id;
	ವಾದಗಳು.tx_endian_ಪದಗಳು = req_endian_ಪದಗಳು;
	ವಾದಗಳು.rx_endian_ಪದಗಳು = resp_endian_ಪದಗಳು;
	mbox_xfer_init_txrx(&xfer, &ವಾದಗಳು,
			req, ಗಾತ್ರ(u32) * req_ಪದಗಳು, RPMI_DEF_TX_TIMEOUT,
			resp, ಗಾತ್ರ(u32) * resp_ಪದಗಳು, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ಹಿಂತಿರುಗಿ rpmi_xlate_error(((u32 *)resp)[0]);
}

ಪೂರ್ಣಾಂಕ rpmi_posted_request(
		ರಚನೆ mbox_chan *chan, u32 service_id,
		ಶೂನ್ಯ *req, u32 req_ಪದಗಳು, u32 req_endian_ಪದಗಳು)
{
	ರಚನೆ mbox_xfer xfer;
	ರಚನೆ rpmi_ಸಂದೇಶ_ವಾದಗಳು ವಾದಗಳು = { 0 };

	ವಾದಗಳು.type = RPMI_ಸಂದೇಶ_POSTED_REQUEST;
	ವಾದಗಳು.flags = RPMI_ಸಂದೇಶ_FLAGS_NO_RX;
	ವಾದಗಳು.service_id = service_id;
	ವಾದಗಳು.tx_endian_ಪದಗಳು = req_endian_ಪದಗಳು;
	mbox_xfer_init_tx(&xfer, &ವಾದಗಳು,
			  req, ಗಾತ್ರ(u32) * req_ಪದಗಳು, RPMI_DEF_TX_TIMEOUT);

	ಹಿಂತಿರುಗಿ mbox_chan_xfer(chan, &xfer);
}
