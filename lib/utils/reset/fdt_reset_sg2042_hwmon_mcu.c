/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/sbi_hart.h>
#অন্তর্ভাৱ <sbi/sbi_types.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_system.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/i2c/fdt_i2c.h>

#define MANGO_BOARD_TYPE_MASK		0x80

#define REG_BOARD_TYPE			0x00
#define REG_CMD				0x03

#define CMD_POWEROFF			0x02
#define CMD_RESET			0x03
#define CMD_REBOOT			0x07

স্থির সংরচনা i2c_adapter *mcu_adapter = রিক্ত;
স্থির uint32_t mcu_reg = 0;

স্থির পূর্ণাঙ্ক sg2042_mcu_reset_check(u32 type, u32 reason)
{
	ৱিকল্প (type) {
	পক্ষ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ঘূৰাই 1;
	পক্ষ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	পক্ষ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ঘূৰাই 255;
	}

	ঘূৰাই 0;
}

স্থির শূন্য sg2042_mcu_reset(u32 type, u32 reason)
{
	ৱিকল্প (type) {
	পক্ষ SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		ভঙ্গ;
	পক্ষ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	পক্ষ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		ভঙ্গ;
	}
}

স্থির সংরচনা sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

স্থির পূর্ণাঙ্ক sg2042_mcu_reset_check_board(সংরচনা i2c_adapter *adap, uint32_t reg)
{
	স্থির uint8_t val;
	পূর্ণাঙ্ক ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	যদি (ret)
		ঘূৰাই ret;

	যদি (!(val & MANGO_BOARD_TYPE_MASK))
		ঘূৰাই SBI_ENODEV;

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক sg2042_mcu_reset_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				 ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 0, &addr, রিক্ত);
	যদি (ret)
		ঘূৰাই ret;

	mcu_reg = addr;

	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	যদি (i2c_bus < 0)
		ঘূৰাই i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	যদি (ret)
		ঘূৰাই ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_add_device(&sg2042_mcu_reset_device);

	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_match sg2042_mcu_reset_match[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_reset_sg2042_mcu = {
	.match_table = sg2042_mcu_reset_match,
	.init = sg2042_mcu_reset_init,
};
