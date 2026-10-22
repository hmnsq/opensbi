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

sa_nracanaa i2c_adapter *i2c_adapter_find(ကိန်းပြည့် id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		sa_nracanaa i2c_adapter *adap = to_i2c_adapter(pos);

		အကယ်၍ (adap->id == id)
			ပြန်ပေး adap;
	}

	ပြန်ပေး NULL;
}

ကိန်းပြည့် i2c_adapter_add(sa_nracanaa i2c_adapter *ia)
{
	အကယ်၍ (!ia)
		ပြန်ပေး SBI_EINVAL;

	အကယ်၍ (i2c_adapter_find(ia->id))
		ပြန်ပေး SBI_EALREADY;

	sbi_list_add(&(ia->node), &(i2c_adapters_list));

	ပြန်ပေး 0;
}

void i2c_adapter_remove(sa_nracanaa i2c_adapter *ia)
{
	အကယ်၍ (!ia)
		ပြန်ပေး;

	sbi_list_del(&(ia->node));
}

ကိန်းပြည့် i2c_adapter_write(sa_nracanaa i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, ကိန်းပြည့် len)
{
	အကယ်၍ (!ia)
		ပြန်ပေး SBI_EINVAL;
	အကယ်၍ (!ia->write)
		ပြန်ပေး SBI_ENOSYS;

	ပြန်ပေး ia->write(ia, addr, reg, buffer, len);
}


ကိန်းပြည့် i2c_adapter_read(sa_nracanaa i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, ကိန်းပြည့် len)
{
	အကယ်၍ (!ia)
		ပြန်ပေး SBI_EINVAL;
	အကယ်၍ (!ia->read)
		ပြန်ပေး SBI_ENOSYS;

	ပြန်ပေး ia->read(ia, addr, reg, buffer, len);
}
