/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#include <sbi/sbi_hart.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_system.h>
#include <sbi/sbi_console.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/mailbox/fdt_mailbox.h>
#include <sbi_utils/mailbox/rpmi_msgprot.h>
#include <sbi_utils/mailbox/rpmi_mailbox.h>

/* struct rpmi_sysreset: RPMI System Reset Context */
struct rpmi_sysreset {
	整数 warm_reset_support;
	struct mbox_chan *chan;
};

static struct rpmi_sysreset sysreset_ctx;

static 整数 rpmi_system_reset_type_check(u32 reset_type)
{
	整数 ret;
	struct rpmi_sysrst_get_reset_attributes_resp resp;

	ret = rpmi_normal_request_with_status(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ATTRIBUTES, &reset_type,
		rpmi_u32_count(reset_type), rpmi_u32_count(reset_type),
		&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	如果 (ret) {
		返回 0;
	}

	返回 (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_MASK) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
static void rpmi_do_system_reset(u32 reset_type)
{
	整数 ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_count(reset_type),
				  rpmi_u32_count(reset_type));
	如果 (ret)
		sbi_printf("system reset failed [type: %d]: ret: %d\n",
			   reset_type, ret);

	sbi_hart_hang();
}

/**
 * rpmi_system_reset_check: Check the support for
 * various reset types
 *
 * @type: SBI System Reset Type
 * @reason: Reason for system reset
 */
static 整数 rpmi_system_reset_check(u32 type, u32 reason)
{
	switch (type) {
	case SBI_SRST_RESET_TYPE_SHUTDOWN:
	case SBI_SRST_RESET_TYPE_COLD_REBOOT:
		返回 1;
	case SBI_SRST_RESET_TYPE_WARM_REBOOT:
		返回 sysreset_ctx.warm_reset_support;
	default:
		返回 0;
	}
}

static void rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	switch (type) {
	case SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		break;
	case SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		break;
	case SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		break;
	default:
		返回;
	}

	rpmi_do_system_reset(reset_type);
}

static struct sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

static 整数 rpmi_reset_init(const void *fdt, 整数 nodeoff,
			   const struct fdt_match *match)
{
	整数 ret;

	/* If channel already available then do nothing. */
	如果 (sysreset_ctx.chan)
		返回 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	如果 (ret)
		返回 SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_add_device(&rpmi_reset_dev);

	返回 SBI_OK;
}

static const struct fdt_match rpmi_reset_match[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

const struct fdt_driver fdt_reset_rpmi = {
	.match_table = rpmi_reset_match,
	.init = rpmi_reset_init,
};
