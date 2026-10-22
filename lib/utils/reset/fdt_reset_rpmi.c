/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/mailbox/fdt_mailbox.h>
#شامل <sbi_utils/mailbox/rpmi_msgprot.h>
#شامل <sbi_utils/mailbox/rpmi_mailbox.h>

/* struct rpmi_sysreset: RPMI System Reset Context */
ساختار rpmi_sysreset {
	صحیح warm_reset_support;
	ساختار mbox_chan *chan;
};

ایستا ساختار rpmi_sysreset sysreset_ctx;

ایستا صحیح rpmi_system_reset_type_check(u32 reset_type)
{
	صحیح ret;
	ساختار rpmi_sysrst_get_reset_attributes_resp resp;

	ret = rpmi_normal_request_with_status(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ATTRIBUTES, &reset_type,
		rpmi_u32_شمار(reset_type), rpmi_u32_شمار(reset_type),
		&resp, rpmi_u32_شمار(resp), rpmi_u32_شمار(resp));
	اگر (ret) {
		بازگشت 0;
	}

	بازگشت (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_MASK) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
ایستا تهی rpmi_do_system_reset(u32 reset_type)
{
	صحیح ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_شمار(reset_type),
				  rpmi_u32_شمار(reset_type));
	اگر (ret)
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
ایستا صحیح rpmi_system_reset_check(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
		بازگشت 1;
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		بازگشت sysreset_ctx.warm_reset_support;
	پیشفرض:
		بازگشت 0;
	}
}

ایستا تهی rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		بشکن;
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		بشکن;
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		بشکن;
	پیشفرض:
		بازگشت;
	}

	rpmi_do_system_reset(reset_type);
}

ایستا ساختار sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

ایستا صحیح rpmi_reset_init(ثابت تهی *fdt, صحیح nodeoff,
			   ثابت ساختار fdt_match *match)
{
	صحیح ret;

	/* If channel already available then do nothing. */
	اگر (sysreset_ctx.chan)
		بازگشت 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	اگر (ret)
		بازگشت SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_add_device(&rpmi_reset_dev);

	بازگشت SBI_OK;
}

ایستا ثابت ساختار fdt_match rpmi_reset_match[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

ثابت ساختار fdt_driver fdt_reset_rpmi = {
	.match_table = rpmi_reset_match,
	.init = rpmi_reset_init,
};
