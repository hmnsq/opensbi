/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/mailbox/fdt_mailbox.h>

/* List of FDT mailbox drivers generated at compile time */
बाह्य स्थिर काष्ठा fdt_driver *स्थिर fdt_mailbox_drivers[];

अटल पूर्णांक fdt_mbox_controller_find(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				    काष्ठा mbox_controller **out_mbox)
{
	पूर्णांक rc;
	काष्ठा mbox_controller *mbox = mbox_controller_find(nodeoff);

	अगर (!mbox) {
		/* mailbox not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff, fdt_mailbox_drivers);
		अगर (rc)
			वापस rc;

		/* Try to find mailbox controller again */
		mbox = mbox_controller_find(nodeoff);
		अगर (!mbox)
			वापस SBI_ENOSYS;
	}

	अगर (out_mbox)
		*out_mbox = mbox;

	वापस 0;
}

पूर्णांक fdt_mailbox_request_chan(स्थिर व्योम *fdt, पूर्णांक nodeoff, पूर्णांक सूचकांक_2,
			     काष्ठा mbox_chan **out_chan)
{
	पूर्णांक rc;
	काष्ठा mbox_chan *chan;
	काष्ठा fdt_mailbox *drv;
	काष्ठा fdt_phandle_args pargs;
	काष्ठा mbox_controller *mbox = शून्य;
	u32 chan_args[MBOX_CHAN_MAX_ARGS];

	अगर (!fdt || (nodeoff < 0) || (सूचकांक_2 < 0) || !out_chan)
		वापस SBI_EINVAL;

	pargs.गाँठ_offset = pargs.args_गिनती = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "mboxes", "#mbox-cells",
					 सूचकांक_2, &pargs);
	अगर (rc)
		वापस rc;

	rc = fdt_mbox_controller_find(fdt, pargs.गाँठ_offset, &mbox);
	अगर (rc)
		वापस rc;

	drv = mbox->driver;
	अगर (!drv || !drv->xlate)
		वापस SBI_ENOSYS;

	rc = drv->xlate(mbox, &pargs, chan_args);
	अगर (rc)
		वापस rc;

	chan = mbox_controller_request_chan(mbox, chan_args);
	अगर (!chan)
		वापस SBI_ENOENT;

	*out_chan = chan;
	वापस 0;
}

पूर्णांक fdt_mailbox_simple_xlate(काष्ठा mbox_controller *mbox,
			     स्थिर काष्ठा fdt_phandle_args *pargs,
			     u32 *out_chan_args)
{
	पूर्णांक i;

	अगर (pargs->args_गिनती < 1)
		वापस SBI_EINVAL;

	out_chan_args[0] = pargs->args[0];
	क्रम (i = 1; i < MBOX_CHAN_MAX_ARGS; i++)
		out_chan_args[i] = 0;

	वापस 0;
}
