/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_types.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/i2c/fdt_i2c.h>

#define MANGO_BOARD_TYPE_MASK		0x80

#define REG_BOARD_TYPE			0x00
#define REG_CMD				0x03

#define CMD_POWEROFF			0x02
#define CMD_RESET			0x03
#define CMD_REBOOT			0x07

স্থির কাঠামো i2c_adapter *mcu_adapter = ফাঁকা;
স্থির uint32_t mcu_reg = 0;

স্থির পূর্ণসংখ্যা sg2042_mcu_reset_check(u32 type, u32 reason)
{
	নির্বাচন (type) {
	ক্ষেত্র SBI_SRST_RESET_TYPE_SHUTDOWN:
		ফেরত 1;
	ক্ষেত্র SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ক্ষেত্র SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ফেরত 255;
	}

	ফেরত 0;
}

স্থির শূন্য sg2042_mcu_reset(u32 type, u32 reason)
{
	নির্বাচন (type) {
	ক্ষেত্র SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		বিরতি;
	ক্ষেত্র SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ক্ষেত্র SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		বিরতি;
	}
}

স্থির কাঠামো sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

স্থির পূর্ণসংখ্যা sg2042_mcu_reset_check_board(কাঠামো i2c_adapter *adap, uint32_t reg)
{
	স্থির uint8_t val;
	পূর্ণসংখ্যা ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	যদি (ret)
		ফেরত ret;

	যদি (!(val & MANGO_BOARD_TYPE_MASK))
		ফেরত SBI_ENODEV;

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা sg2042_mcu_reset_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				 ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &addr, ফাঁকা);
	যদি (ret)
		ফেরত ret;

	mcu_reg = addr;

	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	যদি (i2c_bus < 0)
		ফেরত i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	যদি (ret)
		ফেরত ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_add_device(&sg2042_mcu_reset_device);

	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_match sg2042_mcu_reset_match[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_reset_sg2042_mcu = {
	.match_table = sg2042_mcu_reset_match,
	.init = sg2042_mcu_reset_init,
};
