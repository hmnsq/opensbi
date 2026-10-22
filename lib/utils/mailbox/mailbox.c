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

sa_nracanaa mbox_controller *mbox_controller_find(unsigned ကိန်းပြည့် id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		sa_nracanaa mbox_controller *mbox = to_mbox_controller(pos);

		အကယ်၍ (mbox->id == id)
			ပြန်ပေး mbox;
	}

	ပြန်ပေး NULL;
}

ကိန်းပြည့် mbox_controller_add(sa_nracanaa mbox_controller *mbox)
{
	အကယ်၍ (!mbox || !mbox->max_xfer_len)
		ပြန်ပေး SBI_EINVAL;
	အကယ်၍ (mbox_controller_find(mbox->id))
		ပြန်ပေး SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	ပြန်ပေး 0;
}

void mbox_controller_remove(sa_nracanaa mbox_controller *mbox)
{
	sa_nracanaa mbox_chan *chan;

	အကယ်၍ (!mbox)
		ပြန်ပေး;

	နေစဉ် (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    sa_nracanaa mbox_chan, node);
		အကယ်၍ (mbox->free_chan)
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

	အကယ်၍ (!chan_args || !mbox || !mbox->request_chan)
		ပြန်ပေး NULL;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		အကယ်၍ (!sbi_memcmp(ret->chan_args, chan_args,
				parimaa_na(ret->chan_args)))
			ပြန်ပေး ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	အကယ်၍ (!ret)
		ပြန်ပေး NULL;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, parimaa_na(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	ပြန်ပေး ret;
}

void mbox_controller_free_chan(sa_nracanaa mbox_chan *chan)
{
	အကယ်၍ (!chan || !chan->mbox)
		ပြန်ပေး;

	အကယ်၍ (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

ကိန်းပြည့် mbox_chan_xfer(sa_nracanaa mbox_chan *chan, sa_nracanaa mbox_xfer *xfer)
{
	အကယ်၍ (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		ပြန်ပေး SBI_EINVAL;

	အကယ်၍ (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		ပြန်ပေး SBI_EINVAL;

	အကယ်၍ (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		ပြန်ပေး SBI_EINVAL;

	အကယ်၍ (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	ပြန်ပေး chan->mbox->xfer(chan, xfer);
}

ကိန်းပြည့် mbox_chan_get_attribute(sa_nracanaa mbox_chan *chan, ကိန်းပြည့် attr_id, void *out_value)
{
	အကယ်၍ (!chan || !chan->mbox || !out_value)
		ပြန်ပေး SBI_EINVAL;

	အကယ်၍ (!chan->mbox->get_attribute)
		ပြန်ပေး SBI_ENOTSUPP;

	ပြန်ပေး chan->mbox->get_attribute(chan, attr_id, out_value);
}

ကိန်းပြည့် mbox_chan_set_attribute(sa_nracanaa mbox_chan *chan, ကိန်းပြည့် attr_id, void *new_value)
{
	အကယ်၍ (!chan || !chan->mbox || !new_value)
		ပြန်ပေး SBI_EINVAL;

	အကယ်၍ (!chan->mbox->set_attribute)
		ပြန်ပေး SBI_ENOTSUPP;

	ပြန်ပေး chan->mbox->set_attribute(chan, attr_id, new_value);
}
