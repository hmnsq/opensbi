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

struct mbox_controller *mbox_controller_find(unsigned 整数 id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		struct mbox_controller *mbox = to_mbox_controller(pos);

		如果 (mbox->id == id)
			返回 mbox;
	}

	返回 NULL;
}

整数 mbox_controller_add(struct mbox_controller *mbox)
{
	如果 (!mbox || !mbox->max_xfer_len)
		返回 SBI_EINVAL;
	如果 (mbox_controller_find(mbox->id))
		返回 SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	返回 0;
}

void mbox_controller_remove(struct mbox_controller *mbox)
{
	struct mbox_chan *chan;

	如果 (!mbox)
		返回;

	当 (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    struct mbox_chan, node);
		如果 (mbox->free_chan)
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

	如果 (!chan_args || !mbox || !mbox->request_chan)
		返回 NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		如果 (!sbi_memcmp(ret->chan_args, chan_args,
				sizeof(ret->chan_args)))
			返回 ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	如果 (!ret)
		返回 NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, sizeof(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	返回 ret;
}

void mbox_controller_free_chan(struct mbox_chan *chan)
{
	如果 (!chan || !chan->mbox)
		返回;

	如果 (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

整数 mbox_chan_xfer(struct mbox_chan *chan, struct mbox_xfer *xfer)
{
	如果 (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		返回 SBI_EINVAL;

	如果 (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		返回 SBI_EINVAL;

	如果 (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		返回 SBI_EINVAL;

	如果 (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	返回 chan->mbox->xfer(chan, xfer);
}

整数 mbox_chan_get_attribute(struct mbox_chan *chan, 整数 attr_id, void *out_value)
{
	如果 (!chan || !chan->mbox || !out_value)
		返回 SBI_EINVAL;

	如果 (!chan->mbox->get_attribute)
		返回 SBI_ENOTSUPP;

	返回 chan->mbox->get_attribute(chan, attr_id, out_value);
}

整数 mbox_chan_set_attribute(struct mbox_chan *chan, 整数 attr_id, void *new_value)
{
	如果 (!chan || !chan->mbox || !new_value)
		返回 SBI_EINVAL;

	如果 (!chan->mbox->set_attribute)
		返回 SBI_ENOTSUPP;

	返回 chan->mbox->set_attribute(chan, attr_id, new_value);
}
