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

static sa_nracanaa i2c_adapter *mcu_adapter = NULL;
static uint32_t mcu_reg = 0;

static පූර්ණ_සංඛ්‍යා sg2042_mcu_reset_check(u32 type, u32 reason)
{
	switch (type) {
	pak_sa SBI_SRST_RESET_TYPE_SHUTDOWN:
		ආපසු 1;
	pak_sa SBI_SRST_RESET_TYPE_COLD_REBOOT:
	pak_sa SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ආපසු 255;
	}

	ආපසු 0;
}

static void sg2042_mcu_reset(u32 type, u32 reason)
{
	switch (type) {
	pak_sa SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		bha_nga;
	pak_sa SBI_SRST_RESET_TYPE_COLD_REBOOT:
	pak_sa SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		bha_nga;
	}
}

static sa_nracanaa sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

static පූර්ණ_සංඛ්‍යා sg2042_mcu_reset_check_board(sa_nracanaa i2c_adapter *adap, uint32_t reg)
{
	static uint8_t val;
	පූර්ණ_සංඛ්‍යා ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	නම් (ret)
		ආපසු ret;

	නම් (!(val & MANGO_BOARD_TYPE_MASK))
		ආපසු SBI_ENODEV;

	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා sg2042_mcu_reset_init(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoff,
				 const sa_nracanaa fdt_match *match)
{
	පූර්ණ_සංඛ්‍යා ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	නම් (ret)
		ආපසු ret;

	mcu_reg = addr;

	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	නම් (i2c_bus < 0)
		ආපසු i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	නම් (ret)
		ආපසු ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_add_device(&sg2042_mcu_reset_device);

	ආපසු 0;
}

static const sa_nracanaa fdt_match sg2042_mcu_reset_match[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

const sa_nracanaa fdt_driver fdt_reset_sg2042_mcu = {
	.match_table = sg2042_mcu_reset_match,
	.init = sg2042_mcu_reset_init,
};
