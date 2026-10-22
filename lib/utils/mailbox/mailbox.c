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

struct mbox_controller *mbox_controller_find(unsigned ціле id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		struct mbox_controller *mbox = to_mbox_controller(pos);

		якщо (mbox->id == id)
			повернути mbox;
	}

	повернути NULL;
}

ціле mbox_controller_add(struct mbox_controller *mbox)
{
	якщо (!mbox || !mbox->max_xfer_len)
		повернути SBI_EINVAL;
	якщо (mbox_controller_find(mbox->id))
		повернути SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	повернути 0;
}

void mbox_controller_remove(struct mbox_controller *mbox)
{
	struct mbox_chan *chan;

	якщо (!mbox)
		повернути;

	поки (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    struct mbox_chan, node);
		якщо (mbox->free_chan)
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

	якщо (!chan_args || !mbox || !mbox->request_chan)
		повернути NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		якщо (!sbi_memcmp(ret->chan_args, chan_args,
				sizeof(ret->chan_args)))
			повернути ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	якщо (!ret)
		повернути NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, sizeof(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	повернути ret;
}

void mbox_controller_free_chan(struct mbox_chan *chan)
{
	якщо (!chan || !chan->mbox)
		повернути;

	якщо (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

ціле mbox_chan_xfer(struct mbox_chan *chan, struct mbox_xfer *xfer)
{
	якщо (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		повернути SBI_EINVAL;

	якщо (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		повернути SBI_EINVAL;

	якщо (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		повернути SBI_EINVAL;

	якщо (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	повернути chan->mbox->xfer(chan, xfer);
}

ціле mbox_chan_get_attribute(struct mbox_chan *chan, ціле attr_id, void *out_value)
{
	якщо (!chan || !chan->mbox || !out_value)
		повернути SBI_EINVAL;

	якщо (!chan->mbox->get_attribute)
		повернути SBI_ENOTSUPP;

	повернути chan->mbox->get_attribute(chan, attr_id, out_value);
}

ціле mbox_chan_set_attribute(struct mbox_chan *chan, ціле attr_id, void *new_value)
{
	якщо (!chan || !chan->mbox || !new_value)
		повернути SBI_EINVAL;

	якщо (!chan->mbox->set_attribute)
		повернути SBI_ENOTSUPP;

	повернути chan->mbox->set_attribute(chan, attr_id, new_value);
}
