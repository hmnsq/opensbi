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

struct rpmi_syssusp {
	struct mbox_chan *chan;
	bool cust_res_addr_supported;
	bool suspend_supported;
};

static struct rpmi_syssusp syssusp_ctx;

static цео rpmi_syssusp_attrs(uint32_t *attrs)
{
	цео rc;
	struct rpmi_syssusp_get_attr_resp resp;
	struct rpmi_syssusp_get_attr_req req;

	req.susp_type = SBI_SUSP_SLEEP_TYPE_SUSPEND;

	rc = rpmi_normal_request_with_status(
		syssusp_ctx.chan, RPMI_SYSSUSP_SRV_GET_ATTRIBUTES,
		&req, rpmi_u32_count(req), rpmi_u32_count(req),
		&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	ако (rc)
		врати rc;

	*attrs = resp.flags;

	врати 0;
}

static цео rpmi_syssusp(uint32_t suspend_type, ulong resume_addr)
{
	цео rc;
	struct rpmi_syssusp_suspend_req req;
	struct rpmi_syssusp_suspend_resp resp;

	req.hartid = current_hartid();
	req.suspend_type = suspend_type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			syssusp_ctx.chan, RPMI_SYSSUSP_SRV_SYSTEM_SUSPEND,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	ако (rc)
		врати rc;

	/* Wait for interrupt */
	wfi();

	врати 0;
}

static цео rpmi_system_suspend_check(u32 sleep_type)
{
	врати ((sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) &&
		syssusp_ctx.suspend_supported) ? 0 : SBI_EINVAL;
}

static цео rpmi_system_suspend(u32 sleep_type, ulong resume_addr)
{
	цео rc;

	ако (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		врати SBI_ENOTSUPP;

	rc = rpmi_syssusp(sleep_type, resume_addr);
	ако (rc)
		врати rc;

	врати 0;
}

static struct sbi_system_suspend_device rpmi_suspend_dev = {
	.name = "rpmi-system-suspend",
	.system_suspend_check = rpmi_system_suspend_check,
	.system_suspend = rpmi_system_suspend,
};

static цео rpmi_suspend_init(const void *fdt, цео nodeoff,
			     const struct fdt_match *match)
{
	цео rc;
	uint32_t attrs = 0;

	/* If channel already available then do nothing. */
	ако (syssusp_ctx.chan)
		врати 0;

	/*
	 * If channel request failed then other end does not support
	 * suspend service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &syssusp_ctx.chan);
	ако (rc)
		врати SBI_ENODEV;

	/* Get suspend attributes */
	rc = rpmi_syssusp_attrs(&attrs);
	ако (rc)
		врати rc;

	syssusp_ctx.suspend_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_SUSPENDTYPE;
	syssusp_ctx.cust_res_addr_supported =
			attrs & RPMI_SYSSUSP_ATTRS_FLAGS_RESUMEADDR;

	sbi_system_suspend_set_device(&rpmi_suspend_dev);

	врати 0;
}

static const struct fdt_match rpmi_suspend_match[] = {
	{ .compatible = "riscv,rpmi-system-suspend" },
	{},
};

const struct fdt_driver fdt_suspend_rpmi = {
	.match_table = rpmi_suspend_match,
	.init = rpmi_suspend_init,
};
