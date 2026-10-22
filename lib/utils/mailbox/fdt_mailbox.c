/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
خارجي ثابت بنية fdt_driver *ثابت fdt_mailbox_drivers[];

ساكن صحيح fdt_mbox_controller_find(ثابت فراغ *fdt, صحيح nodeoff,
				    بنية mbox_controller **out_mbox)
{
	صحيح rc;
	بنية mbox_controller *mbox = mbox_controller_find(nodeoff);

	إذا (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		إذا (rc)
			إرجاع rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		إذا (!mbox)
			إرجاع SBI_ENOSYS;
	}

	إذا (out_mbox)
		*out_mbox = mbox;

	إرجاع 0;
}

صحيح fdt_mailbox_request_chan(ثابت فراغ *fdt, صحيح nodeoff, صحيح index,
			     بنية mbox_chan **out_chan)
{
	صحيح rc;
	بنية mbox_chan *chan;
	بنية fdt_mailbox *drv;
	بنية fdt_phandle_args pargs;
	بنية mbox_controller *mbox = عدم;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	إذا (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		إرجاع SBI_EINVAL;

	pargs.عقدة_offset = pargs.args_تعداد = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	إذا (rc)
		إرجاع rc;

	rc = fdt_mbox_controller_find(fdt, pargs.عقدة_offset, &mbox);
	إذا (rc)
		إرجاع rc;

	drv = mbox->driver;
	إذا (!drv || !drv->xlate)
		إرجاع SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	إذا (rc)
		إرجاع rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	إذا (!chan)
		إرجاع SBI_ENOENT;

	*out_chan = chan;
	إرجاع 0;
}

صحيح fdt_mailbox_simple_xlate(بنية mbox_controller *mbox,
			     ثابت بنية fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	صحيح i;

	إذا (pargs->args_تعداد < 1)
		إرجاع SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	لكل (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	إرجاع 0;
}
