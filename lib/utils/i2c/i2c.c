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

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/i2c/i2c.h>

நிலையான SBI_LIST_HEAD(i2c_adapters_list);

கட்டமைப்பு i2c_adapter *i2c_adapter_find(முழு id)
{
	கட்டமைப்பு sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		கட்டமைப்பு i2c_adapter *adap = to_i2c_adapter(pos);

		எனில் (adap->id == id)
			திரும்பு adap;
	}

	திரும்பு வெற்று;
}

முழு i2c_adapter_add(கட்டமைப்பு i2c_adapter *ia)
{
	எனில் (!ia)
		திரும்பு SBI_EINVAL;

	எனில் (i2c_adapter_find(ia->id))
		திரும்பு SBI_EALREADY;

	sbi_list_add(&(ia->முனை), &(i2c_adapters_list));

	திரும்பு 0;
}

வெறுமை i2c_adapter_remove(கட்டமைப்பு i2c_adapter *ia)
{
	எனில் (!ia)
		திரும்பு;

	sbi_list_del(&(ia->முனை));
}

முழு i2c_adapter_write(கட்டமைப்பு i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, முழு len)
{
	எனில் (!ia)
		திரும்பு SBI_EINVAL;
	எனில் (!ia->லிக)
		திரும்பு SBI_ENOSYS;

	திரும்பு ia->லிக(ia, addr, reg, buffer, len);
}


முழு i2c_adapter_read(கட்டமைப்பு i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, முழு len)
{
	எனில் (!ia)
		திரும்பு SBI_EINVAL;
	எனில் (!ia->பட)
		திரும்பு SBI_ENOSYS;

	திரும்பு ia->பட(ia, addr, reg, buffer, len);
}
