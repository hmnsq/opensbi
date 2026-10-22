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

struct i2c_adapter *i2c_adapter_find(số_nguyên id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		struct i2c_adapter *adap = to_i2c_adapter(pos);

		nếu (adap->id == id)
			trả_về adap;
	}

	trả_về NULL;
}

số_nguyên i2c_adapter_add(struct i2c_adapter *ia)
{
	nếu (!ia)
		trả_về SBI_EINVAL;

	nếu (i2c_adapter_find(ia->id))
		trả_về SBI_EALREADY;

	sbi_list_add(&(ia->node), &(i2c_adapters_list));

	trả_về 0;
}

void i2c_adapter_remove(struct i2c_adapter *ia)
{
	nếu (!ia)
		trả_về;

	sbi_list_del(&(ia->node));
}

số_nguyên i2c_adapter_write(struct i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, số_nguyên len)
{
	nếu (!ia)
		trả_về SBI_EINVAL;
	nếu (!ia->write)
		trả_về SBI_ENOSYS;

	trả_về ia->write(ia, addr, reg, buffer, len);
}


số_nguyên i2c_adapter_read(struct i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, số_nguyên len)
{
	nếu (!ia)
		trả_về SBI_EINVAL;
	nếu (!ia->read)
		trả_về SBI_ENOSYS;

	trả_về ia->read(ia, addr, reg, buffer, len);
}
