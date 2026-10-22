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
	पूर्णांक warm_reset_support;
	संरचना mbox_chan *chan;
};

स्थिर संरचना rpmi_sysreset sysreset_ctx;

स्थिर पूर्णांक rpmi_system_reset_type_check(u32 reset_type)
{
	पूर्णांक ret;
	संरचना rpmi_sysrst_get_reset_attributes_resp resp;

	ret = rpmi_normal_request_with_status(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ATTRIBUTES, &reset_type,
		rpmi_u32_मोजणी(reset_type), rpmi_u32_मोजणी(reset_type),
		&resp, rpmi_u32_मोजणी(resp), rpmi_u32_मोजणी(resp));
	जर (ret) {
		परत 0;
	}

	परत (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_MASK) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
स्थिर शून्य rpmi_do_system_reset(u32 reset_type)
{
	पूर्णांक ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_मोजणी(reset_type),
				  rpmi_u32_मोजणी(reset_type));
	जर (ret)
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
स्थिर पूर्णांक rpmi_system_reset_check(u32 type, u32 reason)
{
	निवड (type) {
	स्थिती SBI_SRST_RESET_TYPE_SHUTDOWN:
	स्थिती SBI_SRST_RESET_TYPE_COLD_REBOOT:
		परत 1;
	स्थिती SBI_SRST_RESET_TYPE_WARM_REBOOT:
		परत sysreset_ctx.warm_reset_support;
	पूर्वनिर्धारित:
		परत 0;
	}
}

स्थिर शून्य rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	निवड (type) {
	स्थिती SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		थांबा;
	स्थिती SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		थांबा;
	स्थिती SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		थांबा;
	पूर्वनिर्धारित:
		परत;
	}

	rpmi_do_system_reset(reset_type);
}

स्थिर संरचना sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

स्थिर पूर्णांक rpmi_reset_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
			   स्थिरांक संरचना fdt_match *match)
{
	पूर्णांक ret;

	/* If channel already available then do nothing. */
	जर (sysreset_ctx.chan)
		परत 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	जर (ret)
		परत SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_add_device(&rpmi_reset_dev);

	परत SBI_OK;
}

स्थिर स्थिरांक संरचना fdt_match rpmi_reset_match[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

स्थिरांक संरचना fdt_driver fdt_reset_rpmi = {
	.match_table = rpmi_reset_match,
	.init = rpmi_reset_init,
};
