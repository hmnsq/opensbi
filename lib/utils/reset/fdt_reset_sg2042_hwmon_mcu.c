/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_types.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/i2c/fdt_i2c.h>

#define MANGO_BOARD_TYPE_MASK		0x80

#define REG_BOARD_TYPE			0x00
#define REG_CMD				0x03

#define CMD_POWEROFF			0x02
#define CMD_RESET			0x03
#define CMD_REBOOT			0x07

સ્થિર સંરચના i2c_adapter *mcu_adapter = ખાલી;
સ્થિર uint32_t mcu_reg = 0;

સ્થિર પૂર્ણાંક sg2042_mcu_reset_check(u32 type, u32 reason)
{
	પસંદગી (type) {
	સ્થિતિ SBI_SRST_RESET_TYPE_SHUTDOWN:
		પાછા 1;
	સ્થિતિ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	સ્થિતિ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		પાછા 255;
	}

	પાછા 0;
}

સ્થિર શૂન્ય sg2042_mcu_reset(u32 type, u32 reason)
{
	પસંદગી (type) {
	સ્થિતિ SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		તોડો;
	સ્થિતિ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	સ્થિતિ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		તોડો;
	}
}

સ્થિર સંરચના sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

સ્થિર પૂર્ણાંક sg2042_mcu_reset_check_board(સંરચના i2c_adapter *adap, uint32_t reg)
{
	સ્થિર uint8_t val;
	પૂર્ણાંક ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	જો (ret)
		પાછા ret;

	જો (!(val & MANGO_BOARD_TYPE_MASK))
		પાછા SBI_ENODEV;

	પાછા 0;
}

સ્થિર પૂર્ણાંક sg2042_mcu_reset_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				 અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &addr, ખાલી);
	જો (ret)
		પાછા ret;

	mcu_reg = addr;

	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	જો (i2c_bus < 0)
		પાછા i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	જો (ret)
		પાછા ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_add_device(&sg2042_mcu_reset_device);

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match sg2042_mcu_reset_match[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

અચલ સંરચના fdt_driver fdt_reset_sg2042_mcu = {
	.match_table = sg2042_mcu_reset_match,
	.init = sg2042_mcu_reset_init,
};
