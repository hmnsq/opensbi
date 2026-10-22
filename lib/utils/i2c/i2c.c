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

#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/i2c/i2c.h>

ایستا SBI_LIST_HEAD(i2c_adapters_list);

ساختار i2c_adapter *i2c_adapter_find(صحیح id)
{
	ساختار sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		ساختار i2c_adapter *adap = to_i2c_adapter(pos);

		اگر (adap->id == id)
			بازگشت adap;
	}

	بازگشت پوچ;
}

صحیح i2c_adapter_add(ساختار i2c_adapter *ia)
{
	اگر (!ia)
		بازگشت SBI_EINVAL;

	اگر (i2c_adapter_find(ia->id))
		بازگشت SBI_EALREADY;

	sbi_list_add(&(ia->گره), &(i2c_adapters_list));

	بازگشت 0;
}

تهی i2c_adapter_remove(ساختار i2c_adapter *ia)
{
	اگر (!ia)
		بازگشت;

	sbi_list_del(&(ia->گره));
}

صحیح i2c_adapter_write(ساختار i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, صحیح len)
{
	اگر (!ia)
		بازگشت SBI_EINVAL;
	اگر (!ia->اكتب)
		بازگشت SBI_ENOSYS;

	بازگشت ia->اكتب(ia, addr, reg, buffer, len);
}


صحیح i2c_adapter_read(ساختار i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, صحیح len)
{
	اگر (!ia)
		بازگشت SBI_EINVAL;
	اگر (!ia->اقرأ)
		بازگشت SBI_ENOSYS;

	بازگشت ia->اقرأ(ia, addr, reg, buffer, len);
}
