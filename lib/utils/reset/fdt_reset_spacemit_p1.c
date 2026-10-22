/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#समावेश <libfdt.h>
#समावेश <platform_override.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/i2c/fdt_i2c.h>

/* SpacemiT P1 Power Control Register 2 */
#घोषणा PWR_CTRL2		0x7e
#घोषणा PWR_CTRL2_SHUTDOWN	BIT(2)	/* Shutdown request */
#घोषणा PWR_CTRL2_RST		BIT(1)	/* Reset request */

अटल काष्ठा i2c_adapter *p1_adapter = शून्य;
अटल uint32_t p1_reg = 0;

अटल पूर्णांक p1_system_reset_check(uint32_t type, uint32_t reason)
{
	चयन (type) {
	हाल SBI_SRST_RESET_TYPE_SHUTDOWN:
		वापस 1;
	हाल SBI_SRST_RESET_TYPE_COLD_REBOOT:
	हाल SBI_SRST_RESET_TYPE_WARM_REBOOT:
		वापस 255;
	}

	वापस 0;
}

अटल व्योम p1_ops(uint32_t type)
{
	uint8_t byte;
	पूर्णांक rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	अगर (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		वापस;
	}

	अगर (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	अन्यथा
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	अगर (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

अटल व्योम p1_system_reset(uint32_t type, uint32_t reason)
{
	चयन (type) {
	हाल SBI_SRST_RESET_TYPE_SHUTDOWN:
	हाल SBI_SRST_RESET_TYPE_COLD_REBOOT:
	हाल SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		अवरोध;
	}
}

अटल काष्ठा sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

अटल पूर्णांक p1_reset_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			 स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &addr, शून्य);
	अगर (rc)
		वापस rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	अगर (i2c_bus < 0)
		वापस i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	अगर (rc)
		वापस rc;

	sbi_system_reset_add_device(&p1_reset);

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match p1_reset_match[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_reset_spacemit_p1 = {
	.match_table = p1_reset_match,
	.init = p1_reset_init,
};
