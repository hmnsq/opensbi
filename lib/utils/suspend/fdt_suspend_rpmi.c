/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi/riscv_asm.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/mailbox/fdt_mailbox.h>
#समावेश <sbi_utils/mailbox/mailbox.h>
#समावेश <sbi_utils/mailbox/rpmi_mailbox.h>

काष्ठा rpmi_syssusp {
	काष्ठा mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

अटल काष्ठा rpmi_syssusp syssusp_ctx;

अटल पूर्णांक rpmi_syssusp_attrs(uint32_t *attrs)
{
	पूर्णांक rc;
	काष्ठा rpmi_syssusp_get_attr_resp resp;
	काष्ठा rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_normal_request_with_status(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_ATTRIBUTES,
		&req, rpmi_u32_गिनती(req), rpmi_u32_गिनती(req),
		&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
	अगर (rc)
		वापस rc;

	*attrs = resp.flags;

	वापस 0;
}

अटल पूर्णांक rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	पूर्णांक rc;
	काष्ठा rpmi_syssusp_suspend_req req;
	काष्ठा rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_गिनती(req), rpmi_u32_गिनती(req),
			&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
	अगर (rc)
		वापस rc;

	/* Wait for interrupt */
	wfi();

	वापस 0;
}

अटल पूर्णांक rpmi_system_suspend_check(u32 sleep_type)
{
	वापस ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

अटल पूर्णांक rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	पूर्णांक rc;

	अगर (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		वापस SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	अगर (rc)
		वापस rc;

	वापस 0;
}

अटल काष्ठा sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

अटल पूर्णांक rpmi_suspend_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			     स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	अगर (syssusp_ctx.chan)
		वापस 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	अगर (rc)
		वापस SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	अगर (rc)
		वापस rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_set_device(&rpmi_suspend_dev);

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match rpmi_suspend_match[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

स्थिर काष्ठा fdt_driver fdt_suspend_rpmi = {
	.match_table = rpmi_suspend_match,
	.init = rpmi_suspend_init,
};
