/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/mailbox.h>

ସ୍ଥିର SBI_LIST_HEAD(mbox_list);

ସଂରଚନା mbox_controller *mbox_controller_find(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
	ସଂରଚନା sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		ସଂରଚନା mbox_controller *mbox = to_mbox_controller(pos);

		ଯଦି (mbox->id == id)
			ଫେରନ୍ତୁ mbox;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mbox_controller_add(ସଂରଚନା mbox_controller *mbox)
{
	ଯଦି (!mbox || !mbox->max_xfer_len)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (mbox_controller_find(mbox->id))
		ଫେରନ୍ତୁ SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_next_seq, 0);
	SBI_INIT_LIST_HEAD(&mbox->chan_list);
	sbi_list_add(&mbox->ଗଣ୍ଠି, &mbox_list);

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ mbox_controller_remove(ସଂରଚନା mbox_controller *mbox)
{
	ସଂରଚନା mbox_chan *chan;

	ଯଦି (!mbox)
		ଫେରନ୍ତୁ;

	ଯେତେବେଳେ (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    ସଂରଚନା mbox_chan, ଗଣ୍ଠି);
		ଯଦି (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->ଗଣ୍ଠି);
	}

	sbi_list_del(&mbox->ଗଣ୍ଠି);
}

ସଂରଚନା mbox_chan *mbox_controller_request_chan(ସଂରଚନା mbox_controller *mbox,
					       u32 *chan_args)
{
	ସଂରଚନା mbox_chan *ret;
	ସଂରଚନା sbi_dlist *pos;

	ଯଦି (!chan_args || !mbox || !mbox->request_chan)
		ଫେରନ୍ତୁ ଖାଲି;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		ଯଦି (!sbi_memcmp(ret->chan_args, chan_args,
				ଆକାର(ret->chan_args)))
			ଫେରନ୍ତୁ ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	ଯଦି (!ret)
		ଫେରନ୍ତୁ ଖାଲି;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, ଆକାର(ret->chan_args));
	sbi_list_add(&ret->ଗଣ୍ଠି, &mbox->chan_list);
	ଫେରନ୍ତୁ ret;
}

ଶୂନ୍ୟ mbox_controller_free_chan(ସଂରଚନା mbox_chan *chan)
{
	ଯଦି (!chan || !chan->mbox)
		ଫେରନ୍ତୁ;

	ଯଦି (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->ଗଣ୍ଠି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mbox_chan_xfer(ସଂରଚନା mbox_chan *chan, ସଂରଚନା mbox_xfer *xfer)
{
	ଯଦି (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_next_seq, 1));

	ଫେରନ୍ତୁ chan->mbox->xfer(chan, xfer);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mbox_chan_get_attribute(ସଂରଚନା mbox_chan *chan, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr_id, ଶୂନ୍ୟ *out_ମୂଲ୍ୟ)
{
	ଯଦି (!chan || !chan->mbox || !out_ମୂଲ୍ୟ)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (!chan->mbox->get_attribute)
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	ଫେରନ୍ତୁ chan->mbox->get_attribute(chan, attr_id, out_ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mbox_chan_set_attribute(ସଂରଚନା mbox_chan *chan, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr_id, ଶୂନ୍ୟ *new_ମୂଲ୍ୟ)
{
	ଯଦି (!chan || !chan->mbox || !new_ମୂଲ୍ୟ)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (!chan->mbox->set_attribute)
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	ଫେରନ୍ତୁ chan->mbox->set_attribute(chan, attr_id, new_ମୂଲ୍ୟ);
}
