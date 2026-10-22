/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_types.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/i2c/fdt_i2c.h>

#నిర్వచించు MANGO_BOARD_TYPE_ముసుగు		0x80

#నిర్వచించు REG_BOARD_TYPE			0x00
#నిర్వచించు REG_CMD				0x03

#నిర్వచించు CMD_POWEROFF			0x02
#నిర్వచించు CMD_RESET			0x03
#నిర్వచించు CMD_REBOOT			0x07

స్థిర నిర్మాణం i2c_adapter *mcu_adapter = ఖాళీ;
స్థిర uint32_t mcu_reg = 0;

స్థిర పూర్ణాంకం sg2042_mcu_reset_check(u32 type, u32 reason)
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

స్థిర శూన్య sg2042_mcu_reset(u32 type, u32 reason)
{
	ఎంపిక (type) {
	స్థితి SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		ఆపు;
	స్థితి SBI_SRST_RESET_TYPE_COLD_REBOOT:
	స్థితి SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		ఆపు;
	}
}

స్థిర నిర్మాణం sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

స్థిర పూర్ణాంకం sg2042_mcu_reset_check_board(నిర్మాణం i2c_adapter *adap, uint32_t reg)
{
	స్థిర uint8_t val;
	పూర్ణాంకం ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	ఒకవేళ (ret)
		తిరిగి ret;

	ఒకవేళ (!(val & MANGO_BOARD_TYPE_ముసుగు))
		తిరిగి SBI_ENODEV;

	తిరిగి 0;
}

స్థిర పూర్ణాంకం sg2042_mcu_reset_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				 స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &addr, ఖాళీ);
	ఒకవేళ (ret)
		తిరిగి ret;

	mcu_reg = addr;

	i2c_bus = fdt_మాతృ_స్థానభ్రంశం(fdt, nodeoff);
	ఒకవేళ (i2c_bus < 0)
		తిరిగి i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	ఒకవేళ (ret)
		తిరిగి ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_కూడు_device(&sg2042_mcu_reset_device);

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక sg2042_mcu_reset_సరిపోలిక[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_reset_sg2042_mcu = {
	.సరిపోలిక_పట్టిక = sg2042_mcu_reset_సరిపోలిక,
	.init = sg2042_mcu_reset_init,
};
