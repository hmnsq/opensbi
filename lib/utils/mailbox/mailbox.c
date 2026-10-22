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

ساکن SBI_LIST_سر(mbox_list);

ساخت mbox_controller *mbox_controller_ڈھونڈو(غیرعلامتی صحیح id)
{
	ساخت sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		ساخت mbox_controller *mbox = مقصد_mbox_controller(pos);

		اگر (mbox->id == id)
			واپس mbox;
	}

	واپس تہی;
}

صحیح mbox_controller_جوڑو(ساخت mbox_controller *mbox)
{
	اگر (!mbox || !mbox->max_xfer_len)
		واپس SBI_EINVAL;
	اگر (mbox_controller_ڈھونڈو(mbox->id))
		واپس SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_اگلا_seq, 0);
	SBI_INIT_LIST_سر(&mbox->chan_list);
	sbi_list_جوڑو(&mbox->گرہ, &mbox_list);

	واپس 0;
}

خالی mbox_controller_remove(ساخت mbox_controller *mbox)
{
	ساخت mbox_chan *chan;

	اگر (!mbox)
		واپس;

	جب (!sbi_list_خالی_خانے(&mbox->chan_list)) {
		chan = sbi_list_پہلا_اندراج(&mbox->chan_list,
					    ساخت mbox_chan, گرہ);
		اگر (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->گرہ);
	}

	sbi_list_del(&mbox->گرہ);
}

ساخت mbox_chan *mbox_controller_request_chan(ساخت mbox_controller *mbox,
					       u32 *chan_دلائل)
{
	ساخت mbox_chan *ret;
	ساخت sbi_dlist *pos;

	اگر (!chan_دلائل || !mbox || !mbox->request_chan)
		واپس تہی;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = مقصد_mbox_chan(pos);
		اگر (!sbi_memcmp(ret->chan_دلائل, chan_دلائل,
				حجم(ret->chan_دلائل)))
			واپس ret;
	}

	ret = mbox->request_chan(mbox, chan_دلائل);
	اگر (!ret)
		واپس تہی;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_دلائل, chan_دلائل, حجم(ret->chan_دلائل));
	sbi_list_جوڑو(&ret->گرہ, &mbox->chan_list);
	واپس ret;
}

خالی mbox_controller_free_chan(ساخت mbox_chan *chan)
{
	اگر (!chan || !chan->mbox)
		واپس;

	اگر (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->گرہ);
}

صحیح mbox_chan_xfer(ساخت mbox_chan *chan, ساخت mbox_xfer *xfer)
{
	اگر (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		واپس SBI_EINVAL;

	اگر (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		واپس SBI_EINVAL;

	اگر (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		واپس SBI_EINVAL;

	اگر (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_سیٹ_sequence(xfer,
			atomic_جوڑو_return(&chan->mbox->xfer_اگلا_seq, 1));

	واپس chan->mbox->xfer(chan, xfer);
}

صحیح mbox_chan_get_attribute(ساخت mbox_chan *chan, صحیح attr_id, خالی *برآمد_قدر)
{
	اگر (!chan || !chan->mbox || !برآمد_قدر)
		واپس SBI_EINVAL;

	اگر (!chan->mbox->get_attribute)
		واپس SBI_ENOTSUPP;

	واپس chan->mbox->get_attribute(chan, attr_id, برآمد_قدر);
}

صحیح mbox_chan_سیٹ_attribute(ساخت mbox_chan *chan, صحیح attr_id, خالی *new_قدر)
{
	اگر (!chan || !chan->mbox || !new_قدر)
		واپس SBI_EINVAL;

	اگر (!chan->mbox->سیٹ_attribute)
		واپس SBI_ENOTSUPP;

	واپس chan->mbox->سیٹ_attribute(chan, attr_id, new_قدر);
}
