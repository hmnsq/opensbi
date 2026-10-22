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

		གལ་སྲིད (mbox->id == id)
			ལོག mbox;
	}

	ལོག NULL;
}

ཧྲིལ mbox_controller_add(sa_nracanaa mbox_controller *mbox)
{
	གལ་སྲིད (!mbox || !mbox->max_xfer_len)
		ལོག SBI_EINVAL;
	གལ་སྲིད (mbox_controller_find(mbox->id))
		ལོག SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	ལོག 0;
}

void mbox_controller_remove(sa_nracanaa mbox_controller *mbox)
{
	sa_nracanaa mbox_chan *chan;

	གལ་སྲིད (!mbox)
		ལོག;

	སྐབས (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    sa_nracanaa mbox_chan, node);
		གལ་སྲིད (mbox->free_chan)
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

	གལ་སྲིད (!chan_args || !mbox || !mbox->request_chan)
		ལོག NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		གལ་སྲིད (!sbi_memcmp(ret->chan_args, chan_args,
				parimaa_na(ret->chan_args)))
			ལོག ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	གལ་སྲིད (!ret)
		ལོག NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, parimaa_na(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	ལོག ret;
}

void mbox_controller_free_chan(sa_nracanaa mbox_chan *chan)
{
	གལ་སྲིད (!chan || !chan->mbox)
		ལོག;

	གལ་སྲིད (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

ཧྲིལ mbox_chan_xfer(sa_nracanaa mbox_chan *chan, sa_nracanaa mbox_xfer *xfer)
{
	གལ་སྲིད (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		ལོག SBI_EINVAL;

	གལ་སྲིད (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		ལོག SBI_EINVAL;

	གལ་སྲིད (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		ལོག SBI_EINVAL;

	གལ་སྲིད (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	ལོག chan->mbox->xfer(chan, xfer);
}

ཧྲིལ mbox_chan_get_attribute(sa_nracanaa mbox_chan *chan, ཧྲིལ attr_id, void *out_value)
{
	གལ་སྲིད (!chan || !chan->mbox || !out_value)
		ལོག SBI_EINVAL;

	གལ་སྲིད (!chan->mbox->get_attribute)
		ལོག SBI_ENOTSUPP;

	ལོག chan->mbox->get_attribute(chan, attr_id, out_value);
}

ཧྲིལ mbox_chan_set_attribute(sa_nracanaa mbox_chan *chan, ཧྲིལ attr_id, void *new_value)
{
	གལ་སྲིད (!chan || !chan->mbox || !new_value)
		ལོག SBI_EINVAL;

	གལ་སྲིད (!chan->mbox->set_attribute)
		ལོག SBI_ENOTSUPP;

	ལོག chan->mbox->set_attribute(chan, attr_id, new_value);
}
