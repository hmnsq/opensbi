/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
வெளி மாறிலி கட்டமைப்பு fdt_driver *மாறிலி fdt_mailbox_drivers[];

நிலையான முழு fdt_mbox_controller_find(மாறிலி வெறுமை *fdt, முழு nodeoff,
				    கட்டமைப்பு mbox_controller **out_mbox)
{
	முழு rc;
	கட்டமைப்பு mbox_controller *mbox = mbox_controller_find(nodeoff);

	எனில் (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		எனில் (rc)
			திரும்பு rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		எனில் (!mbox)
			திரும்பு SBI_ENOSYS;
	}

	எனில் (out_mbox)
		*out_mbox = mbox;

	திரும்பு 0;
}

முழு fdt_mailbox_request_chan(மாறிலி வெறுமை *fdt, முழு nodeoff, முழு index,
			     கட்டமைப்பு mbox_chan **out_chan)
{
	முழு rc;
	கட்டமைப்பு mbox_chan *chan;
	கட்டமைப்பு fdt_mailbox *drv;
	கட்டமைப்பு fdt_phandle_args pargs;
	கட்டமைப்பு mbox_controller *mbox = வெற்று;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	எனில் (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		திரும்பு SBI_EINVAL;

	pargs.முனை_offset = pargs.args_மொத்தம் = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	எனில் (rc)
		திரும்பு rc;

	rc = fdt_mbox_controller_find(fdt, pargs.முனை_offset, &mbox);
	எனில் (rc)
		திரும்பு rc;

	drv = mbox->driver;
	எனில் (!drv || !drv->xlate)
		திரும்பு SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	எனில் (rc)
		திரும்பு rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	எனில் (!chan)
		திரும்பு SBI_ENOENT;

	*out_chan = chan;
	திரும்பு 0;
}

முழு fdt_mailbox_simple_xlate(கட்டமைப்பு mbox_controller *mbox,
			     மாறிலி கட்டமைப்பு fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	முழு i;

	எனில் (pargs->args_மொத்தம் < 1)
		திரும்பு SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	க்கு (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	திரும்பு 0;
}
