/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 *
 * derivate: lib/utils/gpio/fdt_gpio.c
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/i2c/fdt_i2c.h>

/* List of FDT i2c adapter drivers generated at compile time */
बाह्य स्थिर काष्ठा fdt_driver *स्थिर fdt_i2c_adapter_drivers[];

अटल पूर्णांक fdt_i2c_adapter_find(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				काष्ठा i2c_adapter **out_adapter)
{
	पूर्णांक rc;
	काष्ठा i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	अगर (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		अगर (rc)
			वापस rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		अगर (!adapter)
			वापस SBI_ENOSYS;
	}

	अगर (out_adapter)
		*out_adapter = adapter;

	वापस 0;
}

पूर्णांक fdt_i2c_adapter_get(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			काष्ठा i2c_adapter **out_adapter)
{
	पूर्णांक rc;
	काष्ठा i2c_adapter *adapter;

	अगर (!fdt || (nodeoff < 0) || !out_adapter)
		वापस SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	अगर (rc)
		वापस rc;

	*out_adapter = adapter;

	वापस 0;
}
