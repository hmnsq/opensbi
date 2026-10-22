/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/mailbox/mailbox.h>
#अन्तर्भाव <sbi_utils/mailbox/rpmi_mailbox.h>

पूर्णाङ्क rpmi_xlate_error(गणना rpmi_error error)
{
	विकल्प(error) {
	पक्ष RPMI_SUCCESS:
		पटिनिवत्त SBI_OK;
	पक्ष RPMI_ERR_FAILED:
		पटिनिवत्त SBI_EFAIL;
	पक्ष RPMI_ERR_NOTSUPP:
		पटिनिवत्त SBI_ENOTSUPP;
	पक्ष RPMI_ERR_INVALID_PARAM:
		पटिनिवत्त SBI_EINVAL;
	पक्ष RPMI_ERR_DENIED:
		पटिनिवत्त SBI_EDENIED;
	पक्ष RPMI_ERR_INVALID_ADDR:
		पटिनिवत्त SBI_EINVALID_ADDR;
	पक्ष RPMI_ERR_ALREADY:
		पटिनिवत्त SBI_EALREADY;
	पक्ष RPMI_ERR_EXTENSION:
		पटिनिवत्त SBI_EFAIL;
	पक्ष RPMI_ERR_HW_FAULT:
		पटिनिवत्त SBI_EIO;
	पक्ष RPMI_ERR_BUSY:
		पटिनिवत्त SBI_EFAIL;
	पक्ष RPMI_ERR_INVALID_STATE:
		पटिनिवत्त SBI_EINVALID_STATE;
	पक्ष RPMI_ERR_BAD_RANGE:
		पटिनिवत्त SBI_EBAD_RANGE;
	पक्ष RPMI_ERR_TIMEOUT:
		पटिनिवत्त SBI_ETIMEDOUT;
	पक्ष RPMI_ERR_IO:
		पटिनिवत्त SBI_EIO;
	पक्ष RPMI_ERR_NO_DATA:
		पटिनिवत्त SBI_EFAIL;
	स्वभाव:
		पटिनिवत्त SBI_EUNKNOWN;
	}
}

पूर्णाङ्क rpmi_normal_request_with_status(
			संरचना mbox_chan *chan, u32 service_id,
			शून्य *req, u32 req_शब्दाः, u32 req_endian_शब्दाः,
			शून्य *resp, u32 resp_शब्दाः, u32 resp_endian_शब्दाः)
{
	पूर्णाङ्क ret;
	संरचना mbox_xfer xfer;
	संरचना rpmi_message_args args = { 0 };

	args.type = RPMI_सन्देशः_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_शब्दाः = req_endian_शब्दाः;
	args.rx_endian_शब्दाः = resp_endian_शब्दाः;
	mbox_xfer_init_txrx(&xfer, &args,
			req, परिमाण(u32) * req_शब्दाः, RPMI_DEF_TX_TIMEOUT,
			resp, परिमाण(u32) * resp_शब्दाः, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	सचे (ret)
		पटिनिवत्त ret;

	पटिनिवत्त rpmi_xlate_error(((u32 *)resp)[0]);
}

पूर्णाङ्क rpmi_posted_request(
		संरचना mbox_chan *chan, u32 service_id,
		शून्य *req, u32 req_शब्दाः, u32 req_endian_शब्दाः)
{
	संरचना mbox_xfer xfer;
	संरचना rpmi_message_args args = { 0 };

	args.type = RPMI_सन्देशः_POSTED_REQUEST;
	args.flags = RPMI_सन्देशः_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_शब्दाः = req_endian_शब्दाः;
	mbox_xfer_init_tx(&xfer, &args,
			  req, परिमाण(u32) * req_शब्दाः, RPMI_DEF_TX_TIMEOUT);

	पटिनिवत्त mbox_chan_xfer(chan, &xfer);
}
