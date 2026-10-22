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

ಸ್ಥಿರ SBI_LIST_ತಲೆ(mbox_list);

ರಚನೆ mbox_controller *mbox_controller_ಕಂಡುಹಿಡಿ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ id)
{
	ರಚನೆ sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		ರಚನೆ mbox_controller *mbox = ಗಮ್ಯ_mbox_controller(pos);

		ಆದರೆ (mbox->id == id)
			ಹಿಂತಿರುಗಿ mbox;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ mbox_controller_ಕೂಡಿಸು(ರಚನೆ mbox_controller *mbox)
{
	ಆದರೆ (!mbox || !mbox->max_xfer_len)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (mbox_controller_ಕಂಡುಹಿಡಿ(mbox->id))
		ಹಿಂತಿರುಗಿ SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_ಮುಂದಿನ_seq, 0);
	SBI_INIT_LIST_ತಲೆ(&mbox->chan_list);
	sbi_list_ಕೂಡಿಸು(&mbox->ಗಂಟು, &mbox_list);

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ mbox_controller_remove(ರಚನೆ mbox_controller *mbox)
{
	ರಚನೆ mbox_chan *chan;

	ಆದರೆ (!mbox)
		ಹಿಂತಿರುಗಿ;

	ಆಗ (!sbi_list_ಖಾಲಿಗಳು(&mbox->chan_list)) {
		chan = sbi_list_ಮೊದಲ_ನಮೂದು(&mbox->chan_list,
					    ರಚನೆ mbox_chan, ಗಂಟು);
		ಆದರೆ (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->ಗಂಟು);
	}

	sbi_list_del(&mbox->ಗಂಟು);
}

ರಚನೆ mbox_chan *mbox_controller_request_chan(ರಚನೆ mbox_controller *mbox,
					       u32 *chan_ವಾದಗಳು)
{
	ರಚನೆ mbox_chan *ret;
	ರಚನೆ sbi_dlist *pos;

	ಆದರೆ (!chan_ವಾದಗಳು || !mbox || !mbox->request_chan)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = ಗಮ್ಯ_mbox_chan(pos);
		ಆದರೆ (!sbi_memcmp(ret->chan_ವಾದಗಳು, chan_ವಾದಗಳು,
				ಗಾತ್ರ(ret->chan_ವಾದಗಳು)))
			ಹಿಂತಿರುಗಿ ret;
	}

	ret = mbox->request_chan(mbox, chan_ವಾದಗಳು);
	ಆದರೆ (!ret)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_ವಾದಗಳು, chan_ವಾದಗಳು, ಗಾತ್ರ(ret->chan_ವಾದಗಳು));
	sbi_list_ಕೂಡಿಸು(&ret->ಗಂಟು, &mbox->chan_list);
	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ mbox_controller_free_chan(ರಚನೆ mbox_chan *chan)
{
	ಆದರೆ (!chan || !chan->mbox)
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->ಗಂಟು);
}

ಪೂರ್ಣಾಂಕ mbox_chan_xfer(ರಚನೆ mbox_chan *chan, ರಚನೆ mbox_xfer *xfer)
{
	ಆದರೆ (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_ಗಣ_sequence(xfer,
			atomic_ಕೂಡಿಸು_return(&chan->mbox->xfer_ಮುಂದಿನ_seq, 1));

	ಹಿಂತಿರುಗಿ chan->mbox->xfer(chan, xfer);
}

ಪೂರ್ಣಾಂಕ mbox_chan_get_attribute(ರಚನೆ mbox_chan *chan, ಪೂರ್ಣಾಂಕ attr_id, ಶೂನ್ಯ *ಹೊರಹೋಗು_ಮೌಲ್ಯ)
{
	ಆದರೆ (!chan || !chan->mbox || !ಹೊರಹೋಗು_ಮೌಲ್ಯ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (!chan->mbox->get_attribute)
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ಹಿಂತಿರುಗಿ chan->mbox->get_attribute(chan, attr_id, ಹೊರಹೋಗು_ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ mbox_chan_ಗಣ_attribute(ರಚನೆ mbox_chan *chan, ಪೂರ್ಣಾಂಕ attr_id, ಶೂನ್ಯ *new_ಮೌಲ್ಯ)
{
	ಆದರೆ (!chan || !chan->mbox || !new_ಮೌಲ್ಯ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (!chan->mbox->ಗಣ_attribute)
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ಹಿಂತಿರುಗಿ chan->mbox->ಗಣ_attribute(chan, attr_id, new_ಮೌಲ್ಯ);
}
