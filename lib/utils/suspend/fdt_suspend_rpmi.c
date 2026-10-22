/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/fdt_mailbox.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/mailbox.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/rpmi_mailbox.h>

ସଂରଚନା rpmi_syssusp {
	ସଂରଚନା mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

ସ୍ଥିର ସଂରଚନା rpmi_syssusp syssusp_ctx;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_syssusp_attrs(uint32_t *attrs)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା rpmi_syssusp_get_attr_resp resp;
	ସଂରଚନା rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_normal_request_with_status(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_ATTRIBUTES,
		&req, rpmi_u32_ଗଣନା(req), rpmi_u32_ଗଣନା(req),
		&resp, rpmi_u32_ଗଣନା(resp), rpmi_u32_ଗଣନା(resp));
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	*attrs = resp.flags;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା rpmi_syssusp_suspend_req req;
	ସଂରଚନା rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_ଗଣନା(req), rpmi_u32_ଗଣନା(req),
			&resp, rpmi_u32_ଗଣନା(resp), rpmi_u32_ଗଣନା(resp));
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	/* Wait for interrupt */
	wfi();

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_system_suspend_check(u32 sleep_type)
{
	ଫେରନ୍ତୁ ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ସଂରଚନା sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_suspend_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			     ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	ଯଦି (syssusp_ctx.chan)
		ଫେରନ୍ତୁ 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	ଯଦି (rc)
		ଫେରନ୍ତୁ SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_set_device(&rpmi_suspend_dev);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match rpmi_suspend_match[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_suspend_rpmi = {
	.match_table = rpmi_suspend_match,
	.init = rpmi_suspend_init,
};
