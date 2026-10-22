/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_types.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/i2c/fdt_i2c.h>

#تعریف_کرو MANGO_BOARD_TYPE_نقاب		0x80

#تعریف_کرو REG_BOARD_TYPE			0x00
#تعریف_کرو REG_CMD				0x03

#تعریف_کرو CMD_POWEROFF			0x02
#تعریف_کرو CMD_RESET			0x03
#تعریف_کرو CMD_REBOOT			0x07

ساکن ساخت i2c_adapter *mcu_adapter = تہی;
ساکن uint32_t mcu_reg = 0;

ساکن صحیح sg2042_mcu_reset_check(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		واپس 1;
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		واپس 255;
	}

	واپس 0;
}

ساکن خالی sg2042_mcu_reset(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		توڑو;
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		توڑو;
	}
}

ساکن ساخت sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

ساکن صحیح sg2042_mcu_reset_check_board(ساخت i2c_adapter *adap, uint32_t reg)
{
	ساکن uint8_t val;
	صحیح ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	اگر (ret)
		واپس ret;

	اگر (!(val & MANGO_BOARD_TYPE_نقاب))
		واپس SBI_ENODEV;

	واپس 0;
}

ساکن صحیح sg2042_mcu_reset_init(ثابت خالی *fdt, صحیح nodeoff,
				 ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &addr, تہی);
	اگر (ret)
		واپس ret;

	mcu_reg = addr;

	i2c_bus = fdt_والد_انحراف(fdt, nodeoff);
	اگر (i2c_bus < 0)
		واپس i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	اگر (ret)
		واپس ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_جوڑو_device(&sg2042_mcu_reset_device);

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت sg2042_mcu_reset_مطابقت[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

ثابت ساخت fdt_driver fdt_reset_sg2042_mcu = {
	.مطابقت_جدول = sg2042_mcu_reset_مطابقت,
	.init = sg2042_mcu_reset_init,
};
