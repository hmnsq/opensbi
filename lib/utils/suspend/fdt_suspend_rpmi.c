/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#include <libfdt.h>
#include <sbi/sbi_system.h>
#include <sbi/riscv_asm.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/mailbox/fdt_mailbox.h>
#include <sbi_utils/mailbox/mailbox.h>
#include <sbi_utils/mailbox/rpmi_mailbox.h>

sa_nracanaa rpmi_syssusp {
	sa_nracanaa mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

static sa_nracanaa rpmi_syssusp syssusp_ctx;

static puur_naa_nka rpmi_syssusp_attrs(uint32_t *attrs)
{
	puur_naa_nka rc;
	sa_nracanaa rpmi_syssusp_get_attr_resp resp;
	sa_nracanaa rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_normal_request_with_status(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_ATTRIBUTES,
		&req, rpmi_u32_count(req), rpmi_u32_count(req),
		&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	ᯀᯮᯝ (rc)
		ᯀᯮᯞ rc;

	*attrs = resp.flags;

	ᯀᯮᯞ 0;
}

static puur_naa_nka rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	puur_naa_nka rc;
	sa_nracanaa rpmi_syssusp_suspend_req req;
	sa_nracanaa rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	ᯀᯮᯝ (rc)
		ᯀᯮᯞ rc;

	/* Wait for interrupt */
	wfi();

	ᯀᯮᯞ 0;
}

static puur_naa_nka rpmi_system_suspend_check(u32 sleep_type)
{
	ᯀᯮᯞ ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

static puur_naa_nka rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	puur_naa_nka rc;

	ᯀᯮᯝ (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		ᯀᯮᯞ SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	ᯀᯮᯝ (rc)
		ᯀᯮᯞ rc;

	ᯀᯮᯞ 0;
}

static sa_nracanaa sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

static puur_naa_nka rpmi_suspend_init(const void *fdt, puur_naa_nka nodeoff,
			     const sa_nracanaa fdt_match *match)
{
	puur_naa_nka rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	ᯀᯮᯝ (syssusp_ctx.chan)
		ᯀᯮᯞ 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	ᯀᯮᯝ (rc)
		ᯀᯮᯞ SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	ᯀᯮᯝ (rc)
		ᯀᯮᯞ rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_set_device(&rpmi_suspend_dev);

	ᯀᯮᯞ 0;
}

static const sa_nracanaa fdt_match rpmi_suspend_match[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

const sa_nracanaa fdt_driver fdt_suspend_rpmi = {
	.match_table = rpmi_suspend_match,
	.init = rpmi_suspend_init,
};
