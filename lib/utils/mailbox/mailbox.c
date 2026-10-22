/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/mailbox.h>

സ്ഥിതം SBI_LIST_HEAD(mbox_list);

ഘടന mbox_controller *mbox_controller_find(unsigned പൂർണ്ണസംഖ്യ id)
{
	ഘടന sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		ഘടന mbox_controller *mbox = to_mbox_controller(pos);

		എങ്കിൽ (mbox->id == id)
			തിരികെ mbox;
	}

	തിരികെ ശൂന്യ;
}

പൂർണ്ണസംഖ്യ mbox_controller_add(ഘടന mbox_controller *mbox)
{
	എങ്കിൽ (!mbox || !mbox->max_xfer_len)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (mbox_controller_find(mbox->id))
		തിരികെ SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->കണ്ണി, &mbox_list);

	തിരികെ 0;
}

ശൂന്യം mbox_controller_remove(ഘടന mbox_controller *mbox)
{
	ഘടന mbox_chan *chan;

	എങ്കിൽ (!mbox)
		തിരികെ;

	ആകുമ്പോൾ (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    ഘടന mbox_chan, കണ്ണി);
		എങ്കിൽ (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->കണ്ണി);
	}

	sbi_list_del(&mbox->കണ്ണി);
}

ഘടന mbox_chan *mbox_controller_request_chan(ഘടന mbox_controller *mbox,
					       u32 *chan_args)
{
	ഘടന mbox_chan *ret;
	ഘടന sbi_dlist *pos;

	എങ്കിൽ (!chan_args || !mbox || !mbox->request_chan)
		തിരികെ ശൂന്യ;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		എങ്കിൽ (!sbi_memcmp(ret->chan_args, chan_args,
				വലുപ്പം(ret->chan_args)))
			തിരികെ ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	എങ്കിൽ (!ret)
		തിരികെ ശൂന്യ;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, വലുപ്പം(ret->chan_args));
	sbi_list_add(&ret->കണ്ണി, &mbox->chan_list);
	തിരികെ ret;
}

ശൂന്യം mbox_controller_free_chan(ഘടന mbox_chan *chan)
{
	എങ്കിൽ (!chan || !chan->mbox)
		തിരികെ;

	എങ്കിൽ (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->കണ്ണി);
}

പൂർണ്ണസംഖ്യ mbox_chan_xfer(ഘടന mbox_chan *chan, ഘടന mbox_xfer *xfer)
{
	എങ്കിൽ (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	തിരികെ chan->mbox->xfer(chan, xfer);
}

പൂർണ്ണസംഖ്യ mbox_chan_get_attribute(ഘടന mbox_chan *chan, പൂർണ്ണസംഖ്യ attr_id, ശൂന്യം *out_മൂല്യം)
{
	എങ്കിൽ (!chan || !chan->mbox || !out_മൂല്യം)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (!chan->mbox->get_attribute)
		തിരികെ SBI_ENOTSUPP;

	തിരികെ chan->mbox->get_attribute(chan, attr_id, out_മൂല്യം);
}

പൂർണ്ണസംഖ്യ mbox_chan_set_attribute(ഘടന mbox_chan *chan, പൂർണ്ണസംഖ്യ attr_id, ശൂന്യം *new_മൂല്യം)
{
	എങ്കിൽ (!chan || !chan->mbox || !new_മൂല്യം)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (!chan->mbox->set_attribute)
		തിരികെ SBI_ENOTSUPP;

	തിരികെ chan->mbox->set_attribute(chan, attr_id, new_മൂല്യം);
}
