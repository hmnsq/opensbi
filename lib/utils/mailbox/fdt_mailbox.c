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

ساكن صحيح fdt_mbox_controller_لبھو(ثابت فراغ *fdt, صحيح nodeoff,
				    بنية mbox_controller **باہرلا_mbox)
{
	صحيح rc;
	بنية mbox_controller *mbox = mbox_controller_لبھو(nodeoff);

	إذا (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_وتھ(fdt, nodeoff, fdt_mailbox_drivers);
		إذا (rc)
			أرجع rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_لبھو(nodeoff);
		إذا (!mbox)
			أرجع SBI_ENOSYS;
	}

	إذا (باہرلا_mbox)
		*باہرلا_mbox = mbox;

	أرجع 0;
}

صحيح fdt_mailbox_request_chan(ثابت فراغ *fdt, صحيح nodeoff, صحيح اشاریہ_2,
			     بنية mbox_chan **باہرلا_chan)
{
	صحيح rc;
	بنية mbox_chan *chan;
	بنية fdt_mailbox *drv;
	بنية fdt_phandle_دلیلاں pargs;
	بنية mbox_controller *mbox = عدم;
	u32 chan_دلیلاں[MBOX_CHAN_MAX_دلیلاں];

	إذا (!fdt || (nodeoff < 0) || (اشاریہ_2 < 0) || !باہرلا_chan)
		أرجع SBI_EINVAL;

	pargs.گرہ_وتھ = pargs.دلیلاں_گنتی = 0;
	rc = fdt_parse_phandle_with_دلیلاں(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 اشاریہ_2, &pargs);
	إذا (rc)
		أرجع rc;

	rc = fdt_mbox_controller_لبھو(fdt, pargs.گرہ_وتھ, &mbox);
	إذا (rc)
		أرجع rc;

	drv = mbox->driver;
	إذا (!drv || !drv->xlate)
		أرجع SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_دلیلاں);
	إذا (rc)
		أرجع rc;

	chan = mbox_controller_request_chan(mbox, chan_دلیلاں);
	إذا (!chan)
		أرجع SBI_ENOENT;

	*باہرلا_chan = chan;
	أرجع 0;
}

صحيح fdt_mailbox_simple_xlate(بنية mbox_controller *mbox,
			     ثابت بنية fdt_phandle_دلیلاں *pargs,
			     u32 *باہرلا_chan_دلیلاں)
{
	صحيح i;

	إذا (pargs->دلیلاں_گنتی < 1)
		أرجع SBI_EINVAL;

	باہرلا_chan_دلیلاں[0] = pargs->دلیلاں[0];
	لكل (i = 1; i < MBOX_CHAN_MAX_دلیلاں; i++)
		باہرلا_chan_دلیلاں[i] = 0;

	أرجع 0;
}
