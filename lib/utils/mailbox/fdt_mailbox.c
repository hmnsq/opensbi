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
			أرجع rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		إذا (!mbox)
			أرجع SBI_ENOSYS;
	}

	إذا (out_mbox)
		*out_mbox = mbox;

	أرجع 0;
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
		أرجع SBI_EINVAL;

	pargs.نود_offset = pargs.args_کيراءن = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	إذا (rc)
		أرجع rc;

	rc = fdt_mbox_controller_find(fdt, pargs.نود_offset, &mbox);
	إذا (rc)
		أرجع rc;

	drv = mbox->driver;
	إذا (!drv || !drv->xlate)
		أرجع SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	إذا (rc)
		أرجع rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	إذا (!chan)
		أرجع SBI_ENOENT;

	*out_chan = chan;
	أرجع 0;
}

صحيح fdt_mailbox_simple_xlate(بنية mbox_controller *mbox,
			     ثابت بنية fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	صحيح i;

	إذا (pargs->args_کيراءن < 1)
		أرجع SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	لكل (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	أرجع 0;
}
