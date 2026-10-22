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

sa_nracanaa mbox_controller *mbox_controller_find(unsigned ཧྲིལ id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		sa_nracanaa mbox_controller *mbox = to_mbox_controller(pos);

		གལ་ཏེ (mbox->id == id)
			ཕྱིར mbox;
	}

	ཕྱིར NULL;
}

ཧྲིལ mbox_controller_add(sa_nracanaa mbox_controller *mbox)
{
	གལ་ཏེ (!mbox || !mbox->max_xfer_len)
		ཕྱིར SBI_EINVAL;
	གལ་ཏེ (mbox_controller_find(mbox->id))
		ཕྱིར SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	ཕྱིར 0;
}

void mbox_controller_remove(sa_nracanaa mbox_controller *mbox)
{
	sa_nracanaa mbox_chan *chan;

	གལ་ཏེ (!mbox)
		ཕྱིར;

	སྐབས (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    sa_nracanaa mbox_chan, node);
		གལ་ཏེ (mbox->free_chan)
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

	གལ་ཏེ (!chan_args || !mbox || !mbox->request_chan)
		ཕྱིར NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		གལ་ཏེ (!sbi_memcmp(ret->chan_args, chan_args,
				parimaa_na(ret->chan_args)))
			ཕྱིར ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	གལ་ཏེ (!ret)
		ཕྱིར NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, parimaa_na(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	ཕྱིར ret;
}

void mbox_controller_free_chan(sa_nracanaa mbox_chan *chan)
{
	གལ་ཏེ (!chan || !chan->mbox)
		ཕྱིར;

	གལ་ཏེ (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

ཧྲིལ mbox_chan_xfer(sa_nracanaa mbox_chan *chan, sa_nracanaa mbox_xfer *xfer)
{
	གལ་ཏེ (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		ཕྱིར SBI_EINVAL;

	གལ་ཏེ (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		ཕྱིར SBI_EINVAL;

	གལ་ཏེ (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		ཕྱིར SBI_EINVAL;

	གལ་ཏེ (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	ཕྱིར chan->mbox->xfer(chan, xfer);
}

ཧྲིལ mbox_chan_get_attribute(sa_nracanaa mbox_chan *chan, ཧྲིལ attr_id, void *out_value)
{
	གལ་ཏེ (!chan || !chan->mbox || !out_value)
		ཕྱིར SBI_EINVAL;

	གལ་ཏེ (!chan->mbox->get_attribute)
		ཕྱིར SBI_ENOTSUPP;

	ཕྱིར chan->mbox->get_attribute(chan, attr_id, out_value);
}

ཧྲིལ mbox_chan_set_attribute(sa_nracanaa mbox_chan *chan, ཧྲིལ attr_id, void *new_value)
{
	གལ་ཏེ (!chan || !chan->mbox || !new_value)
		ཕྱིར SBI_EINVAL;

	གལ་ཏེ (!chan->mbox->set_attribute)
		ཕྱིར SBI_ENOTSUPP;

	ཕྱིར chan->mbox->set_attribute(chan, attr_id, new_value);
}
