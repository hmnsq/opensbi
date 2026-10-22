/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_types.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/i2c/fdt_i2c.h>

#define MANGO_BOARD_TYPE_MASK		0x80

#define REG_BOARD_TYPE			0x00
#define REG_CMD				0x03

#define CMD_POWEROFF			0x02
#define CMD_RESET			0x03
#define CMD_REBOOT			0x07

ಸ್ಥಿರ ಸಂರಚನಾ i2c_adapter *mcu_adapter = ರಿಕ್ತ;
ಸ್ಥಿರ uint32_t mcu_reg = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sg2042_mcu_reset_check(u32 type, u32 reason)
{
	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ಪ್ರತ್ಯಾಗಮ 1;
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಪ್ರತ್ಯಾಗಮ 255;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ sg2042_mcu_reset(u32 type, u32 reason)
{
	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		ಭಙ್ಗ;
	}
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sg2042_mcu_reset_check_board(ಸಂರಚನಾ i2c_adapter *adap, uint32_t reg)
{
	ಸ್ಥಿರ uint8_t val;
	ಪೂರ್ಣಾಙ್ಕ ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ಯದಿ (!(val & MANGO_BOARD_TYPE_MASK))
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sg2042_mcu_reset_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				 ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &addr, ರಿಕ್ತ);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	mcu_reg = addr;

	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	ಯದಿ (i2c_bus < 0)
		ಪ್ರತ್ಯಾಗಮ i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_add_device(&sg2042_mcu_reset_device);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match sg2042_mcu_reset_match[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_reset_sg2042_mcu = {
	.match_table = sg2042_mcu_reset_match,
	.init = sg2042_mcu_reset_init,
};
