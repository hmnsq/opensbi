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

ساكن صحيح fdt_mbox_controller_پیدا_کړه(ثابت فراغ *fdt, صحيح nodeoff,
				    بنية mbox_controller **بهر_mbox)
{
	صحيح rc;
	بنية mbox_controller *mbox = mbox_controller_پیدا_کړه(nodeoff);

	إذا (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_دننه_واټن(fdt, nodeoff, fdt_mailbox_drivers);
		إذا (rc)
			أرجع rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_پیدا_کړه(nodeoff);
		إذا (!mbox)
			أرجع SBI_ENOSYS;
	}

	إذا (بهر_mbox)
		*بهر_mbox = mbox;

	أرجع 0;
}

صحيح fdt_mailbox_request_chan(ثابت فراغ *fdt, صحيح nodeoff, صحيح شاخص_2,
			     بنية mbox_chan **بهر_chan)
{
	صحيح rc;
	بنية mbox_chan *chan;
	بنية fdt_mailbox *drv;
	بنية fdt_phandle_دلیلونه pargs;
	بنية mbox_controller *mbox = عدم;
	u32 chan_دلیلونه[MBOX_CHAN_MAX_دلیلونه];

	إذا (!fdt || (nodeoff < 0) || (شاخص_2 < 0) || !بهر_chan)
		أرجع SBI_EINVAL;

	pargs.غوټه_دننه_واټن = pargs.دلیلونه_شمېر = 0;
	rc = fdt_parse_phandle_with_دلیلونه(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 شاخص_2, &pargs);
	إذا (rc)
		أرجع rc;

	rc = fdt_mbox_controller_پیدا_کړه(fdt, pargs.غوټه_دننه_واټن, &mbox);
	إذا (rc)
		أرجع rc;

	drv = mbox->driver;
	إذا (!drv || !drv->xlate)
		أرجع SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_دلیلونه);
	إذا (rc)
		أرجع rc;

	chan = mbox_controller_request_chan(mbox, chan_دلیلونه);
	إذا (!chan)
		أرجع SBI_ENOENT;

	*بهر_chan = chan;
	أرجع 0;
}

صحيح fdt_mailbox_simple_xlate(بنية mbox_controller *mbox,
			     ثابت بنية fdt_phandle_دلیلونه *pargs,
			     u32 *بهر_chan_دلیلونه)
{
	صحيح i;

	إذا (pargs->دلیلونه_شمېر < 1)
		أرجع SBI_EINVAL;

	بهر_chan_دلیلونه[0] = pargs->دلیلونه[0];
	لكل (i = 1; i < MBOX_CHAN_MAX_دلیلونه; i++)
		بهر_chan_دلیلونه[i] = 0;

	أرجع 0;
}
