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

struct mbox_controller *mbox_controller_find(unsigned цео id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		struct mbox_controller *mbox = to_mbox_controller(pos);

		ако (mbox->id == id)
			врати mbox;
	}

	врати NULL;
}

цео mbox_controller_add(struct mbox_controller *mbox)
{
	ако (!mbox || !mbox->max_xfer_len)
		врати SBI_EINVAL;
	ако (mbox_controller_find(mbox->id))
		врати SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	врати 0;
}

void mbox_controller_remove(struct mbox_controller *mbox)
{
	struct mbox_chan *chan;

	ако (!mbox)
		врати;

	док (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    struct mbox_chan, node);
		ако (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->node);
	}

	sbi_list_del(&mbox->node);
}

struct mbox_chan *mbox_controller_request_chan(struct mbox_controller *mbox,
					       u32 *chan_args)
{
	struct mbox_chan *ret;
	struct sbi_dlist *pos;

	ако (!chan_args || !mbox || !mbox->request_chan)
		врати NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		ако (!sbi_memcmp(ret->chan_args, chan_args,
				sizeof(ret->chan_args)))
			врати ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	ако (!ret)
		врати NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, sizeof(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	врати ret;
}

void mbox_controller_free_chan(struct mbox_chan *chan)
{
	ако (!chan || !chan->mbox)
		врати;

	ако (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

цео mbox_chan_xfer(struct mbox_chan *chan, struct mbox_xfer *xfer)
{
	ако (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		врати SBI_EINVAL;

	ако (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		врати SBI_EINVAL;

	ако (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		врати SBI_EINVAL;

	ако (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	врати chan->mbox->xfer(chan, xfer);
}

цео mbox_chan_get_attribute(struct mbox_chan *chan, цео attr_id, void *out_value)
{
	ако (!chan || !chan->mbox || !out_value)
		врати SBI_EINVAL;

	ако (!chan->mbox->get_attribute)
		врати SBI_ENOTSUPP;

	врати chan->mbox->get_attribute(chan, attr_id, out_value);
}

цео mbox_chan_set_attribute(struct mbox_chan *chan, цео attr_id, void *new_value)
{
	ако (!chan || !chan->mbox || !new_value)
		врати SBI_EINVAL;

	ако (!chan->mbox->set_attribute)
		врати SBI_ENOTSUPP;

	врати chan->mbox->set_attribute(chan, attr_id, new_value);
}
