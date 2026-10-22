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
बाह्य ध्रुव संरचना fdt_driver *ध्रुव fdt_mailbox_drivers[];

स्थिर पूर्णाङ्क fdt_mbox_controller_find(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				    संरचना mbox_controller **बहिर्गमनम्_mbox)
{
	पूर्णाङ्क rc;
	संरचना mbox_controller *mbox = mbox_controller_find(nodeoff);

	यदि (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_विस्थापनम्(fdt, nodeoff, fdt_mailbox_drivers);
		यदि (rc)
			प्रत्यागम rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		यदि (!mbox)
			प्रत्यागम SBI_ENOSYS;
	}

	यदि (बहिर्गमनम्_mbox)
		*बहिर्गमनम्_mbox = mbox;

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_mailbox_request_chan(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, पूर्णाङ्क सूचकांक_2,
			     संरचना mbox_chan **बहिर्गमनम्_chan)
{
	पूर्णाङ्क rc;
	संरचना mbox_chan *chan;
	संरचना fdt_mailbox *drv;
	संरचना fdt_phandle_तर्काः pargs;
	संरचना mbox_controller *mbox = रिक्त;
	u32 chan_तर्काः[MBOX_CHAN_MAX_तर्काः];

	यदि (!fdt || (nodeoff < 0) || (सूचकांक_2 < 0) || !बहिर्गमनम्_chan)
		प्रत्यागम SBI_EINVAL;

	pargs.ग्रन्थिः_विस्थापनम् = pargs.तर्काः_गणनम् = 0;
	rc = fdt_parse_phandle_with_तर्काः(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 सूचकांक_2, &pargs);
	यदि (rc)
		प्रत्यागम rc;

	rc = fdt_mbox_controller_find(fdt, pargs.ग्रन्थिः_विस्थापनम्, &mbox);
	यदि (rc)
		प्रत्यागम rc;

	drv = mbox->driver;
	यदि (!drv || !drv->xlate)
		प्रत्यागम SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_तर्काः);
	यदि (rc)
		प्रत्यागम rc;

	chan = mbox_controller_request_chan(mbox, chan_तर्काः);
	यदि (!chan)
		प्रत्यागम SBI_ENOENT;

	*बहिर्गमनम्_chan = chan;
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_mailbox_simple_xlate(संरचना mbox_controller *mbox,
			     ध्रुव संरचना fdt_phandle_तर्काः *pargs,
			     u32 *बहिर्गमनम्_chan_तर्काः)
{
	पूर्णाङ्क i;

	यदि (pargs->तर्काः_गणनम् < 1)
		प्रत्यागम SBI_EINVAL;

	बहिर्गमनम्_chan_तर्काः[0] = pargs->तर्काः[0];
	प्रति (i = 1; i < MBOX_CHAN_MAX_तर्काः; i++)
		बहिर्गमनम्_chan_तर्काः[i] = 0;

	प्रत्यागम 0;
}
