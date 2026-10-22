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

struct i2c_adapter *i2c_adapter_find(цяло id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		struct i2c_adapter *adap = to_i2c_adapter(pos);

		ако (adap->id == id)
			върни adap;
	}

	върни NULL;
}

цяло i2c_adapter_add(struct i2c_adapter *ia)
{
	ако (!ia)
		върни SBI_EINVAL;

	ако (i2c_adapter_find(ia->id))
		върни SBI_EALREADY;

	sbi_list_add(&(ia->node), &(i2c_adapters_list));

	върни 0;
}

void i2c_adapter_remove(struct i2c_adapter *ia)
{
	ако (!ia)
		върни;

	sbi_list_del(&(ia->node));
}

цяло i2c_adapter_write(struct i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, цяло len)
{
	ако (!ia)
		върни SBI_EINVAL;
	ако (!ia->write)
		върни SBI_ENOSYS;

	върни ia->write(ia, addr, reg, buffer, len);
}


цяло i2c_adapter_read(struct i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, цяло len)
{
	ако (!ia)
		върни SBI_EINVAL;
	ако (!ia->read)
		върни SBI_ENOSYS;

	върни ia->read(ia, addr, reg, buffer, len);
}
