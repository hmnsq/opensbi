/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/fdt_mailbox.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/rpmi_msgprot.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/rpmi_mailbox.h>

/* struct rpmi_sysreset: RPMI System Reset Context */
ಸಂರಚನಾ rpmi_sysreset {
	ಪೂರ್ಣಾಙ್ಕ warm_reset_support;
	ಸಂರಚನಾ mbox_chan *chan;
};

ಸ್ಥಿರ ಸಂರಚನಾ rpmi_sysreset sysreset_ctx;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_system_reset_type_check(u32 reset_type)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ rpmi_sysrst_get_reset_attributes_resp resp;

	ret = rpmi_normal_request_with_status(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ATTRIBUTES, &reset_type,
		rpmi_u32_ಎಣಿಕೆ(reset_type), rpmi_u32_ಎಣಿಕೆ(reset_type),
		&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಯದಿ (ret) {
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಪ್ರತ್ಯಾಗಮ (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_MASK) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
ಸ್ಥಿರ ಶೂನ್ಯ rpmi_do_system_reset(u32 reset_type)
{
	ಪೂರ್ಣಾಙ್ಕ ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_ಎಣಿಕೆ(reset_type),
				  rpmi_u32_ಎಣಿಕೆ(reset_type));
	ಯದಿ (ret)
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_system_reset_check(u32 type, u32 reason)
{
	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_SHUTDOWN:
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ಪ್ರತ್ಯಾಗಮ 1;
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಪ್ರತ್ಯಾಗಮ sysreset_ctx.warm_reset_support;
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ 0;
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ;
	}

	rpmi_do_system_reset(reset_type);
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_reset_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			   ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ ret;

	/* If channel already available then do nothing. */
	ಯದಿ (sysreset_ctx.chan)
		ಪ್ರತ್ಯಾಗಮ 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_add_device(&rpmi_reset_dev);

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match rpmi_reset_match[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_reset_rpmi = {
	.match_table = rpmi_reset_match,
	.init = rpmi_reset_init,
};
