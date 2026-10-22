/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/mailbox/mailbox.h>
#అన్తర్భావ <sbi_utils/mailbox/rpmi_mailbox.h>

పూర్ణాంకం rpmi_xlate_error(గణన rpmi_error error)
{
	ఎంపిక(error) {
	స్థితి RPMI_SUCCESS:
		తిరిగి SBI_OK;
	స్థితి RPMI_ERR_FAILED:
		తిరిగి SBI_EFAIL;
	స్థితి RPMI_ERR_NOTSUPP:
		తిరిగి SBI_ENOTSUPP;
	స్థితి RPMI_ERR_INVALID_PARAM:
		తిరిగి SBI_EINVAL;
	స్థితి RPMI_ERR_DENIED:
		తిరిగి SBI_EDENIED;
	స్థితి RPMI_ERR_INVALID_ADDR:
		తిరిగి SBI_EINVALID_ADDR;
	స్థితి RPMI_ERR_ALREADY:
		తిరిగి SBI_EALREADY;
	స్థితి RPMI_ERR_EXTENSION:
		తిరిగి SBI_EFAIL;
	స్థితి RPMI_ERR_HW_FAULT:
		తిరిగి SBI_EIO;
	స్థితి RPMI_ERR_BUSY:
		తిరిగి SBI_EFAIL;
	స్థితి RPMI_ERR_INVALID_STATE:
		తిరిగి SBI_EINVALID_STATE;
	స్థితి RPMI_ERR_BAD_RANGE:
		తిరిగి SBI_EBAD_RANGE;
	స్థితి RPMI_ERR_TIMEOUT:
		తిరిగి SBI_ETIMEDOUT;
	స్థితి RPMI_ERR_IO:
		తిరిగి SBI_EIO;
	స్థితి RPMI_ERR_NO_దత్తాంశం:
		తిరిగి SBI_EFAIL;
	అప్రమేయ:
		తిరిగి SBI_EUNKNOWN;
	}
}

పూర్ణాంకం rpmi_సాధారణ_request_with_స్థితి(
			నిర్మాణం mbox_chan *chan, u32 service_id,
			శూన్య *req, u32 req_పదాలు, u32 req_endian_పదాలు,
			శూన్య *resp, u32 resp_పదాలు, u32 resp_endian_పదాలు)
{
	పూర్ణాంకం ret;
	నిర్మాణం mbox_xfer xfer;
	నిర్మాణం rpmi_సందేశం_వాదనలు వాదనలు = { 0 };

	వాదనలు.type = RPMI_సందేశం_సాధారణ_REQUEST;
	వాదనలు.service_id = service_id;
	వాదనలు.tx_endian_పదాలు = req_endian_పదాలు;
	వాదనలు.rx_endian_పదాలు = resp_endian_పదాలు;
	mbox_xfer_init_txrx(&xfer, &వాదనలు,
			req, పరిమాణం(u32) * req_పదాలు, RPMI_DEF_TX_TIMEOUT,
			resp, పరిమాణం(u32) * resp_పదాలు, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	ఒకవేళ (ret)
		తిరిగి ret;

	తిరిగి rpmi_xlate_error(((u32 *)resp)[0]);
}

పూర్ణాంకం rpmi_posted_request(
		నిర్మాణం mbox_chan *chan, u32 service_id,
		శూన్య *req, u32 req_పదాలు, u32 req_endian_పదాలు)
{
	నిర్మాణం mbox_xfer xfer;
	నిర్మాణం rpmi_సందేశం_వాదనలు వాదనలు = { 0 };

	వాదనలు.type = RPMI_సందేశం_POSTED_REQUEST;
	వాదనలు.flags = RPMI_సందేశం_FLAGS_NO_RX;
	వాదనలు.service_id = service_id;
	వాదనలు.tx_endian_పదాలు = req_endian_పదాలు;
	mbox_xfer_init_tx(&xfer, &వాదనలు,
			  req, పరిమాణం(u32) * req_పదాలు, RPMI_DEF_TX_TIMEOUT);

	తిరిగి mbox_chan_xfer(chan, &xfer);
}
