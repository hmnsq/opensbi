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

static ကိန်းပြည့် fdt_mbox_controller_find(const void *fdt, ကိန်းပြည့် nodeoff,
				    sa_nracanaa mbox_controller **out_mbox)
{
	ကိန်းပြည့် rc;
	sa_nracanaa mbox_controller *mbox = mbox_controller_find(nodeoff);

	အကယ်၍ (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		အကယ်၍ (rc)
			ပြန်ပေး rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		အကယ်၍ (!mbox)
			ပြန်ပေး SBI_ENOSYS;
	}

	အကယ်၍ (out_mbox)
		*out_mbox = mbox;

	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_mailbox_request_chan(const void *fdt, ကိန်းပြည့် nodeoff, ကိန်းပြည့် index,
			     sa_nracanaa mbox_chan **out_chan)
{
	ကိန်းပြည့် rc;
	sa_nracanaa mbox_chan *chan;
	sa_nracanaa fdt_mailbox *drv;
	sa_nracanaa fdt_phandle_args pargs;
	sa_nracanaa mbox_controller *mbox = NULL;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	အကယ်၍ (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		ပြန်ပေး SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	အကယ်၍ (rc)
		ပြန်ပေး rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	အကယ်၍ (rc)
		ပြန်ပေး rc;

	drv = mbox->driver;
	အကယ်၍ (!drv || !drv->xlate)
		ပြန်ပေး SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	အကယ်၍ (rc)
		ပြန်ပေး rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	အကယ်၍ (!chan)
		ပြန်ပေး SBI_ENOENT;

	*out_chan = chan;
	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_mailbox_simple_xlate(sa_nracanaa mbox_controller *mbox,
			     const sa_nracanaa fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	ကိန်းပြည့် i;

	အကယ်၍ (pargs->args_count < 1)
		ပြန်ပေး SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	အတွက် (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	ပြန်ပေး 0;
}
