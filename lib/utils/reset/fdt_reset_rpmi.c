/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/fdt_mailbox.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/rpmi_msgprot.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/rpmi_mailbox.h>

/* struct rpmi_sysreset: RPMI System Reset Context */
ସଂରଚନା rpmi_sysreset {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା warm_reset_support;
	ସଂରଚନା mbox_chan *chan;
};

ସ୍ଥିର ସଂରଚନା rpmi_sysreset sysreset_ctx;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_system_reset_type_check(u32 reset_type)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା rpmi_sysrst_get_reset_attributes_resp resp;

	ret = rpmi_normal_request_with_status(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ATTRIBUTES, &reset_type,
		rpmi_u32_ଗଣନା(reset_type), rpmi_u32_ଗଣନା(reset_type),
		&resp, rpmi_u32_ଗଣନା(resp), rpmi_u32_ଗଣନା(resp));
	ଯଦି (ret) {
		ଫେରନ୍ତୁ 0;
	}

	ଫେରନ୍ତୁ (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_MASK) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
ସ୍ଥିର ଶୂନ୍ୟ rpmi_do_system_reset(u32 reset_type)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_ଗଣନା(reset_type),
				  rpmi_u32_ଗଣନା(reset_type));
	ଯଦି (ret)
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_system_reset_check(u32 type, u32 reason)
{
	ବାଛ (type) {
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_SHUTDOWN:
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ଫେରନ୍ତୁ 1;
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ଫେରନ୍ତୁ sysreset_ctx.warm_reset_support;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ 0;
	}
}

ସ୍ଥିର ଶୂନ୍ୟ rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	ବାଛ (type) {
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ;
	}

	rpmi_do_system_reset(reset_type);
}

ସ୍ଥିର ସଂରଚନା sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_reset_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			   ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	/* If channel already available then do nothing. */
	ଯଦି (sysreset_ctx.chan)
		ଫେରନ୍ତୁ 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	ଯଦି (ret)
		ଫେରନ୍ତୁ SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_add_device(&rpmi_reset_dev);

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match rpmi_reset_match[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_reset_rpmi = {
	.match_table = rpmi_reset_match,
	.init = rpmi_reset_init,
};
