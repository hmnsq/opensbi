/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#నిర్వచించు ATCWDT200_WP_అంకె 0x5aa5
#నిర్వచించు WREN_REG 0x18
#నిర్వచించు CTRL_REG 0x10
#నిర్వచించు RST_సమయం_OFF 8
#నిర్వచించు RST_సమయం_MSK (0x3 << RST_సమయం_OFF)
#నిర్వచించు RST_CLK_128 (0 << RST_సమయం_OFF)
#నిర్వచించు RST_CLK_256 (1 << RST_సమయం_OFF)
#నిర్వచించు RST_CLK_512 (2 << RST_సమయం_OFF)
#నిర్వచించు RST_CLK_1024 (3 << RST_సమయం_OFF)
#నిర్వచించు INT_సమయం_OFF 4
#నిర్వచించు INT_సమయం_MSK (0xf << INT_సమయం_OFF)
#నిర్వచించు INT_CLK_64 (0 << INT_సమయం_OFF)
#నిర్వచించు INT_CLK_256 (1 << INT_సమయం_OFF)
#నిర్వచించు INT_CLK_1024 (2 << INT_సమయం_OFF)
#నిర్వచించు INT_CLK_2048 (3 << INT_సమయం_OFF)
#నిర్వచించు INT_CLK_4096 (4 << INT_సమయం_OFF)
#నిర్వచించు INT_CLK_8192 (5 << INT_సమయం_OFF)
#నిర్వచించు INT_CLK_16384 (6 << INT_సమయం_OFF)
#నిర్వచించు INT_CLK_32768 (7 << INT_సమయం_OFF)
#నిర్వచించు RST_EN (1 << 3)
#నిర్వచించు INT_EN (1 << 2)
#నిర్వచించు CLK_PCLK (1 << 1)
#నిర్వచించు WDT_EN (1 << 0)

#నిర్వచించు AE350_FLASH_పునాది 0x80000000

స్థిర అస్థిర అక్షరం *wdt_addr = ఖాళీ;

స్థిర పూర్ణాంకం ae350_system_reset_check(u32 type, u32 reason)
{
	ఎంపిక (type) {
	స్థితి SBI_SRST_RESET_TYPE_COLD_REBOOT:
		తిరిగి 1;
	స్థితి SBI_SRST_RESET_TYPE_SHUTDOWN:
	స్థితి SBI_SRST_RESET_TYPE_WARM_REBOOT:
	అప్రమేయ:
		తిరిగి 0;
	}
}

స్థిర శూన్య ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		ఒకవేళ (atcsmu_సమితి_reset_సదిశ(AE350_FLASH_పునాది, i))
			వెళ్ళు fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_అంకె, wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

స్థిర నిర్మాణం sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

స్థిర పూర్ణాంకం atcwdt200_reset_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	uint64_t reg_addr;
	పూర్ణాంకం rc;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &reg_addr, ఖాళీ);
	ఒకవేళ (rc < 0 || !reg_addr)
		తిరిగి SBI_ENODEV;

	wdt_addr = (అస్థిర అక్షరం *)(గుర్తులేని దీర్ఘ)reg_addr;
	sbi_system_reset_కూడు_device(&atcwdt200_reset);

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక atcwdt200_reset_సరిపోలిక[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_reset_atcwdt200 = {
	.సరిపోలిక_పట్టిక = atcwdt200_reset_సరిపోలిక,
	.init	     = atcwdt200_reset_init,
};
