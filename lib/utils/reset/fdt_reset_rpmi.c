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
ರಚನೆ rpmi_sysreset {
	ಪೂರ್ಣಾಂಕ warm_reset_support;
	ರಚನೆ mbox_chan *chan;
};

ಸ್ಥಿರ ರಚನೆ rpmi_sysreset sysreset_ctx;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_system_reset_type_check(u32 reset_type)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ rpmi_sysrst_get_reset_ಗುಣಲಕ್ಷಣಗಳು_resp resp;

	ret = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ಗುಣಲಕ್ಷಣಗಳು, &reset_type,
		rpmi_u32_ಎಣಿಕೆ(reset_type), rpmi_u32_ಎಣಿಕೆ(reset_type),
		&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಆದರೆ (ret) {
		ಹಿಂತಿರುಗಿ 0;
	}

	ಹಿಂತಿರುಗಿ (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_ಮುಖವಾಡ) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
ಸ್ಥಿರ ಶೂನ್ಯ rpmi_do_system_reset(u32 reset_type)
{
	ಪೂರ್ಣಾಂಕ ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_ಎಣಿಕೆ(reset_type),
				  rpmi_u32_ಎಣಿಕೆ(reset_type));
	ಆದರೆ (ret)
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_system_reset_check(u32 type, u32 reason)
{
	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_SHUTDOWN:
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ಹಿಂತಿರುಗಿ 1;
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಹಿಂತಿರುಗಿ sysreset_ctx.warm_reset_support;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ 0;
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ;
	}

	rpmi_do_system_reset(reset_type);
}

ಸ್ಥಿರ ರಚನೆ sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_reset_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			   ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ ret;

	/* If channel already available then do nothing. */
	ಆದರೆ (sysreset_ctx.chan)
		ಹಿಂತಿರುಗಿ 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_ಕೂಡಿಸು_device(&rpmi_reset_dev);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ rpmi_reset_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_reset_rpmi = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = rpmi_reset_ಹೊಂದಿಕೆ,
	.init = rpmi_reset_init,
};
