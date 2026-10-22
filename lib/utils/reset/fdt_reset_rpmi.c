/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 */

#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/mailbox/fdt_mailbox.h>
#समावेश <sbi_utils/mailbox/rpmi_msgprot.h>
#समावेश <sbi_utils/mailbox/rpmi_mailbox.h>

/* struct rpmi_sysreset: RPMI System Reset Context */
काष्ठा rpmi_sysreset {
	पूर्णांक warm_reset_support;
	काष्ठा mbox_chan *chan;
};

अटल काष्ठा rpmi_sysreset sysreset_ctx;

अटल पूर्णांक rpmi_system_reset_type_check(u32 reset_type)
{
	पूर्णांक ret;
	काष्ठा rpmi_sysrst_get_reset_attributes_resp resp;

	ret = rpmi_normal_request_with_status(sysreset_ctx.chan,
		RPMI_SYSRST_SRV_GET_ATTRIBUTES, &reset_type,
		rpmi_u32_गिनती(reset_type), rpmi_u32_गिनती(reset_type),
		&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
	अगर (ret) {
		वापस 0;
	}

	वापस (resp.flags & RPMI_SYSRST_ATTRS_FLAGS_RESETTYPE_MASK) ? 1 : 0;
}

/**
 * rpmi_do_system_reset: Do system reset
 *
 * @reset_type: RPMI System Reset Type
 */
अटल व्योम rpmi_do_system_reset(u32 reset_type)
{
	पूर्णांक ret;

	ret = rpmi_posted_request(sysreset_ctx.chan,
				  RPMI_SYSRST_SRV_SYSTEM_RESET,
				  &reset_type, rpmi_u32_गिनती(reset_type),
				  rpmi_u32_गिनती(reset_type));
	अगर (ret)
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
अटल पूर्णांक rpmi_system_reset_check(u32 type, u32 reason)
{
	चयन (type) {
	हाल SBI_SRST_RESET_TYPE_SHUTDOWN:
	हाल SBI_SRST_RESET_TYPE_COLD_REBOOT:
		वापस 1;
	हाल SBI_SRST_RESET_TYPE_WARM_REBOOT:
		वापस sysreset_ctx.warm_reset_support;
	शेष:
		वापस 0;
	}
}

अटल व्योम rpmi_system_reset(u32 type, u32 reason)
{
	u32 reset_type;

	चयन (type) {
	हाल SBI_SRST_RESET_TYPE_SHUTDOWN:
		reset_type = RPMI_SYSRST_TYPE_SHUTDOWN;
		अवरोध;
	हाल SBI_SRST_RESET_TYPE_COLD_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_COLD_REBOOT;
		अवरोध;
	हाल SBI_SRST_RESET_TYPE_WARM_REBOOT:
		reset_type = RPMI_SYSRST_TYPE_WARM_REBOOT;
		अवरोध;
	शेष:
		वापस;
	}

	rpmi_do_system_reset(reset_type);
}

अटल काष्ठा sbi_system_reset_device rpmi_reset_dev = {
	.name = "rpmi-system-reset",
	.system_reset_check = rpmi_system_reset_check,
	.system_reset = rpmi_system_reset,
};

अटल पूर्णांक rpmi_reset_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			   स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक ret;

	/* If channel already available then do nothing. */
	अगर (sysreset_ctx.chan)
		वापस 0;

	/*
	 * If channel request failed then other end does not support
	 * system reset group so do nothing.
	 */
	ret = fdt_mailbox_request_chan(fdt, nodeoff, 0, &sysreset_ctx.chan);
	अगर (ret)
		वापस SBI_ENODEV;

	sysreset_ctx.warm_reset_support =
			rpmi_system_reset_type_check(RPMI_SYSRST_TYPE_WARM_REBOOT);

	sbi_system_reset_add_device(&rpmi_reset_dev);

	वापस SBI_OK;
}

अटल स्थिर काष्ठा fdt_match rpmi_reset_match[] = {
	{ .compatible = "riscv,rpmi-system-reset" },
	{},
};

स्थिर काष्ठा fdt_driver fdt_reset_rpmi = {
	.match_table = rpmi_reset_match,
	.init = rpmi_reset_init,
};
