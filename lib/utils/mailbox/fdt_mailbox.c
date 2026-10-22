/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <libfdt.h>
#include <sbi/sbi_error.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
extern const sa_nracanaa fdt_driver *const fdt_mailbox_drivers[];

static ཧྲིལ fdt_mbox_controller_find(const void *fdt, ཧྲིལ nodeoff,
				    sa_nracanaa mbox_controller **out_mbox)
{
	ཧྲིལ rc;
	sa_nracanaa mbox_controller *mbox = mbox_controller_find(nodeoff);

	གལ་ཏེ (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		གལ་ཏེ (rc)
			ཕྱིར rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		གལ་ཏེ (!mbox)
			ཕྱིར SBI_ENOSYS;
	}

	གལ་ཏེ (out_mbox)
		*out_mbox = mbox;

	ཕྱིར 0;
}

ཧྲིལ fdt_mailbox_request_chan(const void *fdt, ཧྲིལ nodeoff, ཧྲིལ index,
			     sa_nracanaa mbox_chan **out_chan)
{
	ཧྲིལ rc;
	sa_nracanaa mbox_chan *chan;
	sa_nracanaa fdt_mailbox *drv;
	sa_nracanaa fdt_phandle_args pargs;
	sa_nracanaa mbox_controller *mbox = NULL;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	གལ་ཏེ (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		ཕྱིར SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	གལ་ཏེ (rc)
		ཕྱིར rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	གལ་ཏེ (rc)
		ཕྱིར rc;

	drv = mbox->driver;
	གལ་ཏེ (!drv || !drv->xlate)
		ཕྱིར SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	གལ་ཏེ (rc)
		ཕྱིར rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	གལ་ཏེ (!chan)
		ཕྱིར SBI_ENOENT;

	*out_chan = chan;
	ཕྱིར 0;
}

ཧྲིལ fdt_mailbox_simple_xlate(sa_nracanaa mbox_controller *mbox,
			     const sa_nracanaa fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	ཧྲིལ i;

	གལ་ཏེ (pargs->args_count < 1)
		ཕྱིར SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	ཆེད་དུ (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	ཕྱིར 0;
}
