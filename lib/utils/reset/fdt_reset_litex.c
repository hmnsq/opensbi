/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Inochi Amaoto <inochiama@gmail.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>

#घोषणा RESET_CTRL			0x0

अटल अस्थिर u32 *litex_soc_base;

अटल पूर्णांक litex_reset_check(u32 type, u32 reason)
{
	चयन (type) {
	हाल SBI_SRST_RESET_TYPE_COLD_REBOOT:
	हाल SBI_SRST_RESET_TYPE_WARM_REBOOT:
		वापस 255;
	}

	वापस 0;
}

अटल व्योम litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_base + RESET_CTRL);
}

अटल काष्ठा sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

अटल पूर्णांक litex_reset_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			    स्थिर काष्ठा fdt_match *match)
{
	uint64_t reg_addr;
	पूर्णांक rc;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &reg_addr, शून्य);
	अगर (rc < 0 || !reg_addr)
		वापस SBI_ENODEV;


	litex_soc_base = (अस्थिर u32 *)(अचिन्हित दीर्घ)reg_addr;

	sbi_system_reset_add_device(&litex_reset);

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match litex_reset_match[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_reset_litex = {
	.match_table = litex_reset_match,
	.init = litex_reset_init,
};
