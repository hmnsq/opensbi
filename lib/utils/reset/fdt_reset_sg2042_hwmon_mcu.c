/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_types.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/i2c/fdt_i2c.h>

#define MANGO_BOARD_TYPE_MASK		0x80

#define REG_BOARD_TYPE			0x00
#define REG_CMD				0x03

#define CMD_POWEROFF			0x02
#define CMD_RESET			0x03
#define CMD_REBOOT			0x07

स्थिर संरचना i2c_adapter *mcu_adapter = रिक्त;
स्थिर uint32_t mcu_reg = 0;

स्थिर पूर्णाङ्क sg2042_mcu_reset_check(u32 type, u32 reason)
{
	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
		प्रत्यागम 1;
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
		प्रत्यागम 255;
	}

	प्रत्यागम 0;
}

स्थिर शून्य sg2042_mcu_reset(u32 type, u32 reason)
{
	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		भङ्ग;
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		भङ्ग;
	}
}

स्थिर संरचना sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

स्थिर पूर्णाङ्क sg2042_mcu_reset_check_board(संरचना i2c_adapter *adap, uint32_t reg)
{
	स्थिर uint8_t val;
	पूर्णाङ्क ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	जइ (ret)
		प्रत्यागम ret;

	जइ (!(val & MANGO_BOARD_TYPE_MASK))
		प्रत्यागम SBI_ENODEV;

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क sg2042_mcu_reset_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				 ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoff, 0, &addr, रिक्त);
	जइ (ret)
		प्रत्यागम ret;

	mcu_reg = addr;

	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	जइ (i2c_bus < 0)
		प्रत्यागम i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	जइ (ret)
		प्रत्यागम ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_add_device(&sg2042_mcu_reset_device);

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_match sg2042_mcu_reset_match[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_reset_sg2042_mcu = {
	.match_table = sg2042_mcu_reset_match,
	.init = sg2042_mcu_reset_init,
};
