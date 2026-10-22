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

sa_nracanaa i2c_adapter *i2c_adapter_find(ཧྲིལ id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		sa_nracanaa i2c_adapter *adap = to_i2c_adapter(pos);

		གལ་སྲིད (adap->id == id)
			ལོག adap;
	}

	ལོག NULL;
}

ཧྲིལ i2c_adapter_add(sa_nracanaa i2c_adapter *ia)
{
	གལ་སྲིད (!ia)
		ལོག SBI_EINVAL;

	གལ་སྲིད (i2c_adapter_find(ia->id))
		ལོག SBI_EALREADY;

	sbi_list_add(&(ia->node), &(i2c_adapters_list));

	ལོག 0;
}

void i2c_adapter_remove(sa_nracanaa i2c_adapter *ia)
{
	གལ་སྲིད (!ia)
		ལོག;

	sbi_list_del(&(ia->node));
}

ཧྲིལ i2c_adapter_write(sa_nracanaa i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, ཧྲིལ len)
{
	གལ་སྲིད (!ia)
		ལོག SBI_EINVAL;
	གལ་སྲིད (!ia->write)
		ལོག SBI_ENOSYS;

	ལོག ia->write(ia, addr, reg, buffer, len);
}


ཧྲིལ i2c_adapter_read(sa_nracanaa i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, ཧྲིལ len)
{
	གལ་སྲིད (!ia)
		ལོག SBI_EINVAL;
	གལ་སྲིད (!ia->read)
		ལོག SBI_ENOSYS;

	ལོག ia->read(ia, addr, reg, buffer, len);
}
