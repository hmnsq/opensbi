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

struct i2c_adapter *i2c_adapter_find(целое id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		struct i2c_adapter *adap = to_i2c_adapter(pos);

		если (adap->id == id)
			вернуть adap;
	}

	вернуть NULL;
}

целое i2c_adapter_add(struct i2c_adapter *ia)
{
	если (!ia)
		вернуть SBI_EINVAL;

	если (i2c_adapter_find(ia->id))
		вернуть SBI_EALREADY;

	sbi_list_add(&(ia->node), &(i2c_adapters_list));

	вернуть 0;
}

void i2c_adapter_remove(struct i2c_adapter *ia)
{
	если (!ia)
		вернуть;

	sbi_list_del(&(ia->node));
}

целое i2c_adapter_write(struct i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, целое len)
{
	если (!ia)
		вернуть SBI_EINVAL;
	если (!ia->write)
		вернуть SBI_ENOSYS;

	вернуть ia->write(ia, addr, reg, buffer, len);
}


целое i2c_adapter_read(struct i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, целое len)
{
	если (!ia)
		вернуть SBI_EINVAL;
	если (!ia->read)
		вернуть SBI_ENOSYS;

	вернуть ia->read(ia, addr, reg, buffer, len);
}
