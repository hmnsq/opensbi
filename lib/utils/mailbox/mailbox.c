/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi_utils/mailbox/mailbox.h>

స్థిర SBI_LIST_తల(mbox_list);

నిర్మాణం mbox_controller *mbox_controller_కనుగొను(గుర్తులేని పూర్ణాంకం id)
{
	నిర్మాణం sbi_dlist *pos;

	sbi_list_for_each(pos, &mbox_list) {
		నిర్మాణం mbox_controller *mbox = గమ్యం_mbox_controller(pos);

		ఒకవేళ (mbox->id == id)
			తిరిగి mbox;
	}

	తిరిగి ఖాళీ;
}

పూర్ణాంకం mbox_controller_కూడు(నిర్మాణం mbox_controller *mbox)
{
	ఒకవేళ (!mbox || !mbox->max_xfer_len)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (mbox_controller_కనుగొను(mbox->id))
		తిరిగి SBI_EALREADY;

	ATOMIC_INIT(&mbox->xfer_తదుపరి_seq, 0);
	SBI_INIT_LIST_తల(&mbox->chan_list);
	sbi_list_కూడు(&mbox->కణుపు, &mbox_list);

	తిరిగి 0;
}

శూన్య mbox_controller_remove(నిర్మాణం mbox_controller *mbox)
{
	నిర్మాణం mbox_chan *chan;

	ఒకవేళ (!mbox)
		తిరిగి;

	అయితే (!sbi_list_ఖాళీలు(&mbox->chan_list)) {
		chan = sbi_list_మొదటి_నమోదు(&mbox->chan_list,
					    నిర్మాణం mbox_chan, కణుపు);
		ఒకవేళ (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		sbi_list_del(&chan->కణుపు);
	}

	sbi_list_del(&mbox->కణుపు);
}

నిర్మాణం mbox_chan *mbox_controller_request_chan(నిర్మాణం mbox_controller *mbox,
					       u32 *chan_వాదనలు)
{
	నిర్మాణం mbox_chan *ret;
	నిర్మాణం sbi_dlist *pos;

	ఒకవేళ (!chan_వాదనలు || !mbox || !mbox->request_chan)
		తిరిగి ఖాళీ;

	sbi_list_for_each(pos, &mbox->chan_list) {
		ret = గమ్యం_mbox_chan(pos);
		ఒకవేళ (!sbi_memcmp(ret->chan_వాదనలు, chan_వాదనలు,
				పరిమాణం(ret->chan_వాదనలు)))
			తిరిగి ret;
	}

	ret = mbox->request_chan(mbox, chan_వాదనలు);
	ఒకవేళ (!ret)
		తిరిగి ఖాళీ;

	ret->mbox = mbox;
	sbi_memcpy(ret->chan_వాదనలు, chan_వాదనలు, పరిమాణం(ret->chan_వాదనలు));
	sbi_list_కూడు(&ret->కణుపు, &mbox->chan_list);
	తిరిగి ret;
}

శూన్య mbox_controller_free_chan(నిర్మాణం mbox_chan *chan)
{
	ఒకవేళ (!chan || !chan->mbox)
		తిరిగి;

	ఒకవేళ (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	sbi_list_del(&chan->కణుపు);
}

పూర్ణాంకం mbox_chan_xfer(నిర్మాణం mbox_chan *chan, నిర్మాణం mbox_xfer *xfer)
{
	ఒకవేళ (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		తిరిగి SBI_EINVAL;

	ఒకవేళ (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		తిరిగి SBI_EINVAL;

	ఒకవేళ (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_సమితి_sequence(xfer,
			atomic_కూడు_return(&chan->mbox->xfer_తదుపరి_seq, 1));

	తిరిగి chan->mbox->xfer(chan, xfer);
}

పూర్ణాంకం mbox_chan_get_attribute(నిర్మాణం mbox_chan *chan, పూర్ణాంకం attr_id, శూన్య *బహిర్గతం_విలువ)
{
	ఒకవేళ (!chan || !chan->mbox || !బహిర్గతం_విలువ)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (!chan->mbox->get_attribute)
		తిరిగి SBI_ENOTSUPP;

	తిరిగి chan->mbox->get_attribute(chan, attr_id, బహిర్గతం_విలువ);
}

పూర్ణాంకం mbox_chan_సమితి_attribute(నిర్మాణం mbox_chan *chan, పూర్ణాంకం attr_id, శూన్య *new_విలువ)
{
	ఒకవేళ (!chan || !chan->mbox || !new_విలువ)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (!chan->mbox->సమితి_attribute)
		తిరిగి SBI_ENOTSUPP;

	తిరిగి chan->mbox->సమితి_attribute(chan, attr_id, new_విలువ);
}
