/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/fdt_mailbox.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/mailbox.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/rpmi_mailbox.h>

ರಚನೆ rpmi_syssusp {
	ರಚನೆ mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

ಸ್ಥಿರ ರಚನೆ rpmi_syssusp syssusp_ctx;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_syssusp_attrs(uint32_t *attrs)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ rpmi_syssusp_get_attr_resp resp;
	ರಚನೆ rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_ಗುಣಲಕ್ಷಣಗಳು,
		&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
		&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	*attrs = resp.flags;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ rpmi_syssusp_suspend_req req;
	ರಚನೆ rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	/* Wait for interrupt */
	wfi();

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_system_suspend_check(u32 sleep_type)
{
	ಹಿಂತಿರುಗಿ ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ರಚನೆ sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_suspend_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			     ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	ಆದರೆ (syssusp_ctx.chan)
		ಹಿಂತಿರುಗಿ 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_ಗಣ_device(&rpmi_suspend_dev);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ rpmi_suspend_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_suspend_rpmi = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = rpmi_suspend_ಹೊಂದಿಕೆ,
	.init = rpmi_suspend_init,
};
