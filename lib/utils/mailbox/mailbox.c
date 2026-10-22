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

sa_nracanaa mbox_controller *mbox_controller_find(unsigned จำนวนเต็ม id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		sa_nracanaa mbox_controller *mbox = to_mbox_controller(pos);

		ถ้า (mbox->id == id)
			คืน mbox;
	}

	คืน NULL;
}

จำนวนเต็ม mbox_controller_add(sa_nracanaa mbox_controller *mbox)
{
	ถ้า (!mbox || !mbox->max_xfer_len)
		คืน SBI_EINVAL;
	ถ้า (mbox_controller_find(mbox->id))
		คืน SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	คืน 0;
}

void mbox_controller_remove(sa_nracanaa mbox_controller *mbox)
{
	sa_nracanaa mbox_chan *chan;

	ถ้า (!mbox)
		คืน;

	ขณะ (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    sa_nracanaa mbox_chan, node);
		ถ้า (mbox->free_chan)
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

	ถ้า (!chan_args || !mbox || !mbox->request_chan)
		คืน NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		ถ้า (!sbi_memcmp(ret->chan_args, chan_args,
				parimaa_na(ret->chan_args)))
			คืน ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	ถ้า (!ret)
		คืน NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, parimaa_na(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	คืน ret;
}

void mbox_controller_free_chan(sa_nracanaa mbox_chan *chan)
{
	ถ้า (!chan || !chan->mbox)
		คืน;

	ถ้า (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

จำนวนเต็ม mbox_chan_xfer(sa_nracanaa mbox_chan *chan, sa_nracanaa mbox_xfer *xfer)
{
	ถ้า (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		คืน SBI_EINVAL;

	ถ้า (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		คืน SBI_EINVAL;

	ถ้า (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		คืน SBI_EINVAL;

	ถ้า (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	คืน chan->mbox->xfer(chan, xfer);
}

จำนวนเต็ม mbox_chan_get_attribute(sa_nracanaa mbox_chan *chan, จำนวนเต็ม attr_id, void *out_value)
{
	ถ้า (!chan || !chan->mbox || !out_value)
		คืน SBI_EINVAL;

	ถ้า (!chan->mbox->get_attribute)
		คืน SBI_ENOTSUPP;

	คืน chan->mbox->get_attribute(chan, attr_id, out_value);
}

จำนวนเต็ม mbox_chan_set_attribute(sa_nracanaa mbox_chan *chan, จำนวนเต็ม attr_id, void *new_value)
{
	ถ้า (!chan || !chan->mbox || !new_value)
		คืน SBI_EINVAL;

	ถ้า (!chan->mbox->set_attribute)
		คืน SBI_ENOTSUPP;

	คืน chan->mbox->set_attribute(chan, attr_id, new_value);
}
