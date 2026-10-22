/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_types.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/i2c/fdt_i2c.h>

#define MANGO_BOARD_TYPE_MASK		0x80

#define REG_BOARD_TYPE			0x00
#define REG_CMD				0x03

#define CMD_POWEROFF			0x02
#define CMD_RESET			0x03
#define CMD_REBOOT			0x07

ساكن بنية i2c_adapter *mcu_adapter = عدم;
ساكن uint32_t mcu_reg = 0;

ساكن صحيح sg2042_mcu_reset_check(u32 type, u32 reason)
{
	بدل (type) {
	حالة SBI_SRST_RESET_TYPE_SHUTDOWN:
		أرجع 1;
	حالة SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالة SBI_SRST_RESET_TYPE_WARM_REBOOT:
		أرجع 255;
	}

	أرجع 0;
}

ساكن فراغ sg2042_mcu_reset(u32 type, u32 reason)
{
	بدل (type) {
	حالة SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		اكسر;
	حالة SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالة SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		اكسر;
	}
}

ساكن بنية sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

ساكن صحيح sg2042_mcu_reset_check_board(بنية i2c_adapter *adap, uint32_t reg)
{
	ساكن uint8_t val;
	صحيح ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	𐡄𐡍 (ret)
		أرجع ret;

	𐡄𐡍 (!(val & MANGO_BOARD_TYPE_MASK))
		أرجع SBI_ENODEV;

	أرجع 0;
}

ساكن صحيح sg2042_mcu_reset_init(ثابت فراغ *fdt, صحيح nodeoff,
				 ثابت بنية fdt_match *match)
{
	صحيح ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, عدم);
	𐡄𐡍 (ret)
		أرجع ret;

	mcu_reg = addr;

	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	𐡄𐡍 (i2c_bus < 0)
		أرجع i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	𐡄𐡍 (ret)
		أرجع ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_add_device(&sg2042_mcu_reset_device);

	أرجع 0;
}

ساكن ثابت بنية fdt_match sg2042_mcu_reset_match[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

ثابت بنية fdt_driver fdt_reset_sg2042_mcu = {
	.match_table = sg2042_mcu_reset_match,
	.init = sg2042_mcu_reset_init,
};
