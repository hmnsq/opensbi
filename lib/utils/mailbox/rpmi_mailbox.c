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
		प्रत्यागम SBI_OK;
	पक्ष RPMI_ERR_FAILED:
		प्रत्यागम SBI_EFAIL;
	पक्ष RPMI_ERR_NOTSUPP:
		प्रत्यागम SBI_ENOTSUPP;
	पक्ष RPMI_ERR_INVALID_PARAM:
		प्रत्यागम SBI_EINVAL;
	पक्ष RPMI_ERR_DENIED:
		प्रत्यागम SBI_EDENIED;
	पक्ष RPMI_ERR_INVALID_ADDR:
		प्रत्यागम SBI_EINVALID_ADDR;
	पक्ष RPMI_ERR_ALREADY:
		प्रत्यागम SBI_EALREADY;
	पक्ष RPMI_ERR_EXTENSION:
		प्रत्यागम SBI_EFAIL;
	पक्ष RPMI_ERR_HW_FAULT:
		प्रत्यागम SBI_EIO;
	पक्ष RPMI_ERR_BUSY:
		प्रत्यागम SBI_EFAIL;
	पक्ष RPMI_ERR_INVALID_STATE:
		प्रत्यागम SBI_EINVALID_STATE;
	पक्ष RPMI_ERR_BAD_RANGE:
		प्रत्यागम SBI_EBAD_RANGE;
	पक्ष RPMI_ERR_TIMEOUT:
		प्रत्यागम SBI_ETIMEDOUT;
	पक्ष RPMI_ERR_IO:
		प्रत्यागम SBI_EIO;
	पक्ष RPMI_ERR_NO_DATA:
		प्रत्यागम SBI_EFAIL;
	स्वभाव:
		प्रत्यागम SBI_EUNKNOWN;
	}
}

पूर्णाङ्क rpmi_normal_request_with_status(
			संरचना mbox_chan *chan, u32 service_id,
			शून्य *req, u32 req_शब्दगण, u32 req_endian_शब्दगण,
			शून्य *resp, u32 resp_शब्दगण, u32 resp_endian_शब्दगण)
{
	पूर्णाङ्क ret;
	संरचना mbox_xfer xfer;
	संरचना rpmi_message_args args = { 0 };

	args.type = RPMI_संदेश_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_शब्दगण = req_endian_शब्दगण;
	args.rx_endian_शब्दगण = resp_endian_शब्दगण;
	mbox_xfer_init_txrx(&xfer, &args,
			req, परिमाण(u32) * req_शब्दगण, RPMI_DEF_TX_TIMEOUT,
			resp, परिमाण(u32) * resp_शब्दगण, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	यदि (ret)
		प्रत्यागम ret;

	प्रत्यागम rpmi_xlate_error(((u32 *)resp)[0]);
}

पूर्णाङ्क rpmi_posted_request(
		संरचना mbox_chan *chan, u32 service_id,
		शून्य *req, u32 req_शब्दगण, u32 req_endian_शब्दगण)
{
	संरचना mbox_xfer xfer;
	संरचना rpmi_message_args args = { 0 };

	args.type = RPMI_संदेश_POSTED_REQUEST;
	args.flags = RPMI_संदेश_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_शब्दगण = req_endian_शब्दगण;
	mbox_xfer_init_tx(&xfer, &args,
			  req, परिमाण(u32) * req_शब्दगण, RPMI_DEF_TX_TIMEOUT);

	प्रत्यागम mbox_chan_xfer(chan, &xfer);
}
