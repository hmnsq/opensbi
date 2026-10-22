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

struct mbox_controller *mbox_controller_find(unsigned số_nguyên id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		struct mbox_controller *mbox = to_mbox_controller(pos);

		nếu (mbox->id == id)
			trả_về mbox;
	}

	trả_về NULL;
}

số_nguyên mbox_controller_add(struct mbox_controller *mbox)
{
	nếu (!mbox || !mbox->max_xfer_len)
		trả_về SBI_EINVAL;
	nếu (mbox_controller_find(mbox->id))
		trả_về SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	trả_về 0;
}

void mbox_controller_remove(struct mbox_controller *mbox)
{
	struct mbox_chan *chan;

	nếu (!mbox)
		trả_về;

	trong_khi (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    struct mbox_chan, node);
		nếu (mbox->free_chan)
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

	nếu (!chan_args || !mbox || !mbox->request_chan)
		trả_về NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		nếu (!sbi_memcmp(ret->chan_args, chan_args,
				sizeof(ret->chan_args)))
			trả_về ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	nếu (!ret)
		trả_về NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, sizeof(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	trả_về ret;
}

void mbox_controller_free_chan(struct mbox_chan *chan)
{
	nếu (!chan || !chan->mbox)
		trả_về;

	nếu (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

số_nguyên mbox_chan_xfer(struct mbox_chan *chan, struct mbox_xfer *xfer)
{
	nếu (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		trả_về SBI_EINVAL;

	nếu (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		trả_về SBI_EINVAL;

	nếu (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		trả_về SBI_EINVAL;

	nếu (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	trả_về chan->mbox->xfer(chan, xfer);
}

số_nguyên mbox_chan_get_attribute(struct mbox_chan *chan, số_nguyên attr_id, void *out_value)
{
	nếu (!chan || !chan->mbox || !out_value)
		trả_về SBI_EINVAL;

	nếu (!chan->mbox->get_attribute)
		trả_về SBI_ENOTSUPP;

	trả_về chan->mbox->get_attribute(chan, attr_id, out_value);
}

số_nguyên mbox_chan_set_attribute(struct mbox_chan *chan, số_nguyên attr_id, void *new_value)
{
	nếu (!chan || !chan->mbox || !new_value)
		trả_về SBI_EINVAL;

	nếu (!chan->mbox->set_attribute)
		trả_về SBI_ENOTSUPP;

	trả_về chan->mbox->set_attribute(chan, attr_id, new_value);
}
