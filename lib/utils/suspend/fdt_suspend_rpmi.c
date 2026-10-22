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

ಸಂರಚನಾ rpmi_syssusp {
	ಸಂರಚನಾ mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

ಸ್ಥಿರ ಸಂರಚನಾ rpmi_syssusp syssusp_ctx;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_syssusp_attrs(uint32_t *attrs)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ rpmi_syssusp_get_attr_resp resp;
	ಸಂರಚನಾ rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_normal_request_with_status(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_ATTRIBUTES,
		&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
		&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	*attrs = resp.flags;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ rpmi_syssusp_suspend_req req;
	ಸಂರಚನಾ rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	/* Wait for interrupt */
	wfi();

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_system_suspend_check(u32 sleep_type)
{
	ಪ್ರತ್ಯಾಗಮ ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_suspend_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			     ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	ಯದಿ (syssusp_ctx.chan)
		ಪ್ರತ್ಯಾಗಮ 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_set_device(&rpmi_suspend_dev);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match rpmi_suspend_match[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_suspend_rpmi = {
	.match_table = rpmi_suspend_match,
	.init = rpmi_suspend_init,
};
