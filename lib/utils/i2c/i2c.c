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

sa_nracanaa i2c_adapter *i2c_adapter_find(puur_naa_nka id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		sa_nracanaa i2c_adapter *adap = to_i2c_adapter(pos);

		ᨀᨘ (adap->id == id)
			ᨀᨘᨒ adap;
	}

	ᨀᨘᨒ NULL;
}

puur_naa_nka i2c_adapter_add(sa_nracanaa i2c_adapter *ia)
{
	ᨀᨘ (!ia)
		ᨀᨘᨒ SBI_EINVAL;

	ᨀᨘ (i2c_adapter_find(ia->id))
		ᨀᨘᨒ SBI_EALREADY;

	sbi_list_add(&(ia->node), &(i2c_adapters_list));

	ᨀᨘᨒ 0;
}

void i2c_adapter_remove(sa_nracanaa i2c_adapter *ia)
{
	ᨀᨘ (!ia)
		ᨀᨘᨒ;

	sbi_list_del(&(ia->node));
}

puur_naa_nka i2c_adapter_write(sa_nracanaa i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, puur_naa_nka len)
{
	ᨀᨘ (!ia)
		ᨀᨘᨒ SBI_EINVAL;
	ᨀᨘ (!ia->write)
		ᨀᨘᨒ SBI_ENOSYS;

	ᨀᨘᨒ ia->write(ia, addr, reg, buffer, len);
}


puur_naa_nka i2c_adapter_read(sa_nracanaa i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, puur_naa_nka len)
{
	ᨀᨘ (!ia)
		ᨀᨘᨒ SBI_EINVAL;
	ᨀᨘ (!ia->read)
		ᨀᨘᨒ SBI_ENOSYS;

	ᨀᨘᨒ ia->read(ia, addr, reg, buffer, len);
}
