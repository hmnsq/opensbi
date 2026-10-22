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

#ವ್ಯಾಖ್ಯಾನಿಸು MANGO_BOARD_TYPE_ಮುಖವಾಡ		0x80

#ವ್ಯಾಖ್ಯಾನಿಸು REG_BOARD_TYPE			0x00
#ವ್ಯಾಖ್ಯಾನಿಸು REG_CMD				0x03

#ವ್ಯಾಖ್ಯಾನಿಸು CMD_POWEROFF			0x02
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_RESET			0x03
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_REBOOT			0x07

ಸ್ಥಿರ ರಚನೆ i2c_adapter *mcu_adapter = ಖಾಲಿ;
ಸ್ಥಿರ uint32_t mcu_reg = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sg2042_mcu_reset_check(u32 type, u32 reason)
{
	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ಹಿಂತಿರುಗಿ 1;
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಹಿಂತಿರುಗಿ 255;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ sg2042_mcu_reset(u32 type, u32 reason)
{
	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		ನಿಲ್ಲಿಸು;
	}
}

ಸ್ಥಿರ ರಚನೆ sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sg2042_mcu_reset_check_board(ರಚನೆ i2c_adapter *adap, uint32_t reg)
{
	ಸ್ಥಿರ uint8_t val;
	ಪೂರ್ಣಾಂಕ ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ಆದರೆ (!(val & MANGO_BOARD_TYPE_ಮುಖವಾಡ))
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sg2042_mcu_reset_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				 ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &addr, ಖಾಲಿ);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	mcu_reg = addr;

	i2c_bus = fdt_ಪೋಷಕ_ಸ್ಥಾನಾಂತರ(fdt, nodeoff);
	ಆದರೆ (i2c_bus < 0)
		ಹಿಂತಿರುಗಿ i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_ಕೂಡಿಸು_device(&sg2042_mcu_reset_device);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ sg2042_mcu_reset_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_reset_sg2042_mcu = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = sg2042_mcu_reset_ಹೊಂದಿಕೆ,
	.init = sg2042_mcu_reset_init,
};
