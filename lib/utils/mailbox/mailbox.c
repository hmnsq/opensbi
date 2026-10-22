/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/mailbox.h>

ಸ್ಥಿರ SBI_LIST_HEAD(mbox_list);

ಸಂರಚನಾ mbox_controller *mbox_controller_find(unsigned ಪೂರ್ಣಾಙ್ಕ id)
{
	ಸಂರಚನಾ sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		ಸಂರಚನಾ mbox_controller *mbox = to_mbox_controller(pos);

		ಯದಿ (mbox->id == id)
			ಪ್ರತ್ಯಾಗಮ mbox;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ mbox_controller_add(ಸಂರಚನಾ mbox_controller *mbox)
{
	ಯದಿ (!mbox || !mbox->max_xfer_len)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (mbox_controller_find(mbox->id))
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->ಗಂಟು, &mbox_list);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ mbox_controller_remove(ಸಂರಚನಾ mbox_controller *mbox)
{
	ಸಂರಚನಾ mbox_chan *chan;

	ಯದಿ (!mbox)
		ಪ್ರತ್ಯಾಗಮ;

	ಯಾವತ್ (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    ಸಂರಚನಾ mbox_chan, ಗಂಟು);
		ಯದಿ (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->ಗಂಟು);
	}

	sbi_list_del(&mbox->ಗಂಟು);
}

ಸಂರಚನಾ mbox_chan *mbox_controller_request_chan(ಸಂರಚನಾ mbox_controller *mbox,
					       u32 *chan_args)
{
	ಸಂರಚನಾ mbox_chan *ret;
	ಸಂರಚನಾ sbi_dlist *pos;

	ಯದಿ (!chan_args || !mbox || !mbox->request_chan)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		ಯದಿ (!sbi_memcmp(ret->chan_args, chan_args,
				ಪರಿಮಾಣ(ret->chan_args)))
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	ಯದಿ (!ret)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, ಪರಿಮಾಣ(ret->chan_args));
	sbi_list_add(&ret->ಗಂಟು, &mbox->chan_list);
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ mbox_controller_free_chan(ಸಂರಚನಾ mbox_chan *chan)
{
	ಯದಿ (!chan || !chan->mbox)
		ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->ಗಂಟು);
}

ಪೂರ್ಣಾಙ್ಕ mbox_chan_xfer(ಸಂರಚನಾ mbox_chan *chan, ಸಂರಚನಾ mbox_xfer *xfer)
{
	ಯದಿ (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	ಪ್ರತ್ಯಾಗಮ chan->mbox->xfer(chan, xfer);
}

ಪೂರ್ಣಾಙ್ಕ mbox_chan_get_attribute(ಸಂರಚನಾ mbox_chan *chan, ಪೂರ್ಣಾಙ್ಕ attr_id, ಶೂನ್ಯ *out_ಮೌಲ್ಯ)
{
	ಯದಿ (!chan || !chan->mbox || !out_ಮೌಲ್ಯ)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (!chan->mbox->get_attribute)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	ಪ್ರತ್ಯಾಗಮ chan->mbox->get_attribute(chan, attr_id, out_ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ mbox_chan_set_attribute(ಸಂರಚನಾ mbox_chan *chan, ಪೂರ್ಣಾಙ್ಕ attr_id, ಶೂನ್ಯ *new_ಮೌಲ್ಯ)
{
	ಯದಿ (!chan || !chan->mbox || !new_ಮೌಲ್ಯ)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (!chan->mbox->set_attribute)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	ಪ್ರತ್ಯಾಗಮ chan->mbox->set_attribute(chan, attr_id, new_ಮೌಲ್ಯ);
}
