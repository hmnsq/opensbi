/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
बाह्य स्थिरांक संरचना fdt_driver *स्थिरांक fdt_mailbox_drivers[];

स्थिर पूर्णांक fdt_mbox_controller_find(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
				    संरचना mbox_controller **out_mbox)
{
	पूर्णांक rc;
	संरचना mbox_controller *mbox = mbox_controller_find(nodeoff);

	जर (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		जर (rc)
			परत rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		जर (!mbox)
			परत SBI_ENOSYS;
	}

	जर (out_mbox)
		*out_mbox = mbox;

	परत 0;
}

पूर्णांक fdt_mailbox_request_chan(स्थिरांक शून्य *fdt, पूर्णांक nodeoff, पूर्णांक index,
			     संरचना mbox_chan **out_chan)
{
	पूर्णांक rc;
	संरचना mbox_chan *chan;
	संरचना fdt_mailbox *drv;
	संरचना fdt_phandle_args pargs;
	संरचना mbox_controller *mbox = रिक्त;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	जर (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		परत SBI_EINVAL;

	pargs.गाठ_offset = pargs.args_मोजणी = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	जर (rc)
		परत rc;

	rc = fdt_mbox_controller_find(fdt, pargs.गाठ_offset, &mbox);
	जर (rc)
		परत rc;

	drv = mbox->driver;
	जर (!drv || !drv->xlate)
		परत SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	जर (rc)
		परत rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	जर (!chan)
		परत SBI_ENOENT;

	*out_chan = chan;
	परत 0;
}

पूर्णांक fdt_mailbox_simple_xlate(संरचना mbox_controller *mbox,
			     स्थिरांक संरचना fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	पूर्णांक i;

	जर (pargs->args_मोजणी < 1)
		परत SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	साठी (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	परत 0;
}
