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

sa_nracanaa i2c_adapter *i2c_adapter_find(จำนวนเต็ม id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		sa_nracanaa i2c_adapter *adap = to_i2c_adapter(pos);

		ถ้า (adap->id == id)
			คืน adap;
	}

	คืน NULL;
}

จำนวนเต็ม i2c_adapter_add(sa_nracanaa i2c_adapter *ia)
{
	ถ้า (!ia)
		คืน SBI_EINVAL;

	ถ้า (i2c_adapter_find(ia->id))
		คืน SBI_EALREADY;

	sbi_list_add(&(ia->node), &(i2c_adapters_list));

	คืน 0;
}

void i2c_adapter_remove(sa_nracanaa i2c_adapter *ia)
{
	ถ้า (!ia)
		คืน;

	sbi_list_del(&(ia->node));
}

จำนวนเต็ม i2c_adapter_write(sa_nracanaa i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, จำนวนเต็ม len)
{
	ถ้า (!ia)
		คืน SBI_EINVAL;
	ถ้า (!ia->write)
		คืน SBI_ENOSYS;

	คืน ia->write(ia, addr, reg, buffer, len);
}


จำนวนเต็ม i2c_adapter_read(sa_nracanaa i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, จำนวนเต็ม len)
{
	ถ้า (!ia)
		คืน SBI_EINVAL;
	ถ้า (!ia->read)
		คืน SBI_ENOSYS;

	คืน ia->read(ia, addr, reg, buffer, len);
}
