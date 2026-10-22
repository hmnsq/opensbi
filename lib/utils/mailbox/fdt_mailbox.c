/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ fdt_driver *ਅਚਲ fdt_mailbox_drivers[];

ਸਥਿਰ ਪੂਰਨ fdt_mbox_controller_ਲੱਭੋ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				    ਬਣਤਰ mbox_controller **ਬਾਹਰਲਾ_mbox)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ mbox_controller *mbox = mbox_controller_ਲੱਭੋ(nodeoff);

	ਜੇ (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_ਵਿੱਥ(fdt, nodeoff, fdt_mailbox_drivers);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_ਲੱਭੋ(nodeoff);
		ਜੇ (!mbox)
			ਵਾਪਸ SBI_ENOSYS;
	}

	ਜੇ (ਬਾਹਰਲਾ_mbox)
		*ਬਾਹਰਲਾ_mbox = mbox;

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_mailbox_request_chan(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਪੂਰਨ ਸੂਚਕਾਂਕ_2,
			     ਬਣਤਰ mbox_chan **ਬਾਹਰਲਾ_chan)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ mbox_chan *chan;
	ਬਣਤਰ fdt_mailbox *drv;
	ਬਣਤਰ fdt_phandle_ਦਲੀਲਾਂ pargs;
	ਬਣਤਰ mbox_controller *mbox = ਨਿਰਾ;
	u32 chan_ਦਲੀਲਾਂ[MBOX_CHAN_MAX_ਦਲੀਲਾਂ];

	ਜੇ (!fdt || (nodeoff < 0) || (ਸੂਚਕਾਂਕ_2 < 0) || !ਬਾਹਰਲਾ_chan)
		ਵਾਪਸ SBI_EINVAL;

	pargs.ਗੰਢ_ਵਿੱਥ = pargs.ਦਲੀਲਾਂ_ਗਿਣਤੀ = 0;
	rc = fdt_parse_phandle_with_ਦਲੀਲਾਂ(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 ਸੂਚਕਾਂਕ_2, &pargs);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	rc = fdt_mbox_controller_ਲੱਭੋ(fdt, pargs.ਗੰਢ_ਵਿੱਥ, &mbox);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	drv = mbox->driver;
	ਜੇ (!drv || !drv->xlate)
		ਵਾਪਸ SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_ਦਲੀਲਾਂ);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	chan = mbox_controller_request_chan(mbox, chan_ਦਲੀਲਾਂ);
	ਜੇ (!chan)
		ਵਾਪਸ SBI_ENOENT;

	*ਬਾਹਰਲਾ_chan = chan;
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_mailbox_simple_xlate(ਬਣਤਰ mbox_controller *mbox,
			     ਅਚਲ ਬਣਤਰ fdt_phandle_ਦਲੀਲਾਂ *pargs,
			     u32 *ਬਾਹਰਲਾ_chan_ਦਲੀਲਾਂ)
{
	ਪੂਰਨ i;

	ਜੇ (pargs->ਦਲੀਲਾਂ_ਗਿਣਤੀ < 1)
		ਵਾਪਸ SBI_EINVAL;

	ਬਾਹਰਲਾ_chan_ਦਲੀਲਾਂ[0] = pargs->ਦਲੀਲਾਂ[0];
	ਲਈ (i = 1; i < MBOX_CHAN_MAX_ਦਲੀਲਾਂ; i++)
		ਬਾਹਰਲਾ_chan_ਦਲੀਲਾਂ[i] = 0;

	ਵਾਪਸ 0;
}
