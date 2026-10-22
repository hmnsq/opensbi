/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/mailbox/fdt_mailbox.h>
#அந்தர்பாவ <sbi_utils/mailbox/rpmi_msgprot.h>
#அந்தர்பாவ <sbi_utils/mailbox/rpmi_mailbox.h>

/* struct rpmi_sysreset: RPMI System Reset Context */
கட்டமைப்பு rpmi_sysreset {
	முழு warm_reset_support;
	கட்டமைப்பு mbox_chan *chan;
};

நிலையான கட்டமைப்பு rpmi_sysreset sysreset_ctx;

நிலையான முழு rpmi_system_reset_type_check(u32 reset_type)
{
	முழு ret;
	கட்டமைப்பு rpmi_sysrst_get_reset_attributes_resp resp;

	ret = rpmi_normal_request_with_status(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ATTRIBUTES, &reset_type,
		rpmi_u32_மொத்தம்(reset_type), rpmi_u32_மொத்தம்(reset_type),
		&resp, rpmi_u32_மொத்தம்(resp), rpmi_u32_மொத்தம்(resp));
	எனில் (ret) {
		திரும்பு 0;
	}

	திரும்பு (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_MASK) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
நிலையான வெறுமை rpmi_do_system_reset(u32 reset_type)
{
	முழு ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_மொத்தம்(reset_type),
				  rpmi_u32_மொத்தம்(reset_type));
	எனில் (ret)
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
நிலையான முழு rpmi_system_reset_check(u32 type, u32 reason)
{
	தேர்வு (type) {
	நிலை SBI_SRST_RESET_TYPE_SHUTDOWN:
	நிலை SBI_SRST_RESET_TYPE_COLD_REBOOT:
		திரும்பு 1;
	நிலை SBI_SRST_RESET_TYPE_WARM_REBOOT:
		திரும்பு sysreset_ctx.warm_reset_support;
	இயல்பு:
		திரும்பு 0;
	}
}

நிலையான வெறுமை rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	தேர்வு (type) {
	நிலை SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		நிறுத்து;
	நிலை SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		நிறுத்து;
	நிலை SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		நிறுத்து;
	இயல்பு:
		திரும்பு;
	}

	rpmi_do_system_reset(reset_type);
}

நிலையான கட்டமைப்பு sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

நிலையான முழு rpmi_reset_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			   மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு ret;

	/* If channel already available then do nothing. */
	எனில் (sysreset_ctx.chan)
		திரும்பு 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	எனில் (ret)
		திரும்பு SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_add_device(&rpmi_reset_dev);

	திரும்பு SBI_OK;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match rpmi_reset_match[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

மாறிலி கட்டமைப்பு fdt_driver fdt_reset_rpmi = {
	.match_table = rpmi_reset_match,
	.init = rpmi_reset_init,
};
