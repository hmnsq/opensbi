/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/i2c/fdt_i2c.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MANGO_BOARD_TYPE_ਨਕਾਬ		0x80

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_BOARD_TYPE			0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_CMD				0x03

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_POWEROFF			0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_RESET			0x03
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_REBOOT			0x07

ਸਥਿਰ ਬਣਤਰ i2c_adapter *mcu_adapter = ਨਿਰਾ;
ਸਥਿਰ uint32_t mcu_reg = 0;

ਸਥਿਰ ਪੂਰਨ sg2042_mcu_reset_check(u32 type, u32 reason)
{
	ਚੋਣ (type) {
	ਹਾਲਤ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ਵਾਪਸ 1;
	ਹਾਲਤ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ਹਾਲਤ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ਵਾਪਸ 255;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ sg2042_mcu_reset(u32 type, u32 reason)
{
	ਚੋਣ (type) {
	ਹਾਲਤ SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ਹਾਲਤ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		ਰੋਕੋ;
	}
}

ਸਥਿਰ ਬਣਤਰ sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

ਸਥਿਰ ਪੂਰਨ sg2042_mcu_reset_check_board(ਬਣਤਰ i2c_adapter *adap, uint32_t reg)
{
	ਸਥਿਰ uint8_t val;
	ਪੂਰਨ ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ਜੇ (!(val & MANGO_BOARD_TYPE_ਨਕਾਬ))
		ਵਾਪਸ SBI_ENODEV;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ sg2042_mcu_reset_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				 ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &addr, ਨਿਰਾ);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	mcu_reg = addr;

	i2c_bus = fdt_ਮਾਪਾ_ਵਿੱਥ(fdt, nodeoff);
	ਜੇ (i2c_bus < 0)
		ਵਾਪਸ i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_ਜੋੜੋ_device(&sg2042_mcu_reset_device);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ sg2042_mcu_reset_ਮੇਲ[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_reset_sg2042_mcu = {
	.ਮੇਲ_ਸਾਰਣੀ = sg2042_mcu_reset_ਮੇਲ,
	.init = sg2042_mcu_reset_init,
};
