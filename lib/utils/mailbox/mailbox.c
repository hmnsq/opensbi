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

স্থির SBI_LIST_শীর্ষ(mbox_list);

কাঠামো mbox_controller *mbox_controller_find(unsigned পূর্ণসংখ্যা id)
{
	কাঠামো sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		কাঠামো mbox_controller *mbox = to_mbox_controller(pos);

		যদি (mbox->id == id)
			ফেরত mbox;
	}

	ফেরত ফাঁকা;
}

পূর্ণসংখ্যা mbox_controller_add(কাঠামো mbox_controller *mbox)
{
	যদি (!mbox || !mbox->max_xfer_len)
		ফেরত SBI_EINVAL;
	যদি (mbox_controller_find(mbox->id))
		ফেরত SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_পরবর্তী_seq, 0);
	SBI_INIT_LIST_শীর্ষ(&mbox->chan_list);
	sbi_list_add(&mbox->গ্রন্থি, &mbox_list);

	ফেরত 0;
}

শূন্য mbox_controller_remove(কাঠামো mbox_controller *mbox)
{
	কাঠামো mbox_chan *chan;

	যদি (!mbox)
		ফেরত;

	যখন (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    কাঠামো mbox_chan, গ্রন্থি);
		যদি (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->গ্রন্থি);
	}

	sbi_list_del(&mbox->গ্রন্থি);
}

কাঠামো mbox_chan *mbox_controller_request_chan(কাঠামো mbox_controller *mbox,
					       u32 *chan_args)
{
	কাঠামো mbox_chan *ret;
	কাঠামো sbi_dlist *pos;

	যদি (!chan_args || !mbox || !mbox->request_chan)
		ফেরত ফাঁকা;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		যদি (!sbi_memcmp(ret->chan_args, chan_args,
				আকার(ret->chan_args)))
			ফেরত ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	যদি (!ret)
		ফেরত ফাঁকা;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, আকার(ret->chan_args));
	sbi_list_add(&ret->গ্রন্থি, &mbox->chan_list);
	ফেরত ret;
}

শূন্য mbox_controller_free_chan(কাঠামো mbox_chan *chan)
{
	যদি (!chan || !chan->mbox)
		ফেরত;

	যদি (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->গ্রন্থি);
}

পূর্ণসংখ্যা mbox_chan_xfer(কাঠামো mbox_chan *chan, কাঠামো mbox_xfer *xfer)
{
	যদি (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		ফেরত SBI_EINVAL;

	যদি (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		ফেরত SBI_EINVAL;

	যদি (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		ফেরত SBI_EINVAL;

	যদি (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_পরবর্তী_seq, 1));

	ফেরত chan->mbox->xfer(chan, xfer);
}

পূর্ণসংখ্যা mbox_chan_get_attribute(কাঠামো mbox_chan *chan, পূর্ণসংখ্যা attr_id, শূন্য *out_মান)
{
	যদি (!chan || !chan->mbox || !out_মান)
		ফেরত SBI_EINVAL;

	যদি (!chan->mbox->get_attribute)
		ফেরত SBI_ENOTSUPP;

	ফেরত chan->mbox->get_attribute(chan, attr_id, out_মান);
}

পূর্ণসংখ্যা mbox_chan_set_attribute(কাঠামো mbox_chan *chan, পূর্ণসংখ্যা attr_id, শূন্য *new_মান)
{
	যদি (!chan || !chan->mbox || !new_মান)
		ফেরত SBI_EINVAL;

	যদি (!chan->mbox->set_attribute)
		ফেরত SBI_ENOTSUPP;

	ফেরত chan->mbox->set_attribute(chan, attr_id, new_মান);
}
