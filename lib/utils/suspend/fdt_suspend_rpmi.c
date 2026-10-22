/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/fdt_mailbox.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/mailbox.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/rpmi_mailbox.h>

ഘടന rpmi_syssusp {
	ഘടന mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

സ്ഥിതം ഘടന rpmi_syssusp syssusp_ctx;

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_syssusp_attrs(uint32_t *attrs)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന rpmi_syssusp_get_attr_resp resp;
	ഘടന rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_normal_request_with_status(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_ATTRIBUTES,
		&req, rpmi_u32_എണ്ണം(req), rpmi_u32_എണ്ണം(req),
		&resp, rpmi_u32_എണ്ണം(resp), rpmi_u32_എണ്ണം(resp));
	എങ്കിൽ (rc)
		തിരികെ rc;

	*attrs = resp.flags;

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന rpmi_syssusp_suspend_req req;
	ഘടന rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_എണ്ണം(req), rpmi_u32_എണ്ണം(req),
			&resp, rpmi_u32_എണ്ണം(resp), rpmi_u32_എണ്ണം(resp));
	എങ്കിൽ (rc)
		തിരികെ rc;

	/* Wait for interrupt */
	wfi();

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_system_suspend_check(u32 sleep_type)
{
	തിരികെ ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		തിരികെ SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ 0;
}

സ്ഥിതം ഘടന sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_suspend_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			     സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	എങ്കിൽ (syssusp_ctx.chan)
		തിരികെ 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	എങ്കിൽ (rc)
		തിരികെ SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	എങ്കിൽ (rc)
		തിരികെ rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_set_device(&rpmi_suspend_dev);

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match rpmi_suspend_match[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

സ്ഥിരം ഘടന fdt_driver fdt_suspend_rpmi = {
	.match_table = rpmi_suspend_match,
	.init = rpmi_suspend_init,
};
