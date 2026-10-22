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

بنية mbox_controller *mbox_controller_ڳوليو(بنانشان صحيح id)
{
	بنية sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		بنية mbox_controller *mbox = منزل_mbox_controller(pos);

		إذا (mbox->id == id)
			أرجع mbox;
	}

	أرجع عدم;
}

صحيح mbox_controller_جوڙيو(بنية mbox_controller *mbox)
{
	إذا (!mbox || !mbox->max_xfer_len)
		أرجع SBI_EINVAL;
	إذا (mbox_controller_ڳوليو(mbox->id))
		أرجع SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_ايندڙ_seq, 0);
	SBI_INIT_LIST_سر(&mbox->chan_list);
	sbi_list_جوڙيو(&mbox->ڳنڍ, &mbox_list);

	أرجع 0;
}

فراغ mbox_controller_remove(بنية mbox_controller *mbox)
{
	بنية mbox_chan *chan;

	إذا (!mbox)
		أرجع;

	طالما (!sbi_list_خالي_جايون(&mbox->chan_list)) {
		chan = sbi_list_پهريون_داخلا(&mbox->chan_list,
					    بنية mbox_chan, ڳنڍ);
		إذا (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->ڳنڍ);
	}

	sbi_list_del(&mbox->ڳنڍ);
}

بنية mbox_chan *mbox_controller_request_chan(بنية mbox_controller *mbox,
					       u32 *chan_دليلون)
{
	بنية mbox_chan *ret;
	بنية sbi_dlist *pos;

	إذا (!chan_دليلون || !mbox || !mbox->request_chan)
		أرجع عدم;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = منزل_mbox_chan(pos);
		إذا (!sbi_memcmp(ret->chan_دليلون, chan_دليلون,
				حجم(ret->chan_دليلون)))
			أرجع ret;
	}

	ret = mbox->request_chan(mbox, chan_دليلون);
	إذا (!ret)
		أرجع عدم;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_دليلون, chan_دليلون, حجم(ret->chan_دليلون));
	sbi_list_جوڙيو(&ret->ڳنڍ, &mbox->chan_list);
	أرجع ret;
}

فراغ mbox_controller_free_chan(بنية mbox_chan *chan)
{
	إذا (!chan || !chan->mbox)
		أرجع;

	إذا (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->ڳنڍ);
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
		mbox_xfer_سيٽ_sequence(xfer,
			atomic_جوڙيو_return(&chan->mbox->xfer_ايندڙ_seq, 1));

	أرجع chan->mbox->xfer(chan, xfer);
}

صحيح mbox_chan_get_attribute(بنية mbox_chan *chan, صحيح attr_id, فراغ *ٻاهريون_قدر)
{
	إذا (!chan || !chan->mbox || !ٻاهريون_قدر)
		أرجع SBI_EINVAL;

	إذا (!chan->mbox->get_attribute)
		أرجع SBI_ENOTSUPP;

	أرجع chan->mbox->get_attribute(chan, attr_id, ٻاهريون_قدر);
}

صحيح mbox_chan_سيٽ_attribute(بنية mbox_chan *chan, صحيح attr_id, فراغ *new_قدر)
{
	إذا (!chan || !chan->mbox || !new_قدر)
		أرجع SBI_EINVAL;

	إذا (!chan->mbox->سيٽ_attribute)
		أرجع SBI_ENOTSUPP;

	أرجع chan->mbox->سيٽ_attribute(chan, attr_id, new_قدر);
}
