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

static целое fdt_mbox_controller_find(const void *fdt, целое nodeoff,
				    struct mbox_controller **out_mbox)
{
	целое rc;
	struct mbox_controller *mbox = mbox_controller_find(nodeoff);

	если (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		если (rc)
			вернуть rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		если (!mbox)
			вернуть SBI_ENOSYS;
	}

	если (out_mbox)
		*out_mbox = mbox;

	вернуть 0;
}

целое fdt_mailbox_request_chan(const void *fdt, целое nodeoff, целое index,
			     struct mbox_chan **out_chan)
{
	целое rc;
	struct mbox_chan *chan;
	struct fdt_mailbox *drv;
	struct fdt_phandle_args pargs;
	struct mbox_controller *mbox = NULL;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	если (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		вернуть SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	если (rc)
		вернуть rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	если (rc)
		вернуть rc;

	drv = mbox->driver;
	если (!drv || !drv->xlate)
		вернуть SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	если (rc)
		вернуть rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	если (!chan)
		вернуть SBI_ENOENT;

	*out_chan = chan;
	вернуть 0;
}

целое fdt_mailbox_simple_xlate(struct mbox_controller *mbox,
			     const struct fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	целое i;

	если (pargs->args_count < 1)
		вернуть SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	для (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	вернуть 0;
}
