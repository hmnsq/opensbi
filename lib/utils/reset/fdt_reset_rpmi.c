/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/mailbox/fdt_mailbox.h>
#అన్తర్భావ <sbi_utils/mailbox/rpmi_msgprot.h>
#అన్తర్భావ <sbi_utils/mailbox/rpmi_mailbox.h>

/* struct rpmi_sysreset: RPMI System Reset Context */
నిర్మాణం rpmi_sysreset {
	పూర్ణాంకం warm_reset_support;
	నిర్మాణం mbox_chan *chan;
};

స్థిర నిర్మాణం rpmi_sysreset sysreset_ctx;

స్థిర పూర్ణాంకం rpmi_system_reset_type_check(u32 reset_type)
{
	పూర్ణాంకం ret;
	నిర్మాణం rpmi_sysrst_get_reset_లక్షణాలు_resp resp;

	ret = rpmi_సాధారణ_request_with_స్థితి(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_లక్షణాలు, &reset_type,
		rpmi_u32_లెక్క(reset_type), rpmi_u32_లెక్క(reset_type),
		&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
	ఒకవేళ (ret) {
		తిరిగి 0;
	}

	తిరిగి (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_ముసుగు) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
స్థిర శూన్య rpmi_do_system_reset(u32 reset_type)
{
	పూర్ణాంకం ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_లెక్క(reset_type),
				  rpmi_u32_లెక్క(reset_type));
	ఒకవేళ (ret)
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
స్థిర పూర్ణాంకం rpmi_system_reset_check(u32 type, u32 reason)
{
	ఎంపిక (type) {
	స్థితి SBI_SRST_RESET_TYPE_SHUTDOWN:
	స్థితి SBI_SRST_RESET_TYPE_COLD_REBOOT:
		తిరిగి 1;
	స్థితి SBI_SRST_RESET_TYPE_WARM_REBOOT:
		తిరిగి sysreset_ctx.warm_reset_support;
	అప్రమేయ:
		తిరిగి 0;
	}
}

స్థిర శూన్య rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	ఎంపిక (type) {
	స్థితి SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		ఆపు;
	స్థితి SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		ఆపు;
	స్థితి SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		ఆపు;
	అప్రమేయ:
		తిరిగి;
	}

	rpmi_do_system_reset(reset_type);
}

స్థిర నిర్మాణం sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

స్థిర పూర్ణాంకం rpmi_reset_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			   స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం ret;

	/* If channel already available then do nothing. */
	ఒకవేళ (sysreset_ctx.chan)
		తిరిగి 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	ఒకవేళ (ret)
		తిరిగి SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_కూడు_device(&rpmi_reset_dev);

	తిరిగి SBI_OK;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక rpmi_reset_సరిపోలిక[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_reset_rpmi = {
	.సరిపోలిక_పట్టిక = rpmi_reset_సరిపోలిక,
	.init = rpmi_reset_init,
};
