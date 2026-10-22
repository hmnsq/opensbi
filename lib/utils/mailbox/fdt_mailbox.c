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

static 整数 fdt_mbox_controller_find(const void *fdt, 整数 nodeoff,
				    struct mbox_controller **out_mbox)
{
	整数 rc;
	struct mbox_controller *mbox = mbox_controller_find(nodeoff);

	もし (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		もし (rc)
			戻る rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		もし (!mbox)
			戻る SBI_ENOSYS;
	}

	もし (out_mbox)
		*out_mbox = mbox;

	戻る 0;
}

整数 fdt_mailbox_request_chan(const void *fdt, 整数 nodeoff, 整数 index,
			     struct mbox_chan **out_chan)
{
	整数 rc;
	struct mbox_chan *chan;
	struct fdt_mailbox *drv;
	struct fdt_phandle_args pargs;
	struct mbox_controller *mbox = NULL;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	もし (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		戻る SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	もし (rc)
		戻る rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	もし (rc)
		戻る rc;

	drv = mbox->driver;
	もし (!drv || !drv->xlate)
		戻る SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	もし (rc)
		戻る rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	もし (!chan)
		戻る SBI_ENOENT;

	*out_chan = chan;
	戻る 0;
}

整数 fdt_mailbox_simple_xlate(struct mbox_controller *mbox,
			     const struct fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	整数 i;

	もし (pargs->args_count < 1)
		戻る SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	ために (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	戻る 0;
}
