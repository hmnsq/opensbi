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
কাঠামো rpmi_sysreset {
	পূর্ণসংখ্যা warm_reset_support;
	কাঠামো mbox_chan *chan;
};

স্থির কাঠামো rpmi_sysreset sysreset_ctx;

স্থির পূর্ণসংখ্যা rpmi_system_reset_type_check(u32 reset_type)
{
	পূর্ণসংখ্যা ret;
	কাঠামো rpmi_sysrst_get_reset_attributes_resp resp;

	ret = rpmi_normal_request_with_status(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ATTRIBUTES, &reset_type,
		rpmi_u32_গুনতি(reset_type), rpmi_u32_গুনতি(reset_type),
		&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
	যদি (ret) {
		ফেরত 0;
	}

	ফেরত (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_MASK) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
স্থির শূন্য rpmi_do_system_reset(u32 reset_type)
{
	পূর্ণসংখ্যা ret;

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
স্থির পূর্ণসংখ্যা rpmi_system_reset_check(u32 type, u32 reason)
{
	নির্বাচন (type) {
	ক্ষেত্র SBI_SRST_RESET_TYPE_SHUTDOWN:
	ক্ষেত্র SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ফেরত 1;
	ক্ষেত্র SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ফেরত sysreset_ctx.warm_reset_support;
	পূর্বনির্ধারিত:
		ফেরত 0;
	}
}

স্থির শূন্য rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	নির্বাচন (type) {
	ক্ষেত্র SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		বিরতি;
	ক্ষেত্র SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		বিরতি;
	ক্ষেত্র SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		বিরতি;
	পূর্বনির্ধারিত:
		ফেরত;
	}

	rpmi_do_system_reset(reset_type);
}

স্থির কাঠামো sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

স্থির পূর্ণসংখ্যা rpmi_reset_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			   ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা ret;

	/* If channel already available then do nothing. */
	যদি (sysreset_ctx.chan)
		ফেরত 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	যদি (ret)
		ফেরত SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_add_device(&rpmi_reset_dev);

	ফেরত SBI_OK;
}

স্থির ধ্রুবক কাঠামো fdt_match rpmi_reset_match[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

ধ্রুবক কাঠামো fdt_driver fdt_reset_rpmi = {
	.match_table = rpmi_reset_match,
	.init = rpmi_reset_init,
};
