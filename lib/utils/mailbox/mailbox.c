/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi_utils/mailbox/mailbox.h>

ایستا SBI_LIST_HEAD(mbox_list);

ساختار mbox_controller *mbox_controller_find(unsigned صحیح id)
{
	ساختار sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		ساختار mbox_controller *mbox = to_mbox_controller(pos);

		اگر (mbox->id == id)
			بازگشت mbox;
	}

	بازگشت پوچ;
}

صحیح mbox_controller_add(ساختار mbox_controller *mbox)
{
	اگر (!mbox || !mbox->max_xfer_len)
		بازگشت SBI_EINVAL;
	اگر (mbox_controller_find(mbox->id))
		بازگشت SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->گره, &mbox_list);

	بازگشت 0;
}

تهی mbox_controller_remove(ساختار mbox_controller *mbox)
{
	ساختار mbox_chan *chan;

	اگر (!mbox)
		بازگشت;

	وقتی (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    ساختار mbox_chan, گره);
		اگر (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->گره);
	}

	sbi_list_del(&mbox->گره);
}

ساختار mbox_chan *mbox_controller_request_chan(ساختار mbox_controller *mbox,
					       u32 *chan_args)
{
	ساختار mbox_chan *ret;
	ساختار sbi_dlist *pos;

	اگر (!chan_args || !mbox || !mbox->request_chan)
		بازگشت پوچ;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		اگر (!sbi_memcmp(ret->chan_args, chan_args,
				اندازه(ret->chan_args)))
			بازگشت ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	اگر (!ret)
		بازگشت پوچ;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, اندازه(ret->chan_args));
	sbi_list_add(&ret->گره, &mbox->chan_list);
	بازگشت ret;
}

تهی mbox_controller_free_chan(ساختار mbox_chan *chan)
{
	اگر (!chan || !chan->mbox)
		بازگشت;

	اگر (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->گره);
}

صحیح mbox_chan_xfer(ساختار mbox_chan *chan, ساختار mbox_xfer *xfer)
{
	اگر (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		بازگشت SBI_EINVAL;

	اگر (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		بازگشت SBI_EINVAL;

	اگر (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		بازگشت SBI_EINVAL;

	اگر (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	بازگشت chan->mbox->xfer(chan, xfer);
}

صحیح mbox_chan_get_attribute(ساختار mbox_chan *chan, صحیح attr_id, تهی *out_مقدار)
{
	اگر (!chan || !chan->mbox || !out_مقدار)
		بازگشت SBI_EINVAL;

	اگر (!chan->mbox->get_attribute)
		بازگشت SBI_ENOTSUPP;

	بازگشت chan->mbox->get_attribute(chan, attr_id, out_مقدار);
}

صحیح mbox_chan_set_attribute(ساختار mbox_chan *chan, صحیح attr_id, تهی *new_مقدار)
{
	اگر (!chan || !chan->mbox || !new_مقدار)
		بازگشت SBI_EINVAL;

	اگر (!chan->mbox->set_attribute)
		بازگشت SBI_ENOTSUPP;

	بازگشت chan->mbox->set_attribute(chan, attr_id, new_مقدار);
}
