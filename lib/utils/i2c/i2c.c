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

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/i2c/i2c.h>

স্থির SBI_LIST_HEAD(i2c_adapters_list);

সংরচনা i2c_adapter *i2c_adapter_find(পূর্ণাঙ্ক id)
{
	সংরচনা sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		সংরচনা i2c_adapter *adap = to_i2c_adapter(pos);

		যদি (adap->id == id)
			প্রত্যাগম adap;
	}

	প্রত্যাগম রিক্ত;
}

পূর্ণাঙ্ক i2c_adapter_add(সংরচনা i2c_adapter *ia)
{
	যদি (!ia)
		প্রত্যাগম SBI_EINVAL;

	যদি (i2c_adapter_find(ia->id))
		প্রত্যাগম SBI_EALREADY;

	sbi_list_add(&(ia->গ্রন্থি), &(i2c_adapters_list));

	প্রত্যাগম 0;
}

শূন্য i2c_adapter_remove(সংরচনা i2c_adapter *ia)
{
	যদি (!ia)
		প্রত্যাগম;

	sbi_list_del(&(ia->গ্রন্থি));
}

পূর্ণাঙ্ক i2c_adapter_write(সংরচনা i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, পূর্ণাঙ্ক len)
{
	যদি (!ia)
		প্রত্যাগম SBI_EINVAL;
	যদি (!ia->লিখ)
		প্রত্যাগম SBI_ENOSYS;

	প্রত্যাগম ia->লিখ(ia, addr, reg, buffer, len);
}


পূর্ণাঙ্ক i2c_adapter_read(সংরচনা i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, পূর্ণাঙ্ক len)
{
	যদি (!ia)
		প্রত্যাগম SBI_EINVAL;
	যদি (!ia->পঠ)
		প্রত্যাগম SBI_ENOSYS;

	প্রত্যাগম ia->পঠ(ia, addr, reg, buffer, len);
}
