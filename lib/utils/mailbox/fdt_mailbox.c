/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
బాహ్య స్థిరరాశి నిర్మాణం fdt_driver *స్థిరరాశి fdt_mailbox_drivers[];

స్థిర పూర్ణాంకం fdt_mbox_controller_కనుగొను(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				    నిర్మాణం mbox_controller **బహిర్గతం_mbox)
{
	పూర్ణాంకం rc;
	నిర్మాణం mbox_controller *mbox = mbox_controller_కనుగొను(nodeoff);

	ఒకవేళ (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_స్థానభ్రంశం(fdt, nodeoff, fdt_mailbox_drivers);
		ఒకవేళ (rc)
			తిరిగి rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_కనుగొను(nodeoff);
		ఒకవేళ (!mbox)
			తిరిగి SBI_ENOSYS;
	}

	ఒకవేళ (బహిర్గతం_mbox)
		*బహిర్గతం_mbox = mbox;

	తిరిగి 0;
}

పూర్ణాంకం fdt_mailbox_request_chan(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, పూర్ణాంకం సూచీ_2,
			     నిర్మాణం mbox_chan **బహిర్గతం_chan)
{
	పూర్ణాంకం rc;
	నిర్మాణం mbox_chan *chan;
	నిర్మాణం fdt_mailbox *drv;
	నిర్మాణం fdt_phandle_వాదనలు pargs;
	నిర్మాణం mbox_controller *mbox = ఖాళీ;
	u32 chan_వాదనలు[MBOX_CHAN_MAX_వాదనలు];

	ఒకవేళ (!fdt || (nodeoff < 0) || (సూచీ_2 < 0) || !బహిర్గతం_chan)
		తిరిగి SBI_EINVAL;

	pargs.కణుపు_స్థానభ్రంశం = pargs.వాదనలు_లెక్క = 0;
	rc = fdt_parse_phandle_with_వాదనలు(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 సూచీ_2, &pargs);
	ఒకవేళ (rc)
		తిరిగి rc;

	rc = fdt_mbox_controller_కనుగొను(fdt, pargs.కణుపు_స్థానభ్రంశం, &mbox);
	ఒకవేళ (rc)
		తిరిగి rc;

	drv = mbox->driver;
	ఒకవేళ (!drv || !drv->xlate)
		తిరిగి SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_వాదనలు);
	ఒకవేళ (rc)
		తిరిగి rc;

	chan = mbox_controller_request_chan(mbox, chan_వాదనలు);
	ఒకవేళ (!chan)
		తిరిగి SBI_ENOENT;

	*బహిర్గతం_chan = chan;
	తిరిగి 0;
}

పూర్ణాంకం fdt_mailbox_simple_xlate(నిర్మాణం mbox_controller *mbox,
			     స్థిరరాశి నిర్మాణం fdt_phandle_వాదనలు *pargs,
			     u32 *బహిర్గతం_chan_వాదనలు)
{
	పూర్ణాంకం i;

	ఒకవేళ (pargs->వాదనలు_లెక్క < 1)
		తిరిగి SBI_EINVAL;

	బహిర్గతం_chan_వాదనలు[0] = pargs->వాదనలు[0];
	కొరకు (i = 1; i < MBOX_CHAN_MAX_వాదనలు; i++)
		బహిర్గతం_chan_వాదనలు[i] = 0;

	తిరిగి 0;
}
