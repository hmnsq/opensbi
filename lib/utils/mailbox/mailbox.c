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

		ᨀᨘ (mbox->id == id)
			ᨀᨘᨒ mbox;
	}

	ᨀᨘᨒ NULL;
}

puur_naa_nka mbox_controller_add(sa_nracanaa mbox_controller *mbox)
{
	ᨀᨘ (!mbox || !mbox->max_xfer_len)
		ᨀᨘᨒ SBI_EINVAL;
	ᨀᨘ (mbox_controller_find(mbox->id))
		ᨀᨘᨒ SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	ᨀᨘᨒ 0;
}

void mbox_controller_remove(sa_nracanaa mbox_controller *mbox)
{
	sa_nracanaa mbox_chan *chan;

	ᨀᨘ (!mbox)
		ᨀᨘᨒ;

	ᨀᨛ (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    sa_nracanaa mbox_chan, node);
		ᨀᨘ (mbox->free_chan)
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

	ᨀᨘ (!chan_args || !mbox || !mbox->request_chan)
		ᨀᨘᨒ NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		ᨀᨘ (!sbi_memcmp(ret->chan_args, chan_args,
				parimaa_na(ret->chan_args)))
			ᨀᨘᨒ ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	ᨀᨘ (!ret)
		ᨀᨘᨒ NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, parimaa_na(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	ᨀᨘᨒ ret;
}

void mbox_controller_free_chan(sa_nracanaa mbox_chan *chan)
{
	ᨀᨘ (!chan || !chan->mbox)
		ᨀᨘᨒ;

	ᨀᨘ (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

puur_naa_nka mbox_chan_xfer(sa_nracanaa mbox_chan *chan, sa_nracanaa mbox_xfer *xfer)
{
	ᨀᨘ (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		ᨀᨘᨒ SBI_EINVAL;

	ᨀᨘ (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		ᨀᨘᨒ SBI_EINVAL;

	ᨀᨘ (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		ᨀᨘᨒ SBI_EINVAL;

	ᨀᨘ (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	ᨀᨘᨒ chan->mbox->xfer(chan, xfer);
}

puur_naa_nka mbox_chan_get_attribute(sa_nracanaa mbox_chan *chan, puur_naa_nka attr_id, void *out_value)
{
	ᨀᨘ (!chan || !chan->mbox || !out_value)
		ᨀᨘᨒ SBI_EINVAL;

	ᨀᨘ (!chan->mbox->get_attribute)
		ᨀᨘᨒ SBI_ENOTSUPP;

	ᨀᨘᨒ chan->mbox->get_attribute(chan, attr_id, out_value);
}

puur_naa_nka mbox_chan_set_attribute(sa_nracanaa mbox_chan *chan, puur_naa_nka attr_id, void *new_value)
{
	ᨀᨘ (!chan || !chan->mbox || !new_value)
		ᨀᨘᨒ SBI_EINVAL;

	ᨀᨘ (!chan->mbox->set_attribute)
		ᨀᨘᨒ SBI_ENOTSUPP;

	ᨀᨘᨒ chan->mbox->set_attribute(chan, attr_id, new_value);
}
