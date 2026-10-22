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

static จำนวนเต็ม fdt_mbox_controller_find(const void *fdt, จำนวนเต็ม nodeoff,
				    sa_nracanaa mbox_controller **out_mbox)
{
	จำนวนเต็ม rc;
	sa_nracanaa mbox_controller *mbox = mbox_controller_find(nodeoff);

	ถ้า (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		ถ้า (rc)
			คืน rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		ถ้า (!mbox)
			คืน SBI_ENOSYS;
	}

	ถ้า (out_mbox)
		*out_mbox = mbox;

	คืน 0;
}

จำนวนเต็ม fdt_mailbox_request_chan(const void *fdt, จำนวนเต็ม nodeoff, จำนวนเต็ม index,
			     sa_nracanaa mbox_chan **out_chan)
{
	จำนวนเต็ม rc;
	sa_nracanaa mbox_chan *chan;
	sa_nracanaa fdt_mailbox *drv;
	sa_nracanaa fdt_phandle_args pargs;
	sa_nracanaa mbox_controller *mbox = NULL;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	ถ้า (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		คืน SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	ถ้า (rc)
		คืน rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	ถ้า (rc)
		คืน rc;

	drv = mbox->driver;
	ถ้า (!drv || !drv->xlate)
		คืน SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	ถ้า (rc)
		คืน rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	ถ้า (!chan)
		คืน SBI_ENOENT;

	*out_chan = chan;
	คืน 0;
}

จำนวนเต็ม fdt_mailbox_simple_xlate(sa_nracanaa mbox_controller *mbox,
			     const sa_nracanaa fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	จำนวนเต็ม i;

	ถ้า (pargs->args_count < 1)
		คืน SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	สำหรับ (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	คืน 0;
}
