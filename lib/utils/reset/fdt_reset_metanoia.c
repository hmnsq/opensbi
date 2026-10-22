/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Metanoia Communications Inc.
 *
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>

#घोषणा SWRSTREQ_CTRL_REG_OFFSET 0x00
#घोषणा SWRSTREQ_REG_OFFSET 0x04

काष्ठा reset_metanoia_तथ्य {
	u8 *reg_base;
};

अटल काष्ठा reset_metanoia_तथ्य reset_तथ्य;

अटल पूर्णांक metanoia_system_reset_check(u32 type, u32 reason)
{
	चयन (type) {
	हाल SBI_SRST_RESET_TYPE_WARM_REBOOT:
	हाल SBI_SRST_RESET_TYPE_COLD_REBOOT:
		वापस 1;
	हाल SBI_SRST_RESET_TYPE_SHUTDOWN:
	शेष:
		वापस 0;
	}
}

अटल व्योम metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_तथ्य.reg_base + SWRSTREQ_CTRL_REG_OFFSET);
	writew(0x1, reset_तथ्य.reg_base + SWRSTREQ_REG_OFFSET);
}

अटल काष्ठा sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

अटल पूर्णांक metanoia_reset_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			       स्थिर काष्ठा fdt_match *match)
{
	u64 reg_addr;
	पूर्णांक rc;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &reg_addr, शून्य);
	अगर (rc < 0 || !reg_addr)
		वापस SBI_ENODEV;

	reset_तथ्य.reg_base = (u8 *)(ulong)reg_addr;

	sbi_system_reset_add_device(&metanoia_reset);

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match metanoia_reset_match[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

स्थिर काष्ठा fdt_driver fdt_reset_metanoia = {
	.match_table = metanoia_reset_match,
	.init	     = metanoia_reset_init,
};
