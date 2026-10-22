/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
ബാഹ്യ സ്ഥിരം ഘടന fdt_driver *സ്ഥിരം fdt_mailbox_drivers[];

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_mbox_controller_find(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				    ഘടന mbox_controller **out_mbox)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന mbox_controller *mbox = mbox_controller_find(nodeoff);

	എങ്കിൽ (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		എങ്കിൽ (rc)
			തിരികെ rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		എങ്കിൽ (!mbox)
			തിരികെ SBI_ENOSYS;
	}

	എങ്കിൽ (out_mbox)
		*out_mbox = mbox;

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_mailbox_request_chan(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, പൂർണ്ണസംഖ്യ index,
			     ഘടന mbox_chan **out_chan)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന mbox_chan *chan;
	ഘടന fdt_mailbox *drv;
	ഘടന fdt_phandle_args pargs;
	ഘടന mbox_controller *mbox = ശൂന്യ;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	എങ്കിൽ (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		തിരികെ SBI_EINVAL;

	pargs.കണ്ണി_offset = pargs.args_എണ്ണം = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	എങ്കിൽ (rc)
		തിരികെ rc;

	rc = fdt_mbox_controller_find(fdt, pargs.കണ്ണി_offset, &mbox);
	എങ്കിൽ (rc)
		തിരികെ rc;

	drv = mbox->driver;
	എങ്കിൽ (!drv || !drv->xlate)
		തിരികെ SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	എങ്കിൽ (rc)
		തിരികെ rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	എങ്കിൽ (!chan)
		തിരികെ SBI_ENOENT;

	*out_chan = chan;
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_mailbox_simple_xlate(ഘടന mbox_controller *mbox,
			     സ്ഥിരം ഘടന fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	പൂർണ്ണസംഖ്യ i;

	എങ്കിൽ (pargs->args_എണ്ണം < 1)
		തിരികെ SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	വേണ്ടി (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	തിരികെ 0;
}
