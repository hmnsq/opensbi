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

पूर्णांक rpmi_xlate_error(गणना rpmi_error error)
{
	निवड(error) {
	स्थिती RPMI_SUCCESS:
		परत SBI_OK;
	स्थिती RPMI_ERR_FAILED:
		परत SBI_EFAIL;
	स्थिती RPMI_ERR_NOTSUPP:
		परत SBI_ENOTSUPP;
	स्थिती RPMI_ERR_INVALID_PARAM:
		परत SBI_EINVAL;
	स्थिती RPMI_ERR_DENIED:
		परत SBI_EDENIED;
	स्थिती RPMI_ERR_INVALID_ADDR:
		परत SBI_EINVALID_ADDR;
	स्थिती RPMI_ERR_ALREADY:
		परत SBI_EALREADY;
	स्थिती RPMI_ERR_EXTENSION:
		परत SBI_EFAIL;
	स्थिती RPMI_ERR_HW_FAULT:
		परत SBI_EIO;
	स्थिती RPMI_ERR_BUSY:
		परत SBI_EFAIL;
	स्थिती RPMI_ERR_INVALID_STATE:
		परत SBI_EINVALID_STATE;
	स्थिती RPMI_ERR_BAD_RANGE:
		परत SBI_EBAD_RANGE;
	स्थिती RPMI_ERR_TIMEOUT:
		परत SBI_ETIMEDOUT;
	स्थिती RPMI_ERR_IO:
		परत SBI_EIO;
	स्थिती RPMI_ERR_NO_DATA:
		परत SBI_EFAIL;
	पूर्वनिर्धारित:
		परत SBI_EUNKNOWN;
	}
}

पूर्णांक rpmi_normal_request_with_status(
			संरचना mbox_chan *chan, u32 service_id,
			शून्य *req, u32 req_शब्द, u32 req_endian_शब्द,
			शून्य *resp, u32 resp_शब्द, u32 resp_endian_शब्द)
{
	पूर्णांक ret;
	संरचना mbox_xfer xfer;
	संरचना rpmi_message_args args = { 0 };

	args.type = RPMI_संदेश_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_शब्द = req_endian_शब्द;
	args.rx_endian_शब्द = resp_endian_शब्द;
	mbox_xfer_init_txrx(&xfer, &args,
			req, आकार(u32) * req_शब्द, RPMI_DEF_TX_TIMEOUT,
			resp, आकार(u32) * resp_शब्द, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	जर (ret)
		परत ret;

	परत rpmi_xlate_error(((u32 *)resp)[0]);
}

पूर्णांक rpmi_posted_request(
		संरचना mbox_chan *chan, u32 service_id,
		शून्य *req, u32 req_शब्द, u32 req_endian_शब्द)
{
	संरचना mbox_xfer xfer;
	संरचना rpmi_message_args args = { 0 };

	args.type = RPMI_संदेश_POSTED_REQUEST;
	args.flags = RPMI_संदेश_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_शब्द = req_endian_शब्द;
	mbox_xfer_init_tx(&xfer, &args,
			  req, आकार(u32) * req_शब्द, RPMI_DEF_TX_TIMEOUT);

	परत mbox_chan_xfer(chan, &xfer);
}
