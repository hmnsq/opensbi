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

صحيح rpmi_نورمال_request_with_حالت(
			بنية mbox_chan *chan, u32 service_id,
			فراغ *req, u32 req_ټکي, u32 req_endian_ټکي,
			فراغ *resp, u32 resp_ټکي, u32 resp_endian_ټکي)
{
	صحيح ret;
	بنية mbox_xfer xfer;
	بنية rpmi_پیغام_دلیلونه دلیلونه = { 0 };

	دلیلونه.type = RPMI_پیغام_نورمال_REQUEST;
	دلیلونه.service_id = service_id;
	دلیلونه.tx_endian_ټکي = req_endian_ټکي;
	دلیلونه.rx_endian_ټکي = resp_endian_ټکي;
	mbox_xfer_init_txrx(&xfer, &دلیلونه,
			req, حجم(u32) * req_ټکي, RPMI_DEF_TX_TIMEOUT,
			resp, حجم(u32) * resp_ټکي, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	إذا (ret)
		أرجع ret;

	أرجع rpmi_xlate_error(((u32 *)resp)[0]);
}

صحيح rpmi_posted_request(
		بنية mbox_chan *chan, u32 service_id,
		فراغ *req, u32 req_ټکي, u32 req_endian_ټکي)
{
	بنية mbox_xfer xfer;
	بنية rpmi_پیغام_دلیلونه دلیلونه = { 0 };

	دلیلونه.type = RPMI_پیغام_POSTED_REQUEST;
	دلیلونه.flags = RPMI_پیغام_FLAGS_NO_RX;
	دلیلونه.service_id = service_id;
	دلیلونه.tx_endian_ټکي = req_endian_ټکي;
	mbox_xfer_init_tx(&xfer, &دلیلونه,
			  req, حجم(u32) * req_ټکي, RPMI_DEF_TX_TIMEOUT);

	أرجع mbox_chan_xfer(chan, &xfer);
}
