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
extern const struct fdt_driver *const fdt_mailbox_drivers[];

static цео fdt_mbox_controller_find(const void *fdt, цео nodeoff,
				    struct mbox_controller **out_mbox)
{
	цео rc;
	struct mbox_controller *mbox = mbox_controller_find(nodeoff);

	ако (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		ако (rc)
			врати rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		ако (!mbox)
			врати SBI_ENOSYS;
	}

	ако (out_mbox)
		*out_mbox = mbox;

	врати 0;
}

цео fdt_mailbox_request_chan(const void *fdt, цео nodeoff, цео index,
			     struct mbox_chan **out_chan)
{
	цео rc;
	struct mbox_chan *chan;
	struct fdt_mailbox *drv;
	struct fdt_phandle_args pargs;
	struct mbox_controller *mbox = NULL;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	ако (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		врати SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	ако (rc)
		врати rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	ако (rc)
		врати rc;

	drv = mbox->driver;
	ако (!drv || !drv->xlate)
		врати SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	ако (rc)
		врати rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	ако (!chan)
		врати SBI_ENOENT;

	*out_chan = chan;
	врати 0;
}

цео fdt_mailbox_simple_xlate(struct mbox_controller *mbox,
			     const struct fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	цео i;

	ако (pargs->args_count < 1)
		врати SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	за (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	врати 0;
}
