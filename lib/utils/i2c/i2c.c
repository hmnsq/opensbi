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

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/i2c/i2c.h>

स्थिर SBI_LIST_HEAD(i2c_adapters_list);

संरचना i2c_adapter *i2c_adapter_find(पूर्णाङ्क id)
{
	संरचना sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		संरचना i2c_adapter *adap = to_i2c_adapter(pos);

		यदि (adap->id == id)
			फर्काऊ adap;
	}

	फर्काऊ रिक्त;
}

पूर्णाङ्क i2c_adapter_add(संरचना i2c_adapter *ia)
{
	यदि (!ia)
		फर्काऊ SBI_EINVAL;

	यदि (i2c_adapter_find(ia->id))
		फर्काऊ SBI_EALREADY;

	sbi_list_add(&(ia->गाँठो), &(i2c_adapters_list));

	फर्काऊ 0;
}

शून्य i2c_adapter_remove(संरचना i2c_adapter *ia)
{
	यदि (!ia)
		फर्काऊ;

	sbi_list_del(&(ia->गाँठो));
}

पूर्णाङ्क i2c_adapter_write(संरचना i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, पूर्णाङ्क len)
{
	यदि (!ia)
		फर्काऊ SBI_EINVAL;
	यदि (!ia->लिख)
		फर्काऊ SBI_ENOSYS;

	फर्काऊ ia->लिख(ia, addr, reg, buffer, len);
}


पूर्णाङ्क i2c_adapter_read(संरचना i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, पूर्णाङ्क len)
{
	यदि (!ia)
		फर्काऊ SBI_EINVAL;
	यदि (!ia->पठ)
		फर्काऊ SBI_ENOSYS;

	फर्काऊ ia->पठ(ia, addr, reg, buffer, len);
}
