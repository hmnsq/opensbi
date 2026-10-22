/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/mailbox/fdt_mailbox.h>
#અન્તર્ભાવ <sbi_utils/mailbox/rpmi_msgprot.h>
#અન્તર્ભાવ <sbi_utils/mailbox/rpmi_mailbox.h>

/* struct rpmi_sysreset: RPMI System Reset Context */
સંરચના rpmi_sysreset {
	પૂર્ણાંક warm_reset_support;
	સંરચના mbox_chan *chan;
};

સ્થિર સંરચના rpmi_sysreset sysreset_ctx;

સ્થિર પૂર્ણાંક rpmi_system_reset_type_check(u32 reset_type)
{
	પૂર્ણાંક ret;
	સંરચના rpmi_sysrst_get_reset_attributes_resp resp;

	ret = rpmi_normal_request_with_status(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ATTRIBUTES, &reset_type,
		rpmi_u32_ગણતરી(reset_type), rpmi_u32_ગણતરી(reset_type),
		&resp, rpmi_u32_ગણતરી(resp), rpmi_u32_ગણતરી(resp));
	જો (ret) {
		પાછા 0;
	}

	પાછા (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_MASK) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
સ્થિર શૂન્ય rpmi_do_system_reset(u32 reset_type)
{
	પૂર્ણાંક ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_ગણતરી(reset_type),
				  rpmi_u32_ગણતરી(reset_type));
	જો (ret)
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
સ્થિર પૂર્ણાંક rpmi_system_reset_check(u32 type, u32 reason)
{
	પસંદગી (type) {
	સ્થિતિ SBI_SRST_RESET_TYPE_SHUTDOWN:
	સ્થિતિ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		પાછા 1;
	સ્થિતિ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		પાછા sysreset_ctx.warm_reset_support;
	મૂળભૂત:
		પાછા 0;
	}
}

સ્થિર શૂન્ય rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	પસંદગી (type) {
	સ્થિતિ SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		તોડો;
	સ્થિતિ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		તોડો;
	સ્થિતિ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		તોડો;
	મૂળભૂત:
		પાછા;
	}

	rpmi_do_system_reset(reset_type);
}

સ્થિર સંરચના sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

સ્થિર પૂર્ણાંક rpmi_reset_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			   અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક ret;

	/* If channel already available then do nothing. */
	જો (sysreset_ctx.chan)
		પાછા 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	જો (ret)
		પાછા SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_add_device(&rpmi_reset_dev);

	પાછા SBI_OK;
}

સ્થિર અચલ સંરચના fdt_match rpmi_reset_match[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

અચલ સંરચના fdt_driver fdt_reset_rpmi = {
	.match_table = rpmi_reset_match,
	.init = rpmi_reset_init,
};
