/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi_utils/mailbox/mailbox.h>

நிலையான SBI_LIST_HEAD(mbox_list);

கட்டமைப்பு mbox_controller *mbox_controller_find(unsigned முழு id)
{
	கட்டமைப்பு sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		கட்டமைப்பு mbox_controller *mbox = to_mbox_controller(pos);

		எனில் (mbox->id == id)
			திரும்பு mbox;
	}

	திரும்பு வெற்று;
}

முழு mbox_controller_add(கட்டமைப்பு mbox_controller *mbox)
{
	எனில் (!mbox || !mbox->max_xfer_len)
		திரும்பு SBI_EINVAL;
	எனில் (mbox_controller_find(mbox->id))
		திரும்பு SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->முனை, &mbox_list);

	திரும்பு 0;
}

வெறுமை mbox_controller_remove(கட்டமைப்பு mbox_controller *mbox)
{
	கட்டமைப்பு mbox_chan *chan;

	எனில் (!mbox)
		திரும்பு;

	போது (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    கட்டமைப்பு mbox_chan, முனை);
		எனில் (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->முனை);
	}

	sbi_list_del(&mbox->முனை);
}

கட்டமைப்பு mbox_chan *mbox_controller_request_chan(கட்டமைப்பு mbox_controller *mbox,
					       u32 *chan_args)
{
	கட்டமைப்பு mbox_chan *ret;
	கட்டமைப்பு sbi_dlist *pos;

	எனில் (!chan_args || !mbox || !mbox->request_chan)
		திரும்பு வெற்று;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		எனில் (!sbi_memcmp(ret->chan_args, chan_args,
				அளவு(ret->chan_args)))
			திரும்பு ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	எனில் (!ret)
		திரும்பு வெற்று;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, அளவு(ret->chan_args));
	sbi_list_add(&ret->முனை, &mbox->chan_list);
	திரும்பு ret;
}

வெறுமை mbox_controller_free_chan(கட்டமைப்பு mbox_chan *chan)
{
	எனில் (!chan || !chan->mbox)
		திரும்பு;

	எனில் (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->முனை);
}

முழு mbox_chan_xfer(கட்டமைப்பு mbox_chan *chan, கட்டமைப்பு mbox_xfer *xfer)
{
	எனில் (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		திரும்பு SBI_EINVAL;

	எனில் (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		திரும்பு SBI_EINVAL;

	எனில் (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		திரும்பு SBI_EINVAL;

	எனில் (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	திரும்பு chan->mbox->xfer(chan, xfer);
}

முழு mbox_chan_get_attribute(கட்டமைப்பு mbox_chan *chan, முழு attr_id, வெறுமை *out_மதிப்பு)
{
	எனில் (!chan || !chan->mbox || !out_மதிப்பு)
		திரும்பு SBI_EINVAL;

	எனில் (!chan->mbox->get_attribute)
		திரும்பு SBI_ENOTSUPP;

	திரும்பு chan->mbox->get_attribute(chan, attr_id, out_மதிப்பு);
}

முழு mbox_chan_set_attribute(கட்டமைப்பு mbox_chan *chan, முழு attr_id, வெறுமை *new_மதிப்பு)
{
	எனில் (!chan || !chan->mbox || !new_மதிப்பு)
		திரும்பு SBI_EINVAL;

	எனில் (!chan->mbox->set_attribute)
		திரும்பு SBI_ENOTSUPP;

	திரும்பு chan->mbox->set_attribute(chan, attr_id, new_மதிப்பு);
}
