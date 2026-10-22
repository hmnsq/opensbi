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

		𐡄𐡍 (mbox->id == id)
			أرجع mbox;
	}

	أرجع عدم;
}

صحيح mbox_controller_add(بنية mbox_controller *mbox)
{
	𐡄𐡍 (!mbox || !mbox->max_xfer_len)
		أرجع SBI_EINVAL;
	𐡄𐡍 (mbox_controller_find(mbox->id))
		أرجع SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->node, &mbox_list);

	أرجع 0;
}

فراغ mbox_controller_remove(بنية mbox_controller *mbox)
{
	بنية mbox_chan *chan;

	𐡄𐡍 (!mbox)
		أرجع;

	طالما (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    بنية mbox_chan, node);
		𐡄𐡍 (mbox->free_chan)
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

	𐡄𐡍 (!chan_args || !mbox || !mbox->request_chan)
		أرجع عدم;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		𐡄𐡍 (!sbi_memcmp(ret->chan_args, chan_args,
				حجم(ret->chan_args)))
			أرجع ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	𐡄𐡍 (!ret)
		أرجع عدم;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, حجم(ret->chan_args));
	sbi_list_add(&ret->node, &mbox->chan_list);
	أرجع ret;
}

فراغ mbox_controller_free_chan(بنية mbox_chan *chan)
{
	𐡄𐡍 (!chan || !chan->mbox)
		أرجع;

	𐡄𐡍 (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->node);
}

صحيح mbox_chan_xfer(بنية mbox_chan *chan, بنية mbox_xfer *xfer)
{
	𐡄𐡍 (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		أرجع SBI_EINVAL;

	𐡄𐡍 (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		أرجع SBI_EINVAL;

	𐡄𐡍 (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		أرجع SBI_EINVAL;

	𐡄𐡍 (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	أرجع chan->mbox->xfer(chan, xfer);
}

صحيح mbox_chan_get_attribute(بنية mbox_chan *chan, صحيح attr_id, فراغ *out_value)
{
	𐡄𐡍 (!chan || !chan->mbox || !out_value)
		أرجع SBI_EINVAL;

	𐡄𐡍 (!chan->mbox->get_attribute)
		أرجع SBI_ENOTSUPP;

	أرجع chan->mbox->get_attribute(chan, attr_id, out_value);
}

صحيح mbox_chan_set_attribute(بنية mbox_chan *chan, صحيح attr_id, فراغ *new_value)
{
	𐡄𐡍 (!chan || !chan->mbox || !new_value)
		أرجع SBI_EINVAL;

	𐡄𐡍 (!chan->mbox->set_attribute)
		أرجع SBI_ENOTSUPP;

	أرجع chan->mbox->set_attribute(chan, attr_id, new_value);
}
