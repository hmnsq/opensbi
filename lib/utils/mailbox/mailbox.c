/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi_utils/mailbox/mailbox.h>

अटल SBI_LIST_शीर्ष(mbox_list);

काष्ठा mbox_controller *mbox_controller_find(अचिन्हित पूर्णांक id)
{
	काष्ठा sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		काष्ठा mbox_controller *mbox = to_mbox_controller(pos);

		अगर (mbox->id == id)
			वापस mbox;
	}

	वापस शून्य;
}

पूर्णांक mbox_controller_add(काष्ठा mbox_controller *mbox)
{
	अगर (!mbox || !mbox->max_xfer_len)
		वापस SBI_EINVAL;
	अगर (mbox_controller_find(mbox->id))
		वापस SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_अग्र_seq, 0);
	SBI_INIT_LIST_शीर्ष(&mbox->chan_list);
	sbi_list_add(&mbox->गाँठ, &mbox_list);

	वापस 0;
}

व्योम mbox_controller_remove(काष्ठा mbox_controller *mbox)
{
	काष्ठा mbox_chan *chan;

	अगर (!mbox)
		वापस;

	जबतक (!sbi_list_empty(&mbox->chan_list)) {
		chan = sbi_list_first_entry(&mbox->chan_list,
					    काष्ठा mbox_chan, गाँठ);
		अगर (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->गाँठ);
	}

	sbi_list_del(&mbox->गाँठ);
}

काष्ठा mbox_chan *mbox_controller_request_chan(काष्ठा mbox_controller *mbox,
					       u32 *chan_args)
{
	काष्ठा mbox_chan *ret;
	काष्ठा sbi_dlist *pos;

	अगर (!chan_args || !mbox || !mbox->request_chan)
		वापस शून्य;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		अगर (!sbi_memcmp(ret->chan_args, chan_args,
				माप(ret->chan_args)))
			वापस ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
	अगर (!ret)
		वापस शून्य;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_args, chan_args, माप(ret->chan_args));
	sbi_list_add(&ret->गाँठ, &mbox->chan_list);
	वापस ret;
}

व्योम mbox_controller_free_chan(काष्ठा mbox_chan *chan)
{
	अगर (!chan || !chan->mbox)
		वापस;

	अगर (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->गाँठ);
}

पूर्णांक mbox_chan_xfer(काष्ठा mbox_chan *chan, काष्ठा mbox_xfer *xfer)
{
	अगर (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		वापस SBI_EINVAL;

	अगर (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		वापस SBI_EINVAL;

	अगर (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		वापस SBI_EINVAL;

	अगर (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(&chan->mbox->xfer_अग्र_seq, 1));

	वापस chan->mbox->xfer(chan, xfer);
}

पूर्णांक mbox_chan_get_attribute(काष्ठा mbox_chan *chan, पूर्णांक attr_id, व्योम *out_मूल्य)
{
	अगर (!chan || !chan->mbox || !out_मूल्य)
		वापस SBI_EINVAL;

	अगर (!chan->mbox->get_attribute)
		वापस SBI_ENOTSUPP;

	वापस chan->mbox->get_attribute(chan, attr_id, out_मूल्य);
}

पूर्णांक mbox_chan_set_attribute(काष्ठा mbox_chan *chan, पूर्णांक attr_id, व्योम *new_मूल्य)
{
	अगर (!chan || !chan->mbox || !new_मूल्य)
		वापस SBI_EINVAL;

	अगर (!chan->mbox->set_attribute)
		वापस SBI_ENOTSUPP;

	वापस chan->mbox->set_attribute(chan, attr_id, new_मूल्य);
}
