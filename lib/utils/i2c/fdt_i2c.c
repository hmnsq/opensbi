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

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/i2c/fdt_i2c.h>

/* List of FDT i2c adapter drivers generated at compile time */
बाह्य स्थिरांक संरचना fdt_driver *स्थिरांक fdt_i2c_adapter_drivers[];

स्थिर पूर्णांक fdt_i2c_adapter_find(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
				संरचना i2c_adapter **out_adapter)
{
	पूर्णांक rc;
	संरचना i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	जर (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		जर (rc)
			परत rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		जर (!adapter)
			परत SBI_ENOSYS;
	}

	जर (out_adapter)
		*out_adapter = adapter;

	परत 0;
}

पूर्णांक fdt_i2c_adapter_get(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
			संरचना i2c_adapter **out_adapter)
{
	पूर्णांक rc;
	संरचना i2c_adapter *adapter;

	जर (!fdt || (nodeoff < 0) || !out_adapter)
		परत SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	जर (rc)
		परत rc;

	*out_adapter = adapter;

	परत 0;
}
