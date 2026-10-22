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
بیرونی ثابت ساختار fdt_driver *ثابت fdt_mailbox_drivers[];

ایستا صحیح fdt_mbox_controller_find(ثابت تهی *fdt, صحیح nodeoff,
				    ساختار mbox_controller **out_mbox)
{
	صحیح rc;
	ساختار mbox_controller *mbox = mbox_controller_find(nodeoff);

	اگر (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		اگر (rc)
			بازگشت rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		اگر (!mbox)
			بازگشت SBI_ENOSYS;
	}

	اگر (out_mbox)
		*out_mbox = mbox;

	بازگشت 0;
}

صحیح fdt_mailbox_request_chan(ثابت تهی *fdt, صحیح nodeoff, صحیح index,
			     ساختار mbox_chan **out_chan)
{
	صحیح rc;
	ساختار mbox_chan *chan;
	ساختار fdt_mailbox *drv;
	ساختار fdt_phandle_args pargs;
	ساختار mbox_controller *mbox = پوچ;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	اگر (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		بازگشت SBI_EINVAL;

	pargs.گره_offset = pargs.args_شمار = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	اگر (rc)
		بازگشت rc;

	rc = fdt_mbox_controller_find(fdt, pargs.گره_offset, &mbox);
	اگر (rc)
		بازگشت rc;

	drv = mbox->driver;
	اگر (!drv || !drv->xlate)
		بازگشت SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	اگر (rc)
		بازگشت rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	اگر (!chan)
		بازگشت SBI_ENOENT;

	*out_chan = chan;
	بازگشت 0;
}

صحیح fdt_mailbox_simple_xlate(ساختار mbox_controller *mbox,
			     ثابت ساختار fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	صحیح i;

	اگر (pargs->args_شمار < 1)
		بازگشت SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	برای (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	بازگشت 0;
}
