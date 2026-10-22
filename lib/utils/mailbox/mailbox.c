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

ساكن SBI_LIST_سر(mbox_list);

بنية mbox_controller *mbox_controller_پیدا_کړه(بینښه صحيح id)
{
	بنية sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		بنية mbox_controller *mbox = منزل_mbox_controller(pos);

		إذا (mbox->id == id)
			أرجع mbox;
	}

	أرجع عدم;
}

صحيح mbox_controller_جمع_کړه(بنية mbox_controller *mbox)
{
	إذا (!mbox || !mbox->max_xfer_len)
		أرجع SBI_EINVAL;
	إذا (mbox_controller_پیدا_کړه(mbox->id))
		أرجع SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_بل_seq, 0);
	SBI_INIT_LIST_سر(&mbox->chan_list);
	sbi_list_جمع_کړه(&mbox->غوټه, &mbox_list);

	أرجع 0;
}

فراغ mbox_controller_remove(بنية mbox_controller *mbox)
{
	بنية mbox_chan *chan;

	إذا (!mbox)
		أرجع;

	طالما (!sbi_list_تشه(&mbox->chan_list)) {
		chan = sbi_list_لومړی_ثبت(&mbox->chan_list,
					    بنية mbox_chan, غوټه);
		إذا (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->غوټه);
	}

	sbi_list_del(&mbox->غوټه);
}

بنية mbox_chan *mbox_controller_request_chan(بنية mbox_controller *mbox,
					       u32 *chan_دلیلونه)
{
	بنية mbox_chan *ret;
	بنية sbi_dlist *pos;

	إذا (!chan_دلیلونه || !mbox || !mbox->request_chan)
		أرجع عدم;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = منزل_mbox_chan(pos);
		إذا (!sbi_memcmp(ret->chan_دلیلونه, chan_دلیلونه,
				حجم(ret->chan_دلیلونه)))
			أرجع ret;
	}

	ret = mbox->request_chan(mbox, chan_دلیلونه);
	إذا (!ret)
		أرجع عدم;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_دلیلونه, chan_دلیلونه, حجم(ret->chan_دلیلونه));
	sbi_list_جمع_کړه(&ret->غوټه, &mbox->chan_list);
	أرجع ret;
}

فراغ mbox_controller_free_chan(بنية mbox_chan *chan)
{
	إذا (!chan || !chan->mbox)
		أرجع;

	إذا (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->غوټه);
}

صحيح mbox_chan_xfer(بنية mbox_chan *chan, بنية mbox_xfer *xfer)
{
	إذا (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		أرجع SBI_EINVAL;

	إذا (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		أرجع SBI_EINVAL;

	إذا (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		أرجع SBI_EINVAL;

	إذا (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_سېټ_sequence(xfer,
			atomic_جمع_کړه_return(&chan->mbox->xfer_بل_seq, 1));

	أرجع chan->mbox->xfer(chan, xfer);
}

صحيح mbox_chan_get_attribute(بنية mbox_chan *chan, صحيح attr_id, فراغ *بهر_ارزښت)
{
	إذا (!chan || !chan->mbox || !بهر_ارزښت)
		أرجع SBI_EINVAL;

	إذا (!chan->mbox->get_attribute)
		أرجع SBI_ENOTSUPP;

	أرجع chan->mbox->get_attribute(chan, attr_id, بهر_ارزښت);
}

صحيح mbox_chan_سېټ_attribute(بنية mbox_chan *chan, صحيح attr_id, فراغ *new_ارزښت)
{
	إذا (!chan || !chan->mbox || !new_ارزښت)
		أرجع SBI_EINVAL;

	إذا (!chan->mbox->سېټ_attribute)
		أرجع SBI_ENOTSUPP;

	أرجع chan->mbox->سېټ_attribute(chan, attr_id, new_ارزښت);
}
