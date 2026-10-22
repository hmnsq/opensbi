/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ fdt_driver *ಧ್ರುವ fdt_mailbox_drivers[];

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_mbox_controller_find(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				    ಸಂರಚನಾ mbox_controller **out_mbox)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ mbox_controller *mbox = mbox_controller_find(nodeoff);

	ಯದಿ (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		ಯದಿ (!mbox)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;
	}

	ಯದಿ (out_mbox)
		*out_mbox = mbox;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_mailbox_request_chan(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಪೂರ್ಣಾಙ್ಕ index,
			     ಸಂರಚನಾ mbox_chan **out_chan)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ mbox_chan *chan;
	ಸಂರಚನಾ fdt_mailbox *drv;
	ಸಂರಚನಾ fdt_phandle_args pargs;
	ಸಂರಚನಾ mbox_controller *mbox = ರಿಕ್ತ;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	ಯದಿ (!fdt || (nodeoff < 0) || (index < 0) || !out_chan)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	pargs.ಗಂಟು_offset = pargs.args_ಎಣಿಕೆ = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 index, &pargs);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	rc = fdt_mbox_controller_find(fdt, pargs.ಗಂಟು_offset, &mbox);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	drv = mbox->driver;
	ಯದಿ (!drv || !drv->xlate)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	ಯದಿ (!chan)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

	*out_chan = chan;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_mailbox_simple_xlate(ಸಂರಚನಾ mbox_controller *mbox,
			     ಧ್ರುವ ಸಂರಚನಾ fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	ಪೂರ್ಣಾಙ್ಕ i;

	ಯದಿ (pargs->args_ಎಣಿಕೆ < 1)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	ಪ್ರತಿ (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	ಪ್ರತ್ಯಾಗಮ 0;
}
