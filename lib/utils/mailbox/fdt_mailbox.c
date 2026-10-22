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
	بنية fdt_phandle_وسائط pargs;
	بنية mbox_controller *mbox = عدم;
	u32 chan_وسائط[MBOX_CHAN_MAX_وسائط];

	إذا (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		أرجع SBI_EINVAL;

	pargs.عقدة_offset = pargs.وسائط_عدد = 0;
	rc = fdt_parse_phandle_with_وسائط(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	إذا (rc)
		أرجع rc;

	rc = fdt_mbox_controller_find(fdt, pargs.عقدة_offset, &mbox);
	إذا (rc)
		أرجع rc;

	drv = mbox->driver;
	إذا (!drv || !drv->xlate)
		أرجع SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_وسائط);
	إذا (rc)
		أرجع rc;

	chan = mbox_controller_request_chan(mbox, chan_وسائط);
	إذا (!chan)
		أرجع SBI_ENOENT;

	*out_chan = chan;
	أرجع 0;
}

صحيح fdt_mailbox_simple_xlate(بنية mbox_controller *mbox,
			     ثابت بنية fdt_phandle_وسائط *pargs,
			     u32 *out_chan_وسائط)
{
	صحيح i;

	إذا (pargs->وسائط_عدد < 1)
		أرجع SBI_EINVAL;

	out_chan_وسائط[0] = pargs->وسائط[0];
	لكل (i = 1; i < MBOX_CHAN_MAX_وسائط; i++)
		out_chan_وسائط[i] = 0;

	أرجع 0;
}
