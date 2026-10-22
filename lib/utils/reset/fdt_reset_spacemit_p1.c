/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <platform_override.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/i2c/fdt_i2c.h>

/* SpacemiT P1 Power Control Register 2 */
#నిర్వచించు PWR_CTRL2		0x7e
#నిర్వచించు PWR_CTRL2_SHUTDOWN	బిట్టు(2)	/* Shutdown request */
#నిర్వచించు PWR_CTRL2_RST		బిట్టు(1)	/* Reset request */

స్థిర నిర్మాణం i2c_adapter *p1_adapter = ఖాళీ;
స్థిర uint32_t p1_reg = 0;

స్థిర పూర్ణాంకం p1_system_reset_check(uint32_t type, uint32_t reason)
{
	ఎంపిక (type) {
	స్థితి SBI_SRST_RESET_TYPE_SHUTDOWN:
		తిరిగి 1;
	స్థితి SBI_SRST_RESET_TYPE_COLD_REBOOT:
	స్థితి SBI_SRST_RESET_TYPE_WARM_REBOOT:
		తిరిగి 255;
	}

	తిరిగి 0;
}

స్థిర శూన్య p1_ops(uint32_t type)
{
	uint8_t byte;
	పూర్ణాంకం rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	ఒకవేళ (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		తిరిగి;
	}

	ఒకవేళ (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	లేకపోతే
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	ఒకవేళ (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

స్థిర శూన్య p1_system_reset(uint32_t type, uint32_t reason)
{
	ఎంపిక (type) {
	స్థితి SBI_SRST_RESET_TYPE_SHUTDOWN:
	స్థితి SBI_SRST_RESET_TYPE_COLD_REBOOT:
	స్థితి SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		ఆపు;
	}
}

స్థిర నిర్మాణం sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

స్థిర పూర్ణాంకం p1_reset_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			 స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &addr, ఖాళీ);
	ఒకవేళ (rc)
		తిరిగి rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_మాతృ_స్థానభ్రంశం(fdt, nodeoff);
	ఒకవేళ (i2c_bus < 0)
		తిరిగి i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	ఒకవేళ (rc)
		తిరిగి rc;

	sbi_system_reset_కూడు_device(&p1_reset);

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక p1_reset_సరిపోలిక[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_reset_spacemit_p1 = {
	.సరిపోలిక_పట్టిక = p1_reset_సరిపోలిక,
	.init = p1_reset_init,
};
