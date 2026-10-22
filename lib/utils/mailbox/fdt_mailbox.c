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

static ចំនួនគត់ fdt_mbox_controller_find(const void *fdt, ចំនួនគត់ nodeoff,
				    sa_nracanaa mbox_controller **out_mbox)
{
	ចំនួនគត់ rc;
	sa_nracanaa mbox_controller *mbox = mbox_controller_find(nodeoff);

	ប្រសិនបើ (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		ប្រសិនបើ (rc)
			ត្រឡប់ rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		ប្រសិនបើ (!mbox)
			ត្រឡប់ SBI_ENOSYS;
	}

	ប្រសិនបើ (out_mbox)
		*out_mbox = mbox;

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_mailbox_request_chan(const void *fdt, ចំនួនគត់ nodeoff, ចំនួនគត់ index,
			     sa_nracanaa mbox_chan **out_chan)
{
	ចំនួនគត់ rc;
	sa_nracanaa mbox_chan *chan;
	sa_nracanaa fdt_mailbox *drv;
	sa_nracanaa fdt_phandle_args pargs;
	sa_nracanaa mbox_controller *mbox = NULL;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	ប្រសិនបើ (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		ត្រឡប់ SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	drv = mbox->driver;
	ប្រសិនបើ (!drv || !drv->xlate)
		ត្រឡប់ SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	ប្រសិនបើ (!chan)
		ត្រឡប់ SBI_ENOENT;

	*out_chan = chan;
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_mailbox_simple_xlate(sa_nracanaa mbox_controller *mbox,
			     const sa_nracanaa fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	ចំនួនគត់ i;

	ប្រសិនបើ (pargs->args_count < 1)
		ត្រឡប់ SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	សម្រាប់ (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	ត្រឡប់ 0;
}
