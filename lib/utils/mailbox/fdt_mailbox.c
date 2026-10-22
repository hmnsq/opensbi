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

ساكن صحيح fdt_mbox_controller_پیدا_کن(ثابت فراغ *fdt, صحيح nodeoff,
				    بنية mbox_controller **بیرون_mbox)
{
	صحيح rc;
	بنية mbox_controller *mbox = mbox_controller_پیدا_کن(nodeoff);

	إذا (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_جابه_جایی(fdt, nodeoff, fdt_mailbox_drivers);
		إذا (rc)
			أرجع rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_پیدا_کن(nodeoff);
		إذا (!mbox)
			أرجع SBI_ENOSYS;
	}

	إذا (بیرون_mbox)
		*بیرون_mbox = mbox;

	أرجع 0;
}

صحيح fdt_mailbox_request_chan(ثابت فراغ *fdt, صحيح nodeoff, صحيح شاخص_2,
			     بنية mbox_chan **بیرون_chan)
{
	صحيح rc;
	بنية mbox_chan *chan;
	بنية fdt_mailbox *drv;
	بنية fdt_phandle_آرگومانها pargs;
	بنية mbox_controller *mbox = عدم;
	u32 chan_آرگومانها[MBOX_CHAN_MAX_آرگومانها];

	إذا (!fdt || (nodeoff < 0) || (شاخص_2 < 0) || !بیرون_chan)
		أرجع SBI_EINVAL;

	pargs.گره_جابه_جایی = pargs.آرگومانها_شمار = 0;
	rc = fdt_parse_phandle_with_آرگومانها(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 شاخص_2, &pargs);
	إذا (rc)
		أرجع rc;

	rc = fdt_mbox_controller_پیدا_کن(fdt, pargs.گره_جابه_جایی, &mbox);
	إذا (rc)
		أرجع rc;

	drv = mbox->driver;
	إذا (!drv || !drv->xlate)
		أرجع SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_آرگومانها);
	إذا (rc)
		أرجع rc;

	chan = mbox_controller_request_chan(mbox, chan_آرگومانها);
	إذا (!chan)
		أرجع SBI_ENOENT;

	*بیرون_chan = chan;
	أرجع 0;
}

صحيح fdt_mailbox_simple_xlate(بنية mbox_controller *mbox,
			     ثابت بنية fdt_phandle_آرگومانها *pargs,
			     u32 *بیرون_chan_آرگومانها)
{
	صحيح i;

	إذا (pargs->آرگومانها_شمار < 1)
		أرجع SBI_EINVAL;

	بیرون_chan_آرگومانها[0] = pargs->آرگومانها[0];
	لكل (i = 1; i < MBOX_CHAN_MAX_آرگومانها; i++)
		بیرون_chan_آرگومانها[i] = 0;

	أرجع 0;
}
