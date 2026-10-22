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

		ᰀᰧ (mbox->id == id)
			ᰀᰧᰌ mbox;
	}

	ᰀᰧᰌ NULL;
}

puur_naa_nka mbox_controller_add(sa_nracanaa mbox_controller *mbox)
{
	ᰀᰧ (!mbox || !mbox->max_xfer_len)
		ᰀᰧᰌ SBI_EINVAL;
	ᰀᰧ (mbox_controller_find(mbox->id))
		ᰀᰧᰌ SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	ᰀᰧᰌ 0;
}

void mbox_controller_remove(sa_nracanaa mbox_controller *mbox)
{
	sa_nracanaa mbox_chan *chan;

	ᰀᰧ (!mbox)
		ᰀᰧᰌ;

	ᰀᰬ (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    sa_nracanaa mbox_chan, node);
		ᰀᰧ (mbox->free_chan)
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

	ᰀᰧ (!chan_args || !mbox || !mbox->request_chan)
		ᰀᰧᰌ NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		ᰀᰧ (!sbi_memcmp(ret->chan_args, chan_args,
				parimaa_na(ret->chan_args)))
			ᰀᰧᰌ ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	ᰀᰧ (!ret)
		ᰀᰧᰌ NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, parimaa_na(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	ᰀᰧᰌ ret;
}

void mbox_controller_free_chan(sa_nracanaa mbox_chan *chan)
{
	ᰀᰧ (!chan || !chan->mbox)
		ᰀᰧᰌ;

	ᰀᰧ (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

puur_naa_nka mbox_chan_xfer(sa_nracanaa mbox_chan *chan, sa_nracanaa mbox_xfer *xfer)
{
	ᰀᰧ (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		ᰀᰧᰌ SBI_EINVAL;

	ᰀᰧ (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		ᰀᰧᰌ SBI_EINVAL;

	ᰀᰧ (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		ᰀᰧᰌ SBI_EINVAL;

	ᰀᰧ (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	ᰀᰧᰌ chan->mbox->xfer(chan, xfer);
}

puur_naa_nka mbox_chan_get_attribute(sa_nracanaa mbox_chan *chan, puur_naa_nka attr_id, void *out_value)
{
	ᰀᰧ (!chan || !chan->mbox || !out_value)
		ᰀᰧᰌ SBI_EINVAL;

	ᰀᰧ (!chan->mbox->get_attribute)
		ᰀᰧᰌ SBI_ENOTSUPP;

	ᰀᰧᰌ chan->mbox->get_attribute(chan, attr_id, out_value);
}

puur_naa_nka mbox_chan_set_attribute(sa_nracanaa mbox_chan *chan, puur_naa_nka attr_id, void *new_value)
{
	ᰀᰧ (!chan || !chan->mbox || !new_value)
		ᰀᰧᰌ SBI_EINVAL;

	ᰀᰧ (!chan->mbox->set_attribute)
		ᰀᰧᰌ SBI_ENOTSUPP;

	ᰀᰧᰌ chan->mbox->set_attribute(chan, attr_id, new_value);
}
