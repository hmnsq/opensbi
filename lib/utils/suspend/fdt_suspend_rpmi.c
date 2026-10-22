/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/mailbox/fdt_mailbox.h>
#شامل <sbi_utils/mailbox/mailbox.h>
#شامل <sbi_utils/mailbox/rpmi_mailbox.h>

ساخت rpmi_syssusp {
	ساخت mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

ساکن ساخت rpmi_syssusp syssusp_ctx;

ساکن صحیح rpmi_syssusp_attrs(uint32_t *attrs)
{
	صحیح rc;
	ساخت rpmi_syssusp_get_attr_resp resp;
	ساخت rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_معمول_request_with_حالت(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_اوصاف,
		&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
		&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
	اگر (rc)
		واپس rc;

	*attrs = resp.flags;

	واپس 0;
}

ساکن صحیح rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	صحیح rc;
	ساخت rpmi_syssusp_suspend_req req;
	ساخت rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_معمول_request_with_حالت(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
	اگر (rc)
		واپس rc;

	/* Wait for interrupt */
	wfi();

	واپس 0;
}

ساکن صحیح rpmi_system_suspend_check(u32 sleep_type)
{
	واپس ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

ساکن صحیح rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	صحیح rc;

	اگر (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		واپس SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	اگر (rc)
		واپس rc;

	واپس 0;
}

ساکن ساخت sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

ساکن صحیح rpmi_suspend_init(ثابت خالی *fdt, صحیح nodeoff,
			     ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	اگر (syssusp_ctx.chan)
		واپس 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	اگر (rc)
		واپس SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	اگر (rc)
		واپس rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_سیٹ_device(&rpmi_suspend_dev);

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت rpmi_suspend_مطابقت[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

ثابت ساخت fdt_driver fdt_suspend_rpmi = {
	.مطابقت_جدول = rpmi_suspend_مطابقت,
	.init = rpmi_suspend_init,
};
