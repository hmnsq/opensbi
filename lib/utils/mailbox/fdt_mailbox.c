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

static puur_naa_nka fdt_mbox_controller_find(const void *fdt, puur_naa_nka nodeoff,
				    sa_nracanaa mbox_controller **out_mbox)
{
	puur_naa_nka rc;
	sa_nracanaa mbox_controller *mbox = mbox_controller_find(nodeoff);

	ᬬᬾᬦ᭄ (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		ᬬᬾᬦ᭄ (rc)
			ᬯᬮᬶ rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		ᬬᬾᬦ᭄ (!mbox)
			ᬯᬮᬶ SBI_ENOSYS;
	}

	ᬬᬾᬦ᭄ (out_mbox)
		*out_mbox = mbox;

	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_mailbox_request_chan(const void *fdt, puur_naa_nka nodeoff, puur_naa_nka index,
			     sa_nracanaa mbox_chan **out_chan)
{
	puur_naa_nka rc;
	sa_nracanaa mbox_chan *chan;
	sa_nracanaa fdt_mailbox *drv;
	sa_nracanaa fdt_phandle_args pargs;
	sa_nracanaa mbox_controller *mbox = NULL;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	ᬬᬾᬦ᭄ (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		ᬯᬮᬶ SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	ᬬᬾᬦ᭄ (rc)
		ᬯᬮᬶ rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	ᬬᬾᬦ᭄ (rc)
		ᬯᬮᬶ rc;

	drv = mbox->driver;
	ᬬᬾᬦ᭄ (!drv || !drv->xlate)
		ᬯᬮᬶ SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	ᬬᬾᬦ᭄ (rc)
		ᬯᬮᬶ rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	ᬬᬾᬦ᭄ (!chan)
		ᬯᬮᬶ SBI_ENOENT;

	*out_chan = chan;
	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_mailbox_simple_xlate(sa_nracanaa mbox_controller *mbox,
			     const sa_nracanaa fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	puur_naa_nka i;

	ᬬᬾᬦ᭄ (pargs->args_count < 1)
		ᬯᬮᬶ SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	for (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	ᬯᬮᬶ 0;
}
