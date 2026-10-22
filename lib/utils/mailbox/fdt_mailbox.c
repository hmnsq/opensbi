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

static පූර්ණ_සංඛ්‍යා fdt_mbox_controller_find(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoff,
				    sa_nracanaa mbox_controller **out_mbox)
{
	පූර්ණ_සංඛ්‍යා rc;
	sa_nracanaa mbox_controller *mbox = mbox_controller_find(nodeoff);

	නම් (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		නම් (rc)
			ආපසු rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		නම් (!mbox)
			ආපසු SBI_ENOSYS;
	}

	නම් (out_mbox)
		*out_mbox = mbox;

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_mailbox_request_chan(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoff, පූර්ණ_සංඛ්‍යා index,
			     sa_nracanaa mbox_chan **out_chan)
{
	පූර්ණ_සංඛ්‍යා rc;
	sa_nracanaa mbox_chan *chan;
	sa_nracanaa fdt_mailbox *drv;
	sa_nracanaa fdt_phandle_args pargs;
	sa_nracanaa mbox_controller *mbox = NULL;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	නම් (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		ආපසු SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	නම් (rc)
		ආපසු rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	නම් (rc)
		ආපසු rc;

	drv = mbox->driver;
	නම් (!drv || !drv->xlate)
		ආපසු SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	නම් (rc)
		ආපසු rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	නම් (!chan)
		ආපසු SBI_ENOENT;

	*out_chan = chan;
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_mailbox_simple_xlate(sa_nracanaa mbox_controller *mbox,
			     const sa_nracanaa fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	පූර්ණ_සංඛ්‍යා i;

	නම් (pargs->args_count < 1)
		ආපසු SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	සඳහා (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	ආපසු 0;
}
