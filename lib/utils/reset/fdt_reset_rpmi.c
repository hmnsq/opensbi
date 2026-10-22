/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/mailbox/fdt_mailbox.h>
#অন্তর্ভাব <sbi_utils/mailbox/rpmi_msgprot.h>
#অন্তর্ভাব <sbi_utils/mailbox/rpmi_mailbox.h>

/* struct rpmi_sysreset: RPMI System Reset Context */
সংরচনা rpmi_sysreset {
	পূর্ণাঙ্ক warm_reset_support;
	সংরচনা mbox_chan *chan;
};

স্থির সংরচনা rpmi_sysreset sysreset_ctx;

স্থির পূর্ণাঙ্ক rpmi_system_reset_type_check(u32 reset_type)
{
	পূর্ণাঙ্ক ret;
	সংরচনা rpmi_sysrst_get_reset_attributes_resp resp;

	ret = rpmi_normal_request_with_status(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ATTRIBUTES, &reset_type,
		rpmi_u32_গুনতি(reset_type), rpmi_u32_গুনতি(reset_type),
		&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
	যদি (ret) {
		প্রত্যাগম 0;
	}

	প্রত্যাগম (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_MASK) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
স্থির শূন্য rpmi_do_system_reset(u32 reset_type)
{
	পূর্ণাঙ্ক ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_গুনতি(reset_type),
				  rpmi_u32_গুনতি(reset_type));
	যদি (ret)
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
স্থির পূর্ণাঙ্ক rpmi_system_reset_check(u32 type, u32 reason)
{
	বিকল্প (type) {
	পক্ষ SBI_SRST_RESET_TYPE_SHUTDOWN:
	পক্ষ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		প্রত্যাগম 1;
	পক্ষ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		প্রত্যাগম sysreset_ctx.warm_reset_support;
	স্বভাব:
		প্রত্যাগম 0;
	}
}

স্থির শূন্য rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	বিকল্প (type) {
	পক্ষ SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		ভঙ্গ;
	পক্ষ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		ভঙ্গ;
	পক্ষ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		ভঙ্গ;
	স্বভাব:
		প্রত্যাগম;
	}

	rpmi_do_system_reset(reset_type);
}

স্থির সংরচনা sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

স্থির পূর্ণাঙ্ক rpmi_reset_init(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			   ধ্রুব সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক ret;

	/* If channel already available then do nothing. */
	যদি (sysreset_ctx.chan)
		প্রত্যাগম 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	যদি (ret)
		প্রত্যাগম SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_add_device(&rpmi_reset_dev);

	প্রত্যাগম SBI_OK;
}

স্থির ধ্রুব সংরচনা fdt_match rpmi_reset_match[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

ধ্রুব সংরচনা fdt_driver fdt_reset_rpmi = {
	.match_table = rpmi_reset_match,
	.init = rpmi_reset_init,
};
