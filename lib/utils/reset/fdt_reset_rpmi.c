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
sa_nracanaa rpmi_sysreset {
	puur_naa_nka warm_reset_support;
	sa_nracanaa mbox_chan *chan;
};

static sa_nracanaa rpmi_sysreset sysreset_ctx;

static puur_naa_nka rpmi_system_reset_type_check(u32 reset_type)
{
	puur_naa_nka ret;
	sa_nracanaa rpmi_sysrst_get_reset_attributes_resp resp;

	ret = rpmi_normal_request_with_status(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ATTRIBUTES, &reset_type,
		rpmi_u32_count(reset_type), rpmi_u32_count(reset_type),
		&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	ပေႃး (ret) {
		ပွၵ်ႈ 0;
	}

	ပွၵ်ႈ (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_MASK) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
static void rpmi_do_system_reset(u32 reset_type)
{
	puur_naa_nka ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_count(reset_type),
				  rpmi_u32_count(reset_type));
	ပေႃး (ret)
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
static puur_naa_nka rpmi_system_reset_check(u32 type, u32 reason)
{
	switch (type) {
	pak_sa SBI_SRST_RESET_TYPE_SHUTDOWN:
	pak_sa SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ပွၵ်ႈ 1;
	pak_sa SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ပွၵ်ႈ sysreset_ctx.warm_reset_support;
	default:
		ပွၵ်ႈ 0;
	}
}

static void rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	switch (type) {
	pak_sa SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		bha_nga;
	pak_sa SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		bha_nga;
	pak_sa SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		bha_nga;
	default:
		ပွၵ်ႈ;
	}

	rpmi_do_system_reset(reset_type);
}

static sa_nracanaa sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

static puur_naa_nka rpmi_reset_init(const void *fdt, puur_naa_nka nodeoff,
			   const sa_nracanaa fdt_match *match)
{
	puur_naa_nka ret;

	/* If channel already available then do nothing. */
	ပေႃး (sysreset_ctx.chan)
		ပွၵ်ႈ 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	ပေႃး (ret)
		ပွၵ်ႈ SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_add_device(&rpmi_reset_dev);

	ပွၵ်ႈ SBI_OK;
}

static const sa_nracanaa fdt_match rpmi_reset_match[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

const sa_nracanaa fdt_driver fdt_reset_rpmi = {
	.match_table = rpmi_reset_match,
	.init = rpmi_reset_init,
};
