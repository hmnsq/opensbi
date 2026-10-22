/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_types.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/i2c/fdt_i2c.h>

#define MANGO_BOARD_TYPE_MASK		0x80

#define REG_BOARD_TYPE			0x00
#define REG_CMD				0x03

#define CMD_POWEROFF			0x02
#define CMD_RESET			0x03
#define CMD_REBOOT			0x07

സ്ഥിതം ഘടന i2c_adapter *mcu_adapter = ശൂന്യ;
സ്ഥിതം uint32_t mcu_reg = 0;

സ്ഥിതം പൂർണ്ണസംഖ്യ sg2042_mcu_reset_check(u32 type, u32 reason)
{
	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ SBI_SRST_RESET_TYPE_SHUTDOWN:
		തിരികെ 1;
	അവസ്ഥ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	അവസ്ഥ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		തിരികെ 255;
	}

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം sg2042_mcu_reset(u32 type, u32 reason)
{
	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		നിർത്തുക;
	അവസ്ഥ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	അവസ്ഥ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		നിർത്തുക;
	}
}

സ്ഥിതം ഘടന sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

സ്ഥിതം പൂർണ്ണസംഖ്യ sg2042_mcu_reset_check_board(ഘടന i2c_adapter *adap, uint32_t reg)
{
	സ്ഥിതം uint8_t val;
	പൂർണ്ണസംഖ്യ ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	എങ്കിൽ (ret)
		തിരികെ ret;

	എങ്കിൽ (!(val & MANGO_BOARD_TYPE_MASK))
		തിരികെ SBI_ENODEV;

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sg2042_mcu_reset_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				 സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &addr, ശൂന്യ);
	എങ്കിൽ (ret)
		തിരികെ ret;

	mcu_reg = addr;

	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	എങ്കിൽ (i2c_bus < 0)
		തിരികെ i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	എങ്കിൽ (ret)
		തിരികെ ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_add_device(&sg2042_mcu_reset_device);

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match sg2042_mcu_reset_match[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_reset_sg2042_mcu = {
	.match_table = sg2042_mcu_reset_match,
	.init = sg2042_mcu_reset_init,
};
