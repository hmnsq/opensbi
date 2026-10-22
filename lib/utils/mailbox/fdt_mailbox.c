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

	ᱡᱩᱫᱤ (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		ᱡᱩᱫᱤ (rc)
			ᱨᱩᱣᱟᱹᱲ rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		ᱡᱩᱫᱤ (!mbox)
			ᱨᱩᱣᱟᱹᱲ SBI_ENOSYS;
	}

	ᱡᱩᱫᱤ (out_mbox)
		*out_mbox = mbox;

	ᱨᱩᱣᱟᱹᱲ 0;
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

	ᱡᱩᱫᱤ (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	ᱡᱩᱫᱤ (rc)
		ᱨᱩᱣᱟᱹᱲ rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	ᱡᱩᱫᱤ (rc)
		ᱨᱩᱣᱟᱹᱲ rc;

	drv = mbox->driver;
	ᱡᱩᱫᱤ (!drv || !drv->xlate)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	ᱡᱩᱫᱤ (rc)
		ᱨᱩᱣᱟᱹᱲ rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	ᱡᱩᱫᱤ (!chan)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOENT;

	*out_chan = chan;
	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_mailbox_simple_xlate(sa_nracanaa mbox_controller *mbox,
			     const sa_nracanaa fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	puur_naa_nka i;

	ᱡᱩᱫᱤ (pargs->args_count < 1)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	for (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	ᱨᱩᱣᱟᱹᱲ 0;
}
