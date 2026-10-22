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
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *ಸ್ಥಿರಾಂಕ fdt_mailbox_drivers[];

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_mbox_controller_ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				    ರಚನೆ mbox_controller **ಹೊರಹೋಗು_mbox)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ mbox_controller *mbox = mbox_controller_ಕಂಡುಹಿಡಿ(nodeoff);

	ಆದರೆ (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_ಸ್ಥಾನಾಂತರ(fdt, nodeoff, fdt_mailbox_drivers);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_ಕಂಡುಹಿಡಿ(nodeoff);
		ಆದರೆ (!mbox)
			ಹಿಂತಿರುಗಿ SBI_ENOSYS;
	}

	ಆದರೆ (ಹೊರಹೋಗು_mbox)
		*ಹೊರಹೋಗು_mbox = mbox;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_mailbox_request_chan(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ_2,
			     ರಚನೆ mbox_chan **ಹೊರಹೋಗು_chan)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ mbox_chan *chan;
	ರಚನೆ fdt_mailbox *drv;
	ರಚನೆ fdt_phandle_ವಾದಗಳು pargs;
	ರಚನೆ mbox_controller *mbox = ಖಾಲಿ;
	u32 chan_ವಾದಗಳು[MBOX_CHAN_MAX_ವಾದಗಳು];

	ಆದರೆ (!fdt || (nodeoff < 0) || (ಸೂಚ್ಯಂಕ_2 < 0) || !ಹೊರಹೋಗು_chan)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	pargs.ಗಂಟು_ಸ್ಥಾನಾಂತರ = pargs.ವಾದಗಳು_ಎಣಿಕೆ = 0;
	rc = fdt_parse_phandle_with_ವಾದಗಳು(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 ಸೂಚ್ಯಂಕ_2, &pargs);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	rc = fdt_mbox_controller_ಕಂಡುಹಿಡಿ(fdt, pargs.ಗಂಟು_ಸ್ಥಾನಾಂತರ, &mbox);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	drv = mbox->driver;
	ಆದರೆ (!drv || !drv->xlate)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_ವಾದಗಳು);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	chan = mbox_controller_request_chan(mbox, chan_ವಾದಗಳು);
	ಆದರೆ (!chan)
		ಹಿಂತಿರುಗಿ SBI_ENOENT;

	*ಹೊರಹೋಗು_chan = chan;
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_mailbox_simple_xlate(ರಚನೆ mbox_controller *mbox,
			     ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_phandle_ವಾದಗಳು *pargs,
			     u32 *ಹೊರಹೋಗು_chan_ವಾದಗಳು)
{
	ಪೂರ್ಣಾಂಕ i;

	ಆದರೆ (pargs->ವಾದಗಳು_ಎಣಿಕೆ < 1)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಹೊರಹೋಗು_chan_ವಾದಗಳು[0] = pargs->ವಾದಗಳು[0];
	ಗಾಗಿ (i = 1; i < MBOX_CHAN_MAX_ವಾದಗಳು; i++)
		ಹೊರಹೋಗು_chan_ವಾದಗಳು[i] = 0;

	ಹಿಂತಿರುಗಿ 0;
}
