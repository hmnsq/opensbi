/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/mailbox/fdt_mailbox.h>
#அந்தர்பாவ <sbi_utils/mailbox/mailbox.h>
#அந்தர்பாவ <sbi_utils/mailbox/rpmi_mailbox.h>

கட்டமைப்பு rpmi_syssusp {
	கட்டமைப்பு mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

நிலையான கட்டமைப்பு rpmi_syssusp syssusp_ctx;

நிலையான முழு rpmi_syssusp_attrs(uint32_t *attrs)
{
	முழு rc;
	கட்டமைப்பு rpmi_syssusp_get_attr_resp resp;
	கட்டமைப்பு rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_normal_request_with_status(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_ATTRIBUTES,
		&req, rpmi_u32_மொத்தம்(req), rpmi_u32_மொத்தம்(req),
		&resp, rpmi_u32_மொத்தம்(resp), rpmi_u32_மொத்தம்(resp));
	எனில் (rc)
		திரும்பு rc;

	*attrs = resp.flags;

	திரும்பு 0;
}

நிலையான முழு rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	முழு rc;
	கட்டமைப்பு rpmi_syssusp_suspend_req req;
	கட்டமைப்பு rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_மொத்தம்(req), rpmi_u32_மொத்தம்(req),
			&resp, rpmi_u32_மொத்தம்(resp), rpmi_u32_மொத்தம்(resp));
	எனில் (rc)
		திரும்பு rc;

	/* Wait for interrupt */
	wfi();

	திரும்பு 0;
}

நிலையான முழு rpmi_system_suspend_check(u32 sleep_type)
{
	திரும்பு ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

நிலையான முழு rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	முழு rc;

	எனில் (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		திரும்பு SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	எனில் (rc)
		திரும்பு rc;

	திரும்பு 0;
}

நிலையான கட்டமைப்பு sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

நிலையான முழு rpmi_suspend_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			     மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	எனில் (syssusp_ctx.chan)
		திரும்பு 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	எனில் (rc)
		திரும்பு SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	எனில் (rc)
		திரும்பு rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_set_device(&rpmi_suspend_dev);

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match rpmi_suspend_match[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

மாறிலி கட்டமைப்பு fdt_driver fdt_suspend_rpmi = {
	.match_table = rpmi_suspend_match,
	.init = rpmi_suspend_init,
};
