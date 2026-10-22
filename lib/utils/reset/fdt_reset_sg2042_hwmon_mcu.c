/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Lin Chunzhi <chunzhi.lin@sophgo.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_types.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/i2c/fdt_i2c.h>

#घोषणा MANGO_BOARD_TYPE_MASK		0x80

#घोषणा REG_BOARD_TYPE			0x00
#घोषणा REG_CMD				0x03

#घोषणा CMD_POWEROFF			0x02
#घोषणा CMD_RESET			0x03
#घोषणा CMD_REBOOT			0x07

अटल काष्ठा i2c_adapter *mcu_adapter = शून्य;
अटल uint32_t mcu_reg = 0;

अटल पूर्णांक sg2042_mcu_reset_check(u32 type, u32 reason)
{
	चयन (type) {
	हाल SBI_SRST_RESET_TYPE_SHUTDOWN:
		वापस 1;
	हाल SBI_SRST_RESET_TYPE_COLD_REBOOT:
	हाल SBI_SRST_RESET_TYPE_WARM_REBOOT:
		वापस 255;
	}

	वापस 0;
}

अटल व्योम sg2042_mcu_reset(u32 type, u32 reason)
{
	चयन (type) {
	हाल SBI_SRST_RESET_TYPE_SHUTDOWN:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_POWEROFF);
		अवरोध;
	हाल SBI_SRST_RESET_TYPE_COLD_REBOOT:
	हाल SBI_SRST_RESET_TYPE_WARM_REBOOT:
		i2c_adapter_reg_write(mcu_adapter, mcu_reg,
				      REG_CMD, CMD_REBOOT);
		अवरोध;
	}
}

अटल काष्ठा sbi_system_reset_device sg2042_mcu_reset_device = {
	.name = "sg2042-mcu-reset",
	.system_reset_check = sg2042_mcu_reset_check,
	.system_reset = sg2042_mcu_reset
};

अटल पूर्णांक sg2042_mcu_reset_check_board(काष्ठा i2c_adapter *adap, uint32_t reg)
{
	अटल uint8_t val;
	पूर्णांक ret;

	/* check board type */
	ret = i2c_adapter_reg_read(adap, reg, REG_BOARD_TYPE, &val);
	अगर (ret)
		वापस ret;

	अगर (!(val & MANGO_BOARD_TYPE_MASK))
		वापस SBI_ENODEV;

	वापस 0;
}

अटल पूर्णांक sg2042_mcu_reset_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				 स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक ret, i2c_bus;
	uint64_t addr;

	ret = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &addr, शून्य);
	अगर (ret)
		वापस ret;

	mcu_reg = addr;

	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	अगर (i2c_bus < 0)
		वापस i2c_bus;

	ret = fdt_i2c_adapter_get(fdt, i2c_bus, &mcu_adapter);
	अगर (ret)
		वापस ret;

	ret = sg2042_mcu_reset_check_board(mcu_adapter, mcu_reg);

	sbi_system_reset_add_device(&sg2042_mcu_reset_device);

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match sg2042_mcu_reset_match[] = {
	{ .compatible = "sophgo,sg2042-hwmon-mcu" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_reset_sg2042_mcu = {
	.match_table = sg2042_mcu_reset_match,
	.init = sg2042_mcu_reset_init,
};
