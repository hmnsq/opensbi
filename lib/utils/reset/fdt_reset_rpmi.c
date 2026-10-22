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
ساخت rpmi_sysreset {
	صحیح warm_reset_support;
	ساخت mbox_chan *chan;
};

ساکن ساخت rpmi_sysreset sysreset_ctx;

ساکن صحیح rpmi_system_reset_type_check(u32 reset_type)
{
	صحیح ret;
	ساخت rpmi_sysrst_get_reset_اوصاف_resp resp;

	ret = rpmi_معمول_request_with_حالت(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_اوصاف, &reset_type,
		rpmi_u32_گنتی(reset_type), rpmi_u32_گنتی(reset_type),
		&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
	اگر (ret) {
		واپس 0;
	}

	واپس (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_نقاب) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
ساکن خالی rpmi_do_system_reset(u32 reset_type)
{
	صحیح ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_گنتی(reset_type),
				  rpmi_u32_گنتی(reset_type));
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
ساکن صحیح rpmi_system_reset_check(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
		واپس 1;
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		واپس sysreset_ctx.warm_reset_support;
	طےشدہ:
		واپس 0;
	}
}

ساکن خالی rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		توڑو;
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		توڑو;
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		توڑو;
	طےشدہ:
		واپس;
	}

	rpmi_do_system_reset(reset_type);
}

ساکن ساخت sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

ساکن صحیح rpmi_reset_init(ثابت خالی *fdt, صحیح nodeoff,
			   ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح ret;

	/* If channel already available then do nothing. */
	اگر (sysreset_ctx.chan)
		واپس 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	اگر (ret)
		واپس SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_جوڑو_device(&rpmi_reset_dev);

	واپس SBI_OK;
}

ساکن ثابت ساخت fdt_مطابقت rpmi_reset_مطابقت[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

ثابت ساخت fdt_driver fdt_reset_rpmi = {
	.مطابقت_جدول = rpmi_reset_مطابقت,
	.init = rpmi_reset_init,
};
