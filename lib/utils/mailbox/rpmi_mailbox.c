/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/mailbox.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/rpmi_mailbox.h>

ਪੂਰਨ rpmi_xlate_error(ਗਿਣਤੀ rpmi_error error)
{
	ਚੋਣ(error) {
	ਹਾਲਤ RPMI_SUCCESS:
		ਵਾਪਸ SBI_OK;
	ਹਾਲਤ RPMI_ERR_FAILED:
		ਵਾਪਸ SBI_EFAIL;
	ਹਾਲਤ RPMI_ERR_NOTSUPP:
		ਵਾਪਸ SBI_ENOTSUPP;
	ਹਾਲਤ RPMI_ERR_INVALID_PARAM:
		ਵਾਪਸ SBI_EINVAL;
	ਹਾਲਤ RPMI_ERR_DENIED:
		ਵਾਪਸ SBI_EDENIED;
	ਹਾਲਤ RPMI_ERR_INVALID_ADDR:
		ਵਾਪਸ SBI_EINVALID_ADDR;
	ਹਾਲਤ RPMI_ERR_ALREADY:
		ਵਾਪਸ SBI_EALREADY;
	ਹਾਲਤ RPMI_ERR_EXTENSION:
		ਵਾਪਸ SBI_EFAIL;
	ਹਾਲਤ RPMI_ERR_HW_FAULT:
		ਵਾਪਸ SBI_EIO;
	ਹਾਲਤ RPMI_ERR_BUSY:
		ਵਾਪਸ SBI_EFAIL;
	ਹਾਲਤ RPMI_ERR_INVALID_STATE:
		ਵਾਪਸ SBI_EINVALID_STATE;
	ਹਾਲਤ RPMI_ERR_BAD_RANGE:
		ਵਾਪਸ SBI_EBAD_RANGE;
	ਹਾਲਤ RPMI_ERR_TIMEOUT:
		ਵਾਪਸ SBI_ETIMEDOUT;
	ਹਾਲਤ RPMI_ERR_IO:
		ਵਾਪਸ SBI_EIO;
	ਹਾਲਤ RPMI_ERR_NO_ਅੰਕੜੇ:
		ਵਾਪਸ SBI_EFAIL;
	ਮੂਲ:
		ਵਾਪਸ SBI_EUNKNOWN;
	}
}

ਪੂਰਨ rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			ਬਣਤਰ mbox_chan *chan, u32 service_id,
			ਖਾਲੀ *req, u32 req_ਸ਼ਬਦ, u32 req_endian_ਸ਼ਬਦ,
			ਖਾਲੀ *resp, u32 resp_ਸ਼ਬਦ, u32 resp_endian_ਸ਼ਬਦ)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ mbox_xfer xfer;
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ_ਦਲੀਲਾਂ ਦਲੀਲਾਂ = { 0 };

	ਦਲੀਲਾਂ.type = RPMI_ਸੁਨੇਹਾ_ਸਾਧਾਰਨ_REQUEST;
	ਦਲੀਲਾਂ.service_id = service_id;
	ਦਲੀਲਾਂ.tx_endian_ਸ਼ਬਦ = req_endian_ਸ਼ਬਦ;
	ਦਲੀਲਾਂ.rx_endian_ਸ਼ਬਦ = resp_endian_ਸ਼ਬਦ;
	mbox_xfer_init_txrx(&xfer, &ਦਲੀਲਾਂ,
			req, ਆਕਾਰ(u32) * req_ਸ਼ਬਦ, RPMI_DEF_TX_TIMEOUT,
			resp, ਆਕਾਰ(u32) * resp_ਸ਼ਬਦ, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ਵਾਪਸ rpmi_xlate_error(((u32 *)resp)[0]);
}

ਪੂਰਨ rpmi_posted_request(
		ਬਣਤਰ mbox_chan *chan, u32 service_id,
		ਖਾਲੀ *req, u32 req_ਸ਼ਬਦ, u32 req_endian_ਸ਼ਬਦ)
{
	ਬਣਤਰ mbox_xfer xfer;
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ_ਦਲੀਲਾਂ ਦਲੀਲਾਂ = { 0 };

	ਦਲੀਲਾਂ.type = RPMI_ਸੁਨੇਹਾ_POSTED_REQUEST;
	ਦਲੀਲਾਂ.flags = RPMI_ਸੁਨੇਹਾ_FLAGS_NO_RX;
	ਦਲੀਲਾਂ.service_id = service_id;
	ਦਲੀਲਾਂ.tx_endian_ਸ਼ਬਦ = req_endian_ਸ਼ਬਦ;
	mbox_xfer_init_tx(&xfer, &ਦਲੀਲਾਂ,
			  req, ਆਕਾਰ(u32) * req_ਸ਼ਬਦ, RPMI_DEF_TX_TIMEOUT);

	ਵਾਪਸ mbox_chan_xfer(chan, &xfer);
}
