/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/mailbox/fdt_mailbox.h>
#अन्तर्भाव <sbi_utils/mailbox/rpmi_msgprot.h>
#अन्तर्भाव <sbi_utils/mailbox/rpmi_mailbox.h>

/* struct rpmi_sysreset: RPMI System Reset Context */
संरचना rpmi_sysreset {
	पूर्णाङ्क warm_reset_support;
	संरचना mbox_chan *chan;
};

स्थिर संरचना rpmi_sysreset sysreset_ctx;

स्थिर पूर्णाङ्क rpmi_system_reset_type_check(u32 reset_type)
{
	पूर्णाङ्क ret;
	संरचना rpmi_sysrst_get_reset_attributes_resp resp;

	ret = rpmi_normal_request_with_status(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ATTRIBUTES, &reset_type,
		rpmi_u32_मोजणी(reset_type), rpmi_u32_मोजणी(reset_type),
		&resp, rpmi_u32_मोजणी(resp), rpmi_u32_मोजणी(resp));
	यदि (ret) {
		प्रत्यागम 0;
	}

	प्रत्यागम (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_MASK) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
स्थिर शून्य rpmi_do_system_reset(u32 reset_type)
{
	पूर्णाङ्क ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_मोजणी(reset_type),
				  rpmi_u32_मोजणी(reset_type));
	यदि (ret)
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
स्थिर पूर्णाङ्क rpmi_system_reset_check(u32 type, u32 reason)
{
	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
		प्रत्यागम 1;
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
		प्रत्यागम sysreset_ctx.warm_reset_support;
	स्वभाव:
		प्रत्यागम 0;
	}
}

स्थिर शून्य rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		भङ्ग;
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		भङ्ग;
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		भङ्ग;
	स्वभाव:
		प्रत्यागम;
	}

	rpmi_do_system_reset(reset_type);
}

स्थिर संरचना sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

स्थिर पूर्णाङ्क rpmi_reset_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			   ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क ret;

	/* If channel already available then do nothing. */
	यदि (sysreset_ctx.chan)
		प्रत्यागम 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	यदि (ret)
		प्रत्यागम SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_add_device(&rpmi_reset_dev);

	प्रत्यागम SBI_OK;
}

स्थिर ध्रुव संरचना fdt_match rpmi_reset_match[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

ध्रुव संरचना fdt_driver fdt_reset_rpmi = {
	.match_table = rpmi_reset_match,
	.init = rpmi_reset_init,
};
