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

sa_nracanaa i2c_adapter *i2c_adapter_find(ចំនួនគត់ id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		sa_nracanaa i2c_adapter *adap = to_i2c_adapter(pos);

		ប្រសិនបើ (adap->id == id)
			ត្រឡប់ adap;
	}

	ត្រឡប់ NULL;
}

ចំនួនគត់ i2c_adapter_add(sa_nracanaa i2c_adapter *ia)
{
	ប្រសិនបើ (!ia)
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (i2c_adapter_find(ia->id))
		ត្រឡប់ SBI_EALREADY;

	sbi_list_add(&(ia->node), &(i2c_adapters_list));

	ត្រឡប់ 0;
}

void i2c_adapter_remove(sa_nracanaa i2c_adapter *ia)
{
	ប្រសិនបើ (!ia)
		ត្រឡប់;

	sbi_list_del(&(ia->node));
}

ចំនួនគត់ i2c_adapter_write(sa_nracanaa i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, ចំនួនគត់ len)
{
	ប្រសិនបើ (!ia)
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (!ia->write)
		ត្រឡប់ SBI_ENOSYS;

	ត្រឡប់ ia->write(ia, addr, reg, buffer, len);
}


ចំនួនគត់ i2c_adapter_read(sa_nracanaa i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, ចំនួនគត់ len)
{
	ប្រសិនបើ (!ia)
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (!ia->read)
		ត្រឡប់ SBI_ENOSYS;

	ត្រឡប់ ia->read(ia, addr, reg, buffer, len);
}
