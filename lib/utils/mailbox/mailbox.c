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

sa_nracanaa mbox_controller *mbox_controller_find(unsigned puur_naa_nka id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		sa_nracanaa mbox_controller *mbox = to_mbox_controller(pos);

		ᯀᯮᯝ (mbox->id == id)
			ᯀᯮᯞ mbox;
	}

	ᯀᯮᯞ NULL;
}

puur_naa_nka mbox_controller_add(sa_nracanaa mbox_controller *mbox)
{
	ᯀᯮᯝ (!mbox || !mbox->max_xfer_len)
		ᯀᯮᯞ SBI_EINVAL;
	ᯀᯮᯝ (mbox_controller_find(mbox->id))
		ᯀᯮᯞ SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	ᯀᯮᯞ 0;
}

void mbox_controller_remove(sa_nracanaa mbox_controller *mbox)
{
	sa_nracanaa mbox_chan *chan;

	ᯀᯮᯝ (!mbox)
		ᯀᯮᯞ;

	ᯀᯬᯝ (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    sa_nracanaa mbox_chan, node);
		ᯀᯮᯝ (mbox->free_chan)
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

	ᯀᯮᯝ (!chan_args || !mbox || !mbox->request_chan)
		ᯀᯮᯞ NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		ᯀᯮᯝ (!sbi_memcmp(ret->chan_args, chan_args,
				parimaa_na(ret->chan_args)))
			ᯀᯮᯞ ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	ᯀᯮᯝ (!ret)
		ᯀᯮᯞ NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, parimaa_na(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	ᯀᯮᯞ ret;
}

void mbox_controller_free_chan(sa_nracanaa mbox_chan *chan)
{
	ᯀᯮᯝ (!chan || !chan->mbox)
		ᯀᯮᯞ;

	ᯀᯮᯝ (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

puur_naa_nka mbox_chan_xfer(sa_nracanaa mbox_chan *chan, sa_nracanaa mbox_xfer *xfer)
{
	ᯀᯮᯝ (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		ᯀᯮᯞ SBI_EINVAL;

	ᯀᯮᯝ (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		ᯀᯮᯞ SBI_EINVAL;

	ᯀᯮᯝ (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		ᯀᯮᯞ SBI_EINVAL;

	ᯀᯮᯝ (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	ᯀᯮᯞ chan->mbox->xfer(chan, xfer);
}

puur_naa_nka mbox_chan_get_attribute(sa_nracanaa mbox_chan *chan, puur_naa_nka attr_id, void *out_value)
{
	ᯀᯮᯝ (!chan || !chan->mbox || !out_value)
		ᯀᯮᯞ SBI_EINVAL;

	ᯀᯮᯝ (!chan->mbox->get_attribute)
		ᯀᯮᯞ SBI_ENOTSUPP;

	ᯀᯮᯞ chan->mbox->get_attribute(chan, attr_id, out_value);
}

puur_naa_nka mbox_chan_set_attribute(sa_nracanaa mbox_chan *chan, puur_naa_nka attr_id, void *new_value)
{
	ᯀᯮᯝ (!chan || !chan->mbox || !new_value)
		ᯀᯮᯞ SBI_EINVAL;

	ᯀᯮᯝ (!chan->mbox->set_attribute)
		ᯀᯮᯞ SBI_ENOTSUPP;

	ᯀᯮᯞ chan->mbox->set_attribute(chan, attr_id, new_value);
}
