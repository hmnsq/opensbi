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

#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/i2c/i2c.h>

ساكن SBI_LIST_سر(i2c_adapters_list);

بنية i2c_adapter *i2c_adapter_لبھو(صحيح id)
{
	بنية sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		بنية i2c_adapter *adap = منزل_i2c_adapter(pos);

		إذا (adap->id == id)
			أرجع adap;
	}

	أرجع عدم;
}

صحيح i2c_adapter_جوڑو(بنية i2c_adapter *ia)
{
	إذا (!ia)
		أرجع SBI_EINVAL;

	إذا (i2c_adapter_لبھو(ia->id))
		أرجع SBI_EALREADY;

	sbi_list_جوڑو(&(ia->گرہ), &(i2c_adapters_list));

	أرجع 0;
}

فراغ i2c_adapter_remove(بنية i2c_adapter *ia)
{
	إذا (!ia)
		أرجع;

	sbi_list_del(&(ia->گرہ));
}

صحيح i2c_adapter_write(بنية i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *بفر, صحيح len)
{
	إذا (!ia)
		أرجع SBI_EINVAL;
	إذا (!ia->اكتب)
		أرجع SBI_ENOSYS;

	أرجع ia->اكتب(ia, addr, reg, بفر, len);
}


صحيح i2c_adapter_read(بنية i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *بفر, صحيح len)
{
	إذا (!ia)
		أرجع SBI_EINVAL;
	إذا (!ia->اقرأ)
		أرجع SBI_ENOSYS;

	أرجع ia->اقرأ(ia, addr, reg, بفر, len);
}
