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

static ຈຳນວນເຕັມ fdt_mbox_controller_find(const void *fdt, ຈຳນວນເຕັມ nodeoff,
				    sa_nracanaa mbox_controller **out_mbox)
{
	ຈຳນວນເຕັມ rc;
	sa_nracanaa mbox_controller *mbox = mbox_controller_find(nodeoff);

	ຖ້າ (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		ຖ້າ (!mbox)
			ສົ່ງຄືນ SBI_ENOSYS;
	}

	ຖ້າ (out_mbox)
		*out_mbox = mbox;

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_mailbox_request_chan(const void *fdt, ຈຳນວນເຕັມ nodeoff, ຈຳນວນເຕັມ index,
			     sa_nracanaa mbox_chan **out_chan)
{
	ຈຳນວນເຕັມ rc;
	sa_nracanaa mbox_chan *chan;
	sa_nracanaa fdt_mailbox *drv;
	sa_nracanaa fdt_phandle_args pargs;
	sa_nracanaa mbox_controller *mbox = NULL;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	ຖ້າ (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		ສົ່ງຄືນ SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	rc = fdt_mbox_controller_find(fdt, pargs.node_offset, &mbox);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	drv = mbox->driver;
	ຖ້າ (!drv || !drv->xlate)
		ສົ່ງຄືນ SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	ຖ້າ (!chan)
		ສົ່ງຄືນ SBI_ENOENT;

	*out_chan = chan;
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_mailbox_simple_xlate(sa_nracanaa mbox_controller *mbox,
			     const sa_nracanaa fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	ຈຳນວນເຕັມ i;

	ຖ້າ (pargs->args_count < 1)
		ສົ່ງຄືນ SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	ສຳລັບ (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	ສົ່ງຄືນ 0;
}
