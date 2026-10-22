/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
બાહ્ય અચલ સંરચના fdt_driver *અચલ fdt_mailbox_drivers[];

સ્થિર પૂર્ણાંક fdt_mbox_controller_find(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				    સંરચના mbox_controller **out_mbox)
{
	પૂર્ણાંક rc;
	સંરચના mbox_controller *mbox = mbox_controller_find(nodeoff);

	જો (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		જો (rc)
			પાછા rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		જો (!mbox)
			પાછા SBI_ENOSYS;
	}

	જો (out_mbox)
		*out_mbox = mbox;

	પાછા 0;
}

પૂર્ણાંક fdt_mailbox_request_chan(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, પૂર્ણાંક index,
			     સંરચના mbox_chan **out_chan)
{
	પૂર્ણાંક rc;
	સંરચના mbox_chan *chan;
	સંરચના fdt_mailbox *drv;
	સંરચના fdt_phandle_args pargs;
	સંરચના mbox_controller *mbox = ખાલી;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	જો (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		પાછા SBI_EINVAL;

	pargs.ગાંઠ_offset = pargs.args_ગણતરી = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	જો (rc)
		પાછા rc;

	rc = fdt_mbox_controller_find(fdt, pargs.ગાંઠ_offset, &mbox);
	જો (rc)
		પાછા rc;

	drv = mbox->driver;
	જો (!drv || !drv->xlate)
		પાછા SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	જો (rc)
		પાછા rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	જો (!chan)
		પાછા SBI_ENOENT;

	*out_chan = chan;
	પાછા 0;
}

પૂર્ણાંક fdt_mailbox_simple_xlate(સંરચના mbox_controller *mbox,
			     અચલ સંરચના fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	પૂર્ણાંક i;

	જો (pargs->args_ગણતરી < 1)
		પાછા SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	માટે (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	પાછા 0;
}
