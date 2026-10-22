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

	ꦪꦼꦤ꧀ (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		ꦪꦼꦤ꧀ (rc)
			ꦮꦭꦶ rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		ꦪꦼꦤ꧀ (!mbox)
			ꦮꦭꦶ SBI_ENOSYS;
	}

	ꦪꦼꦤ꧀ (out_mbox)
		*out_mbox = mbox;

	ꦮꦭꦶ 0;
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

	ꦪꦼꦤ꧀ (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		ꦮꦭꦶ SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	ꦪꦼꦤ꧀ (rc)
		ꦮꦭꦶ rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	ꦪꦼꦤ꧀ (rc)
		ꦮꦭꦶ rc;

	drv = mbox->driver;
	ꦪꦼꦤ꧀ (!drv || !drv->xlate)
		ꦮꦭꦶ SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	ꦪꦼꦤ꧀ (rc)
		ꦮꦭꦶ rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	ꦪꦼꦤ꧀ (!chan)
		ꦮꦭꦶ SBI_ENOENT;

	*out_chan = chan;
	ꦮꦭꦶ 0;
}

puur_naa_nka fdt_mailbox_simple_xlate(sa_nracanaa mbox_controller *mbox,
			     const sa_nracanaa fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	puur_naa_nka i;

	ꦪꦼꦤ꧀ (pargs->args_count < 1)
		ꦮꦭꦶ SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	ꦏꦁꦒꦺꦴ (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	ꦮꦭꦶ 0;
}
