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

static 정수 fdt_mbox_controller_find(const void *fdt, 정수 nodeoff,
				    struct mbox_controller **out_mbox)
{
	정수 rc;
	struct mbox_controller *mbox = mbox_controller_find(nodeoff);

	만약 (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		만약 (rc)
			반환 rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		만약 (!mbox)
			반환 SBI_ENOSYS;
	}

	만약 (out_mbox)
		*out_mbox = mbox;

	반환 0;
}

정수 fdt_mailbox_request_chan(const void *fdt, 정수 nodeoff, 정수 index,
			     struct mbox_chan **out_chan)
{
	정수 rc;
	struct mbox_chan *chan;
	struct fdt_mailbox *drv;
	struct fdt_phandle_args pargs;
	struct mbox_controller *mbox = NULL;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	만약 (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		반환 SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	만약 (rc)
		반환 rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	만약 (rc)
		반환 rc;

	drv = mbox->driver;
	만약 (!drv || !drv->xlate)
		반환 SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	만약 (rc)
		반환 rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	만약 (!chan)
		반환 SBI_ENOENT;

	*out_chan = chan;
	반환 0;
}

정수 fdt_mailbox_simple_xlate(struct mbox_controller *mbox,
			     const struct fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	정수 i;

	만약 (pargs->args_count < 1)
		반환 SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	위해 (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	반환 0;
}
