/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/mailbox/fdt_mailbox.h>
#అన్తర్భావ <sbi_utils/mailbox/mailbox.h>
#అన్తర్భావ <sbi_utils/mailbox/rpmi_mailbox.h>

నిర్మాణం rpmi_syssusp {
	నిర్మాణం mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

స్థిర నిర్మాణం rpmi_syssusp syssusp_ctx;

స్థిర పూర్ణాంకం rpmi_syssusp_attrs(uint32_t *attrs)
{
	పూర్ణాంకం rc;
	నిర్మాణం rpmi_syssusp_get_attr_resp resp;
	నిర్మాణం rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_సాధారణ_request_with_స్థితి(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_లక్షణాలు,
		&req, rpmi_u32_లెక్క(req), rpmi_u32_లెక్క(req),
		&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
	ఒకవేళ (rc)
		తిరిగి rc;

	*attrs = resp.flags;

	తిరిగి 0;
}

స్థిర పూర్ణాంకం rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	పూర్ణాంకం rc;
	నిర్మాణం rpmi_syssusp_suspend_req req;
	నిర్మాణం rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_సాధారణ_request_with_స్థితి(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_లెక్క(req), rpmi_u32_లెక్క(req),
			&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
	ఒకవేళ (rc)
		తిరిగి rc;

	/* Wait for interrupt */
	wfi();

	తిరిగి 0;
}

స్థిర పూర్ణాంకం rpmi_system_suspend_check(u32 sleep_type)
{
	తిరిగి ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

స్థిర పూర్ణాంకం rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	పూర్ణాంకం rc;

	ఒకవేళ (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		తిరిగి SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి 0;
}

స్థిర నిర్మాణం sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

స్థిర పూర్ణాంకం rpmi_suspend_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			     స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	ఒకవేళ (syssusp_ctx.chan)
		తిరిగి 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	ఒకవేళ (rc)
		తిరిగి SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	ఒకవేళ (rc)
		తిరిగి rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_సమితి_device(&rpmi_suspend_dev);

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక rpmi_suspend_సరిపోలిక[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_suspend_rpmi = {
	.సరిపోలిక_పట్టిక = rpmi_suspend_సరిపోలిక,
	.init = rpmi_suspend_init,
};
