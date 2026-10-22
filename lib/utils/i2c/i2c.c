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

স্থির SBI_LIST_শীর্ষ(i2c_adapters_list);

কাঠামো i2c_adapter *i2c_adapter_find(পূর্ণসংখ্যা id)
{
	কাঠামো sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		কাঠামো i2c_adapter *adap = to_i2c_adapter(pos);

		যদি (adap->id == id)
			ফেরত adap;
	}

	ফেরত ফাঁকা;
}

পূর্ণসংখ্যা i2c_adapter_add(কাঠামো i2c_adapter *ia)
{
	যদি (!ia)
		ফেরত SBI_EINVAL;

	যদি (i2c_adapter_find(ia->id))
		ফেরত SBI_EALREADY;

	sbi_list_add(&(ia->গ্রন্থি), &(i2c_adapters_list));

	ফেরত 0;
}

শূন্য i2c_adapter_remove(কাঠামো i2c_adapter *ia)
{
	যদি (!ia)
		ফেরত;

	sbi_list_del(&(ia->গ্রন্থি));
}

পূর্ণসংখ্যা i2c_adapter_write(কাঠামো i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *বাফার, পূর্ণসংখ্যা len)
{
	যদি (!ia)
		ফেরত SBI_EINVAL;
	যদি (!ia->লিখ)
		ফেরত SBI_ENOSYS;

	ফেরত ia->লিখ(ia, addr, reg, বাফার, len);
}


পূর্ণসংখ্যা i2c_adapter_read(কাঠামো i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *বাফার, পূর্ণসংখ্যা len)
{
	যদি (!ia)
		ফেরত SBI_EINVAL;
	যদি (!ia->পঠ)
		ফেরত SBI_ENOSYS;

	ফেরত ia->পঠ(ia, addr, reg, বাফার, len);
}
