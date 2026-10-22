/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
বাহ্যিক ধ্রুবক কাঠামো fdt_driver *ধ্রুবক fdt_mailbox_drivers[];

স্থির পূর্ণসংখ্যা fdt_mbox_controller_find(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				    কাঠামো mbox_controller **out_mbox)
{
	পূর্ণসংখ্যা rc;
	কাঠামো mbox_controller *mbox = mbox_controller_find(nodeoff);

	যদি (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		যদি (rc)
			ফেরত rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		যদি (!mbox)
			ফেরত SBI_ENOSYS;
	}

	যদি (out_mbox)
		*out_mbox = mbox;

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_mailbox_request_chan(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff, পূর্ণসংখ্যা সূচক_2,
			     কাঠামো mbox_chan **out_chan)
{
	পূর্ণসংখ্যা rc;
	কাঠামো mbox_chan *chan;
	কাঠামো fdt_mailbox *drv;
	কাঠামো fdt_phandle_args pargs;
	কাঠামো mbox_controller *mbox = ফাঁকা;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	যদি (!fdt || (nodeoff < 0) || (সূচক_2 < 0) || !out_chan)
		ফেরত SBI_EINVAL;

	pargs.গ্রন্থি_offset = pargs.args_গুনতি = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 সূচক_2, &pargs);
	যদি (rc)
		ফেরত rc;

	rc = fdt_mbox_controller_find(fdt, pargs.গ্রন্থি_offset, &mbox);
	যদি (rc)
		ফেরত rc;

	drv = mbox->driver;
	যদি (!drv || !drv->xlate)
		ফেরত SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	যদি (rc)
		ফেরত rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	যদি (!chan)
		ফেরত SBI_ENOENT;

	*out_chan = chan;
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_mailbox_simple_xlate(কাঠামো mbox_controller *mbox,
			     ধ্রুবক কাঠামো fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	পূর্ণসংখ্যা i;

	যদি (pargs->args_গুনতি < 1)
		ফেরত SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	জন্য (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	ফেরত 0;
}
