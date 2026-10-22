/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 *
 * derivate: lib/utils/gpio/gpio.c
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/sbi_error.h>
#include <sbi_utils/i2c/i2c.h>

static SBI_LIST_HEAD(i2c_adapters_list);

struct i2c_adapter *i2c_adapter_find(整数 id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		struct i2c_adapter *adap = to_i2c_adapter(pos);

		如果 (adap->id == id)
			返回 adap;
	}

	返回 NULL;
}

整数 i2c_adapter_add(struct i2c_adapter *ia)
{
	如果 (!ia)
		返回 SBI_EINVAL;

	如果 (i2c_adapter_find(ia->id))
		返回 SBI_EALREADY;

	sbi_list_add(&(ia->node), &(i2c_adapters_list));

	返回 0;
}

void i2c_adapter_remove(struct i2c_adapter *ia)
{
	如果 (!ia)
		返回;

	sbi_list_del(&(ia->node));
}

整数 i2c_adapter_write(struct i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, 整数 len)
{
	如果 (!ia)
		返回 SBI_EINVAL;
	如果 (!ia->write)
		返回 SBI_ENOSYS;

	返回 ia->write(ia, addr, reg, buffer, len);
}


整数 i2c_adapter_read(struct i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, 整数 len)
{
	如果 (!ia)
		返回 SBI_EINVAL;
	如果 (!ia->read)
		返回 SBI_ENOSYS;

	返回 ia->read(ia, addr, reg, buffer, len);
}
