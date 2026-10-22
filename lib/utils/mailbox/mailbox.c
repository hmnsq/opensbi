/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi_utils/mailbox/mailbox.h>

स्थिर SBI_LIST_HEAD(mbox_list);

संरचना mbox_controller *mbox_controller_find(अचिन्हित पूर्णांक id)
{
	संरचना sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		संरचना mbox_controller *mbox = to_mbox_controller(pos);

		जर (mbox->id == id)
			परत mbox;
	}

	परत रिक्त;
}

पूर्णांक mbox_controller_add(संरचना mbox_controller *mbox)
{
	जर (!mbox || !mbox->max_xfer_len)
		परत SBI_EINVAL;
	जर (mbox_controller_find(mbox->id))
		परत SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->गाठ, &mbox_list);

	परत 0;
}

शून्य mbox_controller_remove(संरचना mbox_controller *mbox)
{
	संरचना mbox_chan *chan;

	जर (!mbox)
		परत;

	पर्यंत (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    संरचना mbox_chan, गाठ);
		जर (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->गाठ);
	}

	sbi_list_del(&mbox->गाठ);
}

संरचना mbox_chan *mbox_controller_request_chan(संरचना mbox_controller *mbox,
					       u32 *chan_args)
{
	संरचना mbox_chan *ret;
	संरचना sbi_dlist *pos;

	जर (!chan_args || !mbox || !mbox->request_chan)
		परत रिक्त;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		जर (!sbi_memcmp(ret->chan_args, chan_args,
				आकार(ret->chan_args)))
			परत ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	जर (!ret)
		परत रिक्त;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, आकार(ret->chan_args));
	sbi_list_add(&ret->गाठ, &mbox->chan_list);
	परत ret;
}

शून्य mbox_controller_free_chan(संरचना mbox_chan *chan)
{
	जर (!chan || !chan->mbox)
		परत;

	जर (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->गाठ);
}

पूर्णांक mbox_chan_xfer(संरचना mbox_chan *chan, संरचना mbox_xfer *xfer)
{
	जर (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		परत SBI_EINVAL;

	जर (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		परत SBI_EINVAL;

	जर (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		परत SBI_EINVAL;

	जर (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	परत chan->mbox->xfer(chan, xfer);
}

पूर्णांक mbox_chan_get_attribute(संरचना mbox_chan *chan, पूर्णांक attr_id, शून्य *out_मूल्य)
{
	जर (!chan || !chan->mbox || !out_मूल्य)
		परत SBI_EINVAL;

	जर (!chan->mbox->get_attribute)
		परत SBI_ENOTSUPP;

	परत chan->mbox->get_attribute(chan, attr_id, out_मूल्य);
}

पूर्णांक mbox_chan_set_attribute(संरचना mbox_chan *chan, पूर्णांक attr_id, शून्य *new_मूल्य)
{
	जर (!chan || !chan->mbox || !new_मूल्य)
		परत SBI_EINVAL;

	जर (!chan->mbox->set_attribute)
		परत SBI_ENOTSUPP;

	परत chan->mbox->set_attribute(chan, attr_id, new_मूल्य);
}
