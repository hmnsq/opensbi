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

static số_nguyên fdt_mbox_controller_find(const void *fdt, số_nguyên nodeoff,
				    struct mbox_controller **out_mbox)
{
	số_nguyên rc;
	struct mbox_controller *mbox = mbox_controller_find(nodeoff);

	nếu (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		nếu (rc)
			trả_về rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		nếu (!mbox)
			trả_về SBI_ENOSYS;
	}

	nếu (out_mbox)
		*out_mbox = mbox;

	trả_về 0;
}

số_nguyên fdt_mailbox_request_chan(const void *fdt, số_nguyên nodeoff, số_nguyên index,
			     struct mbox_chan **out_chan)
{
	số_nguyên rc;
	struct mbox_chan *chan;
	struct fdt_mailbox *drv;
	struct fdt_phandle_args pargs;
	struct mbox_controller *mbox = NULL;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	nếu (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		trả_về SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	nếu (rc)
		trả_về rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	nếu (rc)
		trả_về rc;

	drv = mbox->driver;
	nếu (!drv || !drv->xlate)
		trả_về SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	nếu (rc)
		trả_về rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	nếu (!chan)
		trả_về SBI_ENOENT;

	*out_chan = chan;
	trả_về 0;
}

số_nguyên fdt_mailbox_simple_xlate(struct mbox_controller *mbox,
			     const struct fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	số_nguyên i;

	nếu (pargs->args_count < 1)
		trả_về SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	cho (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	trả_về 0;
}
