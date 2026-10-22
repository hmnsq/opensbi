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

ساکن SBI_LIST_سر(i2c_adapters_list);

ساخت i2c_adapter *i2c_adapter_ڈھونڈو(صحیح id)
{
	ساخت sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		ساخت i2c_adapter *adap = مقصد_i2c_adapter(pos);

		اگر (adap->id == id)
			واپس adap;
	}

	واپس تہی;
}

صحیح i2c_adapter_جوڑو(ساخت i2c_adapter *ia)
{
	اگر (!ia)
		واپس SBI_EINVAL;

	اگر (i2c_adapter_ڈھونڈو(ia->id))
		واپس SBI_EALREADY;

	sbi_list_جوڑو(&(ia->گرہ), &(i2c_adapters_list));

	واپس 0;
}

خالی i2c_adapter_remove(ساخت i2c_adapter *ia)
{
	اگر (!ia)
		واپس;

	sbi_list_del(&(ia->گرہ));
}

صحیح i2c_adapter_write(ساخت i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *بفر, صحیح len)
{
	اگر (!ia)
		واپس SBI_EINVAL;
	اگر (!ia->اكتب)
		واپس SBI_ENOSYS;

	واپس ia->اكتب(ia, addr, reg, بفر, len);
}


صحیح i2c_adapter_read(ساخت i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *بفر, صحیح len)
{
	اگر (!ia)
		واپس SBI_EINVAL;
	اگر (!ia->اقرأ)
		واپس SBI_ENOSYS;

	واپس ia->اقرأ(ia, addr, reg, بفر, len);
}
