/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_types.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/i2c/fdt_i2c.h>

#define MANGO_BOARD_TYPE_MASK		0x80

#define REG_BOARD_TYPE			0x00
#define REG_CMD				0x03

#define CMD_POWEROFF			0x02
#define CMD_RESET			0x03
#define CMD_REBOOT			0x07

ସ୍ଥିର ସଂରଚନା i2c_adapter *mcu_adapter = ଖାଲି;
ସ୍ଥିର uint32_t mcu_reg = 0;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sg2042_mcu_reset_check(u32 type, u32 reason)
{
	ବାଛ (type) {
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_SHUTDOWN:
		ଫେରନ୍ତୁ 1;
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ଫେରନ୍ତୁ 255;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ sg2042_mcu_reset(u32 type, u32 reason)
{
	ବାଛ (type) {
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		ଭାଙ୍ଗ;
	}
}

ସ୍ଥିର ସଂରଚନା sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sg2042_mcu_reset_check_board(ସଂରଚନା i2c_adapter *adap, uint32_t reg)
{
	ସ୍ଥିର uint8_t val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	ଯଦି (!(val & MANGO_BOARD_TYPE_MASK))
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sg2042_mcu_reset_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				 ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &addr, ଖାଲି);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	mcu_reg = addr;

	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	ଯଦି (i2c_bus < 0)
		ଫେରନ୍ତୁ i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_add_device(&sg2042_mcu_reset_device);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match sg2042_mcu_reset_match[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_reset_sg2042_mcu = {
	.match_table = sg2042_mcu_reset_match,
	.init = sg2042_mcu_reset_init,
};
