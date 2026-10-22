/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi_utils/mailbox/mailbox.h>

স্থির SBI_LIST_HEAD(mbox_list);

সংরচনা mbox_controller *mbox_controller_find(unsigned পূর্ণাঙ্ক id)
{
	সংরচনা sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		সংরচনা mbox_controller *mbox = to_mbox_controller(pos);

		যদি (mbox->id == id)
			প্রত্যাগম mbox;
	}

	প্রত্যাগম রিক্ত;
}

পূর্ণাঙ্ক mbox_controller_add(সংরচনা mbox_controller *mbox)
{
	যদি (!mbox || !mbox->max_xfer_len)
		প্রত্যাগম SBI_EINVAL;
	যদি (mbox_controller_find(mbox->id))
		প্রত্যাগম SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->গ্রন্থি, &mbox_list);

	প্রত্যাগম 0;
}

শূন্য mbox_controller_remove(সংরচনা mbox_controller *mbox)
{
	সংরচনা mbox_chan *chan;

	যদি (!mbox)
		প্রত্যাগম;

	যাবত্ (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    সংরচনা mbox_chan, গ্রন্থি);
		যদি (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->গ্রন্থি);
	}

	sbi_list_del(&mbox->গ্রন্থি);
}

সংরচনা mbox_chan *mbox_controller_request_chan(সংরচনা mbox_controller *mbox,
					       u32 *chan_args)
{
	সংরচনা mbox_chan *ret;
	সংরচনা sbi_dlist *pos;

	যদি (!chan_args || !mbox || !mbox->request_chan)
		প্রত্যাগম রিক্ত;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		যদি (!sbi_memcmp(ret->chan_args, chan_args,
				পরিমাণ(ret->chan_args)))
			প্রত্যাগম ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	যদি (!ret)
		প্রত্যাগম রিক্ত;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, পরিমাণ(ret->chan_args));
	sbi_list_add(&ret->গ্রন্থি, &mbox->chan_list);
	প্রত্যাগম ret;
}

শূন্য mbox_controller_free_chan(সংরচনা mbox_chan *chan)
{
	যদি (!chan || !chan->mbox)
		প্রত্যাগম;

	যদি (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->গ্রন্থি);
}

পূর্ণাঙ্ক mbox_chan_xfer(সংরচনা mbox_chan *chan, সংরচনা mbox_xfer *xfer)
{
	যদি (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		প্রত্যাগম SBI_EINVAL;

	যদি (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		প্রত্যাগম SBI_EINVAL;

	যদি (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		প্রত্যাগম SBI_EINVAL;

	যদি (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	প্রত্যাগম chan->mbox->xfer(chan, xfer);
}

পূর্ণাঙ্ক mbox_chan_get_attribute(সংরচনা mbox_chan *chan, পূর্ণাঙ্ক attr_id, শূন্য *out_মান)
{
	যদি (!chan || !chan->mbox || !out_মান)
		প্রত্যাগম SBI_EINVAL;

	যদি (!chan->mbox->get_attribute)
		প্রত্যাগম SBI_ENOTSUPP;

	প্রত্যাগম chan->mbox->get_attribute(chan, attr_id, out_মান);
}

পূর্ণাঙ্ক mbox_chan_set_attribute(সংরচনা mbox_chan *chan, পূর্ণাঙ্ক attr_id, শূন্য *new_মান)
{
	যদি (!chan || !chan->mbox || !new_মান)
		প্রত্যাগম SBI_EINVAL;

	যদি (!chan->mbox->set_attribute)
		প্রত্যাগম SBI_ENOTSUPP;

	প্রত্যাগম chan->mbox->set_attribute(chan, attr_id, new_মান);
}
