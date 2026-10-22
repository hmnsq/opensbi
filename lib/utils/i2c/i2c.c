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

struct i2c_adapter *i2c_adapter_find(정수 id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		struct i2c_adapter *adap = to_i2c_adapter(pos);

		만약 (adap->id == id)
			반환 adap;
	}

	반환 NULL;
}

정수 i2c_adapter_add(struct i2c_adapter *ia)
{
	만약 (!ia)
		반환 SBI_EINVAL;

	만약 (i2c_adapter_find(ia->id))
		반환 SBI_EALREADY;

	sbi_list_add(&(ia->node), &(i2c_adapters_list));

	반환 0;
}

void i2c_adapter_remove(struct i2c_adapter *ia)
{
	만약 (!ia)
		반환;

	sbi_list_del(&(ia->node));
}

정수 i2c_adapter_write(struct i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, 정수 len)
{
	만약 (!ia)
		반환 SBI_EINVAL;
	만약 (!ia->write)
		반환 SBI_ENOSYS;

	반환 ia->write(ia, addr, reg, buffer, len);
}


정수 i2c_adapter_read(struct i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, 정수 len)
{
	만약 (!ia)
		반환 SBI_EINVAL;
	만약 (!ia->read)
		반환 SBI_ENOSYS;

	반환 ia->read(ia, addr, reg, buffer, len);
}
