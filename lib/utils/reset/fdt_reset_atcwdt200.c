/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#समावेश <libfdt.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#घोषणा ATCWDT200_WP_अंक 0x5aa5
#घोषणा WREN_REG 0x18
#घोषणा CTRL_REG 0x10
#घोषणा RST_TIME_OFF 8
#घोषणा RST_TIME_MSK (0x3 << RST_TIME_OFF)
#घोषणा RST_CLK_128 (0 << RST_TIME_OFF)
#घोषणा RST_CLK_256 (1 << RST_TIME_OFF)
#घोषणा RST_CLK_512 (2 << RST_TIME_OFF)
#घोषणा RST_CLK_1024 (3 << RST_TIME_OFF)
#घोषणा INT_TIME_OFF 4
#घोषणा INT_TIME_MSK (0xf << INT_TIME_OFF)
#घोषणा INT_CLK_64 (0 << INT_TIME_OFF)
#घोषणा INT_CLK_256 (1 << INT_TIME_OFF)
#घोषणा INT_CLK_1024 (2 << INT_TIME_OFF)
#घोषणा INT_CLK_2048 (3 << INT_TIME_OFF)
#घोषणा INT_CLK_4096 (4 << INT_TIME_OFF)
#घोषणा INT_CLK_8192 (5 << INT_TIME_OFF)
#घोषणा INT_CLK_16384 (6 << INT_TIME_OFF)
#घोषणा INT_CLK_32768 (7 << INT_TIME_OFF)
#घोषणा RST_EN (1 << 3)
#घोषणा INT_EN (1 << 2)
#घोषणा CLK_PCLK (1 << 1)
#घोषणा WDT_EN (1 << 0)

#घोषणा AE350_FLASH_BASE 0x80000000

अटल अस्थिर अक्षर *wdt_addr = शून्य;

अटल पूर्णांक ae350_system_reset_check(u32 type, u32 reason)
{
	चयन (type) {
	हाल SBI_SRST_RESET_TYPE_COLD_REBOOT:
		वापस 1;
	हाल SBI_SRST_RESET_TYPE_SHUTDOWN:
	हाल SBI_SRST_RESET_TYPE_WARM_REBOOT:
	शेष:
		वापस 0;
	}
}

अटल व्योम ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		अगर (atcsmu_set_reset_vector(AE350_FLASH_BASE, i))
			जाओ fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_अंक, wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

अटल काष्ठा sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

अटल पूर्णांक atcwdt200_reset_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				स्थिर काष्ठा fdt_match *match)
{
	uint64_t reg_addr;
	पूर्णांक rc;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &reg_addr, शून्य);
	अगर (rc < 0 || !reg_addr)
		वापस SBI_ENODEV;

	wdt_addr = (अस्थिर अक्षर *)(अचिन्हित दीर्घ)reg_addr;
	sbi_system_reset_add_device(&atcwdt200_reset);

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match atcwdt200_reset_match[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

स्थिर काष्ठा fdt_driver fdt_reset_atcwdt200 = {
	.match_table = atcwdt200_reset_match,
	.init	     = atcwdt200_reset_init,
};
