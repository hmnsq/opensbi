/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_types.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/i2c/fdt_i2c.h>

#define MANGO_BOARD_TYPE_MASK		0x80

#define REG_BOARD_TYPE			0x00
#define REG_CMD				0x03

#define CMD_POWEROFF			0x02
#define CMD_RESET			0x03
#define CMD_REBOOT			0x07

நிலையான கட்டமைப்பு i2c_adapter *mcu_adapter = வெற்று;
நிலையான uint32_t mcu_reg = 0;

நிலையான முழு sg2042_mcu_reset_check(u32 type, u32 reason)
{
	தேர்வு (type) {
	நிலை SBI_SRST_RESET_TYPE_SHUTDOWN:
		திரும்பு 1;
	நிலை SBI_SRST_RESET_TYPE_COLD_REBOOT:
	நிலை SBI_SRST_RESET_TYPE_WARM_REBOOT:
		திரும்பு 255;
	}

	திரும்பு 0;
}

நிலையான வெறுமை sg2042_mcu_reset(u32 type, u32 reason)
{
	தேர்வு (type) {
	நிலை SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		நிறுத்து;
	நிலை SBI_SRST_RESET_TYPE_COLD_REBOOT:
	நிலை SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		நிறுத்து;
	}
}

நிலையான கட்டமைப்பு sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

நிலையான முழு sg2042_mcu_reset_check_board(கட்டமைப்பு i2c_adapter *adap, uint32_t reg)
{
	நிலையான uint8_t val;
	முழு ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	எனில் (ret)
		திரும்பு ret;

	எனில் (!(val & MANGO_BOARD_TYPE_MASK))
		திரும்பு SBI_ENODEV;

	திரும்பு 0;
}

நிலையான முழு sg2042_mcu_reset_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				 மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &addr, வெற்று);
	எனில் (ret)
		திரும்பு ret;

	mcu_reg = addr;

	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	எனில் (i2c_bus < 0)
		திரும்பு i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	எனில் (ret)
		திரும்பு ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_add_device(&sg2042_mcu_reset_device);

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match sg2042_mcu_reset_match[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_reset_sg2042_mcu = {
	.match_table = sg2042_mcu_reset_match,
	.init = sg2042_mcu_reset_init,
};
