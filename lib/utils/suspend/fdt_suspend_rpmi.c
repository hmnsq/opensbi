/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/mailbox/fdt_mailbox.h>
#ضمن <sbi_utils/mailbox/mailbox.h>
#ضمن <sbi_utils/mailbox/rpmi_mailbox.h>

بنية rpmi_syssusp {
	بنية mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

ساكن بنية rpmi_syssusp syssusp_ctx;

ساكن صحيح rpmi_syssusp_attrs(uint32_t *attrs)
{
	صحيح rc;
	بنية rpmi_syssusp_get_attr_resp resp;
	بنية rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_normal_request_with_status(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_ATTRIBUTES,
		&req, rpmi_u32_count(req), rpmi_u32_count(req),
		&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	אם (rc)
		החזר rc;

	*attrs = resp.flags;

	החזר 0;
}

ساكن صحيح rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	صحيح rc;
	بنية rpmi_syssusp_suspend_req req;
	بنية rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	אם (rc)
		החזר rc;

	/* Wait for interrupt */
	wfi();

	החזר 0;
}

ساكن صحيح rpmi_system_suspend_check(u32 sleep_type)
{
	החזר ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

ساكن صحيح rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	صحيح rc;

	אם (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		החזר SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	אם (rc)
		החזר rc;

	החזר 0;
}

ساكن بنية sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

ساكن صحيح rpmi_suspend_init(ثابت فراغ *fdt, صحيح nodeoff,
			     ثابت بنية fdt_match *match)
{
	صحيح rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	אם (syssusp_ctx.chan)
		החזר 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	אם (rc)
		החזר SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	אם (rc)
		החזר rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_set_device(&rpmi_suspend_dev);

	החזר 0;
}

ساكن ثابت بنية fdt_match rpmi_suspend_match[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

ثابت بنية fdt_driver fdt_suspend_rpmi = {
	.match_table = rpmi_suspend_match,
	.init = rpmi_suspend_init,
};
