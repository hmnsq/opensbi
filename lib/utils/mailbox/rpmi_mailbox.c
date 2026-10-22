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
	حالة RPMI_ERR_NO_داده:
		أرجع SBI_EFAIL;
	افتراضي:
		أرجع SBI_EUNKNOWN;
	}
}

صحيح rpmi_نرمال_request_with_وضعیت(
			بنية mbox_chan *chan, u32 service_id,
			فراغ *req, u32 req_واژگان, u32 req_endian_واژگان,
			فراغ *resp, u32 resp_واژگان, u32 resp_endian_واژگان)
{
	صحيح ret;
	بنية mbox_xfer xfer;
	بنية rpmi_پیام_آرگومانها آرگومانها = { 0 };

	آرگومانها.type = RPMI_پیام_نرمال_REQUEST;
	آرگومانها.service_id = service_id;
	آرگومانها.tx_endian_واژگان = req_endian_واژگان;
	آرگومانها.rx_endian_واژگان = resp_endian_واژگان;
	mbox_xfer_init_txrx(&xfer, &آرگومانها,
			req, حجم(u32) * req_واژگان, RPMI_DEF_TX_TIMEOUT,
			resp, حجم(u32) * resp_واژگان, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	إذا (ret)
		أرجع ret;

	أرجع rpmi_xlate_error(((u32 *)resp)[0]);
}

صحيح rpmi_posted_request(
		بنية mbox_chan *chan, u32 service_id,
		فراغ *req, u32 req_واژگان, u32 req_endian_واژگان)
{
	بنية mbox_xfer xfer;
	بنية rpmi_پیام_آرگومانها آرگومانها = { 0 };

	آرگومانها.type = RPMI_پیام_POSTED_REQUEST;
	آرگومانها.flags = RPMI_پیام_FLAGS_NO_RX;
	آرگومانها.service_id = service_id;
	آرگومانها.tx_endian_واژگان = req_endian_واژگان;
	mbox_xfer_init_tx(&xfer, &آرگومانها,
			  req, حجم(u32) * req_واژگان, RPMI_DEF_TX_TIMEOUT);

	أرجع mbox_chan_xfer(chan, &xfer);
}
