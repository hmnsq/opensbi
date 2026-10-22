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

		もし (adap->id == id)
			戻る adap;
	}

	戻る NULL;
}

整数 i2c_adapter_add(struct i2c_adapter *ia)
{
	もし (!ia)
		戻る SBI_EINVAL;

	もし (i2c_adapter_find(ia->id))
		戻る SBI_EALREADY;

	sbi_list_add(&(ia->node), &(i2c_adapters_list));

	戻る 0;
}

void i2c_adapter_remove(struct i2c_adapter *ia)
{
	もし (!ia)
		戻る;

	sbi_list_del(&(ia->node));
}

整数 i2c_adapter_write(struct i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, 整数 len)
{
	もし (!ia)
		戻る SBI_EINVAL;
	もし (!ia->write)
		戻る SBI_ENOSYS;

	戻る ia->write(ia, addr, reg, buffer, len);
}


整数 i2c_adapter_read(struct i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, 整数 len)
{
	もし (!ia)
		戻る SBI_EINVAL;
	もし (!ia->read)
		戻る SBI_ENOSYS;

	戻る ia->read(ia, addr, reg, buffer, len);
}
