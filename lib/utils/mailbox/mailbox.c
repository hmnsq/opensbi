/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_string.h>
#અન્તર્ભાવ <sbi_utils/mailbox/mailbox.h>

સ્થિર SBI_LIST_HEAD(mbox_list);

સંરચના mbox_controller *mbox_controller_find(unsigned પૂર્ણાંક id)
{
	સંરચના sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		સંરચના mbox_controller *mbox = to_mbox_controller(pos);

		જો (mbox->id == id)
			પાછા mbox;
	}

	પાછા ખાલી;
}

પૂર્ણાંક mbox_controller_add(સંરચના mbox_controller *mbox)
{
	જો (!mbox || !mbox->max_xfer_len)
		પાછા SBI_EINVAL;
	જો (mbox_controller_find(mbox->id))
		પાછા SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->ગાંઠ, &mbox_list);

	પાછા 0;
}

શૂન્ય mbox_controller_remove(સંરચના mbox_controller *mbox)
{
	સંરચના mbox_chan *chan;

	જો (!mbox)
		પાછા;

	જ્યારે (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    સંરચના mbox_chan, ગાંઠ);
		જો (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->ગાંઠ);
	}

	sbi_list_del(&mbox->ગાંઠ);
}

સંરચના mbox_chan *mbox_controller_request_chan(સંરચના mbox_controller *mbox,
					       u32 *chan_args)
{
	સંરચના mbox_chan *ret;
	સંરચના sbi_dlist *pos;

	જો (!chan_args || !mbox || !mbox->request_chan)
		પાછા ખાલી;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		જો (!sbi_memcmp(ret->chan_args, chan_args,
				કદ(ret->chan_args)))
			પાછા ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	જો (!ret)
		પાછા ખાલી;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, કદ(ret->chan_args));
	sbi_list_add(&ret->ગાંઠ, &mbox->chan_list);
	પાછા ret;
}

શૂન્ય mbox_controller_free_chan(સંરચના mbox_chan *chan)
{
	જો (!chan || !chan->mbox)
		પાછા;

	જો (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->ગાંઠ);
}

પૂર્ણાંક mbox_chan_xfer(સંરચના mbox_chan *chan, સંરચના mbox_xfer *xfer)
{
	જો (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		પાછા SBI_EINVAL;

	જો (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		પાછા SBI_EINVAL;

	જો (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		પાછા SBI_EINVAL;

	જો (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	પાછા chan->mbox->xfer(chan, xfer);
}

પૂર્ણાંક mbox_chan_get_attribute(સંરચના mbox_chan *chan, પૂર્ણાંક attr_id, શૂન્ય *out_કિંમત)
{
	જો (!chan || !chan->mbox || !out_કિંમત)
		પાછા SBI_EINVAL;

	જો (!chan->mbox->get_attribute)
		પાછા SBI_ENOTSUPP;

	પાછા chan->mbox->get_attribute(chan, attr_id, out_કિંમત);
}

પૂર્ણાંક mbox_chan_set_attribute(સંરચના mbox_chan *chan, પૂર્ણાંક attr_id, શૂન્ય *new_કિંમત)
{
	જો (!chan || !chan->mbox || !new_કિંમત)
		પાછા SBI_EINVAL;

	જો (!chan->mbox->set_attribute)
		પાછા SBI_ENOTSUPP;

	પાછા chan->mbox->set_attribute(chan, attr_id, new_કિંમત);
}
