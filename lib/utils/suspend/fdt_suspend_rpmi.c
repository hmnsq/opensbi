/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/mailbox/fdt_mailbox.h>
#अन्तर्भाव <sbi_utils/mailbox/mailbox.h>
#अन्तर्भाव <sbi_utils/mailbox/rpmi_mailbox.h>

संरचना rpmi_syssusp {
	संरचना mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

स्थिर संरचना rpmi_syssusp syssusp_ctx;

स्थिर पूर्णांक rpmi_syssusp_attrs(uint32_t *attrs)
{
	पूर्णांक rc;
	संरचना rpmi_syssusp_get_attr_resp resp;
	संरचना rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_normal_request_with_status(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_ATTRIBUTES,
		&req, rpmi_u32_मोजणी(req), rpmi_u32_मोजणी(req),
		&resp, rpmi_u32_मोजणी(resp), rpmi_u32_मोजणी(resp));
	जर (rc)
		परत rc;

	*attrs = resp.flags;

	परत 0;
}

स्थिर पूर्णांक rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	पूर्णांक rc;
	संरचना rpmi_syssusp_suspend_req req;
	संरचना rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_मोजणी(req), rpmi_u32_मोजणी(req),
			&resp, rpmi_u32_मोजणी(resp), rpmi_u32_मोजणी(resp));
	जर (rc)
		परत rc;

	/* Wait for interrupt */
	wfi();

	परत 0;
}

स्थिर पूर्णांक rpmi_system_suspend_check(u32 sleep_type)
{
	परत ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

स्थिर पूर्णांक rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	पूर्णांक rc;

	जर (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		परत SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	जर (rc)
		परत rc;

	परत 0;
}

स्थिर संरचना sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

स्थिर पूर्णांक rpmi_suspend_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
			     स्थिरांक संरचना fdt_match *match)
{
	पूर्णांक rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	जर (syssusp_ctx.chan)
		परत 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	जर (rc)
		परत SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	जर (rc)
		परत rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_set_device(&rpmi_suspend_dev);

	परत 0;
}

स्थिर स्थिरांक संरचना fdt_match rpmi_suspend_match[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

स्थिरांक संरचना fdt_driver fdt_suspend_rpmi = {
	.match_table = rpmi_suspend_match,
	.init = rpmi_suspend_init,
};
