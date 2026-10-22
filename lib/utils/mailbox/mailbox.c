/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi_utils/mailbox/mailbox.h>

स्थिर SBI_LIST_शीर्ष(mbox_list);

संरचना mbox_controller *mbox_controller_find(अचिह्नित पूर्णाङ्क id)
{
	संरचना sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		संरचना mbox_controller *mbox = गन्तव्यम्_mbox_controller(pos);

		यदि (mbox->id == id)
			प्रत्यागम mbox;
	}

	प्रत्यागम रिक्त;
}

पूर्णाङ्क mbox_controller_सङ्कलय(संरचना mbox_controller *mbox)
{
	यदि (!mbox || !mbox->max_xfer_len)
		प्रत्यागम SBI_EINVAL;
	यदि (mbox_controller_find(mbox->id))
		प्रत्यागम SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_अग्र_seq, 0);
	SBI_INIT_LIST_शीर्ष(&mbox->chan_list);
	sbi_list_सङ्कलय(&mbox->ग्रन्थिः, &mbox_list);

	प्रत्यागम 0;
}

शून्य mbox_controller_remove(संरचना mbox_controller *mbox)
{
	संरचना mbox_chan *chan;

	यदि (!mbox)
		प्रत्यागम;

	यावत् (!sbi_list_रिक्तानि(&mbox->chan_list)) {
		chan = sbi_list_first_प्रविष्टिः(&mbox->chan_list,
					    संरचना mbox_chan, ग्रन्थिः);
		यदि (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->ग्रन्थिः);
	}

	sbi_list_del(&mbox->ग्रन्थिः);
}

संरचना mbox_chan *mbox_controller_request_chan(संरचना mbox_controller *mbox,
					       u32 *chan_तर्काः)
{
	संरचना mbox_chan *ret;
	संरचना sbi_dlist *pos;

	यदि (!chan_तर्काः || !mbox || !mbox->request_chan)
		प्रत्यागम रिक्त;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = गन्तव्यम्_mbox_chan(pos);
		यदि (!sbi_memcmp(ret->chan_तर्काः, chan_तर्काः,
				परिमाण(ret->chan_तर्काः)))
			प्रत्यागम ret;
	}

	ret = mbox->request_chan(mbox, chan_तर्काः);
	यदि (!ret)
		प्रत्यागम रिक्त;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_तर्काः, chan_तर्काः, परिमाण(ret->chan_तर्काः));
	sbi_list_सङ्कलय(&ret->ग्रन्थिः, &mbox->chan_list);
	प्रत्यागम ret;
}

शून्य mbox_controller_free_chan(संरचना mbox_chan *chan)
{
	यदि (!chan || !chan->mbox)
		प्रत्यागम;

	यदि (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->ग्रन्थिः);
}

पूर्णाङ्क mbox_chan_xfer(संरचना mbox_chan *chan, संरचना mbox_xfer *xfer)
{
	यदि (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		प्रत्यागम SBI_EINVAL;

	यदि (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		प्रत्यागम SBI_EINVAL;

	यदि (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		प्रत्यागम SBI_EINVAL;

	यदि (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_समुच्चयः_sequence(xfer,
			atomic_सङ्कलय_return(&chan->mbox->xfer_अग्र_seq, 1));

	प्रत्यागम chan->mbox->xfer(chan, xfer);
}

पूर्णाङ्क mbox_chan_get_attribute(संरचना mbox_chan *chan, पूर्णाङ्क attr_id, शून्य *बहिर्गमनम्_मानम्)
{
	यदि (!chan || !chan->mbox || !बहिर्गमनम्_मानम्)
		प्रत्यागम SBI_EINVAL;

	यदि (!chan->mbox->get_attribute)
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम chan->mbox->get_attribute(chan, attr_id, बहिर्गमनम्_मानम्);
}

पूर्णाङ्क mbox_chan_समुच्चयः_attribute(संरचना mbox_chan *chan, पूर्णाङ्क attr_id, शून्य *new_मानम्)
{
	यदि (!chan || !chan->mbox || !new_मानम्)
		प्रत्यागम SBI_EINVAL;

	यदि (!chan->mbox->समुच्चयः_attribute)
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम chan->mbox->समुच्चयः_attribute(chan, attr_id, new_मानम्);
}
