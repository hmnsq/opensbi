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

संरचना mbox_controller *mbox_controller_find(unsigned पूर्णाङ्क id)
{
	संरचना sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		संरचना mbox_controller *mbox = to_mbox_controller(pos);

		सचे (mbox->id == id)
			पटिनिवत्त mbox;
	}

	पटिनिवत्त रिक्त;
}

पूर्णाङ्क mbox_controller_add(संरचना mbox_controller *mbox)
{
	सचे (!mbox || !mbox->max_xfer_len)
		पटिनिवत्त SBI_EINVAL;
	सचे (mbox_controller_find(mbox->id))
		पटिनिवत्त SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->ग्रन्थिः, &mbox_list);

	पटिनिवत्त 0;
}

शून्य mbox_controller_remove(संरचना mbox_controller *mbox)
{
	संरचना mbox_chan *chan;

	सचे (!mbox)
		पटिनिवत्त;

	याव (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    संरचना mbox_chan, ग्रन्थिः);
		सचे (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->ग्रन्थिः);
	}

	sbi_list_del(&mbox->ग्रन्थिः);
}

संरचना mbox_chan *mbox_controller_request_chan(संरचना mbox_controller *mbox,
					       u32 *chan_args)
{
	संरचना mbox_chan *ret;
	संरचना sbi_dlist *pos;

	सचे (!chan_args || !mbox || !mbox->request_chan)
		पटिनिवत्त रिक्त;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		सचे (!sbi_memcmp(ret->chan_args, chan_args,
				परिमाण(ret->chan_args)))
			पटिनिवत्त ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	सचे (!ret)
		पटिनिवत्त रिक्त;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, परिमाण(ret->chan_args));
	sbi_list_add(&ret->ग्रन्थिः, &mbox->chan_list);
	पटिनिवत्त ret;
}

शून्य mbox_controller_free_chan(संरचना mbox_chan *chan)
{
	सचे (!chan || !chan->mbox)
		पटिनिवत्त;

	सचे (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->ग्रन्थिः);
}

पूर्णाङ्क mbox_chan_xfer(संरचना mbox_chan *chan, संरचना mbox_xfer *xfer)
{
	सचे (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		पटिनिवत्त SBI_EINVAL;

	सचे (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		पटिनिवत्त SBI_EINVAL;

	सचे (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		पटिनिवत्त SBI_EINVAL;

	सचे (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	पटिनिवत्त chan->mbox->xfer(chan, xfer);
}

पूर्णाङ्क mbox_chan_get_attribute(संरचना mbox_chan *chan, पूर्णाङ्क attr_id, शून्य *out_मानम्)
{
	सचे (!chan || !chan->mbox || !out_मानम्)
		पटिनिवत्त SBI_EINVAL;

	सचे (!chan->mbox->get_attribute)
		पटिनिवत्त SBI_ENOTSUPP;

	पटिनिवत्त chan->mbox->get_attribute(chan, attr_id, out_मानम्);
}

पूर्णाङ्क mbox_chan_set_attribute(संरचना mbox_chan *chan, पूर्णाङ्क attr_id, शून्य *new_मानम्)
{
	सचे (!chan || !chan->mbox || !new_मानम्)
		पटिनिवत्त SBI_EINVAL;

	सचे (!chan->mbox->set_attribute)
		पटिनिवत्त SBI_ENOTSUPP;

	पटिनिवत्त chan->mbox->set_attribute(chan, attr_id, new_मानम्);
}
