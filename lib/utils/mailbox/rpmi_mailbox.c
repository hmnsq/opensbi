/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/mailbox/mailbox.h>
#समावेश <sbi_utils/mailbox/rpmi_mailbox.h>

पूर्णांक rpmi_xlate_error(क्रमागत rpmi_error error)
{
	चयन(error) {
	हाल RPMI_SUCCESS:
		वापस SBI_OK;
	हाल RPMI_ERR_FAILED:
		वापस SBI_EFAIL;
	हाल RPMI_ERR_NOTSUPP:
		वापस SBI_ENOTSUPP;
	हाल RPMI_ERR_INVALID_PARAM:
		वापस SBI_EINVAL;
	हाल RPMI_ERR_DENIED:
		वापस SBI_EDENIED;
	हाल RPMI_ERR_INVALID_ADDR:
		वापस SBI_EINVALID_ADDR;
	हाल RPMI_ERR_ALREADY:
		वापस SBI_EALREADY;
	हाल RPMI_ERR_EXTENSION:
		वापस SBI_EFAIL;
	हाल RPMI_ERR_HW_FAULT:
		वापस SBI_EIO;
	हाल RPMI_ERR_BUSY:
		वापस SBI_EFAIL;
	हाल RPMI_ERR_INVALID_STATE:
		वापस SBI_EINVALID_STATE;
	हाल RPMI_ERR_BAD_RANGE:
		वापस SBI_EBAD_RANGE;
	हाल RPMI_ERR_TIMEOUT:
		वापस SBI_ETIMEDOUT;
	हाल RPMI_ERR_IO:
		वापस SBI_EIO;
	हाल RPMI_ERR_NO_तथ्य:
		वापस SBI_EFAIL;
	शेष:
		वापस SBI_EUNKNOWN;
	}
}

पूर्णांक rpmi_normal_request_with_status(
			काष्ठा mbox_chan *chan, u32 service_id,
			व्योम *req, u32 req_शब्दगण, u32 req_endian_शब्दगण,
			व्योम *resp, u32 resp_शब्दगण, u32 resp_endian_शब्दगण)
{
	पूर्णांक ret;
	काष्ठा mbox_xfer xfer;
	काष्ठा rpmi_message_args args = { 0 };

	args.type = RPMI_संदेश_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_शब्दगण = req_endian_शब्दगण;
	args.rx_endian_शब्दगण = resp_endian_शब्दगण;
	mbox_xfer_init_txrx(&xfer, &args,
			req, माप(u32) * req_शब्दगण, RPMI_DEF_TX_TIMEOUT,
			resp, माप(u32) * resp_शब्दगण, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	अगर (ret)
		वापस ret;

	वापस rpmi_xlate_error(((u32 *)resp)[0]);
}

पूर्णांक rpmi_posted_request(
		काष्ठा mbox_chan *chan, u32 service_id,
		व्योम *req, u32 req_शब्दगण, u32 req_endian_शब्दगण)
{
	काष्ठा mbox_xfer xfer;
	काष्ठा rpmi_message_args args = { 0 };

	args.type = RPMI_संदेश_POSTED_REQUEST;
	args.flags = RPMI_संदेश_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_शब्दगण = req_endian_शब्दगण;
	mbox_xfer_init_tx(&xfer, &args,
			  req, माप(u32) * req_शब्दगण, RPMI_DEF_TX_TIMEOUT);

	वापस mbox_chan_xfer(chan, &xfer);
}
