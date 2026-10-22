/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#include <libfdt.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_types.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_system.h>
#include <sbi/sbi_console.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/i2c/fdt_i2c.h>

#define MANGO_BOARD_TYPE_MASK		0x80

#define REG_BOARD_TYPE			0x00
#define REG_CMD				0x03

#define CMD_POWEROFF			0x02
#define CMD_RESET			0x03
#define CMD_REBOOT			0x07

static struct i2c_adapter *mcu_adapter = NULL;
static uint32_t mcu_reg = 0;

static ціле sg2042_mcu_reset_check(u32 type, u32 reason)
{
	switch (type) {
	case SBI_SRST_RESET_TYPE_SHUTDOWN:
		повернути 1;
	case SBI_SRST_RESET_TYPE_COLD_REBOOT:
	case SBI_SRST_RESET_TYPE_WARM_REBOOT:
		повернути 255;
	}

	повернути 0;
}

static void sg2042_mcu_reset(u32 type, u32 reason)
{
	switch (type) {
	case SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		break;
	case SBI_SRST_RESET_TYPE_COLD_REBOOT:
	case SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		break;
	}
}

static struct sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

static ціле sg2042_mcu_reset_check_board(struct i2c_adapter *adap, uint32_t reg)
{
	static uint8_t val;
	ціле ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	якщо (ret)
		повернути ret;

	якщо (!(val & MANGO_BOARD_TYPE_MASK))
		повернути SBI_ENODEV;

	повернути 0;
}

static ціле sg2042_mcu_reset_init(const void *fdt, ціле nodeoff,
				 const struct fdt_match *match)
{
	ціле ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	якщо (ret)
		повернути ret;

	mcu_reg = addr;

	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	якщо (i2c_bus < 0)
		повернути i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	якщо (ret)
		повернути ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_add_device(&sg2042_mcu_reset_device);

	повернути 0;
}

static const struct fdt_match sg2042_mcu_reset_match[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

const struct fdt_driver fdt_reset_sg2042_mcu = {
	.match_table = sg2042_mcu_reset_match,
	.init = sg2042_mcu_reset_init,
};
