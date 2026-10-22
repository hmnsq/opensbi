/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
बाह्य अचर संरचना fdt_driver *अचर fdt_mailbox_drivers[];

स्थिर पूर्णाङ्क fdt_mbox_controller_find(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
				    संरचना mbox_controller **out_mbox)
{
	पूर्णाङ्क rc;
	संरचना mbox_controller *mbox = mbox_controller_find(nodeoff);

	यदि (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		यदि (rc)
			फर्काऊ rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		यदि (!mbox)
			फर्काऊ SBI_ENOSYS;
	}

	यदि (out_mbox)
		*out_mbox = mbox;

	फर्काऊ 0;
}

पूर्णाङ्क fdt_mailbox_request_chan(अचर शून्य *fdt, पूर्णाङ्क nodeoff, पूर्णाङ्क index,
			     संरचना mbox_chan **out_chan)
{
	पूर्णाङ्क rc;
	संरचना mbox_chan *chan;
	संरचना fdt_mailbox *drv;
	संरचना fdt_phandle_args pargs;
	संरचना mbox_controller *mbox = रिक्त;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	यदि (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		फर्काऊ SBI_EINVAL;

	pargs.गाँठो_offset = pargs.args_गन्ती = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	यदि (rc)
		फर्काऊ rc;

	rc = fdt_mbox_controller_find(fdt, pargs.गाँठो_offset, &mbox);
	यदि (rc)
		फर्काऊ rc;

	drv = mbox->driver;
	यदि (!drv || !drv->xlate)
		फर्काऊ SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	यदि (rc)
		फर्काऊ rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	यदि (!chan)
		फर्काऊ SBI_ENOENT;

	*out_chan = chan;
	फर्काऊ 0;
}

पूर्णाङ्क fdt_mailbox_simple_xlate(संरचना mbox_controller *mbox,
			     अचर संरचना fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	पूर्णाङ्क i;

	यदि (pargs->args_गन्ती < 1)
		फर्काऊ SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	लागि (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	फर्काऊ 0;
}
