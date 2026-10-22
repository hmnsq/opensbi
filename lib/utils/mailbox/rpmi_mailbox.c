/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/mailbox/mailbox.h>
#ضمن <sbi_utils/mailbox/rpmi_mailbox.h>

صحيح rpmi_xlate_error(تعداد rpmi_error error)
{
	بدل(error) {
	حالة RPMI_SUCCESS:
		أرجع SBI_OK;
	حالة RPMI_ERR_FAILED:
		أرجع SBI_EFAIL;
	حالة RPMI_ERR_NOTSUPP:
		أرجع SBI_ENOTSUPP;
	حالة RPMI_ERR_INVALID_PARAM:
		أرجع SBI_EINVAL;
	حالة RPMI_ERR_DENIED:
		أرجع SBI_EDENIED;
	حالة RPMI_ERR_INVALID_ADDR:
		أرجع SBI_EINVALID_ADDR;
	حالة RPMI_ERR_ALREADY:
		أرجع SBI_EALREADY;
	حالة RPMI_ERR_EXTENSION:
		أرجع SBI_EFAIL;
	حالة RPMI_ERR_HW_FAULT:
		أرجع SBI_EIO;
	حالة RPMI_ERR_BUSY:
		أرجع SBI_EFAIL;
	حالة RPMI_ERR_INVALID_STATE:
		أرجع SBI_EINVALID_STATE;
	حالة RPMI_ERR_BAD_RANGE:
		أرجع SBI_EBAD_RANGE;
	حالة RPMI_ERR_TIMEOUT:
		أرجع SBI_ETIMEDOUT;
	حالة RPMI_ERR_IO:
		أرجع SBI_EIO;
	حالة RPMI_ERR_NO_معلومات:
		أرجع SBI_EFAIL;
	افتراضي:
		أرجع SBI_EUNKNOWN;
	}
}

صحيح rpmi_عام_request_with_حالت(
			بنية mbox_chan *chan, u32 service_id,
			فراغ *req, u32 req_لفظ, u32 req_endian_لفظ,
			فراغ *resp, u32 resp_لفظ, u32 resp_endian_لفظ)
{
	صحيح ret;
	بنية mbox_xfer xfer;
	بنية rpmi_پيغام_دليلون دليلون = { 0 };

	دليلون.type = RPMI_نياپو_عام_REQUEST;
	دليلون.service_id = service_id;
	دليلون.tx_endian_لفظ = req_endian_لفظ;
	دليلون.rx_endian_لفظ = resp_endian_لفظ;
	mbox_xfer_init_txrx(&xfer, &دليلون,
			req, حجم(u32) * req_لفظ, RPMI_DEF_TX_TIMEOUT,
			resp, حجم(u32) * resp_لفظ, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	إذا (ret)
		أرجع ret;

	أرجع rpmi_xlate_error(((u32 *)resp)[0]);
}

صحيح rpmi_posted_request(
		بنية mbox_chan *chan, u32 service_id,
		فراغ *req, u32 req_لفظ, u32 req_endian_لفظ)
{
	بنية mbox_xfer xfer;
	بنية rpmi_پيغام_دليلون دليلون = { 0 };

	دليلون.type = RPMI_نياپو_POSTED_REQUEST;
	دليلون.flags = RPMI_نياپو_FLAGS_NO_RX;
	دليلون.service_id = service_id;
	دليلون.tx_endian_لفظ = req_endian_لفظ;
	mbox_xfer_init_tx(&xfer, &دليلون,
			  req, حجم(u32) * req_لفظ, RPMI_DEF_TX_TIMEOUT);

	أرجع mbox_chan_xfer(chan, &xfer);
}
