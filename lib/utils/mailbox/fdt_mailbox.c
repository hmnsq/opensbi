/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
خارجی ثابت ساخت fdt_driver *ثابت fdt_mailbox_drivers[];

ساکن صحیح fdt_mbox_controller_ڈھونڈو(ثابت خالی *fdt, صحیح nodeoff,
				    ساخت mbox_controller **برآمد_mbox)
{
	صحیح rc;
	ساخت mbox_controller *mbox = mbox_controller_ڈھونڈو(nodeoff);

	اگر (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_انحراف(fdt, nodeoff, fdt_mailbox_drivers);
		اگر (rc)
			واپس rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_ڈھونڈو(nodeoff);
		اگر (!mbox)
			واپس SBI_ENOSYS;
	}

	اگر (برآمد_mbox)
		*برآمد_mbox = mbox;

	واپس 0;
}

صحیح fdt_mailbox_request_chan(ثابت خالی *fdt, صحیح nodeoff, صحیح اشاریہ_2,
			     ساخت mbox_chan **برآمد_chan)
{
	صحیح rc;
	ساخت mbox_chan *chan;
	ساخت fdt_mailbox *drv;
	ساخت fdt_phandle_دلائل pargs;
	ساخت mbox_controller *mbox = تہی;
	u32 chan_دلائل[MBOX_CHAN_MAX_دلائل];

	اگر (!fdt || (nodeoff < 0) || (اشاریہ_2 < 0) || !برآمد_chan)
		واپس SBI_EINVAL;

	pargs.گرہ_انحراف = pargs.دلائل_گنتی = 0;
	rc = fdt_parse_phandle_with_دلائل(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 اشاریہ_2, &pargs);
	اگر (rc)
		واپس rc;

	rc = fdt_mbox_controller_ڈھونڈو(fdt, pargs.گرہ_انحراف, &mbox);
	اگر (rc)
		واپس rc;

	drv = mbox->driver;
	اگر (!drv || !drv->xlate)
		واپس SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_دلائل);
	اگر (rc)
		واپس rc;

	chan = mbox_controller_request_chan(mbox, chan_دلائل);
	اگر (!chan)
		واپس SBI_ENOENT;

	*برآمد_chan = chan;
	واپس 0;
}

صحیح fdt_mailbox_simple_xlate(ساخت mbox_controller *mbox,
			     ثابت ساخت fdt_phandle_دلائل *pargs,
			     u32 *برآمد_chan_دلائل)
{
	صحیح i;

	اگر (pargs->دلائل_گنتی < 1)
		واپس SBI_EINVAL;

	برآمد_chan_دلائل[0] = pargs->دلائل[0];
	برائے (i = 1; i < MBOX_CHAN_MAX_دلائل; i++)
		برآمد_chan_دلائل[i] = 0;

	واپس 0;
}
