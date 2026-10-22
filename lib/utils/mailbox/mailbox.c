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

struct mbox_controller *mbox_controller_find(unsigned целое id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		struct mbox_controller *mbox = to_mbox_controller(pos);

		если (mbox->id == id)
			вернуть mbox;
	}

	вернуть NULL;
}

целое mbox_controller_add(struct mbox_controller *mbox)
{
	если (!mbox || !mbox->max_xfer_len)
		вернуть SBI_EINVAL;
	если (mbox_controller_find(mbox->id))
		вернуть SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	вернуть 0;
}

void mbox_controller_remove(struct mbox_controller *mbox)
{
	struct mbox_chan *chan;

	если (!mbox)
		вернуть;

	пока (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    struct mbox_chan, node);
		если (mbox->free_chan)
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

	если (!chan_args || !mbox || !mbox->request_chan)
		вернуть NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		если (!sbi_memcmp(ret->chan_args, chan_args,
				sizeof(ret->chan_args)))
			вернуть ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	если (!ret)
		вернуть NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, sizeof(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	вернуть ret;
}

void mbox_controller_free_chan(struct mbox_chan *chan)
{
	если (!chan || !chan->mbox)
		вернуть;

	если (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

целое mbox_chan_xfer(struct mbox_chan *chan, struct mbox_xfer *xfer)
{
	если (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		вернуть SBI_EINVAL;

	если (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		вернуть SBI_EINVAL;

	если (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		вернуть SBI_EINVAL;

	если (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	вернуть chan->mbox->xfer(chan, xfer);
}

целое mbox_chan_get_attribute(struct mbox_chan *chan, целое attr_id, void *out_value)
{
	если (!chan || !chan->mbox || !out_value)
		вернуть SBI_EINVAL;

	если (!chan->mbox->get_attribute)
		вернуть SBI_ENOTSUPP;

	вернуть chan->mbox->get_attribute(chan, attr_id, out_value);
}

целое mbox_chan_set_attribute(struct mbox_chan *chan, целое attr_id, void *new_value)
{
	если (!chan || !chan->mbox || !new_value)
		вернуть SBI_EINVAL;

	если (!chan->mbox->set_attribute)
		вернуть SBI_ENOTSUPP;

	вернуть chan->mbox->set_attribute(chan, attr_id, new_value);
}
