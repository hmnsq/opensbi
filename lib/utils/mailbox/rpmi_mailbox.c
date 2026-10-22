/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/mailbox.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/rpmi_mailbox.h>

പൂർണ്ണസംഖ്യ rpmi_xlate_error(എണ്ണൽ rpmi_error error)
{
	തിരഞ്ഞെടുപ്പ്(error) {
	അവസ്ഥ RPMI_SUCCESS:
		തിരികെ SBI_OK;
	അവസ്ഥ RPMI_ERR_FAILED:
		തിരികെ SBI_EFAIL;
	അവസ്ഥ RPMI_ERR_NOTSUPP:
		തിരികെ SBI_ENOTSUPP;
	അവസ്ഥ RPMI_ERR_INVALID_PARAM:
		തിരികെ SBI_EINVAL;
	അവസ്ഥ RPMI_ERR_DENIED:
		തിരികെ SBI_EDENIED;
	അവസ്ഥ RPMI_ERR_INVALID_ADDR:
		തിരികെ SBI_EINVALID_ADDR;
	അവസ്ഥ RPMI_ERR_ALREADY:
		തിരികെ SBI_EALREADY;
	അവസ്ഥ RPMI_ERR_EXTENSION:
		തിരികെ SBI_EFAIL;
	അവസ്ഥ RPMI_ERR_HW_FAULT:
		തിരികെ SBI_EIO;
	അവസ്ഥ RPMI_ERR_BUSY:
		തിരികെ SBI_EFAIL;
	അവസ്ഥ RPMI_ERR_INVALID_STATE:
		തിരികെ SBI_EINVALID_STATE;
	അവസ്ഥ RPMI_ERR_BAD_RANGE:
		തിരികെ SBI_EBAD_RANGE;
	അവസ്ഥ RPMI_ERR_TIMEOUT:
		തിരികെ SBI_ETIMEDOUT;
	അവസ്ഥ RPMI_ERR_IO:
		തിരികെ SBI_EIO;
	അവസ്ഥ RPMI_ERR_NO_DATA:
		തിരികെ SBI_EFAIL;
	സ്ഥിരസ്ഥിതി:
		തിരികെ SBI_EUNKNOWN;
	}
}

പൂർണ്ണസംഖ്യ rpmi_normal_request_with_status(
			ഘടന mbox_chan *chan, u32 service_id,
			ശൂന്യം *req, u32 req_വാക്കുകൾ, u32 req_endian_വാക്കുകൾ,
			ശൂന്യം *resp, u32 resp_വാക്കുകൾ, u32 resp_endian_വാക്കുകൾ)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന mbox_xfer xfer;
	ഘടന rpmi_message_args args = { 0 };

	args.type = RPMI_സന്ദേശം_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_വാക്കുകൾ = req_endian_വാക്കുകൾ;
	args.rx_endian_വാക്കുകൾ = resp_endian_വാക്കുകൾ;
	mbox_xfer_init_txrx(&xfer, &args,
			req, വലുപ്പം(u32) * req_വാക്കുകൾ, RPMI_DEF_TX_TIMEOUT,
			resp, വലുപ്പം(u32) * resp_വാക്കുകൾ, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	എങ്കിൽ (ret)
		തിരികെ ret;

	തിരികെ rpmi_xlate_error(((u32 *)resp)[0]);
}

പൂർണ്ണസംഖ്യ rpmi_posted_request(
		ഘടന mbox_chan *chan, u32 service_id,
		ശൂന്യം *req, u32 req_വാക്കുകൾ, u32 req_endian_വാക്കുകൾ)
{
	ഘടന mbox_xfer xfer;
	ഘടന rpmi_message_args args = { 0 };

	args.type = RPMI_സന്ദേശം_POSTED_REQUEST;
	args.flags = RPMI_സന്ദേശം_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_വാക്കുകൾ = req_endian_വാക്കുകൾ;
	mbox_xfer_init_tx(&xfer, &args,
			  req, വലുപ്പം(u32) * req_വാക്കുകൾ, RPMI_DEF_TX_TIMEOUT);

	തിരികെ mbox_chan_xfer(chan, &xfer);
}
