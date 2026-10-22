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
বাহ্য ধ্রুব সংরচনা fdt_driver *ধ্রুব fdt_mailbox_drivers[];

স্থির পূর্ণাঙ্ক fdt_mbox_controller_find(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				    সংরচনা mbox_controller **out_mbox)
{
	পূর্ণাঙ্ক rc;
	সংরচনা mbox_controller *mbox = mbox_controller_find(nodeoff);

	যদি (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		যদি (rc)
			প্রত্যাগম rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		যদি (!mbox)
			প্রত্যাগম SBI_ENOSYS;
	}

	যদি (out_mbox)
		*out_mbox = mbox;

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_mailbox_request_chan(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff, পূর্ণাঙ্ক index,
			     সংরচনা mbox_chan **out_chan)
{
	পূর্ণাঙ্ক rc;
	সংরচনা mbox_chan *chan;
	সংরচনা fdt_mailbox *drv;
	সংরচনা fdt_phandle_args pargs;
	সংরচনা mbox_controller *mbox = রিক্ত;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	যদি (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		প্রত্যাগম SBI_EINVAL;

	pargs.গ্রন্থি_offset = pargs.args_গুনতি = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	যদি (rc)
		প্রত্যাগম rc;

	rc = fdt_mbox_controller_find(fdt, pargs.গ্রন্থি_offset, &mbox);
	যদি (rc)
		প্রত্যাগম rc;

	drv = mbox->driver;
	যদি (!drv || !drv->xlate)
		প্রত্যাগম SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	যদি (rc)
		প্রত্যাগম rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	যদি (!chan)
		প্রত্যাগম SBI_ENOENT;

	*out_chan = chan;
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_mailbox_simple_xlate(সংরচনা mbox_controller *mbox,
			     ধ্রুব সংরচনা fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	পূর্ণাঙ্ক i;

	যদি (pargs->args_গুনতি < 1)
		প্রত্যাগম SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	প্রতি (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	প্রত্যাগম 0;
}
