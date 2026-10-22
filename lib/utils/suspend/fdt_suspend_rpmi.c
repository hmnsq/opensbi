/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/fdt_mailbox.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/mailbox.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/rpmi_mailbox.h>

ਬਣਤਰ rpmi_syssusp {
	ਬਣਤਰ mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

ਸਥਿਰ ਬਣਤਰ rpmi_syssusp syssusp_ctx;

ਸਥਿਰ ਪੂਰਨ rpmi_syssusp_attrs(uint32_t *attrs)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ rpmi_syssusp_get_attr_resp resp;
	ਬਣਤਰ rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_ਗੁਣ,
		&req, rpmi_u32_ਗਿਣਤੀ(req), rpmi_u32_ਗਿਣਤੀ(req),
		&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
	ਜੇ (rc)
		ਵਾਪਸ rc;

	*attrs = resp.flags;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ rpmi_syssusp_suspend_req req;
	ਬਣਤਰ rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_ਗਿਣਤੀ(req), rpmi_u32_ਗਿਣਤੀ(req),
			&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
	ਜੇ (rc)
		ਵਾਪਸ rc;

	/* Wait for interrupt */
	wfi();

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rpmi_system_suspend_check(u32 sleep_type)
{
	ਵਾਪਸ ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

ਸਥਿਰ ਪੂਰਨ rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	ਪੂਰਨ rc;

	ਜੇ (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		ਵਾਪਸ SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਬਣਤਰ sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

ਸਥਿਰ ਪੂਰਨ rpmi_suspend_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			     ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	ਜੇ (syssusp_ctx.chan)
		ਵਾਪਸ 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	ਜੇ (rc)
		ਵਾਪਸ SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_ਸੈੱਟ_device(&rpmi_suspend_dev);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ rpmi_suspend_ਮੇਲ[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_suspend_rpmi = {
	.ਮੇਲ_ਸਾਰਣੀ = rpmi_suspend_ਮੇਲ,
	.init = rpmi_suspend_init,
};
