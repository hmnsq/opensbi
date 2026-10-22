/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/mailbox/fdt_mailbox.h>
#અન્તર્ભાવ <sbi_utils/mailbox/mailbox.h>
#અન્તર્ભાવ <sbi_utils/mailbox/rpmi_mailbox.h>

સંરચના rpmi_syssusp {
	સંરચના mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

સ્થિર સંરચના rpmi_syssusp syssusp_ctx;

સ્થિર પૂર્ણાંક rpmi_syssusp_attrs(uint32_t *attrs)
{
	પૂર્ણાંક rc;
	સંરચના rpmi_syssusp_get_attr_resp resp;
	સંરચના rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_normal_request_with_status(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_ATTRIBUTES,
		&req, rpmi_u32_ગણતરી(req), rpmi_u32_ગણતરી(req),
		&resp, rpmi_u32_ગણતરી(resp), rpmi_u32_ગણતરી(resp));
	જો (rc)
		પાછા rc;

	*attrs = resp.flags;

	પાછા 0;
}

સ્થિર પૂર્ણાંક rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	પૂર્ણાંક rc;
	સંરચના rpmi_syssusp_suspend_req req;
	સંરચના rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_ગણતરી(req), rpmi_u32_ગણતરી(req),
			&resp, rpmi_u32_ગણતરી(resp), rpmi_u32_ગણતરી(resp));
	જો (rc)
		પાછા rc;

	/* Wait for interrupt */
	wfi();

	પાછા 0;
}

સ્થિર પૂર્ણાંક rpmi_system_suspend_check(u32 sleep_type)
{
	પાછા ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

સ્થિર પૂર્ણાંક rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	પૂર્ણાંક rc;

	જો (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		પાછા SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	જો (rc)
		પાછા rc;

	પાછા 0;
}

સ્થિર સંરચના sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

સ્થિર પૂર્ણાંક rpmi_suspend_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			     અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	જો (syssusp_ctx.chan)
		પાછા 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	જો (rc)
		પાછા SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	જો (rc)
		પાછા rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_set_device(&rpmi_suspend_dev);

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match rpmi_suspend_match[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

અચલ સંરચના fdt_driver fdt_suspend_rpmi = {
	.match_table = rpmi_suspend_match,
	.init = rpmi_suspend_init,
};
