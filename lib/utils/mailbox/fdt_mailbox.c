/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
ବାହ୍ୟ ଅଚଳ ସଂରଚନା fdt_driver *ଅଚଳ fdt_mailbox_drivers[];

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_mbox_controller_find(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				    ସଂରଚନା mbox_controller **out_mbox)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା mbox_controller *mbox = mbox_controller_find(nodeoff);

	ଯଦି (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		ଯଦି (!mbox)
			ଫେରନ୍ତୁ SBI_ENOSYS;
	}

	ଯଦି (out_mbox)
		*out_mbox = mbox;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_mailbox_request_chan(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index,
			     ସଂରଚନା mbox_chan **out_chan)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା mbox_chan *chan;
	ସଂରଚନା fdt_mailbox *drv;
	ସଂରଚନା fdt_phandle_args pargs;
	ସଂରଚନା mbox_controller *mbox = ଖାଲି;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	ଯଦି (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		ଫେରନ୍ତୁ SBI_EINVAL;

	pargs.ଗଣ୍ଠି_offset = pargs.args_ଗଣନା = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	rc = fdt_mbox_controller_find(fdt, pargs.ଗଣ୍ଠି_offset, &mbox);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	drv = mbox->driver;
	ଯଦି (!drv || !drv->xlate)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	ଯଦି (!chan)
		ଫେରନ୍ତୁ SBI_ENOENT;

	*out_chan = chan;
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_mailbox_simple_xlate(ସଂରଚନା mbox_controller *mbox,
			     ଅଚଳ ସଂରଚନା fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ଯଦି (pargs->args_ଗଣନା < 1)
		ଫେରନ୍ତୁ SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	ପାଇଁ (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	ଫେରନ୍ତୁ 0;
}
