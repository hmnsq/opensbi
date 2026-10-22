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

		सचे (adap->id == id)
			पटिनिवत्त adap;
	}

	पटिनिवत्त रिक्त;
}

पूर्णाङ्क i2c_adapter_add(संरचना i2c_adapter *ia)
{
	सचे (!ia)
		पटिनिवत्त SBI_EINVAL;

	सचे (i2c_adapter_find(ia->id))
		पटिनिवत्त SBI_EALREADY;

	sbi_list_add(&(ia->ग्रन्थिः), &(i2c_adapters_list));

	पटिनिवत्त 0;
}

शून्य i2c_adapter_remove(संरचना i2c_adapter *ia)
{
	सचे (!ia)
		पटिनिवत्त;

	sbi_list_del(&(ia->ग्रन्थिः));
}

पूर्णाङ्क i2c_adapter_write(संरचना i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, पूर्णाङ्क len)
{
	सचे (!ia)
		पटिनिवत्त SBI_EINVAL;
	सचे (!ia->लिख)
		पटिनिवत्त SBI_ENOSYS;

	पटिनिवत्त ia->लिख(ia, addr, reg, buffer, len);
}


पूर्णाङ्क i2c_adapter_read(संरचना i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, पूर्णाङ्क len)
{
	सचे (!ia)
		पटिनिवत्त SBI_EINVAL;
	सचे (!ia->पठ)
		पटिनिवत्त SBI_ENOSYS;

	पटिनिवत्त ia->पठ(ia, addr, reg, buffer, len);
}
