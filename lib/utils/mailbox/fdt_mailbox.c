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

ساكن صحيح fdt_mbox_controller_ڳوليو(ثابت فراغ *fdt, صحيح nodeoff,
				    بنية mbox_controller **ٻاهريون_mbox)
{
	صحيح rc;
	بنية mbox_controller *mbox = mbox_controller_ڳوليو(nodeoff);

	إذا (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_وٿي(fdt, nodeoff, fdt_mailbox_drivers);
		إذا (rc)
			أرجع rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_ڳوليو(nodeoff);
		إذا (!mbox)
			أرجع SBI_ENOSYS;
	}

	إذا (ٻاهريون_mbox)
		*ٻاهريون_mbox = mbox;

	أرجع 0;
}

صحيح fdt_mailbox_request_chan(ثابت فراغ *fdt, صحيح nodeoff, صحيح اشاريو_2,
			     بنية mbox_chan **ٻاهريون_chan)
{
	صحيح rc;
	بنية mbox_chan *chan;
	بنية fdt_mailbox *drv;
	بنية fdt_phandle_دليلون pargs;
	بنية mbox_controller *mbox = عدم;
	u32 chan_دليلون[MBOX_CHAN_MAX_دليلون];

	إذا (!fdt || (nodeoff < 0) || (اشاريو_2 < 0) || !ٻاهريون_chan)
		أرجع SBI_EINVAL;

	pargs.ڳنڍ_وٿي = pargs.دليلون_ڳڻپ = 0;
	rc = fdt_parse_phandle_with_دليلون(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 اشاريو_2, &pargs);
	إذا (rc)
		أرجع rc;

	rc = fdt_mbox_controller_ڳوليو(fdt, pargs.ڳنڍ_وٿي, &mbox);
	إذا (rc)
		أرجع rc;

	drv = mbox->driver;
	إذا (!drv || !drv->xlate)
		أرجع SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_دليلون);
	إذا (rc)
		أرجع rc;

	chan = mbox_controller_request_chan(mbox, chan_دليلون);
	إذا (!chan)
		أرجع SBI_ENOENT;

	*ٻاهريون_chan = chan;
	أرجع 0;
}

صحيح fdt_mailbox_simple_xlate(بنية mbox_controller *mbox,
			     ثابت بنية fdt_phandle_دليلون *pargs,
			     u32 *ٻاهريون_chan_دليلون)
{
	صحيح i;

	إذا (pargs->دليلون_ڳڻپ < 1)
		أرجع SBI_EINVAL;

	ٻاهريون_chan_دليلون[0] = pargs->دليلون[0];
	لكل (i = 1; i < MBOX_CHAN_MAX_دليلون; i++)
		ٻاهريون_chan_دليلون[i] = 0;

	أرجع 0;
}
