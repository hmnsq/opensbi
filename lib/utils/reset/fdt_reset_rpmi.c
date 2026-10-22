/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/fdt_mailbox.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/rpmi_msgprot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/rpmi_mailbox.h>

/* struct rpmi_sysreset: RPMI System Reset Context */
ਬਣਤਰ rpmi_sysreset {
	ਪੂਰਨ warm_reset_support;
	ਬਣਤਰ mbox_chan *chan;
};

ਸਥਿਰ ਬਣਤਰ rpmi_sysreset sysreset_ctx;

ਸਥਿਰ ਪੂਰਨ rpmi_system_reset_type_check(u32 reset_type)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ rpmi_sysrst_get_reset_ਗੁਣ_resp resp;

	ret = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ਗੁਣ, &reset_type,
		rpmi_u32_ਗਿਣਤੀ(reset_type), rpmi_u32_ਗਿਣਤੀ(reset_type),
		&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
	ਜੇ (ret) {
		ਵਾਪਸ 0;
	}

	ਵਾਪਸ (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_ਨਕਾਬ) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
ਸਥਿਰ ਖਾਲੀ rpmi_do_system_reset(u32 reset_type)
{
	ਪੂਰਨ ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_ਗਿਣਤੀ(reset_type),
				  rpmi_u32_ਗਿਣਤੀ(reset_type));
	ਜੇ (ret)
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
ਸਥਿਰ ਪੂਰਨ rpmi_system_reset_check(u32 type, u32 reason)
{
	ਚੋਣ (type) {
	ਹਾਲਤ SBI_SRST_RESET_TYPE_SHUTDOWN:
	ਹਾਲਤ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ਵਾਪਸ 1;
	ਹਾਲਤ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ਵਾਪਸ sysreset_ctx.warm_reset_support;
	ਮੂਲ:
		ਵਾਪਸ 0;
	}
}

ਸਥਿਰ ਖਾਲੀ rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	ਚੋਣ (type) {
	ਹਾਲਤ SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		ਰੋਕੋ;
	ਮੂਲ:
		ਵਾਪਸ;
	}

	rpmi_do_system_reset(reset_type);
}

ਸਥਿਰ ਬਣਤਰ sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

ਸਥਿਰ ਪੂਰਨ rpmi_reset_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			   ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ ret;

	/* If channel already available then do nothing. */
	ਜੇ (sysreset_ctx.chan)
		ਵਾਪਸ 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	ਜੇ (ret)
		ਵਾਪਸ SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_ਜੋੜੋ_device(&rpmi_reset_dev);

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ rpmi_reset_ਮੇਲ[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_reset_rpmi = {
	.ਮੇਲ_ਸਾਰਣੀ = rpmi_reset_ਮੇਲ,
	.init = rpmi_reset_init,
};
