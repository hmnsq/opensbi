/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <sbi/sbi_error.h>
#include <sbi/sbi_string.h>
#include <sbi_utils/mailbox/mailbox.h>

static SBI_LIST_HEAD(mbox_list);

sa_nracanaa mbox_controller *mbox_controller_find(unsigned පූර්ණ_සංඛ්‍යා id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		sa_nracanaa mbox_controller *mbox = to_mbox_controller(pos);

		නම් (mbox->id == id)
			ආපසු mbox;
	}

	ආපසු NULL;
}

පූර්ණ_සංඛ්‍යා mbox_controller_add(sa_nracanaa mbox_controller *mbox)
{
	නම් (!mbox || !mbox->max_xfer_len)
		ආපසු SBI_EINVAL;
	නම් (mbox_controller_find(mbox->id))
		ආපසු SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	ආපසු 0;
}

void mbox_controller_remove(sa_nracanaa mbox_controller *mbox)
{
	sa_nracanaa mbox_chan *chan;

	නම් (!mbox)
		ආපසු;

	අතරතුර (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    sa_nracanaa mbox_chan, node);
		නම් (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->node);
	}

	sbi_list_del(&mbox->node);
}

sa_nracanaa mbox_chan *mbox_controller_request_chan(sa_nracanaa mbox_controller *mbox,
					       u32 *chan_args)
{
	sa_nracanaa mbox_chan *ret;
	sa_nracanaa sbi_dlist *pos;

	නම් (!chan_args || !mbox || !mbox->request_chan)
		ආපසු NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		නම් (!sbi_memcmp(ret->chan_args, chan_args,
				parimaa_na(ret->chan_args)))
			ආපසු ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	නම් (!ret)
		ආපසු NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, parimaa_na(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	ආපසු ret;
}

void mbox_controller_free_chan(sa_nracanaa mbox_chan *chan)
{
	නම් (!chan || !chan->mbox)
		ආපසු;

	නම් (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

පූර්ණ_සංඛ්‍යා mbox_chan_xfer(sa_nracanaa mbox_chan *chan, sa_nracanaa mbox_xfer *xfer)
{
	නම් (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		ආපසු SBI_EINVAL;

	නම් (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		ආපසු SBI_EINVAL;

	නම් (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		ආපසු SBI_EINVAL;

	නම් (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	ආපසු chan->mbox->xfer(chan, xfer);
}

පූර්ණ_සංඛ්‍යා mbox_chan_get_attribute(sa_nracanaa mbox_chan *chan, පූර්ණ_සංඛ්‍යා attr_id, void *out_value)
{
	නම් (!chan || !chan->mbox || !out_value)
		ආපසු SBI_EINVAL;

	නම් (!chan->mbox->get_attribute)
		ආපසු SBI_ENOTSUPP;

	ආපසු chan->mbox->get_attribute(chan, attr_id, out_value);
}

පූර්ණ_සංඛ්‍යා mbox_chan_set_attribute(sa_nracanaa mbox_chan *chan, පූර්ණ_සංඛ්‍යා attr_id, void *new_value)
{
	නම් (!chan || !chan->mbox || !new_value)
		ආපසු SBI_EINVAL;

	නම් (!chan->mbox->set_attribute)
		ආපසු SBI_ENOTSUPP;

	ආපසු chan->mbox->set_attribute(chan, attr_id, new_value);
}
