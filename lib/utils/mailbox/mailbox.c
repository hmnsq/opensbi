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

sa_nracanaa mbox_controller *mbox_controller_find(unsigned ចំនួនគត់ id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		sa_nracanaa mbox_controller *mbox = to_mbox_controller(pos);

		ប្រសិនបើ (mbox->id == id)
			ត្រឡប់ mbox;
	}

	ត្រឡប់ NULL;
}

ចំនួនគត់ mbox_controller_add(sa_nracanaa mbox_controller *mbox)
{
	ប្រសិនបើ (!mbox || !mbox->max_xfer_len)
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (mbox_controller_find(mbox->id))
		ត្រឡប់ SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	ត្រឡប់ 0;
}

void mbox_controller_remove(sa_nracanaa mbox_controller *mbox)
{
	sa_nracanaa mbox_chan *chan;

	ប្រសិនបើ (!mbox)
		ត្រឡប់;

	ខណៈពេល (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    sa_nracanaa mbox_chan, node);
		ប្រសិនបើ (mbox->free_chan)
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

	ប្រសិនបើ (!chan_args || !mbox || !mbox->request_chan)
		ត្រឡប់ NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		ប្រសិនបើ (!sbi_memcmp(ret->chan_args, chan_args,
				parimaa_na(ret->chan_args)))
			ត្រឡប់ ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	ប្រសិនបើ (!ret)
		ត្រឡប់ NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, parimaa_na(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	ត្រឡប់ ret;
}

void mbox_controller_free_chan(sa_nracanaa mbox_chan *chan)
{
	ប្រសិនបើ (!chan || !chan->mbox)
		ត្រឡប់;

	ប្រសិនបើ (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

ចំនួនគត់ mbox_chan_xfer(sa_nracanaa mbox_chan *chan, sa_nracanaa mbox_xfer *xfer)
{
	ប្រសិនបើ (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	ត្រឡប់ chan->mbox->xfer(chan, xfer);
}

ចំនួនគត់ mbox_chan_get_attribute(sa_nracanaa mbox_chan *chan, ចំនួនគត់ attr_id, void *out_value)
{
	ប្រសិនបើ (!chan || !chan->mbox || !out_value)
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (!chan->mbox->get_attribute)
		ត្រឡប់ SBI_ENOTSUPP;

	ត្រឡប់ chan->mbox->get_attribute(chan, attr_id, out_value);
}

ចំនួនគត់ mbox_chan_set_attribute(sa_nracanaa mbox_chan *chan, ចំនួនគត់ attr_id, void *new_value)
{
	ប្រសិនបើ (!chan || !chan->mbox || !new_value)
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (!chan->mbox->set_attribute)
		ត្រឡប់ SBI_ENOTSUPP;

	ត្រឡប់ chan->mbox->set_attribute(chan, attr_id, new_value);
}
