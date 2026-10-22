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

#define MANGO_BOARD_TYPE_MASK		0x80

#define REG_BOARD_TYPE			0x00
#define REG_CMD				0x03

#define CMD_POWEROFF			0x02
#define CMD_RESET			0x03
#define CMD_REBOOT			0x07

ایستا ساختار i2c_adapter *mcu_adapter = پوچ;
ایستا uint32_t mcu_reg = 0;

ایستا صحیح sg2042_mcu_reset_check(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		بازگشت 1;
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		بازگشت 255;
	}

	بازگشت 0;
}

ایستا تهی sg2042_mcu_reset(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		بشکن;
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		بشکن;
	}
}

ایستا ساختار sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

ایستا صحیح sg2042_mcu_reset_check_board(ساختار i2c_adapter *adap, uint32_t reg)
{
	ایستا uint8_t val;
	صحیح ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	اگر (ret)
		بازگشت ret;

	اگر (!(val & MANGO_BOARD_TYPE_MASK))
		بازگشت SBI_ENODEV;

	بازگشت 0;
}

ایستا صحیح sg2042_mcu_reset_init(ثابت تهی *fdt, صحیح nodeoff,
				 ثابت ساختار fdt_match *match)
{
	صحیح ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_گره_addr_size(fdt, nodeoff, 0, &addr, پوچ);
	اگر (ret)
		بازگشت ret;

	mcu_reg = addr;

	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	اگر (i2c_bus < 0)
		بازگشت i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	اگر (ret)
		بازگشت ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_add_device(&sg2042_mcu_reset_device);

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match sg2042_mcu_reset_match[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

ثابت ساختار fdt_driver fdt_reset_sg2042_mcu = {
	.match_table = sg2042_mcu_reset_match,
	.init = sg2042_mcu_reset_init,
};
