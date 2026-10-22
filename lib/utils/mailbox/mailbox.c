/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/mailbox.h>

ਸਥਿਰ SBI_LIST_ਸਿਰ(mbox_list);

ਬਣਤਰ mbox_controller *mbox_controller_ਲੱਭੋ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ id)
{
	ਬਣਤਰ sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		ਬਣਤਰ mbox_controller *mbox = ਮੰਜ਼ਿਲ_mbox_controller(pos);

		ਜੇ (mbox->id == id)
			ਵਾਪਸ mbox;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

ਪੂਰਨ mbox_controller_ਜੋੜੋ(ਬਣਤਰ mbox_controller *mbox)
{
	ਜੇ (!mbox || !mbox->max_xfer_len)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (mbox_controller_ਲੱਭੋ(mbox->id))
		ਵਾਪਸ SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_ਅਗਲਾ_seq, 0);
	SBI_INIT_LIST_ਸਿਰ(&mbox->chan_list);
	sbi_list_ਜੋੜੋ(&mbox->ਗੰਢ, &mbox_list);

	ਵਾਪਸ 0;
}

ਖਾਲੀ mbox_controller_remove(ਬਣਤਰ mbox_controller *mbox)
{
	ਬਣਤਰ mbox_chan *chan;

	ਜੇ (!mbox)
		ਵਾਪਸ;

	ਜਦੋਂ (!sbi_list_ਖਾਲੀ_ਥਾਵਾਂ(&mbox->chan_list)) {
		chan = sbi_list_ਪਹਿਲਾ_ਇੰਦਰਾਜ(&mbox->chan_list,
					    ਬਣਤਰ mbox_chan, ਗੰਢ);
		ਜੇ (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->ਗੰਢ);
	}

	sbi_list_del(&mbox->ਗੰਢ);
}

ਬਣਤਰ mbox_chan *mbox_controller_request_chan(ਬਣਤਰ mbox_controller *mbox,
					       u32 *chan_ਦਲੀਲਾਂ)
{
	ਬਣਤਰ mbox_chan *ret;
	ਬਣਤਰ sbi_dlist *pos;

	ਜੇ (!chan_ਦਲੀਲਾਂ || !mbox || !mbox->request_chan)
		ਵਾਪਸ ਨਿਰਾ;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = ਮੰਜ਼ਿਲ_mbox_chan(pos);
		ਜੇ (!sbi_memcmp(ret->chan_ਦਲੀਲਾਂ, chan_ਦਲੀਲਾਂ,
				ਆਕਾਰ(ret->chan_ਦਲੀਲਾਂ)))
			ਵਾਪਸ ret;
	}

	ret = mbox->request_chan(mbox, chan_ਦਲੀਲਾਂ);
	ਜੇ (!ret)
		ਵਾਪਸ ਨਿਰਾ;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_ਦਲੀਲਾਂ, chan_ਦਲੀਲਾਂ, ਆਕਾਰ(ret->chan_ਦਲੀਲਾਂ));
	sbi_list_ਜੋੜੋ(&ret->ਗੰਢ, &mbox->chan_list);
	ਵਾਪਸ ret;
}

ਖਾਲੀ mbox_controller_free_chan(ਬਣਤਰ mbox_chan *chan)
{
	ਜੇ (!chan || !chan->mbox)
		ਵਾਪਸ;

	ਜੇ (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->ਗੰਢ);
}

ਪੂਰਨ mbox_chan_xfer(ਬਣਤਰ mbox_chan *chan, ਬਣਤਰ mbox_xfer *xfer)
{
	ਜੇ (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_ਸੈੱਟ_sequence(xfer,
			atomic_ਜੋੜੋ_return(&chan->mbox->xfer_ਅਗਲਾ_seq, 1));

	ਵਾਪਸ chan->mbox->xfer(chan, xfer);
}

ਪੂਰਨ mbox_chan_get_attribute(ਬਣਤਰ mbox_chan *chan, ਪੂਰਨ attr_id, ਖਾਲੀ *ਬਾਹਰਲਾ_ਮੁੱਲ)
{
	ਜੇ (!chan || !chan->mbox || !ਬਾਹਰਲਾ_ਮੁੱਲ)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (!chan->mbox->get_attribute)
		ਵਾਪਸ SBI_ENOTSUPP;

	ਵਾਪਸ chan->mbox->get_attribute(chan, attr_id, ਬਾਹਰਲਾ_ਮੁੱਲ);
}

ਪੂਰਨ mbox_chan_ਸੈੱਟ_attribute(ਬਣਤਰ mbox_chan *chan, ਪੂਰਨ attr_id, ਖਾਲੀ *new_ਮੁੱਲ)
{
	ਜੇ (!chan || !chan->mbox || !new_ਮੁੱਲ)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (!chan->mbox->ਸੈੱਟ_attribute)
		ਵਾਪਸ SBI_ENOTSUPP;

	ਵਾਪਸ chan->mbox->ਸੈੱਟ_attribute(chan, attr_id, new_ਮੁੱਲ);
}
