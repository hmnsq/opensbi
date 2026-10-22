/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/fdt_mailbox.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/rpmi_msgprot.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/rpmi_mailbox.h>

/* struct rpmi_sysreset: RPMI System Reset Context */
ഘടന rpmi_sysreset {
	പൂർണ്ണസംഖ്യ warm_reset_support;
	ഘടന mbox_chan *chan;
};

സ്ഥിതം ഘടന rpmi_sysreset sysreset_ctx;

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_system_reset_type_check(u32 reset_type)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന rpmi_sysrst_get_reset_attributes_resp resp;

	ret = rpmi_normal_request_with_status(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ATTRIBUTES, &reset_type,
		rpmi_u32_എണ്ണം(reset_type), rpmi_u32_എണ്ണം(reset_type),
		&resp, rpmi_u32_എണ്ണം(resp), rpmi_u32_എണ്ണം(resp));
	എങ്കിൽ (ret) {
		തിരികെ 0;
	}

	തിരികെ (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_MASK) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
സ്ഥിതം ശൂന്യം rpmi_do_system_reset(u32 reset_type)
{
	പൂർണ്ണസംഖ്യ ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_എണ്ണം(reset_type),
				  rpmi_u32_എണ്ണം(reset_type));
	എങ്കിൽ (ret)
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
സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_system_reset_check(u32 type, u32 reason)
{
	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ SBI_SRST_RESET_TYPE_SHUTDOWN:
	അവസ്ഥ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		തിരികെ 1;
	അവസ്ഥ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		തിരികെ sysreset_ctx.warm_reset_support;
	സ്ഥിരസ്ഥിതി:
		തിരികെ 0;
	}
}

സ്ഥിതം ശൂന്യം rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		നിർത്തുക;
	അവസ്ഥ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		നിർത്തുക;
	അവസ്ഥ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		തിരികെ;
	}

	rpmi_do_system_reset(reset_type);
}

സ്ഥിതം ഘടന sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_reset_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			   സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ ret;

	/* If channel already available then do nothing. */
	എങ്കിൽ (sysreset_ctx.chan)
		തിരികെ 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	എങ്കിൽ (ret)
		തിരികെ SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_add_device(&rpmi_reset_dev);

	തിരികെ SBI_OK;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match rpmi_reset_match[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

സ്ഥിരം ഘടന fdt_driver fdt_reset_rpmi = {
	.match_table = rpmi_reset_match,
	.init = rpmi_reset_init,
};
