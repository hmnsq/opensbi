/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/sbi_system.h>
#অন্তর্ভাৱ <sbi/riscv_asm.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/mailbox/fdt_mailbox.h>
#অন্তর্ভাৱ <sbi_utils/mailbox/mailbox.h>
#অন্তর্ভাৱ <sbi_utils/mailbox/rpmi_mailbox.h>

সংরচনা rpmi_syssusp {
	সংরচনা mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

স্থির সংরচনা rpmi_syssusp syssusp_ctx;

স্থির পূর্ণাঙ্ক rpmi_syssusp_attrs(uint32_t *attrs)
{
	পূর্ণাঙ্ক rc;
	সংরচনা rpmi_syssusp_get_attr_resp resp;
	সংরচনা rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_normal_request_with_status(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_ATTRIBUTES,
		&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
		&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
	যদি (rc)
		ঘূৰাই rc;

	*attrs = resp.flags;

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	পূর্ণাঙ্ক rc;
	সংরচনা rpmi_syssusp_suspend_req req;
	সংরচনা rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
	যদি (rc)
		ঘূৰাই rc;

	/* Wait for interrupt */
	wfi();

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক rpmi_system_suspend_check(u32 sleep_type)
{
	ঘূৰাই ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

স্থির পূর্ণাঙ্ক rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	পূর্ণাঙ্ক rc;

	যদি (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		ঘূৰাই SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	যদি (rc)
		ঘূৰাই rc;

	ঘূৰাই 0;
}

স্থির সংরচনা sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

স্থির পূর্ণাঙ্ক rpmi_suspend_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			     ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	যদি (syssusp_ctx.chan)
		ঘূৰাই 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	যদি (rc)
		ঘূৰাই SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	যদি (rc)
		ঘূৰাই rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_set_device(&rpmi_suspend_dev);

	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_match rpmi_suspend_match[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

ধ্রুৱ সংরচনা fdt_driver fdt_suspend_rpmi = {
	.match_table = rpmi_suspend_match,
	.init = rpmi_suspend_init,
};
