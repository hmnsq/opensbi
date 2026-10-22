/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi_utils/mailbox/mailbox.h>

ساكن SBI_LIST_HEAD(mbox_list);

بنية mbox_controller *mbox_controller_find(unsigned صحيح id)
{
	بنية sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		بنية mbox_controller *mbox = to_mbox_controller(pos);

		אם (mbox->id == id)
			החזר mbox;
	}

	החזר عدم;
}

صحيح mbox_controller_add(بنية mbox_controller *mbox)
{
	אם (!mbox || !mbox->max_xfer_len)
		החזר SBI_EINVAL;
	אם (mbox_controller_find(mbox->id))
		החזר SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	החזר 0;
}

فراغ mbox_controller_remove(بنية mbox_controller *mbox)
{
	بنية mbox_chan *chan;

	אם (!mbox)
		החזר;

	כלעוד (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    بنية mbox_chan, node);
		אם (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->node);
	}

	sbi_list_del(&mbox->node);
}

بنية mbox_chan *mbox_controller_request_chan(بنية mbox_controller *mbox,
					       u32 *chan_args)
{
	بنية mbox_chan *ret;
	بنية sbi_dlist *pos;

	אם (!chan_args || !mbox || !mbox->request_chan)
		החזר عدم;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		אם (!sbi_memcmp(ret->chan_args, chan_args,
				حجم(ret->chan_args)))
			החזר ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	אם (!ret)
		החזר عدم;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, حجم(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	החזר ret;
}

فراغ mbox_controller_free_chan(بنية mbox_chan *chan)
{
	אם (!chan || !chan->mbox)
		החזר;

	אם (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

صحيح mbox_chan_xfer(بنية mbox_chan *chan, بنية mbox_xfer *xfer)
{
	אם (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		החזר SBI_EINVAL;

	אם (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		החזר SBI_EINVAL;

	אם (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		החזר SBI_EINVAL;

	אם (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	החזר chan->mbox->xfer(chan, xfer);
}

صحيح mbox_chan_get_attribute(بنية mbox_chan *chan, صحيح attr_id, فراغ *out_value)
{
	אם (!chan || !chan->mbox || !out_value)
		החזר SBI_EINVAL;

	אם (!chan->mbox->get_attribute)
		החזר SBI_ENOTSUPP;

	החזר chan->mbox->get_attribute(chan, attr_id, out_value);
}

صحيح mbox_chan_set_attribute(بنية mbox_chan *chan, صحيح attr_id, فراغ *new_value)
{
	אם (!chan || !chan->mbox || !new_value)
		החזר SBI_EINVAL;

	אם (!chan->mbox->set_attribute)
		החזר SBI_ENOTSUPP;

	החזר chan->mbox->set_attribute(chan, attr_id, new_value);
}
