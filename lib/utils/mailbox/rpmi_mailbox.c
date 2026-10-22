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
		फर्काऊ SBI_OK;
	पक्ष RPMI_ERR_FAILED:
		फर्काऊ SBI_EFAIL;
	पक्ष RPMI_ERR_NOTSUPP:
		फर्काऊ SBI_ENOTSUPP;
	पक्ष RPMI_ERR_INVALID_PARAM:
		फर्काऊ SBI_EINVAL;
	पक्ष RPMI_ERR_DENIED:
		फर्काऊ SBI_EDENIED;
	पक्ष RPMI_ERR_INVALID_ADDR:
		फर्काऊ SBI_EINVALID_ADDR;
	पक्ष RPMI_ERR_ALREADY:
		फर्काऊ SBI_EALREADY;
	पक्ष RPMI_ERR_EXTENSION:
		फर्काऊ SBI_EFAIL;
	पक्ष RPMI_ERR_HW_FAULT:
		फर्काऊ SBI_EIO;
	पक्ष RPMI_ERR_BUSY:
		फर्काऊ SBI_EFAIL;
	पक्ष RPMI_ERR_INVALID_STATE:
		फर्काऊ SBI_EINVALID_STATE;
	पक्ष RPMI_ERR_BAD_RANGE:
		फर्काऊ SBI_EBAD_RANGE;
	पक्ष RPMI_ERR_TIMEOUT:
		फर्काऊ SBI_ETIMEDOUT;
	पक्ष RPMI_ERR_IO:
		फर्काऊ SBI_EIO;
	पक्ष RPMI_ERR_NO_DATA:
		फर्काऊ SBI_EFAIL;
	स्वभाव:
		फर्काऊ SBI_EUNKNOWN;
	}
}

पूर्णाङ्क rpmi_normal_request_with_status(
			संरचना mbox_chan *chan, u32 service_id,
			शून्य *req, u32 req_शब्दहरू, u32 req_endian_शब्दहरू,
			शून्य *resp, u32 resp_शब्दहरू, u32 resp_endian_शब्दहरू)
{
	पूर्णाङ्क ret;
	संरचना mbox_xfer xfer;
	संरचना rpmi_message_args args = { 0 };

	args.type = RPMI_सन्देश_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_शब्दहरू = req_endian_शब्दहरू;
	args.rx_endian_शब्दहरू = resp_endian_शब्दहरू;
	mbox_xfer_init_txrx(&xfer, &args,
			req, परिमाण(u32) * req_शब्दहरू, RPMI_DEF_TX_TIMEOUT,
			resp, परिमाण(u32) * resp_शब्दहरू, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	यदि (ret)
		फर्काऊ ret;

	फर्काऊ rpmi_xlate_error(((u32 *)resp)[0]);
}

पूर्णाङ्क rpmi_posted_request(
		संरचना mbox_chan *chan, u32 service_id,
		शून्य *req, u32 req_शब्दहरू, u32 req_endian_शब्दहरू)
{
	संरचना mbox_xfer xfer;
	संरचना rpmi_message_args args = { 0 };

	args.type = RPMI_सन्देश_POSTED_REQUEST;
	args.flags = RPMI_सन्देश_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_शब्दहरू = req_endian_शब्दहरू;
	mbox_xfer_init_tx(&xfer, &args,
			  req, परिमाण(u32) * req_शब्दहरू, RPMI_DEF_TX_TIMEOUT);

	फर्काऊ mbox_chan_xfer(chan, &xfer);
}
